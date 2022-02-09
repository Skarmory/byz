#include "game/mon_ai.h"

#include "core/log.h"
#include "game/map.h"
#include "game/map_cell.h"
#include "game/map_location.h"
#include "game/monster.h"
#include "game/mon_attack.h"
#include "game/movement.h"
#include "game/pathing.h"

#include <stdio.h>

/* Make decision for a given mon */
void update_mon_ai(struct Mon* mon)
{
   //// Player character is not an ai
   //if(mon == g_you->mon)
   //{
   //    return;
   //}

   //struct MapLocation* monloc = map_get_location(g_cmap, mon->x, mon->y);
   //struct MapLocation* youloc = map_get_location(g_cmap, g_you->mon->x, g_you->mon->y);

   //// Find next location closer to the player
   //struct MapLocation* next_loc = next_path_loc(monloc, youloc, mon->move_flags);

   //if(next_loc->x == youloc->x && next_loc->y == youloc->y)
   //{
   //    // Attack the player
   //    do_attack_mon_mon(mon, g_you->mon);
   //}
   //else
   //{
   //    // Move towards the player
   //    move_mon(mon, next_loc->x, next_loc->y);
   //}
}
