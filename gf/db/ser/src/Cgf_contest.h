/*
 * =====================================================================================
 *
 *       Filename:  Cgf_contest.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/23/2011 12:46:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef C_GF_CONTEST_H__
#define C_GF_CONTEST_H__
#include "CtableRoute.h"
#include "proto.h"


class Cgf_contest_info : public Ctable
{
	public:
		Cgf_contest_info(mysql_interface *db); 

		/** 
		 * @brief 拉队伍信息
		 * 
		 */
		int get_contest_team_info(contest_team_info_t ** team, uint32_t * count);

		/** 
		 * @brief  查找自己的队伍信息和捐献情况
		 */

		int get_contest_donate_info(donate_t ** in, uint32_t team_id, uint32_t * count); 

		/** 
		 * @brief  加入一个武斗队伍
		 * 
		 */
		int join_contest_team(uint32_t uid, uint32_t role_time, gf_join_contest_team_in * in);

		/** 
		 * @brief  捐献复活草
		 * 
		 */
		int contest_donate_plant(uint32_t uid, uint32_t role_time, uint32_t add_cnt);

		int get_one_team(uint32_t uid, uint32_t role_time, uint32_t * team_id, uint32_t *self_cnt, uint32_t * win_stage);

	private:
		class Cgf_contest_total;

		Cgf_contest_total * total_team_info;
};

#endif 


