#include "ui/ui.h"

#include "game/camera.h"
#include "core/colour.h"
#include "game/globals.h"
#include "core/log.h"
#include "game/map.h"
#include "game/message.h"
#include "game/monster.h"
#include "game/mon_equip.h"
#include "game/mon_inventory.h"
#include "game/mon_stats.h"
#include "game/object.h"
#include "core/term.h"
#include "game/util.h"
#include "core/string.h"

#include <stdio.h>
#include <string.h>

#define STATUS_X 0
#define STATUS_Y 46
#define STATUS_W 80

void draw_textbox(int x, int y, int w, int h, struct Colour* fg, struct Colour* bg, const char* text)
{
    struct string text_str;
    string_init(&text_str, text);

    term_draw_area(x, y, w, h, fg, bg, 0, ' ');

    int stridx   = 0;
    int textlen = string_size(&text_str);

    // Print text
    while(stridx < textlen)
    {
        int chars = w;
        if((stridx + w) < textlen)
        {
            chars = string_rfindi(&text_str, ' ', stridx + w);

            if(chars == -1)
            {
                chars = w;
            }
            else
            {
                chars -= stridx;
            }
        }

        term_draw_fntext(chars, x, y++, fg, bg, 0, "%s", text + stridx);
        stridx += chars;
    }
}

void draw_textbox_border(int x, int y, int w, int h, struct Colour* fg, struct Colour* bg, const char* text)
{
    int _w = w + 4;
    int _h = h + 4;

    term_draw_area(x, y, _w, _h, fg, bg, 0, ' ');

    for(int _x = x + 1; _x < (x+_w-1); ++_x)
    {
        term_draw_symbol(_x,        y, NULL, COL(CLR_DGREY), A_BOLD_BIT, '=');
        term_draw_symbol(_x, (y+_h-1), NULL, COL(CLR_DGREY), A_BOLD_BIT, '=');
    }

    for(int _y = y + 1; _y < (y+_h-1); ++_y)
    {
        term_draw_symbol(x,        _y, NULL, COL(CLR_DGREY), A_BOLD_BIT, '|');
        term_draw_symbol((x+_w-1), _y, NULL, COL(CLR_DGREY), A_BOLD_BIT, '|');
    }

    term_draw_symbol(x,           y, NULL, COL(CLR_DGREY), A_BOLD_BIT, '+');
    term_draw_symbol(x+_w-1, y+_h-1, NULL, COL(CLR_DGREY), A_BOLD_BIT, '+');
    term_draw_symbol(x+_w-1,      y, NULL, COL(CLR_DGREY), A_BOLD_BIT, '+');
    term_draw_symbol(x,      y+_h-1, NULL, COL(CLR_DGREY), A_BOLD_BIT, '+');

    draw_textbox(x+2, y+2, _w-3, _h-3, fg, bg, text);
}

bool prompt_yn(const char* msg)
{
    clear_msgs();
    display_fmsg_nolog("%s [yn] (n)", msg);
    flush_msg_buffer();
    term_refresh();

    bool decision = (enum YesNoCommand)get_key() == YES;

    display_fmsg_log("%s [yn] (n) %c", msg, decision ? YES : NO);
    flush_msg_buffer();
    term_refresh();
    return decision;
}

char prompt_choice(const char* title, char** choices, int length)
{
    char last_option_id = KEYCODE_a + length - 1;

    int x = (screen_cols / 2) - (g_option_name_max_size / 2);
    int y = (screen_rows / 2) - (length / 2);

    term_draw_ftext(x, y++, NULL, NULL, A_BOLD_BIT, "%s", title);

    for(int option_id = 0; option_id < length; ++option_id)
    {
        term_draw_ftext(x, y++, NULL, NULL, 0, "%c - %s", (KEYCODE_a + option_id), choices[option_id]);
    }

    term_refresh();

    char choice;
    do
    {
        choice = get_key();
    }
    while(choice != KEYCODE_ESC && (choice < KEYCODE_a || choice > last_option_id));

    return choice;
}

