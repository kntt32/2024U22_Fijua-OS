#ifndef INCLUDED_TASK_H
#define INCLUDED_TASK_H

typedef struct {
    void* stackPtr;
    uintn taskType;
} Task;

#endif
