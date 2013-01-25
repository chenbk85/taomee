#ifndef CPROXYDEALMOLE_HPP
#define CPROXYDEALMOLE_HPP

#include "CProxyDealBase.hpp"


#pragma pack(1)
struct mole_db_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint8_t  body[];
};

struct mole_switch_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint8_t  body[];
};

struct mole_user_info_t {
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

struct mole_item_info_t {
	uint32_t	item_id;
	uint32_t	item_rank;
};

#pragma pack()
enum mole_db_cmds {
	mole_dbproto_get_friends_list 	 = 0x1013,
	mole_dbproto_get_user_nick		 = 0x104B,
	mole_dbproto_get_user_info		 = 0x30F3,
	mole_dbproto_get_user_cloth_info = 0x125A,
	mole_dbproto_send_mail			 = 0xE101,
	mole_dbproto_add_items			 = 0x114D,
};

class CProxyDealMole : public CProxyDealBase {
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

