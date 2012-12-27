/*
 * =====================================================================================
 *
 *       Filename:  race_bonus.c
 *
 *    Description: 比赛活动获取奖励物品
 *
 *        Version:  1.0
 *        Created:  08/16/2010 09:01:01 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tommychen
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */


#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "race_bonus.h"


#define MAX_RACEID 100
#define MAX_ITEMS 10
#define RACE_BONUS_TABLE_SIZE 	100

static race_bonus_t race_bonus[RACE_BONUS_TABLE_SIZE];

int gain_race_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);

	uint32_t raceid = 0;
	unpack(body, sizeof(raceid), "L", &raceid);
	if (raceid > MAX_RACEID || raceid <= 0 || race_bonus[raceid-1].raceid != raceid)
	{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_raceid, 1);
	}

	*(uint32_t*)p->session 	= raceid;
	return send_request_to_db(SVR_PROTO_GET_FIRE_CUP_PRIZE, p, 0, NULL, p->id);

}

int get_race_teamid_flag_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t teamid = 0;
	uint32_t flag = 0;
	uint32_t raceid = 0;
	unpkg_host_uint32_uint32((uint8_t *)buf, &teamid, &flag);
	*(uint32_t*)(p->session + 4) = teamid;
	raceid = *(uint32_t*)p->session;

	send_race_bonus_to_db(p, raceid, teamid);

	return 0;

}

static int parse_race_single_item(item_unit_t* iut, uint32_t* cnt, xmlNodePtr cur)
{
	int j = 0;
	int id = 0;
	while (cur)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Item")))
		{
			if (j == MAX_ITEMS)
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

static int parse_race_teams(race_bonus_t *git, uint32_t* cnt, xmlNodePtr chl)
{
	xmlNodePtr cur;
	int i = 0;
	int teamid = 0;

	while (chl)
	{
		if (!xmlStrcmp(chl->name, (const xmlChar *)"Team"))
		{
			DECODE_XML_PROP_INT(teamid, chl, "TeamID");
			git->teams[teamid-1].teamid = teamid;

			cur = chl->xmlChildrenNode;
			if (parse_race_single_item(git->teams[teamid-1].item, &(git->teams[teamid-1].item_count), cur) != 0)
			{
				return -1;
			}
			i++;
		}
		chl = chl->next;
	}

	*cnt = i;
	return 0;

}

int load_race_bonus(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur, chl;
	int i, err = -1;
	int ex_count;

	ex_count = 0;
	memset(race_bonus, 0, sizeof (race_bonus));

	doc = xmlParseFile (file);
	if (!doc) ERROR_RETURN (("load items config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Race"))) {
			DECODE_XML_PROP_INT(i, cur, "RaceID");
			if (ex_count >= RACE_BONUS_TABLE_SIZE - 1 || i <= 0) {
				ERROR_LOG ("parse %s failed, Count=%d, id=%d", file, ex_count, i);
				goto exit;
			}
			race_bonus[i-1].raceid = i;
			if (race_bonus[i-1].raceid > MAX_RACEID) {
				ERROR_LOG ("parse %s failed, Count=%d, raceid=%d", file, ex_count, race_bonus[i-1].raceid);
				goto exit;
			}

			chl = cur->xmlChildrenNode;
			if ( (parse_race_teams(&race_bonus[i-1], &(race_bonus[i-1].team_count), chl) != 0) )
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

int send_race_bonus_to_db(sprite_t *p, uint32_t raceid, uint32_t teamid)
{
	race_bonus_t bonus_t = race_bonus[raceid-1];
	team_item_t team_item = bonus_t.teams[teamid-1];

	uint8_t buff[1024] = {};
	int j = 0;
	int k = 0;
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, team_item.item_count, j);
	PKG_H_UINT32(buff, 202, j);
	PKG_H_UINT32(buff, 0, j);
	for (k = 0; k < team_item.item_count; k++)
	{
		if (pkg_item_kind(p, buff, team_item.item[k].itm->id, &j) == -1)
		{
			return -1;
		}
		PKG_H_UINT32(buff, team_item.item[k].itm->id, j);
		PKG_H_UINT32(buff, team_item.item[k].count, j);
		PKG_H_UINT32(buff, team_item.item[k].itm->max, j);
	}

	send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);

	return 0;

}

int send_race_bonus_to_client(sprite_t *p, uint32_t raceid, uint32_t teamid)
{
    p->team_id = 0;
	race_bonus_t bonus_t = race_bonus[raceid-1];
	team_item_t team_item = bonus_t.teams[teamid-1];

	int len = sizeof(protocol_t);

	PKG_UINT32(msg, team_item.item_count, len);

	int k = 0;
	for (k = 0; k < team_item.item_count; k++)
	{
		PKG_UINT32(msg, team_item.item[k].itm->id, len);
		PKG_UINT32(msg, team_item.item[k].count, len);
	}

	init_proto_head(msg, p->waitcmd, len);
	send_to_self(p, msg, len, 1);

	return 0;

}


