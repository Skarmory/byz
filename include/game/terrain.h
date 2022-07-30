#ifndef BYZ_GAME_TERRAIN_H
#define BYZ_GAME_TERRAIN_H

#include "core/colour.h"

enum BiomeType
{
    BIOME_TYPE_OCEAN,
    BIOME_TYPE_BEACH,
    BIOME_TYPE_SNOW,
    BIOME_TYPE_MOUNTAIN,

    BIOME_TYPE_TUNDRA,
    BIOME_TYPE_TAIGA,
    BIOME_TYPE_SUBTROPICAL_DESERT,
    BIOME_TYPE_TEMPERATE_GRASSLAND,
    BIOME_TYPE_TEMPERATE_DECIDUOUS_FOREST,
    BIOME_TYPE_SAVANNAH,
    BIOME_TYPE_TROPICAL_SEASONAL_FOREST,
    BIOME_TYPE_TEMPERATE_RAIN_FOREST,
    BIOME_TYPE_TROPICAL_RAIN_FOREST,
};

struct Terrain
{
    float elevation;
    float precipitation;
    enum BiomeType biome;
};

enum BiomeType biome_from_params(float elevation, float precipitation);

const char* biome_name_from_enum(enum BiomeType type);
struct Colour biome_colour_from_enum(enum BiomeType type);

#endif
