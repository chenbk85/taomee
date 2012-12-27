
#ifndef ONLINE_SPRING_H
#define ONLINE_SPRING_H

#define SVR_PROTO_GET_YUANBAO_VALUE		0xD040
#define SVR_PROTO_CHANGE_YUANBAO_VALUE	0xD141
#define SVR_PROTO_SET_SPRING_MSG		0xD142
#define SVR_PROTO_GET_SPRING_MSG		0xD043
#define SVR_PROTO_SET_YUANBAO_BOX_FLAG		0xD144
#define SVR_PROTO_GET_YUANBAO_BOX_FLAG		0xD045

#define PET_IS_BECOME_SHUANGJIEGUN(p_) \
		((p_)->suppl_info.cur_form == 1)
#define PET_RECOVER_FROM_SHUANGJIEGUN(p_) \
		((p_)->suppl_info.cur_form = 0)
#define PET_BECOME_SHUANGJIEGUN(p_) \
		((p_)->suppl_info.cur_form = 1)

#define SPRING_MSG_LEN		100

#define db_change_yuanbao_value(p_, buf, id) \
		send_request_to_db(SVR_PROTO_CHANGE_YUANBAO_VALUE, p_, 8, buf, id)
#define db_get_yuanbao_value(p_, id) \
		send_request_to_db(SVR_PROTO_GET_YUANBAO_VALUE, p_, 0, NULL, id)

#define db_set_yuanbao_box_flag(p_) \
		send_request_to_db(SVR_PROTO_SET_YUANBAO_BOX_FLAG, (p_), 0, 0, (p_)->id);
#define db_get_yuanbao_box_flag(p_) \
		send_request_to_db(SVR_PROTO_GET_YUANBAO_BOX_FLAG, (p_), 0, 0, (p_)->id);

int set_spring_msg_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int set_spring_msg_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_spring_msg_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_spring_msg_callback(sprite_t* p, uint32_t id, char* buf, int len);
int db_get_yuanbao_box_flag_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_yuanbao_box_flag_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int set_yuanbao_box_flag_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_yuanbao_value_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int get_yuanbao_value_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_yuanbao_value_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int send_fu_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int list_fu_sent_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_fu_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int buy_item_use_yuanbao_cmd(sprite_t* p, const uint8_t* body, int bodylen);
int change_yuanbao_value_callback(sprite_t* p, uint32_t id, char* buf, int len);
int lahm_become_shuangjiegun_cmd (sprite_t *p, const uint8_t *body, int len);


#endif

