/*
 * =====================================================================================
 *
 *       Filename:  Cgf_hunter_top.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/01/2011 01:37:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Saga (), saga@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#ifndef __GF_CGF_TMP_INFO_H__
#define __GF_CGF_TMP_INFO_H__

#include "CtableRoute.h"
#include "proto.h" 
#include "benchapi.h"
#include "proto.h"
class Cgf_tmp_info:public Ctable
{
	public:
		Cgf_tmp_info(mysql_interface *db);
		int insert(gf_insert_tmp_info_in* p_in);
		int get_id_value(uint32_t id_in, uint32_t type, uint32_t* db_num);
 	private:

};
#endif


