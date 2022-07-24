#ifndef BYZ_GAME_MAP_GEN_H
#define BYZ_GAME_MAP_GEN_H

struct Map;
struct MapLocation;

/* Create a map */
void gen_map(struct Map* map);

void gen_regional_map(struct Map* regional_map, const struct MapLocation* world_loc_ref);

#endif
