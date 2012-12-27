/**
 *============================================================
 *  @file		store_sales.cpp
 *  @brief		cpp for cosumation
 * 
 *  compiler	gcc4.1.2
 *  platform	Linux
 *
 *  copyright:	TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

/*
 * 协议:
 * 是否已经设置过支付密码 	0x008D
 */

#include <libtaomee++/inet/pdumanip.hpp>


using namespace taomee;

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/inet/tcp.h>
#include <async_serv/net_if.h>
#include <arpa/inet.h>
}

#include "utils.hpp"
#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "achievement.hpp"
//#include "map.hpp"
//#include "monster.hpp"
//#include "task.hpp"
//#include "home.hpp"
//#include "login.hpp"
#include "item.hpp"
#include "ambassador.hpp"
#include "store_sales.hpp"
#include "global_data.hpp"
#include "common_op.hpp"
#include "restriction.hpp"
#include "global_data.hpp"


//------------------------------------------------------------------
// global vars
//------------------------------------------------------------------
/*! for packing protocol data and send to mbsvr */
//uint8_t vippkgbuf[vipproto_max_len];

/*! vipproxy socket fd */
//int storesvr_fd = -1;

void hex_printf(char *buf,int len)
{    
    char tmpbuf[1024] = "";
    char *pcur = tmpbuf;
    if (len > (1024))
        return;
    for (int i=0;i<len;i++)
    {
        sprintf(pcur,"%02x ",buf[i]&0xff);                                                                           
        pcur += 3;                                                                                                   
    } 
    KDEBUG_LOG(0, "hex printf: %s \n",tmpbuf);                                                                           
}  

bool is_player_checked_pay_passwd(player_t* p)
{
	if (is_client_need_check_pay_passwd(p)) {
		if (g_server_config[2].value) {
			return p->pay_passwd_check_flg;
		}
	}
	return true;
}

