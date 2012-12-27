/*
 * =====================================================================================
 *
 *       Filename:  effectmodmp.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/12/2011 01:48:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "effectmodmp.h"

EModMp::EModMp()
{

}

EModMp::~EModMp()
{

}

void EModMp::Execute( Player* p, int round )
{
	if( p )
	{
		p->IncMP( m_nEffect );
	}
}
