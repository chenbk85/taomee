
#include "proto.h"
#include "exclu_things.h"
#include "message.h"
#include "communicator.h"
#include "central_online.h"

#include "gift.h"

send_gift_t gifts[GIFTS_TABLE_SIZE];

static int parse_gifts_single_item(item_unit_t* iut, int* cnt, xmlNodePtr cur, int flag)
{
	int id, j = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item"))) {
			if (j == MAX_EXCHANGE_ITEMS) {
				ERROR_RETURN(("too many items"), -1);
			}

			DECODE_XML_PROP_INT (id, cur, "ID");
			if (!(iut[j].itm = get_item_prop (id))) {
				ERROR_RETURN(("can't find item=%d", id), -1);
			}

			DECODE_XML_PROP_INT (iut[j].count, cur, "Count");
			if (iut[j].count <= 0) {
				ERROR_RETURN(("error count=%d, item=%d", iut[j].count, id), -1);
			}
			j++;
		}
		cur = cur->next;
	}
	*cnt = j;

	return 0;
}

static int parse_gifts_items(send_gift_t *sg, xmlNodePtr chl)
{
	xmlNodePtr cur;

	while (chl) {
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemIn")) {
			cur = chl->xmlChildrenNode;
			if (parse_gifts_single_item(sg->in_item, &(sg->in_cnt), cur, 0) != 0)
				return -1;
		}
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemOut")) {
			cur = chl->xmlChildrenNode;
			if (parse_gifts_single_item(sg->out_item, &(sg->out_cnt), cur, 1) != 0)
				return -1;
		}
		chl = chl->next;
	}

	return 0;
}

int load_gifts(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int gifts_count;

	gifts_count = 0;
	memset (gifts, 0, sizeof (gifts));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load gifts config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Entry"))) {
			DECODE_XML_PROP_INT(i, cur, "ID");
			if (gifts_count > GIFTS_TABLE_SIZE - 1 || gifts_count != i) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, gifts_count, i);
				goto exit;
			}

			gifts[i].send_gift_id = i;
			chl = cur->xmlChildrenNode;
			if ( (parse_gifts_items(&gifts[i], chl) != 0) )
				goto exit;

			gifts_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load gifts item file %s", file);
}

static void set_present_list(sprite_t* p, send_one_t* aso)
{
	send_gift_t* lsg = get_gift_entry(aso->gid);

	uint8_t buff[256];
	int j = 0;
	PKG_H_UINT32(buff, p->id, j);
	PKG_H_UINT32(buff, lsg->out_item[0].itm->id, j);
	PKG_STR(buff, p->nick, j, USER_NICK_LEN);
	PKG_H_UINT32(buff, p->color, j);
	PKG_UINT8(buff, ISVIP(p->flag), j);
	PKG_H_UINT32(buff, get_now_tv()->tv_sec, j);
	PKG_STR(buff, aso->mesg, j, MSG_LEN);

	send_request_to_db(SVR_SET_GIFT_LIST, 0, j, buff, aso->frd_id);
}

int exchg_gift_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	switch 	(p->waitcmd){
	case PROTO_SEND_ONE_GIFT:
	{
		char txt[512];
		send_one_t *lso = (send_one_t*)p->session;
		set_present_list(p, lso);

		send_gift_t* lsg = get_gift_entry(lso->gid);
		item_kind_t* kind = find_kind_of_item(lsg->out_item[0].itm->id);
		if (!kind)
			ERROR_RETURN(("error item kind, item=%u, uid=%u", lsg->out_item[0].itm->id, p->id), -1);
#ifdef 	TW_VER
		char* pos = (kind->kind == ANIMAL_BABY_ITEM) ? "牧場倉庫中" : ((kind->kind == HOMELAND_ITEM_KIND) ? "家園倉庫中," : ((kind->kind == HOME_ITEM_KIND) ? "小屋倉庫中，" : "百寶箱中，"));
		snprintf(txt, sizeof(txt), "精美的%s已放入你的%s快去看看吧！%s", lsg->out_item[0].itm->name, pos, lso->mesg);
#else
		char* pos = (kind->kind == ANIMAL_BABY_ITEM) ? "牧场仓库中" : ((kind->kind == HOMELAND_ITEM_KIND) ? "家园仓库中," : ((kind->kind == HOME_ITEM_KIND) ? "小屋仓库中，" : "百宝箱中，"));
		snprintf(txt, sizeof(txt), "精美的%s已放入你的%s快去看看吧！%s", lsg->out_item[0].itm->name, pos, lso->mesg);
#endif
		send_postcard(p->nick, p->id, lso->frd_id, 1000060, txt, 0);
		send_attr_update_noti(p->id, 0, lso->frd_id, 3);

		modify_send_gift_times(0, 1, p->id);
		p->lovely += 2 * lso->count;
		do_db_attr_op(0, p->id, 0, 0, 0, 0, 2 * lso->count, 0, 0);

		DEBUG_LOG("EXCHG GIFT LIMIT\t[%u %u]", p->id, p->daily_limit[0]);
		uint32_t flag = 0;
		if (p->daily_limit[0] < 5) {
			db_single_item_op(0, p->id, 190460, 1, 1);
			p->daily_limit[0] ++;
			db_set_sth_done(0, 1001, 5, p->id);
			flag = 1;
		}

		int j = sizeof(protocol_t);
		PKG_UINT32(msg, lsg->out_item[0].itm->id, j);
		PKG_UINT32(msg, lso->frd_id, j);
		PKG_UINT32(msg, flag ? 190460 : 888888, j);
		init_proto_head(msg, p->waitcmd, j);
		send_to_self(p, msg, j, 1);
		return 0;
	}
	default:
		ERROR_RETURN(("error waitcmd, cmd=%d, id=%u", p->waitcmd, p->id), -1);
	}

}