//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------
int send_request_to_vipsvr(player_t* p, userid_t id, uint16_t cmd, const void* body, uint32_t body_len)
{
	static uint8_t vipbuf[vipproto_max_len];

	if (storesvr_fd == -1) {
		/* TODO: 支持 connect_to_service() */
		//vipsvr_id = connect_to_service(config_get_strval("service_vipsvr"), 0, 65535, 1);
		storesvr_fd = connect_to_svr(config_get_strval("storesvr_ip"), config_get_intval("storesvr_port", 0), 65535, 1);
	}

	if ((storesvr_fd == -1) || (body_len > (sizeof(vipbuf) - sizeof(store_proto_t)))) {
		ERROR_LOG("send to vipsvr failed: fd=%d len=%d", storesvr_fd, body_len);
		if (p) {
			if (p->waitcmd == cli_proto_login
					|| p->waitcmd == cli_proto_enter_own_home || p->waitcmd == cli_proto_enter_other_home ) {
				return -1;
			}
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	store_proto_t* pkg = reinterpret_cast<store_proto_t*>(vipbuf);
	pkg->len = sizeof(store_proto_t) + body_len;
	pkg->seq = (p ? ((p->fd << 16) | p->waitcmd) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	memcpy(pkg->body, body, body_len);
    KDEBUG_LOG(p->id, "SEND to STORE SERVER uid=[%u] pkglen=[%u]\t", id, pkg->len);
	return net_send(storesvr_fd, vipbuf, pkg->len);
}

void handle_storesvr_return(store_proto_t* pkg, uint32_t pkglen)
{
	uint32_t waitcmd = pkg->seq & 0xFFFF;
	int      connfd  = pkg->seq >> 16;

	player_t* p = get_player_by_fd(connfd);
	if (!p || (p->waitcmd != waitcmd)) {
		ERROR_LOG("connection has been closed: uid=%u cmd=%u %u fd=%d",
				pkg->id, pkg->cmd, waitcmd, connfd);
		return;
	}

#define PROC_STORE_PKG(proto, func) \
    case proto: func(p, pkg->body, pkglen - sizeof(store_proto_t), pkg->ret); break

    switch (pkg->cmd) {
	PROC_STORE_PKG(store_proto_mb_trade, vipsvr_mb_trade_callback);
	PROC_STORE_PKG(store_proto_query_mb, vipsvr_query_mb_balance_callback);
	PROC_STORE_PKG(store_proto_gcoin_trade, vipsvr_gcoin_trade_callback);
	PROC_STORE_PKG(store_proto_getback_outdated, vipsvr_getback_outdated_attire_callback);
	PROC_STORE_PKG(store_proto_query_gcoin, vipsvr_query_gcoin_balance_callback);

#undef PROC_STORE_PKG 
	default:
		ERROR_LOG("unsupported vipsvr cmd=%u id=%u waitcmd=%d",
				pkg->cmd, p->id, p->waitcmd);
		break;
    }
}



//------------------------------------------------------------------
// XXX_cmd functions
//------------------------------------------------------------------
/**
  * @brief 查询功夫通宝余额
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int query_gcoin_balance_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    KDEBUG_LOG(p->id, "STORE QUERY GF_COIN\t uid=[%u] cmd=[%u]",p->id, dbproto_query_gcoin_balance);
    //return send_request_to_vipsvr(p, p->id, store_proto_query_gcoin, 0, 0);
    return send_request_to_db(p, p->id, p->role_tm, dbproto_query_gcoin_balance, 0, 0);
}

/**
  * @brief 查询米币余额
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int query_mb_balance_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
#ifndef VERSION_KAIXIN
    KDEBUG_LOG(p->id, "query_mb_balance_cmd uid=[%u] cmd=[%u]",p->id, store_proto_query_mb);
    return send_request_to_vipsvr(p, p->id, store_proto_query_mb, 0, 0);
#else
		p->waitcmd = 0;
		return 0;
#endif

}

/**
 * @brief vipsvr_mb_trade 
 */
int vipsvr_mb_trade(player_t* p, userid_t id, uint32_t product_id, uint32_t count, char* pay_pass, uint32_t pay_pass_len)
{
	assert(pay_pass_len == max_pay_pass_len);
	store_mb_trade_t* pkg = reinterpret_cast<store_mb_trade_t*>(vippkgbuf);
	pkg->payer_id = id;
	pkg->role_tm = p->role_tm;
	pkg->product_id = product_id;
	pkg->product_count = count;
	if (p) {
		pkg->is_vip = is_vip_player(p) ? 1 : 0;
	} else {
		/* 不允许不在线 (容错: 不断线,但没效果) */
		WARN_LOG("BUG! offline player mb_trade, uid=%u", id);
		return 0;
	}
	pkg->buy_place = buy_place_flash;
	str2hex(pay_pass, pay_pass_len, pkg->pay_pass);
    pkg->max_bag = get_player_total_item_bag_grid_count(p);
	
	return send_request_to_vipsvr(p, id, store_proto_mb_trade, pkg, sizeof(*pkg));
}

/**
  * @brief 用米币买东西(功夫通宝)
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int mb_trade_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    /*
    if (!is_vip_player(p)) {
        return send_header_to_player(p, p->waitcmd, cli_err_vip_only, 1);
    }*/
	uint32_t product_id = 0;
	uint32_t product_count = 0;
	char pay_pass[max_pay_pass_len];
    CHECK_VAL_EQ(bodylen, 40);

	int idx = 0;
	unpack(body, product_id, idx);
	unpack(body, product_count, idx);
	unpack(body, pay_pass, sizeof(pay_pass), idx);
    KDEBUG_LOG(p->id, "MB TRADE:\t UID=[%u] [%u %u]",p->id, product_id, product_count);

	return vipsvr_mb_trade(p, p->id, product_id, product_count, pay_pass, sizeof(pay_pass));
}

/**
  * @brief 查询用户在商城中的物品
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int query_user_store_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    return send_request_to_db(p, p->id, p->role_tm, dbproto_user_store_item, 0, 0);
}

/**
  * @brief 用金豆买东西
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int gcoin_trade_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0, idx2 = 0;
    uint32_t product_id = 0;
    uint32_t product_count = 0;
	uint32_t count = 0, id = 0, duration = 0, attire_cnt = 0;

	if (if_version_kaixin() && is_btl_time_limited(p)) {
		KDEBUG_LOG(p->id, "GCOIN TRADE TM LIMIT\t[%u %u %u]", p->id, p->oltoday, battle_time_limit);
		//return send_header_to_player(p, p->waitcmd, cli_err_limit_time_btl, 1);
	}
	if (!is_player_checked_pay_passwd(p)) {
		ERROR_LOG("%u havnot check pay passwd", p->id);
		return -1;
	}

    unpack(body, product_id, idx);
    unpack(body, product_count, idx);
    unpack(body, count, idx);
    KDEBUG_LOG(p->id, " CMD=2816\t UID=[%u] PID PCNT CNT [%u %u %u]",p->id, product_id, product_count, count);
    if ( count < 1 ) {
        return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
    }

	p->sesslen = 0;
	pack_h(p->session, product_id, p->sesslen);
	if (product_id == 320107) {
		if (test_bit_on(p->once_bit, flag_pos_shop_tmp_buy)) {
			TRACE_LOG("already buy it %u %u", p->id, product_id);
			return send_header_to_player(p, p->waitcmd, 300022, 1);
		}
	}
	if (product_id == 320551) {
		if (test_bit_on(p->once_bit, flag_pos_shop_limit_buy)) {
			TRACE_LOG("already buy it %u %u", p->id, product_id);
			return send_header_to_player(p, p->waitcmd, 300022, 1);
		}
	}
	
    CHECK_VAL_EQ(bodylen, count * 4 + 12);
    unpack(body, id, idx);
    const GfItem* itm = items->get_item(id);
    if (!itm) {
        ERROR_LOG("Item cannot usage !!! uid=[%u] id=[%u]", p->id, id);
        return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
    }
    //judge package have space
    uint32_t max_bag = get_player_total_item_bag_grid_count(p);
    if ( items->is_clothes(itm->category()) ) {
        if ( (product_count * count + p->my_packs->all_clothes_cnt()) > max_bag ){
            //TRACE_LOG("attire bag no space,uid=[%u] [%u %u %u]",p->id, product_count * count, 
                //p->my_packs->all_clothes_cnt(), max_bag);
            return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
        }
        duration = itm->duration * clothes_duration_ratio;
        attire_cnt = count;
    } else {
        uint32_t item_id = 0, item_type = 0;
        if ( !is_strengthen_material(id) ) {
            item_type++;
        }
        for (uint32_t i = 1; i < product_count; i++) {
            unpack(body, item_id, idx);
            if ( !is_strengthen_material(item_id) ) {
                item_type++;
            }
        }

        if ( ((product_count * item_type + p->my_packs->all_items_cnt()) > max_bag) && item_type != 0 ) {
            //TRACE_LOG("item bag no space,uid=[%u] [%u %u %u]",p->id, product_count * count,
                //p->my_packs->all_items_cnt(), max_bag);
            return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
        }
    }
    pack_h(vippkgbuf, p->role_tm, idx2);
    pack_h(vippkgbuf, p->id, idx2);
    pack_h(vippkgbuf, product_id, idx2);
    pack_h(vippkgbuf, static_cast<uint16_t>(product_count), idx2);
    uint8_t is_vip = is_vip_player(p) ? 1 : 0;
    pack_h(vippkgbuf, is_vip, idx2);
    pack_h(vippkgbuf, static_cast<uint8_t>(buy_place_flash), idx2);
    pack_h(vippkgbuf, max_bag, idx2);
    pack_h(vippkgbuf, attire_cnt, idx2);
    
    if (attire_cnt > 0) {
        pack_h(vippkgbuf, id, idx2);
        pack_h(vippkgbuf, duration, idx2);

        for (uint32_t i = 1; i < attire_cnt; i++) {
            unpack(body, id, idx);
            itm = items->get_item(id);
            if ( !itm || !items->is_clothes(itm->category()) ) {
                ERROR_LOG("Attire have mistake!!! uid=[%u] id=[%u]",p->id, id);
                return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
            }
            duration = itm->duration * clothes_duration_ratio;
            pack_h(vippkgbuf, id, idx2);
            TRACE_LOG("gfcoin trade: uid=[%u] itemid=[%u]", p->id, id);
            pack_h(vippkgbuf, duration, idx2);
        }
    }

    return send_request_to_vipsvr(p, p->id, store_proto_gcoin_trade, vippkgbuf, idx2);
}
/**
  * @brief 续费过期物品
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int getback_outdated_attire_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0, idx2 = 0;
    uint32_t product_id = 0, product_cnt = 1;
    uint32_t attireid = 0, uniquekey = 0;
    /*
    unpack(body, product_cnt, idx);
    //Now, product_cnt must be one
    if (product_cnt != 1) {
        ERROR_LOG(": parameter error. uid=[%u] product_cnt=[%u]", p->id, product_cnt);
        return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
    }
    */
    CHECK_VAL_EQ(bodylen, 12);
    uint32_t max_bag = get_player_total_item_bag_grid_count(p);
    if ((product_cnt + p->my_packs->all_clothes_cnt()) > max_bag) {
        TRACE_LOG("attire bag no space,uid=[%u] [%u %u %u]",p->id, product_cnt, 
            p->my_packs->all_clothes_cnt(), max_bag);
        return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
    }
    pack_h(vippkgbuf, p->role_tm, idx2);
    pack_h(vippkgbuf, p->id, idx2);
    for (uint32_t i = 0; i < product_cnt; i++) {
        unpack(body, product_id, idx);
        unpack(body, attireid, idx);
        unpack(body, uniquekey, idx);
        pack_h(vippkgbuf, product_id, idx2);
        pack_h(vippkgbuf, uniquekey, idx2);
        //TRACE_LOG("uid=[%u] pid id key [%u %u %u]",p->id, product_id, attireid, uniquekey);
    }
    uint8_t is_vip = is_vip_player(p) ? 1 : 0;
    pack_h(vippkgbuf, is_vip, idx2);
    pack_h(vippkgbuf, static_cast<uint8_t>(buy_place_flash), idx2);
    pack_h(vippkgbuf, max_bag, idx2);

    return send_request_to_vipsvr(p, p->id, store_proto_getback_outdated, vippkgbuf, idx2);
}
/**
  * @brief 校验用户支付密码
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int verify_pay_passwd_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	char pay_pass[max_pay_pass_len] = "";

	if (p->pay_passwd_err_tm) {
		if (get_now_tv()->tv_sec > p->pay_passwd_err_tm) {
			if (get_now_tv()->tv_sec - p->pay_passwd_err_tm < 30) {
				return send_header_to_player(p, p->waitcmd, cli_err_pay_passwd_err_too_many, 1);
			}
		}
		p->pay_passwd_err_tm = 0;
		p->pay_passwd_err_cnt = 0;
	}

	int idx = 0;
	unpack(body, pay_pass, sizeof(pay_pass), idx);
	TRACE_LOG("%s", pay_pass);
	str2hex(pay_pass, max_pay_pass_len, dbpkgbuf);
	return send_request_to_db_no_roletm(p, p->id, dbproto_verify_pay_passwd, dbpkgbuf, 16);;
} 

int is_set_pay_passwd_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return send_request_to_db_no_roletm(p, p->id, dbproto_is_set_pay_passwd, 0, 0);;
}
//------------------------------------------------------------------
// response from DB (db_XXX_callback)
//------------------------------------------------------------------
/**
 * @brief query_gcoin_balance callback
 */
int db_query_gcoin_balance_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    uint32_t gfcoin = *reinterpret_cast<uint32_t*>(body);
    KDEBUG_LOG(id, "GET TONGBAO\t[coin=%u] ", gfcoin);

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, gfcoin, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}
/**
 * @brief user outdated store attire
 */
