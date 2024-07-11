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


//Add New Task and Output Allocated Taskid to *taskid
uintn Task_NewTask(uintn* taskid) {
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


//Set stackptr to Task_Object with taskid
uintn Task_SetStackPtr(uintn taskid, void* stackptr) {
    if(taskid == 0 || stackptr == NULL) return 1;
    for(uintn i=0; i<Task_State.TaskList.count; i++) {
        if(Task_State.TaskList.list[i].taskid == taskid) {
            Task_State.TaskList.list[i].stackptr = stackptr;
            return 0;
        }
    }
    return 2;
}


//Switch Task
void Task_Switch() {

}


