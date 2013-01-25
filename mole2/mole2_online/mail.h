#ifndef MOLE2_MAIL_H
#define MOLE2_MAIL_H

#include "sprite.h"
#include "global.h"
#include "center.h"

enum {
	mail_normal		= 1,
	mail_with_package	= 2,
	mail_system			= 3,
};

enum {
	sys_info_svr_state	= 0,
	sys_info_kill_boss	= 1,
	sys_info_get_item	= 2,
	sys_info_get_pet	= 3,
	sys_info_maze		= 4,
	sys_info_beast		= 8,
};

#define MAIL_LOOP_MAX	20

#define MAIL_ID_CNT_MAX		((MAIL_ID_LIST_LEN / 4) - 1)

typedef struct mail_id_list{
	uint32_t	count;
	uint32_t	ids[MAIL_ID_CNT_MAX];
}mail_id_list_t;



int load_sys_mail(xmlNodePtr cur);
int load_sys_info(xmlNodePtr cur);

void init_sys_info();
void fini_sys_info();
void init_mail();
void fini_mail();
sysinfo_t* get_sys_info(uint32_t type, uint32_t keyval);

int send_mail_cmd(sprite_t *p, uint8_t *body, uint32_t len);

int get_mail_simple_cmd(sprite_t *p, uint8_t *body, uint32_t len);

int get_mail_simple_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int get_mail_content_cmd(sprite_t *p, uint8_t *body, uint32_t len);

int get_mail_content_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int del_mail_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int mail_set_flag_cmd(sprite_t *p, uint8_t *body, uint32_t len);

int mail_set_flag_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int mail_sys_send_cmd(sprite_t *p, uint8_t *body, uint32_t len);

int mail_send_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);

int mail_items_get_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int mail_items_get_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);


static inline void mail_notify_receiver(sprite_t* p, uint32_t recv_id, uint32_t type)
{
	uint8_t mail_buf[1024];
	int recvlen;

	sprite_t* p_recv = get_sprite(recv_id);

	if (p_recv) {
		recvlen = sizeof(protocol_t);
		PKG_UINT32(mail_buf, type, recvlen);
		init_proto_head(mail_buf, proto_cli_mail_notify, recvlen);
		send_to_self(p_recv, mail_buf, recvlen, 0);
	} else {
		recvlen = 0;
		PKG_H_UINT32(mail_buf, recv_id, recvlen);
		PKG_H_UINT32(mail_buf, proto_cli_mail_notify, recvlen);
		PKG_H_UINT32(mail_buf, type, recvlen);
		notify_across_switch(p, mail_buf, recvlen);
	}
};

static inline uint64_t mail_get_key(uint32_t type, uint32_t value1, uint32_t value2)
{	
	return ((((uint64_t)type) << 56) + (((uint64_t)value2) <<32 ) + ((uint64_t)value1));
}

static inline int send_mail_to_db(sprite_t* p,
			uint32_t recvid,
			uint32_t flag, 
			uint32_t type, 
			uint32_t themeid, 
			uint32_t senderid, 
			char* sz_nick, 
			char* sz_title, 
			char* sz_msg, 
			char* sz_items,
			char* sz_ids)
{
	char buf[4096] = {0};
	int j = 0;
	int ret;
	PKG_H_UINT32(buf, flag, j);
	PKG_H_UINT32(buf, type, j);
	PKG_H_UINT32(buf, themeid, j);
	PKG_H_UINT32(buf, senderid, j);
	PKG_STR(buf, sz_nick, j, MAIL_SEND_NICK_LEN);
	PKG_STR(buf, sz_title, j, MAIL_TITLE_LEN);
	PKG_STR(buf, sz_msg, j, MAIL_MESSAGE_LEN);
	if (sz_items) {
		PKG_STR(buf, sz_items, j, MAIL_ITEM_LIST_LEN);
	} else {
		j += MAIL_ITEM_LIST_LEN;
	}
	if (sz_ids) {
		PKG_STR(buf, sz_ids, j, MAIL_ID_LIST_LEN);
	} else {
		j += MAIL_ID_LIST_LEN;
	}
	
	if ( type != mail_with_package) {
		ret = send_request_to_db(NULL, recvid, proto_db_send_mail, buf, j);
	} else {
		ret = send_request_to_db(p, recvid, proto_db_send_mail, buf, j);
	}
	if (SUCC == ret && type != mail_with_package) {
		mail_notify_receiver(p, recvid, type);
	}
	return 0;
};


