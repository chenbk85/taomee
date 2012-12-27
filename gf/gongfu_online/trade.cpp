#include <cstdio>

using namespace std;

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>

using namespace taomee;

extern "C" {
#include <glib.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <async_serv/service.h>
#include <async_serv/net_if.h>
}

#include "common_op.hpp"
#include "message.hpp"
#include "player.hpp"
#include "item.hpp"
#include "global_data.hpp"
#include "switch.hpp"

#include "mail.hpp"
#include "stat_log.hpp"


#pragma pack(1)
struct tr_sw_change_market_rsp_t {
	int32_t old_svr;
	int32_t old_market;
	int32_t change_svr;
	int32_t change_market;
};

struct decorate_rsp_t {
	uint32_t shop_id;
	uint32_t decorate_id;
};

struct change_shop_name_rsp_t {
	uint32_t shop_id;
	uint8_t  shop_name[32];
};

struct get_sell_log_item_t {
	uint32_t type;
	uint32_t itemid;
	uint32_t uniquekey;
	uint32_t item_lv;
	uint32_t cnt;
	uint32_t price;
	uint32_t sell_tm;
};

struct get_sell_log_rsp_t {
	uint32_t count;
	get_sell_log_item_t item[];
};


#pragma pack()


//-------------------------------------------------------------------

void init_trsvr_fds()
{
	for(uint32_t i = 0; i < max_trade_svr_num; ++i)
	{
		trade_fds[i] = - 1;
	}
}

/**
 * @brief load trade servers' config from btlsvr.xml file
 * @param cur the pointer to xml root node
 * @return 0 on success, -1 on error
 */
int load_trade_servers(xmlNodePtr cur)
{
	trade_svr_cnt = 0;

	memset(trade_svrs, 0, sizeof(trade_svrs));


	TRACE_LOG("load_trade_servers");
	cur = cur->xmlChildrenNode; 
	while (cur) {
		TRACE_LOG("%s", cur->name);
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Server"))) {
			if (trade_svr_cnt >= max_trade_svr_num) {
				ERROR_LOG("Too many trade servers: cnt=%d", trade_svr_cnt);
				return -1;
			}

			string svr_name;
			get_xml_prop(svr_name, cur, "SvrName");
			if (svr_name.length() >= 16) {
				ERROR_LOG("Invalid service name: %s idx=%d",
							svr_name.c_str(), trade_svr_cnt);
				return -1;
			}

			int16_t	svr_id;
			get_xml_prop(svr_id, cur, "SvrId");
			if (svr_id == 0) {
				ERROR_LOG("Invalid svr_id: %d idx=%d", svr_id, trade_svr_cnt);
				return -1;
			}
			strncpy(trade_svrs[trade_svr_cnt].svr_name, svr_name.c_str(), sizeof(trade_svrs[trade_svr_cnt].svr_name));
			TRACE_LOG("load trade name:%s id:%u", svr_name.c_str(), svr_id);
			trade_svrs[trade_svr_cnt].id = svr_id;
			TRACE_LOG("%d", trade_svrs[trade_svr_cnt].id);
			++trade_svr_cnt;
		}
		cur = cur->next;
	} 

	return 0;
}

/**
 * @brief reload trade svr config 
 * 
 */

void reload_trade_svr_config()
{
	trade_server_t former_svrs[max_trade_svr_num];

	memcpy(former_svrs, trade_svrs, sizeof(trade_svrs));


	try {
		load_xmlconf("./conf/trsvr.xml", load_trade_servers);
	} catch (const exception& e) {
		INFO_LOG("Fail to Reload 'btlsvr.xml'. Err: %s", e.what());
		return;
	}

	for (size_t i = 0; i < array_elem_num(trade_fds); ++i) {
		if ( strcmp(former_svrs[i].svr_name, trade_svrs[i].svr_name)
				|| (former_svrs[i].id != trade_svrs[i].id) ) {
			KDEBUG_LOG(0, "RELOAD TR SVR\t[idx=%llu name=%s %s id=%d %d]",
						static_cast<unsigned long long>(i),
						trade_svrs[i].svr_name, former_svrs[i].svr_name,
						trade_svrs[i].id, former_svrs[i].id);
			close(trade_fds[i]);
			trade_fds[i] = -1;
		}
	}
}

int get_trsvr_fd_idx(int fd)
{
	for (uint32_t i = 0; i < trade_svr_cnt; ++i) {
		if (trade_fds[i] == fd) {
			return  i;
		}
	}
	return -1;
}

int get_trsvr_fd_idx_by_id(int server_id)
{
	for (uint32_t i = 0; i < trade_svr_cnt; ++i) {
		if (trade_svrs[i].id ==  (uint16_t)server_id) {
			return  i;
		}
	}
	return -1;
}


int send_to_trade_svr(player_t* p, const void* pkgbuf, uint32_t len, int index)
{
	if (trade_fds[index] == -1) {
		trade_fds[index] = connect_to_service(trade_svrs[index].svr_name, trade_svrs[index].id, 65535, 1); 
	}

	if ((trade_fds[index] == -1) || (len > 8192 )) {
		ERROR_LOG("send to trade server failed: fd=%d len=%d", trade_fds[index], len);
		/*
		assert(p->trade_grp);

		free_trade_grp(p->trade_grp);
		p->trade_grp = 0;

		return send_header_to_player(p, cli_proto_trade_end, cli_err_trade_system_err, 1);
		*/
		return 0;
	}
	tr_proto_t* p_tr_head = (tr_proto_t*)pkgbuf;
	TRACE_LOG("index=%d fd=%d p=%u len=%d, cmd=%u", index, trade_fds[index], p ? p->id: 0, len, p_tr_head->cmd);

    return net_send(trade_fds[index], pkgbuf, len);
}

