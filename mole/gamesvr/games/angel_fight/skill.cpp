/*
 * =====================================================================================
 *
 *       Filename:  skill.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/10/2011 01:22:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <algorithm>
#include "../../ant/inet/pdumanip.hpp"
#include "effect.h"
#include "effectmgr.h"
#include "packetmgr.h"
#include "skill.h"

#define SKILL_MAX_BUFF_SIZE		3
#define SKILL_MAX_EFFECT_SIZE	2

Skill::Skill( int32_t id ): m_nSkillID(id)
{
	m_vecBuff.reserve(SKILL_MAX_BUFF_SIZE);
	m_vecEffect.reserve(SKILL_MAX_EFFECT_SIZE);
	m_vecBuff.clear();
	m_vecEffect.clear();
	//default value for test
	m_nDamage 	= 100;
	m_nConsume 	= 50;
	m_nUsePoint = -1;
	m_bIsEvil	= true;
}

Skill::~Skill()
{

}

void Skill::OnSelect( Player * p, VecAddedEffect_t& vecEffect )
{
	if( !p )
	{
		return;
	}
	VecEffectIt_t it = m_vecEffect.begin();
	Effect* effect = NULL;
	while( it != m_vecEffect.end() )
	{
		effect = g_effectMgr.GetEffect( *it );
		if( effect )
		{
			if( effect->CheckCondition(p) )
			{
				effect->OnAdd( p );
				vecEffect.push_back( *it );
			}
		}
		it++;
	}
}

void Skill::Execute( Player * p, bool critical, VecAddedEffect_t& vecEffect )
{
	if( !p )
	{
		return;
	}
	Player* tar = p->GetTar();
	if( !tar )
	{
		return;
	}

	SkillAtkInfo info;
	memset( &info, 0, sizeof(info) );

	info.m_nSkillID = m_nSkillID;
	info.m_isEvad = 0;

	Player* des = NULL;
	if( m_bIsEvil )
	{
		des = tar;
	}
	else
	{
		info.m_isToSelf = 1;
		des = p;
	}

	info.m_nMpConsume = m_nConsume;

	int damage;
	if( m_bIsEvil )
	{
		damage = p->GetAtk() + m_nDamage;
	}
	else
	{
		damage = m_nDamage;
	}
	if( critical )
	{
		info.m_isCritical = 1;
		damage *= 2;
	}

	info.m_nDamage = des->GetDamage( damage );

	des->IncHP( -des->GetDamage( damage ) );

	uint8_t* data_buf = NULL;
	int32_t len;
	uint32_t spaceLeft;
	p->NoticeNewPacket( &data_buf, len, spaceLeft );
	if( data_buf && spaceLeft > sizeof(int32_t)*2 + sizeof(PacketHead) )
	{
		ant::pack( data_buf, info.m_isToSelf, len );
		ant::pack( data_buf, info.m_nSkillID, len );
		ant::pack( data_buf, info.m_isEvad, len );
		ant::pack( data_buf, info.m_isCritical, len );
		ant::pack( data_buf, info.m_nDamage, len );
		ant::pack( data_buf, info.m_nMpConsume, len );
		p->CompletePacket( data_buf, PC_SKILL, len );
	}
	else
	{
		ERROR_LOG( "NoticeNewPacket Error, Ptr:[%p] SpaceLeft:[%d]", data_buf, spaceLeft );
	}

// 	DEBUG_LOG("[%u] SKILL: ToSelf[%d], SkillID[%d], Evad[%d], Critical[%d], Damage[%d] Consume[%d]",
// 				p->GetUserID(), info.m_isToSelf, info.m_nSkillID, info.m_isEvad, info.m_isCritical, info.m_nDamage, info.m_nMpConsume );

	VecBuffIt_t it = m_vecBuff.begin();
	while( it != m_vecBuff.end() )
	{
		des->AddBuff( *it , m_nSkillID );
		it++;
	}

	//执行并移除属性附加效果
	VecEffectIt_t it_eff = m_vecEffect.begin();
	Effect* effect = NULL;
	while( it_eff != m_vecEffect.end() )
	{
		VecAddedEffectIt_t it_added = find( vecEffect.begin(), vecEffect.end(), *it_eff );
		//没有触发
		if( it_added == vecEffect.end() )
		{
			it_eff++;
			continue;
		}
		//已经触发
		effect = g_effectMgr.GetEffect( *it_eff );
		if( effect )
		{
			effect->Execute( p, 0 );
			effect->OnRemove( p );
		}
		vecEffect.erase( it_added );
		it_eff++;
	}
}

void Skill::XmlSetDamage(char * val)
{
	if( val )
	{
		m_nDamage = atoi(val);
	}
	else
	{
		m_nDamage = 0;
	}
}

void Skill::XmlSetConsume(char * val)
{
	if( val )
	{
		m_nConsume = atoi(val);
	}
	else
	{
		m_nConsume = 0;
	}
}

void Skill::XmlSetIsEvil(char * val)
{
	if( val )
	{
		m_bIsEvil = 0 == strcmp( val, "true") ? true : false;
	}
	else
	{
		m_bIsEvil = true;
	}
}

void Skill::XmlAddBuffID(char * val)
{
	if( val )
	{
		m_vecBuff.push_back(atoi(val));
	}
}

void Skill::XmlAddEffectID(char * val)
{
	if( val )
	{
		m_vecEffect.push_back(atoi(val));
	}
}

void Skill::XmlSetUsePoint(char * val)
{
	if( val )
	{
		m_nUsePoint = atoi(val);
	}
}

