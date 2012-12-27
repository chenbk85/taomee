#include <libtaomee++/inet/pdumanip.hpp>
using namespace taomee;

extern "C" {
#include <libtaomee/project/utilities.h>
}
#include "fwd_decl.hpp"
#include "safe_trade.hpp"
#include "cli_proto.hpp"
//#include "trade.hpp"


//------------------------------------------------------
// global function
//------------------------------------------------------

static SafeTrade safe_trade_mgr;
SafeTrade* get_safe_trade_mgr()
{
    return &safe_trade_mgr;
}

void clear_safe_trade_room(int fd)
{
    get_safe_trade_mgr()->del_trade_room_by_fd(fd);
}

void handle_safe_trade_dispatch(fdsession_t* fdsess, uint32_t cmd, uint8_t* body, uint32_t bodylen)
{
    //KDEBUG_LOG(0, "safe trade cmd trace ----> \t[%u]", cmd);
    switch (cmd) {
        case trd_safe_trade_create_room:
            safe_trade_create_room_cmd(fdsess, body, bodylen);
            break;
        case trd_safe_trade_join_room:
            safe_trade_join_room_cmd(fdsess, body, bodylen);
            break;
        case trd_safe_trade_cancel_room:
            safe_trade_cancel_room_cmd(fdsess, body, bodylen);
            break;
        case trd_safe_trade_set_item:
            safe_trade_set_item_cmd(fdsess, body, bodylen);
            break;
        case trd_safe_trade_action_agree:
            safe_trade_action_agree_cmd(fdsess, body, bodylen);
            break;
        default:
            break;
    }
}

//-------------------------------------------------------
//  little function
//-------------------------------------------------------
uint32_t global_room_id(uint32_t id_)
{
    uint32_t server_id = get_server_id();
    TRACE_LOG("%u %u", server_id, id_ |  (server_id << 16 & 0xffff0000));
    return (id_ |  (server_id << 16 & 0xffff0000));
}

uint32_t get_trade_room_id(uint32_t global_room_id)
{
    return (global_room_id & 0x0000ffff);
}

int send_to_room_player(uint32_t trade_id, void* pkgbuf, uint32_t len, int completed)
{
    trade_room_t *room = get_safe_trade_mgr()->get_trade_room_info(trade_id);
    if (room == NULL) {
        ERROR_LOG("find safe trade room [%u] error !!!!", trade_id);
        return 0;
    }

    for (uint32_t i = 0; i < 2; i++) {
        if (room->trader[i].userid != 0) {
            send_to_player_by_uid(room->trader[i].userid, &(room->trader[i].fdsess), pkgbuf, len, completed);
        }
    }

    return 0;
}

trader_info_t *get_trader(uint32_t trade_id, uint32_t userid)
{
    trade_room_t *room = get_safe_trade_mgr()->get_trade_room_info(trade_id);
    if (room == NULL) {
        ERROR_LOG("find safe trade room [%u] error !!!!", trade_id);
        return NULL;
    }
    for (uint32_t i = 0; i < 2; i++) {
        if (room->trader[i].userid == userid) {
            return &(room->trader[i]);
        }
    }
    return NULL;
}

trader_info_t *get_peer_trader(uint32_t trade_id, uint32_t userid)
{
    trade_room_t *room = get_safe_trade_mgr()->get_trade_room_info(trade_id);
    if (room == NULL) {
        ERROR_LOG("find safe trade room [%u] error !!!!", trade_id);
        return NULL;
    }
    for (uint32_t i = 0; i < 2; i++) {
        if (room->trader[i].userid != userid) {
            return &(room->trader[i]);
        }
    }
    return NULL;
}

uint32_t get_trader_step_num(uint32_t trade_id, uint32_t step_id)
{
    uint32_t step_num = 0;
    trade_room_t *room = get_safe_trade_mgr()->get_trade_room_info(trade_id);
    if (room == NULL) {
        ERROR_LOG("find safe trade room [%u] error !!!!", trade_id);
        return 0;
    }
    for (uint32_t i = 0; i < 2; i++) {
        if (room->trader[i].trade_step == step_id) {
            step_num++;
        }
    }
    return step_num;
}

