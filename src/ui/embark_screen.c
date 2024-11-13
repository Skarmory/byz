#include "ui/embark_screen.h"

#include "core/cursor.h"
#include "core/geom.h"
#include "core/math_utils.h"
#include "core/tasking.h"
#include "core/term.h"

#include "game/map.h"
#include "game/map_cell.h"
#include "game/map_gen.h"
#include "game/map_location.h"
#include "game/pathing_node.h"
#include "game/terrain.h"

#include <stdio.h>

enum EmbarkCommand
{
    CURSOR_LEFT  = KEYCODE_h,
    CURSOR_RIGHT = KEYCODE_l,
    CURSOR_DOWN  = KEYCODE_j,
    CURSOR_UP    = KEYCODE_k,
    FOCUS_LEFT   = KEYCODE_H,
    FOCUS_RIGHT  = KEYCODE_L,
    INVALID
};

struct EmbarkScreenGroup
{
    struct Container region;
    struct Cursor cursor;
    struct camera* camera;
};

struct EmbarkScreen
{
    struct Map* map;

    struct EmbarkScreenGroup world_group;
    struct EmbarkScreenGroup regional_group;

    struct Container info_container;

    bool focus_regional;
    enum MapLayer layer;
};

static void _debug_draw_group(struct EmbarkScreenGroup* group)
{
    const int text_start_x = group->region.x;
    const int text_start_y = group->region.y + group->region.h - 1;
    const int max_width = group->region.w - 1;

    char buffer[256];
    snprintf(buffer, max_width, "Cursor (global): (%d, %d)", group->cursor.x, group->cursor.y);
    term_draw_text(text_start_x, text_start_y - 2, COL(CLR_WHITE), COL(CLR_DEFAULT), A_BOLD_BIT, buffer);

    int local_x = -1;
    int local_y = -1;
    math_change_basis(group->cursor.x, group->cursor.y, group->region.x, group->region.y, 0, 0, &local_x, &local_y);
    snprintf(buffer, max_width, "Cursor (local):  (%d, %d)", local_x, local_y);
    term_draw_text(text_start_x, text_start_y - 1, COL(CLR_WHITE), COL(CLR_DEFAULT), A_BOLD_BIT, buffer);

    int world_x = -1;
    int world_y = -1;
    camera_relative_to_world(group->camera, local_x, local_y, &world_x, &world_y);
    snprintf(buffer, max_width, "Cursor (world):  (%d, %d)", world_x, world_y);
    term_draw_text(text_start_x, text_start_y, COL(CLR_WHITE), COL(CLR_DEFAULT), A_BOLD_BIT, buffer);
}

static void _debug_draw(struct EmbarkScreen* embark_screen)
{
    _debug_draw_group(&embark_screen->world_group);
    _debug_draw_group(&embark_screen->regional_group);
}

static struct EmbarkScreenGroup* _get_focussed_group(struct EmbarkScreen* embark_screen)
{
    if(embark_screen->focus_regional)
    {
        return &embark_screen->regional_group;
    }
    else
    {
        return &embark_screen->world_group;
    }
}

static void _handle_cursor_move(struct EmbarkScreen* es, enum EmbarkCommand cmd)
{
    struct EmbarkScreenGroup* group = _get_focussed_group(es);

    int x_off = 0;
    int y_off = 0;
    cursor_get_offset((enum KeyCode)cmd, &x_off, &y_off);

    // Cursor cannot move because it will go out of region bounds.
    const bool cursor_no_move = !geom_point_in_rect(group->cursor.x + x_off, group->cursor.y + y_off, group->region.x, group->region.y, group->region.w, group->region.h);

    //int world_x = -1;
    //int world_y = -1;
    //_group_cursor_to_world(group, &world_x, &world_y);

    if(!cursor_no_move/* && camera_in_bounds(&group->camera, world_x + x_off, world_y + y_off)*/)
    {
        // In camera bounds
        group->cursor.x += x_off;
        group->cursor.y += y_off;
    }
}

static void _handle_focus_switch(struct EmbarkScreen* embark_screen, enum EmbarkCommand cmd)
{
    (void)cmd;
    embark_screen->focus_regional = !embark_screen->focus_regional;
}

static void _draw_container(struct Container* container)
{
    term_draw_area(
            container->x,
            container->y,
            container->w,
            container->h,
            COL(CLR_DEFAULT),
            COL(CLR_FOG_OF_WAR),
            A_NONE_BIT,
            ' ');
}

