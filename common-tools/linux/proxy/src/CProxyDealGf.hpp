#ifndef CPROXYDEALGF_HPP
#define CPROXYDEALGF_HPP

#include "CProxyDealBase.hpp"


#pragma pack(1)
struct gf_db_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint32_t role_tm;
	uint8_t  body[];
};

struct gf_switch_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint8_t  body[];
};

struct gf_db_send_mail_t {
	uint32_t	send_id;
	uint32_t	recv_id;
	uint32_t	mail_templet;
	uint32_t	max_mail_limit;
	char		mail_title[40 + 1];
	char		mail_body[300 + 1];
	char		mail_num_enclosure[1024];
	char		mail_item_enclosure[1024];
	char		mail_equip_enclosure[1024];
};
#pragma pack()

enum db_cmds {
	dbproto_get_friends_list = 0x0635,
	dbproto_get_user_info	 = 0x0670,
	dbproto_send_mail		 = 0x063A,

	swproto_noti_mail_sending	= 60007,
};

class CProxyDealGf : public CProxyDealBase {
public:
	/* dbproxy cmd */
	int get_friends_list_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int get_user_info_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int send_mail_cmd(request_info_t* info, uint8_t* body, int bodylen);
	
	/* switch cmd */
	int noti_mail_sending_cmd(request_info_t* info, uint8_t* body, int bodylen);

	/* customer cmd */
	int customer_service_fixed_cmd(request_info_t* info, uint8_t* body, int bodylen);

	int handle_db_return(void* data, int len);
private:
	int dbproto_get_friends_list_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_get_user_info_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_send_mail_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);

	int dbproto_customer_service_fixed_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);

private:
	//int init_db_proto_head(void* header, uint32_t uid, uint32_t role_tm, uint32_t seq, uint16_t cmd, uint32_t len, int fd);
	int init_db_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len);
	int init_sw_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len);
};

#endif

