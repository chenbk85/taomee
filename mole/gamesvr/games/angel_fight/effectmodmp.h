/*
 * =====================================================================================
 *
 *       Filename:  effectmodmp.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/12/2011 01:47:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_MOD_MP_H
#define EFFECT_MOD_MP_H

#include "effect.h"

class EModMp:public Effect
{
public:
	EModMp();
	virtual ~EModMp();

	virtual void Execute( Player* p, int round );
};

#endif
