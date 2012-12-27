/*
 * =====================================================================================
 *
 *       Filename:  
 *
 *    Description: 通用的排行榜数据
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

#ifndef CGF_RANKER_HPP
#define CGF_RANKER_HPP

#include "CtableRoute.h"
#include "proto.h"

enum {
	range_type_time = 1,
	range_type_score = 2,
	max_ranker_range = 200,
};


class Cgf_ranker_info : public Ctable
{
	public:
		Cgf_ranker_info(mysql_interface *db); 


		int list_ranker_info(uint32_t ranker_id,
							 uint32_t range_type,
							 uint32_t ranker_range,
							 gf_list_ranker_info_out_item **item,
							 uint32_t * count);



		int get_better_count(uint32_t ranker_id,
				             uint32_t range_type,
							 uint32_t score,
			             	 uint32_t * better_count); 

		int get_total_count(uint32_t ranker_id, uint32_t * count);


		int del_worst_info(uint32_t ranker_id, uint32_t range_type);
						   


		int get_user_old_score(uint32_t ranker_id,
				               uint32_t userid,
				               uint32_t role_tm,
							   uint32_t *score);

		int save_user_ranker_info(gf_set_ranker_info_in * in);

};

#endif 


