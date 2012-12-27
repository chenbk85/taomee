/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_love_contribute.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/26/2011 01:23:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  alexhe, alexhe@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "common.h"
#include "Csysarg_love_contribute.h"

#define ONE_TYPE_MAX_COUNT	40

//稀有物品列表，有80%的概率优先存放在捐献表中
const uint32_t excellent_item_list[] ={
	1270063, 1270041, 1270040, 1270055, 1230030,
	1230027, 1593034, 1593037, 1593065, 1353296, 
	1353297, 1353271, 1593028, 1593026, 1353223, 
	1353282, 1353284, 1353285, 1353288, 1353290, 
	1353303, 1353305, 1353307, 1353306
};
#define EXCELLENT_ITEM_CNT	(sizeof(excellent_item_list) / sizeof(excellent_item_list[0]))
#define EXCELLENT_RAND() (rand() % 100 < 95)

Csysarg_love_contribute::Csysarg_love_contribute(mysql_interface * db) :
	Ctable(db, "SYSARG_DB","t_sysarg_love_contribute")
{

}

int Csysarg_love_contribute::add_contribute_item(uint32_t userid, sysarg_add_contribute_item_in* p_in)
{
	uint32_t count = 0;
	int ret = this->get_user_count(p_in->contri_type, &count);
	if (count < ONE_TYPE_MAX_COUNT) {
		uint32_t i = 0;
		for (i = 0; i < EXCELLENT_ITEM_CNT; i++) {
			if (p_in->itemid == excellent_item_list[i]) {
				break;
			}
		}
		
		bool excellent_rand = EXCELLENT_RAND();
		if ((i < EXCELLENT_ITEM_CNT && excellent_rand) ||
			(i == EXCELLENT_ITEM_CNT && !excellent_rand)) {
			ret = this->insert_item(p_in->contri_type, userid, p_in->itemid, p_in->item_cnt, p_in->nick);
		} 
	}
	return SUCC;
}

/*
int Csysarg_love_contribute::delete_contribute_item()
{
	for (int i = 0; i < 4; i++) {
		this->delete_item(i + 1);
	}
	return SUCC;
}
*/

int Csysarg_love_contribute::renew_contribute_list(uint32_t *count, contribute_info *p_out)
{
	*count = 0;
	const uint32_t max_count = 16;
	for (uint32_t type = 1; type <= 4; type++) {
		if (*count == max_count) {
			break;
		}

		uint32_t type_cnt = 0;
		contribute_info* p_list = NULL;
		get_contribute_item(type, &p_list, &type_cnt);

		if (*count + type_cnt <= max_count) {
			memcpy(p_out + (*count), p_list, sizeof(contribute_info) * type_cnt);
			*count += type_cnt;
		} else {
			memcpy(p_out + (*count), p_list, (max_count - *count) * sizeof(contribute_info));
			*count = max_count;
		}
		if (p_list != NULL) {
			free(p_list);
			p_list = NULL;
		}
		this->delete_item(type, type_cnt);
	}
	return 0;
}

int Csysarg_love_contribute::get_user_count(uint32_t type, uint32_t *count)
{
	sprintf(this->sqlstr, "select count(*) from %s where type = %u", this->get_table_name(), type);
	
	*count = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
        INT_CPY_NEXT_FIELD(*count);
    STD_QUERY_ONE_END();
}

int Csysarg_love_contribute::insert_item(uint32_t type, uint32_t userid, uint32_t itemid, uint32_t item_cnt, char* nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
    set_mysql_string(nick_mysql, nick, NICK_LEN);

	sprintf(this->sqlstr, "insert into  %s values(%u, %u, '%s', %u, %u, %u)", 
			this->get_table_name(), type, userid, nick, itemid, item_cnt, (uint32_t)time(0));

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Csysarg_love_contribute::delete_item(uint32_t type, uint32_t limit_cnt)
{
	sprintf(this->sqlstr, "delete from %s where type = %u order by date asc limit %u", this->get_table_name(), type, limit_cnt);

	STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
}

int Csysarg_love_contribute::get_contribute_item(uint32_t type, contribute_info** pp_list, uint32_t* count)
{
	sprintf(this->sqlstr, "select userid, nick, contri_itemid from %s where type = %u order by date asc limit 4", this->get_table_name(), type);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->userid);
		MEM_CPY_NEXT_FIELD((*pp_list + i)->nick, NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_list + i)->itemid);
	STD_QUERY_WHILE_END();
}
