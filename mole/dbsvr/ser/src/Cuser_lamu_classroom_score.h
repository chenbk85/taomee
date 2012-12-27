/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamu_classroom_score.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/17/2010 12:11:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_LAMU_CLASSROOM_SCORE_INCL
#define CUSER_LAMU_CLASSROOM_SCORE_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_lamu_classroom_score : public CtableRoute100x10 {
public:
    Cuser_lamu_classroom_score(mysql_interface * db);
	int insert(userid_t userid, user_lamu_classroom_score_info score_info);
	int get_exam_score(userid_t userid, uint32_t exam_times, 
			user_lamu_classroom_get_exam_info_out_item **pp_list, uint32_t *p_count);
	int delete_score(userid_t userid);
};

#endif /* ----- #ifndef CUSER_LAMU_CLASSROOM_SCORE_INCL ----- */
