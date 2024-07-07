#include <types.h>
#include <kernel.h>
#include <efi.h>
#include "x64.h"
#include "graphic.h"
#include "font.h"
#include "console.h"

#define CONSOLE_BUFF (1000)


static ascii buff[100*10];
static uintn cursorX = 0;
static uintn cursorY = 0;

static uint32* console_framebuff[CONSOLE_BUFF*16*8];
static Graphic_FrameBuff console_framebuffData;


void Console_Init(void) {
    console_framebuffData.frameBuff = console_framebuff;
    console_framebuffData.width = 100*8;
    console_framebuffData.height = 10*16;

    return;
}


static void Console_Scroll(void) {

}


static void Console_FlushLine(uintn line) {

}


void Console_Print(ascii str[]) {

}



