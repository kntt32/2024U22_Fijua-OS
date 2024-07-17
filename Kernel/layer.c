#include <types.h>
#include "graphic.h"
#include "layer.h"

#define mouseCursor_width (16)
#define mouseCursor_height (23)

void Console_Layer_OutData(void** frameBuff, uintn* width, uintn* height);

static uint8 mouseCursor_Bitmap[46] = {
#include "layer_mouse_bitmap"
};
static uint32 mouseCursor_FrameBuff[mouseCursor_width*mouseCursor_height];

static Layer layer;


void Layer_Init(void) {
    //init Layer.Console
    layer.Console.Draw.x = 0;
    layer.Console.Draw.y = 32;
    layer.Console.Draw.width = 0;
    layer.Console.Draw.height = 0;
    
    layer.Console.Change.x = 0;
    layer.Console.Change.y = 0;
    layer.Console.Change.width = 0;
    layer.Console.Change.height = 0;

    layer.Console.FrameBuff.Data.frameBuff = NULL;
    layer.Console.FrameBuff.Data.width = 0;
    layer.Console.FrameBuff.Data.height = 0;

    Console_Layer_OutData(&(layer.Console.FrameBuff.Data.frameBuff), &(layer.Console.FrameBuff.Data.width), &(layer.Console.FrameBuff.Data.height));
    layer.Console.Draw.width = layer.Console.FrameBuff.Data.width;
    layer.Console.Draw.height = layer.Console.FrameBuff.Data.height;

    //init Layer.Object
    layer.Object.count = 0;
    layer.Object.pages = 0;
    layer.Object.Data = NULL;

    //init Layer.Mouse
    layer.Mouse.x = 0;
    layer.Mouse.y = 0;
    layer.Mouse.width = mouseCursor_width;
    layer.Mouse.height = mouseCursor_height;
    layer.Mouse.oldx = 0;
    layer.Mouse.oldy = 0;

    return;
}


static uintn Layer_Update_IsWaste() {
    return 0;
}


void Layer_Update(void) {
    //Graphic_DrawFrom();


    return;
}


void Layer_DrawFrom() {
    return;
}

