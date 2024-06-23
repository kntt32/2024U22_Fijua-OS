#include <types.h>
#include <kernel.h>
#include <efi.h>
#include "x64.h"
#include "font.h"
#include "console.h"

#define CONSOLE_BUFFSIZE (10000)

extern KernelInputStruct* KernelInput;

static uint32* frameBuff_StartAddr = 0;
static uintn frameBuff_ScanLineWidth = 0;
static uintn frameBuff_Width = 0;
static uintn frameBuff_Height = 0;
static uintn frameBuff_Mode = 0;

static uintn Console_CursorX = 0;
static uintn Console_CursorY = 0;
static uintn Console_Width = 40;
static uintn Console_Height = 30;
static ascii Console_Buff[CONSOLE_BUFFSIZE];

static uint32 Console_BackGroundColor = 0;

static uintn Console_Mutex = 0;

void Console_Init() {
    frameBuff_StartAddr = (uint32*)(KernelInput->Graphic.startAddr);
    frameBuff_ScanLineWidth = KernelInput->Graphic.scanlineWidth;
    frameBuff_Width = KernelInput->Graphic.width;
    frameBuff_Height = KernelInput->Graphic.height;
    frameBuff_Mode = KernelInput->Graphic.mode;

    Console_BackGroundColor = (frameBuff_Mode == 1)?(0x003ec3f0):(0x00f0c33e);

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

    for(uintn i=0; i<Console_Height; i++) {
        for(uintn k=0; k<Console_Width; k++) {
            frameBuff_StartAddr[k] = Console_BackGroundColor;
        }
    }

    return;
}

void Console_Scroll() {
    Mutex_Lock(&Console_Mutex);

    for(uintn i=0; i<Console_Height-1; i++) {
        for(uintn k=0; k<Console_Width; k++) {
            Console_Buff[i*Console_Width+k] = Console_Buff[(i+1)*Console_Width+k];
        }
    }
    for(uintn i=0; i<Console_Width; i++) {
        Console_Buff[(Console_Height-1)*Console_Width+i] = ' ';
    }

    Mutex_Lock(&Console_Mutex);

    return;
}

void Console_Print(ascii str[]) {
    Mutex_Lock(&Console_Mutex);

    sintn seekindex = -1;
    while(1) {
        seekindex++;

        if(str[seekindex] == '\0') break;
        if(str[seekindex] == '\n' || Console_CursorX == Console_Width) {
            Console_CursorX = 0;
            Console_CursorY ++;
            if(Console_CursorY == Console_Height) {
                Console_CursorY--;
                Mutex_UnLock(&Console_Mutex);
                Console_Scroll();
                Mutex_Lock(&Console_Mutex);
            }else {
                for(uintn i=0; i<Console_Width; i++) {
                    Console_Buff[Console_CursorY*Console_Width+i] = ' ';
                }
            }
        }
        if(str[seekindex] == '\r') {
            Console_CursorX = 0;
            continue;
        }

        if(str[seekindex] == '\n') continue;
        Console_Buff[Console_CursorY*Console_Width+Console_CursorX] = str[seekindex];

        Console_CursorX++;
    }
    Mutex_UnLock(&Console_Mutex);
    Console_Flush();
    return;
}

void Console_PrintLn(ascii str[]) {
    Console_Print(str);
    Console_Print("\n");
    return;
}

void Console_Flush() {
    for(sintn i=Console_Height-1; 0<=i; i--) {
        Console_FlushLine(i);
    }

    return;
}

void Console_FlushLine(uintn line) {
    Mutex_Lock(&Console_Mutex);
    for(uintn i=line*16; i<line*16+16; i++) {
        for(uintn k=0; k<frameBuff_Width; k++) {
            frameBuff_StartAddr[frameBuff_ScanLineWidth*i + k] = Console_BackGroundColor; //_rgb;
        }
    }
    uintn x = 0;
    uintn y = line*16;
    for(uintn k=0; k<Console_Width; k++) {
        Font_Draw_WhiteFont(Console_Buff[k+line*Console_Width], x, y);
        x += 8;
    }
    y += 16;

    if(Console_CursorY == line) {
        for(uintn i=16*Console_CursorY+14; i<16*Console_CursorY+16; i++) {
            for(uintn k=8*Console_CursorX; k<8*(Console_CursorX+1); k++) {
                frameBuff_StartAddr[frameBuff_ScanLineWidth*i + k] = 0xaaaaaaaa;
            }
        }
    }
    Mutex_UnLock(&Console_Mutex);

    return;
}
