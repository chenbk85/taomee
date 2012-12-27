#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "exclu_things.h"
#include "christmas_sock.h"

#define SOCK_ITEM_ID 1351175

int add_christmas_sock_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    return send_request_to_db(SVR_PROTO_ROOMINFO_GET_CHRISTMAS_SOCK_INFO, p, 0, NULL, p->id);
}

int christmas_sock_add_count_to_db(sprite_t* p)
{
    uint32_t year = get_now_tm()->tm_year + 1900;
    uint32_t month= get_now_tm()->tm_mon + 1;
    uint32_t day = get_now_tm()->tm_mday;
    uint32_t cur_date = year*10000+month*100+day;

    uint32_t db_buff[2] = {0};
    db_buff[0] = 1;
    db_buff[1] = cur_date;

    send_request_to_db(SVR_PROTO_ROOMINFO_ADD_CHRISTMAS_SOCK, NULL, 8, db_buff, p->id);
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}


int get_christmas_sock_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_ROOMINFO_GET_CHRISTMAS_SOCK_INFO, p, 0, NULL, p->id);
}


int get_rand_itemid( )
{
    int val = rand()%10;
    if (val < 5)
    {
        int itemids[] = {150011,190755,190756};
        int index_v = rand()%(sizeof(itemids)/4);
        int itemid = itemids[index_v];
        return itemid;
    }
    else if (val < 7)
    {
        int itemids[] = {190809,1230057};
        int index_v = rand()%(sizeof(itemids)/4);
        int itemid = itemids[index_v];
        return itemid;
    }
    else
    {
        int itemids[] = {1230057,1270001,1270005,1270039};
        int index_v = rand()%(sizeof(itemids)/4);
        int itemid = itemids[index_v];
        return itemid;
    }

}

int get_christmas_sock_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 8);
	int i = 0;
	uint32_t count = 0;
	uint32_t date = 0;
	UNPKG_H_UINT32(buf,count,i);
	UNPKG_H_UINT32(buf,date,i);
	uint32_t year = get_now_tm()->tm_year + 1900;
    uint32_t month= get_now_tm()->tm_mon + 1;
    uint32_t day = get_now_tm()->tm_mday;
    uint32_t cur_date = year*10000+month*100+day;

	if (p->waitcmd == PROTO_CHRISTMAS_SOCK_GET_SOCK_INFO)
	{
	    date = date%100;
	    int l = sizeof(protocol_t);
	    PKG_UINT32(msg,count,l);
        PKG_UINT32(msg,date,l);
	    init_proto_head(msg,p->waitcmd,l);
	    return send_to_self(p,msg,l,1);
	}

    if (p->waitcmd == PROTO_CHRISTMAS_SOCK_ADD_SOCK)
	{
	    if (count >= 5)
	    {
	        return send_to_self_error(p, p->waitcmd, -ERR_christmas_sock_add_max_today, 1);
	    }

	    if ((count != 0) && (date != cur_date))
	    {
	        return send_to_self_error(p, p->waitcmd, -ERR_christmas_sock_get_yestoday_bonus_first, 1);
	    }

	    uint32_t itemid = 190808;
        uint32_t count = 1;

        uint8_t buff[1024] = {};
        int j = 0;
        PKG_H_UINT32(buff, 1, j);
        PKG_H_UINT32(buff, 0, j);
        PKG_H_UINT32(buff, 202, j);
        PKG_H_UINT32(buff, 0, j);
        if (pkg_item_kind(p, buff, itemid, &j) == -1)
        {
            return -1;
        }
        PKG_H_UINT32(buff, itemid, j);
        PKG_H_UINT32(buff, count, j);

        return  send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);

	}

    if ((count == 0) || (cur_date <= date))
    {
        return send_to_self_error(p, p->waitcmd, -ERR_christmas_sock_cannot_get_bonus, 1);
    }

    int itm_count = 0;
    itm_count = get_item_count(count);
    if (itm_count)
    {
        p->sess_len = 0;
	    PKG_H_UINT32(p->session, itm_count, p->sess_len);

        uint8_t buff[1024] = {};
	    int j = 0;
	    PKG_H_UINT32(buff, 0, j);
        PKG_H_UINT32(buff, itm_count, j);
        PKG_H_UINT32(buff, 202, j);
        PKG_H_UINT32(buff, 0, j);
        uint32_t i = 0;
        for (i = 0; i < itm_count; i++)
        {
            int itemid = get_rand_itemid();
            item_t* itm = get_item_prop(itemid);
            if (!itm)
            {
                DEBUG_LOG("--itemid %d----", itemid);
                return -1;
            }
            if (pkg_item_kind(p, buff, itm->id, &j) == -1)
            {
                return -1;
            }

            PKG_H_UINT32(buff, itm->id, j);
            PKG_H_UINT32(buff, 1, j);
            PKG_H_UINT32(buff, itm->max, j);

            PKG_H_UINT32(p->session, itm->id, p->sess_len);
            PKG_H_UINT32(p->session, 1, p->sess_len);
        }

        send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);
    }
    else
    {
        send_request_to_db(SVR_PROTO_ROOMINFO_DEL_CHRISTMAS_SOCK_INFO, NULL, 0, NULL, p->id);
        response_proto_uint32(p, p->waitcmd, itm_count, 0);
    }

    return 0;

}

