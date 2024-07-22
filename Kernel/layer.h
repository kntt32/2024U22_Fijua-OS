#ifndef INCLUDED_LAYER_H
#define INCLUDED_LAYER_H


typedef struct {
    struct {
        sintn x;
        sintn y;
        uintn width;
        uintn height;
    } Draw;

    struct {
        uintn x;
        uintn y;
        uintn width;
        uintn height;
    } Change;

    struct {
        Graphic_FrameBuff Data;
    } FrameBuff;
} Layer_Console;


typedef struct {
    uint16 taskId;

    uint8 hiddenFlag;

    uintn layerId;//!=0

    struct {
        sintn x;
        sintn y;
        uintn visualWidth;
        uintn visualHeight;

        sintn oldx;
        sintn oldy;
        uintn oldVisualWidth;
        uintn oldVisualHeight;
    } Draw;

    struct {
        uintn x;
        uintn y;
        uintn width;
        uintn height;
    } Change;

    struct {
        uintn pages;
        Graphic_FrameBuff Data;
    } FrameBuff;
} Layer_Window;


typedef struct {
    struct {
        sintn x;
        sintn y;
        uintn width;
        uintn height;

        sintn oldx;
        sintn oldy;
    } Draw;

    struct {
        sintn x;
        sintn y;
    } Drag;
} Layer_Mouse;


typedef struct {
    uintn changedFlag;
    uintn isDrawingFlag;

    Layer_Console Console;//LayerId: 0

    struct {
        uintn count;
        uintn pages;
        Layer_Window* Data;
    } Window;

    Layer_Mouse Mouse;
} Layer;


void Layer_Init(void);

void Layer_Update(void);

void Layer_Mouse_NotifyUpdate(uintn x, uintn y, uintn leftButton);

void Layer_Console_NotifyUpdate(uintn x, uintn y, uintn width, uintn height);

uintn Layer_Window_New(uint16 taskId, ascii name[], uintn x, uintn y, uintn width, uintn height, uintn maxWidth, uintn maxHeight);

#endif
