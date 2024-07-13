#ifndef INCLUDED_TASK_H
#define INCLUDED_TASK_H

#define Task_TaskId_NULL (0)

#define Task_Object_Tasklevel_app (0)
#define Task_Object_Tasklevel_graphic (1)
#define Task_Object_Tasklevel_driver (2)


typedef struct {
    uint16 taskid;//disable 0
    uint8 tasklevel;//0: app, 1: graphic, 2: driver
    void* stackptr;
} Task_Object;


typedef struct {
    uint8 enableChangeTaskFlag;

    uint16 runningTaskId;

    uint8 yieldFlag;

    void* haltStackPtr;
    Task_Object newTask;

    struct {
        Queue app;
        Queue graphic;
        Queue driver;
    } Queue;

    struct {
        uint16 count;
        uintn listpages;
        
        Task_Object* list;
    } TaskList;
} Task;


void Task_Init(void);
uintn Task_NewTask(uint16* taskid);



#endif
