#ifndef CPROXYDEALSEER_HPP
#define CPROXYDEALSEER_HPP

#include "CProxyDealBase.hpp"


#pragma pack(1)
struct seer_db_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint8_t  body[];
};

struct seer_switch_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint8_t  body[];
};

struct seer_user_info_t {
	uint32_t	regtime;
	char		nick[16];
	uint32_t	vip;
	uint32_t	vip_rank;
	uint32_t	color;
	uint32_t	texturize;
	uint32_t	max_petlevel;
	uint32_t 	pet_count;
	uint32_t	spt_value;
	uint32_t	mon_king_wins;
	uint32_t	mess_fight_win_count;
	uint32_t	max_stages;
	uint32_t	arena_win_count;
	uint32_t	item_count;
};

struct seer_item_info_t {
	uint32_t	item_id;
	uint32_t	item_rank;
};

#pragma pack()
enum seer_db_cmds {
	seer_dbproto_get_friends_list 	 = 0x1836,
	seer_dbproto_get_user_nick		 = 0x181B,
	seer_dbproto_get_user_info		 = 0x18F8,
	seer_dbproto_get_user_cloth_info = 0x125A,
	seer_dbproto_send_mail			 = 0x1B01,
	//seer_dbproto_add_items			 = 0x1909,
	seer_dbproto_add_items			 = 0x13a8,
};

class CProxyDealSeer : public CProxyDealBase {
public:
	int get_friends_list_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int get_user_nick_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int get_user_info_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int get_user_cloth_info_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int send_mail_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int add_items_cmd(request_info_t* info, uint8_t* body, int bodylen);

	int handle_db_return(void* data, int len);
private:
	int dbproto_get_friends_list_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_get_user_nick_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_get_user_info_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_get_user_cloth_info_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_send_mail_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_add_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);

private:
	//int init_db_proto_head(void* header, uint32_t uid, uint32_t role_tm, uint32_t seq, uint16_t cmd, uint32_t len, int fd);
	int init_db_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len);
};

#endif

