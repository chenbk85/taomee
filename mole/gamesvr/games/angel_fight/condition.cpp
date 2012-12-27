/*
 * =====================================================================================
 *
 *       Filename:  condition.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/21/2011 09:51:17 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ercilee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "condition.h"

Condition::Condition()
{

}

Condition::~Condition()
{

}

bool Condition::IsMeet( Player* p )
{
	return true;
}

void Condition::XmlSetExpect( char* val )
{
	if( val )
	{
		m_nExpect = atoi( val );
	}
}
