/*
 * =====================================================================================
 *
 *       Filename:  Clogin_log.h
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
#ifndef Clogin_log_INC
#define Clogin_log_INC

#include "CtableRoute100.h"
#include "proto.h"
#include "benchapi.h"

/* 
 * @brief 保存修改密码信息的类
 */
class Clogin_log : public CtableRoute100 {

public:
	Clogin_log(mysql_interface *db);
	/* 插入修改信息 */

	int insert(userid_t userid, ff_login_add_in * p_in);
};

#endif
