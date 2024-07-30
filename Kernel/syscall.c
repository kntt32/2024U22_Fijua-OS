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

#define Syscall_SyscallAddr ((void**)0x100000)

uintn Syscall_AppEnter();

void Syscall_Init(void) {
    *Syscall_SyscallAddr = (void*)Syscall_AppEnter;

    return;
}

//新規ウインドウ作成してレイヤIDを返す
sintn Syscall_NewWindow(out uintn* layerId, in uintn x, in uintn y, in uintn width, in uintn height, in ascii title[]) {
    if(layerId == NULL) return 1;

    Task_Yield();

    uint16 taskId = Task_GetRunningTaskId();

    *layerId = Layer_Window_New(taskId, title, x, y, width, height);
    if(*layerId == 0) return -1;

    return 0;
}


//CPUを解放
sintn Syscall_YieldCpu(void) {
    Task_Yield();

    return 0;
}


//ウインドウに四角形を描画
sintn Syscall_DrawSquare(in uintn layerId, in uintn x, in uintn y, in uintn width, in uintn height, Graphic_Color color) {
    Graphic_FrameBuff framebuff;
    if(Layer_Window_GetFrameBuff(layerId, &framebuff)) return -1;

    Task_Yield();

    Graphic_FrameBuff_DrawSquare(framebuff, x, y, width, height, color);

    Layer_Window_NotifyUpdate(layerId, x, y, width, height);
    
    return 0;
}


//ウインドウに文字描画
sintn Syscall_DrawFont(in uintn layerId, in uintn x, in uintn y, ascii asciicode, Graphic_Color color) {
    Graphic_FrameBuff framebuff;
    if(Layer_Window_GetFrameBuff(layerId, &framebuff)) return -1;

    Task_Yield();

    Font_Draw(framebuff, x, y, asciicode, color);

    Layer_Window_NotifyUpdate(layerId, x, y, 8, 16);
    
    return 0;
}


//メッセージを取得する　なければ処理停止
sintn Syscall_ReadMessage(Task_Message* message) {
    if(message == NULL) return 1;
    
    while(1) {
        Task_Messages_DeQueue(Task_GetRunningTaskId(), message);
        if(message->type != Task_Message_Nothing) break;
        Task_Halt();
    }

    return 0;
}