static inline int send_dbmail(sprite_t * p, uint32_t mailid)
{
	if(mailid > 0 && mailid < MAX_MAIL_COUNT) {
		mail_t* p_mail = &sz_mail[mailid];
		if(p_mail->indexid == mailid) {
			return send_mail_to_db(p, p->id, 0, mail_system, p_mail->themeid, 0, p_mail->send_nick, p_mail->title, p_mail->message, p_mail->itmlist, NULL);
		}
	}

	return 0;
}

static inline void send_mail_loop(sprite_t* p, uint32_t index[],char* p_idlist)
{
	int loop;
	for (loop = 0; loop < MAIL_IDX_CNT; ++loop) {
		if (!index[loop]) break;
		mail_t* p_mail = &sz_mail[index[loop]];		
		KDEBUG_LOG(p->id, "MAIL SEND LEVEL [idx=%u %s %s]", p_mail->indexid, p_mail->send_nick, p_mail->title);
		send_mail_to_db(p, p->id, 0, mail_system, p_mail->themeid, 0, p_mail->send_nick, p_mail->title, p_mail->message, p_mail->itmlist, p_idlist);
	}
};

static inline void try_to_send_for_level(sprite_t* p, uint32_t old_v, uint32_t new_v)
{
	for (uint32_t i = old_v + 1; i < new_v + 1; ++i) {
		uint64_t key = mail_get_key(mail_sys_level, i, 0);
		mail_op_t *p_o = (mail_op_t *)g_hash_table_lookup(all_mails, &key);
		if (p_o) {
			send_mail_loop(p, p_o->idx, NULL);
		}
	}
}

/**
 * @brief 发送系统通知邮件 
 *
 * @param p
 */
void try_to_send_for_noti(sprite_t* p);
	/*{*/
	/*uint32_t loop;*/
	/*for (loop = 0; loop < MAX_MAIL_COUNT; ++loop) {*/
	/*mail_op_t* p_o = &sz_mail_op[mail_sys_xm][loop];*/
	/*item_t* pi = cache_get_day_limits(p, p_o->value1);*/
	/*DEBUG_LOG(p->id,"try_to_send_for_noti");*/
	/*if (!pi->count) {*/
	/*//检查如果没发过就发送*/
	/*pi->count++;*/
	/*db_day_add_ssid_cnt(NULL,p->id, ssid_mail_noti_seer,1,-1);*/
	/*send_mail_loop(p, p_o->idx, NULL);*/
	/*}*/
	/*}*/
	/*}*/

static inline void try_to_send_for_xm(sprite_t* p, uint32_t old_v, uint32_t new_v)
{
	uint32_t loop;
	for (loop = 0; loop < MAX_MAIL_COUNT; ++loop) {
		mail_op_t* p_o = &sz_mail_op[mail_sys_xm][loop];
		if (!p_o->value1) break;
		if (old_v > p_o->value1 && new_v < p_o->value1) {
			send_mail_loop(p, p_o->idx, NULL);
		}
	}
}

static inline void try_to_send_for_login(sprite_t* p, uint32_t old_v, uint32_t new_v)
{
// 7 * 24 * 60 *60
	if ((new_v - old_v) > (7 * 24 * 60 *60)) {
		uint64_t key = mail_get_key(mail_sys_login, 1, 0);
		mail_op_t* p_o = (mail_op_t *)g_hash_table_lookup(all_mails, &key);
		if (p_o) {
			send_mail_loop(p, p_o->idx, NULL);
		}
	}
}

static inline void try_to_send_for_task(sprite_t* p, uint32_t taskid)
{
	uint64_t key = mail_get_key(mail_sys_task, taskid, 0);
	mail_op_t* p_o = (mail_op_t *)g_hash_table_lookup(all_mails, &key);
	if (p_o) {
		send_mail_loop(p, p_o->idx, NULL);
	}
}

static inline void try_to_send_for_clothes(sprite_t* p, uint32_t clothes_type, uint32_t old_duration, uint32_t new_duration, uint16_t mduration)
{
	int start	= old_duration < new_duration ? old_duration : new_duration;
	int end		= old_duration < new_duration ? new_duration : old_duration;
	for (int loop = start; loop < end; ++loop) {
		if (loop - start > MAIL_LOOP_MAX) break;
		uint64_t key = mail_get_key(mail_sys_clothes, loop, mduration);
		mail_op_t* p_o = (mail_op_t *)g_hash_table_lookup(all_mails, &key);
		if (p_o) {
			mail_id_list_t idlist = {};
			idlist.count = 1;
			idlist.ids[0] = clothes_type;
			send_mail_loop(p, p_o->idx, (char*)&idlist);
		}
	}
}



#endif
