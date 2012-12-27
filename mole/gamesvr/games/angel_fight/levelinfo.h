/*
 * =====================================================================================
 *
 *       Filename:  levelinfo.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/20/2011 03:59:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef LEVEL_INFO_H
#define LEVEL_INFO_H

#define LEVEL_DROP_ITEM_MAX_COUNT	5

enum LEVEL_TYPE
{
	LT_NORMAL	= 0,
	LT_BOSS,
	LT_BOSS_TIME,
	LT_BOSS_VIP,
	LT_BOSS_COST,
	LT_FRIEND,
	LT_GET_APPRENTICE,		//收徒弟
	LT_GRAB_APPRENTICE,		//抢徒弟
	LT_PK_MASTER,			//挑战师傅
	LT_PK_APPRENTICE,		//挑战徒弟
	LT_TRAIN,				//师徒切磋
	LT_PK,
	LT_BOSS_ACTIVE,			//活动BOSS
	LT_MAX,
	LT_MAKE_DWARD		= 0xFFFFFFF,
};

class LevelInfo
{
public:
	LevelInfo( int id );
	~LevelInfo();

	void	GetRandMonsterID( int* id, int& count );
	bool	CheckTimeRange();
	int		GetLevelType();
	int		GetKey();
	int		GetCard();

	void	XmlAddMonsterID( char* val );
	void	XmlSetCreatureCount( char* val );
	void	XmlSetExpLose( char* val );
	void	XmlSetExpWin( char* val );
	void	XmlSetCPoint( char* val );
	void	XmlSetCondLvlID( char* val );
	void	XmlSetCondPLevel( char* val );
	void	XmlSetConsume( char* val );
	void	XmlSetTimeBegin( char* val );
	void	XmlSetTimeEnd( char* val );
	void	XmlSetKey( char* val );
	void	XmlSetCard( char* val );
	void	XmlSetType( char* val );
public:
	typedef std::vector<int>			VecMonsterID_t;
	typedef VecMonsterID_t::iterator 	VecMonsterIDIt_t;

	VecMonsterID_t	m_vecMonster;
	int				m_nLevelID;
	int				m_nCreatureCount;			//一次出现的怪物数目
	int				m_nExpWin;					//胜利经验奖励
	int				m_nExpLose;					//失败经验奖励
	int				m_nCompletePoint;			//地图完成度加成
	int				m_nCondLevelID;				//要求完成的关卡
	int				m_nCondPLevel;				//要求玩家等级
	int				m_nConsume;					//活力消耗
	int				m_nTimeBegin;				//时间限制区间
	int				m_nTimeEnd;
	int				m_nKeyNeed;					//进入钥匙
	int				m_nCardNeed;				//进入门卡
	int				m_nType;
};

inline int	LevelInfo::GetLevelType()
{
	return m_nType;
}

inline int LevelInfo::GetKey()
{
	return m_nKeyNeed;
}

inline int LevelInfo::GetCard()
{
	return m_nCardNeed;
}

#endif

