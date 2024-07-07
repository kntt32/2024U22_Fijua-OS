#ifndef INCLUDED_FONT_H
#define INCLUDED_FONT_H

void Font_Init(void);

void Font_Draw_WhiteFont(in const ascii asciicode, in const uintn x, in const uintn y);

void Font_Draw(in const ascii str[], in const uintn x, in const uintn y, in const uintn red, in const uintn green, in const uintn blue);

#endif
