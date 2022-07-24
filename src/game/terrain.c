#include "game/terrain.h"

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
