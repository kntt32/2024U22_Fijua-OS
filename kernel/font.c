#include <types.h>
#include <efi.h>
#include <kernel.h>

#include "font.h"


uint8 fontdata_monospace[] = {
#include "fontdata_monospace"
};

static uint8 Font_isInited = 0;
static GraphicData* Font_GraphicDataPtr;


void Font_Init(in KernelInputStruct* kernelInput) {
	Font_GraphicDataPtr = &(kernelInput->Graphic);
	Font_isInited = 1;
	return;
}


//Draw font
void Font_Draw_WhiteFont(in const ascii asciicode, in const uintn x, in const uintn y) {
	if(!Font_isInited) return;

	uint32* targetFrameBuffPtr = (uint32*)(Font_GraphicDataPtr->startAddr) + x + y*Font_GraphicDataPtr->scanlineWidth;
	uint8 fontdata_t[16] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	uint8* fontdata_ptr = fontdata_t;
	uint8 mask = 0x80;

	for(sintn i=0; i<16; i++) {
		mask = 0x80;
		for(sintn k=0; k<8; k++) {
			if(*fontdata_ptr & mask) {
				*targetFrameBuffPtr = 0xFFFFFFFF;
			}
			targetFrameBuffPtr++;
			mask >>= 1;
		}
		fontdata_ptr++;
		targetFrameBuffPtr += Font_GraphicDataPtr->scanlineWidth - 8;
	}

	return;
}

