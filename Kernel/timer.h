#ifndef INCLUDED_TIMER_H
#define INCLUDED_TIMER_H

void Timer_Init(void);

sintn Timer_Set(void (*callback)(void), uintn sec100ns);

sintn Timer_Stop();

#endif
