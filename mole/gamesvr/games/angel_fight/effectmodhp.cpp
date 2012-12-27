/*
 * =====================================================================================
 *
 *       Filename:  effectdot.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/17/2011 05:22:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "effectmodhp.h"

EModHp::EModHp()
{

}

EModHp::~EModHp()
{

}

void EModHp::Execute( Player* p, int round )
{
	if( p )
	{
		p->IncHP( m_nEffect );
	}
}

