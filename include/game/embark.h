#ifndef BYZ_GAME_EMBARK_H
#define BYZ_GAME_EMBARK_H

#include "core/input_keycodes.h"

#include <stdbool.h>

struct Embark;
struct Map;

struct Embark*    embark_new(struct Map* map);
void              embark_free(struct Embark* e);

const struct Map* embark_get_map(struct Embark* e);

bool              embark_update(struct Embark* e, enum KeyCode input);
void              embark_draw(struct Embark* e);

#endif
