#include <types.h>
#include "graphic.h"
#include "queue.h"
#include "task.h"
#include "shell.h"
#include "app_x64.h"

void Console_Print(ascii str[]);

sintn Shell_Main(void) {
    uintn layerId;
    sintn status;

    ascii strBuff[128];
    Task_Message message;
    while(1) {
        App_Syscall_StdOut("shell> ", sizeof("shell> "));
        status = App_Syscall_StdIn(strBuff, sizeof(strBuff));
        if(status != 0) {
            App_Syscall_StdOut("Shell: too large input\n", sizeof("Shell: too large input\n"));
            continue;
        }
        App_Syscall_StdOut(strBuff, sizeof(strBuff));
        App_Syscall_StdOut("\n", sizeof("\n"));
    }

    return 0;
}

