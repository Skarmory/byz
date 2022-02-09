#ifndef BYZ_LOOK_H
#define BYZ_LOOK_H

#include "core/input_keycodes.h"
#include "core/symbol.h"

struct MapLocation;
struct Mon;

struct Symbol look_get_symbol(const struct MapLocation* loc, const struct Mon* looker);
void look(void);

#endif
