#include <types.h>
#include <kernel.h>
#include <efi.h>
#include "console.h"

#define CONSOLE_WIDTH (80)
#define CONSOLE_HEIGHT (60)




static uintn scroll = 0;
static uintn cursorX = 0;
static uintn cursorY = 0;
static char buff[CONSOLE_HEIGHT][CONSOLE_WIDTH+1];

void Console_Init() {
	scroll = 0;
	cursorX = 0;
	cursorY = 0;
	for(uintn i=0; i<CONSOLE_HEIGHT; i++) {
		for(uintn k=0; k<CONSOLE_WIDTH; k++) {
			buff[i][k] = ' ';
		}
		buff[i][CONSOLE_WIDTH] = '\0';
	}

	return;
}

void Console_Print(ascii str[]) {
	sintn seekindex = -1;
	while(1) {
		seekindex++;

		if(str[seekindex] == '\0') break;
		if(str[seekindex] == '\n' || cursorX == CONSOLE_WIDTH) {
			cursorX = 0;
			cursorY ++;
		}
		if(str[seekindex] == '\r') {
			cursorX = 0;
			continue;
		}

		buff[cursorY][cursorX] = str[seekindex];

		cursorX++;
	}
	return;
}

void Console_PrintLn(ascii str[]) {

}

void Console_Flush() {

}

