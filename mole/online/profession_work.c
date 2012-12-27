/*
 * =====================================================================================
 *
 *       Filename:  arch_work.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/06/2010 09:01:01 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "exclu_things.h"
#include "profession_work.h"


struct profession_work_config work_config[MAX_RECORD];


int parse_profession_work_single_item(item_unit_t* iut, int* cnt, xmlNodePtr cur, int flag)
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

int parse_profession_work_items(struct profession_work_config *record, xmlNodePtr chl)
{
	xmlNodePtr cur;

	while (chl) {
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemIn")) {
			cur = chl->xmlChildrenNode;
			if (parse_profession_work_single_item(record->in_item, &(record->in_cnt), cur, 0) != 0)
				return -1;
		}
		if (!xmlStrcmp(chl->name, (const xmlChar *)"ItemOut")) {
			cur = chl->xmlChildrenNode;
			if (parse_profession_work_single_item(record->out_item, &(record->out_cnt), cur, 1) != 0)
				return -1;
		}
		chl = chl->next;
	}
	return 0;
}

int load_profession_work(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int record_count = 1;

	doc = xmlParseFile (file);
	if (!doc) {
		ERROR_RETURN (("load profession work config failed"), -1);
	}
	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Entry"))) {
			DECODE_XML_PROP_INT(i, cur, "ID");
			if (record_count > MAX_RECORD  || record_count != i) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, record_count, i);
				goto exit;
			}
			work_config[i - 1].id = i;

			DECODE_XML_PROP_INT(work_config[i - 1].profession, cur, "PROFESSION");
			if (work_config[i - 1].profession > 50) {
				ERROR_LOG ("parse %s failed, profession = %u", file, work_config[i].profession);
				goto exit;
			}

			DECODE_XML_PROP_INT(work_config[i - 1].level, cur, "LEVEL");
			if (work_config[i - 1].level > 10) {
				ERROR_LOG ("parse %s failed, level=%d", file, work_config[i].level);
				goto exit;
			}

			DECODE_XML_PROP_INT(work_config[i - 1].vip, cur, "VIP");
			if ((work_config[i - 1].vip != 0) && (work_config[i - 1].vip != 1)) {
				ERROR_LOG ("parse %s failed vip=%d", file, work_config[i].vip);
				goto exit;
			}

			chl = cur->xmlChildrenNode;
			if ((parse_profession_work_items(&work_config[record_count - 1], chl) != 0)) {
				goto exit;
			}
			record_count++;
		}
		cur = cur->next;
	}

	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load profession work item file %s", file);
}

/* @brief some check and del some thing and give some thing
 */
int do_profession_work_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	int type = 0, mul = 0, i = 0;
	UNPKG_UINT32(body, type, i);
	UNPKG_UINT32(body, mul, i);


	if ((type > MAX_RECORD ) || (work_config[type - 1].id == 0) || (mul == 0)) {
		ERROR_RETURN(("profession wrong type %d %d", p->id, type), -1);
	}

	if ((p->profession[work_config[type - 1].profession] < work_config[type - 1].level)
		|| (work_config[type - 1].vip > ISVIP(p->flag))) {
		DEBUG_LOG("profession %u  %d", p->id, p->profession[work_config[type - 1].profession]);
		return send_to_self_error(p, p->waitcmd, -ERR_have_not_right_to_do, 1);
	}
	*(uint32_t *)p->session = type - 1;
	*(uint32_t *)(p->session + 4) = mul;


	int j = 0;
	uint8_t buf[1024];
	//package for DB
	PKG_H_UINT32(buf, work_config[type - 1].in_cnt, j);
	PKG_H_UINT32(buf, work_config[type - 1].out_cnt, j);
	PKG_H_UINT32(buf, 100, j);
	PKG_H_UINT32(buf, type, j);
	for (i = 0; i != work_config[type - 1].in_cnt; ++i) {
		if (pkg_item_kind(p, buf, work_config[type - 1].in_item[i].itm->id, &j) == -1)
			return -1;
			PKG_H_UINT32(buf, work_config[type - 1].in_item[i].itm->id, j);
			PKG_H_UINT32(buf, work_config[type - 1].in_item[i].count * mul, j);
	}

	for (i = 0; i != work_config[type - 1].out_cnt; ++i) {
		if (pkg_item_kind(p, buf, work_config[type - 1].out_item[i].itm->id, &j) == -1)
			return -1;
			PKG_H_UINT32(buf, work_config[type - 1].out_item[i].itm->id, j);
			PKG_H_UINT32(buf, work_config[type - 1].out_item[i].count * mul , j);
			PKG_H_UINT32(buf, work_config[type - 1].out_item[i].itm->max, j);
	}

	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buf, p->id);
}


/* @brief give client info about what is add
 */
int do_profession_work_callback(sprite_t *p)
{
	uint32_t type = *(uint32_t *)p->session;
	uint32_t mul = *(uint32_t *)(p->session + 4);

	int i, j = sizeof(protocol_t);
	PKG_UINT32(msg, work_config[type].out_cnt, j);
	for (i = 0; i != work_config[type].out_cnt; ++i) {
			PKG_UINT32(msg, work_config[type].out_item[i].itm->id, j);
			PKG_UINT32(msg, work_config[type].out_item[i].count * mul, j);
	}
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}
