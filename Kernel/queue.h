#ifndef INCLUDED_QUEUE_H
#define INCLUDED_QUEUE_H

typedef struct {
    uintn start;//FIFOのスタートが何バイト目か
    uintn count;//FIFO内のオブジェクトの数
    uintn perSize;//オブジェクト一つあたりのバイト数
    uintn poolPages;//objectPoolのページ数
    uint8* objectPool;//オブジェクトプールのポインタ
} Queue;


Queue* Queue_Init(Queue* this, uintn perSize);
void Queue_DeInit(Queue* this);
void Queue_Print(Queue* this);
Queue* Queue_EnQueue(Queue* this, const void* object);
Queue* Queue_Replace(Queue* this, const void* from, const void* to);
Queue* Queue_CheckQueue(Queue* this, void* object);
Queue* Queue_DeQueue(Queue* this, void* object);
uintn Queue_IsExist(Queue* this, void* object);


#endif
