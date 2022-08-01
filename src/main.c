#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

#include "core/cache.h"
#include "core/colour.h"
#include "core/draw.h"
#include "core/input_keycodes.h"
#include "core/list.h"
#include "core/log.h"
#include "core/term.h"
#include "core/string.h"
#include "game/camera.h"
#include "game/gameplay_commands.h"
#include "game/globals.h"
#include "game/init.h"
#include "game/map.h"
#include "game/map_cell.h"
#include "game/map_location.h"
#include "game/map_gen.h"
#include "game/pathing.h"
#include "game/pathing_eval.h"
#include "game/pathing_node.h"

#include "ui/ui.h"
#include "ui/embark_screen.h"

enum GameState
{
    GAME_STATE_EMBARK,
    GAME_STATE_PLAY
};

struct anonymous
{
    enum GameState game_state;
    struct EmbarkScreen* embark_screen;
} anon;

//struct MAP_SCREEN* screen;

//void handle_cursor_move(int x_off, int y_off)
//{
//    if(state == GAME_STATE_EMBARK)
//    {
//
//    }
//    else
//    {
//        int world_x;
//        int world_y;
//
//        camera_screen_to_world(g_camera, curs.x, curs.y, &world_x, &world_y);
//
//        if(camera_in_bounds(g_camera, world_x + x_off, world_y + y_off))
//        {
//            // In camera bounds
//            curs.x += x_off;
//            curs.y += y_off;
//        }
//        else
//        {
//            // Not in camera bounds, check to see if the camera can move
//            if(map_in_bounds(g_cmap, g_camera->x + x_off, g_camera->y + y_off) &&
//               map_in_bounds(g_cmap, camera_max_x(g_camera) + x_off, camera_max_y(g_camera) + y_off))
//            {
//                g_camera->x += x_off;
//                g_camera->y += y_off;
//            }
//        }
//    }
//}

static bool handle_input()
{
    const bool QUIT = true;
    const bool NO_QUIT = false;

    enum KeyCode input = get_key();

    if(anon.game_state == GAME_STATE_EMBARK)
    {
        if(embark_screen_handle(anon.embark_screen, input))
        {
            return NO_QUIT;
        }
    }

    switch(input)
    {
        case GAMEPLAY_COMMAND_SAVE_AND_QUIT:
            return QUIT;

        default:
            break;
    }

    return NO_QUIT;
}

static void handle_draw()
{
    if(anon.game_state == GAME_STATE_EMBARK)
    {
        embark_screen_draw(anon.embark_screen);
    }
}

//void pathfind(void)
//{
//    int cursor_world_x;
//    int cursor_world_y;
//
//    camera_screen_to_world(g_camera, curs.x, curs.y, &cursor_world_x, &cursor_world_y);
//
//    struct MapLocation* start = map_get_location(g_cmap, 0, 0);
//    struct MapLocation* end   = map_get_location(g_cmap, cursor_world_x, cursor_world_y);
//
//    if(!start || !end)
//    {
//        return;
//    }
//
//    struct List path;
//    list_init(&path);
//
//    pathing_find_path(start->pathing, end->pathing, PATHING_GROUND, manhatten_evaluation, &path);
//
//    struct ListNode* node = NULL;
//    list_for_each(&path, node)
//    {
//        struct CONNECTIVITY_NODE* conn = node->data;
//        int draw_x;
//        int draw_y;
//        if(camera_world_to_screen(g_camera, conn->x, conn->y, &draw_x, &draw_y))
//        {
//            term_draw_symbol(draw_x, draw_y, COL(CLR_WHITE), COL(CLR_RED), A_NONE, ' ');
//        }
//    }
//
//    list_uninit(&path);
//}

