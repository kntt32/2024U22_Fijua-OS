#include <types.h>
#include "graphic.h"
#include "layer.h"
#include "mouse.h"
#include "functions.h"
#include "console.h"

static Layer layer;


void Layer_Init(void) {
    layer.changedFlag = 0;

    //init Layer.Console
    layer.Console.Draw.x = 0;
    layer.Console.Draw.y = 25;
    layer.Console.Draw.width = 0;
    layer.Console.Draw.height = 0;
    
    layer.Console.Change.x = 0;
    layer.Console.Change.y = 0;
    layer.Console.Change.width = 0;
    layer.Console.Change.height = 0;

    layer.Console.FrameBuff.Data.frameBuff = NULL;
    layer.Console.FrameBuff.Data.width = 0;
    layer.Console.FrameBuff.Data.height = 0;

    Console_Layer_SwitchToLayerMode(&(layer.Console.FrameBuff.Data.frameBuff), &(layer.Console.FrameBuff.Data.width), &(layer.Console.FrameBuff.Data.height));
    layer.Console.Draw.width = layer.Console.FrameBuff.Data.width;
    layer.Console.Draw.height = layer.Console.FrameBuff.Data.height;

    //init Layer.Object
    layer.Window.count = 0;
    layer.Window.pages = 0;
    layer.Window.Data = NULL;

    //init Layer.Mouse
    layer.Mouse.Draw.x = 0;
    layer.Mouse.Draw.y = 0;
    layer.Mouse.Draw.width = mouseCursor_width;
    layer.Mouse.Draw.height = mouseCursor_height;
    layer.Mouse.Draw.oldx = 0;
    layer.Mouse.Draw.oldy = 0;

    return;
}


void Layer_Update(void) {
    if(layer.changedFlag == 0) return;

    layer.changedFlag = 0;

    //hide Mouse
    Graphic_Color backcolor = {0x2d, 0x38, 0x81};
    Graphic_DrawSquare(layer.Mouse.Draw.oldx, layer.Mouse.Draw.oldy, layer.Mouse.Draw.width, layer.Mouse.Draw.height, backcolor);
    
    //draw Console
    Graphic_DrawFrom(
        layer.Console.Draw.x, layer.Console.Draw.y,
        layer.Console.Change.x, layer.Console.Change.y,
        layer.Console.Change.width, layer.Console.Change.height,
        layer.Console.FrameBuff.Data
    );

    //draw Window

    //draw Mouse
    Graphic_DrawMouse(layer.Mouse.Draw.x, layer.Mouse.Draw.y);
    layer.Mouse.Draw.oldx = layer.Mouse.Draw.x;
    layer.Mouse.Draw.oldy = layer.Mouse.Draw.y;

    return;
}

//マウス更新をLayerモジュールに通知
void Layer_Mouse_NotifyUpdate(uintn x, uintn y) {
    layer.Mouse.Draw.x = x;
    layer.Mouse.Draw.y = y;
    layer.changedFlag = 1;
    return;
}


//コンソール更新をLayerモジュールに通知
void Layer_Console_NotifyUpdate(uintn x, uintn y, uintn width, uintn height) {
    if(layer.Console.Change.width == 0 || layer.Console.Change.height == 0) {
        layer.Console.Change.x = x;
        layer.Console.Change.y = y;
        layer.Console.Change.width = width;
        layer.Console.Change.height = height;
    }else {
        if(x < layer.Console.Change.x) {
            layer.Console.Change.width += layer.Console.Change.x - x;
            layer.Console.Change.x = x;
        }
        if(y < layer.Console.Change.y) {
            layer.Console.Change.height += layer.Console.Change.y - y;
            layer.Console.Change.y = y;
        }
        if(layer.Console.Change.x+layer.Console.Change.width < x+width) {
            layer.Console.Change.width = x+width-layer.Console.Change.x;
        }
        if(layer.Console.Change.y+layer.Console.Change.height < y+height) {
            layer.Console.Change.height = y+height-layer.Console.Change.y;
        }
    }
    layer.changedFlag = 1;
    return;
}
