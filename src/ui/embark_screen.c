#include "ui/embark_screen.h"

#include "core/geom.h"
#include "core/math_utils.h"
#include "core/term.h"

#include "game/camera.h"
#include "game/map.h"
#include "game/map_cell.h"
#include "game/map_gen.h"
#include "game/map_location.h"
#include "game/pathing_node.h"
#include "game/terrain.h"

#include <stdio.h>

enum Command
{
    CURSOR_LEFT = KEYCODE_h,
    CURSOR_RIGHT = KEYCODE_l,
    CURSOR_DOWN = KEYCODE_j,
    CURSOR_UP = KEYCODE_k,
    FOCUS_LEFT = KEYCODE_H,
    FOCUS_RIGHT = KEYCODE_L,
    INVALID
};

struct Container
{
    int x;
    int y;
    int w;
    int h;
};

struct EmbarkScreenGroup
{
    struct Container region;
    struct Cursor cursor;
    struct camera camera;
};

struct EmbarkScreen
{
    struct Map* map;

    struct EmbarkScreenGroup world_group;
    struct EmbarkScreenGroup regional_group;

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
    camera_relative_to_world(&group->camera, local_x, local_y, &world_x, &world_y);
    snprintf(buffer, max_width, "Cursor (world):  (%d, %d)", world_x, world_y);
    term_draw_text(text_start_x, text_start_y, COL(CLR_WHITE), COL(CLR_DEFAULT), A_BOLD_BIT, buffer);
}

static void _debug_draw(struct EmbarkScreen* embark_screen)
{
    _debug_draw_group(&embark_screen->world_group);
    _debug_draw_group(&embark_screen->regional_group);
}

