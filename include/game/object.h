#ifndef BYZ_OBJECT_H
#define BYZ_OBJECT_H

#include <stdbool.h>

#define OBJECT_WEAPON_OFF 0
#define OBJECT_ARMOUR_OFF 3

struct Weapon;
struct Armour;
struct Symbol;

enum ObjectType
{
    OBJ_TYPE_WEAPON,
    OBJ_TYPE_ARMOUR
};

/**
 * Wrapper class around general game items.
 *
 * Any item that can be picked up should be wrapped by an Object in order to add/remove it from a mon inventory.
 */
struct Object
{
    char* name;
    char* desc;
    struct Symbol* symbol;
    union
    {
        struct Weapon* weapon;
        struct Armour* armour;
    } objtype_ptr;

    enum ObjectType objtype;
};

/**
 * Calls the correct free function depending on object type
 */
void free_obj(struct Object* obj);
void free_obj_wrapper(void* obj);
bool obj_is_equipment(struct Object* obj);

#endif
