#ifndef CPROXYDEALSEER2_HPP
#define CPROXYDEALSEER2_HPP

#include "CProxyDealBase.hpp"


#pragma pack(1)
struct seer2_db_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint8_t  body[];
};

struct seer2_switch_proto_t {
	uint32_t len;
	uint32_t seq;
	uint16_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint8_t  body[];
};

#pragma pack()
enum seer2_db_cmds {
	seer2_dbproto_add_items			 = 0x2A04,
};

enum seer2_switch_cmds {
	seer2_swproto_add_notify	= 63010,
};

class CProxyDealSeer2 : public CProxyDealBase {
public:
	int add_items_cmd(request_info_t* info, uint8_t* body, int bodylen);
	int add_notify_cmd(request_info_t* info, uint8_t* body, int bodylen);

	int handle_db_return(void* data, int len);
	int handle_switch_return(void* data, int len);
private:
	int dbproto_add_items_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);
	//int swproto_add_notify_callback(request_info_t* info, uint8_t* body, int body_len, uint32_t ret);

private:
	int init_db_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len);
	int init_sw_proto_head(void* header, request_info_t* info, uint32_t seq, uint16_t cmd, uint32_t len);
};

#endif