int get_christmas_sock_bonus_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_ROOMINFO_GET_CHRISTMAS_SOCK_INFO, p, 0, NULL, p->id);
}

int get_item_count(uint32_t sock_count)
{
    int rand_val = rand()%10;
    switch(sock_count)
    {
        case 1:
        {
            if (rand_val < 5)
            {
                return 1;
            }
        }
        case 2:
        {
            if (rand_val < 7)
            {
                return 1;
            }
            else
            {
                return 2;
            }
        }
        case 3:
        {
            if (rand_val < 4)
            {
                return 1;
            }
            else
            {
                return 2;
            }
        }
        case 4:
        {
            if (rand_val < 2)
            {
                return 1;
            }
            else if(rand_val < 6)
            {
                return 2;
            }
            else
            {
                return 3;
            }
        }
        case 5:
        {
            if (rand_val < 2)
            {
                return 1;
            }
            else if(rand_val < 5)
            {
                return 2;
            }
            else if(rand_val < 9)
            {
                return 3;
            }
            else
            {
                return 4;
            }
        }
        default:
        {
            return 0;
        }
    }
    return 0;
}

int christmas_sock_send_bonus_to_client(sprite_t * p)
{
    DEBUG_LOG("--userid %u --", p->id);
    send_request_to_db(SVR_PROTO_ROOMINFO_DEL_CHRISTMAS_SOCK_INFO, NULL, 0, NULL, p->id);

    int j = sizeof(protocol_t);

    uint32_t i = 0;
    for (i = 0; i < p->sess_len ; i++)
    {
        PKG_UINT32(msg, *(uint32_t*)(p->session + i*4), j);
    }

	init_proto_head(msg, p->waitcmd, j);
	send_to_self(p, msg, j, 1);
	return 0;
}

//2011圣诞活动
int hang_christmas_sock_cmd(sprite_t* p, uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t friend_id = 0;
	int i = 0;
	UNPKG_UINT32(body, friend_id, i);
	CHECK_VALID_ID(friend_id);

#ifndef TW_VER
	if (get_today() >= 20111225)
#else
	if (get_today() > 20111231)
#endif
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if (friend_id == p->id) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	*(uint32_t*)p->session = friend_id;
	//去好友家检查自己送了几次，并查看好友收到多少袜子
	return send_request_to_db(SVR_PROTO_CHECK_CHRISTMAS_SOCK_CNT, p, sizeof(p->id), &(p->id), friend_id);
}

int reply_hang_christmas_sock(sprite_t* p, uint32_t ret_state)
{
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int check_christmas_sock_cnt_callback(sprite_t* p, uint32_t id, const char *buf, int len)
{
	typedef struct {
		uint32_t is_friend;
		uint32_t self_sock_cnt;
		uint32_t all_cnt;
	}__attribute__((packed)) christmas_sock_cnt_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(christmas_sock_cnt_cbk_pack_t));

	christmas_sock_cnt_cbk_pack_t* cbk_pack = (christmas_sock_cnt_cbk_pack_t*)buf;
	DEBUG_LOG("check hang christmas sock: uid[%u] fid[%u] is_friend[%u] sock_cnt[%u] all_cnt[%u]", p->id, id, cbk_pack->is_friend, cbk_pack->self_sock_cnt, cbk_pack->all_cnt);
	if (cbk_pack->is_friend == 0) {
		uint32_t ret_state = 3;//达到3次限制
		return reply_hang_christmas_sock(p, ret_state);
	}	
	if (cbk_pack->self_sock_cnt >= 3) {
		uint32_t ret_state = 1;//达到3次限制
		return reply_hang_christmas_sock(p, ret_state);
	}
	
	*(uint32_t*)(p->session + 4) = cbk_pack->all_cnt;
	uint8_t db_buf[1024] = {0};
    int buf_len = 0;
    PKG_H_UINT32(db_buf, 99, buf_len);
    PKG_H_UINT32(db_buf, SOCK_ITEM_ID, buf_len);
    PKG_H_UINT32(db_buf, SOCK_ITEM_ID + 1, buf_len);
    PKG_H_UINT8(db_buf, 0, buf_len);
    return send_request_to_db(SVR_PROTO_CHK_ITEM, p, buf_len, db_buf, p->id);
}

