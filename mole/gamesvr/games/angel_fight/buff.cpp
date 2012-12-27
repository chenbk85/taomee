/*
 * =====================================================================================
 *
 *       Filename:  buff.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/13/2011 01:10:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include <algorithm>
#include "../../ant/inet/pdumanip.hpp"
#include <stdlib.h>
#include "effect.h"
#include "effectmgr.h"
#include "buff.h"

Buff::Buff()
{
}

Buff::~Buff()
{
}

void Buff::OnAdd(Player * p, VecAddedEffect_t& vecEffect )
{
	VecEffectIt_t it = m_vecEffect.begin();
	Effect* effect = NULL;
	while( it != m_vecEffect.end() )
	{
		effect = g_effectMgr.GetEffect( *it );
		if( effect && effect->CheckCondition(p) )
		{
			effect->OnAdd(p);
			vecEffect.push_back(*it);
		}
		it++;
	}
}

void Buff::OnRemove( Player * p, VecAddedEffect_t& vecEffect )
{
	VecEffectIt_t it = m_vecEffect.begin();
	Effect* effect = NULL;
	while( it != m_vecEffect.end() )
	{
		VecAddedEffectIt_t it_added = find( vecEffect.begin(), vecEffect.end(), *it );
		if( it_added != vecEffect.end() )
		{
			effect = g_effectMgr.GetEffect( *it );
			if( effect )
			{
				effect->OnRemove(p);
			}
			vecEffect.erase(it_added);
		}
		it++;
	}
}

int32_t Buff::GetRoundLasts()
{
	return m_nRoundLasts;
}

void Buff::Execute( Player * p,int32_t roundLeft, VecAddedEffect_t& vecEffect )
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
		if( !effect )
		{
			it++;
			continue;
		}

		int effectIdx = *it;

		VecAddedEffectIt_t it_added = find( vecEffect.begin(), vecEffect.end(), effectIdx );
		//满足触发条件
		if( effect->CheckCondition(p) )
		{
			//效果未触发
			if( it_added == vecEffect.end() )
			{
				effect->OnAdd(p);
				vecEffect.push_back( effectIdx );
			}
		}
		//不满足触发条件
		else
		{
			//效果已触发
			if( it_added != vecEffect.end() )
			{
				effect->OnRemove(p);
				vecEffect.erase( it_added );
			}
		}
		//更新
		it_added = find( vecEffect.begin(), vecEffect.end(), *it );

		if( it_added != vecEffect.end() )
		{
			effect->Execute( p, m_nRoundLasts - roundLeft );
		}
		it++;
	}
}

void Buff::XmlAddEffect(char * val)
{
	if( val )
	{
		m_vecEffect.push_back( atoi(val) );
	}
}

void Buff::XmlSetLasts(char * val)
{
	if(val)
	{
		m_nRoundLasts = atoi(val);
	}
}

