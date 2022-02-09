#include "ui/ui_menu.h"

#include "core/colour.h"
#include "core/input_keycodes.h"
#include "core/log.h"
#include "game/monster.h"
#include "game/mon_stats.h"
#include "game/mon_type.h"
#include "ui/ui.h"
#include "game/util.h"

#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Print out the character creation choices made so far
 */
//void print_picked(void)
//{
//    int col;
//    term_draw_ftext(1, 2, NULL, NULL, 0, "   name: %s", !g_you->name ? "unknown" : g_you->name);
//
//    col = get_class_colour(g_you->cls);
//    term_draw_text(2, 2, NULL, NULL, 0, "  class: ");
//    term_draw_ftext(11, 2, COL(col), NULL, 0, "%s", !g_you->cls ? "not chosen" : g_you->cls->name);
//}

/**
 * Print character creation current choice to make
 */
//void print_options(int what, short mask)
//{
//    (void)mask;
//    int menu_col = 60, menu_row = 1;
//
//    switch(what)
//    {
//        case PICK_CLASS:
//        {
//            term_draw_text(menu_col, menu_row, NULL, NULL, 0, "Choose your class");
//            menu_row += 2;
//            for(int i = 0; i < g_classes_count; i++, menu_row++)
//            {
//                int col = get_class_colour(&g_classes[i]);
//                term_draw_ftext(menu_col, menu_row, col != CLR_DEFAULT ? COL(col) : NULL, NULL, 0, "%c - %s", g_classes[i].hotkey, g_classes[i].name);
//            }
//            break;
//        }
//
//        case CONFIRM:
//        {
//            term_draw_text(menu_col, menu_row, NULL, NULL, 0, "Confirm this character and start a new game?");
//            menu_row += 2;
//            term_draw_text(menu_col, menu_row++, NULL, NULL, 0, "y - Start game");
//            term_draw_text(menu_col, menu_row++, NULL, NULL, 0, "n - Choose again");
//            break;
//        }
//    }
//
//    menu_row++;
//    term_draw_text(menu_col, menu_row, NULL, NULL, 0, "q - quit");
//
//    term_refresh();
//}
