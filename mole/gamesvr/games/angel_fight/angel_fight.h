/*
 * =====================================================================================
 *
 *       Filename:  angel_fight.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/08/2011 05:56:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef ANGEL_FIGHT_H
#define ANGEL_FIGHT_H

#include "../mpog.hpp"

#define PLAYER_COUNT	2

enum ANGEL_FIGHT_PROTO
{
	AFP_CMD_HEAD				= 30500,
	AFP_NOTIFY_ACTION			= 30501,
	AFP_NOTIFY_GAME_INFO		= 30502,
	AFP_NOTIFY_ROUND_BEGIN		= 30503,
	AFP_NOTIFY_GAME_OVER		= 30504,
	AFP_NOTIFY_CHANGE_PLAYER	= 30505,
	AFP_NOTIFY_AWARD			= 30506,
	AFP_DB_OK					= 30507,
	AFP_PLAYER_LEAVE			= 30508,
};

enum ANGEL_FIGHT_CMD
{
	AFC_ACTION			= (1<<1),
	AFC_SELECT_MONSTER 	= (1<<2),
	AFC_QUIT			= (1<<3),
//	AFC_REMOVE_CARD		= (1<<4),
//	AFC_GET_AWARD		= (1<<5),
	AFC_DATA_OK			= (1<<6),
	AFC_QUICK_GAME		= (1<<7),
	AFC_ANI_OK			= (1<<8),
};

enum TIMER_EVENT
{
	TE_OPERATION		= 1,
	TE_SELECT_MONSTER,
	TE_WAIT_ANI,
	TE_LOAD_DATA,
	TE_CHANGE_PLAYER,
};

enum GAME_STATE
{
	GS_WAIT_DB		= 1,
	GS_WAIT_CLIENT,
	GS_WAIT_DB_FRIEND,
	GS_WAIT_DB_LEVEL,
	GS_OVER
};

enum GAME_TYPE
{
	GT_FRIEND		= 0,	//好友对战
	GT_MONSTER,				//挑战怪物
	GT_GET_APPRENTICE,		//收徒战斗
	GT_GRAB_APPR,			//抢徒弟，挑战对方的师傅
	GT_PK_APPR,				//挑战徒弟
	GT_PK_MASTER,			//挑战师傅
	GT_TRAIN,				//师徒切磋
};

class Player;

int32_t GetExpAward( int32_t levelSelf, int32_t levelTar, bool isWin );
int32_t GetExpMAPK( LEVEL_TYPE lt, int32_t levelSelf,int32_t levelTar, bool isWin );
void InitializeExpAward();

class AngelFight:public mpog
{
public:
	int		handle_data( sprite_t* p, int cmd, const uint8_t body[], int len );
	int		handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code );
	int		handle_timeout( void* data );
	void	init( sprite_t* p );

	AngelFight( game_group_t* grp );
	AngelFight( game_group_t* grp, Player* p1, Player* p2 );
	~AngelFight();

	void	TestLoop();
private:

	int		OnAction( Player* p, const uint8_t buf[], int len );
	int		OnSelectMonster( Player* p, const uint8_t buf[], int len );
	int		OnQuit( Player* p, const uint8_t buf[], int len );
	int		OnLoadDataOK( Player* p, const uint8_t buf[], int len );
	int		OnAniOK( Player* p, const uint8_t buf[], int len );
	int		OnQuickGame( Player* p, const uint8_t buf[], int len );

	int		UnPackDataForTest( uint8_t* data, int32_t len );
	void	TestInitialize();

	//根据sprite_t获取Player指针
	Player* GetPlayer( sprite_t* p );

	void	BeginTimer( TIMER_EVENT evt, uint32_t time );
	bool	EndTimer( bool force = false );

	//游戏开始
	void	GameStart();
	//读取玩家属性
	int		SetPlayerInfo( sprite_t* p, char* data, int len );

	int		SetFriendInfo( uint32_t id, char* data, int len );
	//一回合开始
	int		RoundBegin();
	int		NotifyGameInfo();
	//向玩家广播回合开始
	int 	NotifyRoundBegin();
	//广播玩家操作
	int		ActionAndNotify();
	//广播 游戏结束
	int		NotifyGameOver();
	//检测游戏是否结束
	int		CheckGameOver();

	int		SelectMonster( Player* player );

private:
	int32_t			m_nWinnerTeam;
	game_group_t*	m_pGrp;
	Player*			m_pPlayers[PLAYER_COUNT];	//当前活动角色，数据来自队伍
	Player*			m_pTeam[PLAYER_COUNT];		//角色队伍，每个队伍可能有不止一个人
	uint32_t		m_nPlayerCount;
	list_head_t		m_timer;
	uint32_t		m_nTimerID;
	uint32_t		m_nTimerCounter;
	GAME_STATE		m_nGameState;
	uint32_t 		m_nDBInfoCount;				//数据库返回的玩家信息数量
	bool			m_bQuickGame;				//快速游戏
	int32_t			m_nLevelID;					//关卡ID
	uint32_t		m_nFriendID;				//好友ID
	TIMER_EVENT		m_nTimerEvent;
	LEVEL_TYPE		m_nLevelType;				//游戏类型
	uint32_t		m_nTarApprID;				//抢徒弟时，徒弟原来师父的ID
	bool			m_bAuto;
};

#endif
