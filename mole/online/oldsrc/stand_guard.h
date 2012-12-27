
#ifndef  STAND_GUARD_H
#define  STAND_GUARD_H

#include "benchapi.h"
#include "dbproxy.h"


typedef struct guard_info {
	uint32_t task;
    uint32_t count;
} __attribute__((packed)) guard_info_t;

#define SVR_PROTO_GET_STAND_GUARD_INFO	0xD010
#define SVR_PROTO_SET_GUARD_TASK_STATUS 0xD111
#define SVR_PROTO_SUBMIT_STANDING_ONCE	0xD112

#define db_get_guard_info(p_) \
		send_request_to_db(SVR_PROTO_GET_STAND_GUARD_INFO, (p_), 0, 0, (p_)->id);

#define db_set_guard_task_status(p_, buf) \
		send_request_to_db(SVR_PROTO_SET_GUARD_TASK_STATUS, (p_), 4, &buf, (p_)->id);

#define db_submit_stand_once(p_) \
		send_request_to_db(SVR_PROTO_SUBMIT_STANDING_ONCE, (p_), 0, 0, (p_)->id);

int submit_once_stand_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_guard_task_status_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int db_get_stand_guard_info_callback(sprite_t* p, uint32_t id, char* buf, int len);


#endif
