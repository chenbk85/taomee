/*
 * =====================================================================================
 *
 *       Filename:  effectmoddef.h
 *
 *    Description:  防御力调整效果
 *
 *        Version:  1.0
 *        Created:  06/17/2011 05:31:18 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_MOD_DEF_H
#define EFFECT_MOD_DEF_H
#include "effect.h"

class EModDef:public Effect
{
public:
	EModDef();
	virtual ~EModDef();

	virtual void OnAdd( Player* p );
	virtual void OnRemove( Player* p );
};

#endif
