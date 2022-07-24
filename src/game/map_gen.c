#include "game/map_gen.h"

#include "core/colour.h"
#include "core/log.h"
#include "core/noise.h"
#include "core/rng.h"
#include "core/symbol.h"
#include "core/tasking.h"
#include "core/math_utils.h"

#include "game/feature.h"
#include "game/globals.h"
#include "game/map.h"
#include "game/map_cell.h"
#include "game/map_gen_utils.h"
#include "game/map_location.h"
#include "game/map_utils.h"
#include "game/pathing_node.h"
#include "game/terrain.h"
#include "game/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ELEVATION_MODULO 1000000

enum MapType 
{
    MAP_TYPE_WORLD,
    MAP_TYPE_REGIONAL
};

struct MapGenArgs
{
    int elevation_seed_x;
    int elevation_seed_y;
};

struct GenMapCellTaskArgs
{
    struct MapCell* cell_gen;
    const struct MapLocation* loc_ref;
    const struct MapGenArgs* map_gen_args;
    float step_size;
};

//enum MAP_EDGE
//{
//    MAP_EDGE_NORTH = 0,
//    MAP_EDGE_SOUTH = 1,
//    MAP_EDGE_WEST  = 2,
//    MAP_EDGE_EAST  = 3
//};

//enum MAP_EDGE get_random_map_edge(void)
//{
//    return rand() % 4;
//}

//void get_random_map_edge_cell(struct Map* map, enum MAP_EDGE edge, int* out_x, int* out_y)
//{
//    if(edge == MAP_EDGE_NORTH)
//    {
//        *out_x = rand() % map->width;
//        *out_y = 0;
//    }
//    else if(edge == MAP_EDGE_SOUTH)
//    {
//        *out_x = rand() % map->width;
//        *out_y = map->height-1;
//    }
//    else if(edge == MAP_EDGE_WEST)
//    {
//        *out_x = 0;
//        *out_y = rand() % map->height;
//    }
//    else if(edge == MAP_EDGE_EAST)
//    {
//        *out_x = map->width-1;
//        *out_y = rand() % map->height;
//    }
//}

//void gen_river(struct Map* map)
//{
//    enum MAP_EDGE in_edge  = get_random_map_edge();
//    enum MAP_EDGE out_edge = get_random_map_edge();
//
//    if(in_edge == out_edge)
//    {
//        out_edge = (++out_edge % 4);
//    }
//
//    int in_edge_cell_x = -1;
//    int in_edge_cell_y = -1;
//    int out_edge_cell_x = -1;
//    int out_edge_cell_y = -1;
//
//    get_random_map_edge_cell(map, in_edge, &in_edge_cell_x, &in_edge_cell_y);
//    get_random_map_edge_cell(map, out_edge, &out_edge_cell_x, &out_edge_cell_y);
//}

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

static void _set_biome(struct MapLocation* loc, struct RNG* rng)
{
    const float c_biome_divisor = 20.0f;

    float random = (float)(rng_get(rng) % 100);

    float biome_x = (((float)loc->x) + random) / c_biome_divisor;
    float biome_y = (((float)loc->y) + random) / c_biome_divisor;

    loc->terrain->biome = perlin(biome_x, biome_y);
}

static void _set_elevation(struct MapLocation* loc, float offset_x, float offset_y, const struct MapGenArgs* map_gen_args)
{
    float elevation_x = offset_x + (float)map_gen_args->elevation_seed_x;
    float elevation_y = offset_y + (float)map_gen_args->elevation_seed_y;

    float noise = perlin2(elevation_x, elevation_y, 1.0f, 0.1f, 8);

    loc->terrain->elevation = noise;
}

static void _colour_elevation(struct MapLocation* loc, struct RNG* rng)
{
    static const int TERRAIN_THRESHOLDS_COLOUR_MAP[] =
    {
        CLR_BLUE,   // Water
        CLR_DBROWN, // Dirt/Mud
        CLR_GREEN,  // Grass
        CLR_LGREY,  // Moutain
        CLR_WHITE   // Snow
    };

    struct Colour c;
    int z = clamp((int)( loc->terrain->elevation * 5 ), 0, 4);
    c = *COL( TERRAIN_THRESHOLDS_COLOUR_MAP[z] );

    c.r = clamp(c.r + (rng_get(rng) % 20) - 10, 0, 255);
    c.g = clamp(c.g + (rng_get(rng) % 20) - 10, 0, 255);
    c.b = clamp(c.b + (rng_get(rng) % 20) - 10, 0, 255);

    loc->symbol.bg = c;
    loc->symbol.fg = *COL(CLR_DEFAULT);
    loc->symbol.sym = ' ';
}