int db_user_store_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);

    store_item_list_rsp_t* rsp = reinterpret_cast<store_item_list_rsp_t*>(body);
    
    CHECK_VAL_EQ(bodylen, sizeof(store_item_list_rsp_t) + rsp->attire_cnt * sizeof(store_item_elem_t));

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->attire_cnt, idx);
    for (uint32_t i = 0; i < rsp->attire_cnt; i++) {
        store_item_elem_t* elem = &(rsp->attire[i]);
        pack(pkgbuf, elem->id, idx);
        pack(pkgbuf, elem->uniquekey, idx);
        //pack(pkgbuf, elem->duration, idx);
        pack(pkgbuf, elem->attirelv, idx);
        TRACE_LOG("uid=[%u],id=[%d],unique=[%d], duration=[%u],attirelv=[%u]",
            p->id, elem->id, elem->uniquekey, elem->duration, elem->attirelv);
    }
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}


/**
  * @brief callback for db_check_paypass_set_flag
  * @param p the requester
  * @param uid id of the requestee
  * @param body body of the returning package
  * @param length of body
  * @param ret errno returned from dbproxy
  * @return 0 on success, -1 and p will be deleted
  */
int db_check_paypass_set_flag_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	uint32_t db_set_flag = *reinterpret_cast<uint32_t *>(body);
	if (db_set_flag != 0 && db_set_flag != 1) {
		WARN_LOG("BUG! DB ret invalid paypass_set_flag=%u, uid=%u",
				db_set_flag, p->id);
		/* 交给客户端容错: 尽量不断线 */
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, db_set_flag, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief handle db_verify_pay_passwd callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  **/
int db_verify_pay_passwd_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	if (ret != 1160 && ret != 1105) {
		CHECK_DBERR(p, ret);
	}
	if (!ret) {
		p->pay_passwd_check_flg = true;
	}
	if (ret) {
	//passwd err
		p->pay_passwd_err_cnt++;
		if (p->pay_passwd_err_cnt > 5) {
			p->pay_passwd_err_tm = get_now_tv()->tv_sec;
		}
		ret = 1;
	} else {
	//succ
		p->pay_passwd_err_cnt = 0;
		p->pay_passwd_err_tm = 0;
	}
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, ret, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	KDEBUG_LOG(p->id, "verify_pay_passwd errno:%u", ret);
	return send_to_player(p, pkgbuf, idx, 1);
}

