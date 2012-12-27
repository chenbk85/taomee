#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <libtaomee/conf_parser/config.h>
#include <openssl/md5.h>

#include "util.h"
#include "sprite.h"
#include "protoheaders.h"
#include "proto.h"
#include "dbproxy.h"

#include "mibiserver.h"
#include "gold_bean_serv.h"

typedef struct mibi_item_price{
	uint32_t product_id;
	uint16_t status_code;
	uint32_t base_price;
	uint32_t vip_price;
	uint32_t nonvip_price;
}__attribute__((packed))mibi_item_price_t;

typedef struct mibi_item_detail{
	uint32_t product_id;
	uint32_t base_price;
	uint32_t vip_price;
	uint32_t nonvip_price;
	uint8_t	 must_vip;
	uint32_t max_limit;
	uint32_t total_count;
	uint32_t cur_count;
	uint8_t is_valid;
	uint8_t	 flag;
}__attribute__((packed))mibi_item_detail_t;

#define db_is_set_pay_passwd(p_) \
		send_request_to_db(SVR_PROTO_IS_SET_PAY_PASSWD, p_, 0, NULL, p_->id)

#define mibi_inquire_one_item_price(p_, buf) \
		send_request_to_mibiserver(MIBI_PROTO_INQUIRE_ONE_ITEM_PRICE, p_, 4, buf, p_->id)

#define mibi_inquire_multi_items_price(p_, buf, len) \
		send_request_to_mibiserver(MIBI_PROTO_INQUIRE_MULTI_ITEMS_PRICE, p_, len, buf, p_->id)

#define mibi_inquire_one_item_detail(p_, buf) \
		send_request_to_mibiserver(MIBI_PROTO_INQUIRE_ONE_ITEM_DETAIL, p_, 4, buf, p_->id)

#define mibi_buy_item_use_mibi(p_, buf, len) \
		send_request_to_mibiserver(MIBI_PROTO_BUY_ITEM_USE_MIBI, p_, len, buf, p_->id)
#define mibi_buy_item_use_mibi_coupon(p_, buf, len) \
		send_request_to_mibiserver(MIBI_PROTO_BUY_ITEM_USE_MIBI_COUPON, p_, len, buf, p_->id)
#define get_mibi_coupon_info(p_) \
		send_request_to_mibiserver(MIBI_PROTO_GET_MIBI_COUPON_INFO, p_, 0, NULL, p_->id)

#define mibi_buy_gold_bean_use_mibi(p_, buf, len) \
		send_request_to_mibi_buy_bean_serv(MIBI_PROTO_BUY_GOLD_BEAN, p_, len, buf, p_->id)

int mibiserver_fd;
int couponserver_fd;
int mibi_buy_bean_fd;

int is_set_pay_passwd_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);

	return db_is_set_pay_passwd(p);
}

int is_set_pay_passwd_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t flag = *(uint32_t*)buf;
	if (flag != 0 && flag != 1)
		ERROR_RETURN(("bad flag\t[uid=%u flag=%u]", p->id, flag), -1);

	response_proto_uint32(p, p->waitcmd, flag, 0);
	return 0;
}

int inquire_one_item_price_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t product_id;
	UNPKG_UINT32(body, product_id, j);

	DEBUG_LOG("INQUIRE ITEM\t[uid=%u product_id=%u]", p->id, product_id);
	return mibi_inquire_one_item_price(p, &product_id);
}

static int inquire_one_item_price_callback(sprite_t * p, uint32_t id, char * buf, int len, int ret_code)
{
	if (ret_code != 0 ){
		DEBUG_LOG("INQUIRE ITEM CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		switch 	( ret_code ){
		    case    10:
				return send_to_self_error(p, p->waitcmd, -ERR_mibi_bad_product_id, 1);
		    default:
		        {
				    return send_to_self_error(p, p->waitcmd, -ERR_mibi_bad_product_id, 1);
				}
		}
	}

	CHECK_BODY_LEN(len, 16);

	uint32_t product_id, base_price, vip_price, nonvip_price;
	int j = 0;
	UNPKG_H_UINT32(buf, product_id, j);
	UNPKG_H_UINT32(buf, base_price, j);
	UNPKG_H_UINT32(buf, vip_price, j);
	UNPKG_H_UINT32(buf, nonvip_price, j);

	int k = sizeof(protocol_t);
	PKG_UINT32(msg, product_id, k);
	PKG_UINT32(msg, base_price, k);
	PKG_UINT32(msg, vip_price, k);
	PKG_UINT32(msg, nonvip_price, k);

	DEBUG_LOG("INQUIRE ITEM CALLBACK\t[uid=%u product_id=%u %u %u %u]", p->id, product_id, base_price, vip_price, nonvip_price);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);

}

