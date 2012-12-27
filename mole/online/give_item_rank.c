#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "give_item_rank.h"


int give_item_to_friend_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 12);
    uint32_t userid = 0;
	uint32_t itemid = 0;
	uint32_t count = 0;
	unpack(body, sizeof(userid) + sizeof(itemid) + sizeof(count), "LLL", &userid, &itemid, &count);
	*(uint32_t*)p->session = userid;
	*(uint32_t*)(p->session + 4) = itemid;
	*(uint32_t*)(p->session + 8) = count;

	if ((itemid != 190807) || count != 1)
	{
	    ERROR_RETURN(("error itemid=%d count=%d", itemid, count), -1);
	}

	if (IS_GUEST_ID (userid) || IS_NPC_ID (userid))
	{
		ERROR_RETURN(("not a register user\t[%u]", userid), -1);
	}

	uint32_t buff[3] = {0};
	buff[0] = 1;
	buff[1] = userid;
	if (ISVIP(p->flag))
	{
	    buff[2] = 15;
	}
	else
	{
	    buff[2] = 10;
	}
	return send_request_to_db(SVR_PROTO_CHECK_ITEM_GIVE_FRIEND, p, sizeof(buff), buff, p->id);
}


int check_item_give_friend_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    uint32_t itemid = *(uint32_t*)(p->session + 4);
    uint32_t count = *(uint32_t*)(p->session + 8);

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


int query_type_rank_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    uint32_t type = 0;
	unpack(body, sizeof(type), "L", &type);
    uint32_t buff[2] = {type, 10};
    return send_request_to_db(SVR_PROTO_SYSARG_GET_TYPE_RANK, p, sizeof(buff), buff, p->id);

}

int query_type_rank_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
   struct rank_info_hdr {
        uint32_t	userid;
        uint32_t	cnt;
        uint32_t	rank;
		uint32_t	count;
	}__attribute__((packed));

    struct rank_info {
        uint32_t	userid;
		uint32_t    cnt;
		uint32_t    rank;
	}__attribute__((packed));

	struct rank_info_hdr *p_hdr_info = (struct rank_info_hdr*)body;

	CHECK_BODY_LEN_GE(bodylen,sizeof(struct rank_info_hdr));
    int l = sizeof(protocol_t);;
    PKG_UINT32(msg, p_hdr_info->userid, l);
    PKG_UINT32(msg, p_hdr_info->cnt, l);
    PKG_UINT32(msg, p_hdr_info->rank, l);
    PKG_UINT32(msg, p_hdr_info->count, l);

    CHECK_BODY_LEN(bodylen,sizeof(struct rank_info_hdr) + p_hdr_info->count*sizeof(struct rank_info));
    struct rank_info *p_info = (struct rank_info*)(body + sizeof(struct rank_info_hdr));

	int i = 0;
	for (i = 0; i < p_hdr_info->count; i++)
	{
	    PKG_UINT32(msg, p_info->userid, l);
	    PKG_UINT32(msg, p_info->cnt, l);
	    PKG_UINT32(msg, p_info->rank, l);
	    p_info++;
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int query_get_item_history_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    uint32_t type = 0;
	unpack(body, sizeof(type), "L", &type);

    return send_request_to_db(SVR_PROTO_GET_ITEM_HISTORY_RECORD, p, 4, &type, p->id);
}

int query_get_item_history_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
	typedef struct	user_item{
		uint32_t userid;
		uint32_t stamp;
	}__attribute__((packed)) user_item_t;

    int i = 0,j;
	uint32_t count;
	int pkgsize = sizeof(protocol_t);

	CHECK_BODY_LEN_GE(bodylen, 4);
	UNPKG_H_UINT32(body, count, i);
	PKG_UINT32(msg, count, pkgsize);
	CHECK_BODY_LEN(bodylen,4 + count * sizeof(user_item_t));
	user_item_t* p_info = (user_item_t*)(body + 4);
	for(j = 0; j < count; j++) {
		PKG_UINT32(msg, p_info->userid, pkgsize);
		PKG_UINT32(msg, p_info->stamp, pkgsize);
		p_info++;
	}

	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);
}

