#ifndef INCLUDED_TASK_H
#define INCLUDED_TASK_H

#define Task_TaskId_NULL (0)

#define Task_Object_Tasklevel_app (0)


typedef struct {
    enum {
        Task_Event_UnKnown
    } type;
} Task_Event;


typedef struct {
    uint16 taskId;//disable 0
    void* stackPtr;
    sintn (*taskEntry)(void);
    Queue messageQueue;
} Task_Object;


typedef struct {
    uint8 haltFlag;

    void* kernelStackPtr;
    sintn layerTrigger;

    struct {
        uint16 runningTaskId;
        
        Queue app;
    } Queue;

    struct {
        uintn count;
        uintn listPages;
        
        Task_Object* list;
    } Table;
} Task;


void Task_Init(void);

uint16 Task_New(sintn (*taskEntry)(void));
void Task_Delete(uint16 taskId);
uintn Task_EnQueueTask(uint16 taskId);

void Task_Yield(void);
void Task_Halt(void);

void Task_SetLayerTrigger(void);

void Task_ChangeContext(void);

uint16 Task_GetRunningTaskId(void);

#endif
