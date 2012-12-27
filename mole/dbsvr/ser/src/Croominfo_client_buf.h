/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_client_buf.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/12/2010 10:41:18 AM
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Croominfo_client_buf.h
 *
 *         Author:  tommychen
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef ROOMINFO_CLIENT_BUF_INC
#define ROOMINFO_CLIENT_BUF_INC

#include "CtableRoute10x10.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"

class Croominfo_client_buf : public CtableRoute10x10
{

public:
	Croominfo_client_buf(mysql_interface * db );
	int insert_client_buf(userid_t userid, roominfo_set_client_buf_in* p_in);
	int update_client_buf(userid_t userid, roominfo_set_client_buf_in* p_in);
	int get_client_buf(userid_t userid, uint32_t type_id, roominfo_get_client_buf_out* p_out);
};

#endif


