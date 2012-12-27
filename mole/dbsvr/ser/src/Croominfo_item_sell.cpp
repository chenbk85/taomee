/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_item_sell.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2010 07:44:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croominfo_item_sell.h"

struct sell_max_tag {
	uint32_t npc_id;
	uint32_t itemid;
	uint32_t maxcnt;
};

static struct sell_max_tag sell_max[] = {
	{0,190672,1},
    {0,190673,1},
    {0,190674,1}
};

Croominfo_item_sell::Croominfo_item_sell(mysql_interface * db)
	:CtableRoute10x10(db, "ROOMINFO", "t_roominfo_item_sell", "userid")
{

}

int Croominfo_item_sell::get_npc_item_count(uint32_t npcid)
{
    int i = 0;
	int itm_cnt = 0;
	int count = sizeof(sell_max) / sizeof(sell_max[0]);

	while( i < count) {
		if(sell_max[i].npc_id == npcid) itm_cnt++;
		i++;
	}

	return itm_cnt;
}

int Croominfo_item_sell::get_item_sell_max(uint32_t npcid,uint32_t itemid)
{
	int i = 0;
	int count = sizeof(sell_max) / sizeof(sell_max[0]);

	while( i < count) {
		if( sell_max[i].npc_id == npcid &&
			sell_max[i].itemid == itemid )
		   	return sell_max[i].maxcnt;
		i++;
	}

	return 0;
}

int Croominfo_item_sell::insert_add(userid_t userid, uint32_t npcid,uint32_t itemid,uint32_t count)
{
	sprintf(this->sqlstr, "insert into %s values(%u,%u,%u,%u,%u)",
        this->get_table_name(userid),userid,npcid,itemid,count,get_date(time(NULL)));

    STD_SET_RETURN_EX(this->sqlstr, ROOMINFO_ADD_ITEM_SELL_ERR);
}

int Croominfo_item_sell::update_now(userid_t userid,uint32_t npcid)
{
	uint32_t date = get_date(time(NULL));
	sprintf(this->sqlstr, "update %s set count=0,date=%u where userid=%u and npcid=%u and date != %u",
		this->get_table_name(userid),date,userid,npcid,date);

	STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
}

int Croominfo_item_sell::update_add(userid_t userid, uint32_t npcid,uint32_t itemid,uint32_t count)
{
	sprintf(this->sqlstr, "update %s set count=count+%u where userid=%u and npcid=%u and itemid=%u",
	this->get_table_name(userid),count,userid,npcid,itemid);

    STD_SET_RETURN_EX(this->sqlstr, ROOMINFO_FIND_ITEM_SELL_ERR);
}

int Croominfo_item_sell::get(userid_t userid, uint32_t npcid,uint32_t itemid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count from %s where userid=%u and npcid=%u and itemid=%u",
        this->get_table_name(userid),userid,npcid,itemid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, ROOMINFO_FIND_ITEM_SELL_ERR);
       INT_CPY_NEXT_FIELD(count);
    STD_QUERY_ONE_END();
}

int Croominfo_item_sell::get_sell_list(userid_t userid, uint32_t npcid, roominfo_get_item_sell_out_item **p_out, uint32_t *count)
{
	sprintf(this->sqlstr, "select itemid,count from %s where userid=%u and npcid=%u",
		this->get_table_name(userid),userid,npcid);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out, count);
		INT_CPY_NEXT_FIELD((*p_out + i)->itemid);
		INT_CPY_NEXT_FIELD((*p_out + i)->count);
	STD_QUERY_WHILE_END();
}

int Croominfo_item_sell::get_max_list(uint32_t npcid,roominfo_get_item_sell_out_item **p_out)
{
	
	int i = 0;
	int j = 0;

	int npc_cnt = get_npc_item_count(npcid);
	*p_out = (roominfo_get_item_sell_out_item *)malloc(npc_cnt * sizeof(roominfo_get_item_sell_out_item));

    int count = sizeof(sell_max) / sizeof(sell_max[0]);
    while(i < count && j < npc_cnt) {
		if(sell_max[i].npc_id == npcid) {
        	(*p_out + j)->itemid = sell_max[i].itemid;
			(*p_out + j)->count  = sell_max[i].maxcnt;
 			j++;
		}		
        i++;
    }

    return npc_cnt;
}

