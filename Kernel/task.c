#include <types.h>
#include <kernel.h>
#include <efi.h>
#include "x64.h"
#include "functions.h"
#include "queue.h"
#include "memory.h"
#include "task.h"
#include "console.h"

#include "test.h"
#include "graphic.h"

static const uintn Task_DefaultStackPageSize = 5000;//4KiB*1000

static Task task;


//Initialize TaskScheduler
void Task_Init(void) {
    //reset
    task.enableChangeTaskFlag = 1;
    task.yieldFlag = 0;
    task.switchCount = 0;
    task.kernelStackPtr = NULL;

    task.Queue.runningTaskId = 0;
    Queue_Init(&(task.Queue.app));
    Queue_Init(&(task.Queue.graphic));
    Queue_Init(&(task.Queue.driver));

    task.Table.count = 0;
    task.Table.listPages = 0;
    task.Table.list = NULL;

    return;
}


//Enable Switch Task
void Task_EnableSwitchTask(void) {
    task.enableChangeTaskFlag = 1;
    return;
}


//Disable Switch Task
void Task_DisableSwitchTask(void) {
    task.enableChangeTaskFlag = 0;
    return;
}


//Expand size of task.table.list
static uintn Task_Table_Expand(void) {
    uintn newListPages = task.Table.listPages*2+1;
    Task_Object* newList = Memory_AllocPages(2, newListPages);
    if(newList == NULL) return 1;

    for(uintn i=0; i<task.Table.count; i++) {
        newList[i] = task.Table.list[i];
    }

    Memory_FreePages(2, task.Table.listPages, task.Table.list);
    
    task.Table.list = newList;
    task.Table.listPages = newListPages;

    return 0;
}


//Allocate NewTaskID
static uint16 Task_SeekNewTaskID(void) {
    uint16 nextTaskId = 3;

    uintn i = 0;
    while(1) {
        if(task.Table.count<i) break;

        if(task.Table.list[i].taskId == nextTaskId) {
            nextTaskId++;
            i = 0;
            continue;
        }
        i++;
    }

    return nextTaskId;
}


//Get Index in TaskList
static sintn Task_GetIndexOfTaskList(uint16 taskId) {
    if(taskId == 0 || taskId == 1) return -1;

    for(uintn i=0; i<task.Table.count; i++) {
        if(task.Table.list[i].taskId == taskId) return i;
    }

    return -1;
}


//Add NewTask and return taskID
uint16 Task_NewTask(sintn (*taskEntry)(void)) {
    if(taskEntry == NULL) return 0;

    Task_DisableSwitchTask();
    uint16 newTaskId = Task_SeekNewTaskID();
    if(newTaskId == 0) {
        Task_EnableSwitchTask();
        return 0;
    }

    void* stackPtr = Memory_AllocPages(newTaskId, Task_DefaultStackPageSize);
    if(stackPtr == NULL) {
        Task_EnableSwitchTask();
        return 0;
    }

    if(task.Table.listPages < (((task.Table.count+1)*sizeof(Task_Object)+0xfff)>>12)) {
        if(Task_Table_Expand()) {
            Task_EnableSwitchTask();
            Memory_FreeAll(newTaskId);
            return 0;
        }
    }

    task.Table.list[task.Table.count].taskId = newTaskId;
    task.Table.list[task.Table.count].taskLevel = Task_Object_Tasklevel_app;
    task.Table.list[task.Table.count].stackPtr = Task_NewTask_Asm_SetStartContext((void*)(((uintn)stackPtr)+Task_DefaultStackPageSize-1));
    task.Table.list[task.Table.count].taskEntry = taskEntry;

    task.Table.count++;

    Task_EnQueueTask(newTaskId);

    Task_EnableSwitchTask();

    return newTaskId;
}


//NewTask StartPoint
void Task_NewTask_StartPoint() {
    //Efi_Wrapper(Efi_RestoreTPL, TPL_APPLICATION);

    sintn taskIndex = Task_GetIndexOfTaskList(task.Queue.runningTaskId);
    if(taskIndex != -1) {
        task.Table.list[taskIndex].taskEntry();
    }

    //end task
    Task_Yield();
    while(1);
}


