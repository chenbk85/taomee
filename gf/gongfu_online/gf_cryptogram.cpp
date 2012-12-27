#include <libtaomee++/inet/pdumanip.hpp>

using namespace taomee;

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
}

#include "utils.hpp"
#include "cli_proto.hpp"
#include "player.hpp"
#include "item.hpp"
#include "vip_impl.hpp"
#include "gf_cryptogram.hpp"
#include "global_data.hpp"

/*! file descriptor for connection with gf_cryptogram server */
//int gf_cryptogram_svr_fd = -1;


//---------------package sending and receiving interface with gf_cryptogram server-------------------------
/**
  * @brief uniform interface to send request to gf_cryptogram server
  * @param p consumer 
  * @param cmd command id with mc server
  * @param body package body
  * @param bodylen package body length
  * @return 0 on success, -1 on error
  */
int send_request_to_gf_cryptogram_svr(player_t* p, uint16_t cmd, const uint8_t* body, uint32_t bodylen)
{
	static uint8_t ts_pkg[8192];

	if (gf_cryptogram_svr_fd == -1) {
		gf_cryptogram_svr_fd = connect_to_svr(config_get_strval("gf_cryptogram_ip"), config_get_intval("gf_cryptogram_port", 0), 65535, 1);
	}

	if (gf_cryptogram_svr_fd == -1) {
		WARN_LOG("gf_cryptogram server unavailable");
		if (p) {
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	if ((bodylen > (8192 - sizeof(gf_cryptogram_proto_t))) && p) {
		ERROR_LOG("send package to gf_cryptogram server failed: "
				"pkg too large: [uid=%u bodylen=%u]", p->id, bodylen);
		if (p) {
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	uint32_t len = sizeof(gf_cryptogram_proto_t) + bodylen;

	gf_cryptogram_proto_t* pkg = reinterpret_cast<gf_cryptogram_proto_t*>(ts_pkg);
	pkg->pkglen = len;
	pkg->seq = ((p->fd) << 16) | (p->waitcmd);
	pkg->cmd = cmd; //cmd to/from gf_cryptogram server, diff it from p->waitcmd
	pkg->id = p->id;
	pkg->ret = 0;

	memcpy(pkg->body, body, bodylen);
	KDEBUG_LOG(p->id, "SEND PKG TO CRYPTO_NUMBER SERVER\t"
			"[uid=%u wait_cmd=%d cmd=%u protolen=%u bodylen=%u]", 
            p->id, p->waitcmd, pkg->cmd, len, bodylen);

	return net_send(gf_cryptogram_svr_fd, ts_pkg, len);
}

/**
  * @brief uniform interface to process on return from gf_cryptogram server
  * @param pkg returned package
  * @param pkglen package length
  */
void handle_gf_cryptogram_svr_return(gf_cryptogram_proto_t* pkg, uint32_t pkglen)
{
	uint32_t waitcmd = pkg->seq & 0xFFFF;
	int      connfd  = pkg->seq >> 16;

	player_t* p = get_player_by_fd(connfd);
	if (!p || (p->waitcmd != waitcmd)) {
		ERROR_LOG("connection has been closed: uid=%u cmd=%u %u fd=%d",
				pkg->id, pkg->cmd, waitcmd, connfd);
		return;
	}
    KDEBUG_LOG(p->id, "CRYPTO NUMBER SERVER RETURN\t"
        "[uid=%u cmd=%u waitcmd=%u fd=%d pkglen=%u]",pkg->id, pkg->cmd, waitcmd, connfd, pkglen);

#define PROC_CRYPTO_GIFT_PKG(proto, func) \
    case proto: func(p, pkg->body, pkg->pkglen - sizeof(gf_cryptogram_proto_t), pkg->ret); break

    switch (pkg->cmd) {
	PROC_CRYPTO_GIFT_PKG(tsproto_view_cryptogram_gift, ts_view_cryptogram_gift_callback);
	PROC_CRYPTO_GIFT_PKG(tsproto_consume_cryptogram_gift, ts_consume_cryptogram_gift_callback);

#undef PROC_CRYPTO_GIFT_PKG
	default:
		ERROR_LOG("unsupported gf_cryptogram svr cmd=%u id=%u waitcmd=%d",
				pkg->cmd, p->id, p->waitcmd);
		break;
    }

	return;
}



//---------------------------------------------------------------------------------------------------
// Cmds from client processings
//--------------------------------------------------------------------------------------------------
/**
  * @brief player view gift of gf_cryptogram
  * @param p the player who consumes the code
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int ts_view_cryptogram_gift_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0, idx2 = 0;
    cryptogram_in_header cryptogram_in = {{0},{0},0,0};
    unpack(body, cryptogram_in.cryptogram_num, max_hash_len, idx);
    TRACE_LOG("uid=[%u] cryptogram number=[%s]", p->id, cryptogram_in.cryptogram_num);

    pack(pkgbuf, cryptogram_in.cryptogram_num, max_hash_len, idx2);
    return send_request_to_gf_cryptogram_svr(p, tsproto_view_cryptogram_gift, pkgbuf, idx2);
}

/**
  * @brief player consumes gf_cryptogram for gifts
  * @param p the player who consumes the code
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int ts_consume_cryptogram_gift_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    int idx2 = 0;
    cryptogram_in_header cryptogram_in = {{0},{0},0,0};
    unpack(body, cryptogram_in.cryptogram_num, max_hash_len, idx);
	//CHECK_VAL_EQ(bodylen, sizeof(cryptogram_in) + cryptogram_in.chosecount * 8);
    TRACE_LOG("cryptogram number=[%s] userid=[%u] idx=[%d]",cryptogram_in.cryptogram_num, p->id, idx);

    uint32_t max_bag = get_player_total_item_bag_grid_count(p);
    if ((max_bag - p->my_packs->all_items_cnt()) < 1) {
        KDEBUG_LOG(p->id, "CONSUME CRYPTO GIFT CMD\t package FULL [uid=%u max_bag=%u]",p->id, max_bag);
        return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
    }
    pack(pkgbuf, cryptogram_in.hash_num, max_hash_len, idx2);
    pack(pkgbuf, cryptogram_in.cryptogram_num, max_hash_len, idx2);
    pack_h(pkgbuf, p->fdsess->remote_ip, idx2);
    pack_h(pkgbuf, max_bag, idx2);
    pack_h(pkgbuf, p->role_tm, idx2);

    KDEBUG_LOG(p->id, "CONSUME CRYPTO GIFT CMD\t[uid=%u code=%s rip=%x bag=%u len=%u]", p->id, cryptogram_in.cryptogram_num,p->fdsess->remote_ip, max_bag, idx2);

    //ts_consume_cryptogram_gift_callback(p, pkgbuf, idx2, 0);
	return send_request_to_gf_cryptogram_svr(p, tsproto_consume_cryptogram_gift, pkgbuf, idx2);
}
//--------------------------------------------------------------------------------------------------

//------- Functions for processing  packages received from gf_cryptogram server and responsing to client -------------

/**
  * @brief process on view cryptogram gift code
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from gf_cryptogram server
  * @return 0 on success, -1 on error
  */
int ts_view_cryptogram_gift_callback(player_t* p, const void* body, uint32_t bodylen, uint32_t ret)
{
    if (bodylen < sizeof(view_gift_t) + 4) {
        ERROR_LOG("ret = %u", ret);
        int idx = 0;
        idx = sizeof(cli_proto_t);
        pack(pkgbuf, static_cast<uint32_t>(0), idx);//fail 
        pack(pkgbuf, static_cast<uint32_t>(ret), idx);//err code
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    }
    int u_idx = 0;
    uint32_t gift_cnt = 0;
    unpack_h(body, gift_cnt, u_idx);
    if (gift_cnt > MAX_GIFT_LIMIT) {
        ERROR_LOG("cryptogram gift count=[%u] out of boundary", gift_cnt);
        gift_cnt = 30;
    }
    CHECK_VAL_EQ(bodylen, 4 + sizeof(view_gift_t) * gift_cnt);

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, static_cast<uint32_t>(1), idx);//success
    pack(pkgbuf, static_cast<uint32_t>(0), idx);//success
    pack(pkgbuf, gift_cnt, idx);

    view_gift_t* gift = reinterpret_cast<view_gift_t*>((uint8_t*)body + 4);
    for (uint32_t i = 0; i < gift_cnt; i++) {
        TRACE_LOG("view cryptogram gift: %u %u %u",gift[i].itemid, gift[i].item_cnt, gift[i].itemtype);
        pack(pkgbuf, gift[i].itemid, idx);
        pack(pkgbuf, gift[i].itemtype, idx);
        pack(pkgbuf, gift[i].item_cnt, idx);
    }
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}
/**
  * @brief process on consuming gf_cryptogram code
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from gf_cryptogram server
  * @return 0 on success, -1 on error
  */
int ts_consume_cryptogram_gift_callback(player_t* p, const void* body, uint32_t bodylen, uint32_t ret)
{
	//CHECK_TS_SVR_ERR(p, ret);
    if (bodylen < sizeof(consume_gift_t) + 4) {
        ERROR_LOG("ret = %u", ret);
        int idx = 0;
        idx = sizeof(cli_proto_t);
        pack(pkgbuf, static_cast<uint32_t>(0), idx);//fail
        pack(pkgbuf, static_cast<uint32_t>(ret), idx);//err code
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    }
    int u_idx = 0;
    uint32_t gift_cnt = 1;
    unpack_h(body, gift_cnt, u_idx);
    CHECK_VAL_EQ(bodylen, 4 + sizeof(consume_gift_t) * gift_cnt);
    KDEBUG_LOG(p->id, "CONSUME CRYPTO GIFT BACK\t[uid=%u gift count=%u]", p->id, gift_cnt);

    int p_idx = sizeof(cli_proto_t);
    pack(pkgbuf, static_cast<uint32_t>(1), p_idx);//success
    pack(pkgbuf, static_cast<uint32_t>(0), p_idx);//success
    pack(pkgbuf, gift_cnt, p_idx);

    consume_gift_t* gift = reinterpret_cast<consume_gift_t*>((uint8_t*)body + 4);
    for (uint32_t i = 0; i < gift_cnt; i++){
        TRACE_LOG("consume cryptogram gift: %u %u %u",gift[i].itemid,gift[i].item_cnt,gift[i].itemtype);
        if (gift[i].itemtype == give_type_clothes) {
            p->my_packs->add_clothes(p,gift[i].itemid,gift[i].uniqueid,0, channel_string_magic_num);
        } else if (gift[i].itemtype == give_type_normal_item) {
            p->my_packs->add_item(p, gift[i].itemid, gift[i].item_cnt, channel_string_magic_num, true, system_reward);
        }
        pack(pkgbuf, gift[i].itemid, p_idx);
        pack(pkgbuf, gift[i].uniqueid, p_idx);
        pack(pkgbuf, gift[i].itemtype, p_idx);
        pack(pkgbuf, gift[i].item_cnt, p_idx);
    }
	init_cli_proto_head(pkgbuf, p, p->waitcmd, p_idx);
	return send_to_player(p, pkgbuf, p_idx, 1);
}
