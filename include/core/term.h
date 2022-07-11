#ifndef BYZ_TERM_H
#define BYZ_TERM_H

#include <stdbool.h>

struct Colour;

enum TextAttributeFlag
{
    A_NONE_BIT       = 0,
    A_BOLD_BIT       = 1 << 0,
    A_UNDERSCORE_BIT = 1 << 1,
    A_BLINK_BIT      = 1 << 2,
    A_REVERSE_BIT    = 1 << 3
};
typedef unsigned int TextAttributeFlags;

void term_init(void);
void term_uninit(void);

void term_get_wh(int* w, int* h);
void term_set_cursor(bool on);
void term_set_echo(bool state);
void term_set_canon(bool state);
void term_set_sigint_callback(void(*sig)(int));

void term_clear(void);
void term_clear_area(int x, int y, int w, int h);
void term_refresh(void);
void term_getch(char* buf, int size);
void term_wait_on_input(void);
void term_move_cursor(int x, int y);

void term_set_attr(int x, int y, TextAttributeFlags ta_flags);
void term_unset_attr(int x, int y, TextAttributeFlags ta_flags);

void term_draw_symbol(int x, int y, struct Colour* fg, struct Colour* bg, TextAttributeFlags ta_flags, char symbol);
void term_draw_text(int x, int y, struct Colour* fg, struct Colour* bg, TextAttributeFlags ta_flags, const char* text);
void term_draw_ftext(int x, int y, struct Colour* fg, struct Colour* bg, TextAttributeFlags ta_flags, const char* format, ...);
void term_draw_fntext(int count, int x, int y, struct Colour* fg, struct Colour* bg, TextAttributeFlags ta_flags, const char* format, ...);
void term_draw_area(int x, int y, int w, int h, struct Colour* fg, struct Colour* bg, TextAttributeFlags ta_flags, char symbol);

#endif
