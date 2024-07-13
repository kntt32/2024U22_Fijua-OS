#include <types.h>
#include <kernel.h>
#include "x64.h"
#include "functions.h"
#include "task.h"
#include "console.h"


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
uintn Task_Table_Expand(void) {
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


//Add NewTask
void Task_NewTask(sintn (*taskEntry)(void)) {
    if(taskEntry == NULL) return;



    return;
}


