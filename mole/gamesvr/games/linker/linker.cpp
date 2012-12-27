/*
 * =====================================================================================
 *
 *       Filename:  linker.cpp
 *
 *    Description
 *
 *        Version:  1.0
 *        Created:  01/10/2012 02:32:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee (), ericlee@taomee.com
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

#include "stdlib.h"
#include <vector>
#include "linker.h"

#define MAP3X3_GAME_ID						91
#define MAP5X5_GAME_ID						92
#define LINKER_GAME_MAX_CHESS_CNT			25		//每方每局最多能使用的棋子数
#define LINKER_GAME_AWARD_STEP_CNT			3		//每走指定次数生成一个随机奖励物品
#define LINKER_GAME_PLAYER_SIT_TIMER		(32)
#define LINKER_GAME_RESOUCE_LOAD_TIMER		30
#define LINKER_GAME_WINTER_CHESS_ITEM_ID	1351289
#define LINKER_GAME_SPRING_CHESS_ITEM_ID	1351290

struct AwardInfo
{
	uint32_t itemID;
	uint32_t itemCnt;
};

//随机奖励物品
static AwardInfo randAward[] = {
	{0, 5},
	{0, 10},
	{0, 20},
	{0, 30},
	{0, 50},
	{0, 100},
// 	{1351289, 10},
// 	{1351290, 10},
// 	{1351289, 25},
// 	{1351290, 25},
// 	{1351289, 50},
// 	{1351290, 50}
};
static uint32_t randItemAward[] = { 1351294, 1351295, 1351298, 1351299 };
static uint32_t randItemCnt = sizeof(randItemAward) / sizeof(uint32_t);

Linker::Linker( game_group_t* grp )
{
	INIT_LIST_HEAD( &m_timer );
	m_pGrp = grp;

	if( !m_pGrp )
	{
		return;
	}
	switch( m_pGrp->game->id )
	{
	case MAP3X3_GAME_ID:
		{
			m_nMapSize = 3;
		}
		break;
	case MAP5X5_GAME_ID:
		{
			m_nMapSize = 5;
		}
		break;
	default:
		{
			m_nMapSize = 7;
		}
	}

	m_pMap = new char[m_nMapSize*m_nMapSize];
	m_pTestList = new char[m_nMapSize*m_nMapSize/2];
	m_cStepCnt = 0;
}
Linker::~Linker()
{
	EndTimer(true);
	delete m_pMap;
	m_pMap = NULL;
	delete m_pTestList;
	m_pTestList = NULL;
}

void Linker::init( sprite_t* p )
{
	DEBUG_LOG( "Linker Game Ini" );
	//ini map info
	memset( m_pMap, CF_BLANK, m_nMapSize*m_nMapSize );
	m_cTestListCnt = 0;
	//set flag by protocal
	for( int i = 0; i < m_pGrp->count; ++i )
	{
		if( m_pGrp->players[i]->pos_id == 1 )
		{
			m_player[0].m_pSprite = m_pGrp->players[i];
			m_player[0].m_cFlag = CF_WINTER;
		}
		else
		{
			m_player[1].m_pSprite = m_pGrp->players[i];
			m_player[1].m_cFlag = CF_SPRING;
		}
		memset( m_player[i].m_bHasItem, 0, sizeof(m_player[i].m_bHasItem) );
	}
	m_nGameState = GS_INI;
	//get chess count from db

	int j = 0;
	uint8_t buff[128] = {0};
	for( int i = 0; i < 2; ++i )
	{
		m_player[i].m_nGoldAdd = 0;
		m_player[i].m_bGetRes = false;
		m_player[i].m_nChessCntCur = 0;
		m_player[i].m_nChessCntIni = 0;
		m_player[i].m_nChessUsed = 0;

		j = 0;
		int dbItemCnt = randItemCnt + 1;
		ant::pack_h( buff, (int)2, j );
		ant::pack_h( buff, dbItemCnt, j );

		ant::pack_h( buff, (int)LINKER_GAME_WINTER_CHESS_ITEM_ID + m_player[i].m_cFlag - 1, j );
		ant::pack_h( buff, (int)99, j );

		for( uint32_t itemIdx = 0; itemIdx < randItemCnt; ++itemIdx )
		{
			ant::pack_h( buff, (int)randItemAward[itemIdx], j );
			ant::pack_h( buff, (int)99, j );
		}
		send_request_to_db( db_proto_get_item_array, m_player[i].m_pSprite, j, buff, m_player[i].m_pSprite->id );
	}
	m_nDBRetCntNeed = m_pGrp->count;
}

int Linker::handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code )
{
	DEBUG_LOG( "Linker Game Get DB return ID:[%u]", id );
	switch( m_nGameState )
	{
	case GS_INI:
		{
			//set player info
			PlayerInfo* player = GetPlayer(p);
			if( player )
			{
				int j = 0;
				int count;
				uint32_t itemID;
				uint32_t itemCnt = 0;
				int expLen = sizeof(count);
				CHECK_BODY_LEN_GE( len, expLen );
				ant::unpack( buf, &count, sizeof(count), j );

				expLen += count * ( sizeof(itemID) + sizeof(itemCnt) );
				CHECK_BODY_LEN( len, expLen );
				for( int i = 0; i < count; ++i )
				{
					ant::unpack( buf, &itemID, sizeof(itemID), j );
					ant::unpack( buf, &itemCnt, sizeof(itemCnt), j );
					//DEBUG_LOG( "Player:[%u] Flag:[%d] ItemID:[%u} ItemCnt:[%u]", player->m_pSprite->id, player->m_cFlag, itemID, itemCnt );
					if( itemID == uint32_t(LINKER_GAME_WINTER_CHESS_ITEM_ID + player->m_cFlag - 1) )
					{
						player->m_nChessCntIni = itemCnt;
						player->m_nChessCntCur = itemCnt;
					}
					else
					{
						for( uint32_t k = 0; k < randItemCnt; ++k )
						{
							if( randItemAward[k] == itemID )
							{
								player->m_bHasItem[k] = true;
								break;
							}
						}
					}
				}
				if( player->m_nChessCntCur < 25 )
				{
					player->m_nChessCntCur = 25;
				}
				player->m_nChessUsed = 0;
			}
			else
			{
				player->m_nChessCntCur = 25;
				player->m_nChessCntIni = 0;
				player->m_nChessUsed = 0;
				ERROR_LOG( "Can not get Player Ptr UserID;[%u]", p->id );
			}
			m_nDBRetCntNeed--;
			if( 0 == m_nDBRetCntNeed )
			{
				SendGameInfo();
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

int Linker::handle_data( sprite_t* p, int cmd, const uint8_t body[], int len )
	{
	//DEBUG_LOG( "Linker Game Get Player Cmd" );
	PlayerInfo* player = GetPlayer(p);
	if( !player )
	{
		ERROR_LOG( "SYSTEM ERR Invalid Sprite ID:[%u]", p->id );
		return GER_game_system_err;
	}

	if( cmd == proto_player_leave )
	{
		return GameOver( player->m_cFlag == CF_WINTER ? W_SPRING : W_WINTER, RR_LEAVE );
	}
	else if( cmd != proto_linker_game_cmd )
	{
		//Invlid Cmd
		return 0;
	}
	int localCmd;
	int localMsgLen;
	const uint8_t* localMsgBuf;
	CHECK_BODY_LEN_GE( (uint32_t)len, sizeof(localCmd) );

	localCmd = ntohl(*(int*)body);
	localMsgBuf = body + sizeof(localCmd);
	localMsgLen = len - sizeof(localCmd);

	//DEBUG_LOG( "Player:[%u] Cmd:[%u]", p->id, localCmd );
	switch( m_nGameState )
	{
	case GS_INI:
		break;
	case GS_RESOUCE_LOAD:
		{
			switch( localCmd )
			{
			case LGP_LOAD_OK:
				{
					PlayerInfo* player = GetPlayer(p);
					if( player && !player->m_bGetRes )
					{
						player->m_bGetRes = true;
						m_cResloadCnt--;
						if( 0 == m_cResloadCnt )
						{
							GameStart();
						}
					}
				}
				break;
			default:
				break;
			}
		}
		break;
	case GS_INGAME:
		{
			switch( localCmd )
			{
			case LGP_SITTING:
				{
					if( player->m_cFlag == m_cCurOperator )
					{
						int sitPos;
						CHECK_BODY_LEN( localMsgLen, sizeof(sitPos) );
						int j = 0;
						ant::unpack( localMsgBuf, sitPos, j );
						return Sitting( player, sitPos );
					}
				}
				break;
			case LGP_WIN:
				{
					if( player->m_cFlag == (char)CheckGameOver() )
					{
						return GameOver( player->m_cFlag );
					}
					else
					{
						return GameOver( player->m_cFlag % 2 + 1 );
					}
				}
				break;
			default:
				break;
			}
		}
		break;
	case GS_OVER:
		break;
	default:
		break;
	}
	return 0;
}

int Linker::handle_timeout( void* data )
{
	EndTimer(true);
	switch(m_nTimerEvent)
	{
	case TE_PLAYER_SIT:
		{
			return GameOver( CF_WINTER == m_cCurOperator ? W_SPRING : W_WINTER , RR_TIMEOUT );
		}
		break;
	case TE_PLAYER_LOAD:
		{
			GameStart();
			return 0;
		}
		break;
	default:
		break;
	}
	return 0;
}

void Linker::SendGameInfo()
{
	//begin timer wait player load resouces
	BeginTimer( TE_PLAYER_LOAD, LINKER_GAME_RESOUCE_LOAD_TIMER );
	m_nGameState = GS_RESOUCE_LOAD;
	m_cResloadCnt = 2;
	//notice Game Info
	int localCmd = LGP_GAME_INFO;
	int msgLen = sizeof(protocol_t);
	ant::pack( pkg, localCmd, msgLen );
	ant::pack( pkg, m_player[0].m_pSprite->id, msgLen );
	ant::pack( pkg, m_player[1].m_pSprite->id, msgLen );
	init_proto_head( pkg, proto_linker_game_cmd, msgLen );
	send_to_players( m_pGrp, pkg, msgLen );
	//DEBUG_LOG( "Send Game Info" );
}

void Linker::GameStart()
{
//	DEBUG_LOG( "P1 Ini:[%d] Cur:[%d] P2 Ini:[%d] Cur:[%d]", m_player[0].m_nChessCntIni, m_player[0].m_nChessCntCur, m_player[1].m_nChessCntIni, m_player[1].m_nChessCntCur);
	//关闭等待玩家加载资源的定时器
	EndTimer(true);
	//确定先手方
	m_cCurOperator = ( rand() % 2 ) + 1;
	//begin timer wait player load resouces
	BeginTimer( TE_PLAYER_SIT, LINKER_GAME_PLAYER_SIT_TIMER );
	m_nGameState = GS_INGAME;
	//notice Game Start
	int localCmd = LGP_GAME_START;
	int msgLen = sizeof(protocol_t);
	ant::pack( pkg, localCmd, msgLen );
	ant::pack( pkg, m_player[0].m_pSprite->id, msgLen );
	ant::pack( pkg, m_player[1].m_pSprite->id, msgLen );
	ant::pack( pkg, (int)m_cCurOperator, msgLen );
	ant::pack( pkg, m_player[0].m_nChessCntCur, msgLen );
	ant::pack( pkg, m_player[1].m_nChessCntCur, msgLen );
	init_proto_head( pkg, proto_linker_game_cmd, msgLen );
	send_to_players( m_pGrp, pkg, msgLen );
	DEBUG_LOG( "Linker Game Start ====[%u] VS [%u]====", m_player[0].m_pSprite->id, m_player[1].m_pSprite->id );
}

int Linker::Sitting( PlayerInfo* player, int pos )
{
//	DEBUG_LOG( "Player:[%u], Sitting Pos:[%d]", player->m_pSprite->id, pos );
	if( pos < 0 || pos > m_nMapSize * m_nMapSize || m_pMap[pos] == CF_WINTER || m_pMap[pos] == CF_SPRING )
	{
//		DEBUG_LOG( "Player:[%u], Sitting Pos:[%d] Invalid Pos", player->m_pSprite->id, pos );
		//invalid pos
		return 0;
	}

	//如果当前格子里面有物品则拾取
	if( m_pMap[pos] >= CF_GOLD200 && m_pMap[pos] <= CF_CHESS_S_50 )
	{
		uint32_t itemID = randAward[(int)m_pMap[pos] - CF_GOLD200].itemID;
		uint32_t itemCnt = randAward[(int)m_pMap[pos] - CF_GOLD200].itemCnt;
		if( itemID == 0 )
		{
			player->m_nGoldAdd += itemCnt;
		}
		else
		{
			//只能拾取己方的棋子
			if( (char)(itemID - 1351289 + 1 ) == player->m_cFlag )
			{
				player->m_nChessCntCur += itemCnt;
			}
			else
			{
				itemCnt = 0;
			}
		}
		//notice player get item
//		DEBUG_LOG( "Get Item ID:[%u] Cnt:[%u]", itemID, itemCnt );
		if( itemCnt > 0 )
		{
			int localCmd = LGP_GET_AWARD;
			int msgLen = sizeof(protocol_t);
			ant::pack( pkg, localCmd, msgLen );
			ant::pack( pkg, itemID, msgLen );
			ant::pack( pkg, itemCnt, msgLen );
			init_proto_head( pkg, proto_linker_game_cmd, msgLen );
			send_to_self( player->m_pSprite, pkg, msgLen, 0 );
		}
	}
	else if( m_pMap[pos] != CF_BLANK )
	{
		DEBUG_LOG( "Player:[%u] Sit Invalid Pos:[%d]", player->m_pSprite->id, pos );
		return 0;
	}

	m_pMap[pos] = player->m_cFlag;
	player->m_nChessCntCur--;
	player->m_nChessUsed++;

	//每三次操作生成一个随机奖励
	m_cStepCnt++;
	if( m_cStepCnt >= LINKER_GAME_AWARD_STEP_CNT )
	{
		m_cStepCnt = 0;
		CreateRandomAward();
	}

	//broadcasts to players
	int localCmd = LGP_SITTING;
	int msgLen = sizeof(protocol_t);
	ant::pack( pkg, localCmd, msgLen );
	ant::pack( pkg, (int)player->m_cFlag, msgLen );
	ant::pack( pkg, pos, msgLen );
	init_proto_head( pkg, proto_linker_game_cmd, msgLen );
	send_to_players( m_pGrp, pkg, msgLen );

	EndTimer(true);

	PlayerInfo* other;
	if( player == &m_player[0] )
	{
		other = &m_player[1];
	}
	else
	{
		other = &m_player[0];
	}

	//如果先手方占满棋盘上最后一子则主动判定胜负
	if( player->m_nChessUsed == m_nMapSize * m_nMapSize / 2 + 1 )
	{
		return GameOver( CheckGameOver() );
	}
	//如果对方玩家棋子用光，则判己方赢
	if( 0 == other->m_nChessCntCur )
	{
//		DEBUG_LOG( "Player:[%u] Does Not have Enough Chess To Play", other->m_pSprite->id );
		return GameOver( player->m_cFlag % 2 + 1 );
	}

	//wait the other player siting
	m_cCurOperator++;
	if( m_cCurOperator > CF_SPRING )
	{
		m_cCurOperator = CF_WINTER;
	}
	BeginTimer( TE_PLAYER_SIT, LINKER_GAME_PLAYER_SIT_TIMER );
	return 0;
}
/*
			 ___
   winter___/ 02\___spring
	 ___/ 01\___/ 02\___
	/ 00\___/ 11\___/ 22\
	\___/ 10\___/ 21\___/
  spring\___/ 20\___/winter
			\___/
*/

