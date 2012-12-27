/*
 * =====================================================================================
 * 
 *       Filename:  Cgf_friend.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2011年11月15日 19时52分58秒 CST
 *       Revision:  1.0
 *       Compiler:  gcc
 * 
 *         Author:  saga
 *        Company:  TAOMEE
 * =====================================================================================
 */

#ifndef  __GF_CGF_MASTER_H__
#define  __GF_CGF_MASTER_H__
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 
using namespace std;



enum RELATION_TYPE {
    RELATION_TYPE_MASTER = 1,        /*师傅*/
    RELATION_TYPE_PRENTICE = 2,     /*徒弟*/
};        

struct mp_member_t {
    uint32_t    uid;
    uint32_t    roletm;
    uint32_t    tm;
    uint32_t    lv;
    uint32_t    grade;
};


class Cgf_master :public CtableRoute{
	public:
 		Cgf_master(mysql_interface * db);
        int get_member_list_by_type(userid_t userid, uint32_t roletm, uint32_t type, 
            mp_member_t **plist, uint32_t *pcount);
        int gf_get_master_info(userid_t userid, uint32_t roletm, uint32_t *p_id, uint32_t *p_tm);
        int add_master(userid_t userid, uint32_t roletm, uint32_t apprentice_id, uint32_t apprentice_tm, uint32_t tm);
        int add_apprentice(userid_t userid, uint32_t roletm, gf_master_add_apprentice_in * apprentice);
        int del_master(userid_t userid, uint32_t roletm, uint32_t apprentice_id, uint32_t apprentice_tm);
        int del_apprentice(userid_t userid, uint32_t roletm, uint32_t apprentice_id, uint32_t apprentice_tm);
        int set_int_value(userid_t userid, uint32_t roletm, 
            uint32_t apprentice_id, uint32_t apprentice_tm, uint32_t value, const char * col_name);

        int get_prentice_grade(userid_t userid, uint32_t roletm, uint32_t p_id, uint32_t p_tm, uint32_t *value);
		int update_grade(userid_t userid, uint32_t roletm, 
		    uint32_t apprentice_id, uint32_t apprentice_tm, uint32_t value);
        int get_prentice_cnt(userid_t userid, uint32_t roletm, uint32_t *pcount);
		int clear_role_info(userid_t userid,uint32_t role_regtime);
    private:
		
		int ret;
	public:
};


#endif   /* ----- #ifndef GF_CGF_MASTER_H  ----- */

