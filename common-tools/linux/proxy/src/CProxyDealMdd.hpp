#ifndef CPROXYDEALMDD_HPP
#define CPROXYDEALMDD_HPP

#include "CProxyDealBase.hpp"


#pragma pack(1)
struct moledd_db_proto_t {
	uint32_t len;
	uint16_t cmd;
	uint32_t uid;
	uint32_t seq;
	uint32_t ret;
	uint8_t  body[];
};

#pragma pack()
enum moledd_db_cmds {
	moledd_dbproto_add_items			 = 2007,

	moledd_dbproto_goods_add_items       = 1002,
	moledd_dbproto_goods_check_items     = 1001,
};

class CProxyDealMdd : public CProxyDealBase {
public:
	int add_items_cmd(request_info_t* info, uint8_t* body, int bodylen);
	
	/*goods buy command*/
	int goods_add_items_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int goods_check_items_cmd(request_info_t* info, uint8_t* body, int bodylen);

	int handle_db_return(void* data, int len);
private:
	int dbproto_add_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	
	/*goods buy command*/
	int dbproto_goods_add_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	int dbproto_goods_check_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);

private:
	//int init_db_proto_head(void* header, uint32_t uid, uint32_t role_tm, uint32_t seq, uint16_t cmd, uint32_t len, int fd);
	int init_db_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len);
};

#endif

