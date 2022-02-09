#include "core/string.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

//// FDECL
//void new_game(void);
//void main_loop(void);
//
//// IMPL
//void new_game(void)
//{
//    current_turn_number = 0;
//}

//void main_loop(void)
//{
//    // Basic implementation for debugging
//
//    display_main_screen();
//    term_refresh();
//
//    do
//    {
//        current_turn_number++;
//
//        // Reset path generation info
//        current_path_gen_id = -1;
//
//        gameplay_turn();
//        update_mons();
//
//        clear_msgs();
//        flush_msg_buffer();
//        display_main_screen();
//        term_refresh();
//    }
//    while(true);
//}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    struct string* s = string_new("Hello world!\n");

    string_printf(s);
    printf("Hello world!\n");


    return 0;
}

//int main(int argc, char** argv)
//{
//    (void)argc;
//    (void)argv;
//
//    if(!init_main())
//    {
//        printf("Initialisation failed. Check debug log.");
//        return -1;
//    }
//
//#ifdef DEBUG
//    log_msg(LOG_DEBUG, "program initialised with params:");
//    for(int i = 0; i < argc; ++i)
//    {
//        log_format_msg(LOG_DEBUG, "\t%s", argv[i]);
//    }
//#endif
//
//    // some intro text
//    term_draw_text((screen_cols/2) - 7, screen_rows/2, NULL, NULL, 0, "Welcome to Yun");
//    term_refresh();
//    term_wait_on_input();
//
//    new_player();
//    new_game();
//
//    int map_width = 3;
//    int map_height = 3;
//    g_cmap = map_new(map_width, map_height);
//    gen_map(g_cmap, MAPTYPE_DUNGEON);
//
//    int map_ui_size_x = 0;
//    int map_ui_size_y = 0;
//    int map_ui_size_w = 80;
//    int map_ui_size_h = 40;
//    g_ui->ui_map = ui_map_new(map_ui_size_x, map_ui_size_y, map_ui_size_w, map_ui_size_h, g_cmap);
//
//    mon_set_stat(g_you->mon, STAT_TYPE_HP_MAX, 999);
//    mon_set_stat(g_you->mon, STAT_TYPE_HP, 999);
//
//    main_loop();
//
//    do_quit();
//}
