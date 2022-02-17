#ifndef BYZ_UI_MAP_H
#define BYZ_UI_MAP_H

struct camera;
struct Map;

enum MAP_LAYER
{
    MAP_LAYER_NORMAL  = 0,
    MAP_LAYER_PATHING = 1
};

void ui_map_draw(struct Map* map, struct camera* camera, enum MAP_LAYER layer);

#endif
