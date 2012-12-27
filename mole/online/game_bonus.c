#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "game_bonus.h"
#include "mole_dragon.h"
#include "exclu_things.h"
#include "foot_print.h"
#include "fire_cup.h"

#define MAX_GAMEID 100
#define MAX_ITEMS 20
#define GAME_BONUS_TABLE_SIZE 	100

static game_bonus_t games_bonus[GAME_BONUS_TABLE_SIZE];

int add_game_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t gameid = 0;
	uint32_t score = 0;
	uint32_t medal = 0;

	unpack(body, sizeof(gameid) + sizeof(score), "LL", &gameid, &score);
	if (gameid > MAX_GAMEID || gameid <= 0 || games_bonus[gameid-1].game_id != gameid)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_game_id, 1);
	}

	DEBUG_LOG ("-----id: %u gameid:%d, score:%d-----", p->id, gameid, score);

    if (gameid >= 17 && gameid <= 28)
    {
	    msglog(statistic_logfile, 0x04040600,get_now_tv()->tv_sec, &score, sizeof(score));
    }

	/*
	if(gameid == 49){
		db_exchange_single_item_op(p, 202, 1351334, 1, 0);
	}
	*/
	if(gameid == 50){
		db_exchange_single_item_op(p, 202, 1351339, 1, 0);
	}
	if(gameid == 51){
		db_exchange_single_item_op(p, 202, 1351336, 1, 0);
	}
	if(gameid == 52){
		db_exchange_single_item_op(p, 202, 1351343, 1, 0);
	}
	if(gameid == 53){
		db_exchange_single_item_op(p, 202, 1351341, 1, 0);
	}

	/*如果匹配不到，则给零的下标*/
	*(uint32_t*)p->session 		 = gameid;
	*(uint32_t*)(p->session + 4) = 0;
	*(uint32_t*)(p->session + 8) = 0;

    uint32_t flag = 0;
	game_bonus_t bonus_t = games_bonus[gameid-1];
	int i = 0;
	for (i = 0; i < bonus_t.score_count; i++)
	{
		if (score >= bonus_t.score[i].start && score <= bonus_t.score[i].end)
		{
			medal = bonus_t.score[i].medal;
			*(uint32_t*)p->session 		 = gameid;
			*(uint32_t*)(p->session + 4) = i;
			*(uint32_t*)(p->session + 8) = medal;
			flag = 1;
			break ;
		}
	}

	if (!flag)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_game_id, 1);
	}

	/* 需要根据不同的需求修改这边的参数 */
	if (bonus_t.cmd == 46488) {
		/* 扣除蛋糕兑换的分数 扣除脚印数量*/
		uint32_t buff[] = {score};
		return send_request_to_db(bonus_t.cmd, p, sizeof(buff), buff, p->id);
	} else if (bonus_t.cmd == 4375) {
		/* 卡酷T系列, 游戏需要扣除两个爆豆种子 */
		uint32_t buff[] = {0, 1230068, 2, 99999, 0};
		return send_request_to_db(bonus_t.cmd, p, sizeof(buff), buff, p->id);
	/*校验每天上限*/
	} else if (bonus_t.score[i].day_type) {
		if(medal == 0) {
			return db_set_sth_done(p, bonus_t.score[i].day_type, bonus_t.score[i].day_count, p->id);
		} else {
			return db_set_sth_done_cnt(p, bonus_t.score[i].day_type, bonus_t.score[i].day_count, medal, p->id);
		}
	} else {
		return send_game_bonus_to_db(p, gameid, i);
	}
}

/* @brief cmd命令的回调函数
 * @note 可以根据实际的需求改动
 */
int cmd_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t game_id = *(uint32_t *)p->session;
	uint32_t index = *(uint32_t *)(p->session + 4);
	return db_set_sth_done(p, games_bonus[game_id - 1].score[index].day_type,
		   games_bonus[game_id - 1].score[index].day_count, p->id);
}

/* @brief 校验每天上限的回调函数
 */
int game_bonus_day_limit_callback(sprite_t *p)
{
	uint32_t game_id = *(uint32_t *)p->session;
	uint32_t index = *(uint32_t *)(p->session + 4);
	return send_game_bonus_to_db(p, game_id, index);
}

static int parse_game_single_item(item_unit_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	int id = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item")))
		{
			if (j == MAX_ITEMS)
			{
				ERROR_RETURN(("too many items"), -1);
			}

			DECODE_XML_PROP_INT (id, cur, "ID");

			if (!(iut[j].itm = get_item_prop(id)))
			{
				ERROR_RETURN(("can't find item=%d", id), -1);
			}

			DECODE_XML_PROP_INT (iut[j].count, cur, "Count");

			j++;
		}

		cur = cur->next;
	}

	*cnt = j;

	return 0;
}