static void _draw_location(int screen_i, int screen_j, struct MapLocation* map_location, enum MapLayer layer)
{
    if(layer == MAP_LAYER_PATHING)
    {
        if(map_location)
        {
            if(map_location->pathing->pathing_data.state == PATHING_NODE_STATE_OPEN)
            {
                term_draw_symbol(screen_i, screen_j, COL(CLR_GREEN), COL(CLR_DEFAULT), 0, 'O');
            }
            else if(map_location->pathing->pathing_data.state == PATHING_NODE_STATE_CLOSED)
            {
                term_draw_symbol(screen_i, screen_j, COL(CLR_RED), COL(CLR_DEFAULT), 0, 'X');
            }
            else
            {
                term_draw_symbol(screen_i, screen_j, COL(CLR_DGREY), COL(CLR_DEFAULT), 0, '#');
            }
        }
        else
        {
            term_draw_symbol(screen_i, screen_j, COL(CLR_FOG_OF_WAR), COL(CLR_DEFAULT), 0, ' ');
        }
    }
    else if(layer == MAP_LAYER_ELEVATION)
    {
        if(map_location)
        {
            int col = clamp((int)(255.f * map_location->terrain.elevation), 0, 255);
            struct Colour c = { col, col, col };
            term_draw_symbol(screen_i, screen_j, COL(CLR_DEFAULT), &c, 0, ' ');
        }
        else
        {
            term_draw_symbol(screen_i, screen_j, COL(CLR_FOG_OF_WAR), COL(CLR_DEFAULT), 0, ' ');
        }
    }
    else // (layer == MAP_LAYER_NORMAL)
    {
        if(map_location)
        {
            term_draw_symbol(screen_i, screen_j, &map_location->symbol.fg, &map_location->symbol.bg, 0, map_location->symbol.sym);
        }
        else
        {
            term_draw_symbol(screen_i, screen_j, COL(CLR_DEFAULT), COL(CLR_FOG_OF_WAR), 0, ' ');
        }
    }
}

static void _draw_container_title(const char* title, int title_len, struct Container* container)
{
    term_draw_text(container->x + (container->w / 2) - (title_len / 2), 0, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, title);
}

void embark_screen_draw_world_map(struct EmbarkScreen* es, struct Map* map, struct camera* camera)
{
    int world_x = 0;
    int world_y = 0;

    struct Container* container = &es->world_group.region;
    //const char* title = "World Map";
    //const int title_len_half = 5;
    //term_draw_text((group->region.w / 2) - title_len_half, 0, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, title);

    _draw_container_title("World Map", 10, container);

    for(int screen_x = container->x, rel_x = 0; screen_x < container->x + container->w; ++screen_x, ++rel_x)
    for(int screen_y = container->y, rel_y = 0; screen_y < container->y + container->h; ++screen_y, ++rel_y)
    {
        if(camera_relative_to_world(camera, rel_x, rel_y, &world_x, &world_y))
        {
            struct MapCell* cell = map_get_cell_by_cell_coord(map, world_x, world_y);

            if(cell)
            {
                term_draw_symbol(screen_x, screen_y, &cell->symbol.fg, &cell->symbol.bg, 0, cell->symbol.sym);
            }
            else
            {
                term_draw_symbol(screen_x, screen_y, COL(CLR_DEFAULT), COL(CLR_FOG_OF_WAR), 0, ' ');
            }
        }
    }

    // Draw cursor
    //_group_cursor_to_world(group, &world_x, &world_y);
    //struct MapCell* cell = map_get_cell_by_cell_coord(map, world_x, world_y);
    term_draw_symbol(es->world_group.cursor.x, es->world_group.cursor.y, COL(CLR_BLACK), NULL, A_BLINK_BIT | A_BOLD_BIT, '@' );
}

void embark_screen_draw_regional_map(struct EmbarkScreen* es, struct MapCell* region, struct camera* camera)
{
    int world_x = 0;
    int world_y = 0;
    struct MapLocation* map_location = NULL;
    struct Container* container = &es->regional_group.region;

    //const char* title = "Regional Map";
    //const int title_len_half = 6;
    //term_draw_text(es->regional_group.region.x + (es->regional_group.w / 2) - title_len_half, 0, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, title);
    _draw_container_title("Regional Map", 12, container);

    for(int screen_x = container->x, rel_x = 0; screen_x < container->x + container->w; ++screen_x, ++rel_x)
    for(int screen_y = container->y, rel_y = 0; screen_y < container->y + container->h; ++screen_y, ++rel_y)
    {
        if(camera_relative_to_world(camera, rel_x, rel_y, &world_x, &world_y))
        {
            map_location = map_cell_get_location_relative(region, world_x, world_y);
            _draw_location(screen_x, screen_y, map_location, es->layer); 
        }
    }

    // Draw cursor
    //_group_cursor_to_world(group, &world_x, &world_y);
    //map_location = map_cell_get_location_relative(cell, world_x, world_y);
    term_draw_symbol(es->regional_group.cursor.x, es->regional_group.cursor.y, COL(CLR_BLACK), NULL, A_BLINK_BIT | A_BOLD_BIT, '@' );
}