int db_is_set_pay_passwd_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	
	CHECK_DBERR(p, ret);
	is_set_pay_passwd_rsp_t* rsp = reinterpret_cast<is_set_pay_passwd_rsp_t*>((uint8_t*)body - 4);
	
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->is_setted, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	KDEBUG_LOG(p->id, "is_set_pay_passwd:%u", rsp->is_setted);
	return send_to_player(p, pkgbuf, idx, 1);
}

//------------------------------------------------------------------
// vipsvr_XXX functions
//------------------------------------------------------------------
//------------------------------------------------------------------
// response from mbsvr (vipsvr_XXX_callback)
//------------------------------------------------------------------
/**
  * @brief handle vipsvr_query_mb_balance callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  */
int vipsvr_query_mb_balance_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_VIP_SVR_ERR(p, ret);
	CHECK_VAL_EQ(bodylen, sizeof(vipsvr_query_mb_balance_rsp_t));

	vipsvr_query_mb_balance_rsp_t* rsp = reinterpret_cast<vipsvr_query_mb_balance_rsp_t*>(body);
    KDEBUG_LOG(p->id, "MB BALANCE \t UID=[%u] BALANCE=[%u]",p->id,rsp->balance);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->balance, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief handle vipsvr_mb_buy_item callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  */