//format {y,x}
static char sideOffset[][2] = {
	{-1, 0},	{-1, 1},
	{0, -1},	{0, 1},
	{1, -1},	{1, 0}
};

enum IS_PASS_RET
{
	IPR_ERROR	= -1,
	IPR_PASS,		//到达对岸
	IPR_END,		//没有相邻点
};

int Linker::IsPass( CELL_FLAG flag, int pos )
{
	DEBUG_LOG( "IsPass Flag:[%u], Pos:[%d]", flag, pos );
	int x = pos % m_nMapSize;
	int y = pos / m_nMapSize;
	if( y < 0 || y > m_nMapSize )
	{
		ERROR_LOG( "Linker Check IsPass Invalid Position:[%d]", pos );
		return -1;
	}
	int ret = IPR_END;
	switch( flag )
	{
	case CF_WINTER:
		if( m_nMapSize - 1 == y )
		{
			return IPR_PASS;
		}
		break;
	case CF_SPRING:
		if( m_nMapSize - 1 == x )
		{
			return IPR_PASS;
		}
		break;
	default:
		break;
	}
	//sign
	int testPosList[6];
	int testPosCnt = 0;
	for( int i = 0; i < 6; ++i )
	{
		int curX = x + sideOffset[i][1];
		int curY = y + sideOffset[i][0];
		int testPos = curY * m_nMapSize + curX;
		if( ( curX >= 0 && curX < m_nMapSize && curY >= 0 && curY < m_nMapSize ) && m_pMap[testPos] == flag )
		{
			m_pMap[testPos] |= CF_BE_ADD;
			testPosList[testPosCnt++] = testPos;
		}
		else
		{
			continue;
		}
	}
	//check
	for( int i = 0; i < testPosCnt; ++i )
	{
		ret = IsPass( flag, testPosList[i] );
		//出现错误或者已经联通则返回
		if( IPR_END != ret )
		{
			TestAdd(testPosList[i]);
			return ret;
		}
	}
	return ret;
}

