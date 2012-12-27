/*
 * =====================================================================================
 *
 *       Filename:  effectmodaspd.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/17/2011 05:57:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "effectmodaspd.h"

EModAspd::EModAspd()
{

}

EModAspd::~EModAspd()
{

}

void EModAspd::OnAdd( Player* p )
{
	if( p )
	{
		p->m_nActSpeedEx += m_nEffect;
	}
}

void EModAspd::OnRemove( Player* p )
{
	if( p )
	{
		p->m_nActSpeedEx -= m_nEffect;
	}
}