int vipsvr_mb_trade_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_VIP_SVR_ERR(p, ret);
	CHECK_VAL_GE(bodylen, sizeof(vipsvr_mb_trade_rsp_t));

	vipsvr_mb_trade_rsp_t* rsp = reinterpret_cast<vipsvr_mb_trade_rsp_t*>(body);
    KDEBUG_LOG(p->id, "GF GONGFU\t UID=[%u] [%u %u %u %u %u] [%u]",p->id,rsp->trans_id,rsp->cost,rsp->left,rsp->add_gcoin,rsp->gcoin, rsp->datalen);
    /*
    int idx = 0;
    pack_h(dbpkgbuf, rsp->gcoin, idx);
    send_request_to_db(p, p->id, p->role_tm, dbproto_synchro_db_gfcoin, dbpkgbuf, idx);
*/
	int idx = sizeof(cli_proto_t);
	//pack(pkgbuf, rsp->trans_id, idx);
	//pack(pkgbuf, rsp->cost, idx);
	//pack(pkgbuf, rsp->left, idx);
	//pack(pkgbuf, rsp->add_gcion, idx);
    if (rsp->datalen != 0) {
        coupons_elem_t *coupons = reinterpret_cast<coupons_elem_t *>(rsp->data + 8);
        KDEBUG_LOG(p->id, "GF COUPONS\t UID=[%u] [%u %u]", p->id, coupons->id, coupons->left_cnt);
        uint32_t add_coupons_cnt = coupons->left_cnt - p->my_packs->get_item_cnt(coupons->id);
        p->my_packs->add_item(p, gold_coin_item_id, add_coupons_cnt, channel_string_use_tongbao_item, false, store_trade);
        pack(pkgbuf, static_cast<uint32_t>(2), idx);
        pack(pkgbuf, coupons->left_cnt, idx);
    } else {
        KDEBUG_LOG(p->id, "GF TONGBAO\t UID=[%u] [%u]", p->id, rsp->gcoin);
        pack(pkgbuf, static_cast<uint32_t>(1), idx);
        pack(pkgbuf, rsp->gcoin, idx);
    }
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief handle vipsvr_gcoin_trade callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  */
int vipsvr_gcoin_trade_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_VIP_SVR_ERR(p, ret);
	CHECK_VAL_GE(bodylen, sizeof(vipsvr_gcoin_trade_rsp_t));

	vipsvr_gcoin_trade_rsp_t* rsp = reinterpret_cast<vipsvr_gcoin_trade_rsp_t*>(body);
    KDEBUG_LOG(p->id, "GF TONGBAO\t UID=[%u] [%u %u %u %u]",p->id,rsp->trans_id,rsp->cost,rsp->left,rsp->datalen);
    CHECK_VAL_EQ(bodylen, sizeof(vipsvr_gcoin_trade_rsp_t) + rsp->datalen);

	//300022
	int pro_idx = 0;
	uint32_t product_id = 0;
	unpack_h(p->session, product_id, pro_idx);
	if (product_id == 320107) {
		set_once_bit_on(p, flag_pos_shop_tmp_buy);
		db_set_user_flag(p, 0);
	}
	if (product_id == 320551) {
		set_once_bit_on(p, flag_pos_shop_limit_buy);
		db_set_user_flag(p, 0);
	}

	int idx = sizeof(cli_proto_t);
    //pack(pkgbuf, rsp->trans_id, idx);
    //pack(pkgbuf, rsp->cost, idx);
    pack(pkgbuf, rsp->left, idx);

    //hex_printf((char *)(rsp->data), 16);
    int u_idx = 0;
    uint32_t attire_cnt = 0, item_cnt = 0;
    unpack_h(rsp->data, attire_cnt, u_idx);
    unpack_h(rsp->data, item_cnt, u_idx);

    TRACE_LOG("gf tongbao uid=[%u] a_cnt i_cnt [%u %u]",p->id, attire_cnt, item_cnt);
    pack(pkgbuf, attire_cnt, idx);
    trade_attire_t attire_obj = {0};
    for (uint32_t i = 0; i < attire_cnt; i++) {
        
        unpack_h(rsp->data, attire_obj.attireid, u_idx);
        unpack_h(rsp->data, attire_obj.uniquekey, u_idx);
        unpack_h(rsp->data, attire_obj.gettime, u_idx);
        unpack_h(rsp->data, attire_obj.timelag, u_idx);
/*
        const GfItem* itm = items->get_item(attire_obj.attireid);
        if (!itm) {
            ERROR_LOG("Item cannot usage !!! uid=[%u] id=[%u]", p->id, id);
            return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
        }
*/
        pack(pkgbuf, attire_obj.attireid, idx);
        pack(pkgbuf, attire_obj.uniquekey, idx);
        pack(pkgbuf, attire_obj.gettime, idx);
        pack(pkgbuf, attire_obj.timelag, idx);
        p->my_packs->add_clothes(p,attire_obj.attireid,attire_obj.uniquekey,0, channel_string_storage_shop,attire_obj.gettime,attire_obj.timelag,0);
        TRACE_LOG("trade attire --[%u %u %u %u]",attire_obj.attireid,attire_obj.uniquekey,attire_obj.gettime,attire_obj.timelag);
    }

    pack(pkgbuf, item_cnt, idx);
    trade_item_t item_obj = {0};
	int buy_item_cnt = 0;
    for(uint32_t i = 0; i < item_cnt; i++) {
        unpack_h(rsp->data, item_obj.itemid, u_idx);
        unpack_h(rsp->data, item_obj.itemcnt, u_idx);
        pack(pkgbuf, item_obj.itemid, idx);
        pack(pkgbuf, item_obj.itemcnt, idx);
        p->my_packs->add_item(p, item_obj.itemid, item_obj.itemcnt, channel_string_storage_shop, false, store_trade);
		buy_item_cnt += item_obj.itemcnt;
        TRACE_LOG("trade item --[%u %u]",item_obj.itemid,item_obj.itemcnt);
    }
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	uint32_t gain_coin_cnt = rsp->cost / 100;
	if (is_in_active_time_section(23) ==0 && is_product_vip_box(product_id)) {
		gain_coin_cnt *= 2;
	}

    player_gain_item(p, 2, gold_coin_item_id, gain_coin_cnt, channel_string_use_tongbao_item, false);
	proc_gain_item_achieve_logic(p, 0, buy_item_cnt + attire_cnt, gain_item_vip_shop);
    return 0;
}

