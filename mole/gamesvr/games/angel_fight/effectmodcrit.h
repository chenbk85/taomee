/*
 * =====================================================================================
 *
 *       Filename:  effectmodcrit.h
 *
 *    Description:  ÐÞ¸Ä±©»÷ÂÊ
 *
 *        Version:  1.0
 *        Created:  06/20/2011 09:42:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_MOD_CRIT_H
#define EFFECT_MOD_CRIT_H

#include "effect.h"

class EModCrit:public Effect
{
public:
	EModCrit();
	virtual ~EModCrit();

	virtual void OnAdd( Player* p );
	virtual void OnRemove( Player* p );
};

#endif


