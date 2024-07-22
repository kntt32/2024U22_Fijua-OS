#include <types.h>
#include "graphic.h"
#include "layer.h"
#include "memory.h"
#include "mouse.h"
#include "functions.h"
#include "console.h"
#include "queue.h"
#include "task.h"

static Layer layer;

static const Graphic_Color window_titleBar_backgroundColor = {0xf1, 0xf1, 0xf1};
static const Graphic_Color window_shadow_color = {0x00, 0x00, 0x00};
static const Graphic_Color window_titleBar_closeButton_color = {0xf0, 0x56, 0x56};
static const Graphic_Color window_titleBar_hiddenButton_color = {0xc0, 0xc0, 0xc0};
static const Graphic_Color window_sizeDragger_color = {0xa0, 0xa0, 0xa0};
static const Graphic_Color window_sizeDragger_line_color = {0x50, 0x50, 0x50};
static const sintn window_titleBar_height = 32;
static const sintn window_shadow_overThick = 1;
static const sintn window_shadow_underThick = 2;
static const sintn window_sizeDragger_size = 20;


//Layerを初期化
void Layer_Init(void) {
    layer.changedFlag = 0;
    layer.isDrawingFlag = 0;

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

    //init Layer.Window
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

    layer.Mouse.Drag.x = 0;
    layer.Mouse.Drag.y = 0;

    return;
}


//Layerを描画
void Layer_Update(void) {
    Layer_Window* targetWindow;

    if(layer.changedFlag == 0) return;
    layer.isDrawingFlag = 1;
    layer.changedFlag = 0;

    
    if(layer.Mouse.Drag.x != 0 || layer.Mouse.Drag.y != 0) {
        targetWindow = layer.Window.Data + layer.Window.count - 1;

        for(sintn i=layer.Window.count-1; 0<=i; i--) {
            //move window
            if((targetWindow->Draw.x <= layer.Mouse.Draw.oldx && layer.Mouse.Draw.oldx < targetWindow->Draw.x + (sintn)targetWindow->Draw.visualWidth)
                && (targetWindow->Draw.y <= layer.Mouse.Draw.oldy && layer.Mouse.Draw.oldy < targetWindow->Draw.y + (sintn)window_titleBar_height)) {
                targetWindow->Draw.x += layer.Mouse.Drag.x;
                targetWindow->Draw.y += layer.Mouse.Drag.y;
                break;
            }

            //resize window
            if((targetWindow->Draw.x + (sintn)targetWindow->Draw.visualWidth - window_sizeDragger_size <= layer.Mouse.Draw.oldx && layer.Mouse.Draw.oldx < targetWindow->Draw.x + (sintn)targetWindow->Draw.visualWidth)
                && (targetWindow->Draw.y + (sintn)targetWindow->Draw.visualHeight - window_sizeDragger_size <= layer.Mouse.Draw.oldy && layer.Mouse.Draw.oldy < targetWindow->Draw.y + (sintn)targetWindow->Draw.visualHeight)) {
                if(2 <= (sintn)targetWindow->Draw.visualWidth + layer.Mouse.Drag.x && (sintn)targetWindow->Draw.visualWidth + layer.Mouse.Drag.x < (sintn)targetWindow->FrameBuff.Data.width) {
                    targetWindow->Draw.visualWidth = (sintn)(targetWindow->Draw.visualWidth) + layer.Mouse.Drag.x;
                }else {
                    if(2 > (sintn)targetWindow->Draw.visualWidth + layer.Mouse.Drag.x) {
                        targetWindow->Draw.visualWidth = 2;
                    }else {
                        targetWindow->Draw.visualWidth = targetWindow->FrameBuff.Data.width;
                    }
                }
                break;
            }
            
            targetWindow--;
        }
        
        layer.Mouse.Drag.x = 0;
        layer.Mouse.Drag.y = 0;
    }


    //hide Mouse
    Graphic_Color backcolor = {0x2d, 0x38, 0x81};
    Graphic_DrawSquare(layer.Mouse.Draw.oldx, layer.Mouse.Draw.oldy, layer.Mouse.Draw.width, layer.Mouse.Draw.height, backcolor);

    //draw Console
    Graphic_DrawFrom(
        layer.Console.Draw.x, layer.Console.Draw.y,
        0, 0,
        layer.Console.Draw.width, layer.Console.Draw.height,
        layer.Console.FrameBuff.Data
    );
    layer.Console.Change.x = 0;
    layer.Console.Change.y = 0;
    layer.Console.Change.width = 0;
    layer.Console.Change.height = 0;


    //draw Window
    targetWindow = layer.Window.Data;
    for(uintn i=0; i<layer.Window.count; i++) {
        Graphic_DrawFrom(targetWindow->Draw.x, targetWindow->Draw.y, 0, 0, targetWindow->Draw.visualWidth, targetWindow->Draw.visualHeight, targetWindow->FrameBuff.Data);
        //サイズドラッガー描画
        Graphic_DrawSquare(
            targetWindow->Draw.x + targetWindow->Draw.visualWidth - window_shadow_underThick - window_sizeDragger_size, targetWindow->Draw.y + targetWindow->Draw.visualHeight - window_shadow_underThick - window_sizeDragger_size,
            window_sizeDragger_size, window_sizeDragger_size,
            window_sizeDragger_color);
        Graphic_DrawSquare(
            targetWindow->Draw.x + targetWindow->Draw.visualWidth - window_shadow_underThick - 7, targetWindow->Draw.y + targetWindow->Draw.visualHeight - window_shadow_underThick - window_sizeDragger_size,
            1, window_sizeDragger_size,
            window_sizeDragger_line_color);
        Graphic_DrawSquare(
            targetWindow->Draw.x + targetWindow->Draw.visualWidth - window_shadow_underThick - window_sizeDragger_size, targetWindow->Draw.y + targetWindow->Draw.visualHeight - window_shadow_underThick - 7,
            window_sizeDragger_size, 1,
            window_sizeDragger_line_color);
        
        targetWindow->Draw.oldx = targetWindow->Draw.x;
        targetWindow->Draw.oldy = targetWindow->Draw.y;
        targetWindow->Draw.oldVisualWidth = targetWindow->Draw.oldVisualWidth;
        targetWindow->Draw.oldVisualHeight = targetWindow->Draw.oldVisualHeight;

        targetWindow->Change.x = 0;
        targetWindow->Change.y = 0;
        targetWindow->Change.width = 0;
        targetWindow->Change.height = 0;

        targetWindow++;
    }


    //draw Mouse
    Graphic_DrawMouse(layer.Mouse.Draw.x, layer.Mouse.Draw.y);
    layer.Mouse.Draw.oldx = layer.Mouse.Draw.x;
    layer.Mouse.Draw.oldy = layer.Mouse.Draw.y;

    layer.isDrawingFlag = 0;

    return;
}