/**
  * @brief handle vipsvr_gcoin_trade callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  */
int vipsvr_getback_outdated_attire_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_VIP_SVR_ERR(p, ret);
	CHECK_VAL_GE(bodylen, sizeof(vipsvr_gcoin_trade_rsp_t));

	vipsvr_gcoin_trade_rsp_t* rsp = reinterpret_cast<vipsvr_gcoin_trade_rsp_t*>(body);
    CHECK_VAL_EQ(bodylen, sizeof(vipsvr_gcoin_trade_rsp_t) + rsp->datalen);
    KDEBUG_LOG(p->id, "GF TONGBAO\t UID=[%u] [%u %u %u %u]",p->id,rsp->trans_id,rsp->cost,rsp->left,rsp->datalen);

	int idx = sizeof(cli_proto_t);
    //pack(pkgbuf, rsp->trans_id, idx);
    //pack(pkgbuf, rsp->cost, idx);
    pack(pkgbuf, rsp->left, idx);

    int u_idx = 0;
    uint32_t attire_cnt = 0;
    unpack_h(rsp->data, attire_cnt, u_idx);

    pack(pkgbuf, attire_cnt, idx);
    trade_attire_t attire_obj = {0};
    for (uint32_t i = 0; i < attire_cnt; i++) {
        unpack_h(rsp->data, attire_obj.attireid, u_idx);
        unpack_h(rsp->data, attire_obj.uniquekey, u_idx);
        unpack_h(rsp->data, attire_obj.gettime, u_idx);
        unpack_h(rsp->data, attire_obj.timelag, u_idx);
        pack(pkgbuf, attire_obj.attireid, idx);
        pack(pkgbuf, attire_obj.uniquekey, idx);
        pack(pkgbuf, attire_obj.gettime, idx);
        pack(pkgbuf, attire_obj.timelag, idx);

        p->my_packs->add_clothes(p,attire_obj.attireid,attire_obj.uniquekey,0, channel_string_storage_shop,attire_obj.gettime,attire_obj.timelag,0);
        TRACE_LOG("trade attire --[%u %u %u %u]",attire_obj.attireid,attire_obj.uniquekey,attire_obj.gettime,attire_obj.timelag);
    }
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief handle vipsvr_query_gcoin_balance callback
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic code server
  * @return 0 on success, -1 on error
  */
int vipsvr_query_gcoin_balance_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_VIP_SVR_ERR(p, ret);
	CHECK_VAL_EQ(bodylen, sizeof(vipsvr_query_gcoin_balance_rsp_t));

	vipsvr_query_gcoin_balance_rsp_t* rsp = reinterpret_cast<vipsvr_query_gcoin_balance_rsp_t*>(body);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->balance, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

