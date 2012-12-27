/*
 * =====================================================================================
 *
 *       Filename:  chris_cake.c
 *
 *    Description:  圣诞蛋糕坊之庄园宅急送
 *
 *        Version:  1.0
 *        Created:  12/27/2010 05:09:17 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include <statistic_agent/msglog.h>
#include "dbproxy.h"
#include "util.h"
#include "proto.h"
#include "game_bonus.h"
#include "event.h"
#include "exclu_things.h"
#include "chris_cake.h"

#define WEEKEND_BONUS_TABLE_SIZE 	12
static weekend_bonus_t weekend_bonus[WEEKEND_BONUS_TABLE_SIZE];



static uint32_t login_bonus[][2] = {{0,100},{0,200},{0,500},{1230038,1},{1230054,2},
                                      {0,100},{0,200},{0,500},{1230038,1},{1270042,1},
                                      {0,100},{0,200},{0,500},{1230038,1},{16012,10},
                                      {0,100},{0,200},{0,500},{1230038,1},{1270011,1},
                                      {0,100},{0,200},{0,500},{1230038,1},{1353205,1},
                                      {0,100},{0,200},{0,500},{1230038,1},{1353101,5},
                                      {0,100},{0,200},{0,500},{1230038,1},{190840,30},
                                      {0,100},{0,200},{0,500},{1230038,1},{16012,50},
                                      {0,100},{0,200},{0,500},{1230038,1},{1270078,1},
                                      {0,100},{0,200},{0,500},{1230038,1},{1270079,1},};

/* @brief 得到蛋糕的记录
 */
int get_chris_cake_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	/*type:1表示获取当天做已经送给NPC的蛋糕的数量 
	 *     2表示获取当天蛋糕的信息
	 *     3表示获取全部蛋糕的信息
	 */     
	uint32_t type = 0;
	unpack(body, sizeof(type), "L", &type);
	if (type < 1 || type > 3 || !ISVIP(p->flag)) {
		ERROR_RETURN(("TYPE IS WRONG OR NOT VIP [USER ID %u] [TYPE %u] [VIP %u]",
		p->id, type, ISVIP(p->flag)), -1);
	}

	p->sess_len = 0;
	PKG_H_UINT32(p->session, type, p->sess_len);

	return send_request_to_db(SVR_PROTO_ROOMINFO_GET_CHRIS_CAKE_INFO, p, sizeof(type), &type, p->id);
}

/* @brief 返回蛋糕的信息
 */
int get_chris_cake_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	/*蛋糕的信息*/
	typedef struct chris_cake_info {
		uint32_t date; /* 做蛋糕的日期 */
		uint32_t cake_id; 
		uint32_t state; /* 1表示蛋糕制作完成， 2表示蛋糕送给NPC */
		uint32_t lvl; /* 蛋糕的好评度 */
	}__attribute__((packed)) chris_cake_info_t;

	uint32_t today_make_num = 0; /* 今天用户送给NPC蛋糕的数量, 对应type为1 */
	uint32_t num = 0; /* 如type为2表示今天蛋糕的数量，3表示全部蛋糕的数量 */ 
	unpkg_host_uint32_uint32((uint8_t *)buf, &today_make_num, &num);

	uint32_t type = *(uint32_t *)p->session;
	DEBUG_LOG("userid xxx %u %u", type, p->id);
	if (type == 1){
		response_proto_uint32_uint32(p, p->waitcmd, type, today_make_num, 0);
	} else {
		uint32_t index = sizeof(protocol_t);
		PKG_UINT32(msg, type, index);
		PKG_UINT32(msg, num, index);
		chris_cake_info_t *cake_info = (chris_cake_info_t *)(buf + sizeof(num) + sizeof(today_make_num));
		int i = 0;
		for (i = 0; i < num; i++) {
			PKG_UINT32(msg, (cake_info + i)->date, index);
			PKG_UINT32(msg, (cake_info + i)->cake_id, index);
			PKG_UINT32(msg, (cake_info + i)->state, index);
			PKG_UINT32(msg, (cake_info + i)->lvl, index);
		}

		init_proto_head(msg, p->waitcmd, index);
		return send_to_self(p, msg, index, 1);
	}
	return 0;
}

