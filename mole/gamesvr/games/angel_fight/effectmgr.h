/*
 * =====================================================================================
 *
 *       Filename:  effectmgr.h
 *
 *    Description:  特效管理器
 *
 *        Version:  1.0
 *        Created:  06/13/2011 01:31:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_MGR_H
#define EFFECT_MGR_H

class EffectMgr
{
public:
	EffectMgr();
	~EffectMgr();
	Effect* GetEffect( int32_t idx );
	int 	LoadEffectInfo();
private:
	typedef std::vector<Effect*>	VecEffect_t;
	typedef VecEffect_t::iterator	VecEffectIt_t;

	VecEffect_t	m_vecEffect;
};

extern EffectMgr g_effectMgr;

#endif
