
#ifndef ONLINE_LAHM_DIARY_H
#define ONLINE_LAHM_DIARY_H

#include "benchapi.h"
#include "dbproxy.h"

#define MAX_DIARY_TITLE_LEN		31
#define MAX_LAHM_SURPRISE_LEN	61
#define MAX_WHISPER_LEN			31
#define MAX_STAR_LEN			61
#define MAX_ABC_LEN				145
#define MAX_CONTENT_LEN			601


#define db_set_lahm_diary(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_SET_LAHM_DIARY, (p_), 942, (buf_), (uid_))

#define db_get_lahm_diarys_title(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_GET_DIARY_TITLE, (p_), 12, (buf_), (uid_))

#define db_del_lahm_diary(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_DELETE_DIARY, (p_), 4, (buf_), (uid_))

#define db_set_diary_lock(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_SET_DIARY_LOCK, (p_), 8, (buf_), (uid_))

#define db_send_diary_flower(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_SEND_FLOWER, (p_), 4, (buf_), (uid_))

#define db_get_diary_content(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_GET_LAHM_DIARY, (p_), 8, (buf_), (uid_))

#define db_set_big_lock(p_, buf_, uid_) \
		send_request_to_db(SVR_PROTO_SET_BIG_LOCK, (p_), 4, (buf_), (uid_))

#define db_get_big_lock(p_, uid_) \
		send_request_to_db(SVR_PROTO_GET_BIG_LOCK, (p_), 0, NULL, (uid_))

#define db_get_diary_num(p_, uid_) \
		send_request_to_db(SVR_PROTO_GET_DIARY_NUM, (p_), 0, NULL, (uid_))

int edit_diary_cmd(sprite_t *p, uint8_t *body, int bodylen);
int edit_diary_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_diarys_title_cmd(sprite_t *p, uint8_t *body, int bodylen);
int get_diarys_title_callback(sprite_t* p, uint32_t id, char* buf, int len);
int delete_diary_cmd(sprite_t *p, uint8_t *body, int bodylen);
int delete_diary_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_diary_lock_state_cmd(sprite_t *p, uint8_t *body, int bodylen);
int set_diary_lock_state_callback(sprite_t* p, uint32_t id, char* buf, int len);
int send_diary_flower_cmd(sprite_t *p, uint8_t *body, int bodylen);
int send_diary_flower_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_diary_content_cmd(sprite_t *p, uint8_t *body, int bodylen);
int get_diary_content_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_big_lock_cmd(sprite_t *p, uint8_t *body, int bodylen);
int set_big_lock_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_big_lock_cmd(sprite_t *p, uint8_t *body, int bodylen);
int get_big_lock_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_diary_num_cmd(sprite_t *p, uint8_t *body, int bodylen);
int get_diary_num_callback(sprite_t* p, uint32_t id, char* buf, int len);

#endif
