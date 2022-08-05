#include "game/map_gen.h"

#include "core/colour.h"
#include "core/geom.h"
#include "core/log.h"
#include "core/noise.h"
#include "core/poisson_disk.h"
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

#define ELEVATION_MODULO 100000000
#define PRECIPITATION_MODULO ELEVATION_MODULO

struct MapGenArgs
{
    int elevation_seed_x;
    int elevation_seed_y;
    int precipitation_seed_x;
    int precipitation_seed_y;
};

struct _gen_map_TaskArgs
{
    struct Map* map;
    struct MapGenArgs* map_gen_args;
};

struct _gen_map_cells_TaskArgs
{
    struct _gen_map_TaskArgs gen_map_args;
    int x;
};

struct _gen_map_cell_TaskArgs
{
    struct Map*     map;
    struct MapCell* cell;
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

static float _get_elevation(float offset_x, float offset_y, const struct MapGenArgs* map_gen_args)
{
    float elevation_x = offset_x + (float)map_gen_args->elevation_seed_x;
    float elevation_y = offset_y + (float)map_gen_args->elevation_seed_y;

    float noise = perlin2(elevation_x, elevation_y, 1.0f, 0.1f, 8);

    return noise;
}

static float _get_precipitation(float offset_x, float offset_y, const struct MapGenArgs* map_gen_args)
{
    float precipitation_x = offset_x + (float)map_gen_args->precipitation_seed_x;
    float precipitation_y = offset_y + (float)map_gen_args->precipitation_seed_y;

    float noise = perlin2(precipitation_x, precipitation_y, 1.0f, 0.1f, 8);

    return noise;
}

static struct Colour _get_biome_colour(enum BiomeType biome, struct RNG* rng)
{
    struct Colour c = biome_colour_from_enum(biome);

    // Random stagger colours for some variety
    c.r = clamp(c.r + (rng_get(rng) % 20) - 10, 0, 255);
    c.g = clamp(c.g + (rng_get(rng) % 20) - 10, 0, 255);
    c.b = clamp(c.b + (rng_get(rng) % 20) - 10, 0, 255);

    return c;
}

//static void _set_vegetation(struct MapLocation* loc, float map_seed, struct RNG* rng)
//{
//    const float c_vegetation_divisor = 4.0f;
//    const float c_shrub_range_start  = 0.0f;
//    const float c_shrub_range_end    = 0.4f;
//    const float c_tree_range_start   = 0.7f;
//    const float c_tree_range_end     = 1.0f;
//
//    float vegetation_x = (((float)loc->x) +  map_seed + 0.5f) / c_vegetation_divisor;
//    float vegetation_y = (((float)loc->y) +  map_seed + 0.5f) / c_vegetation_divisor;
//
//    loc->terrain.vegetation = perlin(vegetation_x, vegetation_y);
//
//    if(loc->terrain.elevation < 0.2f)
//    {
//        return;
//    }
//
//    if(loc->terrain.vegetation >= c_shrub_range_start && loc->terrain.vegetation < c_shrub_range_end)
//    {
//        loc->symbol.fg = *COL(CLR_LGREEN);
//
//        int variation = rng_get(rng) % 4;
//        if(variation == 0)
//        {
//            loc->symbol.sym = '.';
//        }
//        else if(variation == 1)
//        {
//            loc->symbol.sym = ',';
//        }
//        else if(variation == 2)
//        {
//            loc->symbol.sym = ';';
//        }
//        else
//        {
//            loc->symbol.sym = ':';
//        }
//
//        loc->symbol.fg.g = rng_get(rng) % 255;
//        //loc->symbol.fg.r = clamp(loc->symbol.fg.r + (rng_get(rng) % 30) - 15, 0, 255);
//        //loc->symbol.fg.g = clamp(loc->symbol.fg.g + (rng_get(rng) % 50) - 50, 0, 255);
//        //loc->symbol.fg.b = clamp(loc->symbol.fg.b + (rng_get(rng) % 30) - 15, 0, 255);
//
//    }
//    else if(loc->terrain.vegetation >= c_tree_range_start && loc->terrain.vegetation < c_tree_range_end)
//    {
//        loc->symbol.fg = *COL(CLR_LGREEN);
//
//        int variation = rng_get(rng) % 2;
//        if(variation == 0)
//        {
//            loc->symbol.sym = '*';
//        }
//        else
//        {
//            loc->symbol.sym = '#';
//        }
//
//        loc->symbol.fg.g = rng_get(rng) % 255;
//
//        //loc->symbol.fg.r = clamp(loc->symbol.fg.r + (rng_get(rng) % 30) - 15, 0, 255);
//        //loc->symbol.fg.g = clamp(loc->symbol.fg.g + (rng_get(rng) % 50) - 50, 0, 255);
//        //loc->symbol.fg.b = clamp(loc->symbol.fg.b + (rng_get(rng) % 30) - 15, 0, 255);
//    }
//
//
//    log_format_msg(LOG_DEBUG, "%d %d %d", loc->symbol.fg.r, loc->symbol.fg.g, loc->symbol.fg.b);
//}

static struct MapGenArgs _get_map_gen_args(struct RNG* rng)
{
    struct MapGenArgs map_gen_args;