int get_type_count_item_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 8);
    uint32_t type = 0;
    uint32_t itemid = 0;
	unpack(body, sizeof(type)+sizeof(itemid), "LL", &type, &itemid);
	uint32_t count = 0;
	if (type == 1)
	{
	    count = get_dec_send_count_by_item(itemid);
	}
	else if (type == 2)
	{
	    count = get_dec_get_count_by_item(itemid);
	}
	else
	{
	    ERROR_RETURN(("error type=%d", type), -1);
	}

    if (!count)
    {
        ERROR_RETURN(("error itemid=%d", itemid), -1);
    }

    *(uint32_t*)p->session = itemid;

	uint32_t buff[2] = {type, count};
    return send_request_to_db(SVR_PROTO_SYSARG_DEC_RANK_TYPE_COUNT, p, sizeof(buff), buff, p->id);
}

int get_type_count_item_bonus_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    uint32_t itemid = *(uint32_t *)p->session;
	uint32_t count = 1;
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
    PKG_H_UINT32(buff, itemid, j);
    PKG_H_UINT32(buff, count, j);
    item_t* item = get_item_prop(itemid);
	PKG_H_UINT32(buff, item->max, j);
    send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, buff, p->id);
    response_proto_uint32(p, p->waitcmd, itemid, 0);
    return 0;

}

int get_dec_send_count_by_item(uint32_t itemid)
{
    switch (itemid)
    {
        case 190808:
        {
            return 2;
        }
        case 1270060:
        {
            return 10;
        }
        case 1270065:
        {
            return 20;
        }
        case 1270018:
        {
            return 40;
        }
        case 1220144:
        {
            return 70;
        }
        case 1270044:
        {
            return 110;
        }
        case 1270055:
        {
            return 150;
        }
        case 1270036:
        {
            return 210;
        }
        default:
        {
            break ;
        }

    }

    return 0;
}

int get_dec_get_count_by_item(uint32_t itemid)
{
    switch (itemid)
    {
        case 190808:
        {
            return 2;
        }
        case 1270060:
        {
            return 10;
        }
        case 1270065:
        {
            return 20;
        }
        case 1270018:
        {
            return 40;
        }
        case 1220144:
        {
            return 70;
        }
        case 1270044:
        {
            return 110;
        }
        case 1270055:
        {
            return 150;
        }
        case 1270036:
        {
            return 210;
        }
        default:
        {
            break ;
        }

    }

    return 0;
}

int get_type_rank_count_info_cmd(sprite_t * p,const uint8_t * body,int len)
{
    int i = 0;
	uint32_t userid = 0;
	uint32_t type = 0;

	CHECK_BODY_LEN(len, 8);
	UNPKG_UINT32(body,userid,i);
	UNPKG_UINT32(body,type,i);
	*(uint32_t*)p->session = userid;

    if (IS_GUEST_ID (userid) || IS_NPC_ID (userid))
	{
		ERROR_RETURN(("not a register user\t[%u]", userid), -1);
	}

	return send_request_to_db(SVR_PROTO_SYSARG_GET_TYPE_RANK_COUNT_INFO, p, 4, &type, userid);
}

int get_type_rank_count_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 12);

    uint32_t userid = *(uint32_t*)p->session;

	int i = 0;
	uint32_t type = 0;
	uint32_t count = 0;
	uint32_t count_all = 0;
	UNPKG_H_UINT32(buf,type,i);
	UNPKG_H_UINT32(buf,count,i);
	UNPKG_H_UINT32(buf,count_all,i);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg,userid,l);
	PKG_UINT32(msg,type,l);
	PKG_UINT32(msg,count,l);
    PKG_UINT32(msg,count_all,l);

	init_proto_head(msg,p->waitcmd,l);
	return send_to_self(p,msg,l,1);
}