#define SET_TR_HANDLE(op_, len_, cmp_) \
		do { \
			if (trsvr_handles[tr_ ## op_ - tr_cmd_start].hdlr != 0) { \
				ERROR_LOG("duplicate cmd=%u name=%s", tr_ ## op_, #op_); \
				return false; \
			} \
			trsvr_handles[tr_ ## op_ - tr_cmd_start] = trsvr_handle_t(trsvr_ ## op_ ## _callback, len_, cmp_); \
		} while (0)

bool init_tr_proto_handles()
{
	memset(trsvr_handles, 0, sizeof(trsvr_handles));
	SET_TR_HANDLE(player_enter_market, 8, cmp_must_ge);
	SET_TR_HANDLE(player_leave_market, 0, cmp_must_eq);
	SET_TR_HANDLE(player_change_market, 8, cmp_must_eq);
	SET_TR_HANDLE(player_setup_shop, 12, cmp_must_eq);
	SET_TR_HANDLE(player_update_shop, 0, cmp_must_eq);
	SET_TR_HANDLE(player_buy_goods, sizeof(buy_goods_rsp_t), cmp_must_eq);
	SET_TR_HANDLE(player_sell_goods, sizeof(sell_goods_rsp_t), cmp_must_eq);
	SET_TR_HANDLE(player_get_shop_list, 4, cmp_must_ge);
	SET_TR_HANDLE(player_shop_detail, 4, cmp_must_ge);
	SET_TR_HANDLE(player_pause_shop, 4, cmp_must_eq);
	SET_TR_HANDLE(player_drop_shop, 8, cmp_must_eq);
	SET_TR_HANDLE(player_decorate_shop, 8, cmp_must_eq);
	SET_TR_HANDLE(player_change_shop_name, 36, cmp_must_eq);

    //safe trade
	SET_TR_HANDLE(safe_trade_create_room, 12, cmp_must_eq);
	SET_TR_HANDLE(safe_trade_join_room, 16, cmp_must_eq);
	SET_TR_HANDLE(safe_trade_cancel, 4, cmp_must_eq);
	SET_TR_HANDLE(safe_trade_set_item, sizeof(safe_trade_set_item_rsp_t), cmp_must_ge);
	SET_TR_HANDLE(safe_trade_action_agree, sizeof(safe_trade_action_agree_t), cmp_must_ge);
	return true;
}


void handle_trade_svr_return(tr_proto_t * data, int len, int tr_fd)
{
	TRACE_LOG("pkg form trade svr %u %u %u %u", data->id, data->cmd, data->len, data->ret);
	player_t* p = get_player(data->id);
	if (p && data->cmd == tr_transmit_only){
		send_to_player(p, data->body, data->len - sizeof(tr_proto_t), 0);
		return;
	}

	if (p == 0) {
		return;
	}
	if (data->cmd != tr_player_enter_market && data->cmd < tr_safe_trade_create_room) {
		if ((p->trade_grp == 0) || (trade_fds[p->trade_grp->fd_idx] != tr_fd)) {
			ERROR_LOG("unknow err %u %u %u %u %p %u", data->id, data->cmd, data->len, data->ret, p->trade_grp, tr_fd);
			return;
		}
	}
	if (data->ret) {
        ERROR_LOG("return non-zero %u %u %u %u", data->id, data->cmd, data->len, data->ret);
	   	send_header_to_player(p, p->waitcmd, data->ret, 1);
		return;
	}

	int err = -1;
	uint32_t bodylen = data->len - sizeof(tr_proto_t);

	int cmd = data->cmd - tr_cmd_start;

	if ( ((trsvr_handles[cmd].cmp_method == cmp_must_eq) && (bodylen != trsvr_handles[cmd].len))
			|| ((trsvr_handles[cmd].cmp_method == cmp_must_ge) && (bodylen < trsvr_handles[cmd].len)) ) {
		if ((bodylen != 0) || (data->ret == 0)) {
			ERROR_LOG("invalid package len=%u %u cmpmethod=%d ret=%u uid=%u cmd = %u",
						bodylen, trsvr_handles[cmd].len,
						trsvr_handles[cmd].cmp_method, data->ret, data->id, data->cmd);
			goto ret;
		}
	}

	err = trsvr_handles[cmd].hdlr(p, data);

ret:
	if (err) {
		close_client_conn(p->fd);
	}
}


/*
void tmp_func(player_t* p)
{
	if (p->trade_grp && p->trade_grp->sell_cnt) {
		uint32_t have_count = p->my_packs->get_item_cnt(1500302);
		uint32_t sell_cnt = p->trade_grp->sell_cnt;
		uint32_t send_cnt = 0;
		if (have_count >= 50) {
			send_cnt = 0;
		} else {
			if (sell_cnt + have_count > 50) {
				send_cnt = 20 - have_count;
			} else {
				send_cnt = sell_cnt;
			}
		}

		send_add_item_mail(p, 1500302, send_cnt);
		p->trade_grp->sell_cnt = 0;
	}

}

void tmp_func(player_t* p)
{
	uint32_t daily_id = 999;
	if (p->trade_grp && p->trade_grp->sell_cnt && p->daily_restriction_count_list[daily_id - 1] < 50) {
		uint32_t have_count = p->daily_restriction_count_list[daily_id - 1];
		uint32_t sell_cnt = p->trade_grp->sell_cnt;
		uint32_t limit_cnt = 50;
		uint32_t send_cnt = 0;
		if (have_count >= limit_cnt) {
			send_cnt = 0;
		} else {
			if (sell_cnt + have_count > limit_cnt) {
				send_cnt = limit_cnt - have_count;
			} else {
				send_cnt = sell_cnt;
			}
		}
		send_cnt = send_cnt > 50 ? 50 : send_cnt;
		if (send_cnt) {
			send_add_item_mail(p, 1500302, send_cnt);
			for (uint32_t i = 0; i < send_cnt; i++) {
				int idx = 0;
				pack_h(dbpkgbuf, daily_id, idx);
				pack_h(dbpkgbuf, 0, idx);
				pack_h(dbpkgbuf, 50, idx);
				send_request_to_db(0, p->id, p->role_tm, dbproto_add_action_count, dbpkgbuf, idx);
				p->daily_restriction_count_list[daily_id - 1] ++;
			}
		}
		p->trade_grp->sell_cnt = 0;
	}

} */


/** 
 * @brief pack usr info form enter trade svr market
 * 
 * @param buf 
 * @param p
 * @param market_id
 * 
 * @return 
 */
static int pack_enter_trade_info(uint8_t *buf, player_t *p, uint32_t market_id)
{
	int idx = 0;
	pack_h(buf, market_id, idx);
	pack_h(buf, p->id,idx);
	pack_h(buf, p->role_tm, idx);
	pack_h(buf, p->role_type, idx);
	pack_h(buf, p->power_user, idx);
	pack_h(buf, p->player_show_state, idx);
	pack_h(buf, p->vip, idx);
	pack_h(buf, p->vip_level, idx);
	pack_h(buf, p->using_achieve_title, idx);
	pack(buf, p->nick, max_nick_size, idx);
	pack_h(buf, p->lv, idx);
	pack_h(buf, p->flag, idx);
	pack_h(buf, p->app_mon, idx);
	pack_h(buf, p->honor, idx);
	pack_h(buf, p->coins, idx);
	pack_h(buf, p->xpos, idx);
	pack_h(buf, p->ypos, idx);
	pack_h(buf, p->clothes_num, idx);
	pack_h(buf, p->summon_mon_num, idx);
	pack_h(buf, p->fight_summon ? p->fight_summon->mon_tm : 0, idx);
	for(uint32_t i = 0; i < p->clothes_num; ++i)
	{
		pack_h(buf, p->clothes[i].clothes_id, idx);
		pack_h(buf, p->clothes[i].gettime, idx);
		pack_h(buf, p->clothes[i].timelag, idx);
		pack_h(buf, static_cast<uint32_t>(p->clothes[i].lv), idx);
	}
	for (uint32_t i = 0; i < p->summon_mon_num; ++i)
	{
		pack_h(buf, p->summons[i].mon_type, idx);
		pack_h(buf, p->summons[i].mon_tm, idx);
		pack_h(buf, p->summons[i].lv, idx);
		pack(buf, p->summons[i].nick, max_nick_size, idx);
		pack_h(buf, p->summons[i].call_flag, idx);
		TRACE_LOG("%u %u %u %u", p->id, p->summon_mon_num, p->summons[i].mon_type, p->summons[i].mon_tm);
	}


	return idx;
}

static int pack_trd_update_shop(uint8_t* buf, player_t *p, uint32_t shop_id, TradeItemMap& item_in_map, TradeAttireMap& attire_in_map)
{	
	int idx = 0;
	uint32_t item_cnt = item_in_map.size();
	uint32_t attire_cnt = attire_in_map.size();
	pack_h(buf, p->trade_grp->market_id, idx);
	pack_h(buf, shop_id, idx);
	pack_h(buf, item_cnt, idx);
	pack_h(buf, attire_cnt, idx);

	TradeItemMap::iterator item_it = item_in_map.begin();
	for ( ; item_it != item_in_map.end(); ++item_it) {
		pack_h(buf, item_it->second.grid_idx, idx);
		pack_h(buf, item_it->second.item_id, idx);
		pack_h(buf, item_it->second.item_cnt, idx);
		pack_h(buf, item_it->second.price, idx);
	}
	TradeAttireMap::iterator attire_it = attire_in_map.begin();
	for ( ; attire_it != attire_in_map.end(); ++attire_it) {
		pack_h(buf, attire_it->second.grid_idx, idx);
		pack_h(buf, attire_it->second.attire_id, idx);
		pack_h(buf, attire_it->second.unique_id, idx);
		pack_h(buf, (uint32_t)(p->my_packs->get_clothes_lv_by_id(attire_it->second.unique_id)), idx);
		pack_h(buf, attire_it->second.duration, idx);
		pack_h(buf, attire_it->second.price, idx);
		TRACE_LOG("%u %u %u %u %u", attire_it->second.grid_idx, attire_it->second.attire_id, attire_it->second.unique_id, attire_it->second.duration, attire_it->second.price);
	}
	return idx;
}

inline void init_player_trade_grp(player_t *p, uint32_t server_id, uint32_t market_id)
{
	if (p->trade_grp == 0) {
		p->trade_grp = alloc_trade_grp();
	}
	p->trade_grp->market_id = market_id;
	p->trade_grp->server_id = server_id;
}

int trade_enter_market_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	if (!is_in_right_hour(6, 24)) {
		return send_header_to_player(p, p->waitcmd, cli_err_enter_market_err_tm, 1);
	}
	/*
	if (!is_market_activity(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_market_limit_enter,1);
	} */
	if (p->battle_grp) {
		ERROR_LOG("can not enter trade in btl %u", p->id);
		return -1;
	}


	if (p->trade_grp == 0)
	{
		
		KDEBUG_LOG(p->id, "FIRST ENTER MARKET\t[%u]", p->id);
		if (recommend_market(p) < 0)
		{
			return send_header_to_player(p, p->waitcmd, cli_err_system_error,1);
		}
		return 0;
	} else {
		int idx = 0;
		uint32_t c_market_id = 0;
		unpack(body, c_market_id, idx);
		//c_market_id = c_market_id >= max_market_num ? c_market_id : c_market_id + max_market_num;
		if (c_market_id <= 0) {
			return send_header_to_player(p, p->waitcmd, cli_err_trade_err_market,1);
		}

		uint32_t server_id = ( (c_market_id - 1) / max_market_num ) + 1; 
		uint32_t market_id = (c_market_id - 1 ) % max_market_num;
		KDEBUG_LOG(p->id, "CHANGE MARKET\t[%u %u :old=%u %u new=%u %u]", p->id, c_market_id, 
				p->trade_grp->server_id, p->trade_grp->market_id, server_id, market_id);
		if ((p->trade_grp->server_id == server_id) && (p->trade_grp->market_id == market_id ))
		{
			int idx = sizeof(cli_proto_t);
			
			pack(trpkgbuf, c_market_id, idx);
			init_cli_proto_head(trpkgbuf, p, p->waitcmd, idx);
			return send_to_player(p, trpkgbuf, idx, 1); 
		}

		if (trade_sw_change_market(p, server_id, market_id) < 0)
		{
			return send_header_to_player(p, p->waitcmd, cli_err_system_error,1);
		}
		return 0;
	}
		
}


int trade_sw_recommend_market_callback(player_t *p, battle_switch_proto_t *data, uint32_t len)
{
	uint32_t svr_id = 0;
	uint32_t market_id = 0;
	int idx =0;
	unpack_h(data->body, svr_id, idx);
	unpack_h(data->body, market_id, idx);
	//svr_id = 6;
	KDEBUG_LOG(p->id, "RECOMM MARKET CB\t[%u %u]", svr_id, market_id);
	if (svr_id > 0 && market_id >= 0) {
		int tr_idx = get_trsvr_fd_idx_by_id(svr_id);
		//if (p->waitcmd == tr_player_enter_market)
		return trsvr_player_enter_market(p, market_id, tr_idx);
	} else {
		WARN_LOG("THE SVR AND MARKET RECOMM ERROR \t[%u %u %u]", p->id, svr_id, market_id);
		send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);	
		return 0;
	}
}


int trade_sw_change_market_callback(player_t *p, battle_switch_proto_t *data, uint32_t len)
{
	tr_sw_change_market_rsp_t * info = reinterpret_cast<tr_sw_change_market_rsp_t *>(data->body);

	if (p->waitcmd == cli_proto_trade_enter_aim_market) {
		KDEBUG_LOG(p->id, "ENTER AIM MARKET CHECK SW CB\t[ %u ]", p->id);
		//read aim server and market
		int server_id = 0;
		int market_id = 0;
		int ix = 0;
		unpack_h(p->session, server_id, ix);
		unpack_h(p->session, market_id, ix);
		if (info->change_svr != server_id || info->change_market != market_id) {
			return 	send_header_to_player(p, p->waitcmd, cli_err_trade_market_full, 1);
		} else {
			int idx = sizeof(cli_proto_t);
			int c_market_id = (server_id - 1) * max_market_num + market_id  + 1;
			pack(trpkgbuf, c_market_id, idx);
			init_cli_proto_head(trpkgbuf, p, p->waitcmd, idx);
			return send_to_player(p, trpkgbuf, idx, 1); 
		}
	}

	KDEBUG_LOG(p->id, "CHANGE MARKET SW CB\t[%u old:%u %u new:%u %u]", p->id, info->old_svr, info->old_market, info->change_svr, info->change_market);

	if (info->change_svr > 0 && info->change_market >= 0)
	{
		if (p->waitcmd == cli_proto_trade_enter_market) {
			if ( p->trade_grp->server_id == (uint32_t)info->change_svr) {
				return trsvr_player_change_market(p, info->change_market);
			} else {
				//store tmp info
				int idx = 0;
				pack_h(p->session, info->change_svr, idx);
			    pack_h(p->session, info->change_market, idx);	
				return trsvr_player_leave_market(p);
			}
		}
		return 0;
	} 
	else {
		return send_header_to_player(p, 2000, 0, 1);
		//return send_header_to_player(p, p->waitcmd, cli_err_trade_market_full, 1);	
	}
}

int trade_list_shop_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	if (p->trade_grp)
	{
		return trsvr_player_get_shop_list(p);
	}
	else {
		WARN_LOG("player %u is not in a market", p->id);
		return send_header_to_player(p, p->waitcmd, cli_err_trade_market_full, 1);
	}
}

int trade_shop_detail_cmd(player_t *p, uint8_t *body, uint32_t bodylen)
{
	uint32_t shop_id = 0;
	int ix = 0;
	unpack(body, shop_id, ix);
	KDEBUG_LOG(p->id, "SHOP DETAIL\t[%u %u]", p->id, shop_id);
	if ( p->trade_grp) {
		int idx = sizeof(tr_proto_t);
		pack_h(trpkgbuf, shop_id, idx);
		init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_shop_detail);
		return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx); 
	} else {
		return send_header_to_player(p, p->waitcmd, cli_err_trade_market_full, 1);
	}
}

int trade_setup_shop_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	uint32_t shop_id = 0;
	int idx = 0;
	unpack(body, shop_id, idx);

	if (!is_in_right_hour(6, 24)) {
		return send_header_to_player(p, p->waitcmd, cli_err_enter_market_err_tm, 1);
	}

	if (p->coins > 1000000000) {
		return send_header_to_player(p, p->waitcmd, cli_err_coins_too_much, 1);
	}

	if (!is_player_checked_pay_passwd(p)) {
		ERROR_LOG("%u havnot check pay passwd", p->id);
		return -1;
	}
	
    //money
    if ( !is_enough_to_buy(p, setup_shop_cost, 1) ) {
        TRACE_LOG("player money is not enough. uid=[%u] [%u %u 1]", p->id, setup_shop_cost, p->coins);
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
	}

	if (p->trade_grp)  {
		if (p->trade_grp->trade_type)
		{
			return send_header_to_player(p, p->waitcmd, cli_err_trade_shop_busy, 1);
		}
		return	trsvr_player_setup_shop(p, shop_id);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_trade_not_in_market, 1);
}