static void _cursor_move_get_offset(enum Command cmd, int* restrict xoff, int* restrict yoff)
{
    *xoff = 0;
    *yoff = 0;

    switch(cmd)
    {
        case CURSOR_LEFT:
        {
            *xoff = -1;
            break;
        }

        case CURSOR_RIGHT:
        {
            *xoff = 1;
            break;
        }

        case CURSOR_DOWN:
        {
            *yoff = 1;
            break;
        }

        case CURSOR_UP:
        {
            *yoff = -1;
            break;
        }

        default: break;
    }
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

static void _group_cursor_to_world(struct EmbarkScreenGroup* group, int* out_wx, int* out_wy)
{
    // Convert cursor screen-space to camera-space
    math_change_basis(group->cursor.x, group->cursor.y, group->region.x, group->region.y, 0, 0, out_wx, out_wy);

    // Convert camera-space to world-space
    camera_relative_to_world(&group->camera, *out_wx, *out_wy, out_wx, out_wy);
}

static void _handle_cursor_move(struct EmbarkScreen* embark_screen, enum Command cmd)
{
    struct EmbarkScreenGroup* group = _get_focussed_group(embark_screen);

    int x_off = 0;
    int y_off = 0;
    _cursor_move_get_offset(cmd, &x_off, &y_off);

    // Cursor cannot move because it will go out of region bounds.
    //const bool cursor_no_move = _does_region_constrain_cursor(group, x_off, y_off);
    const bool cursor_no_move = !geom_point_in_rect(group->cursor.x + x_off, group->cursor.y + y_off, group->region.x, group->region.y, group->region.w, group->region.h);

    int world_x = -1;
    int world_y = -1;
    _group_cursor_to_world(group, &world_x, &world_y);

    if(!cursor_no_move && camera_in_bounds(&group->camera, world_x + x_off, world_y + y_off))
    {
        // In camera bounds
        group->cursor.x += x_off;
        group->cursor.y += y_off;
    }
    else
    {
        // Not in camera bounds, check to see if the camera can move
        bool camera_can_move = true;
        if(!embark_screen->focus_regional)
        {
            camera_can_move = map_in_bounds_cell(embark_screen->map, group->camera.x + x_off, group->camera.y + y_off) &&
                              map_in_bounds_cell(embark_screen->map, camera_max_x(&group->camera) + x_off, camera_max_y(&group->camera) + y_off);

        }
        else
        {
            camera_can_move = map_in_bounds(embark_screen->map, group->camera.x + x_off, group->camera.y + y_off) &&
                              map_in_bounds(embark_screen->map, camera_max_x(&group->camera) + x_off, camera_max_y(&group->camera) + y_off);
        }

        if(camera_can_move)
        {
            group->camera.x += x_off;
            group->camera.y += y_off;
        }
    }

    if(!embark_screen->focus_regional)
    {
        int wx = -1;
        int wy = -1;
        _group_cursor_to_world(&embark_screen->world_group, &wx, &wy);

        struct MapCell* cell = map_get_cell_by_cell_coord(embark_screen->map, wx, wy);
        if(cell && cell->locs == NULL)
        {
            gen_map_cell(embark_screen->map, cell);
        }
    }
}

static void _handle_focus_switch(struct EmbarkScreen* embark_screen, enum Command cmd)
{
    embark_screen->focus_regional = !embark_screen->focus_regional;
}

struct EmbarkScreen* embark_screen_new(struct Map* world_map)
{
    int screen_width = 0;
    int screen_height = 0;
    term_get_wh(&screen_width, &screen_height);

    struct EmbarkScreen* embark_screen = malloc(sizeof(struct EmbarkScreen));

    // Split the screen into regions to display to display camera views in
    embark_screen->world_group.region.x = 0;
    embark_screen->world_group.region.y = 1;
    embark_screen->world_group.region.w = screen_width / 2;
    embark_screen->world_group.region.h = screen_height - 1;

    embark_screen->regional_group.region.x = screen_width / 2 + 1; // Add column of padding
    embark_screen->regional_group.region.y = 1;
    embark_screen->regional_group.region.w = screen_width / 2 - 1; // Account for column of padding
    embark_screen->regional_group.region.h = screen_height - 1;

    // Create cameras to display world and regional map views
    embark_screen->world_group.camera.x = 0;
    embark_screen->world_group.camera.y = 0;
    embark_screen->world_group.camera.w = minu(embark_screen->world_group.region.w, world_map->width);
    embark_screen->world_group.camera.h = minu(embark_screen->world_group.region.h, world_map->height);

    embark_screen->regional_group.camera.x = 0;
    embark_screen->regional_group.camera.y = 0;
    embark_screen->regional_group.camera.w = minu(embark_screen->regional_group.region.w, g_map_cell_width);
    embark_screen->regional_group.camera.h = minu(embark_screen->regional_group.region.h, g_map_cell_height);

    // Set cursors to region origins
    embark_screen->world_group.cursor.x  = embark_screen->world_group.region.x;
    embark_screen->world_group.cursor.y  = embark_screen->world_group.region.y;
    embark_screen->regional_group.cursor.x = embark_screen->regional_group.region.x;
    embark_screen->regional_group.cursor.y = embark_screen->regional_group.region.y;

    // Start off with the cursor focus on the world map
    embark_screen->focus_regional = false;
    embark_screen->layer = MAP_LAYER_NORMAL;

    embark_screen->map = world_map;

    term_draw_area(
            embark_screen->world_group.region.x,
            embark_screen->world_group.region.y,
            embark_screen->world_group.region.w,
            embark_screen->world_group.region.h,
            COL(CLR_DEFAULT),
            COL(CLR_FOG_OF_WAR),
            A_NONE_BIT,
            ' ');

    term_draw_area(
            embark_screen->regional_group.region.x,
            embark_screen->regional_group.region.y,
            embark_screen->regional_group.region.w,
            embark_screen->regional_group.region.h,
            COL(CLR_DEFAULT),
            COL(CLR_FOG_OF_WAR),
            A_NONE_BIT,
            ' ');

    return embark_screen;
}

void embark_screen_free(struct EmbarkScreen* embark_screen)
{
    free(embark_screen);
}

bool embark_screen_handle(struct EmbarkScreen* embark_screen, enum KeyCode input)
{
    enum Command cmd = (enum Command)input;
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
            int col = clamp((int)(255.f * map_location->terrain->elevation), 0, 255);
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

static void _draw_world_view_map(struct EmbarkScreenGroup* group, struct Map* map, enum MapLayer layer)
{
    int world_x = 0;
    int world_y = 0;

    const char* title = "World Map";
    const int title_len_half = 5;

    term_draw_text((group->region.w / 2) - title_len_half, 0, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, title);

    for(int screen_x = group->region.x, rel_x = 0; screen_x < group->region.x + group->region.w; ++screen_x, ++rel_x)
    for(int screen_y = group->region.y, rel_y = 0; screen_y < group->region.y + group->region.h; ++screen_y, ++rel_y)
    {
        if(camera_relative_to_world(&group->camera, rel_x, rel_y, &world_x, &world_y))
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
    _group_cursor_to_world(group, &world_x, &world_y);
    struct MapCell* cell = map_get_cell_by_cell_coord(map, world_x, world_y);
    term_draw_symbol(group->cursor.x, group->cursor.y, COL(CLR_BLACK), &cell->symbol.bg, A_BLINK_BIT | A_BOLD_BIT, '@' );
}

static void _draw_regional_view_map(struct EmbarkScreenGroup* group, struct MapCell* cell, enum MapLayer layer)
{
    int world_x = 0;
    int world_y = 0;
    struct MapLocation* map_location = NULL;

    const char* title = "Regional Map";
    const int title_len_half = 6;

    term_draw_text(group->region.x + (group->region.w / 2) - title_len_half, 0, COL(CLR_WHITE), COL(CLR_DEFAULT), A_NONE_BIT, title);

    for(int screen_x = group->region.x, rel_x = 0; screen_x < group->region.x + group->region.w; ++screen_x, ++rel_x)
    for(int screen_y = group->region.y, rel_y = 0; screen_y < group->region.y + group->region.h; ++screen_y, ++rel_y)
    {
        if(camera_relative_to_world(&group->camera, rel_x, rel_y, &world_x, &world_y))
        {
            map_location = map_cell_get_location_relative(cell, world_x, world_y);
            _draw_location(screen_x, screen_y, map_location, layer); 
        }
    }

    // Draw cursor
    _group_cursor_to_world(group, &world_x, &world_y);
    map_location = map_cell_get_location_relative(cell, world_x, world_y);
    term_draw_symbol(group->cursor.x, group->cursor.y, COL(CLR_BLACK), &map_location->symbol.bg, A_BLINK_BIT | A_BOLD_BIT, '@' );
}

void embark_screen_draw(struct EmbarkScreen* embark_screen)
{
    int wx = -1;
    int wy = -1;
    _group_cursor_to_world(&embark_screen->world_group, &wx, &wy);
    struct MapCell* current_cell = map_get_cell_by_cell_coord(embark_screen->map, wx, wy);

    _draw_world_view_map(&embark_screen->world_group, embark_screen->map, embark_screen->layer);
    _draw_regional_view_map(&embark_screen->regional_group, current_cell, embark_screen->layer);
    _debug_draw(embark_screen);
}