int Linker::CheckGameOver()
{
	DEBUG_LOG( "Check Game Over" );
	for( int i = 0; i < m_nMapSize; ++i )
	{
		DEBUG_LOG( "[%d],[%d],[%d],[%d],[%d],[%d],[%d],", 
			(int)m_pMap[i * m_nMapSize + 0],
			(int)m_pMap[i * m_nMapSize + 1],
			(int)m_pMap[i * m_nMapSize + 2],
			(int)m_pMap[i * m_nMapSize + 3],
			(int)m_pMap[i * m_nMapSize + 4],
			(int)m_pMap[i * m_nMapSize + 5],
			(int)m_pMap[i * m_nMapSize + 6] );
	}
	m_cTestListCnt = 0;
	WINNER winner = W_HAVNT_OVER;
	int pos;
	//for spring
	for( int y = 0; y < m_nMapSize; ++y )
	{
		pos = y * m_nMapSize;
		if( m_pMap[pos] == CF_SPRING )
		{
			m_pMap[pos] |= CF_BE_ADD;
		}
		else
		{
			continue;
		}
		//未联通则清空测试缓存
		if( IPR_PASS != IsPass( CF_SPRING, pos ) )
		{
 			TestListClear();
		}
		else
		{
			TestAdd(pos);
			DEBUG_LOG( "Spring Is Winner, The Chess Info..." );
			for( int i = 0; i < m_cTestListCnt; ++i )
			{
				DEBUG_LOG( "[%d][%d]:Value[%d]", m_pTestList[i]/m_nMapSize, m_pTestList[i]%m_nMapSize, m_pMap[(int)m_pTestList[i]] );
			}
			winner = W_SPRING;
			break;
		}
	}
	if( winner == W_HAVNT_OVER )
	{
		//for winter
		for( int x = 0; x < m_nMapSize; ++x )
		{
			pos = x;
			if( m_pMap[pos] == CF_WINTER )
			{
				m_pMap[pos] |= CF_BE_ADD;
			}
			else
			{
				continue;
			}

			if( IPR_PASS != IsPass( CF_WINTER, pos ) )
			{
				TestListClear();
			}
			else
			{
				TestAdd(pos);
				DEBUG_LOG( "Winter Is Winner, The Chess Info..." );
				for( int i = 0; i < m_cTestListCnt; ++i )
				{
					DEBUG_LOG( "[%d][%d]:Value[%d]", m_pTestList[i]/m_nMapSize, m_pTestList[i]%m_nMapSize, m_pMap[(int)m_pTestList[i]]&~CF_BE_ADD );
				}
				winner = W_WINTER;
				break;
			}
		}
	}
	//清空标志位
	for( int i = 0; i < m_nMapSize*m_nMapSize; ++i )
	{
		m_pMap[i] &= ~CF_BE_ADD;
	}
	//没有分出胜负时判定棋子是不是用完 用完则判对方赢
	if( W_HAVNT_OVER == winner )
	{
		for( int i = 0; i < 2; ++i )
		{
			if( m_player[i].m_nChessUsed >= LINKER_GAME_MAX_CHESS_CNT )
			{
				winner = WINNER( m_player[(i + 1) % 2].m_cFlag );
				break;
			}
		}
	}

	return winner;
}

