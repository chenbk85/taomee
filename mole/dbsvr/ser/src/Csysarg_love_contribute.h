/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_love_contribute.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/26/2011 01:23:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alexhe, alexhe@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CSYSARG_LOVE_CONTRIBUTE_INCL
#define CSYSARG_LOVE_CONTRIBUTE_INCL

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"

class Csysarg_love_contribute : Ctable
{
public:
	Csysarg_love_contribute(mysql_interface * db);
	int add_contribute_item(uint32_t userid, sysarg_add_contribute_item_in* p_in);
	int renew_contribute_list(uint32_t *count, contribute_info *p_list);
private:
	int get_user_count(uint32_t type, uint32_t *count);
	int insert_item(uint32_t type, uint32_t userid, uint32_t itemid, uint32_t item_cnt, char* nick);
	int delete_item(uint32_t type, uint32_t limit_cnt);
	int get_contribute_item(uint32_t type, contribute_info** pp_list, uint32_t* count);
};
#endif
