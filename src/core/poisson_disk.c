#include "core/poisson_disk.h"

#include "core/geom.h"
#include "core/log.h"
#include "core/rng.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct PoissonDiskGrid
{
    struct Rect dimensions;
    bool* occupancy;
};

static struct Point _random_point_around(struct Point* p, float min_radius, float max_radius, struct RNG* rng)
{
    float r1 = rng_get_float(rng);
    float r2 = rng_get_float(rng);
    float radius = min_radius + ((max_radius - min_radius) * r1);
    float rang = 2.0f * M_PI * r2;

    int nx = (int)((float)p->x + radius * cos(rang));
    int ny = (int)((float)p->y + radius * sin(rang));

    struct Point ret = { .x = nx, .y = ny };
    return ret;
}

static bool _point_collides(struct Point* p, struct PoissonDiskGrid* grid, float min_radius)
{
    int irad = (int)min_radius;
    int irad2 = irad * irad;

    for(int x = (p->x - irad); x <= (p->x + irad); ++x)
    for(int y = (p->y - irad); y <= (p->y + irad); ++y)
    {
        struct Point tmp = { .x = x, .y = y };

        if(geom_point_in_rect2(&tmp, &grid->dimensions) && 
           geom_distance2_point_point(p, &tmp) < irad2 &&
           grid->occupancy[tmp.x + (tmp.y * grid->dimensions.w)] == true)
        {
            return true;
        }
    }

    return false;
}

static void _set_occupancy_around(struct Point* p, struct PoissonDiskGrid* grid, float min_radius)
{
    int irad = (int)min_radius;
    int irad2 = irad * irad;

    for(int x = (p->x - irad); x <= (p->x + irad); ++x)
    for(int y = (p->y - irad); y <= (p->y + irad); ++y)
    {
        struct Point tmp = { .x = x, .y = y };
        if(geom_point_in_rect2(&tmp, &grid->dimensions) && 
           geom_distance2_point_point(p, &tmp) < irad2)
        {
            grid->occupancy[tmp.x + (tmp.y * grid->dimensions.w)] = true;
        }
    }
}

static void _dbg_print_occupancy(struct PoissonDiskGrid* grid)
{
    char buf[257];
    memset(buf, 0, sizeof(buf));

    for(int y = 0; y < grid->dimensions.h; ++y)
    {
        for(int x = 0; x < grid->dimensions.w; ++x)
        {
            if(grid->occupancy[x + y * grid->dimensions.w] == true)
            {
                buf[x] = 'X';
            }
            else
            {
                buf[x] = 'O';
            }
        }

        log_msg(LOG_DEBUG, buf);
        memset(buf, 0, sizeof(buf));
    }
}

struct List* poisson_disk(int width, int height, struct RNG* rng)
{
    const int new_points_try = 30;
    const float min_radius = 3.0f;
    const float max_radius = 6.0f;

    struct PoissonDiskGrid poisson_disk_grid =
    {
        .dimensions = { .x = 0, .y = 0, .w = width, .h = height },
        .occupancy = calloc(width * height, sizeof(bool))
    };

    struct List* ret_list = list_new();
    struct List  proc_list;
    list_init(&proc_list);

    struct Point ip = { .x = rng_get(rng) % width, .y = rng_get(rng) % height };
    list_add(&proc_list, &ip);

    int add = 0;

    while(!list_empty(&proc_list))
    {
        int ridx = rng_get(rng) % proc_list.count;

        struct Point* p = list_pop_at(&proc_list, ridx);

        for(int i = 0; i < new_points_try; ++i)
        {
            struct Point np = _random_point_around(p, min_radius, max_radius, rng);

            if(geom_point_in_rect2(&np, &poisson_disk_grid.dimensions) &&
               !_point_collides(&np, &poisson_disk_grid, min_radius))
            {
                struct Point* npalloc = malloc(sizeof(struct Point));
                npalloc->x = np.x;
                npalloc->y = np.y;

                list_add(&proc_list, npalloc);
                list_add(ret_list, npalloc);
                _set_occupancy_around(npalloc, &poisson_disk_grid, min_radius);
                ++add;
            }
        }
    }

    log_format_msg(LOG_DEBUG, "Added %d poisson disks", add);

    _dbg_print_occupancy(&poisson_disk_grid);

    free(poisson_disk_grid.occupancy);
    list_uninit(&proc_list);
    return ret_list;
}
