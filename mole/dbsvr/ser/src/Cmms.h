/*
 * =====================================================================================
 * 
 *       Filename:  Cmms.h
 * 
 *    Description:   毛毛怪
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

#ifndef  CMMS_INCL
#define  CMMS_INCL
#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"
#define  F_WATER  		"v1"
#define  F_MUD 	 		"v2"
#define  F_GETCOUNT 	 "v3"
#define  MMS_TYPE 	 		1

#define  OPT_WATER 	 		0
#define  OPT_MUD 	 		1

class Cmms:public CtableRoute10x10 {
	private:

		int init(uint32_t type, uint32_t userid );
		int get_value_by_type_db(userid_t userid,mms_get_info_by_type_in *p_in,
				mms_get_info_by_type_out *p_out );

	public:
		int get_opt_list(uint32_t type,userid_t userid,uint32_t* p_opt_time, uint32_t* p_opt_count,
			   	mms_opt_list *p_out );
		int get_mms( uint32_t userid ,mms_get_info_out *p_out  );
		Cmms(mysql_interface * db ); 

		int update_opt_list(uint32_t type,userid_t userid ,uint32_t opt_type,
				uint32_t opt_time, uint32_t opt_count, mms_opt_list * p_list );
		int mms_opt(userid_t userid, uint32_t type, mms_opt_item *p_item);
		int mms_gen_one_fruit(userid_t userid );
		int mms_add_getcount(userid_t userid );
		int get_list(userid_t userid, uint32_t *p_count, 
				mms_get_type_list_out_item **pp_out_item );
		int set_value(userid_t userid, mms_set_value_in *p_in );
		int get_value_by_type(userid_t userid,mms_get_info_by_type_in *p_in,mms_get_info_by_type_out *p_out );
		int get_list_ex(userid_t userid, uint32_t *p_count, 
				mms_get_type_list_ex_out_item **pp_out_item );
};

#endif   /* ----- #ifndef CMMS_INCL  ----- */

