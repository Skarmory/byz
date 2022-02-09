#include "game/map_location.h"

#include "game/feature.h"

#include <stddef.h>

/**
 * Add object to given map location
 */
bool loc_add_obj(struct MapLocation* loc, struct Object* obj)
{
    // push object onto location object linked list
    list_add(&loc->obj_list, obj);

    return true;
}

/**
 * Remove object from given map location
 */
bool loc_rm_obj(struct MapLocation* loc, struct Object* obj)
{
    struct ListNode* node = list_find(&loc->obj_list, obj);
    if(node)
    {
        list_rm(&loc->obj_list, node);
        return true;
    }
    return false;
}

bool loc_blocks_sight(struct MapLocation* loc)
{
    // TODO: Eventually mons or objects can block too
    if(loc->feature)
    {
        return loc->feature->block_sight;
    }

    return false;
}

bool loc_has_obj(struct MapLocation* loc)
{
    return !list_empty(&loc->obj_list);
}

bool loc_has_mon(struct MapLocation* loc)
{
    return loc->mon != NULL;
}

struct Object* loc_get_obj(struct MapLocation* loc)
{
    return list_peek_head(&loc->obj_list);
}

struct List* loc_get_objs(struct MapLocation* loc)
{
    return &loc->obj_list;
}

PathingFlags loc_get_pathing(struct MapLocation* loc)
{
    if(loc->feature)
    {
        return loc->feature->pathing_flags;
    }

    return loc->pathing_flags;
}