int trade_leave_market_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	if (p->trade_grp)
	{
		uint32_t map_id = 0;
		int idx = 0;
		unpack(body, map_id, idx);
		if ( map_id < 0 || map_id > 28) {
			map_id = 12;
		}
		idx = 0;
		pack_h(p->session, map_id, idx);
		return trsvr_player_leave_market(p);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_trade_not_in_market, 1); 
}

int trade_drop_shop_cmd(player_t *p, uint8_t *body, uint32_t bodylen)
{
	uint32_t shop_id = 0;
	int idx = 0;
	unpack(body, shop_id, idx);
	if (p->trade_grp != 0 && p->trade_grp->trade_type)
	{
		int ifx = sizeof(tr_proto_t);
		pack_h(trpkgbuf, shop_id, ifx);
		init_tr_proto_head(p, p->id, trpkgbuf, ifx, tr_player_drop_shop);
		return send_to_trade_svr(p, trpkgbuf, ifx, p->trade_grp->fd_idx);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_trade_not_a_shopkeeper, 1);
}

bool goods_tradable_for_player(player_t *p, const Item * goods)
{
	if (is_vip_player(p)) {
		return goods->is_tradable_for_vip();
	} else {
		return goods->is_tradable_for_all();
	}
}

int send_update_shop_info_to_trade(player_t* p) 
{
	int idx = 0;
	uint32_t itm_cnt;
	uint32_t shop_id;
	uint32_t attire_cnt;
	TradeItemMap item_in_map;
	TradeItemMap grid_item_in_map;
	TradeAttireMap attire_in_map;
	
	uint8_t* body = (uint8_t*)(p->tmp_session);
	unpack(body, shop_id, idx);
	unpack(body, itm_cnt, idx);
	unpack(body, attire_cnt, idx);

	if (!(p->trade_grp) || !(p->trade_grp->trade_type)) { 
		return send_header_to_player(p, p->waitcmd, cli_err_trade_not_in_market, 1);
	}

    KDEBUG_LOG(p->id, "UPDATE SHOP\t[uid=%u shopid=%u cnt=%u %u]", p->id, shop_id, itm_cnt, attire_cnt);


	for (uint32_t i = 0; i < itm_cnt; i++) {
		uint32_t item_id;
		uint32_t item_cnt;
		uint32_t grid_idx;
		uint32_t price;
		unpack(body, grid_idx, idx);
		unpack(body, item_id, idx);
		unpack(body, item_cnt, idx);
		unpack(body, price, idx);

		const GfItem* itm = items->get_item(item_id);
		if (!itm || item_cnt > max_one_grid_itm_cnt || 
			!(item_cnt) || 
			grid_idx > max_trd_grid_cnt ||
			!price) {
			WARN_LOG("%u buy not exist %u %u %u", p->id, grid_idx, item_id, item_cnt);
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}

		
		KDEBUG_LOG(p->id, "UPDATE ITEM\t[%u %u %u %u %u]", p->id, grid_idx, item_id, item_cnt, price);
		
		uint32_t have_cnt = 0;
		if (is_strengthen_material(item_id)) {
			have_cnt = p->my_packs->get_material_cnt(item_id);
		} else {
			have_cnt = p->my_packs->get_item_cnt(item_id);
		}
		if (item_cnt > have_cnt) {
			ERROR_LOG("update shop err: pid=%u itm=%u cnt:%u > %u", p->id, item_id, item_cnt, have_cnt);
			return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
		}
		
		TradeItemMap::iterator it = item_in_map.find(item_id);
		TradeItemMap::iterator grid_it = grid_item_in_map.find(grid_idx);
		const Item * goods_item = items->get_item(item_id);
		if (goods_item == 0) {
			ERROR_LOG("NOT EXIT ITEM\t[%u %u]", p->id, item_id);
			return -1;
		}

		if (!goods_tradable_for_player(p, goods_item)) {
			WARN_LOG("player set untradable item in shop [%u %u]", p->id, item_id);
			return send_header_to_player(p, p->waitcmd, cli_err_untradable_item, 1);
		}

		if (price <=0   && price > max_goods_price)
		{
			WARN_LOG("the goods's price error \t [%u %u %u]", p->id, item_id, price);
			return send_header_to_player(p, p->waitcmd, cli_err_goods_price_error, 1);
		}

		if (it != item_in_map.end()) {
			it->second.item_cnt += item_cnt;
		} else {
			item_in_map.insert(TradeItemMap::value_type(item_id, trade_update_shop_item_in_t(grid_idx, item_id, item_cnt, price)));
		}
		if (grid_it != grid_item_in_map.end()) {
			ERROR_LOG("update shop err: pid=%u have two same item=%u cnt=%u", p->id, grid_it->second.item_id, it->second.item_cnt);
			return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
  		} else {
  			grid_item_in_map.insert(TradeItemMap::value_type(grid_idx, trade_update_shop_item_in_t(grid_idx, item_id, item_cnt, price)));
  		}
	}
	TradeItemMap::iterator item_it = item_in_map.begin();
	for (; item_it != item_in_map.end(); ++item_it) {
		uint32_t have_cnt = 0;
		if (is_strengthen_material(item_it->second.item_id)) {
			have_cnt = p->my_packs->get_material_cnt(item_it->second.item_id);
		} else {
			have_cnt = p->my_packs->get_item_cnt(item_it->second.item_id);
		}
		
		if (item_it->second.item_cnt > have_cnt) {
			ERROR_LOG("update shop err: pid=%u itm=%u cnt:%u > %u", p->id, item_it->second.item_id, item_it->second.item_cnt, have_cnt);
			return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
		}
	}
	for (uint32_t i = 0; i < attire_cnt; i++) {
		uint32_t attire_id;
		uint32_t unique_id;
		uint32_t grid_idx;
		uint32_t price;
		unpack(body, grid_idx, idx);
		unpack(body, attire_id, idx);
		unpack(body, unique_id, idx);
		unpack(body, price, idx);

		
		if (!unique_id || !price || !attire_id || grid_idx > max_trd_grid_cnt) {
			WARN_LOG("%u buy not exist %u %u %u", p->id, attire_id, unique_id, price);
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}

		KDEBUG_LOG(p->id, "UPDATE ATTIRE\t[%u %u %u %u %u]", p->id, grid_idx, attire_id, unique_id, price);
		TradeAttireMap::iterator it = attire_in_map.find(unique_id);
		if (it != attire_in_map.end()) {
			ERROR_LOG("update shop err: pid=%u have two same atire=%u unique=%u", p->id, it->second.attire_id, it->second.unique_id);
			return -1;
		} else {
			if (!(p->my_packs->is_clother_item_exist(unique_id, attire_id))) {
				ERROR_LOG("update shop err: pid=%u not have atire=%u unique=%u", p->id, attire_id, unique_id);
				return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
			}

			pack_clothes_info_t *clothes = p->my_packs->get_clothes_info_by_unique_id(unique_id);
			if (clothes && clothes->lv >= 8) {
				ERROR_LOG("SELL ITEM_ATTIRE LV TOO HIGH [%u %u %u]", p->id, clothes->clothes_id,
						clothes->lv);
				return send_header_to_player(p, p->waitcmd, cli_err_untradable_item, 1);
			}
			uint32_t duration = p->my_packs->get_clothes_duration(unique_id);
			attire_in_map.insert(TradeAttireMap::value_type(unique_id, trade_update_shop_attire_in_t(grid_idx, attire_id, unique_id, duration, price)));
		}

	}

	// send cmd to trdserver
	idx = sizeof(tr_proto_t);
	idx += pack_trd_update_shop(trpkgbuf + idx, p, shop_id, grid_item_in_map, attire_in_map);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_update_shop);

	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx);

}

int trade_update_shop_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t itm_cnt;
	uint32_t shop_id;
	uint32_t attire_cnt;
	TradeItemMap item_in_map;
	TradeItemMap grid_item_in_map;
	TradeAttireMap attire_in_map;

	unpack(body, shop_id, idx);
	unpack(body, itm_cnt, idx);
	unpack(body, attire_cnt, idx);

	if (!(p->trade_grp) || !(p->trade_grp->trade_type)) { 
		return send_header_to_player(p, p->waitcmd, cli_err_trade_not_in_market, 1);
	}

    KDEBUG_LOG(p->id, "UPDATE SHOP\t[uid=%u shopid=%u cnt=%u %u]", p->id, shop_id, itm_cnt, attire_cnt);
	CHECK_VAL_EQ(bodylen, 12 + itm_cnt * 16 + attire_cnt* 16);
	if (bodylen >= max_tmp_session_len) {
		ERROR_LOG("update shop err max: %u %u %u %u\n", p->id, bodylen, itm_cnt, attire_cnt);
		return -1;
	}
	int idx_body_len = 0;
	pack(p->tmp_session, body, bodylen, idx_body_len);


	for (uint32_t i = 0; i < itm_cnt; i++) {
		uint32_t item_id;
		uint32_t item_cnt;
		uint32_t grid_idx;
		uint32_t price;
		unpack(body, grid_idx, idx);
		unpack(body, item_id, idx);
		unpack(body, item_cnt, idx);
		unpack(body, price, idx);

		const GfItem* itm = items->get_item(item_id);
		if (!itm || item_cnt > max_one_grid_itm_cnt || 
			!(item_cnt) || 
			grid_idx > max_trd_grid_cnt ||
			!price) {
			WARN_LOG("%u buy not exist %u %u %u", p->id, grid_idx, item_id, item_cnt);
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}

		
		KDEBUG_LOG(p->id, "UPDATE ITEM\t[%u %u %u %u %u]", p->id, grid_idx, item_id, item_cnt, price);
		
		uint32_t have_cnt = 0;
		if (is_strengthen_material(item_id)) {
			have_cnt = p->my_packs->get_material_cnt(item_id);
		} else {
			have_cnt = p->my_packs->get_item_cnt(item_id);
		}
		if (item_cnt > have_cnt) {
			ERROR_LOG("update shop err: pid=%u itm=%u cnt:%u > %u", p->id, item_id, item_cnt, have_cnt);
			return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
		}
		
		TradeItemMap::iterator it = item_in_map.find(item_id);
		TradeItemMap::iterator grid_it = grid_item_in_map.find(grid_idx);
		const Item * goods_item = items->get_item(item_id);
		if (goods_item == 0) {
			ERROR_LOG("NOT EXIT ITEM\t[%u %u]", p->id, item_id);
			return -1;
		}

		if (!goods_item->is_tradable()) {
			WARN_LOG("player set untradable item in shop [%u %u]", p->id, item_id);
			return send_header_to_player(p, p->waitcmd, cli_err_untradable_item, 1);
		}

		if (price <=0 || price > max_goods_price)
		{
			WARN_LOG("the goods's price error \t [%u %u %u]", p->id, item_id, price);
			return send_header_to_player(p, p->waitcmd, cli_err_goods_price_error, 1);
		}

		if (it != item_in_map.end()) {
			it->second.item_cnt += item_cnt;
		} else {
			item_in_map.insert(TradeItemMap::value_type(item_id, trade_update_shop_item_in_t(grid_idx, item_id, item_cnt, price)));
		}
		if (grid_it != grid_item_in_map.end()) {
			ERROR_LOG("update shop err: pid=%u have two same item=%u cnt=%u", p->id, grid_it->second.item_id, it->second.item_cnt);
			return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
  		} else {
  			grid_item_in_map.insert(TradeItemMap::value_type(grid_idx, trade_update_shop_item_in_t(grid_idx, item_id, item_cnt, price)));
  		}
	}
	TradeItemMap::iterator item_it = item_in_map.begin();
	for (; item_it != item_in_map.end(); ++item_it) {
		uint32_t have_cnt = 0;
		if (is_strengthen_material(item_it->second.item_id)) {
			have_cnt = p->my_packs->get_material_cnt(item_it->second.item_id);
		} else {
			have_cnt = p->my_packs->get_item_cnt(item_it->second.item_id);
		}
		
		if (item_it->second.item_cnt > have_cnt) {
			ERROR_LOG("update shop err: pid=%u itm=%u cnt:%u > %u", p->id, item_it->second.item_id, item_it->second.item_cnt, have_cnt);
			return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
		}
	}
	for (uint32_t i = 0; i < attire_cnt; i++) {
		uint32_t attire_id;
		uint32_t unique_id;
		uint32_t grid_idx;
		uint32_t price;
		unpack(body, grid_idx, idx);
		unpack(body, attire_id, idx);
		unpack(body, unique_id, idx);
		unpack(body, price, idx);

		
		if (!unique_id || !price || !attire_id || grid_idx > max_trd_grid_cnt) {
			WARN_LOG("%u buy not exist %u %u %u", p->id, attire_id, unique_id, price);
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}

		KDEBUG_LOG(p->id, "UPDATE ATTIRE\t[%u %u %u %u %u]", p->id, grid_idx, attire_id, unique_id, price);
		TradeAttireMap::iterator it = attire_in_map.find(unique_id);
		if (it != attire_in_map.end()) {
			ERROR_LOG("update shop err: pid=%u have two same atire=%u unique=%u", p->id, it->second.attire_id, it->second.unique_id);
			return -1;
		} else {
			if (!(p->my_packs->is_clother_item_exist(unique_id, attire_id))) {
				ERROR_LOG("update shop err: pid=%u not have atire=%u unique=%u", p->id, attire_id, unique_id);
				return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
			}

			pack_clothes_info_t *clothes = p->my_packs->get_clothes_info_by_unique_id(unique_id);
			if (clothes && clothes->lv >= 8) {
				ERROR_LOG("SELL ITEM_ATTIRE LV TOO HIGH [%u %u %u]", p->id, clothes->clothes_id,
						clothes->lv);
				return send_header_to_player(p, p->waitcmd, cli_err_untradable_item, 1);
			}
			uint32_t duration = p->my_packs->get_clothes_duration(unique_id);
			attire_in_map.insert(TradeAttireMap::value_type(unique_id, trade_update_shop_attire_in_t(grid_idx, attire_id, unique_id, duration, price)));
		}

	}
	idx = 0;
	pack_h(dbpkgbuf, (uint32_t)item_in_map.size(), idx);
	pack_h(dbpkgbuf, (uint32_t)attire_in_map.size(), idx);
	TradeItemMap::iterator item_it_cur = item_in_map.begin();
	for ( ; item_it_cur != item_in_map.end(); ++item_it_cur) {
		pack_h(dbpkgbuf, item_it_cur->second.item_id, idx);
		pack_h(dbpkgbuf, item_it_cur->second.item_cnt, idx);
	}
	TradeAttireMap::iterator attire_it_cur = attire_in_map.begin();
	for ( ; attire_it_cur != attire_in_map.end(); ++attire_it_cur) {
		pack_h(dbpkgbuf, attire_it_cur->second.attire_id, idx);
		pack_h(dbpkgbuf, attire_it_cur->second.unique_id, idx);
	}

	return send_request_to_db(p, p->id, p->role_tm, dbproto_check_update_trade_info, dbpkgbuf, idx);
}

