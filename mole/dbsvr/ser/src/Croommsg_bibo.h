/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_gen_vip.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:21:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"



class Croommsg_bibo: public CtableRoute10x10 {
	public:
		Croommsg_bibo(mysql_interface *db);
		
		int insert(userid_t userid, roommsg_insert_bibo_in *p_in);
		
		int update(userid_t userid, roommsg_insert_bibo_in *p_in);

		int bibo_edit(userid_t userid, roommsg_insert_bibo_in *p_in);

		int del(userid_t userid, uint32_t date); 
		int get_list(userid_t userid, roommsg_get_bibo_list_in *p_in, roommsg_get_bibo_list_out_item **pp_list,
		             uint32_t *p_count);


};