static int parse_game_scores(game_bonus_t *git, uint32_t* cnt, xmlNodePtr chl)
{
	xmlNodePtr cur;
	int i = 0;

	while (chl)
	{
		if (!xmlStrcmp(chl->name, (const xmlChar *)"Score"))
		{
			DECODE_XML_PROP_INT(git->score[i].start, chl, "Start");
			DECODE_XML_PROP_INT(git->score[i].end, chl, "End");
			DECODE_XML_PROP_INT_DEFAULT(git->score[i].medal, chl, "Medal", 0);
			DECODE_XML_PROP_INT_DEFAULT(git->score[i].day_type, chl, "DayType", 0);
			DECODE_XML_PROP_INT_DEFAULT(git->score[i].day_count, chl, "DayCount", 0);

			cur = chl->xmlChildrenNode;
			if (parse_game_single_item(git->score[i].item, &(git->score[i].item_count), cur) != 0)
			{
				return -1;
			}

			i++;
		}

		chl = chl->next;
	}

	*cnt = i;

	return 0;

}

int load_game_bonus(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int ex_count;

	ex_count = 0;
	memset(games_bonus, 0, sizeof (games_bonus));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Game"))) {
			DECODE_XML_PROP_INT(i, cur, "GameID");
			if (ex_count >= GAME_BONUS_TABLE_SIZE - 1 || i <= 0) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}
			games_bonus[i-1].game_id = i;
			DECODE_XML_PROP_INT_DEFAULT(games_bonus[i-1].cmd, cur, "CMD", 0);
			if (games_bonus[i-1].game_id > MAX_GAMEID) {
				ERROR_LOG ("parse %s failed, Count=%d, game_id=%d", file, ex_count, games_bonus[i-1].game_id);
				goto exit;
			}

			chl = cur->xmlChildrenNode;
			if ( (parse_game_scores(&games_bonus[i-1], &(games_bonus[i-1].score_count), chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load game bonus item file %s", file);

}

int send_game_bonus_to_db(sprite_t *p, uint32_t gameid, uint32_t i)
{
	game_bonus_t bonus_t = games_bonus[gameid-1];
	score_item_t score_item = bonus_t.score[i];

	uint8_t buff[1024] = {};
	int j = 0;
	int k = 0;
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, score_item.item_count, j);
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 0, j);
	for (k = 0; k < score_item.item_count; k++)
	{
		if (pkg_item_kind(p, buff, score_item.item[k].itm->id, &j) == -1)
		{
			return -1;
		}
		PKG_H_UINT32(buff, score_item.item[k].itm->id, j);
		PKG_H_UINT32(buff, score_item.item[k].count, j);
		PKG_H_UINT32(buff, score_item.item[k].itm->max, j);
		//add_fire_cup_medal_count(p, score_item.item[k].itm->id, score_item.item[k].count);
		//add_lahm_sport_medal_count(p, score_item.item[k].itm->id, score_item.item[k].count);
	}

	send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);

	return 0;

}

