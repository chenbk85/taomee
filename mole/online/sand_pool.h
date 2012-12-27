#ifndef SAND_POOL_H_
#define SAND_POOL_H_

#include "benchapi.h"

typedef struct sand_pool_mail{
    uint32_t    flag;
	uint32_t    sent_tm;
	uint16_t    msglen;
	char        msg[];
} __attribute__((packed)) sand_pool_mail_t;

typedef struct send_mail_buff{
	uint16_t    msglen;
	char        msg[];
} __attribute__((packed)) send_mail_buff_t;


int get_time_mail_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_time_mail_callback(sprite_t* p, uint32_t id, char* buf, int len);
int send_time_mail_cmd(sprite_t* p, uint8_t* body, int bodylen);
int send_time_mail_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_time_item_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_time_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
int send_time_item_cmd(sprite_t* p, uint8_t* body, int bodylen);
int send_time_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_time_mail_item_count_cmd(sprite_t* p, uint8_t* body, int bodylen);
int get_time_mail_item_count_callback(sprite_t* p, uint32_t id, char* buf, int len);


#endif