int Linker::GameOver( int winner, int reason )
{
	EndTimer(true);
	DEBUG_LOG( "Linker Game ====[%u] VS [%u]==== Over Winner Is [%d]", m_player[0].m_pSprite->id, m_player[1].m_pSprite->id, winner );
	m_nGameState = GS_OVER;
	uint8_t dbBuf[128] = {0};
	int length;
	for( int i = 0; i < m_pGrp->count; ++i )
	{
		length = 0;
		if( !m_player[i].m_pSprite )
		{
			continue;
		}

		int removeCnt = 0;
		int addCnt = 0;
		int goldModify = m_player[i].m_nGoldAdd;
		if( m_player[i].m_cFlag == (char)winner )
		{
			goldModify += m_player[(i + 1) % 2].m_nChessUsed;
		}
		else
		{
			goldModify -= m_player[i].m_nChessUsed;
		}
		goldModify *= 10;

		if( goldModify > 0 )
		{
			addCnt = 1;
		}
		else if( goldModify < 0 )
		{
			removeCnt = 1;
		}

		if( goldModify != 0 )
		{
			ant::pack_h( dbBuf, removeCnt, length );
			//add cnt
			ant::pack_h( dbBuf, addCnt, length );
			//reason 1
			ant::pack_h( dbBuf, 0, length );
			//reason 2
			ant::pack_h( dbBuf, 0, length );
			if( addCnt == 1 )
			{
				ant::pack_h( dbBuf, 0, length );
				ant::pack_h( dbBuf, 0, length );
				ant::pack_h( dbBuf, goldModify, length );
				ant::pack_h( dbBuf, 99999, length );
			}
			else
			{
				ant::pack_h( dbBuf, 0, length );
				ant::pack_h( dbBuf, 0, length );
				ant::pack_h( dbBuf, -goldModify, length );
			}
			send_request_to_db( db_proto_modify_items, NULL, length , dbBuf, m_player[i].m_pSprite->id );
		}

		/****已下架***/
// 		uint32_t chessItemID = ( m_player[i].m_cFlag == CF_WINTER ? LINKER_GAME_WINTER_CHESS_ITEM_ID : LINKER_GAME_SPRING_CHESS_ITEM_ID );
// 		int chessModify = m_player[i].m_nChessCntCur - m_player[i].m_nChessCntIni;
// 
// 		//赢家可拿走棋盘上所有棋子
// 		if( m_player[i].m_cFlag == (char)winner )
// 		{
// 			chessModify += m_player[0].m_nChessUsed + m_player[1].m_nChessUsed;
// 		}
// 		//DEBUG_LOG( "Player:[%u] Flag[%d] ItemID:[%u] CurChess Cnt:[%u] Modify:[%d]", m_player[i].m_pSprite->id, m_player[i].m_cFlag, chessItemID, m_player[i].m_nChessCntCur, chessModify );
// 		//无论胜负，双方赠送随机物品	
// 		uint32_t randID = rand() % ( sizeof(randItemAward) / sizeof(uint32_t) );
// 		uint32_t awardID = randItemAward[randID];
// 		uint32_t awardCnt = 1;
// 		int removeCnt = 0;
// 		int addCnt = 0;
// 		if( m_player[i].m_nGoldAdd > 0 )
// 		{
// 			addCnt++;
// 		}
// 		if( chessModify > 0 )
// 		{
// 			addCnt++;
// 		}
// 		else if( chessModify < 0 )
// 		{
// 			removeCnt++;
// 		}
// 		//没有获取过就送一个
// 		if( !m_player[i].m_bHasItem[randID] )
// 		{
// 			addCnt++;
// 		}
// 		else
// 		{
// 			awardCnt = 0;
// 		}
// 		ant::pack_h( dbBuf, removeCnt, length );
// 		//add cnt
// 		ant::pack_h( dbBuf, addCnt, length );
// 		//reason 1
// 		ant::pack_h( dbBuf, 203, length );
// 		//reason 2
// 		ant::pack_h( dbBuf, 0, length );
// 		//棋子变化
// 		if( chessModify < 0 )
// 		{
// 			//DEBUG_LOG( " Reduce Player:[%u] CurChess Cnt:[%u] Modify:[%d]", m_player[i].m_pSprite->id, m_player[i].m_nChessCntCur, chessModify );
// 			ant::pack_h( dbBuf, 99, length );
// 			ant::pack_h( dbBuf, chessItemID, length );
// 			ant::pack_h( dbBuf, -chessModify, length );
// 		}
// 		else if( chessModify > 0 )
// 		{
// 			//DEBUG_LOG( " Add Player:[%u] CurChess Cnt:[%u] Modify:[%d]", m_player[i].m_pSprite->id, m_player[i].m_nChessCntCur, chessModify );
// 			ant::pack_h( dbBuf, 99, length );
// 			ant::pack_h( dbBuf, chessItemID, length );
// 			ant::pack_h( dbBuf, chessModify, length );
// 			ant::pack_h( dbBuf, 99999, length );
// 		}
// 		//摩尔豆获取
// 		if( m_player[i].m_nGoldAdd > 0 )
// 		{
// 			ant::pack_h( dbBuf, 0, length );
// 			ant::pack_h( dbBuf, 0, length );
// 			ant::pack_h( dbBuf, m_player[i].m_nGoldAdd, length );
// 			ant::pack_h( dbBuf, 999999999, length );
// 		}
// 		//没有获取过就送一个
// 		if( !m_player[i].m_bHasItem[randID] )
// 		{
// 			ant::pack_h( dbBuf, 99, length );
// 			ant::pack_h( dbBuf, awardID, length );
// 			ant::pack_h( dbBuf, awardCnt, length );
// 			ant::pack_h( dbBuf, 1, length );
// 			send_request_to_db( db_proto_modify_items, NULL, length , dbBuf, m_player[i].m_pSprite->id );
// 		}

		int awardID = 0;
		int awardCnt = 0;
		//向玩家发送比赛结果
		int localCmd = LGP_GAMEOVER;
		int msgLen = sizeof(protocol_t);
		ant::pack( pkg, localCmd, msgLen);
		ant::pack( pkg, winner, msgLen);
		ant::pack( pkg, awardID, msgLen);
		ant::pack( pkg, awardCnt, msgLen);
		ant::pack( pkg, reason, msgLen);
		ant::pack( pkg, (int)m_cTestListCnt, msgLen);
		for( int posIdx = 0; posIdx < (int)m_cTestListCnt; ++posIdx )
		{
			ant::pack( pkg, (int)m_pTestList[posIdx], msgLen);
		}
		init_proto_head( pkg, proto_linker_game_cmd, msgLen );
		send_to_self( m_player[i].m_pSprite, pkg, msgLen, 0 );
	}

	//modify team power
	//活动下架不再影响队伍能量
// 	length = sizeof(int) * 2;
// 	int* w_add = (int*)dbBuf;
// 	int* s_add = (int*)(dbBuf + sizeof(int) );
// 	if( winner == W_WINTER )
// 	{
// 		*w_add = 10;
// 		*s_add = 0;
// 	}
// 	else
// 	{
// 		*w_add = 0;
// 		*s_add = 10;
// 	}
// 	send_request_to_db( db_proto_modify_team_power, NULL, length , dbBuf, 0 );
	
	return GER_end_of_game;
}

