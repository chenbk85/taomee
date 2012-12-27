/*
 * =====================================================================================
 *
 *       Filename:  levelmgr.h
 *
 *    Description:  ¹Ø¿¨¹ÜÀíÆ÷
 *
 *        Version:  1.0
 *        Created:  06/23/2011 04:08:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef LEVEL_MGR_H
#define LEVEL_MGR_H

#define LEVEL_ID_PVP	0
#define LEVEL_ID_PK		44

class LevelInfo;

class LevelMgr
{
public:
	LevelMgr();
	~LevelMgr();

	void		GetCreatureIDByLevelID( int32_t idx, int* creatureID, int& count );
	int			GetExpByLevelID( int idx, bool isWin );
	int 		LoadLevelInfo();
	int			GetCPoint( int idx );
	LevelInfo*	GetLevelInfo( int idx );
	bool		CheckValidID( int idx );

private:
	typedef std::vector<LevelInfo*>		VecLevelInfo_t;
	typedef VecLevelInfo_t::iterator	VecLevelInfoIt_t;
	VecLevelInfo_t	m_vecLevel;
};

extern LevelMgr g_levelMgr;
#endif