int run(void)
{
    g_camera = (struct camera*)malloc(sizeof(struct camera));
    g_camera->x = 0;
    g_camera->y = 0;
    term_get_wh(&g_camera->w, &g_camera->h);

    bool quit = false;
    int screen_width;
    int screen_height;

    const int world_map_width = 100;
    const int world_map_height = 100;
    const int world_map_seed = 7;/*time(NULL)*/

    g_cmap = map_new(world_map_width, world_map_height, world_map_seed);
    for(int x = 0; x < world_map_width; ++x)
    for(int y = 0; y < world_map_height; ++y)
    {
        list_add(&g_cmap->cell_list, map_cell_new(x, y, 0 ));
    }
    gen_map(g_cmap);
    gen_map_cell(g_cmap, map_get_cell_by_cell_coord(g_cmap, 0, 0));

    anon.embark_screen = embark_screen_new(g_cmap);

    do
    {
        ++current_turn_number;
        term_get_wh(&screen_width, &screen_height);

        draw_begin();
        handle_draw();
        //pathfind();

        //int cursor_world_x;
        //int cursor_world_y;
        //camera_screen_to_world(g_camera, curs.x, curs.y, &cursor_world_x, &cursor_world_y);
        //term_set_attr(curs.x, curs.y, A_BLINK_BIT | A_REVERSE_BIT);
        //term_draw_ftext(0, screen_height-3, COL(CLR_WHITE), COL(CLR_DEFAULT), A_BOLD_BIT, "cursor world (x:%d, y:%d)", cursor_world_x, cursor_world_y);
        //term_draw_ftext(0, screen_height-2, COL(CLR_WHITE), COL(CLR_DEFAULT), A_BOLD_BIT, "cursor screen (x:%d, y:%d)", curs.x, curs.y);
        //term_draw_ftext(0, screen_height-1, COL(CLR_WHITE), COL(CLR_DEFAULT), A_BOLD_BIT, "camera (x:%d, y:%d, w:%d, h:%d)", g_camera->x, g_camera->y, g_camera->w, g_camera->h);
        term_refresh();
        //term_unset_attr(curs.x, curs.y, A_BLINK_BIT | A_REVERSE_BIT);

        quit = handle_input();
    }
    while(!quit);

    embark_screen_free(anon.embark_screen);
    map_free(g_cmap);
    free(g_camera);

    uninit_main();

    return 0;
}

bool comp(void* lhs, void* rhs)
{
    int* l = lhs;
    int* r = rhs;

    return *l < *r;
}

void test_list_sort(void)
{
    struct List list;
    list_init(&list);

    int array[11];

    for(int i = 0; i < 11; ++i)
    {
        array[i] = rand() % 10;
        printf("%d ", array[i]);

        list_add(&list, &array[i]);
    }

    printf("\n");

    list_sort(&list, comp);

    struct ListNode* n = NULL;
    list_for_each(&list, n)
    {
        printf("%d ", *((int*)n->data));
    }

    printf("\n");

    list_uninit(&list);
}

void test_list_add_head(void)
{
    struct List list;
    list_init(&list);

    int array[11];

    printf("IN:\n");
    for(int i = 0; i < 11; ++i)
    {
        array[i] = i;
        printf("%d ", array[i]);

        list_add_head(&list, &array[i]);
    }

    printf("\n");

    printf("OUT:\n");
    struct ListNode* n = NULL;
    list_for_each(&list, n)
    {
        printf("%d ", *((int*)n->data));
    }

    printf("\n");

    list_uninit(&list);
}

void test_list_insert_before(void)
{
    struct List list;
    list_init(&list);

    int array[5];

    printf("IN:\n");
    for(int i = 0; i < 5; ++i)
    {
        array[i] = i;
        list_add(&list, &array[i]);
        printf("%d ", array[i]);

    }
    printf("\n");

    struct ListNode* insert_before = list_find(&list, &array[0]);

    int data = 77;
    list_insert_before(&list, &data, insert_before);

    printf("IN:\n");
    list_for_each(&list, insert_before)
    {
        printf("%d ", *((int*)insert_before->data));
    }
    printf("\n");

    list_uninit(&list);
}

//#include "core/noise.h"
//void test_perlin(void)
//{
//    for(float i = 0.0f; i < 10.0f; i += 0.01f)
//    for(float j = 0.0f; j < 10.0f; j += 0.01f)
//    {
//        log_format_msg(LOG_DEBUG, "%f", perlin(i, j));
//    }
//}

#include "core/rng.h"
void test_rng(void)
{
    struct RNG* rng = rng_new(7777);

    for(int i = 0; i < 10000; ++i)
    {
        log_format_msg(LOG_DEBUG, "%d", rng_get(rng) % 500);
    }

    rng_free(rng);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    if(!init_main())
    {
        printf("Initialisation failed. Check debug log.\n");
        return -1;
    }

    //test_list_sort();
    //test_list_add_head();
    //test_list_insert_before();
    //test_perlin();
    //test_rng();

    run();

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
