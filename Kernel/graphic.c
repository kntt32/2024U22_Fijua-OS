#define FILE_GRAPHIC_C

#include <types.h>
#include <kernel.h>
#include "graphic.h"

#define Graphic_RGB2BGR_UINT64(rgb) (((rgb & 0x00ff000000ff0000) >> 16) + (rgb & 0x0000ff000000ff00) + ((rgb & 0x000000ff000000ff) << 16))
#define Graphic_RGB2BGR_UINT32(rgb) (((rgb & 0x00ff0000) >> 16) + (rgb & 0x0000ff00) + ((rgb & 0x000000ff) << 16))

extern KernelInputStruct* KernelInput;

static uint32* Framebuff = NULL;
static uintn Width;
static uintn Height;
static uintn ScanlineWidth;

static uint8 mouseCursor_Bitmap[mouseCursor_width*mouseCursor_height*2] = {
#include "graphic_mouse_bitmap"
};

void (*Graphic_DrawSquare)(sintn x, sintn y, uintn width, uintn height, Graphic_Color) = NULL;
void (*Graphic_DrawFrom)(sintn x, sintn y, uintn xfrom, uintn yfrom, uintn width, uintn height, Graphic_FrameBuff from) = NULL;

void Graphic_DrawSquare_BGR(sintn x, sintn y, uintn width, uintn height, Graphic_Color color);
void Graphic_DrawSquare_RGB(sintn x, sintn y, uintn width, uintn height, Graphic_Color color);

void Graphic_DrawFrom_BGR(sintn x, sintn y, uintn xfrom, uintn yfrom, uintn width, uintn height, Graphic_FrameBuff from);
void Graphic_DrawFrom_RGB(sintn x, sintn y, uintn xfrom, uintn yfrom, uintn width, uintn height, Graphic_FrameBuff from);


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

    Graphic_Color backcolor = {0x2d, 0x38, 0x81};
    Graphic_DrawSquare(0, 0, Width, Height, backcolor);
    
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


void Graphic_DrawFrom_BGR(sintn x, sintn y, uintn xfrom, uintn yfrom, uintn width, uintn height, Graphic_FrameBuff from) {
    if(from.frameBuff == NULL) return;

    sintn movX = 0;
    sintn movY = 0;

    if(x<0) {
        width -= (uintn)(-x);
        xfrom += (uintn)(-x);
        movX = -xfrom;
        x = 0;
    }
    if(y<0) {
        height -= (uintn)(-y);
        yfrom += (uintn)(-y);
        movY = -yfrom;
        y = 0;
    }

    if(Width <= (uintn)x || Height <= (uintn)y) return;
    if(Width <= (uintn)x+width) width = Width-(uintn)x;
    if(Height <= (uintn)y+height) height = Height-(uintn)y;

    if(from.width <= xfrom || from.height <= yfrom) return;
    if(from.width <= xfrom+width) width = from.width-xfrom;
    if(from.height <= yfrom+height) height = from.height-yfrom;

    uint64* targetFrameBuff = (uint64*)((uintn)Framebuff + (x+xfrom+movX)*4 + (y+yfrom+movY)*ScanlineWidth*4);
    uint64* targetFromFrameBuff = (uint64*)((uintn)from.frameBuff + xfrom*4 + yfrom*from.width*4);

    for(uintn i=0; i<height; i++) {
        for(uintn k=0; k<(width>>1); k++) {
            *targetFrameBuff = *targetFromFrameBuff & 0x00ffffff00ffffff;

            targetFrameBuff++;
            targetFromFrameBuff++;
        }
        if(width & 0x1) {
            *((uint32*)targetFrameBuff) = *((uint32*)targetFromFrameBuff);
        }

        targetFrameBuff = (uint64*)((uintn)targetFrameBuff + ((ScanlineWidth - (width&0xfffffffffffffffe))<<2));
        targetFromFrameBuff = (uint64*)((uintn)targetFromFrameBuff + ((from.width - (width&0xfffffffffffffffe))<<2));
    }

    return;
}


