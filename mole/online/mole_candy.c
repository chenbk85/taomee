#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "exclu_things.h"

#include "mole_candy.h"
#include "gift.h"
#include "small_require.h"
#include "thanksgiving.h"
#include "delicous_food.h"
#include "mole_card.h"

#define MAX_TYPE	30

uint32_t pos_array[4] = {0};


send_gift_t candyitem[MAX_TYPE];

typedef struct PosType {
	uint32_t pos;
	uint32_t type;
}pos_info_t;
static int parse_candy_single_item(item_unit_t* iut, int* cnt, xmlNodePtr cur, int flag)
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
			if (flag == 1 && (iut[j].itm)->tradability != 3) {
				ERROR_RETURN(("bad tradability item=%d tradability=%d", id, (iut[j].itm)->tradability), -1);
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

static int parse_candy_items(send_gift_t *sg, xmlNodePtr chl)
{
	xmlNodePtr cur;

	while (chl) {
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemIn")) {
			cur = chl->xmlChildrenNode;
			if (parse_candy_single_item(sg->in_item, &(sg->in_cnt), cur, 0) != 0)
				return -1;
		}
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemOut")) {
			cur = chl->xmlChildrenNode;
			if (parse_candy_single_item(sg->out_item, &(sg->out_cnt), cur, 1) != 0)
				return -1;
		}
		chl = chl->next;
	}
	return 0;
}
int load_candy(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int gifts_count;

	gifts_count = 0;
	memset (candyitem, 0, sizeof (candyitem));

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

			candyitem[i].send_gift_id = i;
			chl = cur->xmlChildrenNode;
			if ( (parse_candy_items(&candyitem[i], chl) != 0) )
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

void clear_array(uint32_t uid)
{
	int i;
	for (i = 0; i < 4; i ++) {
		if (pos_array[i] == uid) {
			pos_array[i] = 0;
		}
	}
}

int update_candy_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t tmp = 1;
	send_request_to_db(SVR_PROTO_ADD_CANDY_TIMES, 0, 4, &tmp, p->id);
	response_proto_head_not_complete(p, PROTO_UPDATE_CANDY, 0);
	return 0;
}

int do_db_add_candy(sprite_t *p, uint32_t type)
{
	if(type >= MAX_TYPE) {
		ERROR_RETURN(("Invalid index candyitem[%d < 30]",type), -1);
	}
	int  count = candyitem[type].out_item[0].count;
	uint32_t  itmid = candyitem[type].out_item[0].itm->id;
	int i = 0;
	uint8_t buf[8] = {0};
	PKG_H_UINT32(buf, itmid, i);
	PKG_H_UINT32(buf, count * p->r4_cnt, i);
	return send_request_to_db(SVR_PROTO_CANDY_MAKE, p, i, buf, p->id);
}

int process_ok(void* owner, void* data)
{
	sprite_t * p = owner;
	if (p == NULL) {
		return -1;
	}
	clear_array(p->id);
	int i = sizeof(protocol_t), pos = 0;
	for (pos = 0; pos < 4; pos ++) {
		PKG_UINT32(msg, pos_array[pos], i);
	}
	init_proto_head(msg, PROTO_CANDY_POSITION_EX, i);
	send_to_map3(56, msg, i);
	return do_db_add_candy(p, p->r4_type);
}


