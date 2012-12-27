/*
 * =====================================================================================
 *
 *       Filename:  effect.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/20/2011 04:46:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include "effect.h"
Effect::Effect()
{
}
Effect::~Effect()
{
	VecConditionIt_t it = m_vecCondition.begin();
	while( it != m_vecCondition.end() )
	{
		delete *it;
		it++;
	}
	m_vecCondition.clear();
}
void Effect::OnAdd( Player* p )
{
}
void Effect::OnRemove( Player* p )
{
}
void Effect::Execute( Player* p, int round )
{
}

bool Effect::CheckCondition( Player * p )
{
	VecConditionIt_t it = m_vecCondition.begin();
	while( it != m_vecCondition.end() )
	{
		if( !(*it)->IsMeet(p) )
		{
			return false;
		}
		it++;
	}
	return true;
}

void Effect::XmlSetEffect( char* val )
{
	if( val )
	{
		m_nEffect = atoi(val);
	}
}

void Effect::AddCondition(Condition * cond)
{
	if( cond )
	{
		m_vecCondition.push_back(cond);
	}
}
