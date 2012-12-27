/*
 * =====================================================================================
 *
 *       Filename:  Cgf_top_hero.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/07/2010 01:37:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef __GF_CGF_TOP_HERO_H__
#define __GF_CGF_TOP_HERO_H__

#include "CtableRoute.h"
#include "proto.h" 
#include "benchapi.h"

class Cgf_hero_top:public Ctable
{
	public:
		Cgf_hero_top(mysql_interface *db);
		int set_user_hero_top_info(uint32_t userid, uint32_t role_tm, uint32_t lv, uint32_t exp);
		int get_user_hero_top_info(uint32_t userid, uint32_t * count);
	private:

};
#endif


