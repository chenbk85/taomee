/*
 * =====================================================================================
 *
 *       Filename:  condition.h
 *
 *    Description:  ´¥·¢Ìõ¼þ
 *
 *        Version:  1.0
 *        Created:  06/21/2011 09:44:44 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CONDITION_H
#define CONDITION_H

#include <stdlib.h>
#include "player.h"

class Condition
{
public:
	Condition();
	virtual ~Condition();
	virtual bool	IsMeet( Player* p );
	void			XmlSetExpect( char* val );
protected:
	int m_nExpect;
};

#endif