/* @brief 制作完一个蛋糕
 */
int end_make_one_cake_cmd(sprite_t * p,const uint8_t * body,int len)
{
	uint32_t cake_id = 0;
	uint32_t cake_lvl = 0;
	unpack(body, sizeof(cake_id) + sizeof(cake_lvl), "LL", &cake_id, &cake_lvl);
	/* 分为三个等级：【绝世美味】、【休闲食品】、【危险品！】*/
	if (cake_lvl < 1 || cake_lvl > 3) {
		ERROR_RETURN(("CAKE LEVEL IS WRONT [USER ID %u] [LEVEL %u]", p->id, cake_lvl), -1);
	}

	uint32_t end_cake_info[] = {cake_id, cake_lvl};
	send_request_to_db(SVR_PROTO_ROOMINFO_DONE_CHRIS_CAKE, NULL, sizeof(end_cake_info), end_cake_info, p->id);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

/* @brief 送蛋糕给NPC
 */
int give_cake_to_npc_cmd(sprite_t * p,const uint8_t * body,int len)
{
	uint32_t cake_id = 0;
	unpack(body, sizeof(cake_id), "L", &cake_id);
	return send_request_to_db(SVR_PROTO_ROOMINFO_GIVE_CAKE_TO_NPC, p, sizeof(cake_id), &cake_id, p->id);
}

/* @brief 送蛋糕给NPC的回调函数
 */
int give_cake_to_npc_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t cake_lvl = 0;
	unpkg_host_uint32((uint8_t *)buf, &cake_lvl);
	/* 防错处理，如果不在正常范围好评度就设置为3 */
	if (cake_lvl < 1 || cake_lvl > 3) {
		cake_lvl = 3;
	}

	uint32_t info_m[2] = {p->id, cake_lvl};
	msglog(statistic_logfile, 0x020D2019, now.tv_sec, info_m, sizeof(info_m));
	
	uint32_t game_id = 16; /* 在games_bonus.xml配置的游戏ID，根据好评度送东西 */
	p->waitcmd = PROTO_FIRE_CUP_ADD_GAME_BONUS;	
	*(uint32_t *)p->session = game_id;
	*(uint32_t *)(p->session + sizeof(game_id)) = cake_lvl - 1;
	return send_game_bonus_to_db(p, game_id, cake_lvl - 1);
}


/* @brief 查询蛋糕的积分
 */
int get_chris_cake_score_cmd(sprite_t * p,const uint8_t * body,int len)
{
	return send_request_to_db(SVR_PROTO_ROOMINFO_GET_CHRIS_CAKE_SCORE, p, 0, NULL, p->id);
}

/* @brief 返回蛋糕的积分给CLIENT
 */
int get_chris_cake_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t cake_score = 0; /* 蛋糕获取的积分 */
	unpkg_host_uint32((uint8_t *)buf, &cake_score);	
	response_proto_uint32(p, p->waitcmd, cake_score, 0);
	return 0;
}

/* @brief 蛋糕积分兑换中扣除积分的回调函数, 1246协议中向DB发送请求
 */
int exchange_sth_by_cake_score_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t game_id = *(uint32_t *)p->session;
	uint32_t index = *(uint32_t *)(p->session + sizeof(game_id));
	return send_game_bonus_to_db(p, game_id, index);
}

int get_continue_login_bonus_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
    return send_request_to_db(SVR_PROTO_ROOMINFO_GET_LOGIN_BONUS_INFO, p, 0, NULL, p->id);
}