int db_check_update_trade_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	if (ret) {
		ERROR_LOG("check update trade info error %u %u", p->id, ret);
		return -1;
	}
	return send_update_shop_info_to_trade(p);
}

/**
 * @brief player trade action cmd
 */
int trade_buy_goods_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    if (p->trade_grp == NULL) {
        return -1;
    }
    int u_idx = 0;
    trade_action_t tr_obj = {0};
    unpack(body, tr_obj.shop_id, u_idx);
    unpack(body, tr_obj.grid_id, u_idx);
    unpack(body, tr_obj.item_id, u_idx);
    unpack(body, tr_obj.uniquekey, u_idx);
    unpack(body, tr_obj.cnt, u_idx);
    unpack(body, tr_obj.coins, u_idx);

	
	const GfItem* itm = items->get_item(tr_obj.item_id);
	if (!itm || tr_obj.cnt > max_one_grid_itm_cnt || !(tr_obj.cnt) || tr_obj.grid_id > max_trd_grid_cnt) {
		WARN_LOG("%u buy not exist %u %u %u", p->id, tr_obj.grid_id, tr_obj.item_id, tr_obj.cnt);
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
	if (tr_obj.uniquekey && tr_obj.cnt != 1) {
		WARN_LOG("%u buy not exist %u %u", p->id, tr_obj.item_id, tr_obj.cnt);
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
	
	KDEBUG_LOG(p->id, "BUY GOODS\t[uid=%u shopid=%u grid_id=%u itemid=%u unique=%u cnt=%u coins=%u]", 
		p->id, tr_obj.shop_id, tr_obj.grid_id, tr_obj.item_id, tr_obj.uniquekey, tr_obj.cnt, tr_obj.coins);
    //money
    if ( !is_enough_to_buy(p, tr_obj.coins, tr_obj.cnt) ) {
        TRACE_LOG("player money is not enough. uid=[%u] [%u %u %u]",p->id,p->coins,tr_obj.coins,tr_obj.cnt);
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
	}

	if (tr_obj.uniquekey) { 
		uint32_t bags = p->my_packs->get_remain_clothes_bag_grid(p);
		if (!bags) {
			TRACE_LOG("player's bag is no space for goods uid=%u goodsid=%u cnt=%u", p->id, tr_obj.item_id, tr_obj.cnt);
			return send_header_to_player(p, p->waitcmd,  cli_err_bag_no_space, 1);
		}
	} else if ( !p->my_packs->check_enough_bag_item_grid(p, tr_obj.item_id, tr_obj.cnt)) {
		TRACE_LOG("player's bag is no space for goods uid=%u goodsid=%u cnt=%u", p->id, tr_obj.item_id, tr_obj.cnt);
		return send_header_to_player(p, p->waitcmd,  cli_err_bag_no_space, 1);
	}
    //package
    uint32_t max_bag = get_player_total_item_bag_grid_count(p);
    if (tr_obj.uniquekey != 0) {
        if ( p->my_packs->all_clothes_cnt() + 1 > max_bag ) {
            return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
        }
    } else {
        if ( p->my_packs->all_items_cnt() + 1 > max_bag ) {
            return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
        }
    }
	
    int idx = sizeof(tr_proto_t);
    pack_h(trpkgbuf, tr_obj.shop_id, idx);
    pack_h(trpkgbuf, tr_obj.grid_id, idx);
    pack_h(trpkgbuf, tr_obj.cnt, idx);
    pack_h(trpkgbuf, tr_obj.item_id, idx);
    pack_h(trpkgbuf, tr_obj.uniquekey, idx);
    pack_h(trpkgbuf, tr_obj.coins, idx);
    pack_h(trpkgbuf, max_bag, idx);

    init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_buy_goods);
    return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx);
}




int trade_decorate_shop_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	uint32_t shop_id =0;
	uint32_t decorate_id = 0;
	int idx = 0;
	unpack(body, shop_id, idx);
	unpack(body, decorate_id, idx);
	if (p->trade_grp != 0 && p->trade_grp->trade_type)
	{
		TRACE_LOG("player want to decorate shop[%u %u %u]", p->id, shop_id, decorate_id);
		int idx = sizeof(tr_proto_t);
		pack_h(trpkgbuf, shop_id, idx);
		pack_h(trpkgbuf, decorate_id, idx);
		init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_decorate_shop); 
		return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx);
	}	
	return send_header_to_player(p, p->waitcmd, cli_err_trade_not_in_market, 1); 
}

int trsvr_player_decorate_shop_callback(player_t *p, tr_proto_t * data)
{
	decorate_rsp_t * pkg = reinterpret_cast<decorate_rsp_t*>(data->body);
	KDEBUG_LOG(p->id, "PLAYER DECORATE SHOP CB\t[%u %u %u]", p->id, pkg->shop_id, pkg->decorate_id);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, pkg->shop_id, idx);
	pack(pkgbuf, pkg->decorate_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int trade_pause_shop_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
		
	uint32_t shop_id = 0;
	int idx = 0;
	unpack(body, shop_id, idx);
	if (p->trade_grp != 0 && p->trade_grp->trade_type)
	{
		int ifx = sizeof(tr_proto_t);
		pack_h(trpkgbuf, shop_id, ifx);
		init_tr_proto_head(p, p->id, trpkgbuf, ifx, tr_player_pause_shop);
		return send_to_trade_svr(p, trpkgbuf, ifx, p->trade_grp->fd_idx);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_trade_not_in_market, 1); 
}

int trsvr_player_pause_shop_callback(player_t *p, tr_proto_t * pkg )
{
	uint32_t shop_id = 0;
	int idx = 0;
	unpack_h(pkg->body, shop_id, idx);

	KDEBUG_LOG(p->id, "PLAYER PAUSE SHOP \t [%u %u]", p->id, shop_id);

	int ifx = sizeof(cli_proto_t);
	pack(trpkgbuf, shop_id, ifx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, ifx);
    return send_to_player(p, pkgbuf, ifx, 1);	
}

int trsvr_player_enter_market(player_t * p, uint32_t market_id, int tr_idx)
{
	int idx = sizeof(tr_proto_t);
	TRACE_LOG("%u %u %u", p->id, market_id, tr_idx);
	idx += pack_enter_trade_info(trpkgbuf + idx, p, market_id);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_enter_market);
	return send_to_trade_svr(p, trpkgbuf, idx, tr_idx); 
}

int trsvr_player_leave_market(player_t *p)
{
	int idx = sizeof(tr_proto_t);
	//pack_h(trpkgbuf, p->trade_grp->market_id, idx);
	KDEBUG_LOG(p->id, "LEAVE MARKET\t[%u %u]", p->id, p->trade_grp->market_id);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_leave_market);
	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx); 
}

int trsvr_player_change_market(player_t *p, uint32_t market_id)
{
	int idx = sizeof(tr_proto_t);
	pack_h(trpkgbuf, market_id, idx);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_change_market);
	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx); 
}

int trsvr_player_setup_shop(player_t *p, uint32_t shop_id)
{
	int idx = sizeof(tr_proto_t);
	pack_h(trpkgbuf, shop_id, idx);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_setup_shop);
	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx);
}

int trsvr_player_enter_market_callback(player_t * p, tr_proto_t * pkg)
{
	enter_market_rsp_t * tr_grp = reinterpret_cast<enter_market_rsp_t*>(pkg->body);
	//init playe's trade grp
	if (p->trade_grp == 0)
	{
		p->trade_grp = alloc_trade_grp(); 
		//hook player in trade player list
		list_add_tail(&(p->tradehook), &(trade_players.player_list));
		KDEBUG_LOG(p->id, "ADD PLAYER IN TRADE LIST");

	}
	p->trade_grp->market_id = tr_grp->market_id;
	p->trade_grp->server_id = tr_grp->server_id;
	
	p->trade_grp->fd_idx = get_trsvr_fd_idx_by_id(tr_grp->server_id);
	p->trade_grp->trade_type = 0;

	p->invisible = 1;
	send_leave_map_rsp(p, 0);

	int idx = sizeof(cli_proto_t);
	uint32_t c_market_id = (tr_grp->server_id - 1) * max_market_num + tr_grp->market_id + 1;
	KDEBUG_LOG(p->id, "ENTER MARKET CB\t[p=%u svr=%u market=%u c_marketid=%u Coins=%u]", p->id,  tr_grp->server_id, tr_grp->market_id, c_market_id, p->coins);
	pack(pkgbuf, c_market_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);	
}