int send_one_gift_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{

	CHECK_BODY_LEN(bodylen, 163);

	int loop;
	int i = 0;
	send_one_t *lso = (send_one_t*)p->session;
	UNPKG_UINT32(body, lso->gid, i);
	UNPKG_UINT32(body, lso->count, i);
	UNPKG_STR(body, lso->mesg, i, MSG_LEN);
	UNPKG_UINT32(body, lso->frd_id, i);

	CHECK_VALID_ID(lso->frd_id);
	if (lso->frd_id == p->id)
		return send_to_self_error(p, p->waitcmd, ERR_client_not_proc, 1);

	send_gift_t* lsg = get_gift_entry(lso->gid);
	if (!lsg)
		ERROR_RETURN(("bad gift id[%u]", p->id), -1);

	{   // preserved for send gifts
		int len = 0;
		PKG_H_UINT32(lso->send_buf, 0, len);				//in_cnt
		PKG_H_UINT32(lso->send_buf, lsg->out_cnt, len);		//out_cnt
		PKG_H_UINT32(lso->send_buf, 0, len);				//reason
		PKG_H_UINT32(lso->send_buf, 0, len);				//reason
		for (loop = 0; loop < lsg->out_cnt; loop++) {
			item_t *pitm = lsg->out_item[loop].itm;
			item_kind_t* kind = find_kind_of_item(lsg->out_item[loop].itm->id);
			if (!kind)
				ERROR_RETURN(("error item kind, item=%u, uid=%u", pitm->id, p->id), -1);
			uint32_t flag = 0;
			if (kind->kind == HOME_ITEM_KIND) {
				flag = 1;
			} else if (kind->kind == HOMELAND_ITEM_KIND) {
				flag = 2;
			}
			PKG_H_UINT32(lso->send_buf, flag, len);
			PKG_H_UINT32(lso->send_buf, pitm->id, len);
			PKG_H_UINT32(lso->send_buf, lsg->out_item[loop].count * lso->count, len);
			PKG_H_UINT32(lso->send_buf, pitm->max, len);
		}
		lso->send_len = len;
	}
	// discard items
	int j = 0;
	PKG_H_UINT32(lso->dec_buf, lsg->in_cnt, j);
	PKG_H_UINT32(lso->dec_buf, 0, j);	//out_cnt
	PKG_H_UINT32(lso->dec_buf, 0, j);	//reason
	PKG_H_UINT32(lso->dec_buf, 0, j);	//reason
	for (loop = 0; loop < lsg->in_cnt; loop++) {
		PKG_H_UINT32(lso->dec_buf, 0, j); 	// collect item
		PKG_H_UINT32(lso->dec_buf, lsg->in_item[loop].itm->id, j);
		PKG_H_UINT32(lso->dec_buf, lsg->in_item[loop].count * lso->count, j);
	}
	lso->dec_len = j;

	DEBUG_LOG("SEND GIFT[%u %u %u %u]", p->id, lso->frd_id, lso->gid, lsg->out_item[0].itm->id);
	return db_set_sth_done(p, 202, 50, p->id);
}

int get_additional_gift_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	uint32_t additonal_itm[6] = {160446, 160447, 160448, 160449, 160474, 160475};
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);

	uint32_t itmid;
	int j = 0;
	UNPKG_UINT32(body, itmid, j);
	*(uint32_t*)p->session = itmid;

	int loop, searched = 0;
	for (loop = 0; loop < sizeof(additonal_itm)/4; loop++) {
		if (additonal_itm[loop] == itmid)
			searched = 1;
	}
	if (!searched) {
		ERROR_RETURN(("bad item id\t[%u %u]", p->id, itmid), -1);
	}

	return modify_send_gift_times(p, -3, p->id);
}

int modify_send_gift_times_callback(sprite_t* p, uint32_t id, char* buf, int len)
{

	uint32_t itemid = *(uint32_t*)p->session;
	item_t* pitm = get_item_prop(itemid);
	if (!pitm)
		ERROR_RETURN(("bad itemid[%u %u]", p->id, itemid), -1);
	item_kind_t* kind = find_kind_of_item(itemid);
	if (!kind)
		ERROR_RETURN(("bad itemid[%u %u]", p->id, itemid), -1);

	uint32_t flag = 0;
	if (kind->kind == HOME_ITEM_KIND) {
		flag = 1;
	} else if (kind->kind == HOMELAND_ITEM_KIND) {
		flag = 2;
	} else if (kind->kind == HAPPY_CARD_KIND) {
		flag = 6;
	}
	uint32_t tmp_buf[5];
	tmp_buf[0] = flag;
	tmp_buf[1] = itemid;
	tmp_buf[2] = 1;
	tmp_buf[3] = pitm->max;
	tmp_buf[4] = 0;
	send_request_to_db(SVR_PROTO_BUY_ITEM, 0, 20, tmp_buf, p->id);

	DEBUG_LOG("SEND ADDITIONAL ITM\t[%u %u]", p->id, itemid);
	response_proto_head(p,p->waitcmd, 0);
	return 0;
}

/*
 *@ get gifts times
 */
int get_gift_times_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	return db_get_send_gift_times(p);
}

/*
 *@ get home hot from db, handle db 's reply
 */
int get_gift_times_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	response_proto_uint32(p, p->waitcmd, *(uint32_t*)buf, 1);
	return 0;
}

