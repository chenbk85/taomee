#ifndef MOLE_CLASS_H
#define MOLE_CLASS_H
#include "benchapi.h"

#define CHECK_VALID_CLASS(uid_) do {\
    if (!uid_||IS_NPC_ID(uid_)||IS_GUEST_ID(uid_)) {\
        DEBUG_LOG("CHECK VALID CLASS [%d %d %d]", p->id, p->waitcmd, uid_);\
        send_to_self_error(p, p->waitcmd, -ERR_class_not_exist, 1); \
        return 0; \
    }\
}while(0)

static inline int class_not_valid(uint32_t uid)
{
	if (!uid||IS_NPC_ID(uid)||IS_GUEST_ID(uid)) {
		return 1;
	}

	return 0;
}

static inline int db_class_leave_message(sprite_t *p, uint32_t mapid, const uint8_t* content)
{
	char buf[200 + 24];
	int i = 0;

	PKG_H_UINT32 (buf, p->id, i);
	PKG_STR (buf, p->nick, i, 16);
	PKG_H_UINT32 (buf, get_now_tv()->tv_sec, i);
	strcpy(buf + i, (const char*)content);
//	PKG_STR (buf, content, i, 200);

	return send_request_to_db(SVR_PROTO_LEAVE_CLASS_MSG, p, sizeof(buf), buf, mapid);
}

static inline int db_class_verify_message(sprite_t *p, uint32_t msgid, uint8_t flag)
{
	char buf[5];
	int i = 0;

	PKG_H_UINT32 (buf, msgid, i);
	buf[4] = flag;
	return send_request_to_db(SVR_PROTO_VERIFY_CLASS_MSG, 0, sizeof(buf), &buf, p->id);
}

static inline int db_class_reply_message(sprite_t *p, uint32_t msgid, const uint8_t* content)
{
	char buf[100 + 4];
	int i = 0;

	PKG_H_UINT32 (buf, msgid, i);
	strcpy(buf + i, (const char*)content);
//	PKG_STR (buf, content, i, 200);

	return send_request_to_db(SVR_PROTO_REPLY_CLASS_MSG, 0, sizeof(buf), buf, p->id);
}

static inline int db_class_get_messages(sprite_t *p, uint32_t mapid, uint32_t pos)
{
	char buf[8];
	int i = 0;

	PKG_H_UINT32 (buf, p->id, i);
	PKG_H_UINT32 (buf, pos, i);

	return send_request_to_db(SVR_PROTO_GET_CLASS_MSG, p, i, buf, mapid);
}

static inline int db_class_del_message(sprite_t *p, uint32_t msgid, uint32_t flag)
{
	char buf[8];
	int i = 0;

	PKG_H_UINT32 (buf, msgid, i);
	PKG_H_UINT32 (buf, flag, i);
	return send_request_to_db(SVR_PROTO_DEL_CLASS_MSG, 0, i, buf, p->id);
}


int create_class_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_class_info_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_class_simple_info_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int modify_class_info_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int set_first_class_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_first_class_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int join_class_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int quit_class_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int set_itm_class_cmd(sprite_t* p, uint8_t *body, int bodylen);
int get_class_logo_info_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int request_join_class_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int reply_request_join_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int delete_class_member_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int delete_class_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_usr_class_list_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int lock_class_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int unlock_class_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_class_unused_itm_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_member_list_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_top_class_list_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_class_honor_cmd(sprite_t* p, const uint8_t *body, int bodylen);

int buy_itm_class(sprite_t* p, int itmid, int itm_cnt, int itm_max);
int del_class_item(sprite_t* p);
int clean_class(sprite_t* p, uint32_t classid);
int check_class(sprite_t *p, uint32_t uid);

int moleclass_op(uint32_t uid, void* buf, int len);

int create_class_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_class_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_class_simple_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int modify_class_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int set_first_class_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_first_class_callback(sprite_t *p, uint32_t id, char *buf, int len);
int join_class_callback(sprite_t *p, uint32_t id, char *buf, int len);
int quit_class_callback(sprite_t *p, uint32_t id, char *buf, int len);
int set_itm_class_callback(sprite_t *p, uint32_t id, char *buf, int len);
int change_xiaomee_callback(sprite_t *p, uint32_t id, char *buf, int len);
int add_class_itm_callback(sprite_t *p, uint32_t id, char *buf, int len);
int add_class_member_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_class_logo_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int delete_class_member_callback(sprite_t *p, uint32_t id, char *buf, int len);
int delete_class_callback(sprite_t *p, uint32_t id, char *buf, int len);
int set_class_visit_flag_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_class_unused_itm_callback(sprite_t *p, uint32_t id, char *buf, int len);
int del_class_unused_itm_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_member_list_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_usr_class_list_callback(sprite_t *p, uint32_t id, char *buf, int len);
int set_class_reward_callback(sprite_t *p, uint32_t id, char *buf, int len);
int set_class_member_reward_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_class_honor_callback(sprite_t *p, uint32_t id, char *buf, int len);

//tmp: class question

int load_class_questions(const char *file);
int get_class_ques_stat_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_class_ques_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int answer_class_ques_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_class_ques_info_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_class_ques_reward_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int get_class_ques_member_reward_cmd(sprite_t* p, const uint8_t *body, int bodylen);

int get_class_ques_stat_callback(sprite_t *p, uint32_t id, char *buf, int len);
int submit_class_ques_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_class_ques_info_callback(sprite_t *p, uint32_t id, char *buf, int len);
int get_class_ques_reward_callback(sprite_t *p, uint32_t id, char *buf, int len);
int request_add_class_item(sprite_t* p, uint32_t classid, int itmid, int itm_cnt, int itm_max, int ret_needed);
#endif
