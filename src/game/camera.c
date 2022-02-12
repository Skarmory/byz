#include "game/camera.h"

#include <stddef.h>

struct camera* g_camera = NULL;

int camera_max_x(struct camera* camera)
{
    return (camera->x + camera->w - 1);
}

int camera_max_y(struct camera* camera)
{
    return (camera->y + camera->h - 1);
}

bool camera_in_bounds(struct camera* camera, int x, int y)
{
    return x >= camera->x && x <= camera_max_x(camera) &&
           y >= camera->y && y <= camera_max_y(camera);
}

bool camera_world_to_screen(struct camera* camera, int world_x, int world_y, int* out_screen_x, int* out_screen_y)
{
    if(!camera_in_bounds(camera, world_x, world_y))
    {
        return false;
    }

    *out_screen_x = world_x - camera->x;
    *out_screen_y = world_y - camera->y;

    return true;
}

bool camera_screen_to_world(struct camera* camera, int screen_x, int screen_y, int* out_world_x, int* out_world_y)
{
    *out_world_x = screen_x + camera->x;
    *out_world_y = screen_y + camera->y;

    if(!camera_in_bounds(camera, *out_world_x, *out_world_y))
    {
        *out_world_x = -1;
        *out_world_y = -1;
        return false;
    }

    return true;
}
