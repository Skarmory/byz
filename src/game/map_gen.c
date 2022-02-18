#include "game/map_gen.h"

#include "game/map_gen_utils.h"

#include "core/colour.h"
#include "game/feature.h"
#include "game/globals.h"
#include "core/log.h"
#include "game/map.h"
#include "game/map_cell.h"
#include "game/map_location.h"
#include "game/map_utils.h"
#include "core/symbol.h"
#include "core/tasking.h"
#include "game/util.h"

#include "game/pathing_node.h"

#include <stdlib.h>
#include <string.h>

void gen_map_cell(struct MapCell* cell)
{
}

static void _connect_locations(struct MapLocation* cell_loc, struct MapLocation* adjacent_loc)
{
    if(!adjacent_loc)
    {
        return;
    }

    connectivity_node_add_connection(cell_loc->pathing, adjacent_loc->pathing);
    connectivity_node_add_connection(adjacent_loc->pathing, cell_loc->pathing);
}

static void _connect_cells(struct MapCell* cell, struct MapCell* adjacent)
{
    int x_off = adjacent->cell_x - cell->cell_x;
    int y_off = adjacent->cell_y - cell->cell_y;

    // Connect from top left to bottom right both ways, therefore don't need to check anything except where x or y is offset by +1
    if(x_off == 1 && y_off == 0)
    {
        // Adjacent is to the right
        int cell_x = g_map_cell_width - 1;
        int adjacent_x = 0;
        for(int cell_y = 0; cell_y < g_map_cell_height; ++cell_y)
        {
            struct MapLocation* cell_loc = map_cell_get_location_relative(cell, cell_x, cell_y);
            struct MapLocation* adjacent_loc = NULL;

            if(!cell_loc)
            {
                continue;
            }

            adjacent_loc = map_cell_get_location_relative(adjacent, adjacent_x, cell_y - 1);
            _connect_locations(cell_loc, adjacent_loc);

            adjacent_loc = map_cell_get_location_relative(adjacent, adjacent_x, cell_y);
            _connect_locations(cell_loc, adjacent_loc);

            adjacent_loc = map_cell_get_location_relative(adjacent, adjacent_x, cell_y + 1);
            _connect_locations(cell_loc, adjacent_loc);
        }
    }

    if(x_off == 0 && y_off == 1)
    {
        // Adjacent is to the bottom
        int cell_y = g_map_cell_height - 1;
        int adjacent_y = 0;

        for(int cell_x = 0; cell_x < g_map_cell_width; ++cell_x)
        {
            struct MapLocation* cell_loc = map_cell_get_location_relative(cell, cell_x, cell_y);
            struct MapLocation* adjacent_loc = NULL;

            if(!cell_loc)
            {
                continue;
            }

            adjacent_loc = map_cell_get_location_relative(adjacent, cell_x - 1, adjacent_y);
            _connect_locations(cell_loc, adjacent_loc);

            adjacent_loc = map_cell_get_location_relative(adjacent, cell_x, adjacent_y);
            _connect_locations(cell_loc, adjacent_loc);

            adjacent_loc = map_cell_get_location_relative(adjacent, cell_x + 1, adjacent_y);
            _connect_locations(cell_loc, adjacent_loc);
        }
    }

    if(x_off == 1 && y_off == 1)
    {
        // Adjacent is to the bottom right
        struct MapLocation* cell_loc     = map_cell_get_location_relative(cell, g_map_cell_width - 1, g_map_cell_height - 1);
        struct MapLocation* adjacent_loc = map_cell_get_location_relative(adjacent, 0, 0);
        _connect_locations(cell_loc, adjacent_loc);
    }
}

void gen_map(struct Map* map)
{
    for(int x = 0; x < map->width; ++x)
    for(int y = 0; y < map->height; ++y)
    {
        struct MapCell* cell = map_cell_new(x, y);
        gen_map_cell(cell);
        list_add(&map->cell_list, cell);
    }

    // Connect up the connectivity across cells
    for(int x = 0; x < map->width; ++x)
    for(int y = 0; y < map->height; ++y)
    {
        struct MapCell* cell = map_get_cell_by_cell_coord(map, x, y);
        if(!cell)
        {
            continue;
        }

        for(int i = x-1; i < x+2; ++i)
        for(int j = y-1; j < y+2; ++j)
        {
            if(i <= x && j <= y)
            {
                continue;
            }

            if(i < 0 || i >= map->width || j < 0 || j >= map->height)
            {
                continue;
            }


            struct MapCell* adjacent = map_get_cell_by_cell_coord(map, i, j);
            _connect_cells(cell, adjacent);
        }

    }
}