int hang_christmas_scok_callback(sprite_t* p, uint32_t id, const char *buf, int len)
{
	uint32_t count;
    int i = 0;
    UNPKG_H_UINT32(buf, count, i);
	uint32_t ret_state = 0;
    if (count == 0) {
		ret_state = 2;
        return reply_hang_christmas_sock(p, ret_state);
    }

	uint32_t friend_id = *(uint32_t*)p->session;
	uint32_t all_cnt = *(uint32_t*)(p->session + 4);

	typedef struct {
		uint32_t type;
		uint32_t userid;
	}__attribute__((packed)) db_hang_christmas_scok_package_t;
	db_hang_christmas_scok_package_t package = {0};
	{//跟别人挂的记录
		package.type = 1;
		package.userid = friend_id;
		send_request_to_db(SVR_PROTO_HANG_CHRISTMAS_SOCK, NULL, sizeof(package), &package, p->id);
	}
	{//别人挂的记录
		package.type = 2;
		package.userid = p->id;
		send_request_to_db(SVR_PROTO_HANG_CHRISTMAS_SOCK, NULL, sizeof(package), &package, friend_id);
	}

	{//top10设置
		typedef struct {
			uint32_t userid;
			uint32_t all_cnt;
		}__attribute__((packed)) db_christmas_scok_top10_package_t;
		db_christmas_scok_top10_package_t top10_package= {0};
		top10_package.userid = friend_id;
		top10_package.all_cnt = all_cnt + 1;
		DEBUG_LOG("hand christmas sock: uid[%u] fid[%u] cnt[%u]", p->id, friend_id, top10_package.all_cnt);
		send_request_to_db(SVR_PROTO_SET_CHRISTMAS_SOCK_TOP10, NULL, sizeof(top10_package), &top10_package, p->id);
	}

	uint32_t msgbuf[4] = {0};
	if (ISVIP(p->flag)) {
		msgbuf[0] = p->id;
		msgbuf[1] = 1;
	} else {
		msgbuf[2] = p->id;
		msgbuf[3] = 1;
	}
	msglog(statistic_logfile, 0x0409BF24, get_now_tv()->tv_sec, msgbuf, sizeof(msgbuf));

	return reply_hang_christmas_sock(p, ret_state);
}

