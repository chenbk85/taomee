/*
 * =====================================================================================
 *
 *       Filename:  effectmodblock.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/12/2011 05:34:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_MOD_BLOCK_H
#define EFFECT_MOD_BLOCK_H

#include "effect.h"

class EModBlock:public Effect
{
public:
	EModBlock();
	virtual ~EModBlock();

	virtual void OnAdd( Player* p );
	virtual void OnRemove( Player* p );
};

#endif