bool set_trader_step(uint32_t trade_id, uint32_t userid, uint32_t step_id)
{
    trader_info_t *trader = get_trader(trade_id, userid);
    if (trader) {
        trader->trade_step = step_id;
        return true;
    }
    ERROR_LOG("set trader step failed. room [%u] find [%u] failed", trade_id, userid);
    return false;
}

//--------------------------------------------------------
// CMD function
//--------------------------------------------------------

void alloc_trade_room(uint32_t uid, fdsession_t* fdsess)
{
}

void free_trade_room(uint32_t uid)
{
}

/**
 * @brief create safe trade room
 * @param
 * @return
 */
int safe_trade_create_room_cmd(fdsession_t* fdsess, uint8_t* body, uint32_t bodylen)
{
	uint32_t _id = 0, _tm = 0, peer_id = 0, peer_tm = 0;
	int idx = 0;
	unpack_h(body, _id, idx);
	unpack_h(body, _tm, idx);
	unpack_h(body, peer_id, idx);
	unpack_h(body, peer_tm, idx);

    trade_room_t room_obj;

    room_obj.trade_id = get_safe_trade_mgr()->produce_trade_id();

    KDEBUG_LOG(_id, "create ---> room[%u] [%u|%u] [%u|%u]", room_obj.trade_id, _id, _tm, peer_id, peer_tm);
    room_obj.trader[0].userid = _id;
    room_obj.trader[0].role_tm = _tm;
    room_obj.trader[0].trade_step = step_enter_room;
    memcpy(&(room_obj.trader[0].fdsess), fdsess, sizeof(fdsession_t));

    get_safe_trade_mgr()->add_trade_room(&room_obj);
			
    int ifx = sizeof(tr_proto_t);
    pack_h(trpkgbuf, global_room_id(room_obj.trade_id), ifx);
    pack_h(trpkgbuf, peer_id, ifx);
    pack_h(trpkgbuf, peer_tm, ifx);
    init_tr_proto_head(trpkgbuf, trd_safe_trade_create_room, ifx);
    return send_to_player_by_uid(_id, fdsess, trpkgbuf, ifx, 1);
}

/**
 * @brief enter safe trade room
 * @param
 * @return
 */
int safe_trade_join_room_cmd(fdsession_t* fdsess, uint8_t* body, uint32_t bodylen)
{
	uint32_t room_id = 0, userid = 0, role_tm = 0;
	int idx = 0;
	unpack_h(body, room_id, idx);
	unpack_h(body, userid, idx);
	unpack_h(body, role_tm, idx);

    uint32_t trade_id = get_trade_room_id(room_id);

    KDEBUG_LOG(userid, "Join ---> room[%u] [%u|%u]", trade_id, userid, role_tm);
    uint32_t room_full_flag = 0;

    trade_room_t *p_room = get_safe_trade_mgr()->get_trade_room_info(trade_id);
    if (p_room == NULL) {
        ERROR_LOG("[%u] find room [%u] failed !!", userid, trade_id);
        int ifx = sizeof(tr_proto_t);
        pack_h(trpkgbuf, room_id, ifx);
        pack_h(trpkgbuf, room_full_flag, ifx);
        pack_h(trpkgbuf, 0, ifx);
        pack_h(trpkgbuf, 0, ifx);
        init_tr_proto_head(trpkgbuf, trd_safe_trade_join_room, ifx);
        return send_to_player_by_uid(userid, fdsess, trpkgbuf, ifx, 1);
    }

    for (uint32_t i = 0; i < 2; i++) {
        if (p_room->trader[i].userid == 0) {
            p_room->trader[i].userid = userid;
            p_room->trader[i].role_tm = role_tm;
            p_room->trader[i].trade_step = step_enter_room;
            memcpy(&(p_room->trader[i].fdsess), fdsess, sizeof(fdsession_t));
        }
        if (p_room->trader[i].trade_step == step_enter_room) {
            room_full_flag++;
        }
    }

    trader_info_t *peer = get_peer_trader(trade_id, userid);
    if (peer == NULL) {
        ERROR_LOG("[%u] find peer failed !!!", userid);
        return -1;
    }
    int ifx = sizeof(tr_proto_t);
    pack_h(trpkgbuf, global_room_id(p_room->trade_id), ifx);
    pack_h(trpkgbuf, room_full_flag, ifx);
    pack_h(trpkgbuf, peer->userid, ifx);
    pack_h(trpkgbuf, peer->role_tm, ifx);
    init_tr_proto_head(trpkgbuf, trd_safe_trade_join_room, ifx);
    return send_to_player_by_uid(userid, fdsess, trpkgbuf, ifx, 1);
}


