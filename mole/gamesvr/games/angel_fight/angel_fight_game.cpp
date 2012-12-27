/*
 * =====================================================================================
 *
 *       Filename:  angel_fight_game.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/08/2011 05:48:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include <new>
extern "C"
{
#include <libtaomee/log.h>
#include "../../config.h"
#include "../../game.h"
}

#include <vector>

#include "effect.h"
#include "effectmgr.h"
#include "buff.h"
#include "buffmgr.h"
#include "skill.h"
#include "skillmgr.h"
#include "iteminfomgr.h"
#include "creatureinfomgr.h"
#include "levelinfo.h"
#include "levelmgr.h"
#include "angel_fight.h"
#include "matcher.h"

static Matcher* g_pMatcher = NULL;

extern "C"
{

void* create_game( game_group_t* grp )
{
	return g_pMatcher;
}

int game_init()
{
	if( !g_pMatcher )
	{
		//load config file
		g_pMatcher = new Matcher();
		InitializeExpAward();
		g_effectMgr.LoadEffectInfo();
		g_buffMgr.LoadBuffInfo();
		g_skillMgr.LoadSkillInfo();
		g_creatureInfoMgr.LoadCreatureInfo();
		g_itemInfoMgr.LoadItemInfo();
		g_levelMgr.LoadLevelInfo();
	}
	return 0;
}

void game_destroy()
{

}

void beginTest()
{
	DEBUG_LOG("Aagel Fight Initialize Game Config");
	game_init();
	DEBUG_LOG("Aagel Fight Create Game");
	AngelFight af(NULL);
	DEBUG_LOG("Aagel Fight Enter Game Loop");
	af.TestLoop();
}

}