void embark_screen_draw_info_view(struct EmbarkScreen* es, void* info_data)
{
    const char* title = "Location Info";
    const int title_len_half = 7;
    term_draw_text(es->info_container.x + (es->info_container.w / 2) - title_len_half, 0, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, title);

    _draw_container(&es->info_container);

    if(es->focus_regional)
    {
        struct MapLocation* loc = info_data;//_current_regional_map_loc(es);

        term_draw_ftext(es->info_container.x, 2, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, "Coordinates: %d, %d", loc->x, loc->y);
        term_draw_ftext(es->info_container.x, 3, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, "Biome: %s", biome_name_from_enum(loc->terrain.biome));
        term_draw_ftext(es->info_container.x, 4, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, "Elevation: %f", loc->terrain.elevation);
        term_draw_ftext(es->info_container.x, 5, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, "Precipitation: %f", loc->terrain.precipitation);
    }
    else
    {
        struct MapCell* cell = info_data;//_current_world_map_cell(es);

        term_draw_ftext(es->info_container.x, 2, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, "Coordinates: %d, %d", cell->cell_x, cell->cell_y);
    }
}

struct EmbarkScreen* embark_screen_new(void)
{
    int screen_width = 0;
    int screen_height = 0;
    term_get_wh(&screen_width, &screen_height);

    struct EmbarkScreen* es = malloc(sizeof(struct EmbarkScreen));

    int container_space = screen_width;

    // Split the screen into regions to display to display camera views in
    es->world_group.region.x = 0;
    es->world_group.region.y = 1;
    es->world_group.region.w = screen_width / 3;
    es->world_group.region.h = screen_height - 1;

    container_space -= es->world_group.region.w + 1;

    es->regional_group.region.x = screen_width / 3 + 1; // Add column of padding
    es->regional_group.region.y = 1;
    es->regional_group.region.w = screen_width / 3 - 1; // Account for column of padding
    es->regional_group.region.h = screen_height - 1;

    container_space -= es->regional_group.region.w + 1;

    es->info_container.x = screen_width - container_space;
    es->info_container.y = 1;
    es->info_container.w = container_space;
    es->info_container.h = screen_height - 1;

    // Set cursors to region origins
    es->world_group.cursor.x  = es->world_group.region.x;
    es->world_group.cursor.y  = es->world_group.region.y;
    es->regional_group.cursor.x = es->regional_group.region.x;
    es->regional_group.cursor.y = es->regional_group.region.y;

    // Start off with the cursor focus on the world map
    es->focus_regional = false;
    es->layer = MAP_LAYER_NORMAL;

    _draw_container(&es->world_group.region);
    _draw_container(&es->regional_group.region);
    _draw_container(&es->info_container);

    return es;
}

void embark_screen_free(struct EmbarkScreen* es)
{
    free(es);
}

bool embark_screen_world_map_focussed(struct EmbarkScreen* es)
{
    return !es->focus_regional;
}

bool embark_screen_regional_map_focussed(struct EmbarkScreen* es)
{
    return es->focus_regional;
}

struct Cursor embark_screen_get_cursor(struct EmbarkScreen* es)
{
    struct EmbarkScreenGroup* group = _get_focussed_group(es);
    return group->cursor;
}

struct Cursor embark_screen_get_world_map_container_cursor(struct EmbarkScreen* es)
{
    return es->world_group.cursor;
}

struct Cursor embark_screen_get_regional_map_container_cursor(struct EmbarkScreen* es)
{
    return es->regional_group.cursor;
}

struct Container embark_screen_get_container(struct EmbarkScreen* es)
{
    struct EmbarkScreenGroup* group = _get_focussed_group(es);
    return group->region;
}

struct Container embark_screen_get_world_map_container(struct EmbarkScreen* es)
{
    return es->world_group.region;
}

struct Container embark_screen_get_regional_map_container(struct EmbarkScreen* es)
{
    return es->regional_group.region;
}

bool embark_screen_update(struct EmbarkScreen* embark_screen, enum KeyCode input)
{
    enum EmbarkCommand cmd = (enum EmbarkCommand)input;
    switch(cmd)
    {
        case CURSOR_LEFT:
        case CURSOR_RIGHT:
        case CURSOR_DOWN:
        case CURSOR_UP:
        {
            _handle_cursor_move(embark_screen, cmd);
            return true;
        }

        case FOCUS_LEFT:
        case FOCUS_RIGHT:
        {
            _handle_focus_switch(embark_screen, cmd);
            return true;
        }

        default:
            return false;
    }
}

//void embark_screen_draw(struct EmbarkScreen* es, struct Map* map)
//{
//    _draw_world_view_map(&es->world_group, es->map, es->layer);
//    _draw_regional_view_map(&es->regional_group, es->layer);
//    _draw_info_view(es);
//    _debug_draw(es);
//}
