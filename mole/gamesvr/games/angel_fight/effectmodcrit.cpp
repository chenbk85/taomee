/*
 * =====================================================================================
 *
 *       Filename:  effectmodcrit.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/20/2011 09:43:46 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "effectmodcrit.h"

EModCrit::EModCrit()
{

}

EModCrit::~EModCrit()
{

}

void EModCrit::OnAdd(Player * p)
{
	if( !p )
	{
		return;
	}
	p->m_nCriticalEx += m_nEffect;
}

void EModCrit::OnRemove(Player * p)
{
	if( !p )
	{
		return;
	}
	p->m_nCriticalEx -= m_nEffect;
}

