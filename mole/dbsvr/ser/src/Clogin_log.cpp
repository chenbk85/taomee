/*
 * =====================================================================================
 *
 *       Filename:  Clogin_log.cpp
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
#include "Clogin_log.h"

/*
 * @brief 构造函数，一个库100个表
 */
Clogin_log::Clogin_log(mysql_interface *db) : CtableRoute100(db, "LOGIN_DB", "t_login_log", "userid")
{

}

/*
 * @brief 插入密码修改信息到数据库
 */

extern const skinfo_t *g_sk;
int Clogin_log::insert(userid_t userid, ff_login_add_in * p_in)
{
	sprintf(this->sqlstr, "insert into %s values(%u,%u,%u,%u,%u)",
							this->get_table_name(userid),
							userid,
							p_in->loginflag,
							p_in->logintime,
							p_in->onlineid,
							htonl(g_sk->remote_ip)
							);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}
