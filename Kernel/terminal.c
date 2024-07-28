#include <types.h>
#include "terminal.h"

sintn Terminal_Syscall_NewWindow(in uintn x, in uintn y, in uintn width, in uintn height, in ascii title[]);

sintn Terminal_Syscall_YieldCpu(void);

void Console_Print(ascii*);
void HltLoop();
sintn terminal_main(void) {
    Terminal_Syscall_NewWindow(0, 0, 400, 300, "Terminal");

    while(1) {
        Terminal_Syscall_YieldCpu();
    }

    return 0;
}
