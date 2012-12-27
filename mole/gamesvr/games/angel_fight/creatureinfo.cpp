/*
 * =====================================================================================
 *
 *       Filename:  creatureinfo.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/20/2011 05:05:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

extern "C"
{
#include <libtaomee/log.h>
}

#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include "creatureinfo.h"

CreatureInfo::CreatureInfo()
{
	m_nSkillCount = 0;
	m_nItemCount = 0;
	m_nStr 			= 0;
	m_nInt 			= 0;
	m_nAgile 		= 0;
	m_nHabitus 		= 0;
	m_nAttack		= 0;
	m_nActSpeed		= 0;
	m_nEvasion		= 0;
	m_nBlock		= 0;
	m_nCritical		= 0;
	m_nHit			= 0;
	m_nCombo		= 0;
	m_nDef 			= 0;
	m_bIsBoss		= false;
}

CreatureInfo::~CreatureInfo()
{

}

void CreatureInfo::XmlSetStr(char * val)
{
	if( val )
	{
		m_nStr = atoi(val);
	}
}

void CreatureInfo::XmlSetInt(char * val)
{
	if( val )
	{
		m_nInt = atoi(val);
	}
}

void CreatureInfo::XmlSetAgile(char * val)
{
	if( val )
	{
		m_nAgile = atoi(val);
	}
}

void CreatureInfo::XmlSetHabitus(char * val)
{
	if( val )
	{
		m_nHabitus = atoi(val);
	}
}

void CreatureInfo::XmlSetLevel(char * val)
{
	if( val )
	{
		m_nLevel= atoi(val);
	}
}

void CreatureInfo::AddItem(int32_t idx)
{
	if( idx < 0 )
	{
		DEBUG_LOG( "CreatureInfo::AddItem Invalid Item ID:[%d]", idx );
		return;
	}
	if( m_nItemCount < MAX_ITEM_COUNT )
	{
		m_nItem[m_nItemCount] = idx;
		m_nItemCount++;
	}
	else
	{
		DEBUG_LOG( "CreatureInfo::AddItem Invalid Item Count Out of Range Creature ID is:[%d]", m_nCreatureID );
	}
}

void CreatureInfo::AddSkill(int32_t idx)
{
	if( idx < 0 )
	{
		DEBUG_LOG( "CreatureInfo::AddSkill Invalid Skill ID:[%d]", idx );
		return;
	}
	if( m_nSkillCount < MAX_SKILL_COUNT )
	{
		m_nSkill[m_nSkillCount] = idx;
		m_nSkillCount++;
	}
	else
	{
		DEBUG_LOG( "CreatureInfo::AddSkill Invalid Skill Count Out of Range Creature ID is:[%d]", m_nCreatureID );
	}
}

void CreatureInfo::XmlAddItem(char * val)
{
	if( val )
	{
		AddItem( atoi(val) );
	}
}

void CreatureInfo::XmlAddSkill(char * val)
{
	if( val )
	{
		AddSkill( atoi(val) );
	}
}

void CreatureInfo::XmlSetAtk(char * val)
{
	if( val )
	{
		m_nAttack = atoi(val);
	}
}
void CreatureInfo::XmlSetAspd(char * val)
{
	if( val )
	{
		m_nActSpeed = atoi(val);
	}
}
void CreatureInfo::XmlSetEvad(char * val)
{
	if( val )
	{
		m_nEvasion = atoi(val);
	}
}
void CreatureInfo::XmlSetBlock(char * val)
{
	if( val )
	{
		m_nBlock = atoi(val);
	}
}
void CreatureInfo::XmlSetCrit(char * val)
{
	if( val )
	{
		m_nCritical = atoi(val);
	}
}
void CreatureInfo::XmlSetHit(char * val)
{
	if( val )
	{
		m_nHit = atoi(val);
	}
}
void CreatureInfo::XmlSetCombo(char * val)
{
	if( val )
	{
		m_nCombo = atoi(val);
	}
}
void CreatureInfo::XmlSetDef(char * val)
{
	if( val )
	{
		m_nDef = atoi(val);
	}
}

void CreatureInfo::XmlSetIsBoss(char * val)
{
	if( val )
	{
		m_bIsBoss = 0 == strcmp( val, "true" ) ? true : false;

	}
	else
	{
		m_bIsBoss = false;
	}
}

