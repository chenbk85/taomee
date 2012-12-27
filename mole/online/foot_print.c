#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "foot_print.h"
#include "game_bonus.h"

int get_foot_print_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    uint32_t itemid = 0;
    uint32_t count = 0;
    int i = 0;
    uint32_t cnt = 0;
    uint32_t num = 0;
    CHECK_BODY_LEN_GE(bodylen, 4);
	UNPKG_UINT32(body, cnt, i);
	CHECK_BODY_LEN(bodylen, 4+8*cnt);
	if (cnt > 20)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

    int l = 0;
	uint8_t dbbuf[2048] = {};
	PKG_H_UINT32(dbbuf,cnt,l);
	PKG_H_UINT32(dbbuf,0,l);
	PKG_H_UINT32(dbbuf,0,l);
	PKG_H_UINT32(dbbuf,0,l);

	int j = 0;
	for (j = 0; j < cnt; j++)
	{
	    UNPKG_UINT32(body, itemid, i);
	    UNPKG_UINT32(body, count, i);
	    num = num + count;

	    if ((itemid > 190570 || itemid < 190535) && (itemid != 190820))
	    {
	        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	    }

        PKG_H_UINT32(dbbuf,0,l);
	    PKG_H_UINT32(dbbuf,itemid,l);
	    PKG_H_UINT32(dbbuf,count,l);
	}

	if (num != 1 && num != 8 && num != 6 && num != 10 && num != 15)
	{
	    DEBUG_LOG("---num %d--", num);
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

    *(uint32_t*)p->session  = num;

    return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, l, dbbuf, p->id);

}

int get_itemid_by_num(uint32_t num)
{
    switch(num)
    {
        case 1:
        {
            int val = rand()%100;
            if (val < 1)
            {
                int itemid = 16012;
                return itemid;
            }
            else if (val < 6)
            {
                int itemid = 1270071;
                return itemid;
            }
            else if (val < 16)
            {
                int itemids[] = {1230059,1230060,1230061,1230062,1230063,1230064,1230065};
                int index_v = rand()%(sizeof(itemids)/4);
                int itemid = itemids[index_v];
                return itemid;
            }
            else
            {
                int itemids[] = {150004,1270005,180003,180008,17007,1230025,180067,1230024,
                    190028,180071};
                int index_v = rand()%(sizeof(itemids)/4);
                int itemid = itemids[index_v];
                return itemid;
            }
        }
        case 6:
        {
            return 1270071;
        }
        case 10:
        {
            return 13163;
        }
        case 15:
        {
            return 13669;
        }
        default:
        {
            return 0;
        }
    }

    return 0;
}

int get_foot_print_bonus_mgr(sprite_t* p)
{
    uint32_t num = 0;
    num = *(uint32_t*)p->session;

    uint32_t info_m[2] = {p->id, num};
	msglog(statistic_logfile, 0x020D2018, now.tv_sec, info_m, sizeof(info_m));

    if (num == 8)
    {
        int itemids[] = {1230059,1230060,1230061,1230062,1230063,1230064,1230065};
        uint32_t cnt = sizeof(itemids)/4;
        uint8_t buff[1024] = {};
    	int j = 0;
    	PKG_H_UINT32(buff, 0, j);
        PKG_H_UINT32(buff, cnt, j);
        PKG_H_UINT32(buff, 202, j);
        PKG_H_UINT32(buff, 0, j);

        int l = sizeof(protocol_t);
        PKG_UINT32(msg,cnt,l);

        int i = 0;
        for (i = 0; i < cnt; i++)
        {
            uint32_t itemid = itemids[i];
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

            PKG_UINT32(msg,itm->id,l);
            PKG_UINT32(msg,1,l);

        }
        send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, buff, p->id);

        init_proto_head(msg,p->waitcmd,l);
	    return send_to_self(p,msg,l,1);
    }
    else
    {
        uint32_t itemid = get_itemid_by_num(num);
        uint8_t buff[1024] = {};
    	int j = 0;
    	PKG_H_UINT32(buff, 0, j);
        PKG_H_UINT32(buff, 1, j);
        PKG_H_UINT32(buff, 202, j);
        PKG_H_UINT32(buff, 0, j);
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
        send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, buff, p->id);
        response_proto_uint32_uint32_uint32(p, p->waitcmd, 1, itemid, 1, 0);
        return 0;
    }

}

int foot_print_get_items_cmd(sprite_t * p,const uint8_t * body,int len)
{
    int i = 0;
    uint32_t userid = 0;
    uint32_t type = 0;
    CHECK_BODY_LEN(len, 8);
    UNPKG_UINT32(body,userid,i);
    UNPKG_UINT32(body,type,i);
    uint32_t itemid_start = 1352000;
    uint32_t itemid_end = 1352012;

    if (!type){
        itemid_start = 1352000;
        itemid_end = 1352012;
    }
    else {
        itemid_start = 1352012;
        itemid_end = 1352024;
    }

	int j = 0;
	uint8_t buff[13];
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, itemid_start, j);
	PKG_H_UINT32(buff, itemid_end, j);
	PKG_UINT8(buff, 2, j);
	return send_request_to_db(SVR_PROTO_GET_ITEM, p, j, buff, userid);

}

int get_foot_print_count_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
    int i = 0;
	uint32_t userid = 0;
	CHECK_BODY_LEN(len, 4);
	UNPKG_UINT32(body,userid,i);
	*(uint32_t*)p->session = userid;

    if (IS_GUEST_ID (userid) || IS_NPC_ID (userid))
	{
		ERROR_RETURN(("not a register user\t[%u]", userid), -1);
	}

	return send_request_to_db(SVR_PROTO_ROOMINFO_GET_FOOTPRINT_COUNT, p, 0, NULL, userid);
}

int get_foot_print_count_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 8);

    uint32_t userid = *(uint32_t*)p->session;

	int i = 0;
	uint32_t count = 0;
	uint32_t count_all = 0;
	UNPKG_H_UINT32(buf,count,i);
	UNPKG_H_UINT32(buf,count_all,i);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg,userid,l);
	PKG_UINT32(msg,count_all,l);
    PKG_UINT32(msg,count,l);

	init_proto_head(msg,p->waitcmd,l);
	return send_to_self(p,msg,l,1);
}

int exchange_sth_by_footprint_count_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t game_id = *(uint32_t *)p->session;
	uint32_t index = *(uint32_t *)(p->session + sizeof(game_id));
	return send_game_bonus_to_db(p, game_id, index);
}

int add_foot_print_count(sprite_t *p, uint32_t itemid, uint32_t count)
{
	if (itemid >= 1352000 &&  itemid <= 1352011)
	{
	    msglog(statistic_logfile, 0x04040200, now.tv_sec, &count, 4);
	    DEBUG_LOG("---add_foot_print_count itemid %d count %d--", itemid, count);
	    return send_request_to_db(SVR_PROTO_ROOMINFO_FOOTPRINT_ADD_COUNT, NULL, 4, &count, p->id);
	}
	return 0;
}

int exchange_foot_print_msglog(sprite_t *p, uint32_t gameid, uint32_t i)
{
    uint32_t count = 1;
    if (gameid == 17)
    {
        msglog(statistic_logfile, 0x04040601 + i, now.tv_sec, &count, 4);
    }
    else if (gameid == 18)
    {
        msglog(statistic_logfile, 0x04040605 + i, now.tv_sec, &count, 4);
    }

    return 0;
}