static void _set_vegetation(struct MapLocation* loc, float map_seed, struct RNG* rng)
{
    const float c_vegetation_divisor = 4.0f;
    const float c_shrub_range_start  = 0.0f;
    const float c_shrub_range_end    = 0.4f;
    const float c_tree_range_start   = 0.7f;
    const float c_tree_range_end     = 1.0f;

    float vegetation_x = (((float)loc->x) +  map_seed + 0.5f) / c_vegetation_divisor;
    float vegetation_y = (((float)loc->y) +  map_seed + 0.5f) / c_vegetation_divisor;

    loc->terrain->vegetation = perlin(vegetation_x, vegetation_y);

    if(loc->terrain->elevation < 0.2f)
    {
        return;
    }

    if(loc->terrain->vegetation >= c_shrub_range_start && loc->terrain->vegetation < c_shrub_range_end)
    {
        loc->symbol.fg = *COL(CLR_LGREEN);

        int variation = rng_get(rng) % 4;
        if(variation == 0)
        {
            loc->symbol.sym = '.';
        }
        else if(variation == 1)
        {
            loc->symbol.sym = ',';
        }
        else if(variation == 2)
        {
            loc->symbol.sym = ';';
        }
        else
        {
            loc->symbol.sym = ':';
        }

        loc->symbol.fg.g = rng_get(rng) % 255;
        //loc->symbol.fg.r = clamp(loc->symbol.fg.r + (rng_get(rng) % 30) - 15, 0, 255);
        //loc->symbol.fg.g = clamp(loc->symbol.fg.g + (rng_get(rng) % 50) - 50, 0, 255);
        //loc->symbol.fg.b = clamp(loc->symbol.fg.b + (rng_get(rng) % 30) - 15, 0, 255);

    }
    else if(loc->terrain->vegetation >= c_tree_range_start && loc->terrain->vegetation < c_tree_range_end)
    {
        loc->symbol.fg = *COL(CLR_LGREEN);

        int variation = rng_get(rng) % 2;
        if(variation == 0)
        {
            loc->symbol.sym = '*';
        }
        else
        {
            loc->symbol.sym = '#';
        }

        loc->symbol.fg.g = rng_get(rng) % 255;

        //loc->symbol.fg.r = clamp(loc->symbol.fg.r + (rng_get(rng) % 30) - 15, 0, 255);
        //loc->symbol.fg.g = clamp(loc->symbol.fg.g + (rng_get(rng) % 50) - 50, 0, 255);
        //loc->symbol.fg.b = clamp(loc->symbol.fg.b + (rng_get(rng) % 30) - 15, 0, 255);
    }


    log_format_msg(LOG_DEBUG, "%d %d %d", loc->symbol.fg.r, loc->symbol.fg.g, loc->symbol.fg.b);
}

static void gen_map_cell(struct MapCell* cell, const struct MapLocation* loc_ref, const struct MapGenArgs* map_gen_args, float step_size)
{
    struct RNG* rng = rng_new(cell->seed);

    for(int rx = 0; rx < g_map_cell_width; ++rx)
    for(int ry = 0; ry < g_map_cell_height; ++ry)
    {
        struct MapLocation* loc = map_cell_get_location_relative(cell, rx, ry);

        //_set_biome(loc, map_seed);
        //_set_vegetation(loc, map_seed, rng);

        float offset_x = (float)loc_ref->x + ((float)rx * step_size);
        float offset_y = (float)loc_ref->y + ((float)ry * step_size);

        _set_elevation(loc, offset_x, offset_y, map_gen_args);

        _colour_elevation(loc, rng);
    }

    rng_free(rng);
}

static int _gen_map_cell_task_func(void* args)
{
    struct GenMapCellTaskArgs* cast_args = args;
    gen_map_cell(cast_args->cell_gen, cast_args->loc_ref, cast_args->map_gen_args, cast_args->step_size);
    return TASK_STATUS_SUCCESS;
}

static void _gen_map(enum MapType map_type, struct Map* map, const struct MapLocation* loc_ref, float scale)
{
    struct RNG* rng = rng_new(map->seed);

    struct MapGenArgs map_gen_args;
    map_gen_args.elevation_seed_x = rng_get(rng) % ELEVATION_MODULO;
    map_gen_args.elevation_seed_y = rng_get(rng) % ELEVATION_MODULO;

    for(int x = 0; x < map->width; ++x)
    for(int y = 0; y < map->height; ++y)
    {
        struct MapCell* cell = map_get_cell_by_cell_coord(map, x, y);
        cell->seed = rng_get(rng);

        char task_name[256];
        snprintf(task_name, 256, "Generate Map Cell %d, %d", x, y);
        struct GenMapCellTaskArgs task_args = {
            cell,
            map_type == MAP_TYPE_WORLD ? map_cell_get_location_relative(cell, 0, 0) : loc_ref,
            &map_gen_args,
            scale
        };

        struct Task* task = task_new(task_name, &_gen_map_cell_task_func, NULL, &task_args, sizeof(struct GenMapCellTaskArgs));
        tasker_add_task(g_tasker, task);
    }

    tasker_sync(g_tasker);
    tasker_integrate(g_tasker);

    rng_free(rng);
}

void gen_regional_map(struct Map* regional_map, const struct MapLocation* world_loc_ref)
{
    const float scale = 1.0f / (float)g_map_cell_width;
    _gen_map(MAP_TYPE_REGIONAL, regional_map, world_loc_ref, scale);
}

void gen_map(struct Map* map)
{
    _gen_map(MAP_TYPE_WORLD, map, NULL, 1.0f);

    //// Connect up the connectivity across cells
    //for(int x = 0; x < map->width; ++x)
    //for(int y = 0; y < map->height; ++y)
    //{
    //    struct MapCell* cell = map_get_cell_by_cell_coord(map, x, y);
    //    if(!cell)
    //    {
    //        continue;
    //    }

    //    for(int i = x - 1; i < x + 2; ++i)
    //    for(int j = y - 1; j < y + 2; ++j)
    //    {
    //        if(i <= x && j <= y)
    //        {
    //            continue;
    //        }

    //        if(i < 0 || i >= map->width || j < 0 || j >= map->height)
    //        {
    //            continue;
    //        }

    //        struct MapCell* adjacent = map_get_cell_by_cell_coord(map, i, j);
    //        _connect_cells(cell, adjacent);
    //    }
    //}

    //rng_free(rng);
}