int inquire_multi_items_price_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN_GE(len, 8);
	CHECK_VALID_ID(p->id);

	uint8_t buf[2048];
	int j = 0;
	uint32_t count, product_id;
	UNPKG_UINT32(body, count, j);
	CHECK_BODY_LEN(len, 4 + count * 4);

	int i = 0;
	PKG_H_UINT32(buf, count, i);
	int loop;
	for (loop = 0; loop < count; loop++){
		UNPKG_UINT32(body, product_id, j);
		PKG_H_UINT32(buf, product_id, i);
		DEBUG_LOG("INQUIRE MULT ITEMS\t[uid=%u product_id=%u]", p->id, product_id);
	}

	DEBUG_LOG("INQUIRE MULT ITEMS\t[uid=%u count=%u]", p->id, count);
	return mibi_inquire_multi_items_price(p, buf, len);
}

static int inquire_multi_items_price_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
	if (ret_code != 0 ){
		DEBUG_LOG("INQUIRE MULTI ITEMS CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);

		switch ( ret_code ) {
		    case 9:
			    return send_to_self_error(p, p->waitcmd, -ERR_mibi_inquire_max_count_exceeded, 1);
		    default:
		        {
			        return send_to_self_error(p, p->waitcmd, -ERR_mibi_inquire_max_count_exceeded, 1);
			    }
		}
	}

	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + sizeof(mibi_item_price_t) * count);

	mibi_item_price_t* item_price = (mibi_item_price_t*)(buf + 4);
	int j = sizeof(protocol_t);
	int k = j + 4;
	int loop, cnt = 0;
	for (loop = 0; loop < count; loop++, item_price++){
		//DEBUG_LOG("INQUIRE MULT ITEMS CALLBACK\t[uid=%u %u %u %u %u]", p->id, item_price->product_id, item_price->base_price, item_price->vip_price, item_price->nonvip_price);
		if (item_price->status_code != 0){
			//ERROR_LOG("inquire product error\t[%u %u %u]", p->id, item_price->product_id, item_price->status_code);
			continue;
		}
		PKG_UINT32(msg, item_price->product_id, k);
		PKG_UINT32(msg, item_price->base_price, k);
		PKG_UINT32(msg, item_price->vip_price, k);
		PKG_UINT32(msg, item_price->nonvip_price, k);
		cnt++;
	}

	PKG_UINT32(msg, cnt, j);

	//DEBUG_LOG("INQUIRE MULT ITEMS CALLBACK\t[uid=%u count=%u]", p->id, count);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);

}

int inquire_one_item_detail_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t product_id;
	UNPKG_UINT32(body, product_id, j);

	DEBUG_LOG("INQUIRE DETAIL\t[uid=%u product_id=%u]", p->id, product_id);
	return mibi_inquire_one_item_detail(p, &product_id);
}

static int inquire_one_item_detail_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
	if (ret_code != 0 ){
		DEBUG_LOG("INQUIRE DETAIL CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		switch 	( ret_code ){
		case	10:
				return send_to_self_error(p, p->waitcmd, -ERR_mibi_bad_product_id, 1);
		default:
				return send_to_self_error(p, p->waitcmd, -ERR_mibi_bad_product_id, 1);
		}
	}

	CHECK_BODY_LEN(len, sizeof(mibi_item_detail_t));

	mibi_item_detail_t* item_detai = (mibi_item_detail_t*)buf;

	int k = sizeof(protocol_t);
	PKG_UINT32(msg, item_detai->product_id, k);
	PKG_UINT32(msg, item_detai->base_price, k);
	PKG_UINT32(msg, item_detai->vip_price, k);
	PKG_UINT32(msg, item_detai->nonvip_price, k);
	PKG_UINT8(msg, item_detai->must_vip, k);
	PKG_UINT32(msg, item_detai->max_limit, k);
	PKG_UINT32(msg, item_detai->total_count, k);
	PKG_UINT32(msg, item_detai->cur_count, k);
	PKG_UINT8(msg, item_detai->is_valid, k);
	PKG_UINT8(msg, item_detai->flag, k);

	init_proto_head(msg, p->waitcmd, k);

	return send_to_self(p, msg, k, 1);

}

