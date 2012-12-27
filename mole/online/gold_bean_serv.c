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
#include "central_online.h"
#include "message.h"
#include "gold_bean_serv.h"

typedef struct gold_bean_item_price{
	uint32_t product_id;
	uint16_t status_code;
	uint32_t base_price;
	uint32_t vip_price;
	uint32_t nonvip_price;
}__attribute__((packed))gold_bean_item_price_t;

#define gold_bean_inquire_multi_items_price(p_, buf, len)\
		send_request_to_goldbean_serv(GOLD_BEAN_PROTO_INQUIRE_MULTI_ITEMS_PRICE, p_, len, buf, p_->id)
#define gold_bean_inquire_one_item_price(p_, buf) \
		send_request_to_goldbean_serv(GOLD_BEAN_PROTO_INQUIRE_ONE_ITEM_PRICE, p_, 4, buf, p_->id)
#define gold_bean_buy_item_use_gold_bean(p_, buf, len) \
		send_request_to_goldbean_serv(GOLD_BEAN_PROTO_BUY_ITEM_USE_GOLD_BEAN, p_, len, buf, p_->id)

int goldbean_serv_fd;	//金豆消费fd
int getbean_serv_fd;	//查询金豆数量fd
int ticket_serv_fd;		//增加包月抵用券fd

int gold_bean_inquire_multi_items_price_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN_GE(len, 8);
	CHECK_VALID_ID(p->id);

	uint8_t buf[2046] = {0};
	int j = 0;
	uint32_t count, product_id;
	UNPKG_UINT32(body, count, j);
	if (count > 226) {
		return send_to_self_error(p, p->waitcmd, -ERR_gold_bean_inquire_max_count_exceeded, 1);
	}
	CHECK_BODY_LEN(len, 4 + count * 4);
	int i = 0;
	PKG_H_UINT32(buf, count, i);
	int loop;
	for (loop = 0; loop < count; loop++){
		UNPKG_UINT32(body, product_id, j);
		PKG_H_UINT32(buf, product_id, i);
		DEBUG_LOG("GOLD BEAN INQUIRE MULT ITEMS\t[uid=%u product_id=%u]", p->id, product_id);
	}

	DEBUG_LOG("GOLD BEAN INQUIRE MULT ITEMS\t[uid=%u count=%u]", p->id, count);
	return gold_bean_inquire_multi_items_price(p, buf, len);
}

static int gold_bean_inquire_multi_items_price_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
	if (ret_code != 0 ){
		DEBUG_LOG("INQUIRE MULTI ITEMS CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		switch ( ret_code ) {
			case 9:
				return send_to_self_error(p, p->waitcmd, -ERR_gold_bean_inquire_max_count_exceeded, 1);
			default:
			{
				return send_to_self_error(p, p->waitcmd, -ERR_gold_bean_inquire_max_count_exceeded, 1);
			}
		}
	}
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, 4 + sizeof(gold_bean_item_price_t) * count);

	gold_bean_item_price_t* item_price = (gold_bean_item_price_t*)(buf + 4);
	int j = sizeof(protocol_t);
	int k = j + 4;
	int loop, cnt = 0;
	for (loop = 0; loop < count; loop++, item_price++) {
		DEBUG_LOG("GOLD BEAN INQUIRE MULT ITEMS CALLBACK\t[uid=%u %u %u %u %u]", p->id, item_price->product_id, item_price->base_price, item_price->vip_price, item_price->nonvip_price);
		if (item_price->status_code != 0){
			ERROR_LOG("gold bean inquire product error\t[%u %u %u]", p->id, item_price->product_id, item_price->status_code);
			continue;
		}
		PKG_UINT32(msg, item_price->product_id, k);
		PKG_UINT32(msg, item_price->base_price, k);
		PKG_UINT32(msg, item_price->vip_price, k);
		PKG_UINT32(msg, item_price->nonvip_price, k);
		cnt++;
	}
	PKG_UINT32(msg, cnt, j);
	DEBUG_LOG("GOLD BEAN INQUIRE MULT ITEMS CALLBACK\t[uid=%u count=%u retbuf_len=%u]", p->id, count, len);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int gold_bean_inquire_one_item_price_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t product_id;
	UNPKG_UINT32(body, product_id, j);

	DEBUG_LOG("GOLD BEAN INQUIRE ITEM\t[uid=%u product_id=%u]", p->id, product_id);
	return gold_bean_inquire_one_item_price(p, &product_id);
}

