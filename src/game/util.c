#include "game/util.h"

#include "game/init.h"
#include "core/log.h"
#include "game/map.h"
#include "game/monster.h"
#include "game/mon_type.h"
#include "core/term.h"
#include "ui/ui.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Fills in the special format character with the correct game specific string
 */
void convert_arg(char c, char* buf)
{
//    switch(c)
//    {
//        case 'C':
//        {
//            strcpy(buf, g_you->cls->name_plural);
//            return;
//        }
//        case 'c':
//        {
//            strcpy(buf, g_you->cls->name);
//            return;
//        }
//    }
}

/**
 * Handles game shut down. Destroys everything, ends ncurses, and prints a message out to stdout.
 */
void do_quit(void)
{
    char exit_msg[512];
    //if(g_you && g_you->mon && mon_is_dead(g_you->mon))
    //{
    //    sprintf(exit_msg, "The cycle continues.");
    //}
    //else
    //{
    //    sprintf(exit_msg, "The fate of Yun is in your hands.");
    //}

    if(g_cmap)
    {
        map_free(g_cmap);
    }

    uninit_main();
    puts(exit_msg);
    exit(0);
}

void sigint_handler(int _)
{
    (void)_;

    if(prompt_yn("Really quit?"))
    {
        do_quit();
    }
}

/**
 * Get a random int between given range inclusive
 */
int random_int(int lo, int hi)
{
    return lo + (rand() % (hi - lo + 1));
}

int roll_dice(int dice_count, int dice_sides)
{
    int total = 0;

    for(int i = 0; i < dice_count; ++i)
    {
        total += random_int(1, dice_sides);
    }

    return total;
}

/**
 * Get a number between 1 and 100
 */
int roll_d100(void)
{
    return random_int(1, 100);
}

/**
 * Do percentile roll and return value between 0.01 and 1.0
 */
float roll_d100f(void)
{
    return (float)rand() / (float)RAND_MAX;
}

