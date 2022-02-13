#include "ui/ui_map.h"

#include "game/camera.h"
#include "game/feature.h"
#include "game/globals.h"
#include "core/log.h"
#include "game/map.h"
#include "game/map_cell.h"
#include "game/map_location.h"
#include "core/math_utils.h"
#include "game/monster.h"
#include "game/mon_type.h"
#include "game/object.h"
#include "core/term.h"

#include <stdlib.h>

void ui_map_draw(struct Map* map, struct camera* camera)
{
    for(int screen_i = 0; screen_i < camera->w; ++screen_i)
    for(int screen_j = 0; screen_j < camera->h; ++screen_j)
    {
        int world_x = 0;
        int world_y = 0;

        if(camera_screen_to_world(camera, screen_i, screen_j, &world_x, &world_y))
        {
            struct MapLocation* map_location = map_get_location(map, world_x, world_y);

            if(map_location)
            {
                term_draw_symbol(screen_i, screen_j, &map_location->symbol.fg, &map_location->symbol.bg, 0, map_location->symbol.sym);
            }
            else
            {
                term_draw_symbol(screen_i, screen_j, COL(CLR_FOG_OF_WAR), COL(CLR_DEFAULT), 0, ' ');
            }
        }
    }

    //// Calculate where the ui map view origin is within the map
    //int clamp_in_x = g_cx - (ui_map->ui_box.w / 2);
    //int clamp_in_y = g_cy - (ui_map->ui_box.h / 2);
    //int clamp_lo_x = 0;
    //int clamp_lo_y = 0;
    //int clamp_hi_x = (ui_map->map->width  * g_map_cell_width) - ui_map->ui_box.w;
    //int clamp_hi_y = (ui_map->map->height * g_map_cell_height) - ui_map->ui_box.h;

    //// Draw borders
    //ui_box_draw(&ui_map->ui_box, false);

    //for(int i = 1, x = clamp(clamp_in_x, clamp_lo_x, clamp_hi_x); i < ui_map->ui_box.w; ++x, ++i)
    //for(int j = 1, y = clamp(clamp_in_y, clamp_lo_y, clamp_hi_y); j < ui_map->ui_box.h; ++y, ++j)
    //{
    //    struct MapLocation* loc = map_get_location(g_cmap, x, y);
    //    if(!loc)
    //    {
    //        continue;
    //    }

    //    int screen_x = (i + ui_map->ui_box.x);
    //    int screen_y = (j + ui_map->ui_box.y);

    //    //if(!mon_can_see(g_you->mon, x, y))
    //    //{
    //    //    if(loc->seen)
    //    //    {
    //    //        if(loc->feature)
    //    //        {
    //    //            term_draw_symbol(screen_x, screen_y, COL(CLR_FOG_OF_WAR), COL(CLR_DEFAULT), 0, loc->feature->symbol->sym);
    //    //        }
    //    //        else
    //    //        {
    //    //            term_draw_symbol(screen_x, screen_y, COL(CLR_FOG_OF_WAR), COL(CLR_DEFAULT), 0, loc->symbol.sym);
    //    //        }
    //    //    }
    //    //    else
    //    //    {
    //    //        term_draw_symbol(screen_x, screen_y, COL(CLR_DEFAULT), COL(CLR_DEFAULT), 0, ' ');
    //    //    }
    //    //}
    //    //else
    //    {
    //        loc->seen = true;

    //        if(loc->mon)
    //        {
    //            // Draw mon
    //            term_draw_symbol(screen_x, screen_y, &loc->mon->type->symbol->fg, &loc->mon->type->symbol->bg, loc->mon->type->symbol->attr, loc->mon->type->symbol->sym);
    //        }
    //        else if(loc_has_obj(loc))
    //        {
    //            // Draw object
    //            struct Object* obj = loc_get_obj(loc);
    //            term_draw_symbol(screen_x, screen_y, &obj->symbol->fg, &obj->symbol->bg, obj->symbol->attr, obj->symbol->sym);
    //        }
    //        else if(loc->feature)
    //        {
    //            // Draw feature
    //            term_draw_symbol(screen_x, screen_y, &loc->feature->symbol->fg, &loc->feature->symbol->bg, loc->feature->symbol->attr, loc->feature->symbol->sym);
    //        }
    //        else
    //        {
    //            // Draw base floor
    //            term_draw_symbol(screen_x, screen_y, &loc->symbol.fg, &loc->symbol.bg, loc->symbol.attr, loc->symbol.sym);
    //        }
    //    }
    //}
}