int query_item_cnt_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
	switch (p->waitcmd) {
		case PROTO_CANDY_MAKE:
		{
			uint32_t count;
			int j = 0, flag = 0;
			UNPKG_H_UINT32(buf, count, j);
			CHECK_BODY_LEN(len, count * 8 + 4);
			if (candyitem[p->r4_type].in_cnt != count) {
				flag = 1;
			} else {
				int it;
				for (it = 0; it < count; it++) {
					uint32_t itmid, itmcn, itmid_sav, itmcn_sav;
					UNPKG_H_UINT32(buf, itmid, j);
					UNPKG_H_UINT32(buf, itmcn, j);
					itmid_sav = candyitem[p->r4_type].in_item[it].itm->id;
					itmcn_sav = candyitem[p->r4_type].in_item[it].count * p->r4_cnt;
					if (itmid_sav != itmid || itmcn_sav > itmcn) {
						flag = 1;
						break;
					}
				}
			}
			if (flag) {
				uint32_t pos;
				clear_array(p->id);
				int i = sizeof(protocol_t);
				for (pos = 0; pos < 4; pos ++) {
					PKG_UINT32(msg, pos_array[pos], i);
				}
				init_proto_head(msg, PROTO_CANDY_POSITION_EX, i);
				send_to_map3(56, msg, i);
				return send_to_self_error(p, p->waitcmd, -ERR_candy_wrong_id, 1);
			}
			return do_pkg_candy_and_exch(p, p->r4_type);
		}
		case PROTO_GET_ITEMS_COUNT:
		case PROTO_GET_ITEMS_COUNT_NEW:
			return respond_items_count(p, buf, len);
		case PROTO_SEND_ONE_OF_TWO:
			return do_send_gift_oneoftwo(p, buf, len);
		case PROTO_SEND_GIFT_TWO:
			return do_send_gift_twooffour(p, buf, len);
		case PROTO_EXCHANGE_RAND_ITEM:
			return do_send_one_of_enum_items(p, buf, len);
		case PROTO_TANTALIZE_CAT:
			return tantalize_cat_get_something_callbak(p, buf, len);
			/*case PROTO_EXCHANGE_WITH_REPURATION:*/
			/*return check_if_have_garge_callback(p, buf, len);*/
		case PROTO_CREATE_BULDING:
		{
			int i = 0;
			uint32_t count;
			UNPKG_H_UINT32(buf, count, i);
			if(count) {
				UNPKG_H_UINT32(buf, count, i);
				UNPKG_H_UINT32(buf, count, i);
			}
			if(count == 0) {
				return send_to_self_error(p,p->waitcmd,ERR_not_have_certify, 1);
			}
			return send_request_to_db(SVR_PROTO_CREATE_BUILDING, p, 24, p->session + 4, p->id);
		}
		case PROTO_FIX_CARD_GAME_BUG:
			return get_lance_callback(p, p->id, buf, len);
		case PROTO_GET_SIG_CARD_FROM_NPC:
			return check_red_clothe_callback(p, p->id, buf, len);
		default:
			ERROR_RETURN(("Invalid cmd"), -1);

	}
	return 0;

}
int do_add_make_timer(sprite_t * p)
{
	int delay = 5 * p->r4_cnt;
	if(delay > 60) delay = 60;
	ADD_TIMER_EVENT(p, process_ok, 0, now.tv_sec + delay);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_position_info_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	int pos;
	int i = sizeof(protocol_t);
	for (pos = 0; pos < 4; pos ++) {
		PKG_UINT32(msg, pos_array[pos], i);
	}
	init_proto_head(msg, p->waitcmd, i);
	return send_to_self(p, msg, i, 1);
}

int occupy_position(sprite_t* p, uint32_t pos)
{
	if (pos_array[pos]) {
		return 1;
	}
	int k;
	for (k = 0; k < 4; k++) {
		if (pos_array[k] == p->id)
			return 2;
	}
	pos_array[pos] = p->id;
	return 0;
}

int make_candy_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int j = 0;
	uint32_t pos, type, count;
	UNPKG_UINT32(body, pos, j);
	UNPKG_UINT16(body, type, j);
	UNPKG_UINT16(body, count, j);
	if (pos > 3) {
		return send_to_self_error(p, p->waitcmd, -ERR_candy_invaild_sit, 1);
	}
	if (type > MAX_TYPE)
		ERROR_RETURN(("bad R4 type\t[%u %u]", p->id, type), -1);
	if (count > 100000 || count == 0)
		return send_to_self_error(p, p->waitcmd, -ERR_too_much_to_exchange, 1);
	if(type == 0){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3DF, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if(type == 1){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3E1, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	else if( type == 2){
		uint32_t msg_buff[2] = {p->id, 1};
		msglog(statistic_logfile, 0x0409C3E0, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	}
	*(uint32_t*)p->session = pos;
	*(uint16_t*)(p->session + 4) = type;
	*(uint16_t*)(p->session + 6) = count;
	return send_request_to_db(SVR_PROTO_GET_CANDY_TIMES, p, 0, NULL, p->id);
}

int get_candy_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count;
	int j = 0, i = sizeof(protocol_t);
	UNPKG_H_UINT32(buf, count, j);
	PKG_UINT32(msg, count, i);
	CHECK_BODY_LEN(len, 4 + 8 * count);
	int loop;
	for (loop = 0; loop < count; loop ++) {
		uint32_t itm, cnt;
		UNPKG_H_UINT32(buf, itm, j);
		UNPKG_H_UINT32(buf, cnt, j);
		db_single_item_op(0, p->id, itm, cnt, 1);
		PKG_UINT32(msg, itm, i);
		PKG_UINT32(msg, cnt, i);
	}
	init_proto_head(msg, p->waitcmd, i);

	return send_to_self(p, msg, i, 1);
}

int get_candy_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_CANDY_GET, p, 0, NULL, p->id);
	//return db_chk_itm_cnt(p, candyitem[0].out_item[0].itm->id, candyitem[0].out_item[0].itm->id + 1);
}

