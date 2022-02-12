#ifndef BYZ_MAP_H
#define BYZ_MAP_H

#include "core/list.h"
#include "game/mon_attr.h"

#include <stdbool.h>

#define MROWS 40
#define MCOLS 80

struct MapCell;
struct MapLocation;
struct Mon;

/**
 * Contains the map data
 */
struct Map
{
    int width;
    int height;
    struct List cell_list;
};

struct Map* map_new(int width, int height);
void map_free(struct Map* map);

struct MapCell* map_get_cell_by_world_coord(struct Map* map, int x, int y);

struct MapLocation* map_get_location(struct Map* map, int x, int y);

bool map_in_bounds(struct Map* map, int x, int y);

extern struct Map* g_cmap;
extern int g_cx;
extern int g_cy;

#endif
