#include "game/map.h"

#include "core/colour.h"
#include "game/feature.h"
#include "game/gameplay_commands.h"
#include "core/input_keycodes.h"
#include "core/log.h"
#include "core/math_utils.h"
#include "game/map_cell.h"
#include "game/map_location.h"
#include "game/monster.h"
#include "game/mon_type.h"
#include "game/object.h"
#include "game/pathing.h"
#include "core/symbol.h"
#include "core/term.h"

#include <stdlib.h>

struct Map* g_cmap = NULL;
int         g_cx = 0;
int         g_cy = 0;

/**
 * Creates the map and sets map locations to default values
 */
struct Map* map_new(int width, int height, int seed)
{
    struct Map* map = malloc(sizeof(struct Map));
    map->width = width;
    map->height = height;
    map->seed = seed;

    list_init(&map->cell_list);

    return map;
}

/**
 * Safely deletes the map
 */
void map_free(struct Map* map)
{
    list_free_data(&map->cell_list, &map_cell_free_wrapper);
    free(map);
}

struct MapCell* map_get_cell_by_world_coord(struct Map* map, int x, int y)
{
    struct ListNode* node = NULL;
    list_for_each(&map->cell_list, node)
    {
        if(map_cell_is_in_bounds(node->data, x, y))
        {
            return node->data;
        }
    }

    return NULL;
}

struct MapCell* map_get_cell_by_cell_coord(struct Map* map, int x, int y)
{
    struct ListNode* node = NULL;
    list_for_each(&map->cell_list, node)
    {
        struct MapCell* cell = node->data;

        if(cell->cell_x == x && cell->cell_y == y)
        {
            return cell;
        }
    }

    return NULL;
}

struct MapLocation* map_get_location(struct Map* map, int x, int y)
{
    struct MapCell* cell = map_get_cell_by_world_coord(map, x, y);
    if(!cell)
    {
        return NULL;
    }

    return map_cell_get_location(cell, x, y);
}

bool map_in_bounds(struct Map* map, int x, int y)
{
    return map_get_location(map, x, y) != NULL;
}

bool map_in_bounds_cell(struct Map* map, int cx, int cy)
{
    return cx >= 0 && cx < map->width &&
           cy >= 0 && cy < map->height;
}
