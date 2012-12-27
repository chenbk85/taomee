/*
 * =====================================================================================
 *
 *       Filename:  conditionrand.h
 *
 *    Description:  随机条件出发器
 *
 *        Version:  1.0
 *        Created:  06/21/2011 10:26:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CONDTION_RAND_H
#define CONDTION_RAND_H

#include "condition.h"

class CondRand:public Condition
{
public:
	CondRand();
	virtual ~CondRand();
	virtual bool	IsMeet( Player* p );
};

#endif
