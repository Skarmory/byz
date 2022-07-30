#ifndef BYZ_MAP_LOCATION_H
#define BYZ_MAP_LOCATION_H

/*
 * Map locations are the actual tiles/squares in the game world that creatures,
 * objects, and other things can occupy.
 */

#include "core/list.h"
#include "core/symbol.h"
#include "game/pathing_flags.h"

#include <stdbool.h>

#include "game/terrain.h"

struct Feature;
struct Mon;
struct Object;
struct ConnectivityNode;

/**
 * Contains information about a square on the map
 */
struct MapLocation
{
    int x;
    int y;
    struct ConnectivityNode* pathing;
    struct Mon* mon;
    struct Feature* feature;
    struct Terrain terrain;
    struct List obj_list;
    struct Symbol symbol;
    bool seen;
};

bool loc_add_obj(struct MapLocation* loc, struct Object* obj);
bool loc_rm_obj(struct MapLocation* loc, struct Object* obj);
bool loc_blocks_sight(struct MapLocation* loc);
bool loc_has_obj(struct MapLocation* loc);
bool loc_has_mon(struct MapLocation* loc);
struct Object* loc_get_obj(struct MapLocation* loc);
struct List* loc_get_objs(struct MapLocation* loc);
PathingFlags loc_get_pathing(struct MapLocation* loc);

#endif
