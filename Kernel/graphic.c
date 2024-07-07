#include <types.h>
#include <kernel.h>

//#include "graphic.h"

typedef struct {
    uint8 red;
    uint8 green;
    uint8 blue;
} Graphic_Color;

typedef struct {
    void* frameBuff;
    uintn width;
    uintn height;
} Graphic_FrameBuff;

extern KernelInputStruct* KernelInput;

static uint32* Framebuff = NULL;
static uintn Width;
static uintn Height;
static uintn ScanlineWidth;

void (*Graphic_DrawSquare)(sintn x, sintn y, uintn width, uintn height, Graphic_Color) = NULL;
void (*Graphic_DrawFrom)(sintn x, sintn y, uintn width, uintn height, Graphic_FrameBuff from) = NULL;

void Graphic_DrawSquare_BGR(sintn x, sintn y, uintn width, uintn height, Graphic_Color color);
void Graphic_DrawSquare_RGB(sintn x, sintn y, uintn width, uintn height, Graphic_Color color);

void Graphic_DrawFrom_BGR(sintn x, sintn y, uintn width, uintn height, Graphic_FrameBuff from);
void Graphic_DrawFrom_RGB(sintn x, sintn y, uintn width, uintn height, Graphic_FrameBuff from);


void Graphic_Init(void) {
    Framebuff = (uint32*)(KernelInput->Graphic.startAddr);
    ScanlineWidth = KernelInput->Graphic.scanlineWidth;
    Width = KernelInput->Graphic.width;
    Height = KernelInput->Graphic.height;

    if(KernelInput->Graphic.mode == 1) {
        Graphic_DrawSquare = Graphic_DrawSquare_RGB;
        Graphic_DrawFrom = Graphic_DrawFrom_RGB;
    }else {
        Graphic_DrawSquare = Graphic_DrawSquare_BGR;
        Graphic_DrawFrom = Graphic_DrawFrom_BGR;
    }
    
    return;
}


void Graphic_DrawSquare_BGR(sintn x, sintn y, uintn width, uintn height, Graphic_Color color) {
    if(x<0) {
        width += x;
        x = 0;
    }
    if(y<0) {
        height += y;
        y = 0;
    }
    if(Width <= (uintn)x) return;
    if(Height <= (uintn)y) return;
    if(Width <= (uintn)x+width) width = Width-x;
    if(Height <= (uintn)y+height) height = Height-y;

    uint64* targetFrameBuff = (uint64*)((uintn)Framebuff + ((x + y*ScanlineWidth) << 2));
    uint64 drawcolor = 0;
    drawcolor = (color.red<<16) + (color.green << 8) + color.blue;
    drawcolor += (drawcolor << 32);

    for(uintn i=0; i<height; i++) {
        for(uintn k=0; k<(width>>1); k++) {
            *targetFrameBuff = drawcolor;//drawcolor;
            targetFrameBuff++;
        }
        if((width&0x1) == 0x1) *((uint32*)targetFrameBuff) = drawcolor >> 32;
        targetFrameBuff = (uint64*)(((uintn)targetFrameBuff) + ((ScanlineWidth - ((width>>1)<<1))<<2));
    }

    return;
}


void Graphic_DrawSquare_RGB(sintn x, sintn y, uintn width, uintn height, Graphic_Color color) {
    if(x<0) {
        width += x;
        x = 0;
    }
    if(y<0) {
        height += y;
        y = 0;
    }
    if(Width <= (uintn)x) return;
    if(Height <= (uintn)y) return;
    if(Width <= (uintn)x+width) width = Width-x;
    if(Height <= (uintn)y+height) height = Height-y;

    uint64* targetFrameBuff = (uint64*)((uintn)Framebuff + ((x + y*ScanlineWidth) << 2));
    uint64 drawcolor = 0;
    drawcolor = (color.blue << 16) + (color.green << 8) + color.red;
    drawcolor += (drawcolor << 32);

    for(uintn i=0; i<height; i++) {
        for(uintn k=0; k<(width>>1); k++) {
            *targetFrameBuff = drawcolor;//drawcolor;
            targetFrameBuff++;
        }
        if((width&0x1) == 0x1) *((uint32*)targetFrameBuff) = drawcolor >> 32;
        targetFrameBuff = (uint64*)(((uintn)targetFrameBuff) + ((ScanlineWidth - ((width>>1)<<1))<<2));
    }

    return;
}


