
#include "mail.h"

#include <assert.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <glib.h>
#include <libxml/tree.h>

#include "benchapi.h"
#include "cli_proto.h"
#include "sns.h"
#include "items.h"


#define MAIL_UID_CNT		10
#define MAIL_ITEM_CNT_MAX		8
#define MAIL_FLAG_HAS_READ		2
#define MAIL_SYS_INFO_BASE_XIAOMEE	100
#define MAIL_XM_PER_MAIL		10

typedef struct {
	uint32_t itemid;
	uint16_t count;
} __attribute__((packed)) mail_item_t;

typedef struct {
	uint32_t count;
	mail_item_t items[0];
} __attribute__((packed)) mail_item_list_t;

int load_sys_mail_items(mail_t *p_mail,xmlNodePtr cur)
{
	uint32_t count = 0;
	uint32_t itemid = 0;
	uint32_t itmcnt = 0;
	mail_item_list_t *p_list = (mail_item_list_t *)p_mail->itmlist;

	memset(p_mail->itmlist,0, sizeof(p_mail->itmlist));
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Item")) {
			if(count == MAIL_ITEM_CNT_MAX) {
				ERROR_RETURN(("too mouch items (count=%u) for mail (idx=%u)", count, p_mail->indexid), -1);
			}
			DECODE_XML_PROP_UINT32(itemid, cur, "ID");
			if(itemid > 60000 && get_item(itemid) == NULL) {
				ERROR_RETURN(("mail item not exsit id=%u",  itemid), -1);
			}
			DECODE_XML_PROP_UINT32(itmcnt, cur, "Count");
			if(itmcnt >= 30000) {
				ERROR_RETURN(("too much items (count=%u) for item (id=%u)", itmcnt, itemid), -1);
			}

			p_list->items[count].itemid = itemid;
			p_list->items[count].count  = itmcnt;

			count++;
		}
		cur = cur->next;
	}
	p_list->count = count;

	return 0;
}

int load_sys_mail_info(xmlNodePtr cur)
{
	uint32_t	id;
	uint32_t	themeid;
	char	sz_nick[MAIL_SEND_NICK_LEN];
	char	sz_title[MAIL_TITLE_LEN];
	char	sz_msg[MAIL_MESSAGE_LEN];

	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Mail")) { 
			DECODE_XML_PROP_UINT32(id, cur, "ID");
			if (id < 1 || id > MAX_MAIL_COUNT) {
				ERROR_RETURN(("invalid mail id=%u", id), -1);
			}

			decode_xml_prop_uint32_default(&themeid, cur, "ThemeID", 0);
			
			DECODE_XML_PROP_STR(sz_nick, cur, "SenderNick");
			sz_nick[MAIL_SEND_NICK_LEN - 1] = '\0';
			DECODE_XML_PROP_STR(sz_title, cur, "Title");
			sz_title[MAIL_TITLE_LEN - 1] = '\0';
			DECODE_XML_PROP_STR(sz_msg, cur, "Message");
			sz_msg[MAIL_MESSAGE_LEN - 1] = '\0';

			if (sz_mail[id].indexid) {
				ERROR_RETURN(("duplicated mail id=%u", id), -1);
			}
			
			sz_mail[id].indexid 		= id;	
			sz_mail[id].themeid		= themeid;
			memcpy(sz_mail[id].send_nick, sz_nick, sizeof(sz_nick));
			memcpy(sz_mail[id].title, sz_title, sizeof(sz_title));
			memcpy(sz_mail[id].message, sz_msg, sizeof(sz_msg));

			if(load_sys_mail_items(&sz_mail[id], cur)) {
				ERROR_RETURN(("load mail items fail\t[%u]", id), -1);
			}

			KTRACE_LOG(0, "MAIL INFO[id=%u theme=%u sn='%s' ti='%s' ms='%s']", sz_mail[id].indexid,
				sz_mail[id].themeid, sz_mail[id].send_nick, sz_mail[id].title, sz_mail[id].message);
		}

		cur = cur->next;
	}
	return 0;
}

