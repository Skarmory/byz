#include "game/map_cell.h"

#include "core/colour.h"
#include "game/feature.h"
#include "game/monster.h"
#include "game/object.h"
#include "game/pathing_node.h"
#include "game/pathing.h"
#include "game/terrain.h"

#include <stdlib.h>

static inline struct MapLocation* _get_map_location_internal(struct MapCell* cell, int x, int y)
{
    return &cell->locs[y * g_map_cell_width + x];
}

struct MapCell* map_cell_new(int cell_x, int cell_y, int seed)
{
    struct MapCell* cell = malloc(sizeof(struct MapCell));
    cell->cell_x = cell_x;
    cell->cell_y = cell_y;
    cell->world_x = cell_x * g_map_cell_width;
    cell->world_y = cell_y * g_map_cell_height;
    cell->seed = seed;
    list_init(&cell->room_list);
    list_init(&cell->mon_list);

    struct Feature* rock = feature_look_up_by_id("sroc");
    for(int i = 0; i < g_map_cell_width; ++i)
    for(int j = 0; j < g_map_cell_height; ++j)
    {
        struct MapLocation* loc = map_cell_get_location_relative(cell, i, j);
        loc->x = cell->world_x + i;
        loc->y = cell->world_y + j;
        loc->symbol.sym  = '.';
        loc->symbol.fg   = *COL(CLR_WHITE);
        loc->symbol.bg   = *COL(CLR_DEFAULT);
        loc->symbol.attr = 0;
        loc->mon = NULL;
        loc->feature = rock;
        loc->seen = false;
        list_init(&loc->obj_list);

        loc->pathing = malloc(sizeof(struct CONNECTIVITY_NODE));
        connectivity_node_init(loc->pathing, loc->x, loc->y, 0.0f, PATHING_GROUND);

        loc->terrain = malloc(sizeof(struct Terrain));
    }

    // Connect up connectivity nodes
    for(int i = 0; i < g_map_cell_width; ++i)
    for(int j = 0; j < g_map_cell_height; ++j)
    {
        struct MapLocation* loc = map_cell_get_location_relative(cell, i, j);
        struct CONNECTIVITY_NODE* node = loc->pathing;

        for(int x = i - 1; x < i + 2; ++x)
        for(int y = j - 1; y < j + 2; ++y)
        {
            struct MapLocation* connection_loc = map_cell_get_location_relative(cell, x, y);
            if(connection_loc)
            {
                connectivity_node_add_connection(node, connection_loc->pathing);
            }
        }
    }

    return cell;
}

void map_cell_free(struct MapCell* cell)
{
    struct ListNode *node = NULL, *n = NULL;

    // Free mons
    list_for_each_safe(&cell->mon_list, node, n)
    {
        mon_free(node->data);
        free(node);
    }

    // Free rooms
    list_for_each_safe(&cell->room_list, node, n)
    {
        free(node->data);
        free(node);
    }

    for(int idx = 0; idx < (g_map_cell_width * g_map_cell_height); ++idx)
    {
        list_for_each_safe(&cell->locs[idx].obj_list, node, n)
        {
            free_obj(node->data);
            free(node);
        }

        free(cell->locs[idx].pathing);
    }

    free(cell);
}

struct MapLocation* map_cell_get_location(struct MapCell* cell, int x, int y)
{
    if(map_cell_is_in_bounds(cell, x, y))
    {
        int _x = (x % g_map_cell_width);
        int _y = (y % g_map_cell_height);
        return _get_map_location_internal(cell, _x, _y);
    }
    return NULL;
}

struct MapLocation* map_cell_get_location_relative(struct MapCell* cell, int x, int y)
{
    if(x >= 0 && x < g_map_cell_width && y >= 0 &&  y < g_map_cell_height)
    {
        return _get_map_location_internal(cell, x, y);
    }
    return NULL;
}

//struct Room* map_cell_get_room(struct MapCell* cell, int x, int y)
//{
//    struct ListNode* node = NULL;
//    list_for_each(&cell->room_list, node)
//    {
//        if(room_has_location(node->data, x, y))
//        {
//            return node->data;
//        }
//    }
//
//    return NULL;
//}

struct List* map_cell_get_objects(struct MapCell* cell, int x, int y)
{
    struct MapLocation* loc = map_cell_get_location(cell, x, y);
    return loc ? &loc->obj_list : NULL;
}

void map_cell_add_mon(struct MapCell* cell, struct Mon* mon)
{
    map_cell_get_location(cell, mon->x, mon->y)->mon = mon;
    list_add(&cell->mon_list, mon);
}

bool map_cell_rm_mon(struct MapCell* cell, struct Mon* mon)
{
    struct ListNode* node = list_find(&cell->mon_list, mon);
    if(node)
    {
        map_cell_get_location(cell, mon->x, mon->y)->mon = NULL;
        list_rm(&cell->mon_list, node);
        return true;
    }

    return false;
}

bool map_cell_has_mon(struct MapCell* cell, int x, int y)
{
    return map_cell_get_location(cell, x, y)->mon != NULL;
}

bool map_cell_is_in_bounds(struct MapCell* cell, int x, int y)
{
    return (x >= cell->world_x && x < cell->world_x + g_map_cell_width) &&
           (y >= cell->world_y && y < cell->world_y + g_map_cell_height);
}
