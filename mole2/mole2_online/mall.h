#ifndef HERO_MALL_H
#define HERO_MALL_H

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "svr_proto.h"

/**
  * @brief command id for dbproxy
  */
enum mall_cmd_t {
	proto_mall_get_items_price		= 1002,
	proto_mall_get_item_detail		= 1003,
	proto_mall_buy_item				= 1004,
	proto_mall_get_money_balance		= 1005
};

/**
  * @brief errno returned from dbproxy
  */
enum mall_err_t {
	INVALID_COMMAND_ID_ERROR					= 1,
	INVALID_PACKAGE_FORMAT_ERROR				= 2,
	INVALID_PARAMETER_VALUE_ERROR			= 3,
	SYSTEM_PROCESS_ERROR						= 4,
	QUERY_ITEMS_EXCEDD_LIMIT					= 9,
	PRODUCT_NOT_EXIST							= 10,
	BUY_USER_ID_NOT_EXIST_ERROR				= 11,
	BUY_PAY_PASSWD_ERROR						= 12,
	BUY_CURRENT_COUNT_NOT_ENOUGH				= 13,
	BUY_MUST_BE_VIP							= 14,
	BUY_REQUEST_COUNT_EXCEED_MAX_LIMIT		= 15,
	BUY_COUNT_EXCEED_MAX_LIMIT				= 16,
	BUY_COMBO_COUNT_EXCEED_MAX_LIMIT		= 17,
	BUY_MAP_PRODUCT_NOT_EXIST				= 18,
	BUY_HOME_ATTIRE_TYPE_EXCEED_MAX_LIMIT 	= 19,
	BUY_PACKAGE_OUT_OF_RANGE					= 20,
	BUY_VALUE_OUT_OF_RANGE					= 21,
	BUY_EXCEED_MAX_LIMIT						= 22,
	BUY_ATTIRE_NOT_EXIST_ERR					= 23,
	BUY_MBSYS_ACCOUNT_NOT_EXIST				= 102,
	BUY_MBSYS_ACCOUNT_NOT_ACTIVATED			= 104,
	BUY_MBSYS_NOT_ENOUGH_MB					= 105,
	BUY_MBSYS_INVALID_PRODUCT_COUNT			= 106,
	BUY_MBSYS_BEYOND_MONTH_CONSUME_LIMIT 	= 107,
	BUY_MBSYS_BEYOND_PER_CONSUME_LIMIT		= 108,
	BUY_TONGBAOSYS_ACCOUNT_NOT_EXIST		= 202,
	BUY_TONGBAOSYS_ACCOUNT_NOT_ACTIVATED	= 204,
	BUY_TONGBAOSYS_NOT_ENOUGH_MB				= 205,
	BUY_TONGBAOSYS_INVALID_PRODUCT_COUNT 		= 206,
	BUY_TONGBAOSYS_BEYOND_MONTH_CONSUME_LIMIT	= 207,
	BUY_TONGBAOSYS_BEYOND_PER_CONSUME_LIMIT		= 208
};

void init_products();
int load_products(xmlNodePtr cur);


int init_mall_proto_handles(int rstart);
void handle_mall_return(db_proto_t* cpkg, uint32_t pkglen);
int  send_request_to_mall(sprite_t* p, uint32_t cmd, const void* pkgbuf, uint32_t body_len);

#endif // HERO_MALL_H

