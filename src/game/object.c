#include "game/object.h"

#include "core/log.h"
#include "game/obj_armour.h"
#include "game/obj_weapon.h"

#include <stdlib.h>

void free_obj(struct Object* obj)
{
    if(obj->objtype_ptr.weapon)
    {
        switch(obj->objtype)
        {
           case OBJ_TYPE_WEAPON: weapon_free(obj->objtype_ptr.weapon); return;
           case OBJ_TYPE_ARMOUR: armour_free(obj->objtype_ptr.armour); return;
        }
    }
}

void free_obj_wrapper(void* obj)
{
    free_obj((struct Object*)obj);
}

bool obj_is_equipment(struct Object* obj)
{
    return (obj->objtype == OBJ_TYPE_ARMOUR || obj->objtype == OBJ_TYPE_WEAPON);
}
