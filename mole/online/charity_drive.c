#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "charity_drive.h"

static donate_box_t donate_box_items;


int guess_item_price_charity_drive_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
	uint32_t xiaomee = 0;
	unpack(body, sizeof(xiaomee), "L", &xiaomee);

	if ((int)xiaomee < 0)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x020D1008,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	return send_request_to_db(SVR_PROTO_SYSARG_AUCTION_ADD_ONE_RECORD, p, 4, &xiaomee, p->id);
}

int guess_item_price_charity_drive_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

int query_guess_item_charity_drive_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);

    return send_request_to_db(SVR_PROTO_SYSARG_AUCTION_GET_ATTIREID_AND_TIME, p, 0, NULL, p->id);

}

int query_guess_item_charity_drive_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    int i = 0 ;
	uint32_t itemid = 0;
	uint32_t count = 0;
	uint32_t time_last = 0;

	int pkgsize = sizeof(protocol_t);

	CHECK_BODY_LEN(bodylen, 12);
	UNPKG_H_UINT32(body, itemid, i);
	UNPKG_H_UINT32(body, count, i);
	UNPKG_H_UINT32(body, time_last, i);

	int tm_hour = get_now_tm()->tm_hour;
	if(tm_hour < 18 || tm_hour >= 20)
	{
		itemid = 1;
	}

	PKG_UINT32(msg, itemid, pkgsize);
	PKG_UINT32(msg, count, pkgsize);
	PKG_UINT32(msg, time_last, pkgsize);

	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);

}

int donate_item_charity_drive_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 8);
	uint32_t itemid = 0;
	uint32_t count = 0;
	unpack(body, sizeof(itemid)+sizeof(count), "LL", &itemid, &count);

	if ((int)count < 0)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

    if (!check_item_from_donateitem_conf(itemid))
    {
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
    }

    if (itemid != 0)
    {
        uint32_t msgbuff[2] = {p->id, count};
	    msglog(statistic_logfile, 0x020D1009,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
    }
    else
    {
        uint32_t msgbuff[2] = {count, p->id};
	    msglog(statistic_logfile, 0x020D1010,get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
    }

	*(uint32_t*)p->session = itemid;
	*(uint32_t*)(p->session + 4) = count;

	uint8_t buff[1024] = {0};
	int j = 0;
	PKG_H_UINT32(buff, 1, j);
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 1000, j);
	PKG_H_UINT32(buff, 0, j);
	if (pkg_item_kind(p, buff, itemid, &j) == -1)
	{
		return -1;
	}
	PKG_H_UINT32(buff, itemid, j);
	PKG_H_UINT32(buff, count, j);

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);

}

int donate_item_charity_drive_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}


int query_charity_drive_type_rank_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    uint32_t type = 0;
    unpack(body, sizeof(type), "L", &type);

    return send_request_to_db(SVR_PROTO_SYSARG_CONTRIBUTE_GET_RANK, p, 4, &type, p->id);

}

int query_charity_drive_type_rank_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    int i = 0,j;
	uint32_t count;
	uint32_t userid;
	uint32_t value;

	int pkgsize = sizeof(protocol_t);

	CHECK_BODY_LEN_GE(bodylen, 4);
	UNPKG_H_UINT32(body, count, i);
	PKG_UINT32(msg, count, pkgsize);

	CHECK_BODY_LEN(bodylen,4 + count * 8);
	for(j = 0; j < count; j++) {
		UNPKG_H_UINT32(body, userid, i);
		UNPKG_H_UINT32(body, value, i);

		PKG_UINT32(msg, userid, pkgsize);
		PKG_UINT32(msg, value, pkgsize);
	}

	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);

}

int query_charity_drive_info_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);

    return send_request_to_db(SVR_PROTO_SYSARG_QUERY_CHARITY_DRIVE, p, 0, NULL, p->id);

}

int query_charity_drive_info_callback(sprite_t *p, uint32_t id, const char *body, int bodylen)
{
    int i = 0 ;
	uint32_t user_count = 0;
	uint32_t xiaomee_count = 0;
	uint32_t item_count = 0;

	int pkgsize = sizeof(protocol_t);

	CHECK_BODY_LEN(bodylen, 12);
	UNPKG_H_UINT32(body, user_count, i);
	UNPKG_H_UINT32(body, xiaomee_count, i);
	UNPKG_H_UINT32(body, item_count, i);

	PKG_UINT32(msg, user_count, pkgsize);
	PKG_UINT32(msg, xiaomee_count, pkgsize);
	PKG_UINT32(msg, item_count, pkgsize);

	init_proto_head(msg, p->waitcmd, pkgsize);
	return send_to_self(p, msg, pkgsize, 1);

}

/* @breif load donateitem.xml file
 */
int load_donate_item_config(const char* file)
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	uint32_t itemid = 0;
	uint32_t i = 0;
	int err = -1;
	memset(&donate_box_items, 0, sizeof (donate_box_items));

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
			donate_box_items.itemids[i] = itemid;
			i++;
		}
		cur = cur->next;
	}

	donate_box_items.count = i;

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load donate item file %s", file);

}

int check_item_from_donateitem_conf(int itemid)
{
    int i = 0;
    for (i = 0; i < donate_box_items.count; i++)
    {
        if (donate_box_items.itemids[i] == itemid)
        {
            return 1;
        }
    }

    return 0;

}