int buy_item_use_mibi_coupon_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 42);
	CHECK_VALID_ID(p->id);

	uint8_t buf[28];
	uint8_t md5_code[33];
	uint32_t uid, product_id;
	uint16_t count;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	UNPKG_UINT32(body, product_id, j);
	UNPKG_UINT16(body, count, j);

	int i = 0;
	PKG_H_UINT32(buf, uid, i);
	PKG_H_UINT32(buf, product_id, i);
	PKG_H_UINT16(buf, count, i);
	PKG_UINT8(buf, ISVIP(p->flag), i);
	PKG_UINT8(buf, 1, i);
	UNPKG_STR(body, md5_code, j, 32);
	md5_code[32] = '\0';

	int x;
	for (x = 0; x != 16; ++x) {
		uint32_t tmp;
		sscanf((void*)(md5_code + x * 2), "%2x", &tmp);
		*(buf + i + x) = tmp;
	}
	DEBUG_LOG("MIBI AND COUPON BUY ITEM\t[uid=%u uid=%u product_id=%u]", p->id, uid, product_id);
	return mibi_buy_item_use_mibi_coupon(p, buf, sizeof buf);
}

int get_mibi_coupon_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
    //return get_mibi_coupon_info(p);
	DEBUG_LOG("GET MIBI COUNT: uid[%u]", p->id);
	return send_request_to_mibi_buy_bean_serv(GOLD_BEAN_PROTO_GET_GOLD_BEAN_INFO, p, 0, NULL, p->id);
//	return send_request_to_goldbean_serv(GOLD_BEAN_PROTO_GET_GOLD_BEAN_INFO, p, 0, NULL, p->id);
}

int buy_item_use_mibi_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 42);
	CHECK_VALID_ID(p->id);

	uint8_t buf[28];
	uint8_t md5_code[33];
	uint32_t uid, product_id;
	uint16_t count;
	int j = 0;
	UNPKG_UINT32(body, uid, j);
	UNPKG_UINT32(body, product_id, j);
	UNPKG_UINT16(body, count, j);

	int i = 0;
	PKG_H_UINT32(buf, uid, i);
	PKG_H_UINT32(buf, product_id, i);
	PKG_H_UINT16(buf, count, i);
	PKG_UINT8(buf, ISVIP(p->flag), i);
	PKG_UINT8(buf, 1, i);
	UNPKG_STR(body, md5_code, j, 32);
	md5_code[32] = '\0';

	int x;
	for (x = 0; x != 16; ++x) {
		uint32_t tmp;
		sscanf((void*)(md5_code + x * 2), "%2x", &tmp);
		*(buf + i + x) = tmp;
	}
	DEBUG_LOG("MIBI BUY ITEM\t[uid=%u uid=%u product_id=%u]", p->id, uid, product_id);
	return mibi_buy_item_use_mibi(p, buf, sizeof buf);
}

static int buy_item_use_mibi_callback(sprite_t* p, uint32_t id, char * buf, int len, int ret_code)
{
	if (ret_code != 0) {
		DEBUG_LOG("BUY ITEM CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		return send_to_self_error(p, p->waitcmd, -(50000 + ret_code), 1);
	}

	CHECK_BODY_LEN(len, 12);

	uint32_t op_id, used_mibi, balance;
	int j = 0;
	UNPKG_H_UINT32(buf, op_id, j);
	UNPKG_H_UINT32(buf, used_mibi, j);
	UNPKG_H_UINT32(buf, balance, j);

	int k = sizeof(protocol_t);
	PKG_UINT32(msg, used_mibi, k);
	PKG_UINT32(msg, balance, k);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);

}

