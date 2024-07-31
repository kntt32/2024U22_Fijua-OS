#ifndef INCLUDED_TERMINAL_X64_H
#define INCLUDED_TERMINAL_X64_H

sintn Terminal_Syscall_NewWindow(out uintn* layerId, in uintn x, in uintn y, in uintn width, in uintn height, in ascii title[]);

sintn Terminal_Syscall_YieldCpu(void);

sintn Terminal_Syscall_DrawSquare(in uintn layerId, in uintn x, in uintn y, in uintn width, in uintn height, in Graphic_Color color);

sintn Terminal_Syscall_DrawFont(in uintn layerId, in uintn x, in uintn y, in ascii asciicode, in Graphic_Color color);

sintn Terminal_Syscall_ReadMessage(out Task_Message* message);

sintn Terminal_Syscall_CheckMessage(out Task_Message* message);

sintn Terminal_Syscall_SendITCMessage(in uint16 taskId, in uint64 message);

#endif
