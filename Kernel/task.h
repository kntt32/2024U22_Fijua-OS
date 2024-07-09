#ifndef INCLUDED_TASK_H
#define INCLUDED_TASK_H

typedef struct {
    uintn taskId;
    void* stackptr;
} Task;


typedef struct {
    Queue taskQueue_app;
    Queue taskQueue_graphic;
    Queue taskQueue_driver;

    struct {
        uintn count;
        uintn size;
        
        Task* list;
    } TaskList;
} TaskManager;



#endif
