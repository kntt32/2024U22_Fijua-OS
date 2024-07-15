#include <types.h>
#include <kernel.h>
#include "x64.h"
#include "functions.h"
#include "timer.h"
#include "queue.h"
#include "memory.h"
#include "task.h"
#include "console.h"

#include "test.h"
#include "graphic.h"

static const uintn Task_DefaultPageSize = 1000;//4KiB*1000

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


    //enable timer
    Timer_Set(Task_ContextSwitch, 10);

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
static uint16 Task_AllocNewTaskID(void) {
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


//Add taskId to TaskTable
static uintn Task_AddTaskTable(uint16 taskId) {
    //this function must be called in disabled taskswitching
    if(taskId == 0) return 1;

    if((task.Table.listPages<<12) < (task.Table.count+1)*sizeof(Task_Object)) {
        if(Task_Table_Expand()) return 2;
    }

    task.Table.list[task.Table.count].taskId = taskId;
    task.Table.list[task.Table.count].taskLevel = Task_Object_Tasklevel_app;
    task.Table.list[task.Table.count].stackPtr = NULL;
    task.Table.count++;

    return 0;
}


//Get Index in TaskList
static sintn Task_GetIndexOfTaskList(uint16 taskId) {
    if(taskId == 0 || taskId == 1) return -1;

    for(uintn i=0; i<task.Table.count; i++) {
        if(task.Table.list[i].taskId == taskId) return i;
    }

    return -1;
}


//Subroutine of Asm function "Task_NewTask_Asm"
void* Task_NewTask_Asm_AddTaskTable(uintn* taskId) {
#if 0
    static int debugc = 0;
    debugc ++ ;
    if(debugc != 1) Test_Tester3();
#endif
    //return newStack Ptr
    //this function must be called in disabled taskswitching
    *taskId = (uintn)(Task_AllocNewTaskID());

    if(*taskId == Task_TaskId_NULL) return NULL;

#if 0
    {
        ascii strbuff[5];
        SPrintIntX(task.Queue.runningTaskId, 4, strbuff);
        strbuff[3] = '\n';
        strbuff[4] = '\0';
        Console_Print(strbuff);
    }
#endif

    void* stackPtr = Memory_AllocPages(*taskId, Task_DefaultPageSize);
    if(stackPtr == NULL) {
        *taskId = Task_TaskId_NULL;
        return NULL;
    }

    stackPtr = (void*)((uintn)stackPtr + (Task_DefaultPageSize<<12) - 1);
    if(Task_AddTaskTable(*taskId)) {
        Memory_FreeAll(*taskId);
        *taskId = 0;
        return NULL;
    }

    //
    if(task.Queue.runningTaskId != 0) Task_EnQueueTask(task.Queue.runningTaskId);////??bug

    Task_DisableSwitchTask();

    task.Queue.runningTaskId = *taskId;

    return stackPtr;
}


//Add NewTask and return taskID
uint16 Task_NewTask(sintn (*taskEntry)(void)) {
    if(taskEntry == NULL) return 1;

    return Task_NewTask_Asm(&(task.switchCount), &(task.enableChangeTaskFlag), taskEntry);
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
    return;
}

//Subroutine of Asm function "Task_ContextSwitch"
void* Task_ContextSwitch_Subroutine(void* currentStackPtr) {
    if(!task.enableChangeTaskFlag) return currentStackPtr;

    task.enableChangeTaskFlag = 0;

#if 1
{
    ascii tempStrBuff[6];
    SPrintIntX(task.Queue.runningTaskId, 5, tempStrBuff);
    tempStrBuff[4] = ':';
    tempStrBuff[5] = '\0';
    Graphic_Color color;
    color.red = 0;
    color.green = 0;
    color.blue = task.switchCount*10;
    Graphic_DrawSquare(0, 0, 10, 10, color);
    //Console_Print(tempStrBuff);
}
#endif

    task.switchCount++;

    if(task.Queue.runningTaskId == 0) {
        task.kernelStackPtr = currentStackPtr;
#if 1
{
    ascii tempStrBuff[18];
    SPrintIntX((uintn)currentStackPtr, 17, tempStrBuff);
    tempStrBuff[16] = '[';
    tempStrBuff[17] = '\0';
    Console_Print(tempStrBuff);

    for(uintn i=0; i<1; i++) {
        SPrintIntX(*(((uintn*)currentStackPtr) + i), 17,tempStrBuff);
        tempStrBuff[16] = '\n';
    tempStrBuff[17] = '\0';
    Console_Print(tempStrBuff);
    }
}
#endif
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

#if 0
{
    ascii tempStrBuff[6];
    SPrintIntX(nextTaskId, 5, tempStrBuff);
    tempStrBuff[4] = '\n';
    tempStrBuff[5] = '\0';
    Console_Print(tempStrBuff);
    slow;
}
#endif

    //switch to KernelStackPtr
    if(nextTaskId == 0) {
#if 1
{
    ascii tempStrBuff[18];
    SPrintIntX((uintn)task.kernelStackPtr, 17, tempStrBuff);
    tempStrBuff[16] = ']';
    tempStrBuff[17] = '\0';
    Console_Print(tempStrBuff);

    for(uintn i=0; i<1; i++) {
        SPrintIntX(*(((uintn*)task.kernelStackPtr) + i), 17,tempStrBuff);
        tempStrBuff[16] = '\n';
    tempStrBuff[17] = '\0';
    Console_Print(tempStrBuff);
    }
}
#endif
        task.Queue.runningTaskId = 0;
        task.enableChangeTaskFlag = 1;
        return task.kernelStackPtr;
    }

    task.Queue.runningTaskId = nextTaskId;
    task.enableChangeTaskFlag = 1;
    return task.Table.list[nextTaskIndex].stackPtr;
}


void Task_ChangeContext(void) {
    task.switchCount++;

    void** saveStackArea = NULL;
    if(task.Queue.runningTaskId == 0) {
        saveStackArea = &(task.kernelStackPtr);
    }else {
        sintn taskIndex = Task_GetIndexOfTaskList(task.Queue.runningTaskId);
        if(taskIndex != -1) {
            saveStackArea = &(task.kernelStackPtr);
            if(!task.yieldFlag) {
                Task_EnQueueTask(task.Queue.runningTaskId);
            }
        }else {
            while(1);//Err coding now
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
    void* nextStack = NULL;
    if(nextTaskId == 0) {
        task.Queue.runningTaskId = 0;
        nextStack = (task.kernelStackPtr);
    }else {
        task.Queue.runningTaskId = nextTaskId;
        task.enableChangeTaskFlag = 1;
        nextStack = (task.Table.list[nextTaskIndex].stackPtr);
    }


    Task_ContextSwitch2(saveStackArea, nextStack);

    return;
}