int load_sys_mail_operation(xmlNodePtr cur)
{		
	uint32_t type, sz_index[mail_sys_max];
	memset(sz_index, 0, sizeof(sz_index));

	cur = cur->xmlChildrenNode;	
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Operation")) {
			DECODE_XML_PROP_UINT32(type, cur, "Type");
			if (type < 1 || type >= mail_sys_max)
				ERROR_RETURN(("invalid type\t[%u val2=%u]", type, (1 << 8)), -1);

			mail_op_t* p_o = &sz_mail_op[type][sz_index[type]];
			
			DECODE_XML_PROP_UINT32(p_o->value1, cur, "ValueF");
			decode_xml_prop_uint32_default(&p_o->value2, cur, "ValueS", 0);

			if (p_o->value2 >= (1 << 24))
				ERROR_RETURN(("invalid val2\t[%u %u]", p_o->value2, (1 << 24)), -1);

			p_o->key	= mail_get_key(type, p_o->value1, p_o->value2);
			decode_xml_prop_arr_int_default((int*)p_o->idx, MAIL_IDX_CNT, cur, "Index", 0);
			int loop;
			for (loop = 0 ; loop < MAIL_IDX_CNT; ++loop) {
				if (p_o->idx[loop] >= MAX_MAIL_COUNT - 1) {
					ERROR_RETURN(("invalid mail idx [idx=%d loop = %u]", p_o->idx[loop], loop), -1);
				}
			}
			
			++sz_index[type];
			if (sz_index[type] >= MAX_MAIL_COUNT - 1) {
				ERROR_RETURN(("invalid mail sz_index type [%u]", type), -1);
			}
			if (g_hash_table_lookup(all_mails, &p_o->key)) {
				ERROR_RETURN(("duplicated mail op [key=%lu type=%u v1=%u v2=%u]", p_o->key, type, p_o->value1, p_o->value2), -1);				
			}
			g_hash_table_insert(all_mails, &p_o->key, p_o);
		
			KTRACE_LOG(0, "MAIL OP[type=%u val1=%u index=%u %u %u %u]", type, p_o->value1, p_o->idx[0], p_o->idx[1], p_o->idx[2], p_o->idx[3]);
		}
		cur = cur->next;
	}
	return 0;
}

void init_mail()
{
	all_mails = g_hash_table_new(g_int64_hash, g_int64_equal);
}

int load_sys_mail(xmlNodePtr cur)
{
	g_hash_table_remove_all(all_mails);
	memset(sz_mail_op, 0, sizeof(sz_mail_op));
	memset(sz_mail, 0, sizeof(sz_mail));
    cur = cur->xmlChildrenNode;
    while (cur) {
        if (!xmlStrcmp(cur->name, (const xmlChar*)"Operations")) {
            if (-1 == load_sys_mail_operation(cur)) return -1;
        } else if (!xmlStrcmp(cur->name, (const xmlChar*)"Mails")) {
            if (-1 == load_sys_mail_info(cur)) return -1;;
        }
        cur = cur->next;
    }
    return 0;
}

void fini_mail()
{
	g_hash_table_destroy(all_mails);
}

sysinfo_t* get_sys_info(uint32_t type, uint32_t keyval)
{
	uint64_t key = mail_get_key(type, keyval, 0);
	return (sysinfo_t *)g_hash_table_lookup(sys_infos, &key);
}

void init_sys_info()
{
	sys_infos = g_hash_table_new(g_int64_hash, g_int64_equal);
}

static gboolean free_sys_info_by_key(gpointer key, gpointer psys, gpointer userdata)
{
	g_slice_free1(sizeof(sysinfo_t), psys);
	return TRUE;
}

void fini_sys_info()
{
	g_hash_table_destroy(sys_infos);
}

int load_sys_info(xmlNodePtr cur)
{
	g_hash_table_foreach_remove(sys_infos, free_sys_info_by_key, 0);

	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Info")) {
			uint32_t type, keyval;
			DECODE_XML_PROP_UINT32(type, cur, "Type");

			switch(type) {
				case sys_info_maze:
					keyval = 0;
					break;
				case sys_info_kill_boss:
				case sys_info_get_pet:
				case sys_info_get_item:
					DECODE_XML_PROP_UINT32(keyval, cur, "KeyValue");
					break;
				default:
					cur = cur->next;
					continue;
			}
			
			uint64_t key = mail_get_key(type, keyval, 0);
			sysinfo_t* psys = get_sys_info(type, keyval);
			if (psys) {
				KERROR_LOG(0, "duplicate sys info key\t[%u %u]", type, keyval);
				return -1;
			}

			psys = (sysinfo_t *)g_slice_alloc(sizeof(sysinfo_t));
			memset(psys, 0, sizeof(sysinfo_t));
			psys->key = key;
			psys->type = type;
			psys->val1 = keyval;
			decode_xml_prop_uint32_default(&psys->val2, cur, "AdditionInfo", 0);
			g_hash_table_insert(sys_infos, &psys->key, psys);
			DECODE_XML_PROP_STR(psys->message, cur, "Message");
			KDEBUG_LOG(0, "INSERT SYS INFO\t[%u %u]", type, keyval);
		}
		cur = cur->next;
	}
	return 0;
}