void Graphic_DrawFrom_RGB(sintn x, sintn y, uintn xfrom, uintn yfrom, uintn width, uintn height, Graphic_FrameBuff from) {
    if(from.frameBuff == NULL) return;

    sintn movX = 0;
    sintn movY = 0;

    if(x<0) {
        width -= (uintn)(-x);
        xfrom += (uintn)(-x);
        movX = -xfrom;
        x = 0;
    }
    if(y<0) {
        height -= (uintn)(-y);
        yfrom += (uintn)(-y);
        movY = -yfrom;
        y = 0;
    }

    if(Width <= (uintn)x || Height <= (uintn)y) return;
    if(Width <= (uintn)x+width) width = Width-(uintn)x;
    if(Height <= (uintn)y+height) height = Height-(uintn)y;

    if(from.width <= xfrom || from.height <= yfrom) return;
    if(from.width <= xfrom+width) width = from.width-xfrom;
    if(from.height <= yfrom+height) height = from.height-yfrom;

    uint64* targetFrameBuff = (uint64*)((uintn)Framebuff + (x+xfrom+movX)*4 + (y+yfrom+movY)*ScanlineWidth*4);
    uint64* targetFromFrameBuff = (uint64*)((uintn)from.frameBuff + xfrom*4 + yfrom*from.width*4);
    
    for(uintn i=0; i<height; i++) {
        for(uintn k=0; k<(width>>1); k++) {
            *targetFrameBuff = Graphic_RGB2BGR_UINT64((*targetFromFrameBuff));

            targetFrameBuff++;
            targetFromFrameBuff++;
        }
        if(width & 0x1) {
            *((uint32*)targetFrameBuff) = Graphic_RGB2BGR_UINT32((*((uint32*)targetFromFrameBuff)));
        }

        targetFrameBuff = (uint64*)((uintn)targetFrameBuff + ((ScanlineWidth - (width&0xfffffffffffffffe))<<2));
        targetFromFrameBuff = (uint64*)((uintn)targetFromFrameBuff + ((from.width - (width&0xfffffffffffffffe))<<2));
    }

    return;
}


void Graphic_DrawMouse(uintn x, uintn y) {
    uintn targFrameBuff_Index = y*ScanlineWidth+x;
    const uintn width =
                        (Width <= x+mouseCursor_width)
                            ?((x<Width)?(Width-x):(0))
                            :(mouseCursor_width);
    const uintn width1 = (width<8)?(width):(8);
    const uintn width2 = (width<8)?(0):(width);
    const uintn height = (Height <= y+mouseCursor_height)
                            ?((y<Height)?(Height-y):(0))
                            :(mouseCursor_height);

    for(uintn i=0; i<height; i++) {
        //draw white
        for(uintn k=0; k<width1; k++) {
            if(mouseCursor_Bitmap[i*4+2] & (0x80 >> k)) {
                Framebuff[targFrameBuff_Index + k] = 0x00ffffff;
            }
        }
        for(uintn k=8; k<width2; k++) {
            if(mouseCursor_Bitmap[i*4+3] & (0x80 >> (k-8))) {
                Framebuff[targFrameBuff_Index + k] = 0x00ffffff;
            }
        }

        //draw black
        for(uintn k=0; k<width1; k++) {
            if(mouseCursor_Bitmap[i*4] & (0x80 >> k)) {
                Framebuff[targFrameBuff_Index + k] = 0x00000000;
            }
        }
        for(uintn k=8; k<width2; k++) {
            if(mouseCursor_Bitmap[i*4+1] & (0x80 >> (k-8))) {
                Framebuff[targFrameBuff_Index + k] = 0x00000000;
            }
        }

        targFrameBuff_Index += ScanlineWidth;
    }
    return;
}


void Graphic_FrameBuff_DrawSquare(Graphic_FrameBuff framebuff, sintn x, sintn y, uintn width, uintn height, Graphic_Color color) {
    if(framebuff.frameBuff == NULL) return;
    if(x<0) {
        width += x;
        x = 0;
    }
    if(y<0) {
        height += y;
        y = 0;
    }
    if(framebuff.width <= (uintn)x) return;
    if(framebuff.height <= (uintn)y) return;
    if(framebuff.width <= (uintn)x+width) width = framebuff.width-x;
    if(framebuff.height <= (uintn)y+height) height = framebuff.height-y;

    uint64* targetFrameBuff = (uint64*)((uintn)framebuff.frameBuff + ((x + y*framebuff.width) << 2));
    uint64 drawcolor = 0;
    drawcolor = (color.red<<16) + (color.green << 8) + color.blue;
    drawcolor += (drawcolor << 32);

    for(uintn i=0; i<height; i++) {
        for(uintn k=0; k<(width>>1); k++) {
            *targetFrameBuff = drawcolor;//drawcolor;
            targetFrameBuff++;
        }
        if((width&0x1) == 0x1) *((uint32*)targetFrameBuff) = drawcolor >> 32;
        targetFrameBuff = (uint64*)(((uintn)targetFrameBuff) + ((framebuff.width - ((width>>1)<<1))<<2));
    }

    return;
}