static int gold_bean_inquire_one_item_price_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
	if (ret_code != 0 ){
		DEBUG_LOG("GOLD BEAN INQUIRE ITEM CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		switch  ( ret_code ){
			case    10:
				return send_to_self_error(p, p->waitcmd, -ERR_gold_bean_bad_product_id, 1);
			default:
			{
				return send_to_self_error(p, p->waitcmd, -ERR_gold_bean_bad_product_id, 1);
			}
		}
	}
	CHECK_BODY_LEN(len, 37);

	typedef struct item_price_info{
		uint32_t product_id;
		uint32_t price;
		uint32_t vip_price;
		uint32_t nonvip_price;
		uint8_t  type; //商品类型（0 单件 1 组合）
		uint8_t  category; //商品类型：小屋，装扮等
		uint32_t gift_money_2_num;
		uint8_t  must_vip;
		uint32_t max_limit;
		int32_t  total_count;
		int32_t  current_count;
		uint8_t  is_valid;
		uint8_t  flag;
	}__attribute__((packed))item_price_info_t;

	item_price_info_t* price_info = (item_price_info_t*)buf;
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, price_info->product_id, k);
	PKG_UINT32(msg, price_info->price, k);
	PKG_UINT32(msg, price_info->vip_price, k);
	PKG_UINT32(msg, price_info->nonvip_price, k);
	PKG_UINT8(msg, price_info->type, k);
	PKG_UINT8(msg, price_info->category, k);
	PKG_UINT32(msg, price_info->gift_money_2_num, k);
	PKG_UINT8(msg, price_info->must_vip, k);
	PKG_UINT32(msg, price_info->max_limit, k);
	PKG_UINT32(msg, price_info->total_count, k);
	PKG_UINT32(msg, price_info->current_count, k);
	PKG_UINT8(msg, price_info->is_valid, k);
	PKG_UINT8(msg, price_info->flag, k);

	DEBUG_LOG("GOLD BEAN INQUIRE ITEM CALLBACK\t[uid=%u product_id=%u %u %u %u]", p->id, price_info->product_id, price_info->price, price_info->vip_price, price_info->nonvip_price);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int buy_item_use_gold_bean_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 42);
	CHECK_VALID_ID(p->id);

	uint32_t dest_user_id;
	uint32_t product_id;
	uint16_t product_count;
	uint8_t  pay_passwd[33] = {0};
	
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

	//金豆购买商品不需要密码
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

	*(uint32_t*)p->session  = product_id;

	DEBUG_LOG("GOLD BEAN BUY ITEM\t[uid=%u uid=%u product_id=%u]", p->id, dest_user_id, product_id);
	return gold_bean_buy_item_use_gold_bean(p, buf, i);
}