    map_gen_args.elevation_seed_x = rng_get(rng) % ELEVATION_MODULO;
    map_gen_args.elevation_seed_y = rng_get(rng) % ELEVATION_MODULO;
    map_gen_args.precipitation_seed_x = rng_get(rng) % PRECIPITATION_MODULO;
    map_gen_args.precipitation_seed_y = rng_get(rng) % PRECIPITATION_MODULO;

    return map_gen_args;
}

void _gen_map_cell_no_locs(struct MapCell* cell, struct MapGenArgs* map_gen_args)
{
    const float step_size = 1.0f / g_map_cell_width;
    const int stride_x = g_map_cell_width / 4;
    const int stride_y = g_map_cell_height / 4;

    struct RNG* rng = rng_new(cell->seed);

    float avg_precipitation = 0.0f;
    float avg_elevation = 0.0f;

    // Sample 16 points across the cell to get an idea for avg values
    for(int rx = 0; rx < g_map_cell_width; rx+=stride_x)
    for(int ry = 0; ry < g_map_cell_height; ry+=stride_y)
    {
        float offset_x = (float)cell->cell_x + ((float)rx * step_size);
        float offset_y = (float)cell->cell_y + ((float)ry * step_size);

        avg_elevation += _get_elevation(offset_x, offset_y, map_gen_args);
        avg_precipitation += _get_precipitation(offset_x, offset_y, map_gen_args);
    }

    cell->terrain.elevation = avg_elevation / 16.0f;
    cell->terrain.precipitation = avg_precipitation / 16.0f;
    cell->terrain.biome = biome_from_params(cell->terrain.elevation, cell->terrain.precipitation);

    struct Colour c = _get_biome_colour(cell->terrain.biome, rng);
    cell->symbol.bg = c;
    cell->symbol.fg = *COL(CLR_DEFAULT);
    cell->symbol.sym = ' ';

    rng_free(rng);
}

void _gen_map_cell_locs(struct MapCell* cell, struct MapGenArgs* map_gen_args)
{
    const float step_size = 1.0f / g_map_cell_width;

    map_cell_init(cell);

    struct RNG* rng = rng_new(cell->seed);

    for(int rx = 0; rx < g_map_cell_width; ++rx)
    for(int ry = 0; ry < g_map_cell_height; ++ry)
    {
        struct MapLocation* loc = map_cell_get_location_relative(cell, rx, ry);
        loc->x = cell->cell_x * g_map_cell_width + rx;
        loc->y = cell->cell_y * g_map_cell_height + ry;

        float offset_x = (float)cell->cell_x + ((float)rx * step_size);
        float offset_y = (float)cell->cell_y + ((float)ry * step_size);

        loc->terrain.elevation = _get_elevation(offset_x, offset_y, map_gen_args);
        loc->terrain.precipitation = _get_precipitation(offset_x, offset_y, map_gen_args);
        loc->terrain.biome = biome_from_params(loc->terrain.elevation, loc->terrain.precipitation);

        struct Colour c = _get_biome_colour(loc->terrain.biome, rng);
        loc->symbol.bg = c;
        loc->symbol.fg = *COL(CLR_DEFAULT);
        loc->symbol.sym = ' ';
    }

    struct List* vegetation_points = poisson_disk(g_map_cell_width, g_map_cell_height, rng);
    struct ListNode *n = NULL, *nn = NULL;
    list_for_each_safe(vegetation_points, n, nn)
    {
        struct Point* p = n->data;
        struct MapLocation* loc = map_cell_get_location_relative(cell, p->x, p->y);

        loc->symbol.sym = '#';
        loc->symbol.fg = *COL(CLR_DGREEN);
    }

    list_free_data(vegetation_points, NULL);
    list_free(vegetation_points);

    cell->load_state = MAP_CELL_LOADED;

    rng_free(rng);
}

static int _gen_map_cells_task_func(void* args)
{
    struct _gen_map_cells_TaskArgs* cargs = args;
    for(int y = 0; y < cargs->gen_map_args.map->height; ++y)
    {
        log_format_msg(LOG_DEBUG, "Gen map cell %d, %d", cargs->x, y);

        struct MapCell* cell = map_get_cell_by_cell_coord(cargs->gen_map_args.map, cargs->x, y);
        _gen_map_cell_no_locs(cell, cargs->gen_map_args.map_gen_args);
    }

    return TASK_STATUS_SUCCESS;
};

static int _gen_map_cells_task_cbfunc(void* args)
{
    struct _gen_map_cells_TaskArgs* cargs = args;
    log_format_msg(LOG_DEBUG, "Gen map cells [%d, (0 - %d)] complete", cargs->x, cargs->gen_map_args.map->height - 1);

    return TASK_STATUS_SUCCESS;
}

static void _make_gen_map_cells_task(struct Map* map, int x, struct MapGenArgs* args)
{
    struct _gen_map_cells_TaskArgs targs;
    targs.gen_map_args.map = map;
    targs.gen_map_args.map_gen_args = args;
    targs.x = x;

    char task_name[256];
    snprintf(task_name, 256, "Gen Map Cells Task: %d", x);
    struct Task* task = task_new(
        task_name,
        &_gen_map_cells_task_func,
        &_gen_map_cells_task_cbfunc,
        &targs,
        sizeof(struct _gen_map_cells_TaskArgs)
    );

    tasker_add_task(g_tasker, task);
}

void gen_map_cell(struct Map* map, struct MapCell* cell)
{
    struct RNG* rng = rng_new(map->seed);
    struct MapGenArgs gen_args = _get_map_gen_args(rng);
    rng_free(rng);

    _gen_map_cell_locs(cell, &gen_args);
}

static int _gen_map_cell_task_func(void* args)
{
    struct _gen_map_cell_TaskArgs* cargs = args;
    gen_map_cell(cargs->map, cargs->cell);
    return TASK_STATUS_SUCCESS;
}

struct Task* gen_map_cell_async(struct Map* map, struct MapCell* cell)
{
    cell->load_state = MAP_CELL_LOADING;

    struct _gen_map_cell_TaskArgs args;
    args.map = map;
    args.cell = cell;

    struct Task* task = task_new(
        "Gen map cell task",
        &_gen_map_cell_task_func,
        NULL,
        &args,
        sizeof(struct _gen_map_cell_TaskArgs));

    tasker_add_task(g_tasker, task);

    return task;
}

void gen_map(struct Map* map)
{
    struct RNG* rng = rng_new(map->seed);
    struct MapGenArgs map_gen_args = _get_map_gen_args(rng);

    for(int x = 0; x < map->width; ++x)
    for(int y = 0; y < map->height; ++y)
    {
        struct MapCell* cell = map_get_cell_by_cell_coord(map, x, y);
        cell->seed = rng_get(rng);
    }

    rng_free(rng);

    for(int x = 0; x < map->width; ++x)
    {
        log_format_msg(LOG_DEBUG, "Generating cells [%d, (0 - %d)]", x, map->height - 1);

        // Gen the cell so we can calculate the average terrain values for this cell
        _make_gen_map_cells_task(map, x, &map_gen_args);
    }

    tasker_sync(g_tasker);
    tasker_log_state(g_tasker);

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
