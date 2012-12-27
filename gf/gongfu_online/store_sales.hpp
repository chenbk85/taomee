/**
 *============================================================
 *  @file		store_sale.hpp
 *  @brief		hpp for vip consumation
 * 
 *  compiler	gcc4.1.2
 *  platform	Linux
 *
 *  copyright:	TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef __GF_STORE_SALES_HPP__
#define __GF_STORE_SALES_HPP__


extern "C" {
#include <libtaomee/project/types.h>
}
#include "fwd_decl.hpp"



/*! file descriptor for connection with vipsvr */
extern int storesvr_fd;


//------------------------------------------------------------------
// enums
//------------------------------------------------------------------

/**
  * @brief command id for vipsvr
  */
enum store_cmd_t {
	/* for mb */
	store_proto_mb_trade		= 1004,
	store_proto_query_mb    	= 1005,
	store_proto_gcoin_trade		= 2004,
	store_proto_query_gcoin 	= 2005,
	store_proto_getback_outdated 	= 2006,
};

/**
  * @brief errno returned from vipsvr
  */
enum store_err_t {
};

/**
  * @brief some constants
  */
enum {
	/*! max acceptable length in bytes for each package from/to vip server */
	vipproto_max_len	= 4 * 1024,
};

/**
  * @brief change on your own risk
  */
enum {
	/*! 购买地点标识(flash 用 0) */
	buy_place_flash		= 0,
	/*! 支付密码长度 */
	max_pay_pass_len	= 32,
	/*! vipsvr 要求支付密码长度 */
	max_store_pay_pass_len	= 16,
};



//------------------------------------------------------------------
// structures
//------------------------------------------------------------------

#pragma pack(1)
/**
  * @brief storesvr protocol type
  */
struct store_proto_t {
	/*! package length */
	uint32_t	len;
	/*! sequence number ((p->fd << 16) | p->waitcmd) */
	uint32_t	seq;
	/*! command id */
	uint16_t	cmd;
	/*! errno */
	uint32_t	ret;
	/*! user id */
	userid_t	id;
	/*! package body */
	uint8_t		body[];
};

struct store_mb_trade_t {
	/*! 付出米币的用户 */
	userid_t	payer_id;
    /*! 付出米币的用户角色 */
	uint32_t	role_tm;
	/*! 要购买的商品ID(注意不是item_id, 3组根据这个转换成item_id) */
	uint32_t	product_id;
	/*! 要购买的商品数量 */
	uint16_t	product_count;
	/*! 用户是否是VIP用户: 1 是; 0 否 */
	uint8_t		is_vip;
	/*! 购买地点: 0 flash游戏; 1 普通网页 */
	uint8_t		buy_place;
	/*! 用户支付密码(MD5) */
	char		pay_pass[max_store_pay_pass_len];
    /*! 用户背包上限 */
    uint32_t    max_bag;
};

struct vipsvr_gcoin_trade_t {
	/*! 付出米币的用户 */
	userid_t	payer_id;
	/*! 要购买的商品ID(注意不是item_id, 3组根据这个转换成item_id) */
	uint32_t	product_id;
	/*! 要购买的商品数量 */
	uint16_t	product_count;
	/*! 用户是否是VIP用户: 1 是; 0 否 */
	uint8_t		is_vip;
	/*! 购买地点: 0 flash游戏; 1 普通网页 */
	uint8_t		buy_place;
};

struct coupons_elem_t {
    uint32_t    id;
    uint32_t    left_cnt;
};
struct vipsvr_mb_trade_rsp_t {
	/*! 本次交易的事务ID */
	uint32_t	trans_id;
	/*! 交易若成功所扣米币数（单位：米币数100倍）*/
	uint32_t	cost;
	/*! 米币余额 */
	uint32_t	left;
    /*! 本次交易增加的功夫通宝(单位: 通宝数100倍) */
    uint32_t    add_gcoin;
    /*! 用户当前总的功夫通宝数(单位: 通宝数100倍) */
    uint32_t    gcoin;
    uint32_t    datalen;
    uint8_t     data[];
};

struct vipsvr_gcoin_trade_rsp_t {
	/*! 本次交易的事务ID */
	uint32_t	trans_id;
	/*! 交易若成功所扣通宝数（单位：通宝数100倍）*/
	uint32_t	cost;
	/*! 通宝余额 */
	uint32_t	left;
    uint32_t    datalen;
    uint8_t    data[];
};

struct trade_attire_t{
    uint32_t    attireid;
    uint32_t    uniquekey;
    uint32_t    gettime;
    uint32_t    timelag;
};
struct trade_item_t{
    uint32_t    itemid;
    uint32_t    itemcnt;
};

struct vipsvr_query_mb_balance_rsp_t {
	/*! 米币余额 */
	uint32_t	balance;
};

struct is_set_pay_passwd_rsp_t {
	uint32_t is_setted;
};

struct vipsvr_query_gcoin_balance_rsp_t {
	/*! 金豆余额 */
	uint32_t	balance;
};

struct store_item_elem_t {
    uint32_t    id;
    uint32_t    uniquekey;
    uint32_t    duration;
    uint32_t    attirelv;
};
struct store_item_list_rsp_t {
    uint32_t    attire_cnt;
    store_item_elem_t attire[];
};
#pragma pack()



//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------
int send_request_to_vipsvr(player_t* p, userid_t id, uint16_t cmd, const void* body, uint32_t body_len);
void handle_storesvr_return(store_proto_t* pkg, uint32_t pkglen);

bool is_player_checked_pay_passwd(player_t* p);
//------------------------------------------------------------------
// request from client (XXX_cmd)
//------------------------------------------------------------------

/**
  * @brief check_paypass_set_flag_cmd
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int check_paypass_set_flag_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief 用米币买东西
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int mb_trade_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief 查询米币余额
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int query_mb_balance_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief 查询用户在商城中的物品
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int query_user_store_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief 用金豆买东西
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int gcoin_trade_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief 查询金豆余额
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int query_gcoin_balance_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief 续费过期物品
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int getback_outdated_attire_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief 校验用户支付密码
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int verify_pay_passwd_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int is_set_pay_passwd_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

//------------------------------------------------------------------
// response from DB (db_XXX_callback)
//------------------------------------------------------------------
/**
 * @brief query_gcoin_balance callback
 */
int db_query_gcoin_balance_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief query user_store_item callback
 */
int db_user_store_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

//------------------------------------------------------------------
// vipsvr_XXX functions
//------------------------------------------------------------------
int vipsvr_mb_trade(player_t* p, userid_t id, uint32_t product_id, uint32_t count, char* pay_pass, uint32_t pay_pass_len);

int vipsvr_gcoin_trade(player_t* p, userid_t id, uint32_t product_id, uint32_t count);

//------------------------------------------------------------------
// response from mbsvr (vipsvr_XXX_callback)
//------------------------------------------------------------------
/**
 * @brief query_gcoin_balance callback
 */
int db_query_gcoin_balance_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief handle vipsvr_mb_buy_item callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  */
int vipsvr_mb_trade_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief handle vipsvr_gcoin_buy_item callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  */
int vipsvr_gcoin_trade_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief handle vipsvr_query_mb_balance callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  */
int vipsvr_query_mb_balance_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief handle vipsvr_gcoin_trade callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  */
int vipsvr_getback_outdated_attire_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief handle vipsvr_query_gcoin_balance callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  */
int vipsvr_query_gcoin_balance_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief handle db_verify_pay_passwd callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  **/
int db_verify_pay_passwd_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_is_set_pay_passwd_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

#endif // __GF_STORE_SALES_HPP__
