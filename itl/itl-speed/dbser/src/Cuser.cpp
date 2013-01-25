/*
 * =========================================================================
 *
 *        Filename: Cuser.cpp
 *
 *        Version:  1.0
 *        Created:  2011-05-13 15:52:03
 *        Description:   
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#include    "Cuser.h"
#include <time.h>
Cuser::Cuser(mysql_interface * db ) 
	:CtableRoute100x10(db, "POP" , "t_user" , "userid")
{

}
	
int Cuser::get_all_info(userid_t userid,  pop_login_out* p_out)
{
		GEN_SQLSTR( this->sqlstr, "select  register_time, sex, age,nick  from %s where userid=%u", 
				this->get_table_name(userid),userid  );
		STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(p_out->register_time);
			INT_CPY_NEXT_FIELD(p_out->sex);
			INT_CPY_NEXT_FIELD(p_out->age);
			BIN_CPY_NEXT_FIELD(p_out->nick,sizeof(p_out->nick));
		STD_QUERY_ONE_END();
}

int	Cuser::insert(userid_t userid, pop_reg_in *p_in )
{
	char nick_mysql[mysql_str_len(sizeof( p_in->nick) )];
	set_mysql_string(nick_mysql, p_in->nick , sizeof( p_in->nick ) );
	sprintf( this->sqlstr, "insert into %s values( %u,%u, %u, %u, %u, '%s'  )",
		this->get_table_name(userid),
		userid, 
		0,
		uint32_t (time(NULL) ),
		p_in->sex,
		p_in->age,
		nick_mysql 
		);						//expbox
	return this->exec_insert_sql( this->sqlstr, USER_ID_EXISTED_ERR );
}
