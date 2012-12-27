/*
 * =====================================================================================
 *
 *       Filename:  Cuser_lamu_classroom_course.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/02/2010 05:01:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CUSER_LAMU_CLASSROOM_COURSE_INCL
#define CUSER_LAMU_CLASSROON_COURSE_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_lamu_classroom_course : public CtableRoute100x10 {
public:
	Cuser_lamu_classroom_course(mysql_interface * db);
	
	int insert(userid_t userid, uint32_t course_id);
	int get_course_count(userid_t userid, uint32_t course_id, uint32_t& cnt_out);
	int set_course_count(userid_t userid, uint32_t course_id, uint32_t cnt);
	int delete_courses(userid_t userid);
	int find_course_id(userid_t userid, uint32_t course_id, uint32_t &count);
	int add_courses(userid_t userid /* , user_lamu_classroom_add_courses_in* p_in */);

	int get_course_level(uint32_t userid, uint32_t course_id, uint32_t *plevel);	
	int get_course(userid_t userid, user_lamu_classroom_get_course_list_out_item **pp_list, uint32_t *pcount);
};

#endif /* ----- #ifndef CUSER_LAMU_CLASSROOM_INCL ----- */