int send_mail_cmd(sprite_t *p, uint8_t *body, uint32_t bodylen)
{
	uint32_t	recv_cnt, themeid, msg_cnt;
	char		title[MAIL_TITLE_LEN] = {};
	userid_t	users[MAIL_UID_CNT] = {};
	char	message[MAIL_MESSAGE_LEN] = {};

    CHECK_BODY_LEN_GE(bodylen, MAIL_TITLE_LEN + 12);

	/*! unpack and check */
	int i = 0;
	UNPKG_UINT32(body, themeid, i);
	UNPKG_STR(body, title, i, MAIL_TITLE_LEN);
	title[MAIL_TITLE_LEN  - 1] = '\0';
	UNPKG_UINT32(body, recv_cnt, i);
	UNPKG_UINT32(body, msg_cnt, i);

	if (recv_cnt > MAIL_UID_CNT || recv_cnt == 0 || msg_cnt > MAIL_MESSAGE_LEN ) {
		KERROR_LOG(p->id, "invalid mail data len[recv_cnt=%u msg_cnt%u]", recv_cnt, msg_cnt);
		return send_to_self_error(p, p->waitcmd, cli_err_mail_len, 1);
	}
	
	CHECK_BODY_LEN(bodylen, 76 + recv_cnt * sizeof(userid_t) + msg_cnt * sizeof(char));
	uint32_t loop;
	for (loop = 0; loop < recv_cnt; ++loop) {
		UNPKG_UINT32(body, users[loop], i);
		CHECK_VALID_ID(users[loop]);
	}
	UNPKG_STR(body, message, i, msg_cnt);
	message[MAIL_MESSAGE_LEN - 1] = '\0';
	KDEBUG_LOG(p->id, "MAIL DEBUG[%u '%s' %u %u %u msg='%s']", themeid, title, recv_cnt, msg_cnt, bodylen, message);
	
	/*! send to db */
	if (themeid != 10004) {
		uint32_t xm = recv_cnt * MAIL_XM_PER_MAIL;
		CHECK_XIAOMEE_ENOUGH(p, xm);
		p->xiaomee -= xm;
		db_add_xiaomee(NULL, p->id, - xm);
	}
	monitor_sprite(p, "SEND MAIL");
	for (loop = 0; loop < recv_cnt; ++loop) {
		send_mail_to_db(p, users[loop], 0, mail_normal, themeid, p->id, p->nick, title, message, NULL, NULL);
	}

	uint8_t buf[1024];
	int sendlen = sizeof(protocol_t);
	PKG_UINT32(buf, p->xiaomee, sendlen);
	init_proto_head(buf, proto_cli_send_mail, sendlen);
	send_to_self(p, buf, sendlen, 1);
	return 0;
}

int get_mail_simple_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	return send_request_to_db(p, p->id, proto_db_get_mail_simple, NULL, 0);
}

int get_mail_simple_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	uint32_t	count;
	uint32_t	mailid;
	uint32_t	flag;
	uint32_t	type;
	uint32_t	themeid;
	uint32_t	senderid;
	char		sender_nick[MAIL_SEND_NICK_LEN];
	char		title[MAIL_TITLE_LEN];

	int i = sizeof(protocol_t);
	int j = 0;
	
	UNPKG_H_UINT32(buf, count, j);
	PKG_UINT32(msg, count, i);
	KDEBUG_LOG(p->id, "MAIL GET LIST [count=%u]", count);
	CHECK_BODY_LEN(len, 4 + count * 148);

	uint32_t loop;
	for (loop = 0; loop < count; ++loop) {
		UNPKG_H_UINT32(buf, mailid, j);
		UNPKG_H_UINT32(buf, flag, j);
		UNPKG_H_UINT32(buf, type, j);
		UNPKG_H_UINT32(buf, themeid, j);
		UNPKG_H_UINT32(buf, senderid, j);
		UNPKG_STR(buf, sender_nick, j, MAIL_SEND_NICK_LEN);
		UNPKG_STR(buf, title, j, MAIL_TITLE_LEN);

		PKG_UINT32(msg, mailid, i);
		PKG_UINT32(msg, flag, i);
		PKG_UINT32(msg, type, i);
		PKG_UINT32(msg,	themeid, i);
		PKG_UINT32(msg, senderid, i);
		PKG_STR(msg, sender_nick, i, MAIL_SEND_NICK_LEN);
		PKG_STR(msg, title, i, MAIL_TITLE_LEN);
	}
	init_proto_head(msg, proto_cli_get_mail_simple, i);
	
	return send_to_self(p, msg, i, 1);
}

