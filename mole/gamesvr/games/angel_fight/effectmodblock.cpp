/*
 * =====================================================================================
 *
 *       Filename:  effectmodblock.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/12/2011 05:33:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "effectmodblock.h"

EModBlock::EModBlock()
{

}

EModBlock::~EModBlock()
{

}

void EModBlock::OnAdd( Player* p )
{
	p->m_nBlockEx += m_nEffect;
}

void EModBlock::OnRemove( Player* p )
{

	p->m_nBlockEx -= m_nEffect;
}
