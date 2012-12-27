/*
 * =====================================================================================
 *
 *       Filename:  Csysarg_love_auction.cpp
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

#include "Csysarg_love_auction.h"
#include "common.h"
#include <algorithm>

//NPC id和itemid
const uint32_t normal_npc_items[][2] = {
	{1, 1220157},	//梅森
	{2, 190802},	//花婶
	{3, 190749},	//超级小猪
	{4, 190803},	//克劳
	{5, 190748},	//菩提大伯
	{6, 1593028},	//臭臭
	{7, 1270055},	//尤尤
	{7, 1270063},	//尤尤
	{8, 1593026},	//臭臭2号
	{9, 1593037},	//大卫
	{10, 1593041},	//丝尔特
	{11, 1353223},	//艾米
	{12, 1353226},	//西索
	{13, 1353255},	//凯文
	{7, 1270012},	//尤尤
	{1, 1230044},	//梅森
	{1, 1230026},	//梅森
	{14, 1270021},	//大白熊
	{15, 1593021},	//猎人M
	{15, 1593061},	//猎人M
};

const uint32_t spicial_npc_items[][2] = {
	{16, 1270078},	//丫丽
	{16, 1270079},	//丫丽
	{17, 1270066},	//玛丽
	{18, 1270072},	//么么公主
	{19, 1270073},	//捷克王子
	{14, 1270038},	//大白熊
	{10, 1270010},	//丝尔特
	{20, 1270058},	//伊莲
	{5, 1593034},	//菩提大伯
	{6, 1593065},	//臭臭
	{21, 1353296},	//哈哈魔法师
	{22, 1353297},	//彩虹姐姐
	{23, 1353271},	//粒粒
};

#define NORMAL_NPC_CNT	(sizeof(normal_npc_items) / sizeof(normal_npc_items[0]))
#define SPICIAL_NPC_CNT	(sizeof(spicial_npc_items) / sizeof(spicial_npc_items[0]))
#define NORMAL_RAND()  (rand() % 100 < 85)

Csysarg_love_auction::Csysarg_love_auction(mysql_interface * db) :
	Ctable(db, "SYSARG_DB","t_sysarg_love_auction")
{

}

int Csysarg_love_auction::get_auction_list(uint32_t *count, sysarg_get_auction_list_out_item** pp_list, uint32_t old_flag, uint32_t* start_time)
{
	sprintf(this->sqlstr, "select auction_itemid, contri_userid, contri_nick, auction_userid, auction_nick, money,"
			"start_time from %s where is_old_flag = %u limit 20", this->get_table_name(), old_flag);

	*start_time = 0;
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->auction_itemid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->contri_userid);
		MEM_CPY_NEXT_FIELD((*pp_list + i)->contri_nick, NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_list + i)->auction_userid);
		MEM_CPY_NEXT_FIELD((*pp_list + i)->auction_nick, NICK_LEN);
		INT_CPY_NEXT_FIELD((*pp_list + i)->auction_money);
		INT_CPY_NEXT_FIELD(*start_time);
	STD_QUERY_WHILE_END();
}

/*
 * @brief 竞拍列表，先删除旧一轮的，然后设置当前一轮位上一轮，然后插入新的一轮，如果不够20个，随机生成NPC的物品插入
 */
int Csysarg_love_auction::renew_auction_list(uint32_t count, contribute_info *p_list)
{
	int ret = this->delete_item();
	ret = this->set_old_flag();
	uint32_t cur_time = (uint32_t)time(0);
	uint32_t record_cnt = 0;

	for (uint32_t i = 0; i < count; i++) {
		ret = this->insert_item((p_list+i)->itemid, (p_list+i)->userid, (p_list+i)->nick, cur_time);
		if (ret == SUCC) {
			record_cnt++;
		}
	}

	if (record_cnt < 20) {
		uint32_t npc_idx[NORMAL_NPC_CNT] = {0};
		for (uint32_t i = 0; i < NORMAL_NPC_CNT; i++) {
			npc_idx[i] = i;
		}
		std::random_shuffle(npc_idx, npc_idx + NORMAL_NPC_CNT);
		
		if (!NORMAL_RAND()) {
			uint32_t spicial_idx = rand() % SPICIAL_NPC_CNT;
			ret = this->insert_item(spicial_npc_items[spicial_idx][1], spicial_npc_items[spicial_idx][0], (char*)" ", cur_time);
			if (ret == SUCC) {
				record_cnt++;
			}
		}

		for (uint32_t i = 0; record_cnt < 20 && i < NORMAL_NPC_CNT; i++) {
			ret = this->insert_item(normal_npc_items[npc_idx[i]][1], normal_npc_items[npc_idx[i]][0], (char*)" ", cur_time);
			if (ret == SUCC) {
				record_cnt++;
			}
		}
	}

	return SUCC;
}

int Csysarg_love_auction::insert_item(uint32_t itemid, uint32_t contri_userid, char* contri_nick, uint32_t cur_time)
{
	char nick_mysql[mysql_str_len(NICK_LEN)] = {0};
	memset(nick_mysql, 0, sizeof(nick_mysql));
	set_mysql_string(nick_mysql, contri_nick, strlen(contri_nick));

	sprintf(this->sqlstr, "insert into %s values(0, %u, %u, '%s', 0, 0x00000000, 0, 0, %u)", 
			this->get_table_name(), itemid, contri_userid, nick_mysql, cur_time);
    STD_INSERT_RETURN(this->sqlstr, SYSARG_AUCTION_USER_EXIST_ERR);
}

int Csysarg_love_auction::delete_item()
{
	sprintf(this->sqlstr, "delete from %s where is_old_flag = 1", this->get_table_name());
	STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
}

int Csysarg_love_auction::get_auction_item(uint32_t itemid, auction_list_info* p_out)
{
	sprintf(this->sqlstr, "select auction_itemid, contri_userid, auction_userid, money from %s "
			"where auction_itemid = %u and is_old_flag = 0 limit 1", this->get_table_name(), itemid);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(p_out->auction_itemid);
		INT_CPY_NEXT_FIELD(p_out->contri_userid);
		INT_CPY_NEXT_FIELD(p_out->auction_userid);
		INT_CPY_NEXT_FIELD(p_out->auction_money);
    STD_QUERY_ONE_END();
}

int Csysarg_love_auction::set_auction_item(uint32_t itemid, uint32_t userid, uint32_t money, char* auction_nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)] = {0};
	set_mysql_string(nick_mysql, auction_nick, NICK_LEN);

	sprintf(this->sqlstr, "update %s set auction_userid = %u, money = %u, auction_nick = '%s'"
			"where auction_itemid = %u and is_old_flag = 0", this->get_table_name(), userid, money, nick_mysql, itemid);

	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

int Csysarg_love_auction::set_old_flag()
{
	sprintf(this->sqlstr, "update %s set is_old_flag = 1 where is_old_flag = 0", this->get_table_name());
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}