int get_christmas_sock_record_cmd(sprite_t * p,const uint8_t * body, int len)
{
	typedef struct {
		uint32_t type;
		uint32_t page;
	}__attribute__((packed))christmas_sock_record_package_t;
	christmas_sock_record_package_t package = {0};
	int i = 0;
	CHECK_BODY_LEN(len, 8);
	UNPKG_UINT32(body, package.type, i);
	UNPKG_UINT32(body, package.page, i);
	
	if (package.type > 2 || package.page == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	return send_request_to_db(SVR_PROTO_GET_CHRISTMAS_SOCR_RECORD, p, sizeof(package), &package, p->id);
}

int get_christmas_sock_record_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t all_page;
		uint32_t cur_page;
		uint32_t friend_cnt;
	}__attribute__((packed)) christmas_sock_record_cbk_head_t;
	typedef struct {
		uint32_t friend_id;
		uint32_t count;
	}__attribute__((packed)) christmas_sock_record_cbk_pack_t;
	CHECK_BODY_LEN_GE(len, sizeof(christmas_sock_record_cbk_head_t));
	christmas_sock_record_cbk_head_t* cbk_head = (christmas_sock_record_cbk_head_t*)buf;

	CHECK_BODY_LEN(len, sizeof(christmas_sock_record_cbk_head_t) + cbk_head->friend_cnt * sizeof(christmas_sock_record_cbk_pack_t));
	christmas_sock_record_cbk_pack_t* cbk_pack = (christmas_sock_record_cbk_pack_t*)(buf + sizeof(christmas_sock_record_cbk_head_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_head->all_page, l);
	PKG_UINT32(msg, cbk_head->cur_page, l);
	PKG_UINT32(msg, cbk_head->friend_cnt, l);
	uint32_t i = 0;
	for (i = 0; i < cbk_head->friend_cnt; i++) {
		PKG_UINT32(msg, (cbk_pack + i)->friend_id, l);
		PKG_UINT32(msg, (cbk_pack + i)->count, l);
	}
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int get_christmas_sock_top10_cmd(sprite_t * p,const uint8_t * body,int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t type = 0;
	int i = 0;
	UNPKG_UINT32(body, type, i);
	//type == 0查询挂圣诞树袜子排行
	//type == 1查询爱心捐赠排行

	if (type >= 2) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	return send_request_to_db(SVR_PROTO_GET_CHRISTMAS_SOCK_TOP10, p, 4, &type, p->id);
}

int get_christmas_sock_top10_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t top10_cnt;
	}__attribute__((packed)) christmas_sock_top10_cbk_head_t;
	typedef struct {
		uint32_t userid;
		uint32_t count;
	}__attribute__((packed)) christmas_sock_top10_cbk_pack_t;
	CHECK_BODY_LEN_GE(len, sizeof(christmas_sock_top10_cbk_head_t));
	christmas_sock_top10_cbk_head_t* cbk_head = (christmas_sock_top10_cbk_head_t*)buf;

	CHECK_BODY_LEN(len, sizeof(christmas_sock_top10_cbk_head_t) + cbk_head->top10_cnt * sizeof(christmas_sock_top10_cbk_pack_t));
	christmas_sock_top10_cbk_pack_t* cbk_pack = (christmas_sock_top10_cbk_pack_t*)(buf + sizeof(christmas_sock_top10_cbk_head_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_head->top10_cnt, l);
	
	uint32_t i = 0;
	for (i = 0; i < cbk_head->top10_cnt; i++) {
		PKG_UINT32(msg, (cbk_pack + i)->userid, l);
		PKG_UINT32(msg, (cbk_pack + i)->count, l);
	}
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int check_gift_in_box(uint32_t gift_id)
{
	const uint32_t gift_box[] = {160618,160854,160855,160861,160862,1270128,1270012,17008};
	int i = 0;
	for (i = 0; i < sizeof(gift_box); i++) {
		if (gift_box[i] == gift_id) {
			return 1;
		}
	}
	return 0;
}

/*
 * @brief 人气圣诞树 获得礼物盒
 */
int get_christmas_sock_giftbox_cmd(sprite_t* p, const uint8_t * body, int len)
{
	CHECK_BODY_LEN(len, 4);
	
	uint32_t gift_id = 0;
	int i = 0;
	UNPKG_UINT32(body, gift_id, i);
	if (check_gift_in_box(gift_id) == 0) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}	

	const uint32_t day_type = 50014;
	*(uint32_t*)p->session = gift_id;
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &day_type, p->id);
}

