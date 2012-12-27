/*
 * =====================================================================================
 *
 *       Filename:  effectmodatk.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/20/2011 09:40:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "effectmodatk.h"


EModAtk::EModAtk()
{

}

EModAtk::~EModAtk()
{

}

void EModAtk::OnAdd(Player * p)
{
	if( !p )
	{
		return;
	}
	p->m_nAttackEx += m_nEffect;
}

void EModAtk::OnRemove(Player * p)
{
	if( !p )
	{
		return;
	}
	p->m_nAttackEx -= m_nEffect;
}

