#include <types.h>
#include "queue.h"
#include "memory.h"
#include "console.h"
#include "functions.h"

#define Queue_TaskId (2)

//Queueを初期化
Queue* Queue_Init(Queue* this, uintn perSize) {
    if(this == NULL || perSize == 0) return NULL;

    this->start = 0;
    this->count = 0;
    this->perSize = perSize;
    this->poolPages = 0;
    this->objectPool = NULL;

    return this;
}



//Queueを解放
void Queue_DeInit(Queue* this) {
    if(this == NULL) return;

    Memory_FreePages(Queue_TaskId, this->poolPages, this->objectPool);

    this->start = 0;
    this->count = 0;
    this->perSize = 1;
    this->poolPages = 0;
    this->objectPool = NULL;

    return;
}


//QueueのobjectPoolを拡張
static Queue* Queue_Expand(Queue* this) {
    if(this == NULL) return NULL;
    
    uintn newPoolPages = this->poolPages*2 + 1;
    uint8* newObjectPool = Memory_AllocPages(Queue_TaskId, newPoolPages);
    if(newObjectPool == NULL) return NULL;

    uint64* newObjectPool_uint64Ptr = (uint64*)newObjectPool;
    uint64* oldObjectPool_uint64Ptr = (uint64*)this->objectPool;
    uintn oldObjectPool_Index = this->start;
    uintn oldObjectPool_MaxIndex = this->poolPages << 12;
    for(uintn i=0; i<this->perSize*this->count; i++) {
        newObjectPool_uint64Ptr[i] = oldObjectPool_uint64Ptr[oldObjectPool_Index];
        oldObjectPool_Index ++;
        if(oldObjectPool_MaxIndex <= oldObjectPool_Index) oldObjectPool_Index = 0;
    }


    Memory_FreePages(Queue_TaskId, this->poolPages, this->objectPool);

    this->start = 0;
    this->poolPages = newPoolPages;
    this->objectPool = newObjectPool;

    return this;
}


//Queueを表示
void Queue_Print(Queue* this) {
    if(this == NULL) return;
/*
    uintn index = this->start;
    uintn maxIndex = this->poolPages << 12;
    for(uintn i=0; i<this->count; i++) {
        for(uintn k=0; k<this->perSize; k++) {
            printf(" ");
            printf("%x", this->objectPool[index+k] & 0x0f);
            printf("%x", (this->objectPool[index+k] & 0xf0) >> 4);
        }
        printf(",");
        index += this->perSize;
        if(maxIndex <= index) index = 0;
    }
*/
    return;
}


//Queueに追加
Queue* Queue_EnQueue(Queue* this, const void* object) {
    if(this == NULL || object == NULL) return NULL;

    while((this->poolPages<<12) <= (this->count+1)*this->perSize) {
        Queue_Expand(this);
    }

    uintn index = this->start + this->count*this->perSize;
    uintn maxIndex = this->poolPages << 12;
    if((this->poolPages<<12) <= index) index -= this->poolPages;

    const uint8* object_uint8Ptr = (uint8*)object;
    for(uintn i=0; i<this->perSize; i++) {
        this->objectPool[index] = object_uint8Ptr[i];
        index++;
        if(maxIndex <= index) index = 0;
    }

    this->count ++;

    return this;
}


//Queueの*fromを*toに置き換え
Queue* Queue_Replace(Queue* this, const void* from, const void* to) {
    if(this == NULL || from == NULL || to == NULL) return NULL;

    uintn index = this->start;
    uintn maxIndex = this->poolPages << 12;

    const uint8* from_uint8Ptr = (const uint8*)from;
    const uint8* to_uint8Ptr = (const uint8*)to;

    for(uintn i=0; i<this->count; i++) {
        //対象の要素がfromと等しいか判定
        uintn index_copy = index;
        uintn isEqualFlag = 1;
        for(uintn k=0; k<this->perSize; k++) {
            if(this->objectPool[index_copy] != from_uint8Ptr[k]) {
                isEqualFlag = 0;
                break;
            }
            index_copy ++;
            if(maxIndex <= index_copy) index_copy = 0;
        }

        //等しいなら置き換える
        if(isEqualFlag) {
            for(uintn k=0; k<this->perSize; k++) {
                this->objectPool[index] = to_uint8Ptr[k];
                index ++;
                if(maxIndex <= index) index = 0;
            }
        }else {
            index += this->perSize;
            if(maxIndex <= index) index -= maxIndex;
        }
    }

    return this;
}


//Queueをチェック
Queue* Queue_CheckQueue(Queue* this, void* object) {
    if(this == NULL || object == NULL) return NULL;

    uint8* object_uint8Ptr = (uint8*)object;

    uintn maxIndex = this->poolPages << 12;
    uintn index = this->start;
    for(uintn i=0; i<this->perSize; i++) {
        object_uint8Ptr[i] = this->objectPool[index];

        index ++;
        if(maxIndex <= index) index = 0;
    }

    return this;
}


//Queueから取り出す
Queue* Queue_DeQueue(Queue* this, void* object) {
    if(this == NULL || this->count == 0) return NULL;

    if(Queue_CheckQueue(this, object) == NULL) return NULL;

    uintn maxIndex = this->poolPages << 12;

    this->start += this->perSize;
    if(maxIndex <= this->start) this->start -= maxIndex;
    this->count --;

    return this;
}


//Queueに存在しているか判定
uintn Queue_IsExist(Queue* this, void* object) {
    if(this == NULL || object == NULL) return 0;

    uintn index = this->start;
    uintn maxIndex = this->poolPages << 12;

    const uint8* object_uint8Ptr = (const uint8*)object;

    for(uintn i=0; i<this->count; i++) {
        //対象の要素がfromと等しいか判定
        uintn index_copy = index;
        uintn isEqualFlag = 1;
        for(uintn k=0; k<this->perSize; k++) {
            if(this->objectPool[index_copy] != object_uint8Ptr[k]) {
                isEqualFlag = 0;
                break;
            }
            index_copy ++;
            if(maxIndex <= index_copy) index_copy = 0;
        }

        //等しいなら置き換える
        if(isEqualFlag) {
            return 1;
        }

        index += this->perSize;
        if(maxIndex <= index) index -= maxIndex;
    }

    return 0;
}
