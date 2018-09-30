#include "montype.h"

#include "armour.h"
#include "colour.h"
#include "mon_attr.h"
#include "stats.h"
#include "weapon.h"

#include <ncurses.h>

#define MON(n, sym, armour, weapon, stats, path) \
    {                                    \
        n, sym, armour, weapon, stats, path      \
    }

#define WEAPON(w) &weapons[w]
#define ARMOUR(a) &armours[a]

#define SYM(s, fg, bg, attr)\
    s, fg, bg, attr

#define STATS(str, agi, in, spi, sta)\
    str, agi, in, spi, sta

/**
 * Array containing all the mon types in the game
 */
struct MonType mon_type[] =
{
    MON("ghoul",     SYM('g', CLR_DC_1,     CLR_BLACK, 0), ARMOUR(A_UNARMOURED), WEAPON(W_CLAWS), STATS(5, 5, 5, 5, 5),      GROUND),
    MON("human",     SYM('@', CLR_ALLIANCE, CLR_BLACK, 0), ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(35, 35, 35, 35, 35), GROUND),
    MON("dwarf",     SYM('d', CLR_ALLIANCE, CLR_BLACK, 0), ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(35, 35, 35, 35, 35), GROUND),
    MON("night elf", SYM('e', CLR_ALLIANCE, CLR_BLACK, 0), ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(35, 35, 35, 35, 35), GROUND),
    MON("gnome",     SYM('g', CLR_ALLIANCE, CLR_BLACK, 0), ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(35, 35, 35, 35, 35), GROUND),
    MON("orc",       SYM('o', CLR_HORDE,    CLR_BLACK, 0), ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(35, 35, 35, 35, 35), GROUND),
    MON("forsaken",  SYM('u', CLR_HORDE,    CLR_BLACK, 0), ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(35, 35, 35, 35, 35), GROUND),
    MON("tauren",    SYM('t', CLR_HORDE,    CLR_BLACK, 0), ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(35, 35, 35, 35, 35), GROUND),
    MON("troll",     SYM('t', CLR_HORDE,    CLR_BLACK, 0), ARMOUR(A_UNARMOURED), WEAPON(W_FISTS), STATS(35, 35, 35, 35, 35), GROUND)
};
