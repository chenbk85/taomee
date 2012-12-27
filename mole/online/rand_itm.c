#include <libtaomee/time/time.h>
#include <statistic_agent/msglog.h>
#include <libtaomee/log.h>

#include "exclu_things.h"
#include "rand_itm.h"

static do_rand_type_t rand_itms[400];
static uint32_t entry_maxid = 0;

int parse_rand_type_items(do_rand_type_t *p_rand_type, xmlNodePtr cur)
{
	int j = 0;
	while(cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item"))) {
			if (j >= MAX_RAND_CNT) {
				ERROR_RETURN(("out of max range type : %d", j), -1);
			}
			DECODE_XML_PROP_INT (p_rand_type->ittitem[j].type, cur, "Type");
			if ((p_rand_type->ittitem[j].type < RAND_TYPE_BASE) || (p_rand_type->ittitem[j].type > MAX_RAND_TYPE)) {
				ERROR_RETURN(("rand type out of range : %u", p_rand_type->ittitem[j].type), -1);
			}
			DECODE_XML_PROP_INT(p_rand_type->ittitem[j].max, cur, "Max");
			DECODE_XML_PROP_INT(p_rand_type->ittitem[j].itmid, cur, "ID");
			item_t* pitm = get_item_prop(p_rand_type->ittitem[j].itmid);
			if (!pitm)
				ERROR_RETURN(("bad itemid[%u]", p_rand_type->ittitem[j].itmid), -1);
			item_kind_t* kind = find_kind_of_item(p_rand_type->ittitem[j].itmid);
			if (!kind)
				ERROR_RETURN(("bad itemid[%u]", p_rand_type->ittitem[j].itmid), -1);

			DECODE_XML_PROP_INT (p_rand_type->ittitem[j].itm_cnt, cur, "Count");
			DECODE_XML_PROP_INT (p_rand_type->ittitem[j].vip, cur, "Vip");
			if (p_rand_type->ittitem[j].vip > 1) {
				ERROR_RETURN(("bad vip[%u]", p_rand_type->ittitem[j].vip), -1);
			}

			j++;
		}
		cur = cur->next;
	}
	p_rand_type->cnt = j;
	return 0;
}

int load_rand_items(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int rand_type_count = 0;
	memset (rand_itms, 0, sizeof(rand_itms));
	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load rand items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	do_rand_type_t *p_rand_type = NULL;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Entry"))) {
			DECODE_XML_PROP_INT(i, cur, "ID");
			if (rand_type_count >= MAX_RAND_FLAG) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, rand_type_count, i);
				goto exit;
			}
			if (i >= MAX_RAND_FLAG) {
				ERROR_LOG ("parse %d failed, rand flag", i);
				goto exit;
			}
			p_rand_type = &rand_itms[i];
			chl = cur->xmlChildrenNode;
			if (parse_rand_type_items(p_rand_type, chl) != 0) {
				goto exit;
			}
			if(entry_maxid < i) {
				entry_maxid = i;
			}
			rand_type_count++;
		}
		cur = cur->next;
	}
	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load exchange item file %s", file);
}

/* @brief send to client after add one item
 */
int db_add_rand_item_callback(sprite_t *p)
{
	uint32_t rand_flag = *(uint32_t*)p->session;
	uint32_t itmid = *(uint32_t*)(p->session + 4);
	uint32_t itm_cnt = *(uint32_t*)(p->session + 8);
	DEBUG_LOG("SEND RAND ITM CALLBACK\t[%u %u]", p->id, itmid);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, rand_flag, j);
	PKG_UINT32(msg, itmid, j);
	PKG_UINT32(msg, itm_cnt, j);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

/* @brief add rand item
 */
int db_add_rand_item(sprite_t* p)
{
	uint32_t itmid = *(uint32_t *)(p->session + 4);
	uint32_t itm_cnt = *(uint32_t *)(p->session + 8);

	uint8_t buf[pagesize];
	int j = 0;
	PKG_H_UINT32(buf, 0, j);
	PKG_H_UINT32(buf, 1, j);
	PKG_H_UINT32(buf, 202, j);
	PKG_H_UINT32(buf, 0, j);

	if (pkg_item_kind(p, buf, itmid, &j) == -1) {
		return -1;
	}
	item_t *itm = get_item_prop(itmid);
	if (itm == NULL) {
		return -1;
	}
	PKG_H_UINT32(buf, itmid, j);
	PKG_H_UINT32(buf, itm_cnt, j);
	PKG_H_UINT32(buf, itm->max, j);

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buf, p->id);
}

/* @brief get one item and check the every day limit
 */
