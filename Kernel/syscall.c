#include <types.h>
#include <kernel.h>
#include "syscall.h"
#include "queue.h"
#include "task.h"
#include "x64.h"
#include "console.h"
#include "graphic.h"
#include "layer.h"
#include "font.h"
#include "message.h"
#include "functions.h"

#define Syscall_SyscallAddr ((void**)0x100000)

sintn Syscall_AppEnter();

void Syscall_Init(void) {
    Console_Print("Syscall_Init: Initializing Syscall...\n");

    *Syscall_SyscallAddr = (void*)Syscall_AppEnter;

    return;
}


//新規ウインドウ作成してLayerIdを返す
sintn Syscall_NewWindow(out uintn* layerId, in uintn x, in uintn y, in uintn width, in uintn height, in ascii title[]) {
    if(layerId == NULL) return 1;

    uint16 taskId = Task_GetRunningTaskId();

    *layerId = Layer_Window_New(taskId, title, x, y, width, height);
    if(*layerId == 0) return -1;

    return 0;
}


//layerIdのウインドウを閉じる
sintn Syscall_CloseWindow(in uintn layerId) {
    uint16 runningTaskId = Task_GetRunningTaskId();
    uint16 layerTaskID = Layer_Window_GettaskId(layerId);
    if(layerTaskID < 0 || runningTaskId != layerTaskID) return 1;
    
    Layer_Window_Delete(layerId);
    
    return 0;
}


//CPUを解放
sintn Syscall_YieldCpu(void) {
    Task_Yield();

    return 0;
}


//ウインドウに四角形を描画
sintn Syscall_DrawSquare(in uintn layerId, in uintn x, in uintn y, in uintn width, in uintn height, in Graphic_Color color) {
    Graphic_FrameBuff framebuff;
    if(Layer_Window_GetFrameBuff(layerId, &framebuff)) return -1;

    Graphic_FrameBuff_DrawSquare(framebuff, x, y, width, height, color);

    Layer_Window_NotifyUpdate(layerId, x, y, width, height);
    
    return 0;
}


//ウインドウに文字描画
sintn Syscall_DrawFont(in uintn layerId, in uintn x, in uintn y, in ascii asciicode, in Graphic_Color color) {
    Graphic_FrameBuff framebuff;
    if(Layer_Window_GetFrameBuff(layerId, &framebuff)) return -1;

    Font_Draw(framebuff, x, y, asciicode, color);

    Layer_Window_NotifyUpdate(layerId, x, y, 8, 16);
    
    return 0;
}


//メッセージを取得する　なければ処理停止
sintn Syscall_ReadMessage(out Task_Message* message) {
    Task_Yield();

    if(message == NULL) return 1;
    
    while(1) {
        Task_Messages_DeQueue(Task_GetRunningTaskId(), message);
        if(message->type != Task_Message_Nothing) break;
        Task_Halt();
    }

    return 0;
}


//メッセージを取得する　なければTask_Message_Nothingを返す
sintn Syscall_CheckMessage(out Task_Message* message) {
    Task_Yield();

    if(message == NULL) return 1;

    Task_Messages_Check(Task_GetRunningTaskId(), message);

    return 0;
}


//タスク間通信 32バイト送る
sintn Syscall_SendITCMessage(in uint16 taskId, in ascii str[32]) {
    if(taskId == 0) return 1;

    Task_Message taskMessage;
    taskMessage.type = Task_Message_ITCMessage;
    Functions_MemCpy(taskMessage.data.ITCMessage.str, str, sizeof(ascii)*32);

    Message_EnQueue(taskId, &taskMessage);

    Task_Yield();

    return 0;
}


//タスクの終了
sintn Syscall_Exit(void) {
    Task_Delete(Task_GetRunningTaskId());

    while(1) {
        Task_Halt();
    }

    return -1;
}
