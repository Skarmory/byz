#ifndef BYZ_UI_H
#define BYZ_UI_H

#include "core/input_keycodes.h"
#include "core/list.h"

#include <stdbool.h>

#define g_option_name_max_size 32

struct Colour;

enum YesNoCommand
{
    YES = KEYCODE_y,
    NO  = KEYCODE_n
};

void draw_textbox(int x, int y, int w, int h, struct Colour* fg, struct Colour* bg, const char* text);
void draw_textbox_border(int x, int y, int w, int h, struct Colour* fg, struct Colour* bg, const char* text);

bool prompt_yn(const char* msg);
char prompt_choice(const char* title, char** choices, int length);

#endif
