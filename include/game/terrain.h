#ifndef BYZ_GAME_TERRAIN_H
#define BYZ_GAME_TERRAIN_H

enum Biome
{
    BIOME_MEDITERRANEAN = 0
};

struct Terrain
{
    float biome;
    float elevation;
    float vegetation;
};

#endif
