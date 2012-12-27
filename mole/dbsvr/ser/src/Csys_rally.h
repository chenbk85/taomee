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

#ifndef  CSYS_RALLY_INCL
#define  CSYS_RALLY_INCL
#include "Ctable.h"
#include "proto.h"

class Csys_rally : public Ctable{
public:
	Csys_rally(mysql_interface * db);

	int insert(uint32_t team);

	int update_add(uint32_t team, uint32_t value);

	int update_race(uint32_t team, uint32_t index, uint32_t value);

	int get_list_race(sys_get_team_race_list_out_item **pp_list, uint32_t *p_count);

	int get_list_score(sys_get_team_score_list_out_item **pp_list, uint32_t *p_count);
};

#endif   /* ----- #ifndef CSYS_ARG_INCL  ----- */