void Graphic_DrawFrom_BGR(sintn x, sintn y, uintn width, uintn height, Graphic_FrameBuff from) {
    void* fromMemStart = from.frameBuff;

    if(from.width <= width) width = from.width;
    if(from.height <= height) height = from.height;

    if(x<0) {
        width += x;
        fromMemStart = (void*)((uintn)fromMemStart - (x<<2));
        x = 0;
    }
    if(y<0) {
        height += y;
        fromMemStart = (void*)((uintn)fromMemStart - (y*from.width << 2));
        y = 0;
    }
    if(Width <= (uintn)x) return;
    if(Height <= (uintn)y) return;
    if(Width <= (uintn)x+width) width = Width-x;
    if(Height <= (uintn)y+height) height = Height-y;

    uint64* targetFrameBuff = (uint64*)((uintn)Framebuff + ((x + y*ScanlineWidth) << 2));
    uint64* targetFromFrameBuff = (uint64*)fromMemStart;

    for(uintn i=0; i<height; i++) {
        for(uintn k=0; k<width>>1; k++) {
            *targetFrameBuff = *targetFromFrameBuff;
            targetFrameBuff++;
            targetFromFrameBuff++;
        }

        if((width&0x1) == 0x1) *((uint32*)targetFrameBuff) = *((uint32*)targetFromFrameBuff);
        targetFrameBuff = (uint64*)(((uintn)targetFrameBuff) + ((ScanlineWidth - ((width>>1)<<1))<<2));
        targetFromFrameBuff = (uint64*)(((uintn)targetFromFrameBuff) + ((from.width - ((width>>1)<<1))<<2));
    }

    return;
}


void Graphic_DrawFrom_RGB(sintn x, sintn y, uintn width, uintn height, Graphic_FrameBuff from) {
    void* fromMemStart = from.frameBuff;

    if(from.width <= width) width = from.width;
    if(from.height <= height) height = from.height;

    if(x<0) {
        width += x;
        fromMemStart = (void*)((uintn)fromMemStart - (x<<2));
        x = 0;
    }
    if(y<0) {
        height += y;
        fromMemStart = (void*)((uintn)fromMemStart - (y*from.width << 2));
        y = 0;
    }
    if(Width <= (uintn)x) return;
    if(Height <= (uintn)y) return;
    if(Width <= (uintn)x+width) width = Width-x;
    if(Height <= (uintn)y+height) height = Height-y;

    uint64* targetFrameBuff = (uint64*)((uintn)Framebuff + ((x + y*ScanlineWidth) << 2));
    uint64* targetFromFrameBuff = (uint64*)fromMemStart;

    for(uintn i=0; i<height; i++) {
        for(uintn k=0; k<width>>1; k++) {
            *targetFrameBuff = ((*targetFromFrameBuff & 0xff000000ff)<<16) + (*targetFromFrameBuff & 0xff0000ff00) + ((*targetFromFrameBuff & 0xff000000ff0000)>>16);
            targetFrameBuff++;
            targetFromFrameBuff++;
        }

        if((width&0x1) == 0x1) *((uint32*)targetFrameBuff) = ((*((uint32*)targetFromFrameBuff) & 0xff)<<16) + (*((uint32*)targetFromFrameBuff) & 0xff00) + ((*((uint32*)targetFromFrameBuff) & 0xff0000)>>16);
        targetFrameBuff = (uint64*)(((uintn)targetFrameBuff) + ((ScanlineWidth - ((width>>1)<<1))<<2));
        targetFromFrameBuff = (uint64*)(((uintn)targetFromFrameBuff) + ((from.width - ((width>>1)<<1))<<2));
    }

    return;
}

