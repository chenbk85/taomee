/*
 * =====================================================================================
 *
 *       Filename:  linker.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/10/2012 02:32:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee (), ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef LINKER_H
#define LINKER_H

#include "../mpog.hpp"

class Linker:public mpog
{
public:
	//玩家信息
	struct PlayerInfo
	{
		sprite_t*	m_pSprite;
		uint32_t	m_nChessCntIni;		//初始棋子数目
		uint32_t	m_nChessCntCur;		//当前棋子数目
		int32_t		m_nChessUsed;		//已经使用的数目
		uint32_t	m_nGoldAdd;			//获取的游戏币
		char		m_cFlag;
		bool		m_bGetRes;
		bool		m_bHasItem[4];		//赠送的 数量只能为一个的随机物品 “1，2，快，乐”
	};
	//当前系统状态
	enum GameState
	{
		GS_INI,
		GS_RESOUCE_LOAD,
		GS_INGAME,
		GS_OVER,
	};
	//游戏内部协议
	enum LINKER_GAME_PROTO
	{
		LGP_GAME_START	= 0,
		LGP_SITTING,
		LGP_WIN,
		LGP_GAMEOVER,
		LGP_RAND_AWARD,
		LGP_GAME_INFO,
		LGP_LOAD_OK,
		LGP_GET_AWARD,
	};
	//棋盘标记
	enum CELL_FLAG
	{
		CF_BLANK		= 0,
		CF_WINTER		= 1,
		CF_SPRING		= 2,
		CF_GOLD200,
		CF_GOLD500,
		CF_GOLD1000,
		CF_CHESS_W_10,
		CF_CHESS_S_10,
		CF_CHESS_W_25,
		CF_CHESS_S_25,
		CF_CHESS_W_50,
		CF_CHESS_S_50,
		CF_BE_ADD		= (1<<7),		//胜负检测时已加入检测队列的标记
	};

	//胜负检测结果
	enum WINNER
	{
		W_HAVNT_OVER	= 0,
		W_WINTER		= 1,
		W_SPRING		= 2,
		
	};
	enum ResultReason
	{
		RR_NORMAL		= 0,
		RR_LEAVE,
		RR_TIMEOUT,
	};

	enum TimerEvent
	{
		TE_PLAYER_SIT	= 0,		//等待玩家落子
		TE_PLAYER_LOAD,				//等待玩家加载资源
	};

public:
	int		handle_data( sprite_t* p, int cmd, const uint8_t body[], int len );
	int		handle_db_return( sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code );
	int		handle_timeout( void* data );
	void	init( sprite_t* p );
	Linker( game_group_t* grp );
	~Linker();

private:
	void	SendGameInfo();
	void	GameStart();
	int		GameOver( int winner, int reason = RR_NORMAL );
	int		Sitting( PlayerInfo* player, int pos );		//落子 自左下到右上为Y轴正向，左上至右下为x正向
	int		CheckGameOver();							//检测游戏是否结束
	int		IsPass( CELL_FLAG flag, int pos );			//检测指定点到目的地是否联通
	int		GetGameInfo(sprite_t* p);					//获取当前游戏信息
	void	TestAdd( char pos );
	void	TestListClear();
	void	CreateRandomAward();						//生成随机奖品
	Linker::PlayerInfo* GetPlayer( sprite_t* p );

	void	BeginTimer( TimerEvent evt, uint32_t time );
	bool	EndTimer( bool force = false );

	game_group_t*	m_pGrp;
	list_head_t		m_timer;
	uint32_t		m_nTimerID;
	uint32_t		m_nTimerCounter;
	TimerEvent		m_nTimerEvent;
	PlayerInfo		m_player[2];
	char*			m_pMap;				//棋盘
	char*			m_pTestList;
	char			m_cTestListCnt;
	int				m_nMapSize;			//棋盘边长 3*3 5*5 7*7
	uint32_t		m_nDBRetCntNeed;	//db还要返回的数据数量
	GameState		m_nGameState;
	char			m_cStepCnt;
	char			m_cCurOperator;		//当前等待操作的玩家	取值为 CF_WINTER,CF_SPRING
	char			m_cResloadCnt;		//等待加载资源的玩家数量
};

#endif
