#ifndef BYZ_EmbarkScreen_H
#define BYZ_EmbarkScreen_H

#include "core/input_keycodes.h"
#include "core/cursor.h"

#include <stdbool.h>
#include <stdlib.h>

enum MapLayer
{
    MAP_LAYER_NORMAL  = 0,
    MAP_LAYER_PATHING = 1,
    MAP_LAYER_ELEVATION = 2
};

struct EmbarkScreen;
struct Map;

struct EmbarkScreen* embark_screen_new(struct Map* world_map);
void embark_screen_free(struct EmbarkScreen* embark_screen);

bool embark_screen_handle(struct EmbarkScreen* embark_screen, enum KeyCode input);
void embark_screen_draw(struct EmbarkScreen* embark_screen);

#endif