//EnQueue to TaskQueue
uintn Task_EnQueueTask(uint16 taskId) {
    if(taskId == 0 || taskId == 1 || taskId == 2) {
        return 1;
    }
    Task_DisableSwitchTask();

    sintn taskIndex;
    taskIndex = Task_GetIndexOfTaskList(taskId);
    if(taskIndex == -1) {
        Task_EnableSwitchTask();
        return 2;
    }

    switch(task.Table.list[taskIndex].taskLevel) {
        case Task_Object_Tasklevel_app:
            if(!Queue_IsExist(&(task.Queue.app), (uintn)taskId)) {
                Queue_EnQueue(&(task.Queue.app), (uintn)taskId);
            }
            break;
        case Task_Object_Tasklevel_graphic:
            if(!Queue_IsExist(&(task.Queue.graphic), (uintn)taskId)) {
                Queue_EnQueue(&(task.Queue.graphic), (uintn)taskId);
            }
            break;
        case Task_Object_Tasklevel_driver:
            if(!Queue_IsExist(&(task.Queue.driver), (uintn)taskId)) {
                Queue_EnQueue(&(task.Queue.driver), (uintn)taskId);
            }
            break;
        default:
            Task_EnableSwitchTask();
            return 3;
    }

    Task_EnableSwitchTask();

    return 0;
}


//Yield
void Task_Yield(void) {
    task.yieldFlag = 1;
    Task_ContextSwitch();
    return;
}


//Subroutine of Asm function "Task_ContextSwitch"
void* Task_ContextSwitch_Subroutine(void* currentStackPtr) {
    if(!task.enableChangeTaskFlag) return currentStackPtr;

    task.enableChangeTaskFlag = 0;

    task.switchCount++;

    if(task.Queue.runningTaskId == 0) {
        task.kernelStackPtr = currentStackPtr;
    }else {
        sintn taskIndex = Task_GetIndexOfTaskList(task.Queue.runningTaskId);
        if(!(task.yieldFlag) && taskIndex != -1) {
            Task_EnQueueTask(task.Queue.runningTaskId);
        }
        if(taskIndex != -1) {
            task.Table.list[taskIndex].stackPtr = currentStackPtr;
        }
    }
    task.yieldFlag = 0;



    //seek nextTaskId
    uint16 nextTaskId = 0;
    sintn nextTaskIndex = 0;

    if(task.Queue.driver.count != 0) {
        while(1) {
            nextTaskId = Queue_DeQueue(&(task.Queue.driver));
            nextTaskIndex = Task_GetIndexOfTaskList(nextTaskId);
            if(nextTaskIndex == -1) nextTaskId = 0;
            if(nextTaskId != 0 || task.Queue.driver.count == 0) break;
        }
    }
    if(nextTaskId == 0 && task.Queue.graphic.count != 0) {
        while(1) {
            nextTaskId = Queue_DeQueue(&(task.Queue.graphic));
            nextTaskIndex = Task_GetIndexOfTaskList(nextTaskId);
            if(nextTaskIndex == -1) nextTaskId = 0;
            if(nextTaskId != 0 || task.Queue.graphic.count == 0) break;
        }
    }
    if(nextTaskId == 0 && task.Queue.app.count != 0) {
        while(1) {
            nextTaskId = Queue_DeQueue(&(task.Queue.app));
            nextTaskIndex = Task_GetIndexOfTaskList(nextTaskId);
            if(nextTaskIndex == -1) nextTaskId = 0;
            if(nextTaskId != 0 || task.Queue.app.count == 0) break;
        }
    }


    //switch to KernelStackPtr
    if(nextTaskId == 0) {
        task.Queue.runningTaskId = 0;
        task.enableChangeTaskFlag = 1;
        return task.kernelStackPtr;
    }

    task.Queue.runningTaskId = nextTaskId;
    task.enableChangeTaskFlag = 1;
    return task.Table.list[nextTaskIndex].stackPtr;
}
