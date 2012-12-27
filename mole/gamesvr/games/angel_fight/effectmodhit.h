/*
 * =====================================================================================
 *
 *       Filename:  effectmodhit.h
 *
 *    Description:  ÐÞ¸ÄÃüÖÐÂÊ
 *
 *        Version:  1.0
 *        Created:  06/20/2011 09:44:28 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_MOD_HIT_H
#define EFFECT_MOD_HIT_H

#include "effect.h"

class EModHit:public Effect
{
public:
	EModHit();
	virtual ~EModHit();

	virtual void OnAdd( Player* p );
	virtual void OnRemove( Player* p );
};

#endif

