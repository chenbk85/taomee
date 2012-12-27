/*
 * =====================================================================================
 *
 *       Filename:  effectdot.h
 *
 *    Description:  Hp调整效果
 *
 *        Version:  1.0
 *        Created:  06/17/2011 05:16:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_MOD_HP_H
#define EFFECT_MOD_HP_H

#include "effect.h"

class EModHp:public Effect
{
public:
	EModHp();
	virtual ~EModHp();

	virtual void Execute( Player* p, int round );
};

#endif
