/*
 * =====================================================================================
 *
 *       Filename:  angel_fight.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/08/2011 05:58:50 PM
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

#ifndef TW_VER
#include  <statistic_agent/msglog.h>
#endif

}
#include "../../ant/inet/pdumanip.hpp"

#include <vector>
#include <map>

#include "player.h"
#include "creatureinfo.h"
#include "levelinfo.h"
#include "levelmgr.h"
#include "creatureinfomgr.h"
#include "packetmgr.h"
#include "angel_fight.h"

#define	TIMER_ROUND				30
#define TIMER_MONSTER_SELECT	30
#define TIMER_ACTION_ANI		15
#define TIMER_LOAD_DATA			120
#define TIMER_CHANGE_PLAYER		10

//=============================Globle==================================

struct ExpDef
{
	int32_t win;
	int32_t lose;
	ExpDef()
	{
	}
	ExpDef( int32_t w, int32_t l ):win(w), lose(l)
	{
	}
};

ExpDef		g_expInfo[2][19];	//好友对战经验
ExpDef		g_expMAPK[2][6];	//师徒之间对战经验

void InitializeExpAward()
{
	g_expInfo[0][0] = ExpDef(19,15);
	g_expInfo[0][1] = ExpDef(19,15);
	g_expInfo[0][2] = ExpDef(19,15);
	g_expInfo[0][3] = ExpDef(18,14);
	g_expInfo[0][4] = ExpDef(17,13);
	g_expInfo[0][5] = ExpDef(16,12);
	g_expInfo[0][6] = ExpDef(15,11);
	g_expInfo[0][7] = ExpDef(14,10);
	g_expInfo[0][8] = ExpDef(13,9);

	g_expInfo[0][9] = ExpDef(12,8);
	g_expInfo[0][10] = ExpDef(11,7);
	g_expInfo[0][11] = ExpDef(10,6);
	g_expInfo[0][12] = ExpDef(9,5);
	g_expInfo[0][13] = ExpDef(8,4);
	g_expInfo[0][14] = ExpDef(7,3);
	g_expInfo[0][15] = ExpDef(6,2);
	g_expInfo[0][16] = ExpDef(5,2);
	g_expInfo[0][17] = ExpDef(4,2);
	g_expInfo[0][18] = ExpDef(3,2);

	g_expInfo[1][0] = ExpDef(24,20);
	g_expInfo[1][1] = ExpDef(23,19);
	g_expInfo[1][2] = ExpDef(22,18);
	g_expInfo[1][3] = ExpDef(21,17);
	g_expInfo[1][4] = ExpDef(20,16);
	g_expInfo[1][5] = ExpDef(19,15);
	g_expInfo[1][6] = ExpDef(18,14);
	g_expInfo[1][7] = ExpDef(17,13);
	g_expInfo[1][8] = ExpDef(16,12);

	g_expInfo[1][9] = ExpDef(15,11);
	g_expInfo[1][10] = ExpDef(14,10);
	g_expInfo[1][11] = ExpDef(13,9);
	g_expInfo[1][12] = ExpDef(12,8);
	g_expInfo[1][13] = ExpDef(11,7);
	g_expInfo[1][14] = ExpDef(10,6);
	g_expInfo[1][15] = ExpDef(9,5);
	g_expInfo[1][16] = ExpDef(8,4);
	g_expInfo[1][17] = ExpDef(7,3);
	g_expInfo[1][18] = ExpDef(6,2);

	g_expMAPK[0][0] = ExpDef(50,30);
	g_expMAPK[0][1] = ExpDef(40,30);
	g_expMAPK[0][2] = ExpDef(30,25);
	g_expMAPK[0][3] = ExpDef(25,20);
	g_expMAPK[0][4] = ExpDef(20,15);
	g_expMAPK[0][5] = ExpDef(15,10);

	g_expMAPK[1][0] = ExpDef(25,20);
	g_expMAPK[1][1] = ExpDef(30,20);
	g_expMAPK[1][2] = ExpDef(50,35);
	g_expMAPK[1][3] = ExpDef(50,30);
	g_expMAPK[1][4] = ExpDef(50,25);
	g_expMAPK[1][5] = ExpDef(50,20);
}

int32_t GetExpAward(int32_t levelSelf,int32_t levelTar, bool isWin )
{
	DEBUG_LOG( "GetExpAward LevelSelf:[%d], LevelTar:[%d], IsWin:[%d]", levelSelf, levelTar, int(isWin) );
	int32_t region;
	if( levelSelf < 20 )
	{
		region = 0;
	}
	else
	{
		region = 1;
	}
	int32_t pos = levelSelf - levelTar + 9;
	if( pos < 0 )
	{
		pos = 0;
	}
	else if( pos > 18 )
	{
		pos = 18;
	}
	if( isWin )
	{
		return g_expInfo[region][pos].win;
	}
	else
	{
		return g_expInfo[region][pos].lose;
	}
}

int32_t GetExpMAPK( LEVEL_TYPE lt, int32_t levelSelf,int32_t levelTar, bool isWin )
{
	int region;
	int pos;
	if( lt == LT_PK_MASTER )
	{
		pos = levelTar - levelSelf;
		region = 1;
	}
	else if( lt == LT_PK_APPRENTICE || lt == LT_TRAIN )
	{
		pos = levelSelf - levelTar;
		region = 0;
	}
	else
	{
		return 0;
	}
	pos = pos/10 + 2;
	if( pos < 0 )
	{
		pos = 0;
	}
	if( pos > 5 )
	{
		pos = 5;
	}
	DEBUG_LOG( "GetExpMAPK LevelSelf:[%d],LevelTar:[%d], LevelType:[%d], IsWin:[%d], w:[%d]l:[%d]",
		levelSelf, levelTar, int32_t(lt), int32_t(isWin), g_expMAPK[region][pos].win, g_expMAPK[region][pos].lose );
	if( isWin )
	{
		return g_expMAPK[region][pos].win;
	}
	else
	{
		return g_expMAPK[region][pos].lose;
	}
}

//=============================AngelFight==================================

AngelFight::AngelFight( game_group_t* grp ):m_pGrp(grp), m_bQuickGame(false), m_nLevelID(-1), m_bAuto(false)
{
	m_nWinnerTeam = -1;
	INIT_LIST_HEAD( &m_timer );

	if( !m_pGrp )
	{
		return;
	}

	m_nPlayerCount = m_pGrp->count;
	for( int i = 0; i < PLAYER_COUNT; ++i )
	{
		m_pTeam[i] = new Player();
		m_pTeam[i]->SetTeamID(i);
		m_pPlayers[i] = m_pTeam[i];
	}

	m_pPlayers[0]->SetTar( m_pPlayers[1] );
	m_pPlayers[1]->SetTar( m_pPlayers[0] );

	for( uint32_t i = 0; i < m_nPlayerCount; ++i )
	{
		m_pPlayers[i]->SetSprite( m_pGrp->players[i] );
		//get player info from db
		send_request_to_db( db_angel_fight_get_user_info, m_pGrp->players[i], 0, NULL, m_pGrp->players[i]->id );
	}

	for( int i = m_nPlayerCount; i < PLAYER_COUNT; ++i )
	{
		m_pPlayers[i]->SetSprite( NULL );
	}

	m_nGameState = GS_WAIT_DB;
	m_nDBInfoCount 	= 0;
	srand(time(NULL));
	DEBUG_LOG("AngelFight Create Game OK");
}

AngelFight::AngelFight( game_group_t* grp, Player* p1, Player* p2 ):m_bQuickGame(false), m_nLevelID(-1)
{
	m_nWinnerTeam = -1;
	INIT_LIST_HEAD( &m_timer );
	m_pGrp = grp;

	if( !m_pGrp )
	{
		return;
	}

	m_nPlayerCount = 0;
	if( p1 )
	{
		m_pTeam[0] = p1;
		m_pTeam[0]->SetTeamID(0);
		m_pPlayers[0] = p1;
		m_nPlayerCount++;
	}

	if( p2 )
	{
		m_pTeam[1] = p2;
		m_pTeam[1]->SetTeamID(1);
		m_pPlayers[1] = p2;
		m_nPlayerCount++;
	}
	else
	{
		m_pTeam[1] = new Player();
		m_pTeam[1]->SetTeamID(1);
		m_pPlayers[1] = m_pTeam[1];
		m_pPlayers[1]->SetSprite( NULL );
	}

	m_pPlayers[0]->SetTar( m_pPlayers[1] );
	m_pPlayers[1]->SetTar( m_pPlayers[0] );
	srand(time(NULL));
	DEBUG_LOG("AngelFight Create Game OK");
	GameStart();
}

AngelFight::~AngelFight()
{
	for( uint32_t i = 0; i < PLAYER_COUNT; ++i )
	{
		delete m_pTeam[i];
		m_pPlayers[i] = NULL;
		m_pTeam[i] = NULL;
	}
	EndTimer(true);
}

int AngelFight::SetPlayerInfo( sprite_t * p,char * data, int len )
{
	DEBUG_LOG( "AngelFight SetPlayerInfo" );
	Player* player = GetPlayer( p );
	if( player )
	{
		if( 0 != player->SetInfo( data, len ) )
		{
			return GER_game_system_err;
		}
	}
	m_nDBInfoCount++;
	if( m_nDBInfoCount == m_nPlayerCount )
	{
		GameStart();
	}
	return 0;
}

int AngelFight::SetFriendInfo(uint32_t id,char * data,int len)
{
	DEBUG_LOG( "DB Return Set Friend Info OK" );
	if( 0 != m_pPlayers[1]->SetInfo( data, len ) )
	{
		return GER_game_system_err;
	}
	m_pPlayers[1]->SetCreatureID( int32_t(id) );

	return NotifyGameInfo();
}

void AngelFight::GameStart()
{
	DEBUG_LOG( "AngelFight GameStart()" );
	//单人游戏
	if( 1 == m_nPlayerCount )
	{
		BeginTimer( TE_SELECT_MONSTER, TIMER_MONSTER_SELECT );
		m_pPlayers[0]->SetValidCmd( AFC_QUIT | AFC_SELECT_MONSTER );
		m_nGameState = GS_WAIT_CLIENT;
		int len = sizeof(protocol_t);
		init_proto_head( pkg, AFP_DB_OK, len );
		send_to_player( m_pPlayers[0]->GetSprite(), pkg, len );
	}
	else
	{
		m_nLevelID = LEVEL_ID_PK;
		m_nLevelType = LT_PK;
		NotifyGameInfo();
	}

}

int AngelFight::OnAction( Player* p, const uint8_t buf[], int len )
{
	if( !p )
	{
		ERROR_LOG( "AngelFight::OnAction Invalid Player Ptr" );
		return GER_game_system_err;
	}
	if( !buf )
	{
		ERROR_LOG( "AngelFight::OnAction Invalid Buf Info" );
		return GER_game_system_err;
	}
	if( m_bQuickGame )
	{
		DEBUG_LOG("It's in Quick Game");
		return 0;
	}
	int j = 0;
	int32_t action;
	int32_t param;
	int expectLen = sizeof(action) + sizeof(param);
	CHECK_BODY_LEN( len, expectLen );
	ant::unpack( buf, action, j );
	ant::unpack( buf, param, j );
	//DEBUG_LOG("Action:[%d], Param:[%d]", action, param );
	int ret = p->SetActionState( ACTION_STATE(action), param );
	if( 0 != ret )
	{
		DEBUG_LOG("SetActionState Failed");
		return send_to_player_err( p->GetSprite(),p->GetSprite()->waitcmd, -ret );
	}
	else
	{
		if( action ==  AS_AUTO && !m_bAuto )
		{
			m_bAuto = true;
#ifndef TW_VER
			//统计使用自动游戏的人次
			uint32_t msgbuff[2]= { p->GetUserID(), 1 };
			msglog( statistic_file, 0x0409B43C, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff) );
#endif
		}
		DEBUG_LOG("SetActionState OK");
		EndTimer();
		return ActionAndNotify();
	}
}

int AngelFight::OnSelectMonster( Player* p, const uint8_t buf[], int len )
{
	if( !p )
	{
		ERROR_LOG( "AngelFight::OnSelectMonster Invalid Player Ptr" );
		return GER_game_system_err;
	}
	if( !buf )
	{
		ERROR_LOG( "AngelFight::OnSelectMonster Invalid Buf Info" );
		return GER_game_system_err;
	}
	EndTimer();
	if( m_nGameState != GS_WAIT_CLIENT )
	{
		return 0;
	}

	int j = 0;
	int32_t gameType;
	int32_t level_id;
	uint32_t apprID;	//抢徒弟时候的徒弟ID
	int expectLen = sizeof(int32_t) * 3;
	CHECK_BODY_LEN( len, expectLen );
	ant::unpack( buf, gameType, j );
	ant::unpack( buf, level_id, j );
	ant::unpack( buf, apprID, j );
	DEBUG_LOG( "Game type:[%d], level ID:[%d], apprID:[%u]", gameType, level_id, apprID );
	struct LevelCheck
	{
		int32_t levelType;
		union
		{
			int32_t		levelID;
			uint32_t	friendID;
			uint32_t	apprID;
			uint32_t	masterID;
		};
		union
		{
			int32_t		cardID;
		};
		union
		{
			int32_t		keyID;
		};
	}__attribute__((packed));

	LevelCheck db_buf;

	switch( gameType )
	{
	case GT_MONSTER:
		{
			//打怪
			m_nLevelID = level_id;
			if( !g_levelMgr.CheckValidID( m_nLevelID ) )
			{
				DEBUG_LOG( "Invalid Level ID" );
				return GER_end_of_game;
			}
			LevelInfo* levelInfo = 	g_levelMgr.GetLevelInfo( m_nLevelID );
			//无效关卡
			if( !levelInfo )
			{
				DEBUG_LOG( "Invalid Level ID" );
				return GER_end_of_game;
			}
			m_nLevelType = (LEVEL_TYPE)levelInfo->GetLevelType();
			int consume = levelInfo->m_nConsume;
			if( p->GetLevel() < 10 )
			{
				consume *= p->GetLevel();
				consume /= 10;
			}
			switch( m_nLevelType )
			{
			case LT_NORMAL:
				{
					if( p->GetEnergy() < consume ||					//能量不足
						p->GetLevel() < levelInfo->m_nCondPLevel	//等级不足
						)
					{
						DEBUG_LOG( "Conditon Ceck Faild Energy:[%d] Need:[%d] Level:[%d] Need:[%d]",
							p->GetEnergy(), consume, p->GetLevel(), levelInfo->m_nCondPLevel );
						return GER_end_of_game;
					}
					SelectMonster( p );
					return 0;
				}
				break;
			case LT_BOSS:
				{
					if( p->GetLevel() < levelInfo->m_nCondPLevel )
					{
						return GER_end_of_game;
					}
				}
				break;
			case LT_BOSS_TIME:
				{
					if( p->GetLevel() < levelInfo->m_nCondPLevel || !levelInfo->CheckTimeRange() )
					{
						return GER_end_of_game;
					}
				}
				break;
			case LT_BOSS_VIP:
				{
					if( p->GetLevel() < levelInfo->m_nCondPLevel )
					{
						return GER_end_of_game;
					}
				}
				break;
			case LT_BOSS_COST:
				{
					if( p->GetLevel() < levelInfo->m_nCondPLevel )
					{
						return GER_end_of_game;
					}
				}
				break;
			case LT_BOSS_ACTIVE:
				{

				}
				break;
			default:
				{
					return GER_end_of_game;
				}
				break;
			}
			db_buf.levelType = levelInfo->GetLevelType();
			db_buf.levelID = m_nLevelID;
			db_buf.cardID = levelInfo->GetCard();
			db_buf.keyID = levelInfo->GetKey();
		}
		break;
	case GT_FRIEND:
		{
			//挑战好友
			m_nLevelType = LT_FRIEND;
			LevelInfo* levelInfo = 	g_levelMgr.GetLevelInfo( LEVEL_ID_PVP );
			if( !levelInfo )
			{
				DEBUG_LOG( "Get Level Info Error" );
				return GER_game_system_err;
			}
			int consume = levelInfo->m_nConsume;
			if( p->GetLevel() < 10 )
			{
				consume *= p->GetLevel();
				consume /= 10;
			}
			if( p->GetVitality() < consume )		//活力不足
			{
				DEBUG_LOG( "Not Enough Power" );
				return GER_end_of_game;
			}
			m_nLevelID = LEVEL_ID_PVP;
			m_nFriendID = level_id;
			DEBUG_LOG( "Challenge Friend ID:[%u]", m_nFriendID );
			db_buf.levelType = m_nLevelType;
			db_buf.friendID = m_nFriendID;
			db_buf.cardID = 0;
			db_buf.keyID = 0;
		}
		break;
	case GT_GET_APPRENTICE:
		{
			m_nLevelType = LT_GET_APPRENTICE;
			m_nLevelID = LEVEL_ID_PVP;
			m_nFriendID = level_id;
			DEBUG_LOG( "Get Apprentice Game, Apprentice ID:[%u]", m_nFriendID );
			db_buf.levelType = m_nLevelType;
			db_buf.apprID = m_nFriendID;
			db_buf.cardID = 0;
			db_buf.keyID = 0;
		}
		break;
	case GT_GRAB_APPR:
		{
			m_nLevelType = LT_GRAB_APPRENTICE;
			m_nLevelID = LEVEL_ID_PVP;
			m_nFriendID = level_id;
			m_nTarApprID = apprID;
			DEBUG_LOG( "Grab Apprentice Game, Master ID:[%u]", m_nFriendID );
			db_buf.levelType = m_nLevelType;
			db_buf.masterID = m_nFriendID;
			db_buf.cardID = 0;
			db_buf.keyID = 0;
		}
		break;
	case GT_PK_APPR:
		{
			m_nLevelType = LT_PK_APPRENTICE;
			m_nLevelID = LEVEL_ID_PVP;
			m_nFriendID = level_id;
			DEBUG_LOG( "PK Apprentice Game, Apprentice ID:[%u]", m_nFriendID );
			db_buf.levelType = m_nLevelType;
			db_buf.friendID = m_nFriendID;
			db_buf.cardID = 0;
			db_buf.keyID = 0;
		}
		break;
	case GT_PK_MASTER:
		{
			m_nLevelType = LT_PK_MASTER;
			m_nLevelID = LEVEL_ID_PVP;
			m_nFriendID = level_id;
			DEBUG_LOG( "PK Master Game, Master ID:[%u]", m_nFriendID );
			db_buf.levelType = m_nLevelType;
			db_buf.friendID = m_nFriendID;
			db_buf.cardID = 0;
			db_buf.keyID = 0;
		}
		break;
	case GT_TRAIN:
		{
			m_nLevelType = LT_TRAIN;
			m_nLevelID = LEVEL_ID_PVP;
			m_nFriendID = level_id;
			DEBUG_LOG( "Master Train Game, Apprentice ID:[%u]", m_nFriendID );
			db_buf.levelType = m_nLevelType;
			db_buf.friendID = m_nFriendID;
			db_buf.cardID = 0;
			db_buf.keyID = 0;
		}
		break;
	default:
		{
			return GER_end_of_game;
		}
		break;
	}
	uint32_t wait = p->GetSprite()->waitcmd;
	p->GetSprite()->waitcmd = 0;
	send_request_to_db( db_angel_fight_check_level, p->GetSprite(), sizeof(db_buf), &db_buf, p->GetUserID() );
	p->GetSprite()->waitcmd = wait;
	m_nGameState = GS_WAIT_DB_LEVEL;
	return 0;
}

int AngelFight::OnQuit( Player* p, const uint8_t buf[], int len )
{
	if( !p )
	{
		ERROR_LOG( "AngelFight::OnQuit Invalid Player Ptr" );
		return GER_game_system_err;
	}

	p->Leave();
	m_nWinnerTeam = p->GetTar()->GetTeamID();
	if( m_nPlayerCount > 0 )
	{
		int len = sizeof(protocol_t);
		init_proto_head( pkg, AFP_PLAYER_LEAVE, len );
		send_to_player( p->GetTar()->GetSprite(), pkg, len );
	}
	EndTimer( true );
	return NotifyGameOver();
}

int AngelFight::OnLoadDataOK( Player* p, const uint8_t buf[], int len )
{
	if( !p )
	{
		ERROR_LOG( "AngelFight::OnLoadDataOK Invalid Player Ptr" );
		return GER_game_system_err;
	}

	p->SetValidCmd( p->GetValidCmd() & ~AFC_DATA_OK );
	if( EndTimer() )
	{
		return RoundBegin();
	}
	return 0;
}

int AngelFight::OnAniOK( Player* p, const uint8_t buf[], int len )
{
	if( !p )
	{
		ERROR_LOG( "AngelFight::OnAniOK Invalid Player Ptr" );
		return GER_game_system_err;
	}

	p->SetValidCmd( p->GetValidCmd() & ~AFC_ANI_OK );
	if( EndTimer() )
	{
		CheckGameOver();
	}
	return 0;
}

int AngelFight::OnQuickGame( Player* p, const uint8_t buf[], int len )
{
	DEBUG_LOG("Quick Game Begin");
#ifndef TW_VER
	{
		//统计使用闪电战的人次
		uint32_t msgbuff[2]= { p->GetUserID(), 1 };
		msglog( statistic_file, 0x0409B43B, get_now_tv()->tv_sec, &msgbuff, sizeof(msgbuff) );
	}
#endif
	EndTimer(true);
	m_bQuickGame = true;
	return ActionAndNotify();
}

int AngelFight::handle_data( sprite_t* p, int cmd, const uint8_t body[], int len )
{
	Player* player = GetPlayer( p );
	if( !player )
	{
		return GER_game_system_err;
	}

	if( cmd == proto_player_leave )
	{
		return OnQuit( player, NULL, 0 );
	}

	if( m_nGameState != GS_WAIT_CLIENT )
	{
		DEBUG_LOG( "[%d] System Busy", p->id );
		return send_to_player_err( p, p->waitcmd, -AFE_SYSTEM_BUSY );
	}
	//invalid cmd
	if( cmd != AFP_CMD_HEAD )
	{
		DEBUG_LOG( "[%d] Invalid Cmd;[%d]", p->id, cmd );
		return send_to_player_err( p, p->waitcmd, -AFE_INVALID_CMD );
	}
	//invalid data length
	if( len < 0 )
	{
		DEBUG_LOG( "[%d] Invalid Cmd Len:[%d]", p->id, len );
		return GER_invalid_data;
	}

	int cmd2;
	int expectLen = sizeof(cmd2);
	CHECK_BODY_LEN_GE( len, expectLen );
	int j = 0;
	ant::unpack( body, cmd2, j );

	if( !(player->GetValidCmd() & cmd2) )
	{
		DEBUG_LOG( "[%d] Invalid Child Cmd:[%d] And The  Valid Cmd is:[%d]", p->id, cmd2, player->GetValidCmd() );
		return send_to_player_err( p, p->waitcmd, -AFE_INVALID_OPERATE );
	}
//	DEBUG_LOG( "[%d] Child Cmd:[%d]", p->id, cmd2 );
	const uint8_t* realBody = body + expectLen;
	int realLen = len - expectLen;
	switch( cmd2 )
	{
	case AFC_ACTION:
		{
			return OnAction( player, realBody, realLen );
		}
	case AFC_SELECT_MONSTER:
		{
			return OnSelectMonster( player, realBody, realLen );
		}
	case AFC_QUIT:
		{
			return OnQuit( player, realBody, realLen );
		}
	case AFC_DATA_OK:
		{
			return OnLoadDataOK( player, realBody, realLen );
		}
	case AFC_ANI_OK:
		{
			return OnAniOK( player, realBody, realLen );
		}
	case AFC_QUICK_GAME:
		{
			return OnQuickGame( player, realBody, realLen );
		}
	default:
		return send_to_player_err( p, p->waitcmd, -AFE_INVALID_OPERATE );
	}
}

int AngelFight::handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code )
{
	DEBUG_LOG("AngelFight Get DB return GameState=[%d]", int32_t(m_nGameState) );

	switch( m_nGameState )
	{
	case GS_WAIT_DB:
		{
			if( 0 != SetPlayerInfo( p,(char*)buf, len ) )
			{
				return GER_game_system_err;
			}
		}
		break;
	case GS_WAIT_DB_FRIEND:
		{
			return SetFriendInfo( id,(char*)buf, len );
		}
		break;
		//验证当前关卡是否可进入
	case GS_WAIT_DB_LEVEL:
		{
			Player* player = GetPlayer(p);
			if( !player )
			{
				return GER_game_system_err;
			}

			int32_t levelOK = *(int32_t*)buf;
			if( 1 == levelOK/* || true */)
			{
				SelectMonster( player );
			}
			else
			{
				DEBUG_LOG( "Forward Level Hasn't Completed" );
				return GER_end_of_game;
			}
		}
		break;
	case GS_OVER:
		{
			int len = sizeof(protocol_t);
			ant::pack( pkg, m_nWinnerTeam, len );
			ant::pack( pkg, PLAYER_COUNT, len );
			int32_t playerIdx;
			for( uint32_t j = 0; j < PLAYER_COUNT; ++j )
			{
				ant::pack( pkg, m_pPlayers[j]->GetTeamID(), len );

				playerIdx = 0;
				Player* pTemp = m_pTeam[j];
				while( pTemp && m_pPlayers[j] != pTemp )
				{
					playerIdx++;
					pTemp = pTemp->GetPartner();
				}
				if( !pTemp )
				{
					playerIdx = 0;
					ERROR_LOG( "Get Player Idx Failed" );
				}
				DEBUG_LOG( "Team:[%d] Idx:[%d] HP:[%d] MP:[%d]", m_pPlayers[j]->GetTeamID(), playerIdx, m_pPlayers[j]->GetHP(), m_pPlayers[j]->GetMP() );
				ant::pack( pkg, playerIdx, len );
				ant::pack( pkg, m_pPlayers[j]->GetHP(), len );
				ant::pack( pkg, m_pPlayers[j]->GetMP(), len );
			}
			init_proto_head( pkg, AFP_NOTIFY_GAME_OVER, len );

			send_to_player( p, pkg, len );
			m_nDBInfoCount--;
			if( 0 == m_nDBInfoCount )
			{
				return GER_end_of_game;
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

int AngelFight::handle_timeout( void* data )
{
	EndTimer(true);
	m_nTimerID = 0;

	DEBUG_LOG( "AngelFight Timer Out, TimerEvent:[%d]", int32_t(m_nTimerEvent) );

	switch(m_nTimerEvent)
	{
	case TE_OPERATION:
		{
			for( uint32_t i = 0; i < m_nPlayerCount; ++i )
			{
				if( m_pPlayers[i]->GetActionState() == AS_WAIT )
				{
					m_pPlayers[i]->AutoAct();
					ActionAndNotify();
				}
			}
		}
		break;
	case TE_SELECT_MONSTER:
		{
			return GER_end_of_game;
		}
	case TE_WAIT_ANI:
		{
			return CheckGameOver();
		}
		break;
	case TE_LOAD_DATA:
		{
			return GER_end_of_game;
		}
	case TE_CHANGE_PLAYER:
		{
			return RoundBegin();
		}
		break;
	default:
		break;
	}
	return 0;
}

void AngelFight::init( sprite_t * p )
{

}

Player* AngelFight::GetPlayer( sprite_t* p )
{
	for( uint32_t i = 0; i < m_nPlayerCount; ++i )
	{
		if( m_pPlayers[i]->GetSprite() == p )
		{
			return m_pPlayers[i];
		}
	}
	ERROR_LOG("AngelFight::GetPlayer failed");
	return	NULL;
}

int AngelFight::ActionAndNotify()
{
	//DEBUG_LOG("AngelFight::ActionAndNotify()");

	for( uint32_t i = 0; i < PLAYER_COUNT; ++i )
	{
		//somebody has not ready
		if( m_pPlayers[i]->GetActionState() == AS_WAIT )
		{
			if( m_bQuickGame )
			{
				m_pPlayers[i]->AutoAct();
			}
			else
			{
				return 0;
			}
		}
	}

	Player* pAct[PLAYER_COUNT];

	if(	m_pPlayers[0]->GetActSPD() > m_pPlayers[1]->GetActSPD() )
	{
		pAct[0] = m_pPlayers[0];
		pAct[1] = m_pPlayers[1];
	}
	else if( m_pPlayers[0]->GetActSPD() < m_pPlayers[1]->GetActSPD() )
	{
		pAct[0] = m_pPlayers[1];
		pAct[1] = m_pPlayers[0];
	}
	//equal
	else
	{
		int val = rand()%2;
		pAct[0] = m_pPlayers[val];
		pAct[1] = m_pPlayers[val^1];
	}

	int len = sizeof(protocol_t);
	//清空发送缓存
	pAct[0]->NoticeBuffClear();
	pAct[1]->NoticeBuffClear();

	//有一方势力全灭后，不再进行计算，避免出现双方同时阵亡

	//是否只通知操作而不做行动
	bool bDeclare = false;
	for( uint32_t i = 0; i < PLAYER_COUNT; ++i )
	{
		int ret = pAct[i]->Action( bDeclare );

		switch( ret )
		{
		//无阵亡
		case PS_NORMAL:
			{
				//执行buff效果
				if( PS_OVER == pAct[i]->ExecuteBuff() )
				{
					bDeclare = true;
				}
			}
			break;
			//自己阵亡
		case PS_OVER:
			{
				//自己阵亡后若没有后续队友则结束
				if( !pAct[i]->GetValidPartner() )
				{
					bDeclare = true;
				}
			}
			break;
			//对手全体阵亡
		case PS_WIN:
			{
			}
			break;
		default:
			break;
		}
	}

	for( int i = 0; i < PLAYER_COUNT; ++i )
	{
		pAct[i]->ConstraintHpMp();
		pAct[i]->GetNoticeInfo( pkg, len , 4096 );
	}

	init_proto_head( pkg, AFP_NOTIFY_ACTION, len );

	//UnPackDataForTest( pkg, len );

	if( m_pGrp && !m_bQuickGame )
	{
		send_to_players( m_pGrp, pkg, len );
		//DEBUG_LOG( "Send Action Info To  Players User ID1:[%u] ID2:[%u]", m_pPlayers[0]->GetUserID(), m_pPlayers[1]->GetUserID() );
	}

	// wait for client's animation
	if( !m_bQuickGame )
	{
		for( uint32_t i = 0; i < m_nPlayerCount; ++i )
		{
			m_pPlayers[i]->SetValidCmd( AFC_QUIT | AFC_ANI_OK );
		}
		BeginTimer( TE_WAIT_ANI, TIMER_ACTION_ANI );
		return 0;
	}
	else
	{
		return CheckGameOver();
	}
}

int AngelFight::NotifyRoundBegin()
{
	int len = sizeof(protocol_t);
	ant::pack( pkg, PLAYER_COUNT, len );
	for( uint32_t i = 0; i < PLAYER_COUNT; ++i )
	{
		ant::pack( pkg, m_pPlayers[i]->GetTeamID(), len );
		ant::pack( pkg, m_pPlayers[i]->GetHP(), len );
		ant::pack( pkg, m_pPlayers[i]->GetMP(), len );
	}
	init_proto_head( pkg, AFP_NOTIFY_ROUND_BEGIN, len );
	for( int i = 0; i < PLAYER_COUNT; ++i )
	{
		if( !m_pPlayers[i]->GetSprite() )		// Monster

		{
			m_pPlayers[i]->AutoAct();
		}

		sprite_t* p = m_pPlayers[i]->GetSprite();
		if( p )
		{
			send_to_player( p, pkg, len );
		}
	}
	return 0;
}


int AngelFight::CheckGameOver()
{
	for( int i = 0; i < PLAYER_COUNT; ++i )
	{
		if( m_pPlayers[i]->GetHP() <= 0 )
		{
			if( m_pPlayers[i]->GetValidPartner() )
			{
				m_pPlayers[i] = m_pPlayers[i]->GetValidPartner();
				m_pPlayers[i]->GetTar()->SetTar( m_pPlayers[i] );
				//notify change player
				int len = sizeof(protocol_t);
				ant::pack( pkg, m_pPlayers[i]->GetTeamID(), len );
				ant::pack( pkg, m_pPlayers[i]->GetUserID(), len );
				init_proto_head( pkg, AFP_NOTIFY_CHANGE_PLAYER, len );
				if( m_pGrp )
				{
					if( !m_bQuickGame )
					{
						send_to_players( m_pGrp, pkg, len );
						for( uint32_t j = 0; j < m_nPlayerCount; ++j )
						{
							m_pPlayers[j]->SetValidCmd( AFC_QUIT | AFC_ANI_OK );
						}
						BeginTimer( TE_CHANGE_PLAYER, TIMER_CHANGE_PLAYER );
						return 0;
					}
					else
					{
						return RoundBegin();
					}
				}
			}
			else
			{
				m_nWinnerTeam = i ^ 0x1;
				return NotifyGameOver();
			}
		}
	}

	return RoundBegin();
}

int AngelFight::NotifyGameOver()
{
	DEBUG_LOG("AngelFight::NotifyGameOver() Winner Team;[%d]", m_nWinnerTeam );
	if( -1 == m_nWinnerTeam )
	{
		return 0;
	}
	if( m_nLevelID == -1 )
	{
		return GER_end_of_game;
	}
	m_nDBInfoCount = 0;
	m_nGameState = GS_OVER;
	for( int i = 0; i < PLAYER_COUNT; ++i )
	{
		sprite_t* p = m_pPlayers[i]->GetSprite();

		if( !p )
		{
			continue;
		}
		Player* pTar = m_pPlayers[i]->GetTar();
		if( !pTar )
		{
			return GER_end_of_game;
		}
		struct GameOverInfo
		{
			int32_t		expAward;
			int32_t		levelID;
			int32_t		cPoint;
			int32_t		isWin;
			int32_t		tarLevel;
			uint32_t	tarID;
			int32_t		canGetCard;
			int32_t		consume;
			int32_t		isEngry;
			int32_t		levelType;
			union
			{
				int32_t		cardID;
				uint32_t	apprID;
			};
			int32_t		keyID;

		} __attribute__((packed));
		GameOverInfo gameOverInfo;
		memset( &gameOverInfo, 0, sizeof(gameOverInfo) );
		gameOverInfo.isWin = m_pPlayers[i]->GetTeamID() == m_nWinnerTeam;
		gameOverInfo.tarID = pTar->GetUserID();
		gameOverInfo.tarLevel = pTar->GetLevel();
		gameOverInfo.canGetCard = 1;
		gameOverInfo.levelType = m_nLevelType;
		gameOverInfo.levelID = m_nLevelID;
		LevelInfo* levelInfo = g_levelMgr.GetLevelInfo( m_nLevelID );
		if (!levelInfo)
		{
			return GER_end_of_game;
		}
		gameOverInfo.consume = levelInfo->m_nConsume;
		if( m_pPlayers[i]->GetLevel() < 10 )
		{
			gameOverInfo.consume *= m_pPlayers[i]->GetLevel();
			gameOverInfo.consume /= 10;
		}

		if( m_nLevelID == LEVEL_ID_PVP )
		{
			gameOverInfo.cPoint = 0;
			gameOverInfo.isEngry = 0;
			if( m_nLevelType == LT_FRIEND )
			{
				gameOverInfo.expAward = GetExpAward( m_pPlayers[i]->GetLevel(), gameOverInfo.tarLevel, gameOverInfo.isWin );
			}
			else if( m_nLevelType == LT_PK_MASTER || m_nLevelType == LT_PK_APPRENTICE || m_nLevelType == LT_TRAIN )
			{
				gameOverInfo.expAward = GetExpMAPK( m_nLevelType, m_pPlayers[i]->GetLevel(), gameOverInfo.tarLevel, gameOverInfo.isWin );
			}
			else
			{
				gameOverInfo.expAward = 0;
			}
			if( m_nLevelType == LT_GRAB_APPRENTICE )
			{
				gameOverInfo.apprID = m_nTarApprID;
			}
			else
			{
				gameOverInfo.cardID = 0;
			}
			if( m_nLevelType == LT_GET_APPRENTICE || m_nLevelType == LT_GRAB_APPRENTICE )
			{
				gameOverInfo.canGetCard	= 0;
				gameOverInfo.consume	= 0;
			}
			gameOverInfo.keyID = 0;
		}
		else if( m_nLevelID == LEVEL_ID_PK )
		{
			gameOverInfo.cPoint = 0;
			gameOverInfo.isEngry = 0;
			if( m_nLevelType == LT_PK )
			{
				gameOverInfo.expAward = GetExpAward( m_pPlayers[i]->GetLevel(), gameOverInfo.tarLevel, gameOverInfo.isWin );
			}
			else
			{
				gameOverInfo.canGetCard	= 0;
				gameOverInfo.expAward	= 0;
			}
			gameOverInfo.cardID		= 0;
			gameOverInfo.keyID		= 0;
			gameOverInfo.consume	= 0;
		}
		else
		{
			gameOverInfo.cPoint		= g_levelMgr.GetCPoint( m_nLevelID );
			gameOverInfo.isEngry	= 1;
			gameOverInfo.expAward	= g_levelMgr.GetExpByLevelID( m_nLevelID, gameOverInfo.isWin );
			gameOverInfo.cardID		= levelInfo->GetCard();
			gameOverInfo.keyID		= levelInfo->GetKey();
		}
		if( gameOverInfo.expAward != 0 )
		{
			gameOverInfo.expAward += m_pPlayers[i]->GetAddExp();
		}

		DEBUG_LOG( "[%u] Get Exp [%d]", m_pPlayers[i]->GetUserID(), gameOverInfo.expAward );

		//notify DB and game over
		int wait = p->waitcmd;
		p->waitcmd = 0;
		sprite_t* tarSprite = p;
		if( m_pPlayers[i]->IsLeave() )
		{
			tarSprite = NULL;
		}
		else
		{
			m_nDBInfoCount++;
		}
		send_request_to_db( db_angel_fight_game_result, tarSprite, sizeof(gameOverInfo), &gameOverInfo, p->id );
		p->waitcmd = wait;
		
		//挑战好友时 给好友加经验
		if( m_nLevelID == LEVEL_ID_PVP )
		{
			gameOverInfo.cPoint		= 0;
			gameOverInfo.isWin		^= 0x1;
			gameOverInfo.tarLevel	= m_pPlayers[i]->GetLevel();
			gameOverInfo.tarID		= m_pPlayers[i]->GetUserID();
			gameOverInfo.canGetCard	= 0;
			gameOverInfo.keyID		= 0;
			gameOverInfo.cardID		= 0;
			gameOverInfo.consume	= 0;
			if( m_nLevelType == LT_FRIEND )
			{
				gameOverInfo.expAward = GetExpAward( pTar->GetLevel(), gameOverInfo.tarLevel, gameOverInfo.isWin );
			}
			else if( m_nLevelType == LT_PK_MASTER )
			{
				gameOverInfo.expAward = GetExpMAPK( LT_PK_APPRENTICE, pTar->GetLevel(), gameOverInfo.tarLevel, gameOverInfo.isWin );
			}
			else if( m_nLevelType == LT_PK_APPRENTICE || m_nLevelType == LT_TRAIN )
			{
				gameOverInfo.expAward = GetExpMAPK( LT_PK_MASTER, pTar->GetLevel(), gameOverInfo.tarLevel, gameOverInfo.isWin );
			}
			else
			{
				gameOverInfo.expAward = 0;
				continue;
			}
			gameOverInfo.expAward += pTar->GetAddExp();
			DEBUG_LOG( "[%u] Get Exp [%d]", pTar->GetUserID(), gameOverInfo.expAward );
			send_request_to_db( db_angel_fight_game_result, NULL, sizeof(gameOverInfo), &gameOverInfo, pTar->GetUserID() );
		}
	}
	if( 0 == m_nDBInfoCount )
	{
		return GER_end_of_game;
	}
	return 0;
}

int AngelFight::RoundBegin()
{
	//DEBUG_LOG( "AngelFight RoundBegin()" );

	for( uint32_t i = 0; i < PLAYER_COUNT; ++i )
	{
		m_pPlayers[i]->SetValidCmd( AFC_ACTION | AFC_QUIT );
		m_pPlayers[i]->ClearActionState();
	}

	if( 1 == m_nPlayerCount && !m_bQuickGame )
	{
		m_pPlayers[0]->SetValidCmd( AFC_ACTION | AFC_QUIT | AFC_QUICK_GAME );
	}

	if( !m_bQuickGame )
	{
		BeginTimer( TE_OPERATION, TIMER_ROUND );
		return NotifyRoundBegin();
	}
	else
	{
		return ActionAndNotify();
	}
}

int AngelFight::NotifyGameInfo()
{
	DEBUG_LOG( "AngelFight NotifyGameInfo()" );

	BeginTimer( TE_LOAD_DATA, TIMER_LOAD_DATA );

	m_nGameState = GS_WAIT_CLIENT;

	int len = sizeof(protocol_t);
	ant::pack( pkg, PLAYER_COUNT, len );
	for( uint32_t i = 0; i < PLAYER_COUNT; ++i )
	{
		m_pPlayers[i]->SetValidCmd( AFC_QUIT | AFC_DATA_OK );

		int count = m_pTeam[i]->GetTeamSize();
		ant::pack( pkg, m_pTeam[i]->GetTeamID(), len );
		ant::pack( pkg, count, len );
		Player* p = m_pTeam[i];
		while( p )
		{
			ant::pack( pkg, p->GetUserID(), len );
			ant::pack( pkg, p->GetMaxHP(), len );
			ant::pack( pkg, p->GetHP(), len );
			ant::pack( pkg, p->GetMaxMP(), len );
			ant::pack( pkg, p->GetMP(), len );
			ant::pack( pkg, p->GetLevel(), len );
			ant::pack( pkg, p->GetEquipID(), len );
			ant::pack( pkg, p->GetName(), MAX_NAME_LEN * sizeof(char), len );
			ant::pack( pkg, p->GetColor(), len );
			VecSkillID_t& skillInfo = p->GetSkillInfo();
			ant::pack( pkg, (int)skillInfo.size(), len );
			for( uint32_t j = 0; j < skillInfo.size(); ++j )
			{
				ant::pack( pkg, skillInfo[j].id, len );
			}
			p = p->GetPartner();
		}

	}
	init_proto_head( pkg, AFP_NOTIFY_GAME_INFO, len );
	send_to_players( m_pGrp, pkg, len );
	return 0;
}

void AngelFight::BeginTimer(TIMER_EVENT evt,uint32_t time)
{
	//DEBUG_LOG( "AngelFight::BeginTimer Event:[%d], Timer:[%u]second", int32_t(evt), time );
	if( !m_pGrp )
	{
		return;
	}
	m_nTimerEvent = evt;
	int val;
	m_nTimerID = add_timer_event(&m_timer, on_game_timer_expire, m_pGrp, &val, now.tv_sec + time );
	m_nTimerCounter = m_nPlayerCount;
	//DEBUG_LOG( "Add Timer Event OK ID:[%d]", m_nTimerID );
}

bool AngelFight::EndTimer( bool force /*= false*/ )
{
	//DEBUG_LOG( "AngelFight::EndTimer If Force[%d]", int32_t(force) );
	if( m_nTimerCounter > 0 )
	{
		m_nTimerCounter--;
	}
	else
	{
		m_nTimerCounter = 0;
	}

	if( ( 0 == m_nTimerCounter || force ) && m_nTimerID != 0 )
	{
		remove_timer( &m_timer, m_nTimerID );
		//DEBUG_LOG( "Remove Timer OK TimerID:[%d]", m_nTimerID);
		m_nTimerCounter = 0;
		m_nTimerID = 0;
	}

	if( 0 == m_nTimerID )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AngelFight::TestInitialize()
{
	DEBUG_LOG("AngelFight::TestInitialize()");
	m_nPlayerCount = 0;

	for( int i = 0; i < PLAYER_COUNT; ++i )
	{
		m_pPlayers[i] = new Player();
		m_pPlayers[i]->SetSprite( NULL );
	}

	m_pPlayers[0]->SetTar( m_pPlayers[1] );
	m_pPlayers[1]->SetTar( m_pPlayers[0] );

	m_nGameState = GS_WAIT_CLIENT;
	m_nDBInfoCount = 0;
}

void AngelFight::TestLoop()
{
	TestInitialize();
	DEBUG_LOG("AngelFight::Enter Main Loop");
	while( m_nWinnerTeam == 0 )
	{
		for( int i = 0; i < PLAYER_COUNT; ++i )
		{
			m_pPlayers[i]->ClearActionState();
		}

		for( int i = 0; i < PLAYER_COUNT; ++i )
		{
			m_pPlayers[i]->AutoAct();
		}
		DEBUG_LOG("AngelFight Current Player ACT Info");
		for( int i = 0; i < PLAYER_COUNT; ++i )
		{
			DEBUG_LOG("Player ID:[%u],Action:[%d],Param:[%d]", m_pPlayers[i]->GetUserID(), m_pPlayers[i]->GetActionState(), m_pPlayers[i]->GetASParam() );
		}

		ActionAndNotify();

		DEBUG_LOG("AngelFight Current Player ATTR Info");
		for( int i = 0; i < PLAYER_COUNT; ++i )
		{
			DEBUG_LOG("Player ID:[%u],HP:[%d],MP:[%d]", m_pPlayers[i]->GetUserID(), m_pPlayers[i]->GetHP(),m_pPlayers[i]->GetMP());
		}
	}
	DEBUG_LOG("AngelFight::Test Over");
}

int AngelFight::UnPackDataForTest(uint8_t * data,int32_t len)
{
	int32_t expectLen = sizeof(protocol_t);
	CHECK_BODY_LEN_GE( len, expectLen);
	DEBUG_LOG( "Packet Length:[%d]", len );
	protocol_t* pCmdHead = (protocol_t*)data;
	DEBUG_LOG( "Packet Head: Len[%d], Ver[%d], Cmd[%d], ID[%d], Ret[%d]",
		ntohl(pCmdHead->len), pCmdHead->ver,
		ntohl(pCmdHead->cmd), ntohl(pCmdHead->id), ntohl(pCmdHead->ret) );

	uint8_t* body = pCmdHead->body;
	if( !body )
	{
		DEBUG_LOG("Invalid Body");
		return 0;
	}

	int32_t dataLen = len - expectLen;
	int32_t subCmd;
	int32_t cmdLen;
	int j = 0;
	while( j < dataLen )
	{
		expectLen += sizeof(int32_t)*2;
		CHECK_BODY_LEN_GE( len, expectLen );
		ant::unpack( body, subCmd, j );
		ant::unpack( body, cmdLen, j );
		DEBUG_LOG( "Sub Cmd:[%d], Len:[%d]", subCmd, cmdLen );
		switch( subCmd )
		{
		case PC_ATTACK:
		case PC_COMBO:
			{
				AttackInfo info;
				ant::unpack( body, info.m_isBlock, j );
				ant::unpack( body, info.m_isCritical,j );
				int damage;
				ant::unpack( body, damage, j );

				info.m_nDamage = damage;

				DEBUG_LOG( "Damage:[%d], IsBlock:[%d], IsCritical:[%d]", info.m_nDamage, info.m_isBlock, info.m_isCritical );
			}
			break;
		default:
			{
				j += cmdLen - sizeof(int32_t)*2;
			}
			 break;
		}

	}
	return 0;
}

int AngelFight::SelectMonster( Player* player )
{
	DEBUG_LOG( "Select Monster OK" );
	if( m_nLevelID != LEVEL_ID_PVP )
	{
		int32_t monster_id[4];
		int32_t monster_count;
		g_levelMgr.GetCreatureIDByLevelID( m_nLevelID, monster_id, monster_count );
		if( monster_count == 0 )
		{
			return send_to_player_err( player->GetSprite(), player->GetSprite()->waitcmd, -AFE_INVALID_TASK_ID );
		}

		Player* monster = m_pTeam[1];
		for( int i = 0; i < monster_count; ++i )
		{
			CreatureInfo* monster_info = g_creatureInfoMgr.GetCreatureInfo( monster_id[i] );
			if( !monster_info )
			{
				ERROR_LOG(" Get Creature Info Failed Creature ID:[%d]", monster_id[i] );
				continue;
			}

			if( !monster )
			{
				monster = new Player();
				m_pTeam[1]->AddPartner( monster );
			}
			monster->SetInfo( monster_info );
			monster = NULL;
		}
		return NotifyGameInfo();
	}
	else
	{
		m_nGameState = GS_WAIT_DB_FRIEND;
		uint32_t wait = player->GetSprite()->waitcmd;
		player->GetSprite()->waitcmd = 0;
		send_request_to_db( db_angel_fight_get_user_info, player->GetSprite(), 0, NULL, m_nFriendID );
		player->GetSprite()->waitcmd = wait;
		return 0;
	}
}
