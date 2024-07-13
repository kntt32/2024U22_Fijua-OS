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
static const uintn Task_StackSize = 1000000;//1MB


//Initialize Task Module
void Task_Init(void) {
    //reset value of Task_State
    Task_State.enableChangeTaskFlag = 1;
    Task_State.runningTaskId = Task_TaskId_NULL;
    Task_State.yieldFlag = 0;

    Queue_Init(&(Task_State.Queue.app));
    Queue_Init(&(Task_State.Queue.graphic));
    Queue_Init(&(Task_State.Queue.driver));

    Task_State.TaskList.count = 0;
    Task_State.TaskList.listpages = 0;
    Task_State.TaskList.list = NULL;

    Task_State.haltStackPtr = NULL;

    Task_State.newTask.taskid = 0;

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
    targetTaskObject->stackptr = NULL;

    return 0;
}


//Start New Task
uintn Task_Start(uint16 taskid, sintn (*taskEntry)(void)) {
    if(taskid == 0 || taskEntry == NULL) return 1;
    if(Task_GetIndexOfTaskList(taskid, NULL)) return 2;
//

    return 0;
}


static void Task_WaitForSwitchTask(void) {
    uint16 nowTaskId = Task_State.runningTaskId;
    while(1) {
        if(Task_State.runningTaskId != nowTaskId) break;
    }
    return;
}


void Task_Yield(void) {
    Task_State.yieldFlag = 1;

    Queue_Replace(&(Task_State.Queue.driver), Task_State.runningTaskId, 0);
    Queue_Replace(&(Task_State.Queue.graphic), Task_State.runningTaskId, 0);
    Queue_Replace(&(Task_State.Queue.app), Task_State.runningTaskId, 0);

    Task_WaitForSwitchTask();

    return;
}


/*
//Set stackptr to Task_Object with taskid
uintn Task_SetStackPtr(uint16 taskid, void* stackptr) {
    if(taskid == Task_TaskId_NULL || stackptr == NULL) return 1;

    uintn taskIndex;
    if(Task_GetIndexOfTaskList(taskid, &taskIndex) == 0) {
        Task_State.TaskList.list[taskIndex].stackptr = stackptr;
        return 0;
    }
    
    return 2;
}*/


//Append to TaskQueue and change mode to Task_Object_Mode_WorkingMode
uintn Task_EnQueueTask(uint16 taskid) {
    if(taskid == Task_TaskId_NULL) return 1;

    uintn taskIndex;
    if(Task_GetIndexOfTaskList(taskid, &taskIndex) == 0) {
        switch(Task_State.TaskList.list[taskIndex].tasklevel) {
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
        Task_State.TaskList.list[taskIndex].tasklevel = tasklevel;
        return 0;
    }

    return 3;
}


//Delete Task
uintn Task_Delete(uint16 taskid) {
    if(taskid == 0) return 1;

    Queue_Replace(&(Task_State.Queue.driver), taskid, 0);
    Queue_Replace(&(Task_State.Queue.graphic), taskid, 0);
    Queue_Replace(&(Task_State.Queue.app), taskid, 0);

    //coding now

    return 2;
}


//Switch Task
void* Task_Switch(void* runningTaskRsp) {
    /*
    input now task's rsp and return next task's rsp
    */

    uintn indexOfQueue = 0;

    //save rsp and enqueue task if it is working mode
    if(Task_State.runningTaskId == 0) {
        Task_State.haltStackPtr = runningTaskRsp;
    }else if(!Task_GetIndexOfTaskList(Task_State.runningTaskId, &indexOfQueue)) {
        Task_State.TaskList.list[indexOfQueue].stackptr = runningTaskRsp;
        if(!Task_State.yieldFlag) {
            Task_EnQueueTask(Task_State.runningTaskId);
        }
    }
    Task_State.yieldFlag = 0;

    //get nexttaskid
    uint16 nextTaskId = 0;
    if(Task_State.Queue.driver.count != 0) {
        while(1) {
            nextTaskId = Queue_DeQueue(&(Task_State.Queue.driver));
            if(nextTaskId != 0 || Task_State.Queue.driver.count == 0) break;
        }
    }
    if(nextTaskId == 0 && Task_State.Queue.graphic.count != 0) {
        while(1) {
            nextTaskId = Queue_DeQueue(&(Task_State.Queue.graphic));
            if(nextTaskId != 0 || Task_State.Queue.graphic.count == 0) break;
        }
    }
    if(nextTaskId == 0 && Task_State.Queue.app.count != 0) {
        while(1) {
            nextTaskId = Queue_DeQueue(&(Task_State.Queue.app));
            if(nextTaskId != 0 || Task_State.Queue.app.count == 0) break;
        }
    }

    //switch to next task
    if(!Task_GetIndexOfTaskList(nextTaskId, &indexOfQueue)) {
        Task_State.runningTaskId = nextTaskId;
        return Task_State.TaskList.list[indexOfQueue].stackptr;
    }

    //switch to default task
    Task_State.runningTaskId = 0;
    return Task_State.haltStackPtr;

    return NULL;
}
