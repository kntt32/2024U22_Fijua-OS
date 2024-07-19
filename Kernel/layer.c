#include <types.h>
#include "graphic.h"
#include "layer.h"
#include "mouse.h"
#include "functions.h"
#include "console.h"

void Console_Layer_OutData(void** frameBuff, uintn* width, uintn* height);

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




static uintn Layer_Update_IsWaste() {
    return 0;
}

void Layer_Update(void) {
    /*
    //draw Console
    if(!(layer.Console.Change.width == 0 || layer.Console.Change.height == 0))
        Graphic_DrawFrom(
            layer.Console.Draw.x, layer.Console.Draw.y,
            layer.Console.Change.x, layer.Console.Change.y,
            layer.Console.Change.width, layer.Console.Change.height,
            layer.Console.FrameBuff.Data
        );
    layer.Console.Change.x = 0;
    layer.Console.Change.y = 0;
    layer.Console.Change.width = 0;
    layer.Console.Change.height = 0;
*/
    //draw Window

    //draw Mouse
    if(!(layer.Mouse.Draw.x == layer.Mouse.Draw.oldx && layer.Mouse.Draw.y == layer.Mouse.Draw.oldy)) {
        
#if 0
        ascii strbuff[19];
        SPrintIntX(layer.Mouse.Draw.x, 17, strbuff);
        strbuff[16] = ']';
        strbuff[17] = '\n';
        strbuff[18] = '\0';
        Console_Print(strbuff);
#endif
#if 1
        Graphic_Color backcolor = {0x2d, 0x38, 0x81};
        Graphic_DrawSquare(layer.Mouse.Draw.oldx, layer.Mouse.Draw.oldy, layer.Mouse.Draw.width, layer.Mouse.Draw.height, backcolor);
#endif

        Graphic_DrawMouse(layer.Mouse.Draw.x, layer.Mouse.Draw.y);

        layer.Mouse.Draw.oldx = layer.Mouse.Draw.x;
        layer.Mouse.Draw.oldy = layer.Mouse.Draw.y;
    }

    return;
}


void Layer_NotifyChanged(uintn layerId, uintn x, uintn y, uintn width, uintn height) {
    return;
}


//マウスが動いたことをLayerモジュールに通知
void Layer_Mouse_NotifyMove(uintn x, uintn y) {
    layer.Mouse.Draw.x = x;
    layer.Mouse.Draw.y = y;
    return;
}

/*
void Layer_DrawFrom(uintn layerId, sintn x, sintn y, uintn xfrom, uintn yfrom, uintn width, uintn height, Graphic_FrameBuff from) {
    if(from.frameBuff == NULL) return;
    void* targLayer_frameBuff = NULL;
    uintn targLayer_width = 0;
    uintn targLayer_height = 0;
    if(layerId == 0) {
        //Console
        targLayer_frameBuff = layer.Console.FrameBuff.Data.frameBuff;
        targLayer_width = layer.Console.FrameBuff.Data.width;
        targLayer_height = layer.Console.FrameBuff.Data.height;
    }else {
        return;
    }

    if(x<0) {
        width -= (uintn)(-x);
        xfrom += (uintn)(-x);
        x = 0;
    }
    if(y<0) {
        height -= (uintn)(-y);
        yfrom += (uintn)(-y);
        y = 0;
    }

    if(Width <= (uintn)x || Height <= (uintn)y) return;
    if(Width <= (uintn)x+width) width = Width-(uintn)x;
    if(Height <= (uintn)y+height) height = Height-(uintn)y;

    if(from.width <= xfrom || from.height <= yfrom) return;
    if(from.width <= xfrom+width) width = from.width-xfrom;
    if(from.height <= yfrom+height) height = from.height-yfrom;

    uint64* targetFrameBuff = (uint64*)((uintn)framebuff + (x+xfrom)*4 + (y+yfrom)*ScanlineWidth*4);
    uint64* targetFromFrameBuff = (uint64*)((uintn)from.frameBuff + xfrom*4 + y*from.width*4);

    for(uintn i=0; i<height; i++) {
        for(uintn k=0; k<(width>>1); k++) {
            *targetFrameBuff = *targetFromFrameBuff & 0x00ffffff00ffffff;

            targetFrameBuff++;
            targetFromFrameBuff++;
        }
        if(width & 0x1) {
            *((uint32*)targetFrameBuff) = *((uint32*)targetFromFrameBuff);
        }

        targetFrameBuff = (uint64*)((uintn)targetFrameBuff + ((ScanlineWidth - width)<<2));
        targetFromFrameBuff = (uint64*)((uintn)targetFromFrameBuff + ((from.width - width)<<2));
    }

    return;
}
*/