int get_continue_login_bonus_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
 	typedef struct login_bonus_info {
		uint32_t count; 
		uint32_t total; 
		uint32_t round; 
		uint32_t date; 
		uint32_t flag; 
	}__attribute__((packed)) login_bonus_info_t;

	CHECK_BODY_LEN(len, sizeof(login_bonus_info_t));
	login_bonus_info_t *login_info = (login_bonus_info_t *)(buf);
	if (login_info->count > 5 || login_info->count < 1 || login_info->round < 1)
	{
	    ERROR_LOG("---count:%d, round:%d--", login_info->count, login_info->round);
	    login_info->count = 1;
	    login_info->round = 1;
	}

    login_info->flag = 0; //������¼����¼�
    if (login_info->flag == 1)
    {
        uint32_t info_m[2] = {p->id, login_info->total};
	    msglog(statistic_logfile, 0x02103010, now.tv_sec, info_m, sizeof(info_m));
	
        uint32_t itemid = 0;
        uint32_t count = 0;  
        if (login_info->total > 60)
        {
            itemid = 1353102;
            count = 4;
        }
        else if (login_info->total >= 50 && login_info->total <= 60)
        {
            itemid = 1270079;
            count = 1;
        }
        else
        {
            if (login_info->round > 10)
            {
                login_info->round = 10;
            }
            uint32_t index = (login_info->round - 1)*5 + (login_info->count - 1) ;
            itemid = login_bonus[index][0];
            count = login_bonus[index][1];
        }
        uint8_t buff[1024] = {};
        int j = 0;
        PKG_H_UINT32(buff, 0, j);
        PKG_H_UINT32(buff, 1, j);
        PKG_H_UINT32(buff, 202, j);
        PKG_H_UINT32(buff, 0, j);  
        if (pkg_item_kind(p, buff, itemid, &j) == -1)
        {
            return -1;
        }

        item_t* p_item = get_item_prop(itemid);
        if (!p_item)
        {
            return -1;
        }
        PKG_H_UINT32(buff, p_item->id, j);
        PKG_H_UINT32(buff, count, j); 
        PKG_H_UINT32(buff, p_item->max, j);
        
        send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, buff, p->id);

        int l = sizeof(protocol_t);
        PKG_UINT32(msg, login_info->count, l);  
        PKG_UINT32(msg, login_info->total, l);
        PKG_UINT32(msg, login_info->round, l);
        PKG_UINT32(msg, login_info->date, l);
        PKG_UINT32(msg, login_info->flag, l);
        PKG_UINT32(msg, 1, l);
        PKG_UINT32(msg, itemid, l);
        PKG_UINT32(msg, count, l);
    	init_proto_head(msg, p->waitcmd, l);
    	send_to_self(p, msg, l, 1); 	
    }
    else
    {
        uint32_t info_m[2] = {p->id, login_info->total};
	    msglog(statistic_logfile, 0x02103010, now.tv_sec, info_m, sizeof(info_m));
	    
        int j = sizeof(protocol_t);
        PKG_UINT32(msg, login_info->count, j);  
        PKG_UINT32(msg, login_info->total, j);
        PKG_UINT32(msg, login_info->round, j);
        PKG_UINT32(msg, login_info->date, j);
        PKG_UINT32(msg, login_info->flag, j);
        PKG_UINT32(msg, 0, j);
    	init_proto_head(msg, p->waitcmd, j);
    	send_to_self(p, msg, j, 1);
    }

    return 0;
    
}

