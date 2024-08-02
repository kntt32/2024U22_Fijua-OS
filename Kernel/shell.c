#include <types.h>
#include "graphic.h"
#include "queue.h"
#include "task.h"
#include "shell.h"
#include "app_x64.h"

void Console_Print(ascii str[]);

sintn Shell_Main(void) {
    uint16 n;
    App_Syscall_GetStdIoTaskId(&n);
    App_Syscall_SendIPCMessage(3, "hello by shell\n");

    uintn layerId;
    App_Syscall_NewWindow(&layerId, 0, 0, 200, 200, "Shell");

    Task_Message message;
    while(1) {
        App_Syscall_ReadMessage(&message);
    }

    return 0;
}

