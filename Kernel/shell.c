#include <types.h>
#include "graphic.h"
#include "queue.h"
#include "task.h"
#include "shell.h"
#include "app_x64.h"

void Console_Print(ascii str[]);

sintn Shell_Main(void) {

    uintn layerId;
    App_Syscall_NewWindow(&layerId, 0, 0, 200, 200, "Shell");

    App_Syscall_StdOut("Hello", sizeof("Hello"));
    App_Syscall_StdOut("ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890abcdefghijklmnopqrstuvwxyz,./?;:]@[^", sizeof("ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890abcdefghijklmnopqrstuvwxyz,./?;:]@[^"));

    ascii strBuff[64];
    App_Syscall_StdIn(strBuff, sizeof(strBuff));

    App_Syscall_StdOut(strBuff, sizeof(strBuff));

    Task_Message message;
    while(1) {
        App_Syscall_ReadMessage(&message);
    }

    return 0;
}