int Linker::GetGameInfo(sprite_t* p)
{
	if( !p || (!IS_GAME_WATCHER(p) && !IS_GAME_PLAYER(p)) )
	{
		ERROR_LOG( "Linker Game Invalid User Get Game Info UserID:[%u]", p->id );
		return 0;
	}
	int msgLen = sizeof(protocol_t);
	ant::pack( pkg, m_nMapSize, msgLen );
	for( int i = 0; i < 2; ++i )
	{
		ant::pack( pkg, m_player[i].m_cFlag, msgLen );
		ant::pack( pkg, m_player[i].m_nChessCntCur, msgLen );
		ant::pack( pkg, m_player[i].m_nChessUsed, msgLen );
	}
	ant::pack( pkg, m_pMap, sizeof(char) * m_nMapSize * m_nMapSize, msgLen );
	init_proto_head( pkg, LGP_GAME_INFO, msgLen );
	return send_to_player( p, pkg, msgLen );
}

inline void Linker::TestAdd( char pos )
{
	m_pTestList[(int)m_cTestListCnt] = pos;
	m_cTestListCnt++;
}

inline void Linker::TestListClear()
{
	memset( m_pTestList, 0, sizeof(char) * m_cTestListCnt );
	m_cTestListCnt = 0;
}

void Linker::CreateRandomAward()
{
	if( m_player[0].m_nChessUsed + m_player[1].m_nChessUsed == m_nMapSize * m_nMapSize )
	{
		DEBUG_LOG( "CreateRandomAward Board is full" );
		return;
	}
	int pos;
	pos = rand() % ( m_nMapSize * m_nMapSize );
	int count = 0;
	while ( m_pMap[pos] != 0 )
	{
		pos++;
		pos %= ( m_nMapSize * m_nMapSize );
		count++;
		if( count >= m_nMapSize*m_nMapSize )
		{
			break;
		}
	}
	if( m_pMap[pos] == 0 )
	{
		m_pMap[pos] = CF_GOLD200 + rand() % ( CF_CHESS_S_50 - CF_GOLD200 );

		//DEBUG_LOG( "Random Item Pos:[%d], Item:[%d]", pos, m_pMap[pos] );
		//notice to players
		int localCmd = LGP_RAND_AWARD;
		int msgLen = sizeof(protocol_t);
		ant::pack( pkg, localCmd, msgLen );
		ant::pack( pkg, pos, msgLen );
		ant::pack( pkg, (int)m_pMap[pos], msgLen );
		init_proto_head( pkg, proto_linker_game_cmd, msgLen );
		send_to_players( m_pGrp, pkg, msgLen );
	}
}

void Linker::BeginTimer(TimerEvent evt, uint32_t time)
{
	if( !m_pGrp )
	{
		return;
	}
	m_nTimerEvent = evt;
	int val;
	m_nTimerID = add_timer_event(&m_timer, on_game_timer_expire, m_pGrp, &val, now.tv_sec + time );
	m_nTimerCounter = 1;
	//DEBUG_LOG( "BeginTimer TimerID:{%u]", m_nTimerID );
}

bool Linker::EndTimer( bool force /*= false*/ )
{
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
		//DEBUG_LOG( "EnderTimer TimerID:{%u]", m_nTimerID );
		remove_timer( &m_timer, m_nTimerID );
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

Linker::PlayerInfo* Linker::GetPlayer( sprite_t* p )
{
	if( p )
	{
		for( int i = 0; i < 2; ++i )
		{
			if( m_player[i].m_pSprite == p )
			{
				return &m_player[i];
			}
		}
	}
	return NULL;
}
