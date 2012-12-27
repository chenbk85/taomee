/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_npc.h
 * 
 *    Description:  一个NPC与一个用户之间存在一个相关的系数，称为好感度
 *    				好感度不同可能影响用户与该NPC的交互，这里记录着这个
 *    				好感度。
 * 
 *        Version:  1.0
 *        Created:  2008年12月22日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zheng, zheng@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_NPC_INCL
#define  CUSER_NPC_INCL
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_npc: public CtableRoute100x10
{
public:
	Cuser_npc(mysql_interface * db); 

	int insert(userid_t userid); 
	
	int get_npc(userid_t userid, user_npc_out *p_out);

	int update_npc(userid_t userid, int32_t change_value, uint32_t index);

	int change_npc(userid_t userid, int32_t chagne_value, uint32_t index);

	int update_all_npc(userid_t userid,user_npc_update_web_in *p_in);

	int get_one_npc(userid_t userid, uint32_t index, int32_t *p_out);
};

#endif   /* ----- #ifndef CUSER_CARD_INCL  ----- */

