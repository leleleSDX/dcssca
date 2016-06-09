//
// Created by Jeremy Marlin Gurr on 6/8/16.
//

#include "AppHdr.h"

#include "branch.h"
#include "command.h"
#include "itemname.h"
#include "player.h"
#include "rune_curse.h"
#include "state.h"
#include "stepdown.h"
#include "unicode.h"
#include "stringutil.h"

const int active_rune_curses()
{
    int curses_active = 0;
    for (int rune = FIRST_RUNE; rune < NUM_RUNE_TYPES; rune++)
    {
        if (you.rune_curse_active[rune])
            curses_active++;
    }

    return curses_active;
}

const int rune_curse_hd_adjust(int hd, bool absolute)
{
    const int runes = active_rune_curses();
    const game_difficulty_level difficulty = crawl_state.difficulty;
    int multiplier = difficulty + 1;

    hd = hd + (runes * multiplier + 3) / 6;
    if (absolute && hd > 1)
    {
        hd = hd + difficulty - 2;
        hd = max(1, hd);
    }

    return hd;
}

const int rune_curse_hp_adjust(int hp, bool absolute)
{
    const int runes = active_rune_curses();
    hp = qpow(hp, 100 + crawl_state.difficulty + 1, 100, runes, false);
    return hp;
}

const int rune_curse_dam_adjust(int dam, bool absolute)
{
    const int runes = active_rune_curses();
    if (runes > 0 && dam != INSTANT_DEATH)
        dam = qpow(dam, 100 + crawl_state.difficulty + 1, 100, runes, false);
    return dam;
}

const int rune_curse_mon_spellpower_adjustment(int spellpower)
{
    if (you.rune_curse_active[RUNE_ELF])
    {
        spellpower = spellpower * 4 / 3;
    }

    return spellpower;
}

const int rune_curse_depth_adjust(int depth)
{
    /* not ready yet
    if (runes > 0)
        depth += runes;
        */
    return depth;
}

const string rune_curse_description(const rune_type rune)
{
    string message = "Coming soon...";

    switch (rune)
    {
        case RUNE_ELF:
            message = "Enemy spell power is increased.";
            break;

        case RUNE_SLIME:
            message = "Removing mutations is more difficult.";
            break;

        case RUNE_ABYSSAL:
            message = "More monsters are generated. Bands are larger.";
            break;

        case RUNE_SWAMP:
        case RUNE_SNAKE:
        case RUNE_SHOALS:
        case RUNE_VAULTS:
        case RUNE_TOMB:

        case RUNE_DIS:
        case RUNE_GEHENNA:
        case RUNE_COCYTUS:
        case RUNE_TARTARUS:

        case RUNE_DEMONIC:

        case RUNE_MNOLEG:
        case RUNE_LOM_LOBON:
        case RUNE_CEREBOV:
        case RUNE_GLOORX_VLOQ:

        case RUNE_SPIDER:
        case RUNE_DWARF:
        case RUNE_CRYPT:

        default:
            break;
    }

    return message;
}

void list_rune_curses()
{
    int cols = get_number_of_cols() - 1;

    bool at_least_one_active = false;
    for (int i = FIRST_RUNE; i < NUM_RUNE_TYPES; i++)
    {
        if (!you.rune_curse_active[i])
            continue;

        if (!at_least_one_active)
        {
            mprf(MSGCH_DANGER, "Rune curses in effect:");
        }

        at_least_one_active = true;
        const char* name = rune_type_name(i);
        const string curse_description = rune_curse_description((rune_type)i);

        const string output = chop_string(make_stringf("%-10s: %s", name, curse_description.c_str()), cols);
        mprf(MSGCH_DANGER, output.c_str());
    }

    if (!at_least_one_active)
        mprf(MSGCH_DANGER, "You are not affected by any rune curses.");
}

void choose_branch_rune_requirements()
{
    for (int branch_index = BRANCH_FIRST; branch_index < NUM_BRANCHES; branch_index++)
    {
        const bool is_rune_branch = branches[branch_index].runes.size() > 0;
        if(is_rune_branch)
        {
            while (coinflip())
            {
                you.branch_requires_runes[branch_index] += random2(3);
            }

            you.branch_requires_runes[branch_index] = min(you.branch_requires_runes[branch_index], 10);
        }
    }

    // make sure a few of them don't have any requirements
    int cleared = 0;
    while(cleared < 5)
    {
        int branch_index = random2(NUM_BRANCHES);
        const bool is_rune_branch = branches[branch_index].runes.size() > 0;
        if (!is_rune_branch)
        {
            continue;
        }

        you.branch_requires_runes[branch_index] = 0;
        cleared++;
    }
}