int send_game_bonus_to_client(sprite_t *p, uint32_t gameid, uint32_t i)
{
    exchange_foot_print_msglog(p, gameid, i);

	uint32_t medal = 0;
	game_bonus_t bonus_t = games_bonus[gameid-1];
	score_item_t score_item = bonus_t.score[i];
	medal = score_item.medal;

	int len = sizeof(protocol_t);
    /*if (bonus_t.cmd)
    {
	    PKG_UINT32(msg, score_item.item_count + 1, len);
	    PKG_UINT32(msg, 8, len);
	    PKG_UINT32(msg, medal, len);
    }
    else
    { */
    PKG_UINT32(msg, score_item.item_count, len);
	//}

	int k = 0;

	for (k = 0; k < score_item.item_count; k++)
	{
		PKG_UINT32(msg, score_item.item[k].itm->id, len);
		if (((p->dragon_id == 0) || is_dragon_adult(p)) && (score_item.item[k].itm->id == 7))
		{
			PKG_UINT32(msg, 0, len);
		}
		else
		{
		    if ((p->dragon_id != 0) && (score_item.item[k].itm->id == 7))
		    {
		        p->dragon_growth = p->dragon_growth + score_item.item[k].count;
		    }
			PKG_UINT32(msg, score_item.item[k].count, len);
		}
	}

	if(gameid >= 4 && gameid <= 7) {
	    uint32_t msg_buff[7] = {0,0,0,0,0,1,p->id};
		msglog(statistic_logfile, 0x02041000 + 0x4B+gameid-4,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}

    if(gameid == 29) {
	    uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x4040B01,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		if (i == 0)
		{
		    msglog(statistic_logfile, 0x2042201,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
	}
	else if(gameid == 30) {
	    uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x4040B00,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (gameid == 32)
	{
	    uint32_t msg_buff[7] = {0,0,0,0,0,1,p->id};
		msglog(statistic_logfile, 0x04040B03,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (gameid == 40)
	{
	    uint32_t msg_buff[7] = {0,0,0,0,0,1,p->id};
		msglog(statistic_logfile, 0x0409B3F2,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (gameid >= 41 && gameid <= 44)
	{
	    uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409BD7C + gameid - 41, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (gameid == 45)
	{
	    uint32_t msg_buff[2] = {p->id, 1};
	    if (ISVIP(p->flag))
		{
		    msglog(statistic_logfile, 0x0409BD92, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
		else
		{
		    msglog(statistic_logfile, 0x0409BD93, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
		}
	}
	else if (gameid == 47)
	{
	    uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C352, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (gameid == 48)
	{
	    uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C351, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (gameid == 50)
	{
	    uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C38E, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (gameid == 51)
	{
	    uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C38B, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (gameid == 52)
	{
	    uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C390, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if (gameid == 54)
	{
	    uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C44B, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}

	if (gameid == 16) {
		p->waitcmd = PROTO_GIVE_CAKE_TO_NPC;
	}
	init_proto_head(msg, p->waitcmd, len);
	send_to_self(p, msg, len, 1);
	DEBUG_LOG("send game cake");
	return 0;

}

/* @brief 根据游戏ID和分数，打包返回给DB端的物品ID及数量
 * @param buff 保存打包数据的数组
 * @param index 打包数组的下标
 * @param game_bonus_id 配置文件里的ID号
 * @param score 游戏获取的分数
 * @param itmid 除了在配置文件里配置送的物品以外，其他送的物品
 * @note game_bonus_id 的具体数值是在games.xml文件里配置的
 */
int sendto_db_game_items(sprite_t *p, uint8_t *body)
{
	uint32_t count;
	uint32_t itemid;
	uint32_t itemcnt;

	int j=0;
	UNPKG_UINT32(body,count,j);
	if(count == 0) return 0;

	int bytes = 0;
	uint8_t buff[1024];
	PKG_H_UINT32(buff, 0, bytes);
	PKG_H_UINT32(buff, count,bytes);
	PKG_H_UINT32(buff, 0, bytes);
	PKG_H_UINT32(buff, 0, bytes);

	int k = 0;
	for (k = 0; k < count; k++) {
		UNPKG_UINT32(body,itemid,j);
		UNPKG_UINT32(body,itemcnt,j);

		if (pkg_item_kind(p, buff,itemid, &bytes) == -1) {
			return -1;
		}

		item_t *itm = get_item_prop(itemid);
		if(!itm) {
			return -1;
		}

		PKG_H_UINT32(buff, itemid, bytes);
		PKG_H_UINT32(buff, itemcnt, bytes);
		PKG_H_UINT32(buff, itm->max, bytes);
	}
	DEBUG_LOG("game bonus %u", p->id);
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM,NULL,bytes,buff,p->id);
}

/* @brief 根据游戏ID和分数，打包返回给客户端的物品ID及数量
 * @param buff 保存打包数据的数组
 * @param index 打包数组的下标
 * @param game_bonus_id 配置文件里的ID号
 * @param score 游戏获取的分数
 * @param itmid 除了在配置文件里配置送的物品以外，其他送的物品
 * @note game_bonus_id 的具体数值是在games.xml文件里配置的
 */
int pack_client_itmid_accord_id_score(sprite_t *p, uint8_t buff[], int *index, int game_bonus_id, int score, int itmid)
{
	if (game_bonus_id != 0) {
		int i = 0;
		for (i = 0; i < games_bonus[game_bonus_id - 1].score_count; i++) {
			if (score >= games_bonus[game_bonus_id - 1].score[i].start
				&& score <= games_bonus[game_bonus_id - 1].score[i].end) {
				break ;
			}
		}

		if(i == games_bonus[game_bonus_id - 1].score_count)  {
			PKG_UINT32(buff, 0, *index);
			return 0;
		}

		PKG_UINT32(buff, games_bonus[game_bonus_id - 1].score[i].item_count + (!!itmid), *index);

		int k = 0;
		for (k = 0; k < games_bonus[game_bonus_id - 1].score[i].item_count; k++) {
			PKG_UINT32(buff, games_bonus[game_bonus_id - 1].score[i].item[k].itm->id, *index);
			PKG_UINT32(buff, games_bonus[game_bonus_id - 1].score[i].item[k].count, *index);
		}
	}

	if (itmid && game_bonus_id) {
		PKG_UINT32(buff, itmid, *index);
		/*数量默认为1*/
		PKG_UINT32(buff, 1, *index);
	} else if (itmid) {
		PKG_UINT32(buff, 1, *index);
		PKG_UINT32(buff, itmid, *index);
		PKG_UINT32(buff, 1, *index);
	}
	return 0;
}
