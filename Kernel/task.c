#include <types.h>
#include <kernel.h>

#include "memory.h"
#include "console.h"
#include "functions.h"
#include "queue.h"
#include "x64.h"
#include "task.h"


extern KernelInputStruct* kernelInput;


static Task Task_State;


//Initialize Task Module
void Task_Init(void) {
    //reset value of Task_State
    Task_State.enableChangeTaskFlag = 1;
    Task_State.runningTaskId = Task_TaskId_NULL;

    Queue_Init(&(Task_State.Queue.app));
    Queue_Init(&(Task_State.Queue.graphic));
    Queue_Init(&(Task_State.Queue.driver));

    Task_State.TaskList.count = 0;
    Task_State.TaskList.listpages = 0;
    Task_State.TaskList.list = NULL;

    //Set Timer

    return;
}


//expand tasklist
static uintn Task_Expand() {
    uintn newlistpages = Task_State.TaskList.listpages*2 + 1;
    Task_Object* newlist = Memory_AllocPages(2, newlistpages);

    if(newlist == NULL) return 1;

    Task_Object* targetTaskObject = NULL;
    for(uintn i=0; i<Task_State.TaskList.count; i++) {
        targetTaskObject = Task_State.TaskList.list + i;
        newlist[i].taskid = targetTaskObject->taskid;
        newlist[i].stackptr = targetTaskObject->stackptr;
    }
    Memory_FreePages(2, Task_State.TaskList.listpages, Task_State.TaskList.list);

    Task_State.TaskList.listpages = newlistpages;
    Task_State.TaskList.list = newlist;

    return 0;
}


//Get indef of taskList
static uintn Task_GetIndexOfTaskList(uint16 taskid, uintn* index) {
    if(taskid == Task_TaskId_NULL) return 1;

    for(uintn i=0; i<Task_State.TaskList.count; i++) {
        if(Task_State.TaskList.list[i].taskid == taskid) {
            if(index != NULL) *index = i;
            return 0;
        }
    }

    return 2;
}


//Add New Task and Output Allocated Taskid to *taskid
uintn Task_NewTask(uint16* taskid) {
    //expand tasklist
    if(Task_State.TaskList.count == (Task_State.TaskList.listpages<<12)) {
        if(Task_Expand() != 0) return 1;
    }

    //allocate taskid
    uint16 seeking_taskid = 3;

    uintn i=0;
    while(1) {
        if(Task_State.TaskList.count <= i) break;
        if(Task_State.TaskList.list[i].taskid == seeking_taskid) {
            seeking_taskid++;
            i = 0;
        }
    }
    *taskid = seeking_taskid;

    //add task
    Task_Object* targetTaskObject = Task_State.TaskList.list + Task_State.TaskList.count;
    targetTaskObject->taskid = seeking_taskid;
    targetTaskObject->tasklevel = Task_Object_Tasklevel_app;
    targetTaskObject->mode = Task_Object_Mode_Sleep;
    targetTaskObject->stackptr = NULL;

    return 0;
}


//Set stackptr to Task_Object with taskid
uintn Task_SetStackPtr(uint16 taskid, void* stackptr) {
    if(taskid == Task_TaskId_NULL || stackptr == NULL) return 1;

    uintn taskIndex;
    if(Task_GetIndexOfTaskList(taskid, &taskIndex) == 0) {
        Task_State.TaskList.list[taskIndex].stackptr = stackptr;
        return 0;
    }
    
    return 2;
}


//Set sleepmode
uintn Task_SetSleep(uint16 taskid) {
    if(taskid == Task_TaskId_NULL) return 1;

    uintn taskIndex;
    if(Task_GetIndexOfTaskList(taskid, &taskIndex) == 0) {
        Task_State.TaskList.list[taskIndex].mode = Task_Object_Mode_Sleep;
        return 0;
    }

    return 2;
}


//Set workingmode
uintn Task_SetWork(uint16 taskid) {
    if(taskid == Task_TaskId_NULL) return 1;

    uintn taskIndex;
    if(Task_GetIndexOfTaskList(taskid, &taskIndex) == 0) {
        Task_State.TaskList.list[taskIndex].mode = Task_Object_Mode_Working;
        return 0;
    }

    return 2;
}


//Append to TaskQueue and change mode to Task_Object_Mode_WorkingMode
uintn Task_EnQueueTask(uint16 taskid) {
    if(taskid == Task_TaskId_NULL) return 1;

    uintn taskIndex;
    if(Task_GetIndexOfTaskList(taskid, &taskIndex) == 0) {
        Task_State.TaskList.list[taskIndex].mode = Task_Object_Mode_Working;

        switch(Task_State.TaskList.list[taskIndex].mode) {
            case Task_Object_Tasklevel_app:
                Queue_EnQueue(&(Task_State.Queue.app), taskid);
                break;
            case Task_Object_Tasklevel_graphic:
                Queue_EnQueue(&(Task_State.Queue.graphic), taskid);
                break;
            case Task_Object_Tasklevel_driver:
                Queue_EnQueue(&(Task_State.Queue.driver), taskid);
                break;
            default:
                return 2;
        }
        return 0;
    }

    return 2;
}


//Change TaskLevel
uintn Task_ChangeLevel(uint16 taskid, uint8 tasklevel) {
    if(taskid == Task_TaskId_NULL) return 1;
    if(!(0<=tasklevel && tasklevel<=2)) return 2;

    uintn taskIndex;
    if(Task_GetIndexOfTaskList(taskid, &taskIndex) == 0) {
        Task_State.TaskList.list[taskIndex].mode = tasklevel;
        return 0;
    }

    return 3;
}


//Delete Task
uintn Task_Delete(uint16 taskid) {
    return 1;
}


//Switch Task
void Task_Switch() {
    //check changing task is enabled
    if(!Task_State.enableChangeTaskFlag) return;

    //append to taskqueue if the task is working mode
    if(!(Task_State.runningTaskId == Task_TaskId_NULL || Task_State.TaskList.list[Task_State.runningTaskId].mode == Task_Object_Mode_Sleep)) {
        Task_EnQueueTask(Task_State.runningTaskId);
    }

    //get newtaskid
    uint16 newTaskID = 0;
    if(newTaskID == 0 && Task_State.Queue.driver.count != 0) {
        while(1) {
            newTaskID = Queue_DeQueue(&(Task_State.Queue.driver));
            if(newTaskID == Task_TaskId_NULL && Task_State.Queue.driver.count != 0) continue;
            break;
        }
    }
    if(newTaskID == 0 && Task_State.Queue.graphic.count != 0) {
        while(1) {
            newTaskID = Queue_DeQueue(&(Task_State.Queue.graphic));
            if(newTaskID == Task_TaskId_NULL && Task_State.Queue.graphic.count != 0) continue;
            break;
        }
    }
    if(newTaskID == 0 && Task_State.Queue.app.count != 0) {
        while(1) {
            newTaskID = Queue_DeQueue(&(Task_State.Queue.app));
            if(newTaskID == Task_TaskId_NULL && Task_State.Queue.app.count != 0) continue;
            break;
        }
    }
    if(newTaskID == 0) {
        //halt
        while(1);
    }

    //coding now
    

    return;
}


