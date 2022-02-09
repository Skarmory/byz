#include "ui/ui_stats.h"

#include "game/globals.h"
#include "core/input_keycodes.h"
#include "game/monster.h"
#include "game/mon_stats.h"
#include "core/term.h"

#include <stddef.h>

enum CharacterScreenCommand
{
    CHARACTER_SCREEN_COMMAND_QUIT = KEYCODE_q
};

//void _display_stats(void)
//{
//    term_draw_ftext(2, 0, NULL, NULL, 0, "Name: %s", g_you->name);
//    term_draw_ftext(1, 1, NULL, NULL, 0, "Class: %s", g_you->cls->name);
//
//    term_draw_ftext(1, screen_rows-1, NULL, NULL, 0, "%c = close character screen", CHARACTER_SCREEN_COMMAND_QUIT);
//}
//
//bool character_screen_handler(void)
//{
//    enum CharacterScreenCommand in;
//    do
//    {
//        term_clear();
//        _display_stats();
//        term_refresh();
//        in = (enum CharacterScreenCommand)get_key();
//    }
//    while(in != CHARACTER_SCREEN_COMMAND_QUIT);
//
//    term_clear();
//
//    return false;
//}