int trsvr_player_change_market_callback(player_t *p, tr_proto_t * pkg)
{
	enter_market_rsp_t * tr_grp = reinterpret_cast<enter_market_rsp_t*>(pkg->body);

	p->trade_grp->market_id = tr_grp->market_id;
	p->trade_grp->server_id = tr_grp->server_id;
	p->trade_grp->fd_idx = get_trsvr_fd_idx_by_id(tr_grp->server_id);
	p->trade_grp->trade_type = 0;

	int idx = sizeof(cli_proto_t);
	uint32_t c_market_id = (tr_grp->server_id - 1) * max_market_num + tr_grp->market_id + 1;
	KDEBUG_LOG(p->id, "CHANGE MARKET CB\t[p=%u svr=%u market=%u cmarket=%u", p->id, tr_grp->server_id, tr_grp->market_id, c_market_id);
	pack(pkgbuf, c_market_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);	
}

int db_get_coins_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_coins_rsp_t *p_rsp = reinterpret_cast<get_coins_rsp_t*>(body);
	if (p->waitcmd == cli_proto_trade_leave_market) {
		p->coins = p_rsp->left_coins;
		KDEBUG_LOG(p->id, "GET COINS\t[%u %u %u]", p->id, p->coins, p_rsp->left_coins);
		list_del(&p->tradehook);
		KDEBUG_LOG(p->id, "DEL PLAYER FROM TRADE LIST");	
		free_trade_grp(p->trade_grp);	
		p->trade_grp = 0;
		p->invisible = 0;
		uint32_t map = 0;
		int idx = 0;
		unpack_h(p->session, map, idx);
		if (map != 12) {
			if(!p->cur_map) {
				ERROR_LOG("not in map uid=%u cmd=%u", p->id, p->waitcmd);
				return -1;
			}
			map_t * old_m = p->cur_map;
			assert(old_m->player_num>0);
			--(old_m->player_num);
			list_del(&p->maphook);
			p->cur_map=0;
			map_t* new_m = get_map(map);
			reset_common_action(p);
			if (!new_m) {
				ERROR_LOG("can't find map %u uid=%u", map, p->id);
				return -1;
			}
			p->xpos = new_m->init_x;
			p->ypos = new_m->init_y;
			list_add_tail(&(p->maphook), &(new_m->playerlist));
			++(new_m->player_num);
			p->cur_map = new_m;
		}
		send_entermap_rsp(p, 0);
		send_header_to_player(p, p->waitcmd, 0, 1);
	}
	return 0;
}


int trsvr_player_leave_market_callback(player_t *p, tr_proto_t  *pkg)
{  
	KDEBUG_LOG(p->id, "LEAVE MARKET WITH ONLINE COINS = %u", p->coins);
	if (p->waitcmd == cli_proto_trade_leave_market) {
	
		return send_request_to_db(p, p->id, p->role_tm, dbproto_get_coins, 0, 0);
		/*
		free_trade_grp(p->trade_grp);	
		p->trade_grp = 0;
		p->invisible = 0;
		send_entermap_rsp(p, 0);
		return send_header_to_player(p, p->waitcmd, 0, 1);
		*/
	} else if (p->waitcmd == cli_proto_trade_enter_market) {
	
		//get tmp info
		int idx = 0;
		uint32_t server_id = 0;
		uint32_t market_id = 0;
		unpack_h(p->session, server_id, idx);
		unpack_h(p->session, market_id, idx);
		p->trade_grp->trade_type = 0;
		return trsvr_player_enter_market(p, market_id, get_trsvr_fd_idx_by_id(server_id));
	}
	return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
}

int trsvr_player_update_shop_callback(player_t * p, tr_proto_t * pkg)
{
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

/** 
 * @brief  handle for usr buy goods pkg callback
 * 
 */
int trsvr_player_buy_goods_callback(player_t * p, tr_proto_t * pkg)
{
	CHECK_DBERR(p, pkg->ret);
    buy_goods_rsp_t *rsp = reinterpret_cast<buy_goods_rsp_t*>(pkg->body);
    if (rsp->type == 1) {
        p->my_packs->add_clothes(p,rsp->itemid,rsp->uniquekey,rsp->duration, channel_string_trade,rsp->gettime,rsp->endtime,rsp->attirelv);
    } else if ( rsp->type == 2) {
        p->my_packs->add_item(p, rsp->itemid, rsp->cnt, channel_string_trade);
    }
	KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s cost=%u leftcoins=%u]", 
					rsp->type, rsp->itemid, rsp->cnt, channel_string_trade, p->coins > rsp->leftcoins ? 
						p->coins - rsp->leftcoins : rsp->leftcoins - p->coins, rsp->leftcoins);

	if (!(rsp->update_flag)) {
		p->coins = rsp->leftcoins;
	}
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->update_flag, idx);
    pack(pkgbuf, rsp->type, idx);
	pack(pkgbuf, rsp->grid_id, idx);
    pack(pkgbuf, rsp->itemid, idx);
    pack(pkgbuf, rsp->uniquekey, idx);
    pack(pkgbuf, rsp->cnt, idx);
    pack(pkgbuf, rsp->duration, idx);
    pack(pkgbuf, rsp->attirelv, idx);
    pack(pkgbuf, rsp->leftcoins, idx);
    //memcpy(pkgbuf + idx, pkg->body, pkg->len);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

/** 
 * @brief handle for player's shop sell goods pkg 
 * 
 */
int trsvr_player_sell_goods_callback(player_t * p, tr_proto_t * pkg)
{
    sell_goods_rsp_t *rsp = reinterpret_cast<sell_goods_rsp_t*>(pkg->body);
    if (rsp->type == 1) {
        p->my_packs->del_clothes(rsp->uniquekey, channel_string_trade);
    } else if ( rsp->type == 2 ) {
        p->my_packs->del_item(p, rsp->item_id, rsp->cnt, channel_string_trade);
    }
	if ((rsp->item_id == 1500303 || rsp->item_id == 1500304 || rsp->item_id == 1500305) && p->trade_grp) {
		p->trade_grp->sell_cnt += rsp->cnt;
	}
	do_stat_log_in_two_total_coin(p, p->coins, rsp->leftcoins);
    KDEBUG_LOG(p->id, "SELL GOODS CB\t[uid=%u item=%u %u %u coins=%u",p->id, rsp->type, rsp->item_id, rsp->cnt, rsp->leftcoins);
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->type, idx);
    pack(pkgbuf, rsp->shop_id, idx);
    pack(pkgbuf, rsp->goods_id, idx);
    pack(pkgbuf, rsp->item_id, idx);
    pack(pkgbuf, rsp->uniquekey, idx);
    pack(pkgbuf, rsp->cnt, idx);
    pack(pkgbuf, rsp->leftcoins, idx);
	p->coins = rsp->leftcoins;
    //memcpy(pkgbuf + idx, pkg->body, pkg->len);
    init_cli_proto_head(pkgbuf, p, 4008, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

/** 
 * @brief  pkg for setup shop callback 
 * 
 */
int trsvr_player_setup_shop_callback(player_t *p, tr_proto_t *pkg)
{
    uint32_t * shop_id = reinterpret_cast<uint32_t*>(pkg->body);
    KDEBUG_LOG(p->id, "SETUP SHOP CB\t[p=%u shopid=%u]",p->id,*shop_id);
	p->trade_grp->trade_type = 1;
	p->trade_grp->sell_cnt = 0;
	p->coins -= setup_shop_cost;
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, *shop_id, idx);
	pack(pkgbuf, shop_id[1], idx);
	pack(pkgbuf, shop_id[2], idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

/** 
 * @brief  load all the shop's simple info in the trade market
 * 
 */
int trsvr_player_get_shop_list(player_t *p)
{
	p->waitcmd = 0;
	int idx = sizeof(tr_proto_t);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_get_shop_list);
	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx); 
}

/** 
 * @brief  callback pkg for load shop simple info
 * 
 */
int trsvr_player_get_shop_list_callback(player_t *p, tr_proto_t *pkg)
{
	KDEBUG_LOG(p->id, "SHOP LIST\t[p=%u %lu]", p->id, pkg->len - sizeof(tr_proto_t));
	int idx = sizeof(cli_proto_t);
	pack(trpkgbuf, pkg->body, pkg->len - sizeof(tr_proto_t), idx);
	init_cli_proto_head(trpkgbuf, p, p->waitcmd, idx); 
	return send_to_player(p, trpkgbuf, idx, 1); 
}

/** 
 * @brief walk in trade market 
 */
int trsvr_player_walk(player_t *p, uint32_t x, uint32_t y, uint32_t flag)
{
	p->waitcmd = 0;
	int idx = sizeof(tr_proto_t);
	pack_h(trpkgbuf, x, idx);
	pack_h(trpkgbuf, y, idx);
	pack_h(trpkgbuf, flag, idx);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_walk);
	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx);
}

/** 
 * @brief  walk keyboard in trade market
 * 
 */
int trsvr_player_walk_keyboard(player_t *p, uint32_t x, uint32_t y, uint8_t dir, uint8_t state)
{
	p->waitcmd = 0;
	int idx = sizeof(tr_proto_t);
	pack_h(trpkgbuf, x, idx);
	pack_h(trpkgbuf, y, idx);
	pack_h(trpkgbuf, dir, idx);
	pack_h(trpkgbuf, state, idx);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_walk_keyboard);
	return  send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx);
}

/** 
 * @brief  stand in trade market
 * 
 */
int trsvr_player_stand(player_t *p, uint32_t x, uint32_t y, uint8_t dir)
{
	p->waitcmd = 0;
	int idx = sizeof(tr_proto_t);
	pack_h(trpkgbuf, x, idx);
	pack_h(trpkgbuf, y, idx);
	pack_h(trpkgbuf, dir, idx);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_stand);
	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx); 
}

/** 
 * @brief jump in trade market
 */
int trsvr_player_jump(player_t *p, uint32_t x, uint32_t y)
{
	p->waitcmd = 0;
	int idx = sizeof(tr_proto_t);
	pack_h(trpkgbuf, x, idx);
	pack_h(trpkgbuf, y, idx);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_jump);
	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx);
}

/** 
 * @brief talk in trade market  
 * 
 */
int trsvr_player_talk(player_t* p, uint8_t* msg, uint32_t msg_len, userid_t recvid)
{
	p->waitcmd = 0;
	int idx = sizeof(tr_proto_t);

	TRACE_LOG("%u %u %u ", p->id, recvid, msg_len);
	pack_h(trpkgbuf, recvid, idx);
	pack_h(trpkgbuf, msg_len, idx);
	pack(trpkgbuf, msg, msg_len, idx);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_talk);
	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx);
}

/** 
 * @brief  get all the usr info in the market
 * 
 */
int trsvr_player_list_user(player_t *p)
{
	p->waitcmd = 0;
	int idx = sizeof(tr_proto_t);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_list_user);
	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx);
}

/** 
 * @brief  load all the goods info of a shop
 * 
 */
