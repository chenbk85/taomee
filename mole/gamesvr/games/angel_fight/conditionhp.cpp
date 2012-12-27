/*
 * =====================================================================================
 *
 *       Filename:  conditionhp.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/21/2011 10:22:56 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "conditionhp.h"

CondHP::CondHP()
{

}

CondHP::~CondHP()
{

}

bool CondHP::IsMeet( Player* p )
{
	if( p->GetHP() < m_nExpect )
	{
		return true;
	}
	else return false;
}
