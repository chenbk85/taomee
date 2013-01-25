#ifndef CPROXYDEALHERO_HPP
#define CPROXYDEALHERO_HPP

#include "CProxyDealBase.hpp"


#pragma pack(1)
struct hero_db_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint8_t  body[];
};

struct hero_switch_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint8_t  body[];
};

struct hero_db_send_mail_t {
	uint32_t	flag;
	uint32_t	type;
	uint32_t	themeid;
	uint32_t	send_id;
	char		nick[64];
	char		mail_title[64];
	char		mail_body[256];
	char		mail_item[64];
	char		mail_ids[32];
};
#pragma pack()

enum hero_db_cmds {
	hero_dbproto_get_friends_list = 0x0000,
	hero_dbproto_get_user_info	 = 0x0000,
	hero_dbproto_send_mail		 = 0x09B9,
	hero_dbproto_goods_add_items = 0x0858,
	hero_dbproto_goods_add_clothes = 0x0859,
	hero_dbproto_goods_check_items = 0x085C,
};

class CProxyDealHero : public CProxyDealBase {
public:
	/* dbproxy cmd */
	int get_friends_list_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int get_user_info_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int send_mail_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int goods_add_items_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int goods_check_items_cmd(request_info_t* info, uint8_t* body, int bodylen);

	/* customer cmd */
	int customer_service_fixed_cmd(request_info_t* info, uint8_t* body, int bodylen);

	int handle_db_return(void* data, int len);
private:
	int dbproto_get_friends_list_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_get_user_info_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_send_mail_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_goods_add_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_goods_check_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);

	int dbproto_customer_service_fixed_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);

private:
	//int init_db_proto_head(void* header, uint32_t uid, uint32_t role_tm, uint32_t seq, uint16_t cmd, uint32_t len, int fd);
	int init_db_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len);
};

#endif

