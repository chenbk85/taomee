/*
 * =====================================================================================
 *
 *       Filename:  effectmoddef.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/17/2011 05:35:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "effectmoddef.h"


EModDef::EModDef()
{

}

EModDef::~EModDef()
{

}

void EModDef::OnAdd( Player* p )
{
	if( p )
	{
		p->m_nDefEx += m_nEffect;
	}
}

void EModDef::OnRemove( Player* p )
{
	if( p )
	{
		p->m_nDefEx -= m_nEffect;
	}
}
