#ifndef BYZ_UI_H
#define BYZ_UI_H

#include "core/input_keycodes.h"
#include "core/list.h"

#include <stdbool.h>

#define g_option_name_max_size 32

struct Colour;
struct UIMap;

enum YesNoCommand
{
    YES = KEYCODE_y,
    NO  = KEYCODE_n
};

struct UI
{
    struct UIMap* ui_map;
};

void draw_textbox(int x, int y, int w, int h, struct Colour* fg, struct Colour* bg, const char* text);
void draw_textbox_border(int x, int y, int w, int h, struct Colour* fg, struct Colour* bg, const char* text);

bool prompt_yn(const char* msg);
char prompt_choice(const char* title, char** choices, int length);

void display_main_screen(void);
void display_char_status(void);
void display_char_info_screen(void);

extern struct UI* g_ui;

#endif
