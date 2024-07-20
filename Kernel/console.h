#ifndef INCLUDED_CONSOLE_H
#define INCLUDED_CONSOLE_H

void Console_Init(void);
void Console_Print(ascii str[]);

void Console_Layer_SwitchToLayerMode(void** frameBuff, uintn* width, uintn* height);

#endif