static int buy_item_use_gold_bean_callback(sprite_t * p, uint32_t id, char * buf, int len, int ret_code)
{
	if (ret_code != 0) {
		DEBUG_LOG("GOLD BEAN BUY ITEM CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		return send_to_self_error(p, p->waitcmd, -(52000 + ret_code), 1);
	}

    uint32_t product_ids[] = {101151,101152,101153,101154,101155,
        101156,101157,101158,101778,101780,101782,101672,100982,
        100984,101784}; 
	uint32_t product_id = 0; 
	product_id = *(uint32_t*)p->session;

	uint32_t j = 0;
	for (j = 0; j < sizeof(product_ids); j++){
	    if (product_id == product_ids[j]){

            send_request_to_db(SVR_PROTO_USER_MIDDLE_YEAR_CHEAP_SEAL, NULL, 0, NULL, p->id);
	        break;
	    }
	}

	typedef struct buy_item_cbk{
		uint32_t op_id;
		uint32_t money_1_number;
		uint32_t money_1_balance;
		uint32_t money_2_number;
		uint32_t money_2_balance;
		uint32_t data_len;
	}__attribute__((packed))buy_item_cbk_t;

	CHECK_BODY_LEN_GE(len, sizeof(buy_item_cbk_t));
	buy_item_cbk_t* p_buy_item = (buy_item_cbk_t*)buf;
	CHECK_BODY_LEN(len, sizeof(buy_item_cbk_t) + p_buy_item->data_len);

	int k = sizeof(protocol_t);
	PKG_UINT32(msg, p_buy_item->money_1_number, k);
	PKG_UINT32(msg, p_buy_item->money_1_balance, k);
	PKG_UINT32(msg, p_buy_item->money_2_number, k);
	PKG_UINT32(msg, p_buy_item->money_2_balance, k);
	PKG_UINT32(msg, p_buy_item->data_len, k);
	PKG_STR(msg, (buf + sizeof(buy_item_cbk_t)), k, p_buy_item->data_len);

	DEBUG_LOG("GOLD BEAN BUY ITEM CALLBACK\t [%u %u %u %u %u]", p->id, p_buy_item->money_1_number, p_buy_item->money_1_balance, p_buy_item->money_2_number, p_buy_item->money_2_balance);

	give_gift_after_buy_item(p, p_buy_item->money_1_number / 100);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

/*
 * @brief 米币购买商品后送积分
 */
int give_gift_after_buy_item(sprite_t* p, uint32_t money_1_number)
{
	uint32_t db_buf[32] = {0};
	int l = 4;
	db_buf[1] = 1;	//送商城积分,必送

	db_buf[l++] = 0;		//积分类型
	db_buf[l++] = 1351056;	//ID
#ifndef TW_VER
	if ((get_now_tm()->tm_mon == 9 && get_now_tm()->tm_mday >= 1 && get_now_tm()->tm_mday < 8)
			|| (get_now_tm()->tm_mon == 8 && get_now_tm()->tm_mday == 30)){//20110930-20111007内获得3倍积分
#else
	if (get_now_tm()->tm_mon == 9 && get_now_tm()->tm_mday >= 8 && get_now_tm()->tm_mday < 16) {//台湾20111008-16内获得3倍积分
#endif
		db_buf[l++] = money_1_number * 3;
	} else {
		db_buf[l++] = money_1_number;
	}
	db_buf[l++] = 999999999;	//积分最大值

	DEBUG_LOG("give_gift_after_buy_item: uid[%u] id1[%u] cnt1[%u]", p->id, db_buf[5], db_buf[6]);
	return send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, l * sizeof(uint32_t), db_buf, p->id);
}


int get_gold_bean_info_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
	if (ret_code != 0) {
		DEBUG_LOG("GET GOLD BEAN INFO CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		return send_to_self_error(p, p->waitcmd, -(52000 + ret_code), 1);
	}
	CHECK_BODY_LEN(len, 4);
	int money_balance = *(uint32_t*)buf;;
	DEBUG_LOG("GET MONEY BALANCE INFO [%d %d]", p->id, money_balance);
	int k = sizeof(protocol_t);
	PKG_UINT32(msg, money_balance, k);
	PKG_UINT32(msg, 0, k);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

// function definitions
int send_request_to_goldbean_serv(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];

	server_proto_t *pkg = NULL;
	int len;

	if (goldbean_serv_fd == -1) {
		goldbean_serv_fd = connect_to_svr(config_get_strval("goldbean_serv_ip"), config_get_intval("goldbean_serv_port", 0), 65535, 1);
		DEBUG_LOG("RECONNECT TO GOLD BEAN SERVER [%d %d]", p->id, goldbean_serv_fd);
	}

	len = sizeof (server_proto_t) + body_len;
	if (goldbean_serv_fd == -1 || goldbean_serv_fd > epi.maxfd ||
			epi.fds[goldbean_serv_fd].cb.sndbufsz < epi.fds[goldbean_serv_fd].cb.sendlen + len ||
			body_len > sizeof(dbbuf) - sizeof(*pkg)) {
		ERROR_LOG ("send to dbproxy failed, buflen=%d, fd=%d",
					epi.fds[goldbean_serv_fd].cb.sendlen, goldbean_serv_fd);
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

	return net_send (goldbean_serv_fd, dbbuf, len, 0);
}

/**
  * @brief handle the gold bean server return message
  *
  * @param fd, the file discriptor which used to communicate with gold bean server.
  * @param goldbean_pkg,  message from gold bean server.
  * @param len, the length of message
  * @param sp,  used to store sprite pointer
  *
  * @return int, 0 on success, -1 on error.
  */
int handle_goldbean_serv_return(int fd, server_proto_t* goldbean_pkg, int len, sprite_t** sp)
{
	int waitcmd = goldbean_pkg->seq & 0xFFFF;
	int conn    = goldbean_pkg->seq >> 16;

	if ( !goldbean_pkg->seq )
		return 0;

	//to prevent from reclose the connection, it return 0
	if (!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd) {
		ERROR_RETURN(("gold bean connection has been closed, id=%u, cmd=0x%X %d, seq=%u",
						goldbean_pkg->id, goldbean_pkg->cmd, waitcmd, goldbean_pkg->seq), 0);
	}

	DEBUG_LOG("GOLD BEAN SERVER RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, goldbean_pkg->id, waitcmd, goldbean_pkg->cmd, goldbean_pkg->ret);

	int ret = -1;
#define PROC_GOLDBEAN_MESSAGE(n, func) \
	case n: ret = func(*sp, goldbean_pkg->id, goldbean_pkg->body, len - sizeof (server_proto_t), goldbean_pkg->ret); break;

	switch (goldbean_pkg->cmd) {
		PROC_GOLDBEAN_MESSAGE(GOLD_BEAN_PROTO_INQUIRE_MULTI_ITEMS_PRICE, gold_bean_inquire_multi_items_price_callback);
		PROC_GOLDBEAN_MESSAGE(GOLD_BEAN_PROTO_INQUIRE_ONE_ITEM_PRICE,gold_bean_inquire_one_item_price_callback);
		PROC_GOLDBEAN_MESSAGE(GOLD_BEAN_PROTO_BUY_ITEM_USE_GOLD_BEAN,buy_item_use_gold_bean_callback);

#undef PROC_GOLDBEAN_MESSAGE
		default:
			ERROR_LOG("gold bean unknow cmd=0x%x, id=%u, waitcmd=%d", goldbean_pkg->cmd, (*sp)->id, (*sp)->waitcmd);
			break;
	}

	return ret;
}

int get_gold_bean_count_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	uint8_t passwd[16] = {0};
	memset(passwd, 0, sizeof(passwd));
	DEBUG_LOG("GET GOLD BEAN COUNT");
	return send_request_to_getbean_serv(GET_BEAN_PROTO_GET_GOLD_BEAN_COUNT, p, 16, passwd, p->id);
}

static int get_gold_bean_count_callback(sprite_t * p,uint32_t id,char * buf,int len, int ret_code)
{
	int k = sizeof(protocol_t);
	if (ret_code == 102 || ret_code == 104) {
		ret_code = 0;
		PKG_UINT32(msg, 0, k);
	} else {
		if (ret_code != 0) {
			DEBUG_LOG("GET GOLD BEAN COUNT CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
			return send_to_self_error(p, p->waitcmd, -(52000 + ret_code), 1);
		}
		CHECK_BODY_LEN(len, 8);
		int gold_bean_balance = *(uint32_t*)buf;
		DEBUG_LOG("GET GOLD BEAN COUNT [%d %d]", p->id, gold_bean_balance);
		PKG_UINT32(msg, gold_bean_balance, k);
	}
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int send_request_to_getbean_serv(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];

	server_proto_t *pkg = NULL;
	int len;

	if (getbean_serv_fd == -1) {
		getbean_serv_fd = connect_to_svr(config_get_strval("getbean_serv_ip"), config_get_intval("getbean_serv_port", 0), 65535, 1);
		DEBUG_LOG("RECONNECT TO GET BEAN SERVER [%d %d]", p->id, getbean_serv_fd);
	}

	len = sizeof (server_proto_t) + body_len;
	if (getbean_serv_fd == -1 || getbean_serv_fd > epi.maxfd ||
			epi.fds[getbean_serv_fd].cb.sndbufsz < epi.fds[getbean_serv_fd].cb.sendlen + len ||
			body_len > sizeof(dbbuf) - sizeof(*pkg)) {
		ERROR_LOG ("send to dbproxy failed, buflen=%d, fd=%d",
					epi.fds[getbean_serv_fd].cb.sendlen, getbean_serv_fd);
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

	return net_send (getbean_serv_fd, dbbuf, len, 0);
}

/**
  * @brief handle the gold bean server return message
  *
  * @param fd, the file discriptor which used to communicate with gold bean server.
  * @param goldbean_pkg,  message from gold bean server.
  * @param len, the length of message
  * @param sp,  used to store sprite pointer
  *
  * @return int, 0 on success, -1 on error.
  */
int handle_getbean_serv_return(int fd, server_proto_t* getbean_pkg, int len, sprite_t** sp)
{
	int waitcmd = getbean_pkg->seq & 0xFFFF;
	int conn    = getbean_pkg->seq >> 16;

	if ( !getbean_pkg->seq )
		return 0;

	//to prevent from reclose the connection, it return 0
	if (!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd) {
		ERROR_RETURN(("gold bean connection has been closed, id=%u, cmd=0x%X %d, seq=%u",
						getbean_pkg->id, getbean_pkg->cmd, waitcmd, getbean_pkg->seq), 0);
	}

	DEBUG_LOG("GOLD BEAN SERVER RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, getbean_pkg->id, waitcmd, getbean_pkg->cmd, getbean_pkg->ret);

	int ret = -1;
#define PROC_GOLDBEAN_MESSAGE(n, func) \
	case n: ret = func(*sp, getbean_pkg->id, getbean_pkg->body, len - sizeof (server_proto_t), getbean_pkg->ret); break;

	switch (getbean_pkg->cmd) {
		PROC_GOLDBEAN_MESSAGE(GET_BEAN_PROTO_GET_GOLD_BEAN_COUNT, get_gold_bean_count_callback);

#undef PROC_GOLDBEAN_MESSAGE
		default:
			ERROR_LOG("gold bean unknow cmd=0x%x, id=%u, waitcmd=%d", getbean_pkg->cmd, (*sp)->id, (*sp)->waitcmd);
			break;
	}

	return ret;

}

/*
 * @brief 查询商城连续登录次数,及本次登录商城送的积分
 */
int get_bean_mall_login_times_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_GET_BEAN_MALL_LOGIN_TIMES, p, 0, NULL, p->id);
}

/*
 * @brief db返回商城连续登录次数，和送的积分，如果积分为0表示不是当天第一次登录
 */
int get_bean_mall_login_times_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t login_times = 0;//登录次数
	uint32_t get_count = 0;//送的积分
	int i = 0;
	UNPKG_H_UINT32(buf, login_times, i);
	UNPKG_H_UINT32(buf, get_count, i);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, login_times, l);
	PKG_UINT32(msg, get_count, l);
	uint32_t next_count = get_count;//前端要知道明天能得到多少个积分
	if (get_count == 5) {
		next_count = 8;
	} else if (get_count == 8) {
		next_count = 10;
	}
	PKG_UINT32(msg, next_count, l);

	uint32_t msgbuff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0408A347, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}


/*
 * @brief 包月抵用券服务器
 */
int send_request_to_ticket_serv(int cmd, sprite_t* p, int body_len, const void* body_buf, uint32_t id)
{
	static uint8_t dbbuf[PAGESIZE];

	server_proto_t *pkg = NULL;
	int len;

	if (ticket_serv_fd == -1) {
		ticket_serv_fd = connect_to_svr(config_get_strval("ticket_serv_ip"), config_get_intval("ticket_serv_port", 0), 65535, 1);
		DEBUG_LOG("RECONNECT TO VIP TICKET SERVER uid[%u] fd[%d]", id, ticket_serv_fd);
	}

	len = sizeof (server_proto_t) + body_len;
	if (ticket_serv_fd == -1 || ticket_serv_fd > epi.maxfd ||
			epi.fds[ticket_serv_fd].cb.sndbufsz < epi.fds[ticket_serv_fd].cb.sendlen + len ||
			body_len > sizeof(dbbuf) - sizeof(*pkg)) {
		ERROR_LOG ("send to dbproxy failed,uid=%u buflen=%d, fd=%d",
					id, epi.fds[ticket_serv_fd].cb.sendlen, ticket_serv_fd);
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

	return net_send(ticket_serv_fd, dbbuf, len, 0);
}

/**
  * @brief 包月抵用券返回
  *
  * @param fd, the file discriptor which used to communicate with gold bean server.
  * @param goldbean_pkg,  message from gold bean server.
  * @param len, the length of message
  * @param sp,  used to store sprite pointer
  *
  * @return int, 0 on success, -1 on error.
  */
int handle_ticket_serv_return(int fd, server_proto_t* ticket_pkg, int len, sprite_t** sp)
{
	int waitcmd = ticket_pkg->seq & 0xFFFF;
	int conn    = ticket_pkg->seq >> 16;

	if ( !ticket_pkg->seq )
		return 0;

	//to prevent from reclose the connection, it return 0
	if (!(*sp = get_sprite_by_fd(conn)) || (*sp)->waitcmd != waitcmd) {
		ERROR_RETURN(("gold bean connection has been closed, id=%u, cmd=0x%X %d, seq=%u",
						ticket_pkg->id, ticket_pkg->cmd, waitcmd, ticket_pkg->seq), 0);
	}

	DEBUG_LOG("GOLD BEAN SERVER RETURN\t[uid=%u %u cmd=%d 0x%X ret=%u]",
				(*sp)->id, ticket_pkg->id, waitcmd, ticket_pkg->cmd, ticket_pkg->ret);

	int ret = -1;
#define PROC_TICKET_MESSAGE(n, func) \
	case n: ret = func(*sp, ticket_pkg->id, ticket_pkg->body, len - sizeof (server_proto_t), ticket_pkg->ret); break;

	switch (ticket_pkg->cmd) {
		//PROC_TICKET_MESSAGE(GET_BEAN_PROTO_GET_GOLD_BEAN_COUNT, get_gold_bean_count_callback);
		PROC_TICKET_MESSAGE(TICKET_SERVER_PROTO_GET_TICKET_INFO, get_vip_month_ticket_info_callback);
		PROC_TICKET_MESSAGE(FREE_VIP_SERVER_PROTO_SET_INFO, set_free_vip_info_callback);
		PROC_TICKET_MESSAGE(FREE_VIP_SERVER_PROTO_GET_INFO, get_free_vip_info_callback);
		PROC_TICKET_MESSAGE(FREE_VIP_SERVER_PROTO_LEFT_TIME, get_lottery_draw_left_time_callback);

#undef PROC_TICKET_MESSAGE
		default:
			ERROR_LOG("gold bean unknow cmd=0x%x, id=%u, waitcmd=%d", ticket_pkg->cmd, (*sp)->id, (*sp)->waitcmd);
			break;
	}

	return ret;

}

/*
 * @brief 向包月抵用券服务器发送增加密闭抵用券的消息
 */
int add_vip_month_ticket(sprite_t* p, uint32_t ticket_type, uint32_t id)
{
	const uint16_t ticket_channel_id = 90;
	const char* ticket_security_code = "65398374";
	typedef struct {
		uint16_t channel_id;
		uint8_t verify_code[32];
		uint8_t ticket_type;
	}__attribute__((packed)) add_vip_month_ticket_package_t;
	add_vip_month_ticket_package_t package = {0};
	package.channel_id = ticket_channel_id;
	package.ticket_type = (uint8_t)ticket_type;
	
	uint8_t src[100] = {0};
	int len = snprintf((char*)src, sizeof src, "channelId=%d&securityCode=%s&data=", package.channel_id, ticket_security_code);
	src[len] = package.ticket_type;

	uint8_t md_out[16], md[33];
	MD5(src, len + 1, md_out);
	int i;
    for (i = 0; i != 16; ++i) {
        sprintf((char*)md + i * 2, "%.2x", md_out[i]);
    }
    memcpy(package.verify_code, md, 32);

	return send_request_to_ticket_serv(TICKET_SERVER_PROTO_ADD_TICKET_CNT, p, sizeof(package), &package, id);
}

int get_vip_month_ticket_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	return send_request_to_ticket_serv(TICKET_SERVER_PROTO_GET_TICKET_INFO, p, 0, NULL, p->id);
}

int get_vip_month_ticket_info_callback(sprite_t * p, uint32_t id, char * buf, int len, int ret_code)
{
	if (ret_code != 0) {
		DEBUG_LOG("GET MONTH TICKET INFO CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		return send_to_self_error(p, p->waitcmd, -(54000 + ret_code), 1);
	}

	typedef struct {
		uint16_t coupon1;
		uint16_t coupon2;
		uint16_t coupon3;
	}__attribute__((packed))month_ticket_info_cbk_pack_t;

	CHECK_BODY_LEN(len, sizeof(month_ticket_info_cbk_pack_t));
	month_ticket_info_cbk_pack_t* cbk_pack = (month_ticket_info_cbk_pack_t*)buf;;
	DEBUG_LOG("GET MONTH TICKET INFO [%d %d %d %d]", p->id, cbk_pack->coupon1, cbk_pack->coupon2, cbk_pack->coupon3);

	int k = sizeof(protocol_t);
	PKG_UINT32(msg, cbk_pack->coupon1, k);
	PKG_UINT32(msg, cbk_pack->coupon2, k);
	PKG_UINT32(msg, cbk_pack->coupon3, k);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

// free vip activity
int set_free_vip_info_cmd(sprite_t *p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);
	return  send_request_to_db(SVR_PROTO_CHECK_PRIOR_DICE, p, 0, NULL, p->id);

}

int check_prior_dice_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	int j = 0;
	uint32_t state = 0;
	UNPKG_H_UINT32(buf, state, j);


	const uint16_t channel_id = 90;
	const char* security_code = "65398374";
	typedef struct {
		uint16_t channel_id;
		uint8_t verify_code[32];
		uint8_t state;
	}__attribute__((packed)) free_super_lahm_t;
	free_super_lahm_t package = {0};
	package.channel_id = channel_id;
	package.state = state;
	uint8_t src[100] = {0};
	int lens = snprintf((char*)src, sizeof src, "channelId=%d&securityCode=%s&data=", package.channel_id, security_code);

	uint8_t md_out[16], md[33];
	MD5(src, lens, md_out);
	int i;
	for (i = 0; i != 16; ++i) {
		sprintf((char*)md + i * 2, "%.2x", md_out[i]);
	}
	memcpy(package.verify_code, md, 32);

	DEBUG_LOG("verify_code: %s", package.verify_code);
	return send_request_to_ticket_serv(FREE_VIP_SERVER_PROTO_SET_INFO, p, sizeof(free_super_lahm_t), &package, p->id);

}

int set_free_vip_info_callback(sprite_t *p, uint32_t id, char* buf, int len, int ret_code)
{

	if (ret_code != 0) {
		DEBUG_LOG("PRAY SUPER LAHM INFO CALLBACK\t[id=%u, cmd=0x%X, errnum=%d]", p->id, p->waitcmd, ret_code);
		return send_to_self_error(p, p->waitcmd, -(54000 + ret_code), 1);
	}

    uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409C34D, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	CHECK_BODY_LEN(len, sizeof(uint8_t));
	int k = sizeof(protocol_t);
	PKG_UINT8(msg, *(uint8_t*)buf, k);

	uint8_t type =  *(uint8_t*)buf;
	if(type == 4 || type == 5 || type == 6){
		
		uint32_t cnt = 0;
		if(type ==  4){
			cnt = 1;
		}
		else if(type == 5){
			cnt = 3;
		}
		else if(type == 6){
			cnt = 6;
		}
	//	char txt[256] = {0};
		
		//store month bigger than 1 month
		typedef struct{
			uint32_t type;
			char nick[USER_NICK_LEN];
			uint32_t month;
		}vip_month_t;
		vip_month_t vip_item;
		memset(&vip_item, 0, sizeof(vip_month_t));

		vip_item.type = 0;
		memcpy(vip_item.nick, p->nick, USER_NICK_LEN);
		vip_item.month = cnt;
		send_request_to_db(SVR_PROTO_SYSARG_ADD_GUESS_PRICE_INFO, NULL, sizeof(vip_month_t),
			&vip_item, id);

/*

#ifndef TW_VER
		int msg_len = sprintf(txt, "恭喜%s获得了%u个月超级拉姆！",p->nick, cnt);
#else
		int msg_len = sprintf(txt, "恭喜%s獲得了%u個月超級拉姆!", p->nick, cnt);
#endif
		tell_flash_some_msg_across_svr(NULL, CBMT_FREE_SUPER_LAHM, msg_len, txt);
*/
	}

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);

}
int get_free_vip_info_cmd(sprite_t *p, const uint8_t* body, int len)
{

	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_ticket_serv(FREE_VIP_SERVER_PROTO_GET_INFO, p, 0, NULL, p->id);
}

int get_free_vip_info_callback(sprite_t* p, uint32_t id, char* buf, int len, int ret_code)
{
	typedef struct free_vip{
		uint8_t days3;
		uint8_t days7;
		uint8_t days14;
		uint8_t month1;
		uint8_t month3;
		uint8_t month6;
	}vip_free_t;

	CHECK_BODY_LEN(len, sizeof(vip_free_t));

	vip_free_t *head = (vip_free_t*)buf;
	
	int k = sizeof(protocol_t);
	PKG_UINT8(msg, head->days3, k);
	PKG_UINT8(msg, head->days7, k);
	PKG_UINT8(msg, head->days14, k);
	PKG_UINT8(msg, head->month1, k);
	PKG_UINT8(msg, head->month3, k);
	PKG_UINT8(msg, head->month6, k);

	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int get_lottery_draw_left_time_cmd(sprite_t *p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_ticket_serv(FREE_VIP_SERVER_PROTO_LEFT_TIME , p, 0, NULL, p->id);
}
int get_lottery_draw_left_time_callback(sprite_t* p, uint32_t id, char* buf, int len, int ret_code)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t last_time = *(uint32_t*)buf;

	int k = sizeof(protocol_t);
	//uint32_t left_time = 0;
	//uint32_t now = time(NULL);

	//DEBUG_LOG("now: %u, last_time: %u", now, last_time);
	//if(180 > (now - last_time)){
	//	left_time = 180 - (now - last_time);
	//}

	PKG_UINT32(msg, last_time, k);
	init_proto_head(msg, p->waitcmd, k);
	return send_to_self(p, msg, k, 1);
}

int get_free_player_info_cmd(sprite_t *p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);
	return  send_request_to_db(SVR_PROTO_GET_FREE_VIP_PLAYER, p, 0, NULL, p->id);
}

int get_free_player_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t uid;
		uint32_t month;
		char nick[16];
	}free_player_t;

	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = 0;
	count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, sizeof(count)+sizeof(free_player_t)*count);

	DEBUG_LOG("count: %u", count);
	free_player_t *cur = (free_player_t*)(buf+sizeof(count));
	int l = sizeof(protocol_t);
	
	PKG_UINT32(msg, count, l);
	uint32_t k = 0;
	for(; k < count; ++k){
		PKG_UINT32(msg, cur->uid, l);
		PKG_UINT32(msg, cur->month, l);
		PKG_STR(msg, cur->nick, l, 16);
		++cur;
	}
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}
