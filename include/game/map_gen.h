#ifndef BYZ_GAME_MAP_GEN_H
#define BYZ_GAME_MAP_GEN_H

struct Map;
struct MapCell;

void gen_map_cell(struct Map* map, struct MapCell* cell);
void gen_map(struct Map* map);

#endif
