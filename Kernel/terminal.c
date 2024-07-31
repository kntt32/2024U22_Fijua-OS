#include <types.h>
#include "terminal.h"

typedef struct {
    uint8 red;
    uint8 green;
    uint8 blue;
} Graphic_Color;

typedef struct {
    enum {
        Task_Message_Nothing,
        Task_Message_ITCMessage
    } type;

    union {
        uint64 ITCMessage;
    } data;
} Task_Message;;

sintn Terminal_Syscall_NewWindow(out uintn* layerId, in uintn x, in uintn y, in uintn width, in uintn height, in ascii title[]);

sintn Terminal_Syscall_YieldCpu(void);

sintn Terminal_Syscall_DrawSquare(in uintn layerId, in uintn x, in uintn y, in uintn width, in uintn height, Graphic_Color color);

sintn Terminal_Syscall_DrawFont(in uintn layerId, in uintn x, in uintn y, ascii asciicode, Graphic_Color color);

sintn Terminal_Syscall_ReadMessage(Task_Message* message);

sintn Terminal_Syscall_SendITCMessage(uint16 taskId, uint64 message);


void Console_Print(ascii*);
void HltLoop();

sintn terminal_main(void) {
    uintn newWindowId;
    Terminal_Syscall_NewWindow(&newWindowId, 100, 100, 400, 300, "Terminal");

    Graphic_Color color = {0, 0xff, 0xff};
    Terminal_Syscall_DrawSquare(newWindowId, 0, 0, 10, 350, color);
    Terminal_Syscall_DrawFont(newWindowId, 10, 10, 'A', color);

    Task_Message message;


    Terminal_Syscall_SendITCMessage(3, 5);

    while(1) {
        Terminal_Syscall_ReadMessage(&message);
        Console_Print("Message Recieved\n");
    }

    return 0;
}
