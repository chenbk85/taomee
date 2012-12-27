/*
 * =====================================================================================
 *
 *       Filename:  effectmodatk.h
 *
 *    Description:  ÐÞ¸Ä¹¥»÷Á¦
 *
 *        Version:  1.0
 *        Created:  06/20/2011 09:36:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_MOD_ATK_H
#define EFFECT_MOD_ATK_H
#include "effect.h"

class EModAtk:public Effect
{
public:
	EModAtk();
	virtual ~EModAtk();

	virtual void OnAdd( Player* p );
	virtual void OnRemove( Player* p );
};

#endif
