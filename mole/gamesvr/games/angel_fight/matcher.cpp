/*
 * =====================================================================================
 *
 *       Filename:  matcher.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/19/2011 10:07:54 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <algorithm>
#include <libtaomee++/random/random.hpp>
#include <netinet/in.h>

extern "C"
{
#include "../../proto.h"
#include "../../dbproxy.h"
#include "../../timer.h"
#include "../../game.h"
#include "../../dll.h"

#ifndef TW_VER
#include  <statistic_agent/msglog.h>
#endif
}
#include "../../ant/inet/pdumanip.hpp"

#include <map>
#include <vector>
#include "levelinfo.h"
#include "player.h"
#include "angel_fight.h"
#include "matcher.h"

#define MAX_LEVEL_DEFENCE	6

Matcher::Matcher()
{
	memset( m_pWaitPlayers, 0, sizeof(m_pWaitPlayers) );
	m_mapPlayer.clear();
}

Matcher::~Matcher()
{
	for( int i = 0; i < PLAYER_MAX_LEVEL; ++i )
	{
		if( m_pWaitPlayers[i] )
		{
			delete m_pWaitPlayers[i];
			m_pWaitPlayers[i] = NULL;
		}
	}
	m_mapPlayer.clear();
}

void Matcher::init( sprite_t* p )
{
	if( !p )
	{
		return;
	}
	DEBUG_LOG( "Player Enter Matcher ID:[%u] Wait DB ...", p->id );
	send_request_to_db( db_angel_fight_get_user_info, p, 0, NULL, p->id );
}

int Matcher::handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code )
{
	DEBUG_LOG( "Get DB return ID:[%u]", id );
	if( !p )
	{
		p->group->game_handler = NULL;
		return GER_end_of_game;
	}
	if( COMPETE_MODE_GAME(p->group->game) )
	{
		AddPlayer( p, (char*)buf, len );
	}
	else
	{
		Player* player = new Player();
		player->SetTeamID(0);
		player->SetInfo( (char*)buf, len );
		player->SetSprite(p);
		AngelFight* game = new AngelFight( p->group, player, NULL );
		p->group->game_handler = game;
		p->waitcmd = 0;
	}
	return 0;
}

int Matcher::handle_data( sprite_t* p, int cmd, const uint8_t body[], int len )
{
	DEBUG_LOG( "Player Leave Wait Queue Sprite:[%p] ID:[%u]", p, p->id );
	//离开竞赛游戏队列
	p->group->game_handler = NULL;

	if( COMPETE_MODE_GAME(p->group->game) )
	{
		Player* palyer = GetPlayer(p);
		RemovePalyer( palyer );
	}

	return GER_end_of_game;
}

int Matcher::handle_timeout( void* data )
{
	return 0;
}

bool Matcher::AddPlayer( sprite_t* p, char* data, int32_t len )
{
	if( !p || !data )
	{
		DEBUG_LOG( "AddPalyer Error Invalid arguments Sprite:[%p] Data:[%p] Len:[%d]", p, data, len );
		return false;
	}

	if( m_mapPlayer.find(p) != m_mapPlayer.end() )
	{
		ERROR_LOG( "AddPlayer Error Player was already Existed" );
		return false;
	}
#ifndef TW_VER
	{
		//统计参加pvp的人数
		uint32_t msgbuff[2]= { p->id, 1 };
		msglog( statistic_file, 0x0409B49B, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff) );
	}
#endif

	Player* newPlayer = new Player();
	newPlayer->SetInfo( data, len, true );
	newPlayer->SetSprite(p);
	int playerLevel = newPlayer->GetLevel();
	p->waitcmd = 0;
	DEBUG_LOG( "Player Enter Wait Queue ID:[%d] Level:[%d]", p->id, playerLevel );
// 	//莫名其妙的设定
// 	if( playerLevel > 25 )
// 	{
// 		playerLevel = 25;
// 	}
	if( playerLevel > 0 && playerLevel <= PLAYER_MAX_LEVEL )
	{
		playerLevel--;
		bool bGetTeam = false;
		int otherID;
		if( m_pWaitPlayers[playerLevel] != NULL )
		{
			otherID = playerLevel;
			bGetTeam = true;
		}
		for( int def = 1; def < MAX_LEVEL_DEFENCE; ++def )
		{
			int pos = playerLevel + def;
			if( pos >= 0 && pos < PLAYER_MAX_LEVEL && m_pWaitPlayers[ pos ] != NULL )
			{
				otherID = pos;
				bGetTeam = true;
				break;
			}
			else
			{
				pos = playerLevel - def;
				if( pos >= 0 && pos < PLAYER_MAX_LEVEL && m_pWaitPlayers[ pos ] != NULL )
				{
					otherID = pos;
					bGetTeam = true;
					break;
				}
			}
		}
		if( bGetTeam )
		{
			sprite_t* other = m_pWaitPlayers[otherID]->GetSprite();
			if( !other )
			{
				return false;
			}
			DEBUG_LOG( "Team Match OK P1:[%u], P2:[%u]", p->id, other->id );
			free_game_group( p->group );
			p->group = other->group;
			p->group->players[1] = p;
			p->group->count = 2;
			AngelFight* game = new AngelFight( p->group, newPlayer, m_pWaitPlayers[otherID] );
			//remove player from queue
			m_pWaitPlayers[otherID] = NULL;
			m_mapPlayer.erase(other);
			p->group->game_handler = game;
		}
		else
		{
			m_pWaitPlayers[playerLevel] = newPlayer;
			m_mapPlayer.insert( MapPlayerElem_t(p, newPlayer) );
		}
		return true;
	}
	else
	{
		ERROR_LOG( "AddPlayer Invalid Player Level:[%d]", playerLevel );
		return false;
	}
}

bool Matcher::RemovePalyer( Player* p )
{
	bool ret = true;
	if( !p )
	{
		DEBUG_LOG( "RemovePlayer Invalid Player Ptr" );
		return false;
	}

	int playerLevel = p->GetLevel();

// 	if( playerLevel > 25 )
// 	{
// 		playerLevel = 25;
// 	}

	DEBUG_LOG( "Remove Player ID:[%u], Level:[%d]", p->GetUserID(), playerLevel );

	MapPlayerIt_t it = m_mapPlayer.find( p->GetSprite() );
	if( it != m_mapPlayer.end() )
	{
		m_mapPlayer.erase(it);
	}
	else
	{
		ERROR_LOG( "RemovePlayer Can not Find The Player In Map" );
		ret = false;
	}


	if( playerLevel > 0 && playerLevel <= PLAYER_MAX_LEVEL && m_pWaitPlayers[playerLevel - 1] == p )
	{
		m_pWaitPlayers[playerLevel - 1] = NULL;
	}
	else
	{
		ERROR_LOG( "AddPlayer Invalid Player Level:[%d]", playerLevel );
		ret = false;
	}

	delete p;
	return ret;
}

Player* Matcher::GetPlayer( sprite_t* p )
{
	if( !p )
	{
		DEBUG_LOG( "GetPlayer Invalid Sprite" );
		return NULL;
	}
	MapPlayerIt_t it = m_mapPlayer.find(p);
	if( it != m_mapPlayer.end() )
	{
		return it->second;
	}
	else
	{
		DEBUG_LOG( "GetPlayer Can Not Find The Player Sprite:[%p]", p );
		return NULL;
	}
}