static int get_mibi_coupon_info_callback(sprite_t* p, uint32_t id, char * buf, int len, int ret_code)
{
	if (ret_code != 0) {
		DEBUG_LOG("GET MIBI COUPON INFO\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		return send_to_self_error(p, p->waitcmd, -(50000 + ret_code), 1);
	}
    CHECK_BODY_LEN(len, 8);
    int mibi_balance, coupon_balance;
    mibi_balance = *(uint32_t*)buf;
    coupon_balance = *(uint32_t*)(buf + 4);
    DEBUG_LOG("GET MIBI COUPON INFO [%d %d %d]", p->id, mibi_balance, coupon_balance);
    int k = sizeof(protocol_t);
	PKG_UINT32(msg, mibi_balance, k);
	PKG_UINT32(msg, coupon_balance, k);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

static int buy_item_use_mibi_coupon_callback(sprite_t* p, uint32_t id, char * buf, int len, int ret_code)
{
	if (ret_code != 0) {
		DEBUG_LOG("BUY ITEM USE MIBI COUPON CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		return send_to_self_error(p, p->waitcmd, -(50000 + ret_code), 1);
	}

	CHECK_BODY_LEN(len, 20);

	uint32_t op_id, used_mibi, mibi_balance, used_coupon, coupon_balance;
	int j = 0;
	UNPKG_H_UINT32(buf, op_id, j);
	UNPKG_H_UINT32(buf, used_mibi, j);
	UNPKG_H_UINT32(buf, mibi_balance, j);
	UNPKG_H_UINT32(buf, used_coupon, j);
	UNPKG_H_UINT32(buf, coupon_balance, j);

	int k = sizeof(protocol_t);
	PKG_UINT32(msg, used_mibi, k);
	PKG_UINT32(msg, mibi_balance, k);
	PKG_UINT32(msg, used_coupon, k);
	PKG_UINT32(msg, coupon_balance, k);
    DEBUG_LOG("BUY ITEM USE MIBI COUPON [%d %d %d %d %d %d]", p->id, op_id, used_mibi, mibi_balance, used_coupon, coupon_balance);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);

}

int buy_gold_bean_use_mibi_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 42);
	CHECK_VALID_ID(p->id);

	uint32_t dest_user_id;
    uint32_t product_id;
	uint16_t product_count;
	uint8_t  pay_passwd[33] = {0};
	//uint32_t add_data_1;
	//uint8_t  add_data_2[17] = {0};
	//uint32_t ext_data_len;

	uint8_t buf[1024] = {0};
	int j = 0;
	UNPKG_UINT32(body, dest_user_id, j);
	UNPKG_UINT32(body, product_id, j);
	UNPKG_UINT16(body, product_count, j);

    int i = 0;
	PKG_H_UINT32(buf, dest_user_id, i);
	PKG_H_UINT32(buf, product_id, i);
	PKG_H_UINT16(buf, product_count, i);
	PKG_UINT8(buf, ISVIP(p->flag), i);
	PKG_UINT8(buf, 0, i);

	UNPKG_STR(body, pay_passwd, j, 32);
	pay_passwd[32] = '\0';
	int x;
	for (x = 0; x != 16; ++x) {
		uint32_t tmp;
		sscanf((void*)(pay_passwd + x * 2), "%2x", &tmp);
		*(buf + i + x) = tmp;
	}
	i += 16;

	PKG_H_UINT32(buf, 0, i);
	for (x = 0; x != 16; ++x) {
		*(buf + i + x) = '\0';
	}
	i += 16;
	PKG_H_UINT32(buf, 0, i);

	DEBUG_LOG("MIBI BUY GOLD BEAN\t[uid=%u uid=%u product_id=%u]", p->id, dest_user_id, product_id);
	return mibi_buy_gold_bean_use_mibi(p, buf, i);
}

