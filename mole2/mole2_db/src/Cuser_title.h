/*
 * =====================================================================================
 *
 *       Filename:  Cuser_title.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/16/2010 11:17:59 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef  CUSER_TITLE_INC
#define  CUSER_TITLE_INC

#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"
 
#include <time.h>
#include <algorithm>







class Cuser_title : public CtableRoute
{
public:
	Cuser_title(mysql_interface * db);

	int insert(uint32_t userid, uint32_t titleid, uint32_t time);

	int delete_title(uint32_t userid, uint32_t titleid);
	int title_set(uint32_t userid, uint32_t titleid_old, uint32_t titleid_new);


	int title_list_get(userid_t userid, 
		stru_mole2_user_title_info** pp_out_item, uint32_t* p_count,
		uint32_t honorid, bool* p_use_flag );
	
	int del(uint32_t userid, uint32_t titleid);
	
	int get_titles(uint32_t userid, std::vector<uint32_t> &titles);
	int get_titles_ex(uint32_t userid, std::vector<title_t> &titles);
};

#endif