int get_mail_content_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t	mailid;
	int i = 0;
	int j = 0;
	
	UNPKG_UINT32(body, mailid, j);

	uint8_t	buf[1024];
	PKG_H_UINT32(buf, mailid, i);
	return send_request_to_db(p, p->id, proto_db_get_mail_content, buf, i);
}

int get_mail_content_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t	mailid;
	char	message[MAIL_MESSAGE_LEN];
	char	item_list[MAIL_ITEM_LIST_LEN];
	char	id_list[MAIL_ID_LIST_LEN];

	int i = 0;
	int j = sizeof(protocol_t);
	int tmplen;
	UNPKG_H_UINT32(buf, mailid, i);
	UNPKG_STR(buf, message, i, MAIL_MESSAGE_LEN); 	
	UNPKG_STR(buf, item_list, i, MAIL_ITEM_LIST_LEN);
	UNPKG_STR(buf, id_list, i, MAIL_ID_LIST_LEN);

	mail_item_list_t* p_items	= (mail_item_list_t*)item_list;
	mail_id_list_t* p_ids		= (mail_id_list_t*)id_list;
	//KDEBUG_LOG(p->id, "MAIL CONTENT INFO [%s slen= %u cnt=%u]", message, strlen(message), p_items->count);
	if (p_items->count > MAIL_ITEM_CNT_MAX) {
		KERROR_LOG(p->id, "error mail item count [cnt=%u]", p_items->count);
		return 0;
	}
	if (p_ids->count > MAIL_ID_CNT_MAX) {
		KERROR_LOG(p->id, "error mail id count [cnt=%u]", p_ids->count);
		memset(p_ids, 0, MAIL_ID_LIST_LEN);
	}
	tmplen = strlen(message) < MAIL_MESSAGE_LEN ? strlen(message) : MAIL_MESSAGE_LEN;
	PKG_UINT32(msg, mailid, j);
	PKG_UINT32(msg, tmplen, j);
	PKG_UINT32(msg, p_items->count, j);
	PKG_UINT32(msg, p_ids->count, j);
	PKG_STR(msg, message, j, tmplen);
	
	uint32_t loop;
	for (loop = 0; loop < p_items->count; ++loop) {
		PKG_UINT32(msg, p_items->items[loop].itemid, j);
		PKG_UINT32(msg, p_items->items[loop].count, j);
	}
	for (loop = 0; loop < p_ids->count; ++loop) {
		PKG_UINT32(msg, p_ids->ids[loop], j);
		//KDEBUG_LOG(p->id, "MAIL ID LIST INFO[cnt=%u id=%u]", p_ids->count, p_ids->ids[loop]);
	}

	msg_log_read_mail(p->id);
	init_proto_head(msg, proto_cli_get_mail_content, j);
	
	return send_to_self(p, msg, j, 1);
}

int cli_del_mail(sprite_t* p, Cmessage* c_in)
{
	cli_del_mail_in* p_in = P_IN;
	
	int j = 0;
	uint8_t	buf[8192] = {};
	uint32_t count = p_in->mail_ids.size();
	PKG_H_UINT32(buf, count, j);

	for (uint32_t loop = 0; loop < count; ++loop) {
		PKG_H_UINT32(buf, p_in->mail_ids[loop], j);
	}
	
	return send_request_to_db(p, p->id, proto_db_del_mail, buf, j);
}

int del_mail_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t	count;
	uint32_t	mailid;
	
	int recvlen = 0;
	UNPKG_H_UINT32(buf, count, recvlen);
	CHECK_BODY_LEN(len, 4 + count * 4);
	
	cli_del_mail_out cli_out;
	for (uint32_t loop = 0; loop < count; ++loop) {
		UNPKG_H_UINT32(buf, mailid, recvlen);
		cli_out.mail_ids.push_back(mailid);
	}
	return send_msg_to_self(p, p->waitcmd, &cli_out, 1);
}

int mail_set_flag_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t mailid;
	uint32_t flag;
	uint32_t recvlen = 0;
	uint32_t sendlen = 0;
	
	UNPKG_UINT32(body, mailid, recvlen);
	UNPKG_UINT32(body, flag, recvlen);
	switch (flag) {
		case MAIL_FLAG_HAS_READ: //has read
			break;
		default:
			KERROR_LOG(p->id, "mail invalid set flag [flag=%u]", flag);
			return send_to_self_error(p, proto_cli_mail_set_flag, cli_err_mail_len, 1);
	}

	uint8_t buf[8];
	PKG_H_UINT32(buf, mailid, sendlen);
	PKG_H_UINT32(buf, flag,	sendlen);

	return send_request_to_db(p, p->id, proto_db_mail_set_flag, buf, sendlen);
}