int get_30_day_login_bonus_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
    CHECK_BODY_LEN(len, 0);
    uint32_t cur_time = get_now_tv()->tv_sec;
    DEBUG_LOG("-000--last online time:%d  cur_time:%d ollast:%d", p->old_last_time, cur_time, p->ollast);
    if (p->old_last_time && (cur_time > p->old_last_time))
    {     
        if ((cur_time - p->old_last_time) > 30*24*60*60)
        {
            uint32_t itemid = 1353209;
            uint32_t count = 2;
            db_exchange_single_item_op(p, 202, itemid, count, 0);

            uint32_t itemid1 = 1353405;
            uint32_t count1 = 2;
            uint32_t itemid2 = 1353418;
            uint32_t count2 = 2;
            uint32_t dbbuf[] = {2, itemid1, 3, count1, itemid2, 1, count2};
            send_request_to_db(SVR_PROTO_ADD_ANGEL, NULL, sizeof(dbbuf), dbbuf, p->id);
            
            int j = sizeof(protocol_t);
            uint32_t flag = 1;
            PKG_UINT32(msg, flag, j);  
            PKG_UINT32(msg, 3, j);
            PKG_UINT32(msg, itemid, j);
            PKG_UINT32(msg, count, j);
            PKG_UINT32(msg, itemid1, j);
            PKG_UINT32(msg, count1, j);
            PKG_UINT32(msg, itemid2, j);
            PKG_UINT32(msg, count2, j);
            
    	    init_proto_head(msg, p->waitcmd, j);
    	    send_to_self(p, msg, j, 1);
    	    p->old_last_time = cur_time;
    	    msglog(statistic_logfile, 0x02103017,get_now_tv()->tv_sec, &(p->id), sizeof(p->id));
    	    return 0;
        }
    }

    int l = sizeof(protocol_t);
    PKG_UINT32(msg, 0, l);  
    PKG_UINT32(msg, 0, l);
    init_proto_head(msg, p->waitcmd, l);
    send_to_self(p, msg, l, 1);
    return 0;
    
}

int get_weekend_login_bonus_cmd(sprite_t * p,const uint8_t * body,int len)
{
    CHECK_BODY_LEN(len, 8);
    uint32_t week_count = 0;
    uint32_t index = 0;
	unpack(body, sizeof(week_count)+sizeof(index), "LL", &week_count, &index);

	if (week_count > 10)
	{
	    week_count = 10;
	}
	
    if (!is_holiday(get_now_tm()->tm_wday) || week_count > 10 || weekend_bonus[week_count].weekid != week_count
        || index > weekend_bonus[week_count].item_count || index < 1)
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
    }

    *(uint32_t *)p->session = week_count;
	*(uint32_t *)(p->session + 4) = index;
    return send_request_to_db(SVR_PROTO_ROOMINFO_GET_LOGIN_WEEKEND_COUNT, p, 0, NULL, p->id);
}

int get_weekend_login_bonus(sprite_t * p)
{
    uint32_t week_client = 0;
    uint32_t index = 0;    
    week_client = *(uint32_t *)p->session;
	index = *(uint32_t *)(p->session + 4);

    if (week_client > 10)
    {
        week_client = 10;
    }
    uint32_t info_m[2] = {p->id, 1};
	msglog(statistic_logfile, 0x04041080+week_client-1, now.tv_sec, info_m, sizeof(info_m));
	    
    uint32_t itemid = weekend_bonus[week_client].item[index-1].itm->id;
    uint32_t count = weekend_bonus[week_client].item[index-1].count;
    db_exchange_single_item_op(p, 202, itemid, count, 0);
    response_proto_uint32_uint32_uint32(p, p->waitcmd, 1,itemid,count,0); 
    return 0;
}

int get_weekend_continue_login_count_cmd(sprite_t * p,const uint8_t * body,int len)
{
    return send_request_to_db(SVR_PROTO_ROOMINFO_GET_LOGIN_WEEKEND_COUNT, p, 0, NULL, p->id);
}

int get_weekend_continue_login_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    uint32_t week_count = 0;
    uint32_t week_last = 0;
	unpkg_host_uint32_uint32((uint8_t *)buf, &week_count, &week_last);	

	if (p->waitcmd == PROTO_GET_WEEKEND_CONTINUE_LOGIN_COUNT)
	{
	    uint32_t info_m[2] = {p->id, 1};
	    msglog(statistic_logfile, 0x04041008+week_count-1, now.tv_sec, info_m, sizeof(info_m));
	
	    response_proto_uint32(p, p->waitcmd, week_count, 0);
	    return 0;
	}
	
    uint32_t week_client = 0;
    uint32_t index = 0;
    week_client = *(uint32_t *)p->session;
	index = *(uint32_t *)(p->session + 4);

	if (week_count > 10)
    {
        week_count = 10;
    }

    if (week_client != week_count)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	
    uint32_t day_count = 0;
    day_count = weekend_bonus[week_client].item_count;
    return  db_set_sth_done(p, 40006, day_count, p->id); 
	
}

