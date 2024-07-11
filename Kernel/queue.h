#ifndef INCLUDED_QUEUE_H
#define INCLUDED_QUEUE_H

typedef struct {
    uintn start;
    uintn count;
    uintn size;
    uintn* queue;
} Queue;


Queue* Queue_Init(Queue* this);
void Queue_DeInit(Queue* this);
Queue* Queue_EnQueue(Queue* this, uintn object);
Queue* Queue_Replace(Queue* this, uintn from, uintn to);
uintn Queue_DeQueue(Queue* this);


#endif
