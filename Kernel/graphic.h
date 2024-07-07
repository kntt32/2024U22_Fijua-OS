#ifndef INCLUDED_GRAPHIC_H

#define Graphic_Color2BGR(color) ((((uint32)color.red) << 16) + (((uint32)color.green) << 8) + ((uint32)color.blue))

void Graphic_Init(void);

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

extern void (*Graphic_DrawSquare)(sintn x, sintn y, uintn width, uintn height, Graphic_Color color);//draw square
extern void (*Graphic_DrawFrom)(sintn x, sintn y, uintn width, uintn height, Graphic_FrameBuff fromFrameBuff);//draw memmapped bgr frame

#endif
