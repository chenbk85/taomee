/*
 * =====================================================================================
 *
 *       Filename:  conditionhp.h
 *
 *    Description:  生命值条件触发器
 *
 *        Version:  1.0
 *        Created:  06/21/2011 10:19:57 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CONDITION_HP_H
#define CONDITION_HP_H

#include "condition.h"

class CondHP:public Condition
{
public:
	CondHP();
	virtual ~CondHP();
	virtual bool	IsMeet( Player* p );
};

#endif
