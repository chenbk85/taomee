/*
 * =====================================================================================
 *
 *       Filename:  effectmodevad.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/12/2011 05:33:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "effectmodevad.h"

EModEvad::EModEvad()
{

}

EModEvad::~EModEvad()
{

}

void EModEvad::OnAdd( Player* p )
{
	p->m_nEvasionEx += m_nEffect;
}

void EModEvad::OnRemove( Player* p )
{
	p->m_nEvasionEx -= m_nEffect;
}

