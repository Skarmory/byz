#ifndef BYZ_CREATURE_TYPE_H
#define BYZ_CREATURE_TYPE_H

#include "game/mon_attr.h"
#include "game/mon_stats.h"

struct ArmourBase;
struct Weapon;
struct Symbol;

/**
 * Struct containing data about the type of a creature (not an individual monster)
 *
 * e.g. human, cow, sheep, etc.
 */
struct MonType
{
    char               id[5];
    char               name[32];
    char               desc[32];
    struct Symbol*     symbol;
    struct ArmourBase* base_armour;
    struct WeaponBase* base_weapon; 
    MonAttrMoveFlags   move_flags;
    int                vision_radius;
    struct Stats       base_stats;
};

struct MonType* mon_type_look_up_by_id(const char* id);

extern struct MonType* g_mon_type;
extern int g_mon_type_count;

#endif
