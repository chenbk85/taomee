/*
 * =====================================================================================
 *
 *       Filename:  Cchange_passwd.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/08/2009 11:18:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CCHANGE_PASSWD_INC
#define CCHANGE_PASSWD_INC

#include "CtableRoute100.h"
#include "proto.h"
#include "benchapi.h"

/* 
 * @brief 保存修改密码信息的类
 */
class Cchange_passwd : public CtableRoute100 {

public:
	Cchange_passwd(mysql_interface *db);
	/* 插入修改信息 */
	int insert(userid_t userid, passwd_add_change_in * p_in);
};

#endif
