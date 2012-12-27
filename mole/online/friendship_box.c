#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "friendship_box.h"

static friend_box_t friend_box_items;
static friend_box_t mw_friend_box_items;


int add_item_in_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 8);
	uint32_t itemid = 0;
	uint32_t count = 0;
	unpack(body, sizeof(itemid) + sizeof(count), "LL", &itemid, &count);

    if (!check_item_from_friendbox_conf(itemid))
    {
        return send_to_self_error(p, p->waitcmd, -ERR_friendbox_item_limit, 1);
    }

	uint8_t buff[1024] = {};
	int j = 0;
	if (pkg_item_kind(p, buff, itemid, &j) == -1)
	{
		return -1;
	}
	PKG_H_UINT32(buff, itemid, j);
	PKG_H_UINT32(buff, count, j);

	return send_request_to_db(SVR_PROTO_PUT_ITEM_TO_FRIEND_BOX, p, j, buff, p->id);
}

int add_item_in_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    response_proto_head(p, p->waitcmd, 0);
	uint32_t msg_buff[1] = {p->id};
	msglog(statistic_logfile, 0x02100501,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	uint32_t msgbuff[2] = {1, p->id};
	msglog(statistic_logfile, 0x02103301,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

    return 0;
}

int get_item_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen)//self or other
{
    CHECK_BODY_LEN(bodylen, 12);
    uint32_t userid = 0;
	uint32_t itemid = 0;
	uint32_t count = 0;
	unpack(body, sizeof(userid) + sizeof(itemid) + sizeof(count), "LLL", &userid, &itemid, &count);

    uint32_t max_value = 0;
	item_t* item = get_item_prop(itemid);
	if (item)
	{
	    max_value = item->max;
	}

	uint32_t buff[4] = { };
	buff[0] = userid;
	buff[1] = itemid;
	buff[2] = count;
	buff[3] = max_value;
	*(uint32_t*)p->session = userid;
	*(uint32_t*)(p->session + 4) = itemid;
	*(uint32_t*)(p->session + 8) = count;
	*(uint32_t*)(p->session + 12) = max_value;

	return send_request_to_db(SVR_PROTO_CHECK_ITEM_FROM_FRIEND, p, sizeof(buff), buff, p->id);
}

int get_item_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    uint32_t itemid = 0;
    uint32_t count = 0;
    uint32_t max_value = 0;
    uint32_t userid = 0;
    userid = *(uint32_t*)p->session;
    itemid = *(uint32_t*)(p->session + 4);
    count = *(uint32_t*)(p->session + 8);
    max_value = *(uint32_t*)(p->session + 12);

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
	PKG_H_UINT32(buff, max_value, j);

	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, buff, p->id);

    send_request_to_db(SVR_PROTO_SET_TAG_ITEM_FROM_FRIEND, NULL, 4, &(userid), p->id);

	uint32_t msg_buff[2] = {p->id,1};
	msglog(statistic_logfile, 0x02100502,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	uint32_t msgbuff[2] = {1, p->id};
	msglog(statistic_logfile, 0x02103302,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

    response_proto_head(p, p->waitcmd, 0);
    return 0;
}


int check_item_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    struct  user_info{
            uint32_t userid;
            uint32_t itemid;
            char     name[16];
            uint32_t color;
            uint32_t is_vip;
            uint32_t stamp;
            uint32_t count;
    }__attribute__((packed));

    struct  user_info item_info = {};

    uint32_t userid = 0;
	userid = *(uint32_t*)p->session;

    item_info.userid =  p->id;
    item_info.itemid = *(uint32_t*)(p->session + 4);
    item_info.count = *(uint32_t*)(p->session + 8);
    if (p->id != userid)
	{
	    item_info.count = 1;
	    *(uint32_t*)(p->session + 8) = item_info.count;
	}
	memcpy(item_info.name, p->nick, 16);
	item_info.color = p->color;
	item_info.is_vip = ISVIP(p->flag);
	item_info.stamp = time(NULL);


    return send_request_to_db(SVR_PROTO_GET_ITEM_FROM_FRIEND_BOX, p, sizeof(item_info), &item_info, userid);
}


int query_items_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen)//self or other
{
    CHECK_BODY_LEN(bodylen, 4);
    uint32_t userid = 0;
    unpack(body, sizeof(userid), "L", &userid);

    return send_request_to_db(SVR_PROTO_GET_FRIEND_BOX_ITEMS, p, 0, NULL, userid);

}

int query_items_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    int i = 0,j;
	uint32_t count;
	uint32_t itemid;
	uint32_t item_count;

	int pkgsize = sizeof(protocol_t);

	CHECK_BODY_LEN_GE(bodylen, 4);
	UNPKG_H_UINT32(body, count, i);
	PKG_UINT32(msg, count, pkgsize);

	CHECK_BODY_LEN(bodylen,4 + 6 * 8);
	for(j = 0; j < count; j++) {
		UNPKG_H_UINT32(body, itemid, i);
		UNPKG_H_UINT32(body, item_count, i);

		PKG_UINT32(msg, itemid, pkgsize);
		PKG_UINT32(msg, item_count, pkgsize);
	}

	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);

}

