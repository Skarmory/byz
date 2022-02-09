#ifndef BYZ_UI_MAP_H
#define BYZ_UI_MAP_H

#include "ui/ui_box.h"

struct Map;

struct UIMap
{
    struct UIBox ui_box;
    struct Map*  map;
};

struct UIMap* ui_map_new(int x, int y, int w, int h, struct Map* map);
void          ui_map_free(struct UIMap* ui_map);

void          ui_map_draw(struct UIMap* ui_map);
void          ui_map_set_map(struct UIMap* ui_map, struct Map* map);

#endif
