#include <types.h>
#include "functions.h"
#include "memory.h"
#include "queue.h"
#include "console.h"


#define Queue_OwnerID (2)


Queue* Queue_Init(Queue* this) {
    if(this == NULL) return NULL;

    this->start = 0;
    this->count = 0;
    this->size = 0;
    this->queue = NULL;

    return this;
}


void Queue_DeInit(Queue* this) {
    if(this == NULL) return;

    Memory_FreePages(Queue_OwnerID, (this->size*sizeof(uintn)) >> 12, (void*)(this->queue));

    this->start = 0;
    this->count = 0;
    this->size = 0;

    return;
}


static Queue* Queue_Expand(Queue* this) {
    if(this == NULL) return NULL;

    uintn newSize = ((((this->size*sizeof(uintn)+0xfff) >> 12)*2 + 1)<<12)/sizeof(uintn);
    uintn* newQueue = Memory_AllocPages(Queue_OwnerID, (newSize*sizeof(uintn)+0xfff)>>12);
    if(newQueue == NULL) return NULL;

    uintn indexOfOldQueue = this->start;
    for(uintn i=0; i<this->count; i++) {
        newQueue[i] = this->queue[indexOfOldQueue];

        indexOfOldQueue ++;
        if(this->size <= indexOfOldQueue) indexOfOldQueue = 0;
    }

    Memory_FreePages(Queue_OwnerID, ((this->size*sizeof(uintn)) >> 12), (void*)(this->queue));

    this->queue = newQueue;
    this->start = 0;
    this->size = newSize;

    return this;
}


void Queue_Print(Queue* this) {
    if(this == NULL) return;

    Console_Print("[");

    uintn indexOfQueue = this->start;
    ascii tempStrBuff[17];
    for(uintn i=0; i<this->count; i++) {
        if(i != 0) Console_Print(", ");

        SPrintIntX(this->queue[indexOfQueue], 17, tempStrBuff);
        Console_Print(tempStrBuff);
        indexOfQueue++;
        if(indexOfQueue == this->size) indexOfQueue = 0;
    }

    Console_Print("]\n");

    return;
}


Queue* Queue_EnQueue(Queue* this, uintn object) {
    if(this == NULL) return NULL;

    if(this->size < this->count+1) {
        if(Queue_Expand(this) == NULL) return NULL;
    }

    uintn indexOfQueue = (this->start + this->count)%(this->size);

    this->queue[indexOfQueue] = object;

    this->count++;

    return this;
}


Queue* Queue_Replace(Queue* this, uintn from, uintn to) {
    if(this == NULL) return NULL;

    uintn indexOfQueue = this->start;
    for(uintn i=0; i<this->count; i++) {
        if(this->queue[indexOfQueue] == from) {
            this->queue[indexOfQueue] = to;
        }

        indexOfQueue++;
        if(indexOfQueue == this->size) indexOfQueue = 0;
    }

    return this;
}


uintn Queue_DeQueue(Queue* this) {
    if(this == NULL) return 0;

    uintn result = this->queue[this->start];
    this->start ++;
    this->count --;
    if(this->start == this->size) this->start = 0;

    return result;
}


uintn Queue_IsExist(Queue* this, uintn object) {
    if(this == NULL) return 0;
    uintn indexOfQueue = this->start;
    for(uintn i=0; i<this->count; i++) {
        if(this->queue[indexOfQueue] == object) return 1;
        indexOfQueue++;
        if(indexOfQueue == this->count) indexOfQueue = 0;
    }
    return 0;
}
