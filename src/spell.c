#include "spell.h"

#include "colour.h"
#include "gameplay_commands.h"
#include "geom.h"
#include "list.h"
#include "log.h"
#include "look.h"
#include "map.h"
#include "map_cell.h"
#include "message.h"
#include "monster.h"
#include "mon_type.h"
#include "player.h"
#include "spell_effect.h"
#include "term.h"

#include <string.h>

struct Spell* g_spells = NULL;
int g_spells_count = 0;

enum SpellCastCommand
{
    SPELL_CAST_COMMAND_CONFIRM = KEYCODE_ENTER,
    SPELL_CAST_COMMAND_CANCEL  = KEYCODE_ESC
};

/* ---------- SECTION TARGETTING ---------- */

static void _spell_cast_skewer_get_targets(const struct MapLocation* origin, const struct MapLocation* target, List* loc_cache, List* mon_cache)
{

    int ox = origin->x; // Line origin (start) coords
    int oy = origin->y;
    int nx = ox; // Line segement next coords
    int ny = oy;
    int tx = target->x; // Line target (end) coords
    int ty = target->y;
    float err = 0.0f;

    struct MapLocation* loc = NULL;

    // Set the in-between line segments
    while(geom_gen_line_increment(ox, oy, tx, ty, &nx, &ny, &err))
    {
        loc = map_cell_get_location(map_get_cell_by_world_coord(g_cmap, nx, ny), nx, ny);
        list_add(loc_cache, loc);

        if(loc->mon)
        {
            list_add(mon_cache, loc->mon);
        }
    }
}

static void _spell_cast_get_targets(const struct Spell* spell, const struct MapLocation* origin, const struct MapLocation* target, List* loc_cache, List* mon_cache)
{
    switch(spell->spatial_type)
    {
        case SPELL_SPATIAL_SKEWER:
        {
            _spell_cast_skewer_get_targets(origin, target, loc_cache, mon_cache);
            break;
        }
        default:
        {
            break;
        }
    }
}

/* ---------- SECTION VISUALS ---------- */

// Generate a line, caching the line locations, and setting the term visuals
static void _spell_cast_skewer_visuals(const struct Mon* caster, List* loc_cache)
{
    int count = 0;
    ListNode* n = NULL;
    list_for_each(loc_cache, n)
    {
        int sx = -1;
        int sy = -1;
        struct MapLocation* loc = n->data;
        struct Symbol what_is_seen = look_get_symbol(loc, caster);

        map_get_screen_coord_by_world_coord(g_cmap, loc->x, loc->y, &sx, &sy);

        if(count == loc_cache->count - 1)
        {
            // Set the "head"/target segment
            term_draw_symbol(sx, sy, &what_is_seen.fg, &g_colours[CLR_WHITE], A_BLINK_BIT, what_is_seen.sym);
        }
        else
        {
            // Set the "tail" segments
            term_draw_symbol(sx, sy, &what_is_seen.fg, &g_colours[CLR_LGREY], A_BLINK_BIT, what_is_seen.sym);
        }

        ++count;
    }
}

// Select correct subfunction to set visuals
static void _spell_cast_set_visuals(const struct Spell* spell, const struct Mon* caster, List* loc_cache)
{
    switch(spell->spatial_type)
    {
        case SPELL_SPATIAL_INSTANTANEOUS:
            break;
        case SPELL_SPATIAL_SKEWER:
        {
            _spell_cast_skewer_visuals(caster, loc_cache);
            break;
        }
        case SPELL_SPATIAL_MISSILE:
            break;
        case SPELL_SPATIAL_AREA_SQUARE:
            break;
    }
}