int mail_set_flag_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	uint32_t	mailid;
	uint32_t	flag;
	uint32_t	recvlen = 0;
	uint32_t	sendlen	= sizeof(protocol_t);

	UNPKG_H_UINT32(buf, mailid, recvlen);
	UNPKG_H_UINT32(buf, flag, recvlen);

	PKG_UINT32(msg, mailid, sendlen);
	PKG_UINT32(msg, flag, sendlen);
	init_proto_head(msg, proto_cli_mail_set_flag, sendlen);
	return send_to_self(p, msg, sendlen, 1);
}

int mail_sys_send_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	uint32_t mid;
	int recvlen = 0;
	int sendlen = 0;
	UNPKG_UINT32(body, mid, recvlen);
	if (mid < 1 || mid >= MAX_MAIL_COUNT) {
		KERROR_LOG(p->id, "invalid sys mail indexid[mid=%u]", mid);
		return send_to_self_error(p, proto_cli_mail_sys_send, cli_err_mail_len, 1);
	}
	uint8_t buf[1024];
	PKG_H_UINT32(buf, mid, sendlen);
	return send_mail_to_db(p, p->id, 0, mail_system, sz_mail[mid].themeid, 0, sz_mail[mid].send_nick, sz_mail[mid].title, sz_mail[mid].message, NULL, NULL);
}

int mail_send_callback(sprite_t * p,uint32_t id,uint8_t * buf,uint32_t len,uint32_t ret)
{
	CHECK_DBERR(p, ret);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

int mail_items_get_cmd(sprite_t* p, uint8_t* body, uint32_t len)
{
	int bytes = 0;
	uint32_t mailid;
	UNPKG_UINT32(body, mailid, bytes);
	
	p->sess_len = 0;
	PKG_H_UINT32(p->session,mailid,p->sess_len);
	PKG_H_UINT32(p->session,0,p->sess_len);
	return send_request_to_db(p, p->id, proto_db_mail_items_get, p->session,p->sess_len);
}

int mail_items_get_callback(sprite_t* p, uint32_t id, uint8_t* buf, uint32_t len, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	uint32_t i = 0;
	int bytes = 0;
	uint32_t mailid;
	uint32_t isrecv = 0;

	p->sess_len = 0;
	UNPKG_H_UINT32(p->session, mailid, bytes);
	UNPKG_H_UINT32(p->session, isrecv, bytes);
	mail_item_list_t *p_items = (mail_item_list_t *)buf;

	normal_item *pni;

	if(isrecv == 0) {
		while(i < p_items->count) {
			if(p_items->items[i].count) {
				if(p_items->items[i].itemid != 50001) {
					pni = get_item(p_items->items[i].itemid);
					if(pni) {
						CHECK_CAN_HAVE_MORE_ITEM(p,pni,p_items->items[i].count);
					}
				}
			}
			i++;
		}

		p->sess_len = 0;
		PKG_H_UINT32(p->session,mailid,p->sess_len);
		PKG_H_UINT32(p->session,1,p->sess_len);
		return send_request_to_db(p, p->id, proto_db_mail_items_get, p->session,p->sess_len);
	}

	while(i < p_items->count) {
		if(p_items->items[i].count) {
			if(p_items->items[i].itemid == 50001) {
				p->xiaomee += p_items->items[i].count;
				monitor_sprite(p, "MAIL GET");
			} else {
				cache_add_kind_item(p, p_items->items[i].itemid, p_items->items[i].count);
			}
		}
		i++;
	}

	response_proto_uint32(p, p->waitcmd, mailid, 1, 0);
	return 0;
}

void try_to_send_for_noti(sprite_t* p)
{
	uint32_t loop;
	for (loop = 0; loop < MAX_MAIL_COUNT; ++loop) {
		mail_op_t* p_o = &sz_mail_op[mail_sys_noti][loop];
		item_t* pi = cache_get_day_limits(p, p_o->value1);
		//DEBUG_LOG("ixxxxxxxx %u %u %u %u %u",p_o->value1,p_o->idx[0],p_o->idx[1],p_o->idx[2],p_o->idx[3]);
		if (!pi->count) {
			//检查如果没发过就发送
			pi->count++;
			db_day_add_ssid_cnt(NULL,p->id, ssid_mail_noti_seer,1,-1);
			send_mail_loop(p, p_o->idx, NULL);
		}
	}
}
