/*
 * =====================================================================================
 *
 *       Filename:  Cchange_passwd.cpp
 *
 *    Description: 把密码修改信息存入数据库 
 *
 *        Version:  1.0
 *        Created:  04/08/2009 12:03:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "Cchange_passwd.h"

/*
 * @brief 构造函数，一个库100个表
 */
Cchange_passwd::Cchange_passwd(mysql_interface *db) : CtableRoute100(db, "PASSWD_CHANGE_DB", "t_passwd_change", "userid")
{

}


/*
 * @brief 插入密码修改信息到数据库
 */
int Cchange_passwd::insert(userid_t userid, passwd_add_change_in * p_in)
{
	char passwd_str[mysql_str_len(PASSWD_LEN)];
	set_mysql_string(passwd_str, p_in->passwd, sizeof(p_in->passwd));
	sprintf(this->sqlstr, "insert into %s values(%u, %u,%u, '%s')",
							this->get_table_name(userid),
							userid,
							p_in->opt_type,
							p_in->time,
							passwd_str
							);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}
