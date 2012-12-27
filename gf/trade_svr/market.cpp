#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
using namespace taomee;
extern "C" {
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
#include <libtaomee/timer.h>
}
#include "fwd_decl.hpp"

#include "player.hpp"
#include "market.hpp"

Market::Market(int id)
{
	market_id = id;
	Shopes = std::vector<Shop>(max_shop_cnt);
	for (uint32_t i = 0; i < max_shop_cnt; i++) {
		Shopes[i].set_shop_id(i + 1);
	}
	player_count = 0;
}

void Market::Player_Enter(Player * p)
{
	players[p->id] = p;
	p->p_market = this;
	player_count++;
	KDEBUG_LOG(p->id, "ENTER MKT\t[%u %u %u]", p->id, market_id, player_count);
	player_enter_map(p);
	trade_sw_player_enter_market(p, market_id);
} 

int Market::list_users(Player* p)
{
	TRACE_LOG("%u %u %u", p->id, market_id, player_count);
	int ix = sizeof(tr_proto_t);
	ix += (sizeof(cli_proto_t) + 4);
	uint32_t cnt = 0;
	for (iterator it = players.begin(); it != players.end(); ++it)
	{
		cnt++;
		it->second->pack_player_map_info(trpkgbuf, ix, 1); 
	}

	int ifx = 0;
	pack(trpkgbuf + sizeof(tr_proto_t) + sizeof(cli_proto_t), cnt, ifx);
	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, ix, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), p->id, cli_proto_list_user, 0, ix-sizeof(tr_proto_t));

	return send_to_player(p, trpkgbuf, ix, 1);
}


void Market::Player_Leave(Player *p)
{
	if (p->p_shop) {
		p->p_shop->leaveShop(p);
	}

	player_count--;
	player_leave_map(p);

	players.erase(p->id);
	KDEBUG_LOG(p->id, "LEAVE MKT\t[%u %u %u]", p->id, market_id, player_count);
	trade_sw_player_leave_market(p, market_id);
}

void Market::player_leave_map(Player *p)
{
	int idx = sizeof(tr_proto_t);
	idx += sizeof(cli_proto_t);
	pack(trpkgbuf, p->id, idx);
	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, idx, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), p->id, cli_proto_leave_map, 0, idx-sizeof(tr_proto_t));
	send_to_market(p, trpkgbuf, idx, 0);
}

void Market::player_enter_map(Player *p)
{
	p->xpos = 1000;
	p->ypos = 400;

	int idx = sizeof(tr_proto_t);
	idx += sizeof(cli_proto_t);
	p->pack_player_map_info(trpkgbuf, idx, 1);
	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, idx, 0);
	init_cli_proto_head_full(trpkgbuf + sizeof(tr_proto_t), p->id, cli_proto_enter_map, 0, idx-sizeof(tr_proto_t));
	send_to_market(0, trpkgbuf, idx, 0);
}

bool Market::is_full()
{
	if (players.size() < max_player_cnt)
	{
		return false;
	}
	return true;
}

Shop * Market::get_shop(uint32_t shop_id)
{
	uint32_t shop_idx = shop_id - 1;
	if (shop_idx > max_shop_cnt - 1) {
		return 0;
	}
	return &Shopes[shop_idx];
}

Shop* Market::get_unused_shop(uint32_t shop_id)
{
	Shop* p_s = get_shop(shop_id);
	if (p_s && p_s->get_shop_status() == shop_status_null) {
		return p_s;
	}
	return 0;
}

Shop* Market::get_used_shop(uint32_t shop_id)
{
	Shop* p_s = get_shop(shop_id);
	if (p_s && p_s->get_shop_status() != shop_status_null) {
		return p_s;
	}
	return 0;
}


void Market::send_to_market(Player* p, uint8_t *buf, uint32_t len, uint32_t completed)
{
	for (iterator it = players.begin(); it != players.end(); ++it)
	{	
		if (!p ||  p != it->second) {
			send_to_player(it->second, buf, len, completed);
		}
	}
}

int Market::list_shops(Player *p)
{
	int ifx = sizeof(tr_proto_t) + 4;
	ifx += sizeof(cli_proto_t);
	uint32_t shop_cnt = 0;
	for (std::vector<Shop>::iterator it = Shopes.begin(); it != Shopes.end(); ++it) {
		if (it ->shop_status) {
			shop_cnt ++;
			it->pack_shop_info(trpkgbuf, ifx);
		}
	}

	KDEBUG_LOG(p->id, "LIST SHOP\t[%u %u]", p->id, shop_cnt);

	int ix = 0;
	pack(trpkgbuf + sizeof(tr_proto_t) + sizeof(cli_proto_t), shop_cnt, ix);
	/*
	init_tr_proto_head_full(trpkgbuf, p->waitcmd, ifx, 0);
	return send_to_player(p, trpkgbuf, ifx, 1);*/
	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, ifx, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), 0, 4009, 0, ifx-sizeof(tr_proto_t));
	return send_to_player(p, trpkgbuf, ifx, 1);
}

void Market::shop_status_change(uint32_t shop_id)  
{
	Shop * i_shop = get_shop(shop_id);

	int idx = 0;

	idx = sizeof(tr_proto_t);
	idx += sizeof(cli_proto_t);

	i_shop->pack_shop_info(trpkgbuf, idx);

	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, idx, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), 0, cli_proto_trade_shop_change, 0, idx-sizeof(tr_proto_t));
	send_to_market(0, trpkgbuf, idx, 0);
}