int do_pkg_candy_and_exch(sprite_t* p, uint32_t type)
{
	uint8_t buf[512];
	int i, j = 0;
	PKG_H_UINT32(buf, candyitem[type].in_cnt, j);
	PKG_H_UINT32(buf, 0, j);
	PKG_H_UINT32(buf, 0, j);
	PKG_H_UINT32(buf, 0, j);
	for (i = 0; i < candyitem[type].in_cnt; i++) {
		if (pkg_item_kind(p, buf, candyitem[type].in_item[i].itm->id, &j) == -1) {
			return -1;
		}
		PKG_H_UINT32(buf, candyitem[type].in_item[i].itm->id, j);
		PKG_H_UINT32(buf, candyitem[type].in_item[i].count * p->r4_cnt, j);
	}
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buf, p->id);
}

int get_candy_from_recy_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);
	uint32_t item_array[] = {1230010,1230013,1230012,1230002,1230004};
	uint32_t array_len = (sizeof(item_array) / sizeof(item_array[0]));
	uint32_t itmid, j = 0;

	UNPKG_UINT32(body, itmid, j);
	for (j = 0; j < array_len; j ++) {
		if (itmid == item_array[j]) {
			break;
		}
	}
	if (j == array_len)
		return send_to_self_error(p, p->waitcmd, -ERR_candy_invaild_id, 1);
	*(uint32_t*)p->session = itmid;
	uint32_t msg[2] = {1, p->id};
    msglog(statistic_logfile, 0x02020004, get_now_tv()->tv_sec, msg, sizeof(msg));
	return send_request_to_db(SVR_PROTO_GET_CANDY_TIMES, p, 0, NULL, p->id);
}

int get_candy_status_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t status = *(uint32_t*)buf;
	response_proto_uint32(p, p->waitcmd, status, 0);
	return 0;
}

int get_candy_status_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	return send_request_to_db(SVR_PROTO_CANDY_STATUS, p, 0, NULL, p->id);
}

int do_fetch_from_recy(sprite_t *p)
{
	uint32_t itmid = *(uint32_t*)p->session;
	db_single_item_op(0, p->id, itmid, 1, 1);
	response_proto_uint32(p, p->waitcmd, itmid, 0);
	return 0;
}

int fetch_candy_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	uint32_t times = *(uint32_t*)buf;
	switch (p->waitcmd) {
		case PROTO_CANDY_MAKE:
		{
			DEBUG_LOG("fetch_candy_callback[uid=%u times=%u]",p->id, times);
			if (times >= 200) {
				return send_to_self_error(p, p->waitcmd, -ERR_candy_reach_limit, 1);
			}
			uint32_t pos = *(uint32_t*)p->session;
			int err = occupy_position(p, pos);
			if (err == 1) {
				return send_to_self_error(p, p->waitcmd, -ERR_candy_no_sit, 1);
			} else if (err == 2) {
				return send_to_self_error(p, p->waitcmd, -ERR_candy_only_self, 1);
			}
			p->r4_type = *(uint16_t*)(p->session + 4);
			p->r4_cnt = *(uint16_t*)(p->session + 6);

			int loop, i = sizeof(protocol_t);
			for (loop = 0; loop < 4; loop++) {
				PKG_UINT32(msg, pos_array[loop], i);
			}
			init_proto_head(msg, PROTO_CANDY_POSITION_EX, i);
			send_to_map3(56, msg, i);

			int count = candyitem[p->r4_type].in_cnt;
			uint8_t buff[1024];
			int j = 0, it;
			PKG_H_UINT32(buff, count, j);
			for (it = 0; it < count; it++) {
				PKG_H_UINT32(buff, candyitem[p->r4_type].in_item[it].itm->id, j);
			}
			return send_request_to_db(SVR_QUERY_ITEM_COUNT, p, j, buff, p->id);
		}
		case PROTO_CANDY_FROM_RECY:
		{
			if (times >= 0) {
				return db_set_sth_done(p, 205, 2, p->id);

			} else {
				return send_to_self_error(p, p->waitcmd, -ERR_candy_nothing, 1);
			}
		}
		default:
			ERROR_RETURN(("Unexpected WaitCmd %d uid=%u", p->waitcmd, p->id), -1);
     }
	return 0;
}

