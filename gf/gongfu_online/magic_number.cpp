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
#include "magic_number.hpp"
#include "global_data.hpp"

/*! file descriptor for connection with magic_number server */
//int magic_number_svr_fd = -1;


//---------------package sending and receiving interface with magic_number server-------------------------
/**
  * @brief uniform interface to send request to magic_number server
  * @param p consumer 
  * @param cmd command id with mc server
  * @param body package body
  * @param bodylen package body length
  * @return 0 on success, -1 on error
  */
int send_request_to_magic_number_svr(player_t* p, uint16_t cmd, const uint8_t* body, uint32_t bodylen)
{
	static uint8_t ts_pkg[8192];

	if (magic_number_svr_fd == -1) {
		magic_number_svr_fd = connect_to_svr(config_get_strval("magic_number_ip"), config_get_intval("magic_number_port", 0), 65535, 1);
	}

	if (magic_number_svr_fd == -1) {
		WARN_LOG("magic_number server unavailable");
		if (p) {
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	if ((bodylen > (8192 - sizeof(magic_number_proto_t))) && p) {
		ERROR_LOG("send package to magic_number server failed: "
				"pkg too large: [uid=%u bodylen=%u]", p->id, bodylen);
		if (p) {
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	uint32_t len = sizeof(magic_number_proto_t) + bodylen;

	magic_number_proto_t* pkg = reinterpret_cast<magic_number_proto_t*>(ts_pkg);
	pkg->pkglen = len;
	pkg->seq = ((p->fd) << 16) | (p->waitcmd);
	pkg->cmd = cmd; //cmd to/from magic_number server, diff it from p->waitcmd
	pkg->id = p->id;
	pkg->ret = 0;

	memcpy(pkg->body, body, bodylen);
	KDEBUG_LOG(p->id, "SEND PKG TO MAGIC_NUMBER SERVER\t"
			"[uid=%u wait_cmd=%d cmd=%u protolen=%u bodylen=%u]", 
            p->id, p->waitcmd, pkg->cmd, len, bodylen);

	return net_send(magic_number_svr_fd, ts_pkg, len);
}

/**
  * @brief uniform interface to process on return from magic_number server
  * @param pkg returned package
  * @param pkglen package length
  */
void handle_magic_number_svr_return(magic_number_proto_t* pkg, uint32_t pkglen)
{
	uint32_t waitcmd = pkg->seq & 0xFFFF;
	int      connfd  = pkg->seq >> 16;

	player_t* p = get_player_by_fd(connfd);
	if (!p || (p->waitcmd != waitcmd)) {
		ERROR_LOG("connection has been closed: uid=%u cmd=%u %u fd=%d",
				pkg->id, pkg->cmd, waitcmd, connfd);
		return;
	}
    KDEBUG_LOG(p->id, "MAGIC NUMBER SERVER RETURN\t"
        "[uid=%u cmd=%u waitcmd=%u fd=%d pkglen=%u]",pkg->id, pkg->cmd, waitcmd, connfd, pkglen);

#define PROC_MAGIC_GIFT_PKG(proto, func) \
    case proto: func(p, pkg->body, pkg->pkglen - sizeof(magic_number_proto_t), pkg->ret); break

    switch (pkg->cmd) {
	PROC_MAGIC_GIFT_PKG(tsproto_view_magic_gift, ts_view_magic_gift_callback);
	PROC_MAGIC_GIFT_PKG(tsproto_consume_magic_gift, ts_consume_magic_gift_callback);

#undef PROC_MAGIC_GIFT_PKG
	default:
		ERROR_LOG("unsupported magic_number svr cmd=%u id=%u waitcmd=%d",
				pkg->cmd, p->id, p->waitcmd);
		break;
    }

	return;
}



//---------------------------------------------------------------------------------------------------
// Cmds from client processings
//--------------------------------------------------------------------------------------------------
/**
  * @brief player view gift of magic_number
  * @param p the player who consumes the code
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int ts_view_magic_gift_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0, idx2 = 0;
    magic_in_header magic_in = {{0},0};
    unpack(body, magic_in.magic_num, c_ts_hash_len, idx);
    TRACE_LOG("uid=[%u] magic number=[%s]", p->id, magic_in.magic_num);

    pack(pkgbuf, magic_in.magic_num, c_ts_hash_len, idx2);
    return send_request_to_magic_number_svr(p, tsproto_view_magic_gift, pkgbuf, idx2);
}

/**
  * @brief player consumes magic_number for gifts
  * @param p the player who consumes the code
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int ts_consume_magic_gift_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    int idx2 = 0;
    magic_in_header magic_in = {{0},0};
    //CHECK_VAL_GE(bodylen, sizeof(magic_in));
    unpack(body, magic_in.magic_num, c_ts_hash_len, idx);
    unpack(body, magic_in.chosecount, idx);
	CHECK_VAL_EQ(bodylen, sizeof(magic_in) + magic_in.chosecount * 8);
    TRACE_LOG("magic number=[%s] userid=[%u] chosecount=[%u] idx=[%d]",magic_in.magic_num,
        p->id, magic_in.chosecount, idx);
    uint32_t max_bag = get_player_total_item_bag_grid_count(p);
    if ((max_bag - p->my_packs->all_items_cnt()) < magic_in.chosecount) {
        KDEBUG_LOG(p->id, "CONSUME MAGIC GIFT CMD\t package FULL [uid=%u max_bag=%u]",p->id, max_bag);
        return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
    }
    pack(pkgbuf, magic_in.magic_num, c_ts_hash_len, idx2);
    pack_h(pkgbuf, p->id, idx2);
    pack_h(pkgbuf, p->role_tm, idx2);
    pack_h(pkgbuf, p->fdsess->remote_ip, idx2);
    pack_h(pkgbuf, max_bag, idx2);
    pack_h(pkgbuf, static_cast<uint16_t>(magic_in.chosecount), idx2);

    uint32_t itemid = 0, item_cnt = 0;
    for ( uint32_t i = 0; i < magic_in.chosecount; i++) {
        unpack(body, itemid, idx);
        unpack(body, item_cnt, idx);
        pack_h(pkgbuf, itemid, idx2);
        pack_h(pkgbuf, static_cast<uint16_t>(item_cnt), idx2);
        TRACE_LOG("magic item id=[%u] cnt=[%d] len=[%u]",itemid, item_cnt, idx2);
    }
    KDEBUG_LOG(p->id, "CONSUME MAGIC GIFT CMD\t[uid=%u code=%s rip=%x bag=%u len=%u]", p->id, magic_in.magic_num,p->fdsess->remote_ip, max_bag, idx2);

    //ts_consume_magic_gift_callback(p, pkgbuf, idx2, 0);
	return send_request_to_magic_number_svr(p, tsproto_consume_magic_gift, pkgbuf, idx2);
}
//--------------------------------------------------------------------------------------------------

//------- Functions for processing  packages received from magic_number server and responsing to client -------------
/**
  * @brief process on view magic gift code
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic_number server
  * @return 0 on success, -1 on error
  */
int ts_view_magic_gift_callback(player_t* p, const void* body, uint32_t bodylen, uint32_t ret)
{
    if ( ret != 0 ) {
        ERROR_LOG("ret = %u", ret);
        int idx = 0;
        idx = sizeof(cli_proto_t);
        pack(pkgbuf, static_cast<uint32_t>(0), idx);//fail 
        pack(pkgbuf, static_cast<uint32_t>(ret), idx);//err code
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    }
    int u_idx = 0;
    uint16_t max_cnt = 0, used_cnt = 0, left_cnt = 0;
    unpack_h(body, max_cnt, u_idx);
    unpack_h(body, used_cnt, u_idx);
    unpack_h(body, left_cnt, u_idx);
    KDEBUG_LOG(p->id, "VIEW MAGIC GIFT CALLBACK\t [%u %u %u]", max_cnt, used_cnt, left_cnt);
    CHECK_VAL_EQ(bodylen, 6 + sizeof(magic_in_item) * left_cnt);

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, static_cast<uint32_t>(1), idx);//success
    pack(pkgbuf, static_cast<uint32_t>(0), idx);//success
    pack(pkgbuf, static_cast<uint32_t>(max_cnt), idx);
    pack(pkgbuf, static_cast<uint32_t>(used_cnt), idx);
    pack(pkgbuf, static_cast<uint32_t>(left_cnt), idx);

    magic_in_item* gift = reinterpret_cast<magic_in_item*>((uint8_t*)body + 6);
    for (uint32_t i = 0; i < left_cnt; i++) {
        TRACE_LOG("view magic gift: %u %u",gift[i].itemid, gift[i].item_cnt);
        pack(pkgbuf, gift[i].itemid, idx);
        pack(pkgbuf, static_cast<uint32_t>(gift[i].item_cnt), idx);
    }
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief process on consuming magic_number code
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic_number server
  * @return 0 on success, -1 on error
  */
int ts_consume_magic_gift_callback(player_t* p, const void* body, uint32_t bodylen, uint32_t ret)
{
	//CHECK_TS_SVR_ERR(p, ret);
    if (bodylen < sizeof(magic_out_item) + 4) {
        int idx = 0;
        idx = sizeof(cli_proto_t);
        pack(pkgbuf, static_cast<uint32_t>(0), idx);//fail
        pack(pkgbuf, static_cast<uint32_t>(ret), idx);//err code
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    }
    int u_idx = 0;
    uint16_t gift_cnt = 0;
    unpack_h(body, gift_cnt, u_idx);
    TRACE_LOG("Magic gift userid=[%u] cnt=[%u]", p->id, gift_cnt);
    CHECK_VAL_EQ(bodylen, 4 + sizeof(magic_out_item) * gift_cnt);

    int p_idx = sizeof(cli_proto_t);
    pack(pkgbuf, static_cast<uint32_t>(1), p_idx);//success
    pack(pkgbuf, static_cast<uint32_t>(0), p_idx);//success
    pack(pkgbuf, static_cast<uint32_t>(gift_cnt), p_idx);

    magic_out_item* item = reinterpret_cast<magic_out_item*>((uint8_t*)body + 4);
    for (uint32_t i = 0; i < gift_cnt; i++){
        pack(pkgbuf, item[i].itemid, p_idx);
        pack(pkgbuf, item[i].item_cnt, p_idx);
        p->my_packs->add_item(p, item[i].itemid, item[i].item_cnt, channel_string_magic_num, true, system_reward);
        TRACE_LOG("magic item id unique cnt --[%u %u %u]",item[i].itemid,item[i].uniqueid,item[i].item_cnt);
    }
    pack(pkgbuf, static_cast<uint32_t>(0), p_idx); // set attire count = 0
	KDEBUG_LOG(p->id, "CONSUME MAGIC GIFT BACK\t[uid=%u gift count=%u]", p->id, gift_cnt);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, p_idx);
	return send_to_player(p, pkgbuf, p_idx, 1);
}

