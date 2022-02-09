#include "game/init.h"

#include "core/colour.h"
#include "game/feature.h"
#include "game/gameplay.h"
#include "game/globals.h"
#include "game/console_commands_init.h"
#include "core/log.h"
#include "game/mon_attack.h"
#include "game/mon_type.h"
#include "game/obj_armour.h"
#include "game/obj_weapon.h"
#include "game/parsing.h"
#include "core/symbol.h"
#include "core/tasking.h"
#include "game/util.h"

#include "ui/ui.h"
#include "ui/ui_map.h"

#include <stddef.h>
#include <stdlib.h>
#include <time.h>

static inline bool _init_symbols(void)
{
    g_symbol_weapon_metal.fg = *COL(g_symbol_weapon_metal.base_fg_idx);
    g_symbol_weapon_metal.bg = *COL(g_symbol_weapon_metal.base_bg_idx);

    g_symbol_weapon_wood.fg = *COL(g_symbol_weapon_wood.base_fg_idx);
    g_symbol_weapon_wood.bg = *COL(g_symbol_weapon_wood.base_bg_idx);

    return true;
}

static inline bool _init_gamedata(void)
{
    log_msg(LOG_DEBUG, "parsing attack types");
    if(parse_attack_methods() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing armours");
    if(parse_armours() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing weapons");
    if(parse_weapons() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing mon types");
    if(parse_mon_types() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing features");
    if(parse_features() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    return true;
}

void _uninit_gamedata(void)
{
    for(int idx = 0; idx < g_features_count; ++idx)
    {
        free(g_features[idx].symbol);
    }

    free(g_features);
    g_features_count = 0;

    for(int idx = 0; idx < g_mon_type_count; ++idx)
    {
        free(g_mon_type[idx].symbol);
    }

    free(g_mon_type);
    g_mon_type_count = 0;

    free(g_weapon_base);
    g_weapon_base_count = 0;

    free(g_armour_base);
    g_armour_base_count = 0;

    free(g_attack_methods);
    g_attack_methods_count = 0;
}

static bool _init_ui(void)
{
    g_ui = malloc(sizeof(struct UI));

    // TODO: Define things in data for easy configuration
    //struct UIMap* ui_map = ui_map_new(0, 0, 25, 25, NULL);

    return true;
}

static void _uninit_ui(void)
{
    ui_map_free(g_ui->ui_map);
    free(g_ui);
    g_ui = NULL;
}

bool init_main(void)
{
    srand(time(NULL));

    init_logs();
    term_init();
    term_set_sigint_callback(&sigint_handler);
    term_get_wh(&screen_cols, &screen_rows);

    g_tasker = tasker_new();

    if(!_init_symbols())
    {
        return false;
    }

    if(!_init_gamedata())
    {
        return false;
    }

    if(!_init_ui())
    {
        return false;
    }

    init_console_commands();

    return true;
}

void uninit_main(void)
{
    uninit_console_commands();
    _uninit_ui();
    _uninit_gamedata();
    tasker_free(g_tasker);
    term_uninit();
    uninit_logs();
}
