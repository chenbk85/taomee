#ifndef CPROXYDEALBASE_HPP
#define CPROXYDEALBASE_HPP

#include "common.hpp"
#include "CProxyClientManage.hpp"

class CProxyDealBase {
public:
	virtual int get_friends_list_cmd(request_info_t* info, uint8_t* body, int bodylen) {return 0;}
	virtual int get_user_nick_cmd(request_info_t* info, uint8_t* body, int bodylen) {return 0;}
	virtual int get_user_info_cmd(request_info_t* info, uint8_t* body, int bodylen) {return 0;}
	virtual int get_user_cloth_info_cmd(request_info_t* info, uint8_t* body, int bodylen) {return 0;}
	virtual int send_mail_cmd(request_info_t* info, uint8_t* body, int bodylen) {return 0;}
	virtual int add_items_cmd(request_info_t* info, uint8_t* body, int bodylen) {return 0;}
	virtual int goods_add_items_cmd(request_info_t* info, uint8_t* body, int bodylen) {return 0;}
	virtual int goods_check_items_cmd(request_info_t* info, uint8_t* body, int bodylen) {return 0;}
	virtual int add_notify_cmd(request_info_t* info, uint8_t* body, int bodylen) {return 0;}
	
	virtual int customer_service_fixed_cmd(request_info_t* info, uint8_t* body, int bodylen) {return 0;}

	virtual int handle_db_return(void* data, int len);
	virtual int handle_switch_return(void* data, int len);

	//int send_request_to_svr(int svr_fd, uint8_t* pkg, int pkglen);
	int send_request_to_svr(request_info_t* info, uint8_t* pkg, int pkglen, uint16_t svr_type = 0);
	int send_header_to_client(request_info_t* info, int cmd, uint32_t err);

	int init_cli_header(void *pkg, uint32_t len, request_info_t* info, uint16_t cmd, uint32_t ret);

	//const char* req_verify_md5(const char* req_body, int req_body_len, uint16_t channel_id);
protected:
	int		m_pkg_len;
	uint8_t	pkgbuf[max_pkg_len];
	
	uint8_t	cli_pkgbuf[max_pkg_len];
	uint8_t	sw_pkgbuf[max_pkg_len];
};


enum item_type_t {
	t_attribute	= 0,
	t_item		= 1,
	t_clothes	= 2,
};

#pragma pack(1)

struct item_info_t {
	uint32_t	item_type;
	uint32_t	item_id;
	uint32_t	item_count;
};

struct send_mail_t {
	uint32_t	send_id;
	char		nick[NICK_LEN];
	uint32_t	send_time;
	uint32_t	mail_type;
	uint32_t	title_len;
	char		title[40];
	uint32_t	msg_len;
	char		msg[150];
	uint32_t	enclosure_cnt;
	item_info_t	enclosures[];
};

struct add_item_info_t {
	uint32_t	item_type;
	uint32_t	item_id;
	uint32_t	item_count;
	uint32_t	max_count;
};

struct add_items_t {
	uint32_t	item_cnt;
	add_item_info_t	items[];
};

struct add_notify_t {
	uint32_t	receive_id;
	uint32_t	npc_id;
	uint32_t	msg_time;
	uint32_t	msg_len;
	char		msgs[300];
};

struct get_friends_list_out {
	uint32_t friends_cnt;
};

struct goods_add_items_info_t {
	uint32_t item_id;
	uint32_t item_count;
	//uint32_t item_type;
	uint32_t validtype;
	//uint32_t duration;
};

struct goods_add_items_t {
	uint32_t max_limit;
	uint32_t add_data_1;
	char	 add_data_2[16];
	uint32_t item_type;
	uint32_t count;
	goods_add_items_info_t goods_items[];
};

struct goods_check_items_info_t {
	uint32_t item_id;
	uint32_t add_count;
	uint32_t max_limit;
};

struct goods_check_items_t {
	uint32_t add_data_1;
	char     add_data_2[16];
	uint32_t items_type;
	uint32_t count;
	goods_check_items_info_t check_items[];
};

#pragma pack()


#endif

