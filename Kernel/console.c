#include <types.h>
#include <kernel.h>
#include <efi.h>
#include "font.h"
#include "console.h"

#define CONSOLE_BUFFSIZE (10000)

extern KernelInputStruct* KernelInput;

static uint32* frameBuff_StartAddr = 0;
static uintn frameBuff_ScanLineWidth = 0;
static uintn frameBuff_Width = 0;
static uintn frameBuff_Height = 0;

static uintn Console_CursorX = 0;
static uintn Console_CursorY = 0;
static uintn Console_Width = 40;
static uintn Console_Height = 30;
static ascii Console_Buff[CONSOLE_BUFFSIZE];

void Console_Init() {
    frameBuff_StartAddr = (uint32*)(KernelInput->Graphic.startAddr);
    frameBuff_ScanLineWidth = KernelInput->Graphic.scanlineWidth;
    frameBuff_Width = KernelInput->Graphic.width;
    frameBuff_Height = KernelInput->Graphic.height;

    Console_CursorX = 0;
    Console_CursorY = 0;
    Console_Width = frameBuff_Width/8;
    Console_Height = frameBuff_Height/16;
    if(CONSOLE_BUFFSIZE <= Console_Width*Console_Height) {
        Console_Height = CONSOLE_BUFFSIZE/Console_Width;
    }
    for(uintn i=0; i<Console_Height; i++) {
        for(uintn k=0; k<Console_Width; k++) {
            Console_Buff[i*Console_Width+k] = ' ';
        }
    }

    return;
}

void Console_Print(ascii str[]) {
    sintn seekindex = -1;
    while(1) {
        seekindex++;

        if(str[seekindex] == '\0') break;
        if(str[seekindex] == '\n' || Console_CursorX == Console_Width) {
            Console_CursorX = 0;
            Console_CursorY ++;
            for(uintn i=0; i<Console_Width; i++) {
                Console_Buff[Console_CursorY*Console_Width+i] = ' ';
            }
        }
        if(str[seekindex] == '\r') {
            Console_CursorX = 0;
            continue;
        }
        if(Console_CursorY == Console_Height) Console_CursorY = 0;

        if(str[seekindex] == '\n') continue;
        Console_Buff[Console_CursorY*Console_Width+Console_CursorX] = str[seekindex];

        Console_CursorX++;
    }
    return;
}

void Console_PrintLn(ascii str[]) {
    Console_Print(str);
    Console_Print("\n");
    return;
}

void Console_Flush() {
    for(uintn i=0; i<Console_Height*16; i++) {
        for(uintn k=0; k<Console_Width*8; k++) {
            frameBuff_StartAddr[frameBuff_ScanLineWidth*i + k] = 0x11111111;
        }
    }
    uintn x = 0;
    uintn y = 0;
    for(uintn i=0; i<Console_Height; i++) {
        x = 0;
        for(uintn k=0; k<Console_Width; k++) {
            Font_Draw_WhiteFont(Console_Buff[k+i*Console_Width], x, y);
            x += 8;
        }
        y += 16;
    }

    for(uintn i=16*Console_CursorY+15; i<16*Console_CursorY+18; i++) {
        for(uintn k=8*Console_CursorX; k<8*(Console_CursorX+1); k++) {
            frameBuff_StartAddr[frameBuff_ScanLineWidth*i + k] = 0xaaaaaaaa;
        }
    }

    return;
}
