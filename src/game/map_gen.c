#include "game/map_gen.h"

#include "game/map_gen_utils.h"

#include "core/colour.h"
#include "game/feature.h"
#include "game/globals.h"
#include "core/log.h"
#include "game/map.h"
#include "game/map_cell.h"
#include "game/map_location.h"
#include "game/map_utils.h"
#include "core/symbol.h"
#include "core/tasking.h"
#include "game/util.h"

#include <stdlib.h>
#include <string.h>

void gen_map_cell(struct MapCell* cell)
{
}

void gen_map(struct Map* map)
{
    for(int x = 0; x < map->width; ++x)
    for(int y = 0; y < map->height; ++y)
    {
        struct MapCell* cell = map_cell_new(x, y);
        gen_map_cell(cell);
        list_add(&map->cell_list, cell);
    }
}
