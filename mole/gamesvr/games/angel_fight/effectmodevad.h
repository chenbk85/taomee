/*
 * =====================================================================================
 *
 *       Filename:  effectmodevad.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/12/2011 05:33:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_MOD_EVAD_H
#define EFFECT_MOD_EVAD_H

#include "effect.h"

class EModEvad:public Effect
{
public:
	EModEvad();
	virtual ~EModEvad();

	virtual void OnAdd( Player* p );
	virtual void OnRemove( Player* p );
};

#endif