//Layer.Windowのサイズ拡張
static uintn Layer_Window_Expand(void) {
    uintn newPages = layer.Window.pages*2 + 1;
    Layer_Window* newData = Memory_AllocPages(2, newPages);
    if(newData == NULL) return 1;
    for(uintn i=0; i<layer.Window.count; i++) {
        newData[i].taskId = layer.Window.Data[i].taskId;
        newData[i].hiddenFlag = layer.Window.Data[i].hiddenFlag;
        newData[i].layerId = layer.Window.Data[i].layerId;

        newData[i].Draw.x = layer.Window.Data[i].Draw.x;
        newData[i].Draw.y = layer.Window.Data[i].Draw.y;
        newData[i].Draw.visualWidth = layer.Window.Data[i].Draw.visualWidth;
        newData[i].Draw.visualHeight = layer.Window.Data[i].Draw.visualHeight;
        newData[i].Draw.oldx = layer.Window.Data[i].Draw.oldx;
        newData[i].Draw.oldy = layer.Window.Data[i].Draw.oldy;
        newData[i].Draw.oldVisualWidth = layer.Window.Data[i].Draw.oldVisualWidth;
        newData[i].Draw.oldVisualHeight = layer.Window.Data[i].Draw.oldVisualHeight;
        
        newData[i].Change.x = layer.Window.Data[i].Change.x;
        newData[i].Change.y = layer.Window.Data[i].Change.y;
        newData[i].Change.width = layer.Window.Data[i].Change.width;
        newData[i].Change.height = layer.Window.Data[i].Change.height;

        newData[i].FrameBuff.pages = layer.Window.Data[i].FrameBuff.pages;
        newData[i].FrameBuff.Data = layer.Window.Data[i].FrameBuff.Data;
    }
    Memory_FreePages(2, layer.Window.pages, layer.Window.Data);
    layer.Window.pages = newPages;
    layer.Window.Data = newData;

    return 0;
}


