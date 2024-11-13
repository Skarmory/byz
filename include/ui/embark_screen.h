#ifndef BYZ_UI_EMBARK_SCREEN_H
#define BYZ_UI_EMBARK_SCREEN_H

#include "core/cursor.h"
#include "core/input_keycodes.h"
#include "game/camera.h"
#include "ui/container.h"

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
struct MapCell;

struct EmbarkScreen* embark_screen_new(void);
void embark_screen_free(struct EmbarkScreen* embark_screen);

bool embark_screen_world_map_focussed(struct EmbarkScreen* es);
bool embark_screen_regional_map_focussed(struct EmbarkScreen* es);
struct Cursor embark_screen_get_cursor(struct EmbarkScreen* es);
struct Cursor embark_screen_get_world_map_container_cursor(struct EmbarkScreen* es);
struct Cursor embark_screen_get_regional_map_container_cursor(struct EmbarkScreen* es);
struct Container embark_screen_get_container(struct EmbarkScreen* es);
struct Container embark_screen_get_world_map_container(struct EmbarkScreen* es);
struct Container embark_screen_get_regional_map_container(struct EmbarkScreen* es);
struct camera embark_screen_get_camera(struct EmbarkScreen* es);

bool embark_screen_update(struct EmbarkScreen* embark_screen, enum KeyCode input);

void embark_screen_draw_world_map(struct EmbarkScreen* es, struct Map* map, struct camera* camera);
void embark_screen_draw_regional_map(struct EmbarkScreen* es, struct MapCell* region, struct camera* camera);
void embark_screen_draw_info_view(struct EmbarkScreen* es, void* info_data);

//void embark_screen_draw(struct EmbarkScreen* embark_screen);

#endif
