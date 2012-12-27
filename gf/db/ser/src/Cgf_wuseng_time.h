/*
 * =====================================================================================
 *
 *       Filename:  Cgf_wusheng_time.h
 *
 *    Description:  真假武圣成绩信息 
 *
 *        Version:  1.0
 *        Created:  10/11/2011 10:46:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus@taomee.com
 *        Company:  Taomee.sh
 *
 * =====================================================================================
 */
#ifndef CGF_WUSENG_INFO
#define CGF_WUSENG_INFO

#include "CtableRoute.h"
#include "proto.h"


class Cgf_wuseng_info : public Ctable
{
	public:
		Cgf_wuseng_info(mysql_interface *db); 

		/** 
		 * @brief 
		 * 
		 */
		int list_wuseng_pass_info(gf_list_true_wusheng_info_out_item ** item, uint32_t * count);

		int set_wuseng_pass_info(gf_set_true_wusheng_info_in *in);

		int get_user_pass_info(uint32_t uid, uint32_t role_time, uint32_t * pass_time);
		
		int get_quicker_pass(uint32_t pass_time, uint32_t * quicker_count);

		/*
		int get_slowest_pass(uint32_t * userid, uint32_t * role_time);

		int del_slowest_pass(uint32_t userid, uint32_t role_time);
		*/

};

#endif 


