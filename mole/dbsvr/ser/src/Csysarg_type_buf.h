/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_type_buf.h
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

#ifndef SYSARG_TYPE_BUF_INC
#define SYSARG_TYPE_BUF_INC

#include "Ctable.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"

class Csysarg_type_buf : public Ctable
{

public:
	Csysarg_type_buf(mysql_interface * db );
	int insert_type_buf(sysarg_set_type_buf_in* p_in);
	int update_type_buf(sysarg_set_type_buf_in* p_in);
	int get_type_buf(uint32_t type, sysarg_get_type_buf_out* p_out);
};

#endif


