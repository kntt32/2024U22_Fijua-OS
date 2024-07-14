#ifndef INCLUDED_TASK_H
#define INCLUDED_TASK_H

#define Task_TaskId_NULL (0)

#define Task_Object_Tasklevel_app (0)
#define Task_Object_Tasklevel_graphic (1)
#define Task_Object_Tasklevel_driver (2)


typedef struct {
    uint16 taskId;//disable 0
    uint8 taskLevel;//0: app, 1: graphic, 2: driver
    void* stackPtr;
} Task_Object;


typedef struct {
    uint8 enableChangeTaskFlag;
    uint8 yieldFlag;

    uintn switchCount;

    void* kernelStackPtr;

    struct {
        uint16 runningTaskId;
        
        Queue app;
        Queue graphic;
        Queue driver;
    } Queue;

    struct {
        uintn count;
        uintn listPages;
        
        Task_Object* list;
    } Table;
} Task;


void Task_Init(void);

void Task_EnableSwitchTask(void);
void Task_DisableSwitchTask(void);

uint16 Task_NewTask(sintn (*taskEntry)(void));
uintn Task_EnQueueTask(uint16 taskId);


#endif