static int buy_gold_bean_use_mibi_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
	if (ret_code != 0) {
		DEBUG_LOG("GOLD BEAN BUY ITEM CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		return send_to_self_error(p, p->waitcmd, -(50000 + ret_code), 1);
	}

	typedef struct buy_gold_bean_cbk{
		uint32_t op_id;
		uint32_t money_1_number;
		uint32_t money_1_balance;
		uint32_t money_2_number;
		uint32_t money_2_balance;
		uint32_t data_len;
	}__attribute__((packed))buy_gold_bean_cbk_t;

	CHECK_BODY_LEN_GE(len, sizeof(buy_gold_bean_cbk_t));
	buy_gold_bean_cbk_t* p_buy_item = (buy_gold_bean_cbk_t*)buf;
	CHECK_BODY_LEN(len, 24 + p_buy_item->data_len);

	int k = sizeof(protocol_t);
	PKG_UINT32(msg, p_buy_item->money_1_number, k);
	PKG_UINT32(msg, p_buy_item->money_1_balance, k);
	PKG_UINT32(msg, p_buy_item->money_2_number, k);
	PKG_UINT32(msg, p_buy_item->money_2_balance, k);
	PKG_UINT32(msg, p_buy_item->data_len, k);
	PKG_STR(msg, (buf + sizeof(buy_gold_bean_cbk_t)), k, p_buy_item->data_len);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

// function definitions
int send_request_to_mibiserver(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	return 0;
	static uint8_t dbbuf[PAGESIZE];

	server_proto_t *pkg;
	int len;

	if (mibiserver_fd == -1) {
		mibiserver_fd = connect_to_svr(config_get_strval("mibiserver_ip"), config_get_intval("mibiserver_port", 0), 65535, 1);
	}

	len = sizeof (server_proto_t) + body_len;
	if (mibiserver_fd == -1 || mibiserver_fd > epi.maxfd ||
			epi.fds[mibiserver_fd].cb.sndbufsz < epi.fds[mibiserver_fd].cb.sendlen + len ||
			body_len > sizeof(dbbuf) - sizeof(*pkg)) {
		ERROR_LOG ("send to dbproxy failed, waitcmd=%d, cmd=%d, buflen=%d, fd=%d, uid=%u",
					p->waitcmd, cmd, epi.fds[mibiserver_fd].cb.sendlen, mibiserver_fd, id);
		if (p)
			return send_to_self_error(p, p->waitcmd, -ERR_system_error, 1);
		return -1;
	}

	pkg = (server_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	if (!p) pkg->seq = 0;
	else pkg->seq = (sprite_fd (p) << 16) | p->waitcmd;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);

	return net_send (mibiserver_fd, dbbuf, len, 0);
}

/**
  * @brief handle the mibiserver return message
  *
  * @param fd, the file discriptor which used to communicate with mibi server.
  * @param mibipkg,  message from mibi server.
  * @param len, the length of message
  * @param sp,  used to store sprite pointer
  *
  * @return int, 0 on success, -1 on error.
  */
int handle_mibiserver_return(int fd, server_proto_t* mibipkg, int len, sprite_t** sp)
{
	int waitcmd = mibipkg->seq & 0xFFFF;
	int conn    = mibipkg->seq >> 16;

	if ( !mibipkg->seq )
		return 0;

	//to prevent from reclose the connection, it return 0
	if (!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd) {
		ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%X %d, seq=%u",
						mibipkg->id, mibipkg->cmd, waitcmd, mibipkg->seq), 0);
	}

	DEBUG_LOG("MIBISERVER RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, mibipkg->id, waitcmd, mibipkg->cmd, mibipkg->ret);

	int ret = -1;
#define PROC_MIBI_MESSAGE(n, func) \
	case n: ret = func(*sp, mibipkg->id, mibipkg->body, len - sizeof (server_proto_t), mibipkg->ret); break

	switch (mibipkg->cmd) {
		PROC_MIBI_MESSAGE(MIBI_PROTO_INQUIRE_ONE_ITEM_PRICE, inquire_one_item_price_callback);
		PROC_MIBI_MESSAGE(MIBI_PROTO_INQUIRE_MULTI_ITEMS_PRICE, inquire_multi_items_price_callback);
		PROC_MIBI_MESSAGE(MIBI_PROTO_INQUIRE_ONE_ITEM_DETAIL, inquire_one_item_detail_callback);
		PROC_MIBI_MESSAGE(MIBI_PROTO_BUY_ITEM_USE_MIBI, buy_item_use_mibi_callback);
		PROC_MIBI_MESSAGE(MIBI_PROTO_BUY_ITEM_USE_MIBI_COUPON, buy_item_use_mibi_coupon_callback);
		PROC_MIBI_MESSAGE(MIBI_PROTO_GET_MIBI_COUPON_INFO, get_mibi_coupon_info_callback);

#undef PROC_MIBI_MESSAGE
		default:
			ERROR_LOG("unknow cmd=0x%x, id=%u, waitcmd=%d", mibipkg->cmd, (*sp)->id, (*sp)->waitcmd);
			break;
	}

	return ret;
}