//Layer.Windowを作成してlayerIdを返す
uintn Layer_Window_New(uint16 taskId, ascii name[], uintn x, uintn y, uintn width, uintn height, uintn maxWidth, uintn maxHeight) {
    if(taskId == 0 || taskId == 1) return 0;
    if(width == 0 || height == 0) return 0;
    if(maxWidth<width || maxHeight<height) return 0;

    //割り当てるlayerIdを取得
    uintn layerId = 1;
    for(sintn i=0; i<(sintn)(layer.Window.count); i++) {
        if(layer.Window.Data[i].layerId == layerId) {
            layerId++;
            i = -1;
            continue;
        }
    }

    //Layer.Windowに追加
    if((layer.Window.pages << 12) <= layer.Window.count*sizeof(Layer_Window)) {
        if(Layer_Window_Expand()) return 0;
    }

    Layer_Window* newWindow = layer.Window.Data + layer.Window.count;
    newWindow->taskId = taskId;
    newWindow->hiddenFlag = 0;
    newWindow->layerId = layerId;

    newWindow->Draw.x = x;
    newWindow->Draw.y = y;
    newWindow->Draw.visualWidth = width;
    newWindow->Draw.visualHeight = height;
    newWindow->Draw.oldx = x;
    newWindow->Draw.oldy = y;
    newWindow->Draw.oldVisualWidth = 0;
    newWindow->Draw.oldVisualHeight = 0;

    newWindow->Change.x = 0;
    newWindow->Change.y = 0;
    newWindow->Change.width = width;
    newWindow->Change.height = height;

    newWindow->FrameBuff.pages = (maxWidth*maxHeight*sizeof(uint32)+0xfff)>>12;
    newWindow->FrameBuff.Data.width = maxWidth;
    newWindow->FrameBuff.Data.height = maxHeight;
    newWindow->FrameBuff.Data.frameBuff = Memory_AllocPages(taskId, newWindow->FrameBuff.pages);
    if(newWindow->FrameBuff.Data.frameBuff == NULL) return 0;

    layer.Window.count++;

    layer.changedFlag = 1;

    //フレームバッファを白く塗りつぶす
    Graphic_Color defaultColor = {0xff, 0xff, 0xff};
    Graphic_FrameBuff_DrawSquare(
        newWindow->FrameBuff.Data,
        0, 0,
        width, height,
        defaultColor);

    //タイトルバー描画
    Graphic_FrameBuff_DrawSquare(
        newWindow->FrameBuff.Data,
        window_shadow_overThick, window_shadow_overThick,
        width - window_shadow_overThick - window_shadow_underThick, window_titleBar_height,
        window_titleBar_backgroundColor);

    Graphic_FrameBuff_DrawSquare(
        newWindow->FrameBuff.Data,
        width - window_shadow_underThick - 1 - (window_titleBar_height - 2),window_shadow_overThick + 1,
        window_titleBar_height - 2, window_titleBar_height - 2,
        window_titleBar_closeButton_color);

    Graphic_FrameBuff_DrawSquare(
        newWindow->FrameBuff.Data,
        window_shadow_overThick + 1, window_shadow_overThick + 1,
        window_titleBar_height - 2, window_titleBar_height - 2,
        window_titleBar_hiddenButton_color);

    //影描画
    Graphic_FrameBuff_DrawSquare(
        newWindow->FrameBuff.Data,
        width - window_shadow_underThick, 0,
        window_shadow_underThick, height,
        window_shadow_color);
    Graphic_FrameBuff_DrawSquare(
        newWindow->FrameBuff.Data,
        0, 0,
        width, window_shadow_overThick,
        window_shadow_color);
    Graphic_FrameBuff_DrawSquare(
        newWindow->FrameBuff.Data,
        0, height - window_shadow_underThick,
        width, window_shadow_underThick,
        window_shadow_color);
    Graphic_FrameBuff_DrawSquare(
        newWindow->FrameBuff.Data,
        0, 0,
        window_shadow_overThick, height,
        window_shadow_color);



    return layerId;
}


//マウス更新をLayerモジュールに通知
void Layer_Mouse_NotifyUpdate(uintn x, uintn y, uintn leftButton) {
    if(layer.isDrawingFlag) return;
    if(leftButton) {
        layer.Mouse.Drag.x = (sintn)x - (sintn)layer.Mouse.Draw.oldx;
        layer.Mouse.Drag.y = (sintn)y - (sintn)layer.Mouse.Draw.oldy;
    }
    layer.Mouse.Draw.x = x;
    layer.Mouse.Draw.y = y;
    layer.changedFlag = 1;

    Task_SetLayerTrigger();
    
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
