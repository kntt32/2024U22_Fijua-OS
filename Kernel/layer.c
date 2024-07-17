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
    /*
    //Init Console Layer
    layer.console.taskId = 2;
    layer.console.hiddenFlag = 0;
    layer.console.type = Layer_Object_Type_Console;
    layer.console.Draw.x = 0;
    layer.console.Draw.y = 0;
    layer.console.Draw.visualWidth = 0;
    layer.console.Draw.visualHeight = 0;
    layer.console.Change.x = 0;
    layer.console.Change.y = 0;
    layer.console.Change.width = 0;
    layer.console.Change.height = 0;
    layer.console.FrameBuff.pages = 0;
    layer.console.FrameBuff.Data.frameBuff = 0;
    layer.console.FrameBuff.Data.width = 0;
    layer.console.FrameBuff.Data.height = 0;
    Console_Layer_OutData(&(layer.console.FrameBuff.Data.frameBuff), &(layer.console.FrameBuff.Data.width), &(layer.console.FrameBuff.Data.height));
    layer.console.Draw.visualWidth = layer.console.FrameBuff.Data.width;
    layer.console.Draw.visualHeight = layer.console.FrameBuff.Data.height;
    
    //Init Object
    layer.Object.count = 0;
    layer.Object.pages = 0;
    layer.Object.Data = NULL;
*/
    return;
}


static uintn Layer_Update_IsHidden() {
    return 0;
}


void Layer_Update(void) {
    //Graphic_DrawFrom();


    return;
}