int get_item_history_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen)//self or other
{
    CHECK_BODY_LEN(bodylen, 4);
    uint32_t userid = 0;
    unpack(body, sizeof(userid), "L", &userid);

    return send_request_to_db(SVR_PROTO_GET_FRIEND_BOX_HISTORY, p, 0, NULL, userid);
}

int get_item_history_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
	typedef struct	user_item{
		uint32_t userid;
		uint32_t itemid;
		char	 name[16];
		uint32_t color;
		uint32_t is_vip;
		uint32_t stamp;
	}__attribute__((packed)) user_item_t;

    int i = 0,j;
	uint32_t count;
	int pkgsize = sizeof(protocol_t);

	CHECK_BODY_LEN_GE(bodylen, 4);
	UNPKG_H_UINT32(body, count, i);
	PKG_UINT32(msg, count, pkgsize);
	CHECK_BODY_LEN(bodylen,4 + 20 * 36);
	user_item_t* p_info = (user_item_t*)(body + 4);
	for(j = 0; j < count; j++) {
		PKG_UINT32(msg, p_info->userid, pkgsize);
		PKG_UINT32(msg, p_info->itemid, pkgsize);
		PKG_STR(msg, p_info->name, pkgsize, 16);
		PKG_UINT32(msg, p_info->color, pkgsize);
		PKG_UINT32(msg, p_info->is_vip, pkgsize);
		PKG_UINT32(msg, p_info->stamp, pkgsize);
		p_info++;
	}

	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);
}

/* @breif load friendbox.xml file
 */
int load_friendbox_config(const char* file)
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	uint32_t itemid = 0;
	uint32_t i = 0;
	int err = -1;
	memset(&friend_box_items, 0, sizeof (friend_box_items));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur)
	{
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Item"))
		{
			DECODE_XML_PROP_UINT32(itemid, cur, "ID");
			friend_box_items.itemids[i] = itemid;
			i++;
		}
		cur = cur->next;
	}

	friend_box_items.count = i;

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load friend box item file %s", file);

}

int check_item_from_friendbox_conf(int itemid)
{
    int i = 0;
    for (i = 0; i < friend_box_items.count; i++)
    {
        if (friend_box_items.itemids[i] == itemid)
        {
            return 1;
        }
    }

    return 0;

}


int mw_add_item_in_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 8);
	uint32_t itemid = 0;
	uint32_t count = 0;
	unpack(body, sizeof(itemid) + sizeof(count), "LL", &itemid, &count);

    if (!mw_check_item_from_friendbox_conf(itemid))
    {
        return send_to_self_error(p, p->waitcmd, -ERR_friendbox_item_limit, 1);
    }

	uint8_t buff[1024] = {};
	int j = 0;
	if (pkg_item_kind(p, buff, itemid, &j) == -1)
	{
		return -1;
	}
	PKG_H_UINT32(buff, itemid, j);
	PKG_H_UINT32(buff, count, j);

	return send_request_to_db(SVR_PROTO_MW_PUT_ITEM_TO_FRIEND_BOX, p, j, buff, p->id);
}

int mw_add_item_in_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    response_proto_head(p, p->waitcmd, 0);
	uint32_t msg_buff[1] = {p->id};
	msglog(statistic_logfile, 0x02100501,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	uint32_t msgbuff[2] = {1, p->id};
	msglog(statistic_logfile, 0x02103301,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

    return 0;
}

int mw_get_item_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen)//self or other
{
    CHECK_BODY_LEN(bodylen, 12);
    uint32_t userid = 0;
	uint32_t itemid = 0;
	uint32_t count = 0;
	unpack(body, sizeof(userid) + sizeof(itemid) + sizeof(count), "LLL", &userid, &itemid, &count);

    uint32_t max_value = 0;
	item_t* item = get_item_prop(itemid);
	if (item)
	{
	    max_value = item->max;
	}

	uint32_t buff[4] = { };
	buff[0] = userid;
	buff[1] = itemid;
	buff[2] = count;
	buff[3] = max_value;
	*(uint32_t*)p->session = userid;
	*(uint32_t*)(p->session + 4) = itemid;
	*(uint32_t*)(p->session + 8) = count;
	*(uint32_t*)(p->session + 12) = max_value;

	return send_request_to_db(SVR_PROTO_MW_CHECK_ITEM_FROM_FRIEND, p, sizeof(buff), buff, p->id);
}

