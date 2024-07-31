#ifndef INCLUDED_TASK_H
#define INCLUDED_TASK_H

#define Task_TaskId_NULL (0)

#define Task_Object_Tasklevel_app (0)


typedef struct {
    enum {
        Task_Message_Nothing,
        Task_Message_ITCMessage
    } type;

    union {
        uint64 ITCMessage;
    } data;
} Task_Message;


typedef struct {
    uint16 taskId;//0,1は無効、2はカーネル用に予約済み
    uint16 stdio_taskId;//標準入出力を送信するtaskId 0は未設定
    void* stackPtr;
    sintn (*taskEntry)(void);
    Queue messages;
} Task_Object;


typedef struct {
    uint8 haltFlag;

    void* kernelStackPtr;

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

void Task_ChangeContext(void);

uint16 Task_GetRunningTaskId(void);

uintn Task_Messages_EnQueue(uint16 taskId, const Task_Message* message);
uintn Task_Messages_Check(uint16 taskId, Task_Message* message);
uintn Task_Messages_DeQueue(uint16 taskId, Task_Message* message);

#endif