int trsvr_player_shop_detail_callback(player_t *p, tr_proto_t* pkg)
{
	uint32_t left_tm = 0;
	uint32_t goods_cnt = 0;
	int x = 0;
	unpack(pkg->body, left_tm, x);
	unpack(pkg->body, goods_cnt, x);

	KDEBUG_LOG(p->id, "SHOP DETAIL CB\t[p=%u len=%lu cnt=%u tm=%u]", p->id, pkg->len - sizeof(tr_proto_t), goods_cnt, left_tm);
	int idx = sizeof(cli_proto_t);
	pack(trpkgbuf, pkg->body, pkg->len - sizeof(tr_proto_t), idx);
	init_cli_proto_head(trpkgbuf, p, p->waitcmd, idx); 
	return send_to_player(p, trpkgbuf, idx, 1); 
}


/** 
 * @brief  trade svr callback pkg for one drop his shop
 * 
 */
int trsvr_player_drop_shop_callback(player_t *p, tr_proto_t * pkg)
{
	uint32_t sell_cnt = 0;
	uint32_t shop_id = 0;
	int idx = 0;
	unpack_h(pkg->body, sell_cnt, idx);
	unpack_h(pkg->body, shop_id, idx);
	KDEBUG_LOG(p->id, "PLAYER CLOSED SHOP CB\t [p=%u shop=%u cnt=%u]", p->id, shop_id, sell_cnt);

	p->trade_grp->trade_type = 0;
	int ifx = sizeof(cli_proto_t);
	pack(pkgbuf, shop_id, ifx);
	init_cli_proto_head(pkgbuf, p, cli_proto_trade_drop_shop, ifx);
	if (p->waitcmd == cli_proto_trade_drop_shop) {
    	send_to_player(p, pkgbuf, ifx, 1);
	} else {
		send_to_player(p, pkgbuf, ifx, 0);
	}
	
	//tmp_func(p);
	return 0;
}

/** 
 * @brief  player change his shop's name
 * 
 */
int trade_change_shop_name_cmd(player_t *p, uint8_t *body, uint32_t len)
{
	body[len - 1] = '\0';
	
	CHECK_DIRTYWORD(p, body + 4);
	CHECK_INVALID_WORD(p, body + 4);

	int idx = 0;
	uint32_t shopid = 0;
	unpack(body, shopid, idx);

	TRACE_LOG("player want change shop name \t [%u %u %s]", p->id, shopid, body + 4);
	if (!(p->trade_grp)) { 
		return send_header_to_player(p, p->waitcmd, cli_err_trade_not_in_market, 1);
	}

	if (!p->trade_grp->trade_type) {
		return send_header_to_player(p, p->waitcmd, cli_err_trade_not_a_shopkeeper, 0);  
	}

	idx = sizeof(tr_proto_t);

	pack_h(trpkgbuf, shopid, idx);
	pack(trpkgbuf, body + sizeof(shopid), len - sizeof(shopid), idx);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_player_change_shop_name);
	return send_to_trade_svr(p, trpkgbuf, idx, p->trade_grp->fd_idx);

}


/** 
 * @brief  trade svr return pkg for usr change shop name  cmd  
 */
int trsvr_player_change_shop_name_callback(player_t *p, tr_proto_t *data)
{
	change_shop_name_rsp_t *pkg = reinterpret_cast<change_shop_name_rsp_t*>(data->body); 
	KDEBUG_LOG(p->id, "PLAYER CHANGE SHOP NAME\t [%u %u %s]", p->id, pkg->shop_id, pkg->shop_name);
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, pkg->shop_id, idx);
	pack(pkgbuf, pkg->shop_name, 32, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return  send_to_player(p, pkgbuf, idx, 1);
}

/** 
 * @brief player load sell log 
 */
int trade_get_sell_log_cmd(player_t *p, uint8_t *body, uint32_t len)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_sell_log, 0, 0);
}

/** 
 * @brief db return player' sell log info 
 * 
 */
int db_get_sell_log_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	get_sell_log_rsp_t* p_rsp = reinterpret_cast<get_sell_log_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_sell_log_rsp_t) + p_rsp->count * sizeof(get_sell_log_item_t));

	KDEBUG_LOG(p->id, "SELL LOG CB\t[p=%u cnt=%u]", p->id, p_rsp->count);
	get_sell_log_item_t* p_item = p_rsp->item;
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p_rsp->count, idx);
	for (uint32_t i = 0; i < p_rsp->count; i++) {
		pack(pkgbuf, p_item->type, idx);
		pack(pkgbuf, p_item->itemid, idx);
		pack(pkgbuf, p_item->uniquekey, idx);
		pack(pkgbuf, p_item->item_lv, idx);
		pack(pkgbuf, p_item->cnt, idx);
		pack(pkgbuf, p_item->price, idx);
		pack(pkgbuf, p_item->sell_tm, idx);
		TRACE_LOG("%u %u %u %u %u %u", p_item->type, p_item->itemid, p_item->uniquekey, p_item->cnt, p_item->price, p_item->sell_tm);
		p_item++;
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx); 
	return send_to_player(p, pkgbuf, idx, 1); 	

}

/** 
 * @brief player want change into a aimed market
 */
int trade_enter_aim_market_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
	if(p->trade_grp == NULL) {
		ERROR_LOG("PLAYER %d NOT IN A TRADE MARKET ", p->id);
		return -1;
	}

	if (p->trade_grp->trade_type) {
		return send_header_to_player(p, p->waitcmd, cli_err_trade_shop_busy, 1);
	}

	int idx = 0;
	uint32_t c_market_id = 0;
	unpack(body, c_market_id, idx);
	if (c_market_id <= 0 || c_market_id > max_market_num *  max_trade_svr_num )
	{
		return send_header_to_player(p, p->waitcmd, cli_err_trade_err_market, 1);

	}
	uint32_t server_id = ( (c_market_id - 1) / max_market_num ) + 1; 
	uint32_t market_id = (c_market_id - 1 ) % max_market_num;
	KDEBUG_LOG(p->id, "ENTER AIM MARKET\t[%u %u :old=%u %u new=%u %u]", p->id, c_market_id, 
			p->trade_grp->server_id, p->trade_grp->market_id, server_id, market_id);
	if ((p->trade_grp->server_id == server_id) && (p->trade_grp->market_id == market_id ))
	{
		int idx = sizeof(cli_proto_t);
		pack(trpkgbuf, c_market_id, idx);
		init_cli_proto_head(trpkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, trpkgbuf, idx, 1); 
	}

	if (trade_sw_change_market(p, server_id, market_id) < 0) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error,1);
	}
	int ix = 0;
	pack_h(p->session, server_id, ix);
	pack_h(p->session, market_id, ix);
	return 0;
}

int trade_mcast_message_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{

    if (p->trade_grp) {
        uint32_t tm_diff = time(0)  - p->last_mcast_tm; 

        if (tm_diff < 30) {
            TRACE_LOG("player %d use trade mcast in 30 's ", p->id);
            return send_header_to_player(p, p->waitcmd, cli_err_trade_mcast_in_30s, 1);
        } 

        trade_mcast_info_t *rsp = reinterpret_cast<trade_mcast_info_t*>(body);

        uint32_t front_id = 0; // have not been  used now!
        uint32_t item_id = 0;
        item_id = taomee::bswap(rsp->item_id);
        front_id = taomee::bswap(rsp->front_id);

        //check if there has this item
        if (!is_trade_bcast_item(item_id)) {
            TRACE_LOG("no such item tonic\t[uid=%u item_id=%u]",p->id, item_id);
            return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
        }

        const GfItem* itm = items->get_item(item_id);

        if ( !itm || !items->can_use(itm->category()) ) {
            TRACE_LOG("no such item tonic\t[uid=%u item_id=%u]",p->id, item_id);
            return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
        }

        uint32_t have_cnt = p->my_packs->get_item_cnt(item_id);

        if (have_cnt > 0 ) {

            //store the mcast info 
            body[bodylen - 1] = '\0';

            //CHECK_DIRTYWORD(p, body + 8);
            //CHECK_INVALID_WORD(p, body + 8);
            REPLACE_DIRTYWORD(p, body + 8);


            p->sesslen = 0;
            pack_h(p->session, front_id, p->sesslen); 
            pack(p->session, rsp->info, max_trade_mcast_size, p->sesslen); 

            bool is_shop_item = itm->is_item_shop_sale();
            return db_use_item(p, item_id, 1, is_shop_item);
        } else {
            TRACE_LOG(" player %d don't have any trade mcast item!", p->id);
            return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
        }
    } else {
        TRACE_LOG("player %d is not in any trade market!", p->id);
        return send_header_to_player(p, p->waitcmd, cli_err_trade_not_in_market, 1);
    }
}

int player_mcast_message_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
    int idx = 0;
    player_mcast_msg_t rsp = {0};
    unpack(body, rsp.item_id, idx);
    unpack(body, rsp.front_id, idx);
    unpack(body, rsp.msg, max_trade_mcast_size, idx);

    TRACE_LOG("player mcast [%u|%u|%s]",rsp.item_id,rsp.front_id,rsp.msg);
    if (!is_trade_bcast_item(rsp.item_id) && rsp.item_id != world_notice_item_id) {
        return 0;
    }

    if ( is_trade_bcast_item(rsp.item_id) && !(p->trade_grp) ) {
        TRACE_LOG("no such item tonic\t[uid=%u item_id=%u]",p->id, rsp.item_id);
        return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
    }

    const GfItem* itm = items->get_item(rsp.item_id);
    if ( !itm  ) {
        TRACE_LOG("no such item or cannot use tonic\t[uid=%u item_id=%u]",p->id, rsp.item_id);
        return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
    }

    uint32_t have_cnt = p->my_packs->get_item_cnt(rsp.item_id);
    if (have_cnt <= 0) {
        TRACE_LOG(" player %d don't have any trade mcast item!", p->id);
        return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
    }

    uint32_t cur_time = time(NULL);
    if ( is_trade_bcast_item(rsp.item_id) && cur_time < p->last_mcast_tm + 30) {
        return send_header_to_player(p, p->waitcmd, cli_err_trade_mcast_in_30s, 1);
    } else if (rsp.item_id == world_notice_item_id && cur_time < p->last_world_tm + 3 * 60) {
        return send_header_to_player(p, p->waitcmd, cli_err_trade_mcast_in_3min, 1);
    }
    //store the mcast info 
    body[bodylen - 1] = '\0';

    CHECK_DIRTYWORD(p, body + 8);
    CHECK_INVALID_WORD(p, body + 8);

    p->sesslen = 0;
    pack_h(p->session, rsp.front_id, p->sesslen); 
    pack(p->session, rsp.msg, max_trade_mcast_size, p->sesslen); 

    bool is_shop_item = itm->is_item_shop_sale();
    return db_use_item(p, rsp.item_id, 1, is_shop_item);
}


//------------------------------------------------------------------
//  担保交易
//------------------------------------------------------------------

uint32_t get_safe_trade_server_id(uint32_t room_id)
{
    return ((room_id >> 16) & 0x0000ffff) - 1;
}

int do_cancel_safe_trade(player_t *p)
{
    int trd_idx = get_safe_trade_server_id(p->safe_trade_room_id);
    int idx = sizeof(tr_proto_t);
    pack_h(trpkgbuf, p->safe_trade_room_id, idx);
    pack_h(trpkgbuf, p->id, idx);
    pack_h(trpkgbuf, p->role_tm, idx);
    init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_safe_trade_cancel);
    return send_to_trade_svr(p, trpkgbuf, idx, trd_idx);
}

