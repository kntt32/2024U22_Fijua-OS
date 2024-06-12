#include <types.h>
#include <kernel.h>
#include <efi.h>
#include "font.h"
#include "console.h"

#define CONSOLE_WIDTH (40)
#define CONSOLE_HEIGHT (30)

extern KernelInputStruct* KernelInput;

static uint32* frameBuff_StartAddr = 0;
static uintn frameBuff_ScanLineWidth = 0;
static uintn frameBuff_Width = 0;
static uintn frameBuff_Height = 0;

static uintn Console_CursorX = 0;
static uintn Console_CursorY = 0;
static ascii Console_Buff[CONSOLE_HEIGHT*CONSOLE_WIDTH];

void Console_Init() {
    frameBuff_StartAddr = (uint32*)(KernelInput->Graphic.startAddr);
    frameBuff_ScanLineWidth = KernelInput->Graphic.scanlineWidth;
    frameBuff_Width = KernelInput->Graphic.width;
    frameBuff_Height = KernelInput->Graphic.height;

    Console_CursorX = 0;
    Console_CursorY = 0;
    for(uintn i=0; i<CONSOLE_HEIGHT; i++) {
        for(uintn k=0; k<CONSOLE_WIDTH; k++) {
            Console_Buff[i*CONSOLE_WIDTH+k] = ' ';
        }
    }

    return;
}

void Console_Print(ascii str[]) {
    sintn seekindex = -1;
    while(1) {
        seekindex++;

        if(str[seekindex] == '\0') break;
        if(str[seekindex] == '\n' || Console_CursorX == CONSOLE_WIDTH) {
            Console_CursorX = 0;
            Console_CursorY ++;
            for(int i=0; i<CONSOLE_WIDTH; i++) {
                Console_Buff[Console_CursorY*CONSOLE_WIDTH+i] = ' ';
            }
        }
        if(str[seekindex] == '\r') {
            Console_CursorX = 0;
            continue;
        }
        if(Console_CursorY == CONSOLE_HEIGHT) Console_CursorY = 0;

        if(str[seekindex] == '\n') continue;
        Console_Buff[Console_CursorY*CONSOLE_WIDTH+Console_CursorX] = str[seekindex];

        Console_CursorX++;
    }
    return;
}

void Console_PrintLn(ascii str[]) {

}

void Console_Flush() {
    for(int i=0; i<CONSOLE_HEIGHT*16; i++) {
        for(int k=0; k<CONSOLE_WIDTH*8; k++) {
        	frameBuff_StartAddr[frameBuff_ScanLineWidth*i + k] = 0x11111111;
        }
    }
    uintn x = 0;
    uintn y = 0;
    for(int i=0; i<CONSOLE_HEIGHT; i++) {
    	x = 0;
    	for(int k=0; k<CONSOLE_WIDTH; k++) {
		    Font_Draw_WhiteFont(Console_Buff[k+i*CONSOLE_WIDTH], x, y);
		    x += 8;
		}
		y += 16;
    }
/*
    for(int i=0; i<5; i++) {
        for(int k=8*Console_CursorX; k<8*(Console_CursorX+1); k++) {

        }
    }*/
    return;
}
