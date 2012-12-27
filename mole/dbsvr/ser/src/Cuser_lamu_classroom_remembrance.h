/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamu_classroom_remembrance.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/07/2010 09:59:11 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_LAMU_CLASSROOM_REMEMBRANCE_INCL 
#define CUSER_LAMU_CLASSROOM_REMEMBRANCE_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_lamu_classroom_remembrance : public CtableRoute100x10 {
public:
	Cuser_lamu_classroom_remembrance(mysql_interface * db);
	int insert(userid_t userid, uint32_t remem_id, uint32_t remem_cnt, uint32_t got_time);
	int get_remembrances(userid_t userid, user_lamu_classroom_get_remembrance_out_item **pp_item, uint32_t *pcount);
	int update_remem_cnt(userid_t userid, uint32_t remem_id, uint32_t remem_cnt);
	
	int	cal_student_graduate(userid_t userid,
			user_lamu_classroom_graduate_direction_info &graduate_info,
			user_lamu_classroom_get_lamus_info_out_item *p_lamu_list, uint32_t lamu_cnt,
			user_lamu_classroom_get_remembrance_out_item *p_remem_list, uint32_t remen_cnt,
			user_lamu_classroom_get_exam_info_out_item *p_score_list,
			uint32_t &great_cnt);
private:
	uint32_t cal_student_direct(userid_t userid,
				user_lamu_classroom_get_lamus_info_out_item *p_lamu, 
			    user_lamu_classroom_get_exam_info_out_item *p_score_list, uint32_t lamu_cnt);

	char *msglog_file;
};

#endif /* ----- CUSER_LAMU_CLASSROOM_REMEMBRANCE_INCL ----- */
