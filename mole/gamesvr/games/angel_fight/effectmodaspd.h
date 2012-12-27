/*
 * =====================================================================================
 *
 *       Filename:  effectmodaspd.h
 *
 *    Description:  行动速度调整效果
 *
 *        Version:  1.0
 *        Created:  06/17/2011 05:51:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_MOD_ASPD_H
#define EFFECT_MOD_ASPD_H

#include "effect.h"

class EModAspd:public Effect
{
public:
	EModAspd();
	virtual ~EModAspd();
	virtual void OnAdd( Player* p );
	virtual void OnRemove( Player* p );
};

#endif