int mw_get_item_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    uint32_t itemid = 0;
    uint32_t count = 0;
    uint32_t max_value = 0;
    uint32_t userid = 0;
    userid = *(uint32_t*)p->session;
    itemid = *(uint32_t*)(p->session + 4);
    count = *(uint32_t*)(p->session + 8);
    max_value = *(uint32_t*)(p->session + 12);

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
	PKG_H_UINT32(buff, max_value, j);

	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, j, buff, p->id);

    send_request_to_db(SVR_PROTO_MW_SET_TAG_ITEM_FROM_FRIEND, NULL, 4, &(userid), p->id);

	uint32_t msg_buff[2] = {p->id,1};
	msglog(statistic_logfile, 0x02100502,get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	uint32_t msgbuff[2] = {1, p->id};
	msglog(statistic_logfile, 0x02103302,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

    response_proto_head(p, p->waitcmd, 0);
    return 0;
}


int mw_check_item_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    struct  user_info{
            uint32_t userid;
            uint32_t itemid;
            char     name[16];
            uint32_t color;
            uint32_t is_vip;
            uint32_t stamp;
            uint32_t count;
    }__attribute__((packed));

    struct  user_info item_info = {};

    uint32_t userid = 0;
	userid = *(uint32_t*)p->session;

    item_info.userid =  p->id;
    item_info.itemid = *(uint32_t*)(p->session + 4);
    item_info.count = *(uint32_t*)(p->session + 8);
    if (p->id != userid)
	{
	    item_info.count = 1;
	    *(uint32_t*)(p->session + 8) = item_info.count;
	}
	memcpy(item_info.name, p->nick, 16);
	item_info.color = p->color;
	item_info.is_vip = ISVIP(p->flag);
	item_info.stamp = time(NULL);


    return send_request_to_db(SVR_PROTO_MW_GET_ITEM_FROM_FRIEND_BOX, p, sizeof(item_info), &item_info, userid);
}


int mw_query_items_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen)//self or other
{
    CHECK_BODY_LEN(bodylen, 4);
    uint32_t userid = 0;
    unpack(body, sizeof(userid), "L", &userid);

    return send_request_to_db(SVR_PROTO_MW_GET_FRIEND_BOX_ITEMS, p, 0, NULL, userid);

}

int mw_query_items_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    int i = 0,j;
	uint32_t count;
	uint32_t itemid;
	uint32_t item_count;

	int pkgsize = sizeof(protocol_t);

	CHECK_BODY_LEN_GE(bodylen, 4);
	UNPKG_H_UINT32(body, count, i);
	PKG_UINT32(msg, count, pkgsize);

	CHECK_BODY_LEN(bodylen,4 + 6 * 8);
	for(j = 0; j < count; j++) {
		UNPKG_H_UINT32(body, itemid, i);
		UNPKG_H_UINT32(body, item_count, i);

		PKG_UINT32(msg, itemid, pkgsize);
		PKG_UINT32(msg, item_count, pkgsize);
	}

	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);

}

int mw_get_item_history_from_friendship_box_cmd(sprite_t* p, uint8_t* body, int bodylen)//self or other
{
    CHECK_BODY_LEN(bodylen, 4);
    uint32_t userid = 0;
    unpack(body, sizeof(userid), "L", &userid);

    return send_request_to_db(SVR_PROTO_MW_GET_FRIEND_BOX_HISTORY, p, 0, NULL, userid);
}

int mw_get_item_history_from_friendship_box_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
	typedef struct	user_item{
		uint32_t userid;
		uint32_t itemid;
		char	 name[16];
		uint32_t color;
		uint32_t is_vip;
		uint32_t stamp;
	}__attribute__((packed)) user_item_t;

    int i = 0,j;
	uint32_t count;
	int pkgsize = sizeof(protocol_t);

	CHECK_BODY_LEN_GE(bodylen, 4);
	UNPKG_H_UINT32(body, count, i);
	PKG_UINT32(msg, count, pkgsize);
	CHECK_BODY_LEN(bodylen,4 + 20 * 36);
	user_item_t* p_info = (user_item_t*)(body + 4);
	for(j = 0; j < count; j++) {
		PKG_UINT32(msg, p_info->userid, pkgsize);
		PKG_UINT32(msg, p_info->itemid, pkgsize);
		PKG_STR(msg, p_info->name, pkgsize, 16);
		PKG_UINT32(msg, p_info->color, pkgsize);
		PKG_UINT32(msg, p_info->is_vip, pkgsize);
		PKG_UINT32(msg, p_info->stamp, pkgsize);
		p_info++;
	}

	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);
}

/* @breif load friendbox.xml file
 */
int mw_load_friendbox_config(const char* file)
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	uint32_t itemid = 0;
	uint32_t i = 0;
	int err = -1;
	memset(&mw_friend_box_items, 0, sizeof (mw_friend_box_items));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur)
	{
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Item"))
		{
			DECODE_XML_PROP_UINT32(itemid, cur, "ID");
			mw_friend_box_items.itemids[i] = itemid;
			i++;
		}
		cur = cur->next;
	}

	mw_friend_box_items.count = i;

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load mw friend box item file %s", file);

}

int mw_check_item_from_friendbox_conf(int itemid)
{
    int i = 0;
    for (i = 0; i < mw_friend_box_items.count; i++)
    {
        if (mw_friend_box_items.itemids[i] == itemid)
        {
            return 1;
        }
    }
    
    return 0;
}