int do_safe_trade_cancel_logic(uint32_t trade_id, uint32_t userid)
{
    int idx = sizeof(tr_proto_t);
    pack_h(trpkgbuf, userid, idx);
    init_tr_proto_head(trpkgbuf, trd_safe_trade_cancel_room, idx);
    send_to_room_player(trade_id, trpkgbuf, idx, 1);

    get_safe_trade_mgr()->del_trade_room(trade_id);
    get_safe_trade_mgr()->release_trade_id(trade_id);

    return 0;
}

/**
 * @brief cancel safe trade room
 * @param
 * @return
 */
int safe_trade_cancel_room_cmd(fdsession_t* fdsess, uint8_t* body, uint32_t bodylen)
{
	uint32_t room_id = 0, userid = 0, role_tm = 0;
	int idx = 0;
	unpack_h(body, room_id, idx);
	unpack_h(body, userid, idx);
	unpack_h(body, role_tm, idx);

    uint32_t trade_id = get_trade_room_id(room_id);

    KDEBUG_LOG(userid, "CANCEL ---> room[%u] [%u|%u]", trade_id, userid, role_tm);

    return do_safe_trade_cancel_logic(trade_id, userid);
}


int pack_set_item_pkg(uint8_t *buf, set_item_t *pkg)
{
    int idx = 0;
    pack_h(buf, pkg->room_id, idx);
    pack_h(buf, pkg->userid, idx);
    pack_h(buf, pkg->action, idx);
    pack_h(buf, pkg->xiaomee, idx);
    pack_h(buf, pkg->itemcnt, idx);
    pack_h(buf, pkg->attirecnt, idx);
    trade_item_t *items = reinterpret_cast<trade_item_t*>(pkg->items);
    trade_attire_t *attires = reinterpret_cast<trade_attire_t*>(pkg->items + pkg->itemcnt * sizeof(trade_item_t));
    for (uint32_t i = 0; i < pkg->itemcnt; i++) {
        pack_h(buf, items[i].item_id, idx);
        pack_h(buf, items[i].itemcnt, idx);
    }
    for (uint32_t i = 0; i < pkg->attirecnt; i++) {
        pack_h(buf, attires[i].attire_id, idx);
        pack_h(buf, attires[i].unique_id, idx);
        pack_h(buf, attires[i].item_lv, idx);
    }

    return idx;
}

int safe_trade_set_item_cmd(fdsession_t* fdsess, uint8_t* body, uint32_t bodylen)
{
    set_item_t *pkg = reinterpret_cast<set_item_t*>(body);
    uint32_t trade_id = get_trade_room_id(pkg->room_id);
	KDEBUG_LOG(pkg->userid, "safe trade set item\t[%u] [%u|%u] [%u] [%u|%u %u]", 
        trade_id, pkg->userid, pkg->role_tm, pkg->action, pkg->xiaomee, pkg->itemcnt, pkg->attirecnt); 

	CHECK_VAL_EQ(bodylen, sizeof(set_item_t) + (pkg->itemcnt) * sizeof(trade_item_t) + (pkg->attirecnt) * sizeof(trade_attire_t));

    trader_info_t *trader = get_trader(trade_id, pkg->userid);
    if (trader == NULL) {
        ERROR_LOG("find myself error!!!!!! uid[%u] room[%u]", pkg->userid, trade_id);
        return do_safe_trade_cancel_logic(trade_id, pkg->userid);
    }

    if (pkg->action == 1) { //set item
        //save item
        trader->trade_step = step_set_item;
        trader->xiaomee    = pkg->xiaomee;
        trader->item_cnt    = pkg->itemcnt;
        trader->attire_cnt  = pkg->attirecnt;

        trade_item_t *items = reinterpret_cast<trade_item_t*>(pkg->items);
        for (uint32_t i = 0; i < pkg->itemcnt; i++) {
            trader->item_array[i].item_id   = items->item_id;
            trader->item_array[i].itemcnt   = items->itemcnt;
            items++;
        }

        trade_attire_t *attires = reinterpret_cast<trade_attire_t*>(pkg->items + pkg->itemcnt * sizeof(trade_item_t));
        for (uint32_t i = 0; i < pkg->attirecnt; i++) {
            trader->attire_array[i].attire_id = attires->attire_id;
            trader->attire_array[i].unique_id = attires->unique_id;
            trader->attire_array[i].item_lv   = attires->item_lv;
            attires++;
        }

    } else if (pkg->action == 2) { //cancel item
        //clear item
        trader->trade_step  = step_enter_room;
        trader->xiaomee     = 0;
        trader->item_cnt    = 0;
        trader->attire_cnt  = 0;
        memset(trader->item_array, 0x00, sizeof(trade_item_t) * max_safe_trade_item_cnt);
        memset(trader->attire_array, 0x00, sizeof(trade_attire_t) * max_safe_trade_item_cnt);

        trader_info_t *peer = get_peer_trader(trade_id, pkg->userid);
        if (peer && peer->trade_step == step_agree_trade) {
            peer->trade_step = step_set_item;
        }

    }

    int idx = sizeof(tr_proto_t);
    idx += pack_set_item_pkg(trpkgbuf + idx, pkg);
    init_tr_proto_head(trpkgbuf, trd_safe_trade_set_item, idx);

    return send_to_room_player(trade_id, trpkgbuf, idx, 1);
}