int send_rand_item_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	CHECK_BODY_LEN(bodylen, 4);
	int i = 0;
	uint32_t rand_flag;
	UNPKG_UINT32(body, rand_flag, i);

	if (rand_flag > entry_maxid) {
		ERROR_RETURN(("Invalid rand_flag %u", rand_flag), -1);
	}
	if (rand_itms[rand_flag].cnt == 0) {
		ERROR_RETURN(("Invalid rand_flag %u", rand_flag), -1);
	}

	/*randy one item from Entry*/
	uint32_t indx = rand() % rand_itms[rand_flag].cnt;
	item_type_t* pitt = &rand_itms[rand_flag].ittitem[indx];
	if (pitt->type == 0) {
		ERROR_RETURN(("Invalid rand_flag %u", rand_flag), -1);
	}

	*(uint32_t*)p->session = rand_flag;
	*(uint32_t*)(p->session + 4) = pitt->itmid;
	*(uint32_t*)(p->session + 8) = pitt->itm_cnt;

	if(rand_itms[rand_flag].ittitem[indx].vip == 1) {
		if(!ISVIP(p->flag)) {
			return send_to_self_error(p, p->waitcmd, -ERR_no_super_lahm, 1);
		}
	}

	DEBUG_LOG("SEND RAND ITM\t[%u %u]", p->id, pitt->itmid);
	return db_set_sth_done(p, pitt->type, pitt->max, p->id);
}

int rand_item_swap_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	CHECK_BODY_LEN(bodylen, 8);
	int i = 0;
	uint32_t flag;
	uint32_t score;
	uint32_t item_count;
	UNPKG_UINT32(body, score, i);
	UNPKG_UINT32(body, item_count, i);

	if(item_count < 1) {
		ERROR_RETURN(("Invalid item_count %u", item_count), -1);
	}

	switch(score) {
		case 0 ... 1000:
			flag = 0;
			break;
		case 1001 ... 5000:
			flag = 1;
			break;
		case 5001 ... 20000:
			flag = 2;
			break;
		case 20001 ... 50000:
			flag = 3;
			break;
		case 50001 ... 100000:
			flag = 4;
			break;
		default:
			flag = 5;
			break;
	}

	uint32_t uid[2] = {p->id,1};
	msglog(statistic_logfile, 0x02040040, get_now_tv()->tv_sec, uid, 8);
	msglog(statistic_logfile, 0x020F0041 + flag, get_now_tv()->tv_sec, uid, 8);

	int index;
	uint32_t type;
	uint32_t max;
	uint32_t itmid;
	uint32_t itm_cnt;

	switch(score) {
		case 0 ... 1000:
			type = 648;
			max = 99999;
			itmid = 0;
			itm_cnt = 0;
			flag = 45;
			break;
		case 1001 ... 5000:
		{
			item_type_t itt[] = {
				{649,5,190671,1,0},
				{649,5,190671,1,0},
				{649,5,190671,1,0},
				{649,5,190671,1,0},
				{649,5,190671,1,0},
				{649,5,190671,1,0},
				{649,5,190671,1,0},
				{650,99999,190196,3,0},
				{650,99999,190196,3,0},
				{650,99999,190196,3,0}
				};

			index = rand() % (sizeof(itt) / sizeof(item_type_t));
			type = itt[index].type;
			max = itt[index].max;
			itmid = itt[index].itmid;
			itm_cnt = itt[index].itm_cnt;
			flag = 46;
			break;
		}
		case 5001 ... 10000:
		{
			item_type_t itt[] = {
				{651,1,190671,2,0},
				{651,1,190671,2,0},
				{652,99999,190142,3,0},
				{653,99999,190683,3,0}
				};

			index = rand() % (sizeof(itt) / sizeof(item_type_t));
			type = itt[index].type;
			max = itt[index].max;
			itmid = itt[index].itmid;
			itm_cnt = itt[index].itm_cnt;
			flag = 47;
			break;
		}
		case 10001 ... 20000:
		{
			item_type_t itt[] = {
				{654,1,190671,3,0},
				{652,99999,190142,5,0},
				{653,99999,190683,5,0},
				{655,99999,1230045,3,0},
				{655,99999,1230045,3,0}
				};

			index = rand() % (sizeof(itt) / sizeof(item_type_t));
			type = itt[index].type;
			max = itt[index].max;
			itmid = itt[index].itmid;
			itm_cnt = itt[index].itm_cnt;
			flag = 48;
			break;
		}
		default:
		{
			item_type_t itt[] = {
				{656,1,190671,10,0},
				{656,1,190671,10,0},
				{656,1,190671,10,0},
				{656,1,190671,10,0},

				{657,1,1270010,1,0},
				{657,1,1270010,1,0},
				{657,1,1270010,1,0},
				{657,1,1270010,1,0},

				{658,99999,1230005,3,0},

				{659,99999,1230048,3,0}
				};
			index = rand() % (sizeof(itt) / sizeof(item_type_t));
			type = itt[index].type;
			max = itt[index].max;
			itmid = itt[index].itmid;
			itm_cnt = itt[index].itm_cnt;
			flag = 49;
			break;
		}
	}

	*(uint32_t*)p->session = flag;
	*(uint32_t*)(p->session + 4) = itmid;
	*(uint32_t*)(p->session + 8) = itm_cnt;
	*(uint32_t*)(p->session + 12) = item_count;

	DEBUG_LOG("SEND RAND ITM\t[%u %u]", p->id, itmid);
	return db_set_sth_done(p, type, max, p->id);
}


