#ifndef INCLUDED_LAYER_H
#define INCLUDED_LAYER_H

typedef struct {
    uint16 taskId;

    uint8 hiddenFlag;

    enum {
        Layer_Object_Type_Console,
        Layer_Object_Type_Window
    } type;

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
    //uintn 
} Layer_Mouse;

typedef struct {
    Layer_Object console;

    struct {
        uintn count;
        uintn pages;
        Layer_Object* Data;
    } Object;

    Layer_Mouse mouse;
} Layer;

void Layer_Init(void);

#endif
