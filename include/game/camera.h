#ifndef BYZ_CAMERA_H
#define BYZ_CAMERA_H

#include <stdbool.h>

struct camera
{
    int x;
    int y;
    int w;
    int h;
};

int  camera_max_x(struct camera* camera);
int  camera_max_y(struct camera* camera);
bool camera_in_bounds(struct camera* camera, int x, int y);

bool camera_world_to_screen(struct camera* camera, int world_x, int world_y, int* out_screen_x, int* out_screen_y);
bool camera_screen_to_world(struct camera* camera, int screen_x, int screen_y, int* out_world_x, int* out_world_y);
bool camera_relative_to_world(struct camera* camera, int rx, int ry, int* out_world_x, int* out_world_y);

extern struct camera* g_camera;

#endif