int do_cancel_safe_trade_by_room_id(player_t *p, uint32_t room_id)
{
    int trd_idx = get_safe_trade_server_id(room_id);
    int idx = sizeof(tr_proto_t);
    pack_h(trpkgbuf, room_id, idx);
    pack_h(trpkgbuf, p->id, idx);
    pack_h(trpkgbuf, p->role_tm, idx);
    init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_safe_trade_cancel);
    return send_to_trade_svr(p, trpkgbuf, idx, trd_idx);
}

/** 
 * @brief create safe trade room 
 * @param p
 * @param body
 * @param bodylen
 * @return 
 */
int cancel_safe_trade_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
    if (!p->safe_trade_room_id) {
        p->safe_trade_room_id = 0;

        int idx = sizeof(cli_proto_t);
        pack(pkgbuf, p->id, idx);
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx); 
        return send_to_player(p, pkgbuf, idx, 1); 	
    }
    return do_cancel_safe_trade(p);
}

/** 
 * @brief  trade svr callback pkg
 * 
 */
int trsvr_safe_trade_cancel_callback(player_t *p, tr_proto_t * pkg)
{
    int idx = 0;
    uint32_t userid = 0;
    unpack_h(pkg->body, userid, idx);
	KDEBUG_LOG(userid, "PLAYER cancel safe trade CB\t [p=%u]", p->id);

    p->safe_trade_room_id = 0;

    idx = sizeof(cli_proto_t);
    pack(pkgbuf, userid, idx);
    init_cli_proto_head(pkgbuf, p, cli_proto_cancel_safe_trade, idx); 
    return send_to_player(p, pkgbuf, idx, 1); 	
}


int safe_trade_create_room(player_t *p, uint32_t invitee_id, uint32_t invitee_tm)
{
	int idx = sizeof(tr_proto_t);
	pack_h(trpkgbuf, p->id, idx);
	pack_h(trpkgbuf, p->role_tm, idx);
	pack_h(trpkgbuf, invitee_id, idx);
	pack_h(trpkgbuf, invitee_tm, idx);
    int trd_idx = trade_svr_cnt - 1;
	if (!trade_svr_cnt) {
		trd_idx = 0;
	}
    /*
    if (p->trade_grp) {
        trd_idx = p->trade_grp->fd_idx;
    } else {
        trd_idx = 6;//get_safe_trade_server_id(p->safe_trade_room_id);
    }*/
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_safe_trade_create_room);
	return send_to_trade_svr(p, trpkgbuf, idx, trd_idx);
}
/** 
 * @brief invite safe trade 
 * @param p
 * @param body
 * @param bodylen
 * @return 
 */
int invite_safe_trade_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
    //create safe trade room
    uint32_t invitee_id = 0, invitee_tm = 0;
    int idx = 0;
    unpack(body, invitee_id, idx);
    unpack(body, invitee_tm, idx);

    if (!is_in_right_hour(6, 24)) {
        return send_header_to_player(p, p->waitcmd, cli_err_enter_market_err_tm, 1);
    }

    if (p->coins < 10000) {
        return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
    }
	if (p->coins > 4000000000) {
		return send_header_to_player(p, p->waitcmd, cli_err_coins_too_much, 1);
	}

    if (p->my_packs->get_remain_clothes_bag_grid(p) < 1
        || p->my_packs->get_remain_item_bag_grid(p) < 1) {
        return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
    }

    if (p->safe_trade_room_id) {
        return -1;
    }

    if (!is_player_checked_pay_passwd(p)) {
        ERROR_LOG("%u havnot check pay passwd", p->id);
        return -1;
    }

    if (p->battle_grp || p->home_grp) {
        return send_header_to_player(p, p->waitcmd, cli_err_cannot_safe_trade, 1);
    }

    if (p->trade_grp && p->trade_grp->trade_type)  {
        return send_header_to_player(p, p->waitcmd, cli_err_cannot_safe_trade, 1);
    }

    return	safe_trade_create_room(p, invitee_id, invitee_tm);
}

/** 
 * @brief  trade svr callback pkg for one drop his shop
 * 
 */
int trsvr_safe_trade_create_room_callback(player_t *p, tr_proto_t * pkg)
{
    uint32_t invitee_id = 0, invitee_tm = 0, room_id = 0;
	int idx = 0;
	unpack_h(pkg->body, room_id, idx);
	unpack_h(pkg->body, invitee_id, idx);
	unpack_h(pkg->body, invitee_tm, idx);
	KDEBUG_LOG(p->id, "PLAYER create safe trade room CB\t [%u %u]", room_id, invitee_id);

    p->safe_trade_room_id = room_id;
    // reduce coin
    p->coins -= 10000;
    db_reduce_money(p, 10000);

    uint32_t type = 11; //invite trade
    player_t* to = get_player(invitee_id);
    if (to) {
        if (to->battle_grp || to->home_grp || (to->trade_grp && to->trade_grp->trade_type)) {
            //cancel safe trade
            do_cancel_safe_trade(p);
            return send_header_to_player(p, p->waitcmd, 0, 1);
        } else {
            send_rltm_notification(cli_proto_rltm_notify, invitee_id, p, type, 0, room_id, 0, 0);
        }
    } else {
        send_rltm_notification(cli_proto_rltm_notify, invitee_id, p, type, 0, room_id, 0, 0);
    }

    return send_header_to_player(p, p->waitcmd, 0, 1);
}


int safe_trade_enter_room(player_t *p, uint32_t room_id)
{
    int trd_idx = get_safe_trade_server_id(room_id);
	int idx = sizeof(tr_proto_t);
	pack_h(trpkgbuf, room_id, idx);
	pack_h(trpkgbuf, p->id, idx);
	pack_h(trpkgbuf, p->role_tm, idx);
	init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_safe_trade_join_room);
	return send_to_trade_svr(p, trpkgbuf, idx, trd_idx);
}

int trsvr_safe_trade_join_room_callback(player_t *p, tr_proto_t * pkg)
{
    int idx = 0;
    uint32_t peer_id = 0, peer_tm = 0, room_id = 0, flag = 0;
    unpack_h(pkg->body, room_id, idx);
    unpack_h(pkg->body, flag, idx); // 2-->sucess
    unpack_h(pkg->body, peer_id, idx);
    unpack_h(pkg->body, peer_tm, idx);

	KDEBUG_LOG(p->id, "PLAYER enter safe trade room CB\t [%u] [%u %u]", p->waitcmd, room_id, peer_id);

    uint32_t status = 1;
    if (flag == 2) {
        status = 0;
        send_rltm_notification(cli_proto_reply_btl_invite_notify, peer_id, p, 12, 1, room_id);
    } else {
        p->safe_trade_room_id = 0;
    }

    idx = sizeof(cli_proto_t);
    pack(pkgbuf, status, idx);
    pack(pkgbuf, peer_id, idx);
    pack(pkgbuf, room_id, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

/** 
 * @brief reply safe trade 
 * @param p
 * @param body
 * @param bodylen
 * @return 
 */
int reply_safe_trade_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
    uint32_t cur_map_id = get_mapid_low32(p->cur_map->id);
    if (cur_map_id >= 43 && cur_map_id <= 52) {
        p->waitcmd = 0;
        return 0;
    }

    int idx = 0;
    uint32_t type = 0, inviter_id = 0, room_id = 0, accept = 0;
    unpack(body, type, idx);
    unpack(body, inviter_id, idx);
    unpack(body, room_id, idx);
    unpack(body, accept, idx);
    type = 12; //应答邀请交易

    uint32_t err_cmd = 0;
    if (p->coins < 10000) {
        accept = 0;
        err_cmd = cli_err_no_enough_coins;
    } else if (p->battle_grp || p->home_grp) {
        accept = 3;
        err_cmd = cli_err_cannot_safe_trade;
    } else if (p->safe_trade_room_id || (p->trade_grp && p->trade_grp->trade_type)) {
        accept = 4;
        err_cmd = cli_err_cannot_safe_trade;
    }

    KDEBUG_LOG(p->id, "PLAYER reply safe trade\t [%u] [%u %u]", accept, inviter_id, room_id);

    if (accept == 1) {
		if (p->coins > 4000000000) {
			return send_header_to_player(p, p->waitcmd, cli_err_coins_too_much, 1);
		}
        p->safe_trade_room_id = room_id;
        safe_trade_enter_room(p, room_id);
        //reduce coins
        p->coins -= 10000;
        db_reduce_money(p, 10000);
    } else {
        //send_rltm_notification(cli_proto_reply_btl_invite_notify, inviter_id, p, type, accept, 0);
        do_cancel_safe_trade_by_room_id(p, room_id);
        return send_header_to_player(p, p->waitcmd, err_cmd, 1);
    }
    return 0;
}

/** 
 * @brief  safe trade set item
 * @param p
 * @param body
 * @param bodylen
 * @return 
 */
int safe_trade_set_item_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
    int u_idx = 0;
    uint32_t room_id = 0, xiaomee = 0, item_cnt = 0, attire_cnt = 0;
    uint32_t player_action = 0; //1-->set_item_action; 2-->cancel_item_action

    if (p->home_grp || p->battle_grp || (p->trade_grp && p->trade_grp->trade_type)) {
        ERROR_LOG("%u in trade/home/battle cannot safe trade", p->id);
        return -1;
    }

    unpack(body, room_id, u_idx);
    unpack(body, player_action, u_idx);
    unpack(body, xiaomee, u_idx);
    unpack(body, item_cnt, u_idx);
    unpack(body, attire_cnt, u_idx);

    TRACE_LOG(" [%u] IN header %u %u %u %u", p->id, room_id, xiaomee, item_cnt, attire_cnt);

    if (room_id != p->safe_trade_room_id) return -1;
    if (item_cnt + attire_cnt > 10) return -1;

    CHECK_VAL_GE(bodylen, 12 + item_cnt * sizeof(trade_item_unit_t));

    if ( p->coins < xiaomee) {
        return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
    }

    if (xiaomee > 20000000 || p->coins > 4000000000) {
        return send_header_to_player(p, p->waitcmd, cli_err_coins_too_much, 1);
    }

    trade_item_unit_t item_unit = {0};
    int idx = sizeof(tr_proto_t);
    pack_h(trpkgbuf, room_id, idx);
    pack_h(trpkgbuf, p->id, idx); // 
    pack_h(trpkgbuf, p->role_tm, idx); //
    pack_h(trpkgbuf, player_action, idx);
    pack_h(trpkgbuf, xiaomee, idx); //money
    pack_h(trpkgbuf, item_cnt, idx);
    pack_h(trpkgbuf, attire_cnt, idx);
    for (uint32_t i = 0; i < item_cnt; i++) {
        unpack(body, item_unit.item_id, u_idx);
        unpack(body, item_unit.itemcnt, u_idx);

        TRACE_LOG("item info [%u %u]", item_unit.item_id, item_unit.itemcnt);

        const GfItem* itm = items->get_item(item_unit.item_id);
        if (!itm || !itm->is_tradable()) {
            return send_header_to_player(p, p->waitcmd, cli_err_failed_to_sell, 1);
        }

        if ( !p->my_packs->check_bag_item_count(item_unit.item_id, item_unit.itemcnt) ) {
            ERROR_LOG("%u set item not exist !!!! [%u %u]", p->id, item_unit.item_id, item_unit.itemcnt);
            return -1;
        }

        pack_h(trpkgbuf, item_unit.item_id, idx);
        pack_h(trpkgbuf, item_unit.itemcnt, idx);
    }

    trade_attire_unit_t attire_unit = {0};
    //unpack(body, attire_cnt, u_idx);
    //TRACE_LOG("[%u] set attire cnt %u", p->id, attire_cnt);
    //*(reinterpret_cast<uint32_t*>(trpkgbuf + 24)) = attire_cnt;
    
    for (uint32_t i = 0; i < attire_cnt; i++) {
        unpack(body, attire_unit.attire_id, u_idx);
        unpack(body, attire_unit.unique_id, u_idx);
        unpack(body, attire_unit.item_lv, u_idx);

        TRACE_LOG("item info [%u %u %u]", attire_unit.attire_id, attire_unit.unique_id, attire_unit.item_lv);

        const GfItem* itm = items->get_item(attire_unit.attire_id);
        if (!itm || !itm->is_tradable()) {
            return send_header_to_player(p, p->waitcmd, cli_err_failed_to_sell, 1);
        }

        if ( !p->my_packs->is_clother_item_exist(attire_unit.unique_id, attire_unit.attire_id, attire_unit.item_lv) ) {
            ERROR_LOG("uid [%u] item not exist !!!! [%u %u %u]", p->id, attire_unit.attire_id, attire_unit.unique_id, attire_unit.item_lv);
            return -1;
        }
		pack_clothes_info_t * clothes = p->my_packs->get_clothes_info_by_unique_id(attire_unit.unique_id);
		if (!clothes || clothes->lv > 8) {
			ERROR_LOG("%u not have this clothes %u %u", p->id, attire_unit.unique_id, clothes ? clothes->lv : 0);
			return -1;
		}

        pack_h(trpkgbuf, attire_unit.attire_id, idx);
        pack_h(trpkgbuf, attire_unit.unique_id, idx);
        pack_h(trpkgbuf, attire_unit.item_lv, idx);
    }

	KDEBUG_LOG(p->id, "SAFE TRADE SET ITEM\t[room_id=%u xiaomee=%u cnt=%u]", room_id, xiaomee, item_cnt);

    int trd_idx = get_safe_trade_server_id(p->safe_trade_room_id);
    init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_safe_trade_set_item);
    return send_to_trade_svr(p, trpkgbuf, idx, trd_idx);
}

