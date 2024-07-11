#include <types.h>
#include "memory.h"
#include "queue.h"


#define Queue_OwnerID (2)


//constructer of queue
Queue* Queue_Init(Queue* this) {
    if(this == NULL) return NULL;

    this->start = 0;
    this->count = 0;
    this->size = 0;
    this->queue = NULL;

    return this;
}


//destructer of queue
void Queue_DeInit(Queue* this) {
    if(this == NULL) return;

    Memory_FreePages(Queue_OwnerID, this->size >> 12, (void*)(this->queue));

    this->start = 0;
    this->count = 0;
    this->size = 0;

    return;
}


//expand size of queue
static Queue* Queue_Expand(Queue* this) {
    if(this == NULL) return NULL;

    uintn newSize = (this->size + 1)*2;
    uintn* newQueue = Memory_AllocPages(Queue_OwnerID, newSize);
    if(newQueue == NULL) return NULL;

    uintn indexOfOldQueue = this->start;
    for(uintn i=0; i<this->count; i++) {
        newQueue[i] = this->queue[indexOfOldQueue];

        indexOfOldQueue ++;
        if(this->count <= indexOfOldQueue) indexOfOldQueue = 0;
    }

    Memory_FreePages(Queue_OwnerID, (this->size >> 12), this->queue);

    this->queue = newQueue;
    this->start = 0;
    this->size = newSize;

    return this;
}


//enqueue
Queue* Queue_EnQueue(Queue* this, uintn object) {
    if(this == NULL) return NULL;

    if(this->size == this->count) {
        if(Queue_Expand(this) == NULL) return NULL;
    }

    uintn indexOfQueue = (this->start + this->count)%(this->size);

    this->queue[indexOfQueue] = object;

    this->count++;

    return this;
}


//replace value of queue
Queue* Queue_Replace(Queue* this, uintn from, uintn to) {
    if(this == NULL) return NULL;

    uintn indexOfQueue = this->start;
    for(uintn i=0; i<this->count; i++) {
        if(this->queue[indexOfQueue] == from) {
            this->queue[indexOfQueue] = to;
        }

        indexOfQueue++;
        if(indexOfQueue == this->count) indexOfQueue = 0;
    }

    return this;
}


//dequeue
uintn Queue_DeQueue(Queue* this) {
    if(this == NULL) return 0;

    uintn result = this->queue[this->start];
    this->start ++;
    if(this->start == this->size) this->start = 0;

    return result;
}
