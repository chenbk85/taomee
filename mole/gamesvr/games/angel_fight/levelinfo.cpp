/*
 * =====================================================================================
 *
 *       Filename:  levelinfo.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/20/2011 04:18:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
extern "C"
{
#include <libtaomee/log.h>
#include "../../timer.h"
}

#include <stdlib.h>
#include <vector>
#include "creatureinfo.h"
#include "creatureinfomgr.h"
#include "levelinfo.h"
#include <algorithm>

#define MONSTER_PRE_LEVEL	5

int cmp(const void *v1, const void *v2)
{
	return *(uint32_t*)v1 - *(uint32_t*)v2;
}

LevelInfo::LevelInfo( int id ):m_nLevelID(id)
{
	m_vecMonster.reserve(MONSTER_PRE_LEVEL);
	m_nType			= LT_NORMAL;
	m_nCondLevelID	= -1;
	m_nCondPLevel	= 1;
	m_nTimeBegin	= 0;
	m_nTimeEnd		= 24*60;
	m_nKeyNeed		= 0;
	m_nCardNeed		= 0;
}

LevelInfo::~LevelInfo()
{
	m_vecMonster.clear();
}

void LevelInfo::GetRandMonsterID(int * id,int & count )
{
	if( !id || m_vecMonster.empty() )
	{
		count = 0;
		return;
	}

	count = m_nCreatureCount;
	char sign[4];

	for( int i = 0; i < count; ++i )
	{
		sign[i] = rand() % m_vecMonster.size();
		for( int j = 0; j < i; ++j )
		{
			if( sign[i] == sign[j] )
			{
				sign[i] = ( sign[i] + 1 ) % m_vecMonster.size();
			}
		}
	}
	for( int i = 0; i < count; ++i )
	{
		id[i] = m_vecMonster[ sign[i] ];
	}
}

bool LevelInfo::CheckTimeRange()
{
	const struct tm* cur_time = get_now_tm();
	int cur = cur_time->tm_hour * 60 + cur_time->tm_min;
	DEBUG_LOG( "CurTime:[%d]min BeginTiem:[%d]min, EndTime:[%d]min", cur, m_nTimeBegin, m_nTimeEnd );
	if( cur > m_nTimeBegin && cur < m_nTimeEnd )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void LevelInfo::XmlAddMonsterID(char * val)
{
	if( val )
	{
		m_vecMonster.push_back( atoi(val) );
	}
}

void LevelInfo::XmlSetCreatureCount(char * val)
{
	if( val )
	{
		m_nCreatureCount = atoi(val);
	}
}

void LevelInfo::XmlSetExpLose(char * val)
{
	if( val )
	{
		m_nExpLose = atoi(val);
	}
}

void LevelInfo::XmlSetExpWin(char * val)
{
	if( val )
	{
		m_nExpWin = atoi(val);
	}
}

void LevelInfo::XmlSetCPoint(char * val)
{
	if( val )
	{
		m_nCompletePoint = atoi(val);
	}
}

void LevelInfo::XmlSetCondLvlID( char* val )
{
	if( val )
	{
		m_nCondLevelID = atoi(val);
	}
}

void LevelInfo::XmlSetCondPLevel( char* val )
{
	if( val )
	{
		m_nCondPLevel = atoi(val);
	}
}

void LevelInfo::XmlSetConsume( char* val )
{
	if( val )
	{
		m_nConsume = atoi(val);
	}
}

void LevelInfo::XmlSetTimeBegin( char* val )
{
	if( val )
	{
		m_nTimeBegin = atoi(val);
	}
}

void LevelInfo::XmlSetTimeEnd( char* val )
{
	if( val )
	{
		m_nTimeEnd = atoi(val);
	}
}

void LevelInfo::XmlSetKey( char* val )
{
	if( val )
	{
		m_nKeyNeed = atoi(val);
	}
}

void LevelInfo::XmlSetCard( char* val )
{
	if( val )
	{
		m_nCardNeed = atoi(val);
	}
}

void LevelInfo::XmlSetType( char* val )
{
	if( val )
	{
		m_nType = atoi(val);
	}
}
