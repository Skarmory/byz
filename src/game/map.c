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
struct Map* map_new(int width, int height)
{
    struct Map* map = malloc(sizeof(struct Map));
    map->width = width;
    map->height = height;
    list_init(&map->cell_list);

    return map;
}

/**
 * Safely deletes the map
 */
void map_free(struct Map* map)
{
    struct ListNode *node = NULL, *next = NULL;
    list_for_each_safe(&map->cell_list, node, next)
    {
        map_cell_free(node->data);
        free(node);
    }

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
