#include "ui_map.h"

#include "feature.h"
#include "globals.h"
#include "log.h"
#include "map.h"
#include "map_cell.h"
#include "map_location.h"
#include "math_utils.h"
#include "monster.h"
#include "mon_type.h"
#include "object.h"
#include "player.h"
#include "term.h"

#include <stdlib.h>

struct UIMap* ui_map_new(int x, int y, int w, int h, struct Map* map)
{
    struct UIMap* ui_map = malloc(sizeof(struct UIMap));

    if( (x + w) > screen_cols )
    {
        w = screen_cols - w;
    }

    if( (y + h) > screen_rows )
    {
        h = screen_rows - h;
    }

    ui_map->ui_box.x = x;
    ui_map->ui_box.y = y;
    ui_map->ui_box.w = w;
    ui_map->ui_box.h = h;
    ui_map->map      = map;

    return ui_map;
}

void ui_map_free(struct UIMap* ui_map)
{
    free(ui_map);
}

void ui_map_draw(struct UIMap* ui_map)
{
    // Calculate where the ui map view origin is within the map
    int clamp_in_x = g_you->mon->x - (ui_map->ui_box.w / 2);
    int clamp_in_y = g_you->mon->y - (ui_map->ui_box.h / 2);
    int clamp_lo_x = 0;
    int clamp_lo_y = 0;
    int clamp_hi_x = (ui_map->map->width  * g_map_cell_width) - ui_map->ui_box.w;
    int clamp_hi_y = (ui_map->map->height * g_map_cell_height) - ui_map->ui_box.h;

    // Draw borders
    ui_box_draw(&ui_map->ui_box, false);

    for(int i = 1, x = clamp(clamp_in_x, clamp_lo_x, clamp_hi_x); i < ui_map->ui_box.w; ++x, ++i)
    for(int j = 1, y = clamp(clamp_in_y, clamp_lo_y, clamp_hi_y); j < ui_map->ui_box.h; ++y, ++j)
    {
        struct MapLocation* loc = map_get_location(g_cmap, x, y);
        if(!loc)
        {
            continue;
        }

        int screen_x = (i + ui_map->ui_box.x);
        int screen_y = (j + ui_map->ui_box.y);

        if(!mon_can_see(g_you->mon, x, y))
        {
            if(loc->seen)
            {
                if(loc->feature)
                {
                    term_draw_symbol(screen_x, screen_y, COL(CLR_FOG_OF_WAR), COL(CLR_DEFAULT), 0, loc->feature->symbol->sym);
                }
                else
                {
                    term_draw_symbol(screen_x, screen_y, COL(CLR_FOG_OF_WAR), COL(CLR_DEFAULT), 0, loc->symbol.sym);
                }
            }
            else
            {
                term_draw_symbol(screen_x, screen_y, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, ' ');
            }
        }
        else
        {
            loc->seen = true;

            if(loc->mon)
            {
                // Draw mon
                term_draw_symbol(screen_x, screen_y, &loc->mon->type->symbol->fg, &loc->mon->type->symbol->bg, loc->mon->type->symbol->attr, loc->mon->type->symbol->sym);
            }
            else if(loc_has_obj(loc))
            {
                // Draw object
                struct Object* obj = loc_get_obj(loc);
                term_draw_symbol(screen_x, screen_y, &obj->symbol->fg, &obj->symbol->bg, obj->symbol->attr, obj->symbol->sym);
            }
            else if(loc->feature)
            {
                // Draw feature
                term_draw_symbol(screen_x, screen_y, &loc->feature->symbol->fg, &loc->feature->symbol->bg, loc->feature->symbol->attr, loc->feature->symbol->sym);
            }
            else
            {
                // Draw base floor
                term_draw_symbol(screen_x, screen_y, &loc->symbol.fg, &loc->symbol.bg, loc->symbol.attr, loc->symbol.sym);
            }
        }
    }
}

void ui_map_set_map(struct UIMap* ui_map, struct Map* map)
{
    ui_map->map = map;
}