//20120209
int get_another_weekend_login_info_cmd(sprite_t * p,const uint8_t * body,int len)
{

	CHECK_VALID_ID(p->id); 
	return send_request_to_db(SVR_PROTO_USER_GET_WEEKEND_LOGIN_INFO, p, 0, NULL, p->id);
}
int get_another_weekend_login_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{

	typedef struct ret_weekend_info{
		uint32_t count;
		uint32_t flags;
	}ret_weekend_t;

	CHECK_BODY_LEN(len, sizeof(ret_weekend_t));
	ret_weekend_t* info = (ret_weekend_t*)buf;
	switch(p->waitcmd){
		case PROTO_GET_ANOTHER_WEEKEND_LOGIN_INFO:
			{
				uint32_t offset =  sizeof(protocol_t);
				PKG_UINT32(msg, info->count, offset);
				uint32_t low = info->flags;
				PKG_UINT32(msg, low, offset);

				init_proto_head(msg, p->waitcmd, offset);
				return send_to_self(p, msg, offset, 1);
			}

		case PROTO_GET_ANOTHER_WEEKEND_LGOIN_BONUS:
			{
				uint32_t week_count = info->count;
				if (week_count > 9)
				{
					week_count = 9;
				}	
			
				if(week_count > 0 && info->flags == 0){

					uint32_t rev_flag = 1;
					send_request_to_db(SVR_PROTO_USER_GET_WEEKEND_LOGIN_BONUS, NULL, 4, &rev_flag, p->id);
					uint32_t offset =  sizeof(protocol_t);
					PKG_UINT32(msg,  weekend_bonus[week_count].item_count, offset);

					uint32_t index = 0;
					for(; index <  weekend_bonus[week_count].item_count; ++index){
						uint32_t itemid = weekend_bonus[week_count].item[index].itm->id;
						uint32_t count = weekend_bonus[week_count].item[index].count;
						db_exchange_single_item_op(p, 202, itemid, count, 0);

						PKG_UINT32(msg, itemid, offset);
						PKG_UINT32(msg, count, offset);
					}	
					uint32_t msg_buf[] = {p->id, 1};
					msglog(statistic_logfile, 0x0409C327, get_now_tv()->tv_sec, msg_buf, sizeof(msg_buf));
					
					init_proto_head(msg, p->waitcmd, offset);
					return send_to_self(p, msg, offset, 1);					
					
				}
				else{
					return send_to_self_error(p, p->waitcmd, -ERR_weekend_login_award_over, 1);
				}
				
				break;
			}
	}
	return 0;
}

int get_another_weekend_login_bonus_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_VALID_ID(p->id); 
	return send_request_to_db(SVR_PROTO_USER_GET_WEEKEND_LOGIN_INFO, p, 0, NULL, p->id);

}
 
static int parse_weekend_single_item(item_unit_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	int id = 0;
	while (cur) 
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item"))) 
		{
			if (j == 15) 
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

int load_weekend_bonus(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl; 
	int i, err = -1;
	int ex_count;
	
	ex_count = 0;
	memset(weekend_bonus, 0, sizeof (weekend_bonus));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc); 
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode; 
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Week"))) {
			DECODE_XML_PROP_INT(i, cur, "WeekID");
			if (ex_count >= WEEKEND_BONUS_TABLE_SIZE - 1 || i <= 0) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}
			weekend_bonus[i].weekid = i;
			if (weekend_bonus[i].weekid > WEEKEND_BONUS_TABLE_SIZE - 1) {
				ERROR_LOG ("parse %s failed, Count=%d, weekid=%d", file, ex_count, weekend_bonus[i].weekid);
				goto exit;
			}

			chl = cur->xmlChildrenNode;
			if ( (parse_weekend_single_item(weekend_bonus[i].item, &(weekend_bonus[i].item_count), chl) != 0) )
				goto exit;

			ex_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load race bonus item file %s", file);

}



