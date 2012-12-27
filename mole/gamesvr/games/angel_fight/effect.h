/*
 * =====================================================================================
 *
 *       Filename:  effect.h
 *
 *    Description:  特效接口
 *
 *        Version:  1.0
 *        Created:  06/13/2011 01:19:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef EFFECT_H
#define EFFECT_H
#include <vector>
#include "player.h"
#include "condition.h"

class Effect
{
public:
	Effect();
	virtual ~Effect();
	virtual void OnAdd( Player* p );
	virtual void OnRemove( Player* p );
	virtual void Execute( Player* p, int round );

	bool CheckCondition( Player* p );
	void XmlSetEffect( char* val );
	void AddCondition( Condition* cond );
protected:
	int	m_nEffect;
	typedef std::vector<Condition*>		VecCondition_t;
	typedef VecCondition_t::iterator	VecConditionIt_t;

	VecCondition_t	m_vecCondition;
};

#endif