/**
 * @brief
 * @param flag : 1--> fail 2--> clear cache
 */
int send_action_agree_result_to_online(uint32_t userid, fdsession_t* fdsess, uint32_t flag, int completed)
{
    int idx = sizeof(tr_proto_t);
    pack_h(trpkgbuf, flag, idx);
    pack_h(trpkgbuf, static_cast<uint32_t>(0), idx);
    pack_h(trpkgbuf, static_cast<uint32_t>(0), idx);
    pack_h(trpkgbuf, static_cast<uint32_t>(0), idx);
    pack_h(trpkgbuf, static_cast<uint32_t>(0), idx);
    pack_h(trpkgbuf, static_cast<uint32_t>(0), idx);
    pack_h(trpkgbuf, static_cast<uint32_t>(0), idx);
    init_tr_proto_head(trpkgbuf, trd_safe_trade_action_agree, idx);
    return send_to_player_by_uid(userid, fdsess, trpkgbuf, idx, completed);
}

int safe_trade_action_agree_cmd(fdsession_t* fdsess, uint8_t* body, uint32_t bodylen)
{
	uint32_t room_id = 0, userid = 0, role_tm = 0;
	int idx = 0;
	unpack_h(body, room_id, idx);
	unpack_h(body, userid, idx);
	unpack_h(body, role_tm, idx);

    uint32_t trade_id = get_trade_room_id(room_id);
	KDEBUG_LOG(userid, "safe trade action agree\t[%u] [%u|%u]", trade_id, userid, role_tm); 

    trader_info_t *trader = get_trader(trade_id, userid);
    if (trader == NULL) {
        ERROR_LOG("find myself error!!!!!! uid[%u] room[%u]", userid, trade_id);
        return send_action_agree_result_to_online(userid, fdsess, 1, 0);
    }

    uint32_t set_item_step_num = get_trader_step_num(trade_id, step_set_item);
    uint32_t agree_step_num = get_trader_step_num(trade_id, step_agree_trade);
    KDEBUG_LOG(userid, "safe trade set_item agree\t[%u] [%u]", set_item_step_num, agree_step_num);

    if (set_item_step_num + agree_step_num < 2) {
        return send_action_agree_result_to_online(userid, &(trader->fdsess), 1, 1);
    }

    trader->trade_step = step_agree_trade;
    if (agree_step_num < 1) {
        //do nothing
    	KDEBUG_LOG(userid, "agree ret 2 \t[%u] [%u]", set_item_step_num, agree_step_num);
        return send_action_agree_result_to_online(userid, &(trader->fdsess), 2, 1);
    }

    //db del item
    return db_safe_trade_item(trade_id, userid, role_tm, 1);
}

//--------------------------------------------------
// dbproxy function
//--------------------------------------------------

