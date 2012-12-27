/*
 * =====================================================================================
 *
 *       Filename:  Cuser_wish_wall.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/09/2012 03:38:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_wish_wall.h"

#define SP_CP_SIZE 10

Cuser_wish_wall::Cuser_wish_wall(mysql_interface *db):
	CtableRoute100x10(db, "USER", "t_user_wish_wall", "userid")
{

}

int Cuser_wish_wall::insert(userid_t userid, char* sp_1, char* sp_2, char* sp_3)
{
	//横批
	char sp_cp_1[mysql_str_len(4)];
	memset(sp_cp_1, 0, sizeof(sp_cp_1));
    set_mysql_string(sp_cp_1 ,sp_1, 4);
	//左联
	char sp_cp_2[mysql_str_len(7)];
	memset(sp_cp_2, 0, sizeof(sp_cp_2));
    set_mysql_string(sp_cp_2 ,sp_2, 7);
	//右联
	char sp_cp_3[mysql_str_len(7)];
	memset(sp_cp_3, 0, sizeof(sp_cp_3));
    set_mysql_string(sp_cp_3 ,sp_3, 7);

	sprintf(this->sqlstr, "insert into %s values(NULL, %u, '%s', '%s', '%s')",
			this->get_table_name(userid),
			userid,
			sp_cp_1,
			sp_cp_2,
			sp_cp_3
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Cuser_wish_wall::get_all(userid_t userid, user_get_wish_wall_out_item** pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select  id, sp_couplet_1, sp_couplet_2, sp_couplet_3 \
			from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

 	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD ((*pp_list+i)->index);
		BIN_CPY_NEXT_FIELD ((*pp_list+i)->spcp_1, SP_CP_SIZE);
		BIN_CPY_NEXT_FIELD ((*pp_list+i)->spcp_2, SP_CP_SIZE);
		BIN_CPY_NEXT_FIELD ((*pp_list+i)->spcp_3, SP_CP_SIZE);
	STD_QUERY_WHILE_END();
}

int Cuser_wish_wall::get_count(userid_t userid, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*count);
	STD_QUERY_ONE_END();

}
