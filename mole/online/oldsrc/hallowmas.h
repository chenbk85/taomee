#ifndef ONLINE_HALLOWMAS_H_
#define ONLINE_HALLOWMAS_H_

#include "benchapi.h"

#include "dbproxy.h"

#define SVR_PROTO_ADD_CANDYS			0xD106
#define SVR_PROTO_CHK_CANDYS			0xD007
#define SVR_PROTO_SET_HALLOWMAS_TASK	0xD108

// DB Operations
int db_add_candys(sprite_t* p, userid_t uid, int increment, int flag, int ret_needed);
#define db_chk_candys(p_, uid_) \
		send_request_to_db(SVR_PROTO_CHK_CANDYS, (p_), 0, 0, (uid_))
#define db_set_hallowmas_task(p_, task_) \
		send_request_to_db(SVR_PROTO_SET_HALLOWMAS_TASK, 0, 4, (task_), (p_)->id)

// DB Callbacks
int  add_candys_callback(sprite_t* p, uint32_t id, char* buf, int len);
int  chk_candys_callback(sprite_t* p, uint32_t id, char* buf, int len);

// Cmds
int get_candys_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int chk_candy_num_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_candy_can_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_candys_weekly_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int upgrade_candy_can_cmd(sprite_t* p, const uint8_t* body, int bodylen);

int get_pumpkin_lamp_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int lahm_get_candys_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int fetch_candy_dress_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int become_corpse_bat_cmd (sprite_t *p, const uint8_t *body, int len);

#endif // ONLINE_HALLOWMAS_H_