int get_christmas_sock_giftbox_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t day_cnt = *(uint32_t*)buf;
	uint32_t ret_state = 0;
	if (day_cnt == 0) {
		const uint32_t day_type = 50014;
		db_set_sth_done(p, day_type, 1, p->id);
	
		uint32_t gift_id = *(uint32_t*)p->session;
		const item_t* itm = get_item_prop(gift_id);
	    uint32_t flag ;
	    int temp_len = 0;
	    pkg_item_kind(p, (uint8_t*)(&flag), gift_id, &temp_len);
	    uint32_t db_buf[] = {
			0, 1, 0, 0, flag, gift_id, 1, itm->max
		};
	    send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, sizeof(db_buf), db_buf, p->id);	
	} else {
		ret_state = 1;
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, ret_state, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int get_christmas_sock_gift_cmd(sprite_t* p, const uint8_t * body, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t friend_id = 0;
	uint32_t opt_flag = 0;
	int i = 0;
	UNPKG_UINT32(body, friend_id, i);
	UNPKG_UINT32(body, opt_flag, i);
	CHECK_VALID_ID(friend_id);
	if (opt_flag == 0 || opt_flag > 2) {
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	*(uint32_t*)(p->session) = friend_id;
	*(uint32_t*)(p->session + 4) = opt_flag;
	return send_request_to_db(SVR_PROTO_CHECK_CHRISTMAS_GIFT, p, 4, &friend_id, p->id);
}

int check_christmas_sock_gift_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct {
		uint32_t sock_cnt;
		uint32_t is_get;
	}__attribute__((packed)) check_christmas_gift_cbk_pack_t;
	CHECK_BODY_LEN(len, sizeof(check_christmas_gift_cbk_pack_t));
	check_christmas_gift_cbk_pack_t* cbk_pack = (check_christmas_gift_cbk_pack_t*)buf;
	
	uint32_t friend_id = *(uint32_t*)(p->session);
	uint32_t opt_flag = *(uint32_t*)(p->session + 4);
	DEBUG_LOG("check christmas sock gift: uid[%u] fid[%u] cnt[%u] is_get[%u]", p->id, friend_id, cbk_pack->sock_cnt, cbk_pack->is_get);

	int l = sizeof(protocol_t);
	uint32_t ret_state = 0;
	if (opt_flag == 1) {	//查询
		if (cbk_pack->is_get != 0) {
			ret_state = (cbk_pack->sock_cnt == 0) ? 2 : 1;
		}
		PKG_UINT32(msg, ret_state, l);
		PKG_UINT32(msg, 0, l);
	} else {	//领取
		if (cbk_pack->is_get != 0) {
			ret_state = 1;
			PKG_UINT32(msg, ret_state, l);
			PKG_UINT32(msg, 0, l);
		} else {
			ret_state = (cbk_pack->sock_cnt == 0) ? 2 : 0;
			PKG_UINT32(msg, ret_state, l);
			DEBUG_LOG("check christmas sock gift:ret_state[%u]", ret_state);
			if (ret_state == 2) {
				PKG_UINT32(msg, 0, l);
			} else {
				if (cbk_pack->sock_cnt > 3) {
					cbk_pack->sock_cnt = 3;
				}
				PKG_UINT32(msg, cbk_pack->sock_cnt, l);
				pack_christmas_sock_gift(p, msg, cbk_pack->sock_cnt, &l);
			
				send_request_to_db(SVR_PROTO_SET_CHRISTMAS_GIFT, NULL, 4, &friend_id, p->id);
			}
		}
	}
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int pack_christmas_sock_gift(sprite_t* p, uint8_t* message, uint32_t gift_cnt, int *l)
{
	const uint32_t gift_list[][2] = {
		{160605, 1},
		{160620, 1},
		{160604, 1},
		{160619, 1},
		{160855, 1},
		{160861, 1},
		{1270128, 1},
		{1270012, 1},
		{17008, 1},
		{1270036, 1},
		{1270019, 1},
		{16012, 3},
		{1270028, 1},
		{1270029, 1},
		{1270032, 1},
		{1270034, 1},
		{1270047, 1},
		{1270067, 1},
	};

	uint32_t idx[3] = {0};
	int i, j;
	for (i = 0; i < gift_cnt && i < 3; i++) {
		int v = rand() % (sizeof(gift_list) / 8);
		for (j = 0; j < i; ) {
			if (v == idx[j]) {
				v = rand() % (sizeof(gift_list) / 8);
			} else {
				j++;
			}
		}
		idx[i] = v;
	}
	uint8_t db_buf[1024] = {0};
	int buf_len = 0;
	PKG_H_UINT32(db_buf, 0, buf_len);
	PKG_H_UINT32(db_buf, gift_cnt, buf_len);
	PKG_H_UINT32(db_buf, 0, buf_len);
	PKG_H_UINT32(db_buf, 0, buf_len);
	for (i = 0; i < gift_cnt; i++) {
		PKG_UINT32(message, gift_list[idx[i]][0], (*l));
		PKG_UINT32(message, gift_list[idx[i]][1], (*l));
		
		const item_t* itm = get_item_prop(gift_list[idx[i]][0]);
		uint32_t flag;
		int temp_len = 0;
		pkg_item_kind(p, (uint8_t*)(&flag), itm->id, &temp_len);
		PKG_H_UINT32(db_buf, flag, buf_len);
		PKG_H_UINT32(db_buf, gift_list[idx[i]][0], buf_len);	
		PKG_H_UINT32(db_buf, gift_list[idx[i]][1], buf_len);	
		PKG_H_UINT32(db_buf, itm->max, buf_len);
		DEBUG_LOG("pack_christmas_sock_gift: uid[%u] item[%u]", p->id, itm->id);
	}

	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, buf_len, db_buf, p->id);	
	return 0;
}

