#include <types.h>
#include "graphic.h"
#include "queue.h"
#include "task.h"
#include "terminal.h"
#include "terminal_x64.h"

#define Terminal_StrWidth (50)
#define Terminal_StrHeight (20)


typedef struct {
    uintn layerId;

    uintn cursorX;
    uintn cursorY;

    ascii strBuff[Terminal_StrWidth*Terminal_StrHeight];
    uint8 updateFlag[Terminal_StrHeight];
} Terminal;


void Terminal_Init(Terminal* this);
void Terminal_Flush(Terminal* this);
void Terminal_Print(Terminal* this, ascii str[]);

static const Graphic_Color Terminal_BackgroundColor = {0x1b, 0x1d, 0x29};
static const Graphic_Color Terminal_FontColor = {0xd3, 0xd4, 0xde};


sintn Terminal_Main(void) {
    Terminal terminal;

    Terminal_Init(&terminal);
    if(Terminal_Syscall_NewWindow(&(terminal.layerId), 100, 100, Terminal_StrWidth*8, Terminal_StrHeight*16, "Terminal")) return -1;
    Terminal_Syscall_DrawSquare(terminal.layerId, 0, 0, Terminal_StrWidth*8, Terminal_StrHeight*16, Terminal_BackgroundColor);

    Terminal_Flush(&terminal);

    Terminal_Print(&terminal, "Hello, Terminal!");
    Terminal_Print(&terminal, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    Terminal_Print(&terminal, "123456789");

    Terminal_Flush(&terminal);

    Task_Message message;
    while(1) {
        Terminal_Syscall_ReadMessage(&message);
    }

    return 0;
}


//ターミナルの初期化
void Terminal_Init(Terminal* this) {
    if(this == NULL) return;

    this->layerId = 0;

    this->cursorX = 0;
    this->cursorY = 0;
    for(uintn i=0; i<Terminal_StrWidth*Terminal_StrHeight; i++) {
        this->strBuff[i] = ' ';
    }
    for(uintn i=0; i<Terminal_StrHeight; i++) this->updateFlag[i] = 1;

    return;
}


//ターミナルのスクロール
void Terminal_Scroll(Terminal* this) {
    if(this == NULL) return;

    for(uintn i=0; i<Terminal_StrHeight; i++) {
        this->updateFlag[i] = 1;
    }
    for(uintn i=1; i<Terminal_StrHeight; i++) {
        for(uintn k=0; k<Terminal_StrWidth; k++) {
            this->strBuff[k+(i-1)*Terminal_StrWidth] = this->strBuff[k+i*Terminal_StrWidth];
        }
    }
    for(uintn i=0; i<Terminal_StrWidth; i++) {
        this->strBuff[i+(Terminal_StrHeight-1)*Terminal_StrWidth] = ' ';
    }
    this->cursorY--;

    return;
}


//ターミナルの描画
void Terminal_Flush(Terminal* this) {
    if(this == NULL) return;

    for(uintn i=0; i<Terminal_StrHeight; i++) {
        if(this->updateFlag[i]) {
            this->updateFlag[i] = 0;
            for(uintn k=0; k<Terminal_StrWidth; k++) {
                Terminal_Syscall_DrawSquare(this->layerId, k*8, i*16, 8, 16, Terminal_BackgroundColor);
                Terminal_Syscall_DrawFont(this->layerId, k*8, i*16, this->strBuff[k+i*Terminal_StrWidth], Terminal_FontColor);
            }
        }
    }
    
    return;
}


void Terminal_Print(Terminal* this, ascii str[]) {
    if(this == NULL) return;

    sintn index = -1;
    while(1) {
        index ++;
        switch(str[index]) {
            case '\0':
                return;
            case '\n':
                this->cursorX = 0;
                this->cursorY ++;
                //if(Terminal_StrHeight <= this->cursorY) Terminal_Scroll(this);
                break;
            default:
                this->strBuff[this->cursorX + this->cursorY*Terminal_StrWidth] = str[index];
                this->updateFlag[this->cursorY] = 1;
                
                this->cursorX ++;/*
                if(Terminal_StrWidth <= this->cursorX) {
                    this->cursorX = 0;
                    this->cursorY ++;
                    if(Terminal_StrHeight <= this->cursorY) Terminal_Scroll(this);
                }*/
                break;
        }
    }

    return;
}


