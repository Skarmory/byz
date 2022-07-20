#ifndef BYZ_GAME_MAP_GEN_H
#define BYZ_GAME_MAP_GEN_H

struct Map;

/* Create a map */
void gen_map(struct Map* map);

void gen_regional_map(struct Map* regional_map, int world_map_x, int world_map_y);

#endif