int handle_safe_trade_db_dispatch(db_proto_t* dbpkg, uint32_t pkglen)
{
    uint32_t trade_id = *reinterpret_cast<uint32_t*>(dbpkg->body);
    trader_info_t *trader = get_trader(trade_id, dbpkg->id);
    if (trader == NULL) {
        ERROR_LOG("cannot fint trader [%u] in room [%u]", dbpkg->id, trade_id);
        return 0;
    }

    KDEBUG_LOG(0, "safe trade db callback cmd\t [%u]", dbpkg->cmd);
    uint32_t bodylen = dbpkg->len - sizeof(db_proto_t);
    switch (dbpkg->cmd) {
        case dbproto_safe_trade_item:
            db_safe_trade_item_callback(trader, dbpkg->body, bodylen, dbpkg->ret);
            /*
        case dbproto_safe_trade_del:
            db_safe_trade_del_callback(trader, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case dbproto_safe_trade_add:
            db_safe_trade_add_callback(trader, dbpkg->body, bodylen, dbpkg->ret);
            */
            break;
        default:
            break;
    }

    return 0;
}

/**
 * @brief 
 * @param action: 1-->del, 2-->add;
 */
int db_safe_trade_item(uint32_t trade_id, uint32_t userid, uint32_t role_tm, uint32_t action) 
{
    trader_info_t *trader = NULL;
    if (action == 1) {
        trader = get_trader(trade_id, userid);
    } else if (action == 2) {
        trader = get_peer_trader(trade_id, userid);
    }

    if (trader == NULL) return 0;

    KDEBUG_LOG(userid, "safe trade\t[%u|%u] [%u|%u %u %u]", trader->userid, action, 
        trade_id, trader->xiaomee, trader->item_cnt, trader->attire_cnt);
    int idx = 0;
    pack_h(dbpkgbuf, trade_id, idx);
    pack_h(dbpkgbuf, action, idx);
    pack_h(dbpkgbuf, trader->xiaomee, idx);
    pack_h(dbpkgbuf, trader->item_cnt, idx);
    pack_h(dbpkgbuf, trader->attire_cnt, idx);
    
    for(uint32_t i = 0; i < trader->item_cnt; i++) {
        pack_h(dbpkgbuf, trader->item_array[i].item_id, idx);
        pack_h(dbpkgbuf, trader->item_array[i].itemcnt, idx);

        KDEBUG_LOG(userid, "safe trade\t[%u %u]", trader->item_array[i].item_id, trader->item_array[i].itemcnt);
    }
    for(uint32_t i = 0; i < trader->attire_cnt; i++) {
        pack_h(dbpkgbuf, trader->attire_array[i].attire_id, idx);
        pack_h(dbpkgbuf, trader->attire_array[i].unique_id, idx);
        pack_h(dbpkgbuf, trader->attire_array[i].item_lv, idx);

        KDEBUG_LOG(userid, "safe trade\t[%u %u %u]", trader->attire_array[i].attire_id, trader->attire_array[i].unique_id, trader->attire_array[i].item_lv);
    }

    return send_request_to_db(0, userid, role_tm, dbproto_safe_trade_item, dbpkgbuf, idx);
}


/**
 * @brief player safe trade del db callback
 * @return 0 on success, -1 on error
 */
int db_safe_trade_del_callback(trader_info_t *trader, uint32_t trade_id, db_safe_trade_item_t *datas)
{
    set_trader_step(trade_id, trader->userid, step_delete_item);

    trader_info_t *peer = get_peer_trader(trade_id, trader->userid);
    if (peer == NULL) {
        ERROR_LOG("find myself error!!!!!! uid[%u] room[%u]", trader->userid, trade_id);
        return -1;
    }

    uint32_t del_item_step_num = get_trader_step_num(trade_id, step_delete_item);
    if (del_item_step_num < 2) {
        db_safe_trade_item(trade_id, peer->userid, peer->role_tm, 1);
    } else {
        db_safe_trade_item(trade_id, trader->userid, trader->role_tm, 2);
        db_safe_trade_item(trade_id, peer->userid, peer->role_tm, 2);
    }
    return 0;   
}