int send_request_to_couponserver(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t couponbuf[PAGESIZE];

	server_proto_t *pkg;
	int len;

	if (couponserver_fd == -1) {
		couponserver_fd = connect_to_svr(config_get_strval("coupon_server_ip"), config_get_intval("coupon_server_port", 0), 65535, 1);
        DEBUG_LOG("RECONNECT TO COUPON SERVER [%d %d]", p->id, couponserver_fd);
	}

	len = sizeof (server_proto_t) + body_len;
	if (couponserver_fd == -1 || couponserver_fd > epi.maxfd ||
			epi.fds[couponserver_fd].cb.sndbufsz < epi.fds[couponserver_fd].cb.sendlen + len ||
			body_len > sizeof(couponbuf) - sizeof(*pkg)) {
		ERROR_LOG ("send to coupon server failed, buflen=%d, fd=%d",
					epi.fds[couponserver_fd].cb.sendlen, couponserver_fd);
		if (p)
			return send_to_self_error(p, p->waitcmd, -ERR_system_error, 1);

		return -1;
	}

	pkg = (server_proto_t *)couponbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	if (!p) pkg->seq = 0;
	else pkg->seq = (sprite_fd (p) << 16) | p->waitcmd;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);

	return net_send (couponserver_fd, couponbuf, len, 0);
}



int add_mibi_consume_coupon(sprite_t* p, int cnt)
{
    typedef struct _req_coupon {
        uint16_t channel_id;
        char verify_info[32];
        uint32_t count;
        int event_id;
    } __attribute__ ((packed)) req_coupon_t;
    req_coupon_t coupon_req;
    //test
    coupon_req.channel_id = 103;
    coupon_req.count = cnt;
    coupon_req.event_id = 0;
	unsigned char src[100];
	unsigned char md_out[16], md[33];
	int len = snprintf((char*)src, sizeof src, "channelId=%d&securityCode=%s&data=", 103, security_key);
    *(uint32_t*)(src + len) = cnt;
    *(uint32_t*)(src + len + 4) = 0;
    MD5(src, len + 8, md_out);
    int i;
    for (i = 0; i != 16; ++i) {
        sprintf((char*)md + i * 2, "%.2x", md_out[i]);
    }
    memcpy(coupon_req.verify_info, md, 32);
    DEBUG_LOG("ADD CONSUME COUPON [%d %d %d]", p->id, p->waitcmd, cnt);
    return send_request_to_couponserver(COUPON_PROTO_ADD, p, sizeof(req_coupon_t), &coupon_req, p->id);
}

static int add_consume_coupon_callback(sprite_t * p, uint32_t id, char * buf, int len, uint32_t ret_code)
{
    if(ret_code) {
        return send_to_self_error(p, p->waitcmd, -ERR_mibi_bad_product_id, 1);
    }
    DEBUG_LOG("ADD CONSUME COUPON CALLBACK [%u %u %u]", p->id, p->waitcmd, ret_code);
    switch(p->waitcmd) {
        case PROTO_REQ_TMP_SUPERLAMN:
            response_proto_uint32(p, p->waitcmd, 2, 0);
		case PROTO_EXCHG_MIBI_COUPON:
			response_proto_head(p, p->waitcmd, 0);
            break;
        default:
            ERROR_LOG("unknown coupon cmd [%d %d]", p->id, p->waitcmd);
    }
    return 0;
}

