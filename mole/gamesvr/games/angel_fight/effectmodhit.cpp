/*
 * =====================================================================================
 *
 *       Filename:  effectmodhit.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/20/2011 10:17:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "effectmodhit.h"

EModHit::EModHit()
{

}

EModHit::~EModHit()
{

}

void EModHit::OnAdd(Player * p)
{
	if( !p )
	{
		return;
	}
	p->m_nHitEx += m_nEffect;
}

void EModHit::OnRemove(Player * p)
{
	if( !p )
	{
		return;
	}
	p->m_nHitEx -= m_nEffect;
}



