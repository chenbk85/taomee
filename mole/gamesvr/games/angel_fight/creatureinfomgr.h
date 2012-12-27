/*
 * =====================================================================================
 *
 *       Filename:  creatureinfomgr.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/14/2011 10:17:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CREATURE_INFO_MGR_H
#define CREATURE_INFO_MGR_H

class CreatureInfo;

class CreatureInfoMgr
{
public:
	CreatureInfoMgr();
	~CreatureInfoMgr();
	CreatureInfo* 	GetCreatureInfo( int32_t idx );
	int				LoadCreatureInfo();
private:
	typedef std::vector<CreatureInfo*>	VecCreatureInfo_t;
	typedef VecCreatureInfo_t::iterator	VecCreatureInfoIt_t;

	VecCreatureInfo_t	m_vecCreatureInfo;
};

extern CreatureInfoMgr g_creatureInfoMgr;

#endif
