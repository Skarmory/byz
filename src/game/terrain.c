#include "game/terrain.h"

static const char* BIOME_NAMES[] =
{
    "ocean",
    "beach",
    "snow",
    "mountain",

    "tundra",
    "taiga",
    "subtropical desert",
    "temperate grassland",
    "temperate deciduous forest",
    "savannah",
    "tropical seasonal forest",
    "temperate rain forest",
    "tropical rain forest"
};

static const struct Colour BIOME_COLOURS[] =
{
    { 0, 0, 255 },     // Ocean
    { 235, 245, 20 },  // Beach
    { 255, 255, 255 }, // Snow
    { 128, 128, 128 }, // Mountain
    { 87, 235, 249 },  // Tundra
    { 5, 102, 33 },    // Taiga
    { 250, 148, 24 },  // Subtropical desert
    { 250, 219, 4 },   // Temperate grassland
    { 46, 177, 83 },   // Temperate deciduous forest
    { 200, 225, 35 },  // Savannah
    { 155, 225, 35 },  // Tropical seasonal forest
    { 80, 220, 65 },   // Temperate rain forest
    { 8, 250, 54 }    // Tropical rain forest
};

enum BiomeType biome_from_params(float elevation, float precipitation)
{
    if(elevation < 0.1f)
        return BIOME_TYPE_OCEAN;

    if(elevation < 0.12f)
        return BIOME_TYPE_BEACH;

    if(elevation > 0.7f)
    {
        if(precipitation < 0.2f)
            return BIOME_TYPE_MOUNTAIN;

        if(precipitation < 0.5f)
            return BIOME_TYPE_TUNDRA;

        return BIOME_TYPE_SNOW;
    }

    if(elevation > 0.6f)
    {
        if(precipitation < 0.1f)
        {
            return BIOME_TYPE_SUBTROPICAL_DESERT;
        }

        if(precipitation < 0.33f)
        {
            return BIOME_TYPE_TEMPERATE_GRASSLAND;
        }

        return BIOME_TYPE_TAIGA;
    }

    if(elevation > 0.5f)
    {
        if(precipitation < 0.1f)
        {
            return BIOME_TYPE_SUBTROPICAL_DESERT;
        }

        if(precipitation < 0.33f)
        {
            return BIOME_TYPE_TEMPERATE_GRASSLAND;
        }

        if(precipitation < 0.9f)
        {
            return BIOME_TYPE_TEMPERATE_DECIDUOUS_FOREST;
        }

        return BIOME_TYPE_TEMPERATE_RAIN_FOREST;
    }

    if(elevation > 0.3)
    {
        if(precipitation < 0.1f)
        {
            return BIOME_TYPE_SUBTROPICAL_DESERT;
        }

        if(precipitation < 0.3f)
        {
            return BIOME_TYPE_SAVANNAH;
        }

        if(precipitation < 0.8f)
        {
            return BIOME_TYPE_TROPICAL_SEASONAL_FOREST;
        }
        return BIOME_TYPE_TEMPERATE_RAIN_FOREST;
    }

    if(precipitation < 0.2f)
    {
        return BIOME_TYPE_SUBTROPICAL_DESERT;
    }

    if(precipitation < 0.4f)
    {
        return BIOME_TYPE_SAVANNAH;
    }

    if(precipitation < 0.6f)
    {
        return BIOME_TYPE_TROPICAL_SEASONAL_FOREST;
    }

    return BIOME_TYPE_TROPICAL_RAIN_FOREST;
}

const char* biome_name_from_enum(enum BiomeType type)
{
    return BIOME_NAMES[(int)type];
}

struct Colour biome_colour_from_enum(enum BiomeType type)
{
    return BIOME_COLOURS[(int)type];
}
