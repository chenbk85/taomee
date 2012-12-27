/*
 * =====================================================================================
 * 
 *       Filename:  Csys_arg.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CSYS_FIRE_CUP_INCL
#define  CSYS_FIRE_CUP_INCL
#include "Ctable.h"
#include "proto.h"

class Csys_fire_cup : public Ctable{
public:
	Csys_fire_cup(mysql_interface * db);
	int insert(uint32_t teamid, int32_t value);
	int update(uint32_t teamid, int32_t value);
	int get_team_medal(uint32_t teamid, uint32_t* p_count);
	int get_list_medal(sys_get_all_medal_list_out_item **pp_list, uint32_t *p_count);
};

#endif   /* ----- #ifndef CSYS_ARG_INCL  ----- */

