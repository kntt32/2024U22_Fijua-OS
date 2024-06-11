#include <types.h>
#include <efi.h>
#include <kernel.h>

#include "font.h"


static uint8 fontdata_monospace[] = {
#include "fontdata_monospace"
};

static GraphicData* Font_GraphicDataPtr;
static uintn frameBuff_StartAddr;
static uintn frameBuff_ScanLineWidth;
static uintn frameBuff_Width;
static uintn frameBuff_Height;
static uint8 isRGB = 0;


void Font_Init(in KernelInputStruct* kernelInput) {
    Font_GraphicDataPtr = &(kernelInput->Graphic);
    frameBuff_StartAddr = kernelInput->Graphic.startAddr;
    frameBuff_ScanLineWidth = kernelInput->Graphic.scanlineWidth;
    frameBuff_Width = kernelInput->Graphic.width;
    frameBuff_Height = kernelInput->Graphic.height;

    if(kernelInput->Graphic.mode == 1) isRGB = 1;
    else isRGB = 0;

    return;
}


//draw font
void Font_Draw_WhiteFont(in const ascii asciicode, in const uintn x, in const uintn y) {
	if(frameBuff_Width<=x+8 || frameBuff_Height<=y+16) return;
    uint32* targetFrameBuffPtr = (uint32*)(frameBuff_StartAddr) + x + y*frameBuff_ScanLineWidth;
    uint8* fontdata_ptr = fontdata_monospace + (asciicode<<4);
    uint8 fontdata_copy = 0;
    uint8 mask = 0x80;

    for(sintn i=0; i<16; i++) {
        mask = 0x80;
        fontdata_copy = *fontdata_ptr;
        for(sintn k=0; k<8; k++) {
            if(fontdata_copy & mask) {
                *targetFrameBuffPtr = 0xFFFFFFFF;
            }
            targetFrameBuffPtr++;
            mask >>= 1;
        }
        fontdata_ptr++;
        targetFrameBuffPtr += frameBuff_ScanLineWidth - 8;
    }

    return;
}


//draw str
void Font_Draw(in const ascii str[], in const uintn x, in const uintn y, in const uintn red, in const uintn green, in const uintn blue) {
    uintn strindex = 0;
    uintn drawX = x;
    uintn drawY = y;

    uint8* targetFrameBuffPtr;
    uint8* fontdata_ptr;
    uint8 fontdata_copy;
    uint8 mask;

	if(isRGB) {
	    while(1) {
	        if(str[strindex] == '\0') break;
	        if(str[strindex] == '\n') {
	            drawX = x;
	            drawY += 16;
	        }
	        if(frameBuff_Width <= drawX+8) {
	        	strindex++;
	        	continue;
	    	}
	        if(frameBuff_Height <= drawY+16) break;

	        fontdata_ptr = fontdata_monospace + (str[strindex]<<4);
	        targetFrameBuffPtr = (uint8*)(frameBuff_StartAddr + (drawX + drawY*frameBuff_ScanLineWidth)*4);

	        for(int i=0; i<16; i++) {
	            mask = 0x80;
	            fontdata_copy = fontdata_ptr[i];
	            for(int k=0; k<8; k++) {
	                if(fontdata_copy & mask) {
	                    targetFrameBuffPtr[0] = red;
	                    targetFrameBuffPtr[1] = green;
	                    targetFrameBuffPtr[2] = blue;
	                }
	                targetFrameBuffPtr += 4;
	                mask >>= 1;
	            }
	            targetFrameBuffPtr += (frameBuff_ScanLineWidth - 8)*4;
	        }

	        drawX += 8;

	        strindex++;
	    }
	}else {
	    while(1) {
	        if(str[strindex] == '\0') break;
	        if(str[strindex] == '\n') {
	            drawX = x;
	            drawY += 16;
	        }
	        if(frameBuff_Width <= drawX+8) {
	        	strindex++;
	        	continue;
	    	}
	        if(frameBuff_Height <= drawY+16) break;

	        fontdata_ptr = fontdata_monospace + (str[strindex]<<4);
	        targetFrameBuffPtr = (uint8*)(frameBuff_StartAddr + (drawX + drawY*frameBuff_ScanLineWidth)*4);

	        for(int i=0; i<16; i++) {
	            mask = 0x80;
	            fontdata_copy = fontdata_ptr[i];
	            for(int k=0; k<8; k++) {
	                if(fontdata_copy & mask) {
	                    targetFrameBuffPtr[0] = blue;
	                    targetFrameBuffPtr[1] = green;
	                    targetFrameBuffPtr[2] = red;
	                }
	                targetFrameBuffPtr += 4;
	                mask >>= 1;
	            }
	            targetFrameBuffPtr += (frameBuff_ScanLineWidth - 8)*4;
	        }

	        drawX += 8;

	        strindex++;
	    }
	}

    return;
}