int pack_safe_trade_item_pkg(void *buf, uint32_t trade_id, trader_info_t *trader, trade_attire_t *pattire, uint32_t add_attire_cnt)
{
    trader_info_t *peer = get_peer_trader(trade_id, trader->userid);
    if (trader == NULL || peer == NULL) return 0;

    //KDEBUG_LOG(trader->userid, "trace ---> \t [%u %u %u | %u %u (%u|%u)]", trader->xiaomee, trader->item_cnt, trader->attire_cnt, peer->xiaomee, peer->item_cnt, peer->attire_cnt, add_attire_cnt);

    int idx = 0;
    pack_h(buf, static_cast<uint32_t>(0), idx);
    pack_h(buf, trader->xiaomee, idx);
    pack_h(buf, trader->item_cnt, idx);
    pack_h(buf, trader->attire_cnt, idx);
    pack_h(buf, peer->xiaomee, idx);
    pack_h(buf, peer->item_cnt, idx);
    pack_h(buf, add_attire_cnt, idx);
    //pack_h(buf, peer->attire_cnt, idx);

    for (uint32_t i = 0; i < trader->item_cnt; i++) {
        pack_h(buf, trader->item_array[i].item_id, idx);
        pack_h(buf, trader->item_array[i].itemcnt, idx);
    }
    for (uint32_t i = 0; i < trader->attire_cnt; i++) {
        pack_h(buf, trader->attire_array[i].attire_id, idx);
        pack_h(buf, trader->attire_array[i].unique_id, idx);
        pack_h(buf, trader->attire_array[i].item_lv, idx);
    }

    for (uint32_t i = 0; i < peer->item_cnt; i++) {
        pack_h(buf, peer->item_array[i].item_id, idx);
        pack_h(buf, peer->item_array[i].itemcnt, idx);
    }

    for (uint32_t i = 0; i < add_attire_cnt; i++) {
        pack_h(buf, pattire->attire_id, idx);
        pack_h(buf, pattire->unique_id, idx);
        pack_h(buf, pattire->item_lv, idx);
        pattire++;
    }
    /*
    for (uint32_t i = 0; i < peer->attire_cnt; i++) {
        pack_h(buf, peer->attire_array[i].attire_id, idx);
        pack_h(buf, peer->attire_array[i].unique_id, idx);
        pack_h(buf, peer->attire_array[i].item_lv, idx);
    }*/

    return idx;
}

/**
 * @brief player safe trade add db callback
 * @return 0 on success, -1 on error
 */
int db_safe_trade_add_callback(trader_info_t *trader, uint32_t trade_id, db_safe_trade_item_t *datas)
{
    trade_attire_t *pattire = reinterpret_cast<trade_attire_t*>(datas->items + datas->item_cnt * sizeof(trade_item_t));
    /*
    for (uint32_t i = 0; i < trader->attire_cnt; i++) {
        trader->attire_array[i].unique_id = pattire->unique_id;

        pattire++;
    }*/

    int idx = sizeof(tr_proto_t);
    idx += pack_safe_trade_item_pkg(trpkgbuf + idx, trade_id, trader, pattire, datas->attire_cnt);
    init_tr_proto_head(trpkgbuf, trd_safe_trade_action_agree, idx);
    send_to_player_by_uid(trader->userid, &(trader->fdsess), trpkgbuf, idx, 1);

    set_trader_step(trade_id, trader->userid, step_add_item);
    uint32_t add_item_step_num = get_trader_step_num(trade_id, step_add_item);
    if (add_item_step_num == 2) {
        get_safe_trade_mgr()->del_trade_room(trade_id);
        get_safe_trade_mgr()->release_trade_id(trade_id);
    }

    return 0;
}

/**
 * @brief player safe trade del db callback
 * @return 0 on success, -1 on error
 */
int db_safe_trade_item_callback(trader_info_t *trader, void* body, uint32_t bodylen, uint32_t ret)
{
    if (ret) {
        return send_header_to_player_2(trader->userid, &(trader->fdsess), trd_safe_trade_action_agree, ret, 1);
    }
	
    db_safe_trade_item_t *rsp = reinterpret_cast<db_safe_trade_item_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(db_safe_trade_item_t) + (rsp->item_cnt) * sizeof(trade_item_t) + 
        (rsp->attire_cnt) * sizeof(trade_attire_t));

    KDEBUG_LOG(trader->userid, "safe trade callback\t [%u %u] [%u %u %u]", rsp->action, rsp->trade_id, 
        rsp->xiaomee, rsp->item_cnt, rsp->attire_cnt);
    if (rsp->action == 1) {
        return db_safe_trade_del_callback(trader, rsp->trade_id, rsp);
    } else if (rsp->action == 2) {
        return db_safe_trade_add_callback(trader, rsp->trade_id, rsp);
    }
    return 0;
} 


