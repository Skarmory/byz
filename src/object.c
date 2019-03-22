#include "object.h"

#include "armour.h"
#include "log.h"
#include "weapon.h"

#include <stdlib.h>

void free_obj(struct Object* obj)
{
    if(obj->objtype_ptr.weapon)
    {
        switch(obj->objtype)
        {
           case OBJ_TYPE_WEAPON: free_weapon(obj->objtype_ptr.weapon); return;
           case OBJ_TYPE_ARMOUR: free_armour(obj->objtype_ptr.armour); return;
        }
    }
}

bool obj_is_equipment(struct Object* obj)
{
    return (obj->objtype == OBJ_TYPE_ARMOUR || obj->objtype == OBJ_TYPE_WEAPON);
}