// Take the cached locations and unset the term visuals, then clean the cache out
static void _spell_cast_unset_visuals(const struct Mon* caster, List* loc_cache)
{
    int sx = 0;
    int sy = 0;
    ListNode* n = NULL, *nn = NULL;

    list_for_each_safe(loc_cache, n, nn)
    {
        struct MapLocation* loc = n->data;

        map_get_screen_coord_by_world_coord(g_cmap, loc->x, loc->y, &sx, &sy);

        struct Symbol what_can_be_seen = look_get_symbol(loc, caster);

        term_draw_symbol(sx, sy, &what_can_be_seen.fg, &what_can_be_seen.bg, A_NONE_BIT, what_can_be_seen.sym);

        list_rm(loc_cache, n);
    }
}

/* ---------- SECTION INFO ---------- */

static void _spell_cast_target_info(struct Mon* caster, struct MapLocation* loc)
{
}

static bool _spell_cast_is_valid_loc(const struct Spell* spell, const struct Mon* caster, struct MapLocation* loc)
{
    if(!loc)
    {
        return false;
    }

    struct MapCell* cell = map_get_cell_by_world_coord(g_cmap, loc->x, loc->y);
    if(!cell)
    {
        // Out of bounds location
        return false;
    }

    switch(spell->spatial_type)
    {
        case SPELL_SPATIAL_SKEWER:
        {
            if(caster->x == loc->x && caster->y == loc->y)
            {
                // Cannot skewer yourself!
                return false;
            }

            if(loc_get_pathing(loc) == PATHING_NONE)
            {
                // TODO: Maybe this is a bit naive? Better solution needed?
                return false;
            }

            break;
        }
        default:
        {
            return false;
        }
    }

    return true;
}

void spell_cast(const struct Spell* spell)
{
    struct Mon* caster = g_you->mon;
    struct MapLocation* origin = map_cell_get_location(map_get_cell_by_world_coord(g_cmap, g_you->mon->x, g_you->mon->y), g_you->mon->x, g_you->mon->y);;
    struct MapLocation* target = origin;

    List loc_cache;
    List mon_cache;
    list_init(&loc_cache);
    list_init(&mon_cache);

    bool found_target = false;
    while(!found_target)
    {
        list_uninit(&loc_cache);
        list_uninit(&mon_cache);

        display_msg_nolog("Move cursor to select a target [hjklyubn]. Confirm [ENTER]. Cancel [ESC].");

        _spell_cast_get_targets(spell, origin, target, &loc_cache, &mon_cache);
        _spell_cast_set_visuals(spell, caster, &loc_cache);
        _spell_cast_target_info(caster, target);

        term_refresh();
        clear_msgs();
        flush_msg_buffer();

        _spell_cast_unset_visuals(caster, &loc_cache);

        struct MapLocation* next_target = NULL;
        enum SpellCastCommand cmd = cursor_free_move(target, &next_target);
        log_format_msg(LOG_DEBUG, "Spell cast command: %d", cmd);

        if(cmd == SPELL_CAST_COMMAND_CONFIRM)
        {
            found_target = true;
            break;
        }

        if(cmd == SPELL_CAST_COMMAND_CANCEL)
        {
            break;
        }

        if(!_spell_cast_is_valid_loc(spell, caster, next_target))
        {
            continue;
        }

        target = next_target;
    }

    log_msg(LOG_DEBUG, "Spell got targets:");
    ListNode* n = NULL;
    list_for_each(&mon_cache, n)
    {
        log_format_msg(LOG_DEBUG, "\t%s - %p", ((struct Mon*)n->data)->type->name, n->data);
    }

    if(found_target)
    {
        struct SpellEffectArgs args;
        args.affected_locations = &loc_cache;
        args.affected_mons = &mon_cache;
        list_for_each(&spell->spell_effects, n)
        {
            spell_effect_execute((struct SpellEffect*)n->data, &args);
        }
    }

    list_uninit(&loc_cache);
    list_uninit(&mon_cache);
}

struct Spell* spell_look_up_by_id(const char* id)
{
    for(int idx = 0; idx < g_spells_count; ++idx)
    {
        if(strcmp(g_spells[idx].id, id) == 0)
        {
            return &g_spells[idx];
        }
    }

    return NULL;
}
