#ifndef INCLUDED_LAYER_H
#define INCLUDED_LAYER_H


typedef struct {
    struct {
        uintn x;
        uintn y;
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
        uintn x;
        uintn y;
        uintn visualWidth;
        uintn visualHeight;

        uintn oldx;
        uintn oldy;
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
        uintn x;
        uintn y;
        uintn width;
        uintn height;

        uintn oldx;
        uintn oldy;
    } Draw;
} Layer_Mouse;


typedef struct {
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

void Layer_Mouse_NotifyMove(uintn x, uintn y);


#endif