int pack_safe_trade_set_item_pkg(uint8_t* buf, uint32_t room_id, uint32_t palyer_action,
    uint32_t peer_id, uint32_t xiaomee, uint32_t item_cnt, uint32_t attire_cnt, uint8_t* item_info) 
{
    int idx = 0;
	pack(buf, room_id, idx);
	pack(buf, palyer_action, idx);
	pack(buf, peer_id, idx);
	pack(buf, xiaomee, idx);
	//pack(buf, (uint32_t)get_server_id(), idx);
	pack(buf, item_cnt, idx);
    TRACE_LOG("[%u %u %u %u] item count [%u | %u]", room_id, palyer_action, peer_id, xiaomee, item_cnt, attire_cnt);
    trade_item_unit_t* p_items = reinterpret_cast<trade_item_unit_t*>(item_info);
    for (uint32_t i = 0; i < item_cnt; i++) {
        TRACE_LOG("%u %u", p_items[i].item_id, p_items[i].itemcnt);

        pack(buf, p_items[i].item_id, idx);
        pack(buf, p_items[i].itemcnt, idx);
    }

    pack(buf, attire_cnt, idx);

    trade_attire_unit_t* p_attires = reinterpret_cast<trade_attire_unit_t*>(item_info + item_cnt * sizeof(trade_item_unit_t));
    for (uint32_t i = 0; i < attire_cnt; i++) {
        TRACE_LOG("%u %u %u", p_attires[i].attire_id, p_attires[i].unique_id, p_attires[i].item_lv);

        pack(buf, p_attires[i].attire_id, idx);
        pack(buf, p_attires[i].unique_id, idx);
        pack(buf, p_attires[i].item_lv, idx);
    }

	return idx;
}

/** 
 * @brief  trade svr callback pkg
 */
int trsvr_safe_trade_set_item_callback(player_t *p, tr_proto_t * pkg)
{
    safe_trade_set_item_rsp_t *rsp = reinterpret_cast<safe_trade_set_item_rsp_t*>(pkg->body);
	KDEBUG_LOG(p->id, "SAFE TRADE SET ITEM CB\t action[%u] [%u %u %u|%u %u]", rsp->player_action, rsp->room_id, rsp->peer_id, rsp->xiaomee, rsp->itemcnt, rsp->attirecnt);

    CHECK_VAL_EQ(pkg->len - sizeof(tr_proto_t), sizeof(safe_trade_set_item_rsp_t) + rsp->itemcnt * sizeof(trade_item_unit_t) + rsp->attirecnt * sizeof(trade_attire_unit_t));

    //if ( !is_valid_uid(rsp->peer_id) ) {
    if ( rsp->peer_id == 0 ) {
        ERROR_LOG("safe trade get invalid uid [%u|%u]", p->id, rsp->peer_id);
        return -1;
    }

    int idx = sizeof(cli_proto_t);
    idx += pack_safe_trade_set_item_pkg(pkgbuf + idx, rsp->room_id, rsp->player_action, 
        rsp->peer_id, rsp->xiaomee, rsp->itemcnt, rsp->attirecnt, rsp->items);
    init_cli_proto_head(pkgbuf, p, cli_proto_safe_trade_set_item, idx);
    if (rsp->peer_id == p->id) {
        return send_to_player(p, pkgbuf, idx, 1);
    } else {
        return send_to_player(p, pkgbuf, idx, 0);
    }
}


/** 
 * @brief  safe trade 
 * @param p
 * @param body
 * @param bodylen
 * @return 
 */
int safe_trade_action_agree_cmd(player_t *p, uint8_t * body, uint32_t bodylen)
{
    if (p->home_grp || p->battle_grp || (p->trade_grp && p->trade_grp->trade_type)) {
        ERROR_LOG("%u in trade/home/battle cannot safe trade", p->id);
        return -1;
    }

    if (!p->safe_trade_room_id) return -1;

    int trd_idx = get_safe_trade_server_id(p->safe_trade_room_id);
    TRACE_LOG("%u safe trade agree [%u]", p->id, p->safe_trade_room_id);
    int idx = sizeof(tr_proto_t);
    pack_h(trpkgbuf, p->safe_trade_room_id, idx);
    pack_h(trpkgbuf, p->id, idx);
    pack_h(trpkgbuf, p->role_tm, idx);
    init_tr_proto_head(p, p->id, trpkgbuf, idx, tr_safe_trade_action_agree);
    return send_to_trade_svr(p, trpkgbuf, idx, trd_idx);
}

/** 
 * @brief  trade svr callback pkg
 */
int trsvr_safe_trade_action_agree_callback(player_t *p, tr_proto_t * pkg)
{
    safe_trade_action_agree_t *rsp = reinterpret_cast<safe_trade_action_agree_t*>(pkg->body);
	KDEBUG_LOG(p->id, "SAFE TRADE action agree CB\t [%u][%u %u %u|%u %u %u]", rsp->flag, rsp->del_xiaomee, rsp->del_item_cnt, rsp->del_attire_cnt, rsp->add_xiaomee, rsp->add_item_cnt, rsp->add_attire_cnt);

    if (rsp->flag == 1) {
        int idx = sizeof(cli_proto_t);
        pack(pkgbuf, rsp->flag, idx);
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    } else if (rsp->flag == 2) {
        p->waitcmd = 0;
		return 0;
    }

    CHECK_VAL_EQ(pkg->len - sizeof(tr_proto_t), sizeof(safe_trade_action_agree_t) + (rsp->del_item_cnt + rsp->add_item_cnt) * sizeof(trade_item_unit_t) + (rsp->del_attire_cnt + rsp->add_attire_cnt) * sizeof(trade_attire_unit_t));


    p->safe_trade_room_id = 0;
    p->coins -= rsp->del_xiaomee;
    p->coins += rsp->add_xiaomee;
    KDEBUG_LOG(p->id, "XIAOMEE ----> -|%u| +|%u|", rsp->del_xiaomee, rsp->add_xiaomee);

    trade_item_unit_t * del_item = reinterpret_cast<trade_item_unit_t*>(rsp->datas);
    trade_attire_unit_t *del_attire = reinterpret_cast<trade_attire_unit_t*>(rsp->datas + 
        rsp->del_item_cnt * sizeof(trade_item_unit_t));
    trade_item_unit_t * add_item = reinterpret_cast<trade_item_unit_t*>(rsp->datas + 
        rsp->del_item_cnt * sizeof(trade_item_unit_t) + 
        rsp->del_attire_cnt * sizeof(trade_attire_unit_t));
    trade_attire_unit_t * add_attire = reinterpret_cast<trade_attire_unit_t*>(rsp->datas + 
        (rsp->del_item_cnt +rsp->add_item_cnt) * sizeof(trade_item_unit_t) + 
        rsp->del_attire_cnt * sizeof(trade_attire_unit_t));

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->flag, idx);
    pack(pkgbuf, p->coins, idx);
    pack(pkgbuf, rsp->del_item_cnt, idx);
    for (uint32_t i = 0; i < rsp->del_item_cnt; i++) {
        KDEBUG_LOG(p->id, "DEL ----> %u %u", del_item->item_id, del_item->itemcnt);

        p->my_packs->del_item(p, del_item->item_id, del_item->itemcnt, channel_string_trade);

        pack(pkgbuf, del_item->item_id, idx);
        pack(pkgbuf, del_item->itemcnt, idx);
        del_item++;
    }

    pack(pkgbuf, rsp->del_attire_cnt, idx);
    for (uint32_t i = 0; i < rsp->del_attire_cnt; i++) {
        KDEBUG_LOG(p->id, "DEL ----> %u %u %u", del_attire->attire_id, del_attire->unique_id, del_attire->item_lv);

        p->my_packs->del_clothes(del_attire->unique_id, channel_string_trade);

        pack(pkgbuf, del_attire->attire_id, idx);
        pack(pkgbuf, del_attire->unique_id, idx);
        pack(pkgbuf, del_attire->item_lv, idx);
        del_attire++;
    }

    pack(pkgbuf, rsp->add_item_cnt, idx);
    for (uint32_t i = 0; i < rsp->add_item_cnt; i++) {
        KDEBUG_LOG(p->id, "ADD ----> %u %u", add_item->item_id, add_item->itemcnt);

        p->my_packs->add_item(p, add_item->item_id, add_item->itemcnt, channel_string_trade);

        pack(pkgbuf, add_item->item_id, idx);
        pack(pkgbuf, add_item->itemcnt, idx);
        add_item++;
    }

    pack(pkgbuf, rsp->add_attire_cnt, idx);
    for (uint32_t i = 0; i < rsp->add_attire_cnt; i++) {
        KDEBUG_LOG(p->id, "ADD ----> %u %u %u", add_attire->attire_id, add_attire->unique_id, add_attire->item_lv);

        p->my_packs->add_clothes(p, add_attire->attire_id, add_attire->unique_id, 0, channel_string_trade, 
				get_now_tv()->tv_sec, 0, add_attire->item_lv);

        pack(pkgbuf, add_attire->attire_id, idx);
        pack(pkgbuf, add_attire->unique_id, idx);
        pack(pkgbuf, add_attire->item_lv, idx);
        add_attire++;
    }

    init_cli_proto_head(pkgbuf, p, cli_proto_safe_trade_action_agree, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}


