#ifndef BYZ_MAP_CELL_H
#define BYZ_MAP_CELL_H

#include "core/list.h"
#include "core/symbol.h"
#include "game/map_location.h"
#include "game/mon_attr.h"

#include <stdbool.h>

#define g_map_cell_width  100
#define g_map_cell_height 100

struct Mon;

struct MapCell
{
    int cell_x;
    int cell_y;
    int world_x;
    int world_y;
    int seed;
    struct MapLocation* locs;
    struct List mon_list;

    struct Symbol symbol;
};

struct MapCell* map_cell_new(int cell_x, int cell_y, int seed);
void map_cell_free(struct MapCell* cell);

/**
 * Return a map location based on world coordinates
 */
struct MapLocation* map_cell_get_location(struct MapCell* cell, int x, int y);

/**
 * Return a map location based on coordinates relative to the starting coordinates of the cell.
 * i.e. (0, 0) <= (x, y) < (g_map_cell_width, g_map_cell_height)
 */
struct MapLocation* map_cell_get_location_relative(struct MapCell* cell, int x, int y);

//struct Room* map_cell_get_room(struct MapCell* cell, int x, int y);

struct List* map_cell_get_objects(struct MapCell* cell, int x, int y);
void map_cell_add_mon(struct MapCell* cell, struct Mon* mon);
bool map_cell_rm_mon(struct MapCell* cell, struct Mon* mon);

bool map_cell_has_mon(struct MapCell* cell, int x, int y);
bool map_cell_is_in_bounds(struct MapCell* cell, int x, int y);

#endif