int handle_couponserver_return(int fd, server_proto_t* couponpkg, int len, sprite_t** sp)
{
	int waitcmd = couponpkg->seq & 0xFFFF;
	int conn    = couponpkg->seq >> 16;

	if ( !couponpkg->seq )
		return 0;
	//to prevent from reclose the connection, it return 0
	if (!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd) {
		ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%X %d, seq=%u",
						couponpkg->id, couponpkg->cmd, waitcmd, couponpkg->seq), 0);
	}

	DEBUG_LOG("COUPONSERVER RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, couponpkg->id, waitcmd, couponpkg->cmd, couponpkg->ret);

	int ret = -1;
#define PROC_COUPON_MESSAGE(n, func) \
	case n: ret = func(*sp, couponpkg->id, couponpkg->body, len - sizeof (server_proto_t), couponpkg->ret); break

	switch (couponpkg->cmd) {
		PROC_COUPON_MESSAGE(COUPON_PROTO_ADD, add_consume_coupon_callback);

#undef PROC_COUPON_MESSAGE
		default:
			ERROR_LOG("unknow cmd=0x%x, id=%u, waitcmd=%d", couponpkg->cmd, (*sp)->id, (*sp)->waitcmd);
			break;
	}

	return ret;
}

int send_request_to_mibi_buy_bean_serv(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t mibibuf[PAGESIZE] = {0};

	server_proto_t *pkg = NULL;
	int len;

	if (mibi_buy_bean_fd == -1) {
		mibi_buy_bean_fd = connect_to_svr(config_get_strval("mibi_buy_bean_ip"), config_get_intval("mibi_buy_bean_port", 0), 65535, 1);
        DEBUG_LOG("RECONNECT TO MIBI BUY GOLD BEAN SERVER [%d %d]", p->id, couponserver_fd);
	}

	len = sizeof (server_proto_t) + body_len;
	if (mibi_buy_bean_fd == -1 || mibi_buy_bean_fd > epi.maxfd ||
			epi.fds[mibi_buy_bean_fd].cb.sndbufsz < epi.fds[mibi_buy_bean_fd].cb.sendlen + len ||
			body_len > sizeof(mibibuf) - sizeof(*pkg)) {
		ERROR_LOG ("send to mibi buy gold bean server failed, buflen=%d, fd=%d",
					epi.fds[mibi_buy_bean_fd].cb.sendlen, mibi_buy_bean_fd);
		if (p)
			return send_to_self_error(p, p->waitcmd, -ERR_system_error, 1);

		return -1;
	}

	pkg = (server_proto_t *)mibibuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	if (!p) pkg->seq = 0;
	else pkg->seq = (sprite_fd (p) << 16) | p->waitcmd;
	pkg->ret = 0;
	memcpy (pkg->body, body_buf, body_len);

	return net_send (mibi_buy_bean_fd, mibibuf, len, 0);
}

int handle_mibi_buy_bean_return(int fd, server_proto_t* mibipkg, int len, sprite_t** sp)
{
	int waitcmd = mibipkg->seq & 0xFFFF;
	int conn    = mibipkg->seq >> 16;

	if ( !mibipkg->seq )
		return 0;
	//to prevent from reclose the connection, it return 0
	if (!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd) {
		ERROR_RETURN(("connection has been closed, id=%u, cmd=0x%X %d, seq=%u",
						mibipkg->id, mibipkg->cmd, waitcmd, mibipkg->seq), 0);
	}

	DEBUG_LOG("MIBI BUY GOLD BEAN SERVER RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, mibipkg->id, waitcmd, mibipkg->cmd, mibipkg->ret);

	int ret = -1;
#define PROC_MIBI_BUY_BEAN_MESSAGE(n, func) \
	case n: ret = func(*sp, mibipkg->id, mibipkg->body, len - sizeof (server_proto_t), mibipkg->ret); break

	switch (mibipkg->cmd) {
		PROC_MIBI_BUY_BEAN_MESSAGE(MIBI_PROTO_BUY_GOLD_BEAN, buy_gold_bean_use_mibi_callback);
		PROC_MIBI_BUY_BEAN_MESSAGE(GOLD_BEAN_PROTO_GET_GOLD_BEAN_INFO, get_gold_bean_info_callback);

#undef PROC_MIBI_BUY_BEAN_MESSAGE
		default:
			ERROR_LOG("mibi buy gold bean unknow cmd=0x%x, id=%u, waitcmd=%d", mibipkg->cmd, (*sp)->id, (*sp)->waitcmd);
			break;
	}

	return ret;
}


