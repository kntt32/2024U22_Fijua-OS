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
} Layer_Object;


typedef struct {
    uintn x;
    uintn y;
    uintn width;
    uintn height;

    uintn oldx;
    uintn oldy;
} Layer_Mouse;


typedef struct {
    Layer_Console Console;

    struct {
        uintn count;
        uintn pages;
        Layer_Object* Data;
    } Object;

    Layer_Mouse Mouse;
} Layer;


void Layer_Init(void);


#endif
