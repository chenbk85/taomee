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
	
int Cuser::get_all_info(userid_t userid,  user_base_info_t* p_out)
{
		GEN_SQLSTR( this->sqlstr, "select  register_time,flag,xiaomee, color, age,nick,last_islandid,last_mapid,last_x,last_y, last_login,online_time from %s where userid=%u", 
				this->get_table_name(userid),userid  );
		STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR);
			INT_CPY_NEXT_FIELD(p_out->register_time);
			INT_CPY_NEXT_FIELD(p_out->flag);
			INT_CPY_NEXT_FIELD(p_out->xiaomee);
			INT_CPY_NEXT_FIELD(p_out->color);
			INT_CPY_NEXT_FIELD(p_out->age);
			BIN_CPY_NEXT_FIELD(p_out->nick,sizeof(p_out->nick));
			INT_CPY_NEXT_FIELD(p_out->last_islandid);
			INT_CPY_NEXT_FIELD(p_out->last_mapid);
			INT_CPY_NEXT_FIELD(p_out->last_x);
			INT_CPY_NEXT_FIELD(p_out->last_y);
			INT_CPY_NEXT_FIELD(p_out->last_login);
			INT_CPY_NEXT_FIELD(p_out->online_time);
		STD_QUERY_ONE_END();
}

int	Cuser::change_xiaomee(userid_t userid, int change_value )
{
  	uint32_t cur_value;
	return this->change_int_value(userid,"xiaomee",change_value,999999,&cur_value );
}

int	Cuser::insert(userid_t userid, pop_reg_in *p_in )
{
	char nick_mysql[mysql_str_len(sizeof( p_in->nick) )];
	set_mysql_string(nick_mysql, p_in->nick , sizeof( p_in->nick ) );
	uint32_t xiaomee=0;
	sprintf( this->sqlstr, "insert into %s values( %u,%u,%u, %u, %u, %u, '%s',0,0,0,0,0,0,0)",
		this->get_table_name(userid),
		userid, 
		0,
		xiaomee,
		uint32_t (time(NULL) ),
		p_in->color,
		p_in->age,
		nick_mysql 
		);						//expbox
	return this->exec_insert_sql( this->sqlstr, USER_ID_EXISTED_ERR );
}


int	Cuser::update_reg(userid_t userid, pop_reg_in *p_in )
{

	char nick_mysql[mysql_str_len(sizeof( p_in->nick) )];
	set_mysql_string(nick_mysql, p_in->nick , sizeof( p_in->nick ) );

	
	sprintf( this->sqlstr, "update %s set flag=flag|%u, color=%u, age=%u,\
		   	nick='%s' where userid=%u ",
		this->get_table_name(userid), 
		p_in->flag,
		p_in->color,
		p_in->age,
		nick_mysql,
		userid);
	return this->exec_update_sql( this->sqlstr, USER_ID_NOFIND_ERR );
}

int	Cuser::update_logout_info(userid_t userid, pop_logout_in *p_in )
{
	sprintf( this->sqlstr, "update %s set last_islandid=%u, last_mapid=%u, last_x=%u, \
			last_y=%u ,last_login=%u,online_time=%u, last_online_id=%u where userid=%u ",
			this->get_table_name(userid), 
			p_in->last_islandid,
			p_in->last_mapid,
			p_in->last_x,
			p_in->last_y,
			p_in->last_login,
			p_in->online_time,
			p_in->last_online_id,
			userid);

	return this->exec_update_sql( this->sqlstr, USER_ID_NOFIND_ERR );

}
int	Cuser::set_nick(userid_t userid,  pop_set_nick_in *p_in )
{
	char nick_mysql[mysql_str_len(sizeof( p_in->nick) )];
	set_mysql_string(nick_mysql, p_in->nick , sizeof( p_in->nick ) );
	sprintf( this->sqlstr, "update %s set nick='%s' where userid=%u ",
		this->get_table_name(userid),
		nick_mysql ,
		userid 
		);			

	return this->exec_update_sql( this->sqlstr, USER_ID_NOFIND_ERR );
}

int	Cuser::set_flag(userid_t userid,  pop_set_flag_in *p_in)
{
	sprintf( this->sqlstr, "update %s set flag=(flag&%u)|%u where userid=%u",
		this->get_table_name(userid), ~p_in->mask, p_in->flag&p_in->mask, userid);
	return this->exec_update_sql( this->sqlstr, USER_ID_NOFIND_ERR );
}

int	Cuser::set_color(userid_t userid,  pop_set_color_in *p_in)
{
	sprintf( this->sqlstr, "update %s set color=%u where userid=%u",
		this->get_table_name(userid), p_in->color, userid);
	return this->exec_update_sql( this->sqlstr, USER_ID_NOFIND_ERR );
}

int	Cuser::set_field_value(userid_t userid,  pop_user_set_field_value_in *p_in )
{
    char mysql_value[mysql_str_len(sizeof(p_in->field_value))];
    set_mysql_string(mysql_value, p_in->field_value, sizeof(p_in->field_value));
                
    GEN_SQLSTR(this->sqlstr, "update %s set %s = '%s' where userid = %u",
                this->get_table_name(userid), p_in->field_name,  mysql_value,
                  userid);
    return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int	Cuser::del(userid_t userid ){
	sprintf( this->sqlstr, "delete from %s where userid=%u  ",
		this->get_table_name(userid),  userid );
	return this->exec_delete_sql( this->sqlstr, USER_ID_NOFIND_ERR );
}

int Cuser::get_last_online_id(userid_t userid, uint32_t& last_online_id)
{
	GEN_SQLSTR( this->sqlstr, "select  last_online_id from %s where userid=%u", 
			this->get_table_name(userid),userid  );
	STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(last_online_id);
	STD_QUERY_ONE_END();

	return 0;
}
