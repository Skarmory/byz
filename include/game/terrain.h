#ifndef BYZ_TERRAIN_H
#define BYZ_TERRAIN_H

enum BIOME
{
    MEDITERRANEAN = 0
};

struct TERRAIN
{
    //enum BIOME biome;
    float biome;
    float elevation;
    float vegetation;
};

#endif
