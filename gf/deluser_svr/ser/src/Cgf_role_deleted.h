
#ifndef  CGF_ROLE_DELETED_H
#define  CGF_ROLE_DELETED_H
#include "CtableRoute10.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 

#define GF_ROLE_DEL_NUM_MAX	10 //每次清空的最大角色数

class Cgf_role_deleted :public CtableRoute10 {
	
	private:
	public:
		Cgf_role_deleted(mysql_interface * db ); 
		
		int add_role_deleted(userid_t userid, uint32_t role_regtime);

		int get_list_role_deleted(userid_t userid, uint32_t* p_count,
			gf_role_deleted_stru** pp_list);

		int del_role_deleted(userid_t userid, uint32_t role_regtime);

		int get_list_all_role_deleted(char* dbname,uint32_t del_time, 
			uint32_t* p_count,gf_role_deleted_stru** pp_list);

};


#endif   /* ----- #ifndef CUSER_INC  ----- */

