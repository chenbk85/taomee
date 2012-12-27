/*
 * =====================================================================================
 *
 *       Filename:  conditionrand.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/21/2011 10:31:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include "conditionrand.h"

CondRand::CondRand()
{

}

CondRand::~CondRand()
{

}

bool CondRand::IsMeet( Player* p )
{
	if( rand()%100 < m_nExpect )
	{
		return true;
	}
	else
	{
		return false;
	}
}
