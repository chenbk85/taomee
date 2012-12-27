#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
using namespace taomee;
extern "C" {
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
	
}
#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "utils.hpp"
#include "trade.hpp"
#include "market.hpp"
#include "item.hpp"

typedef std::map<uint32_t, Market*> MarketMap;
typedef std::map<uint32_t, Market*>::iterator MarketIterator;

static MarketMap allMarket;

uint8_t trpkgbuf[1<<21];

int trade_switch_fd;

TradeSvr sSvr = singleton<TradeSvr>::instance();


void init_market()
{
	//int server_id = get_server_id();
	for (int i = 0; i < max_market_num; ++i)
	{
		uint32_t market_id = i;
		allMarket[market_id] = new Market(market_id);
	}
}

void fini_market()
{
	for (MarketMap::iterator iter = allMarket.begin(); iter != allMarket.end(); ++iter)
	{
		if (iter->second != 0) {
			delete iter->second;
			iter->second = 0;
		};
	}

	allMarket.clear();
}

void init_trade_svr()
{

	init_market();
	trade_switch_fd = - 1;
	sSvr.init();
	connect_to_trade_switch();
}

void fini_trade_svr()
{
	fini_market();
	sSvr.fini();
}


int send_drop_shop_rsp(Player* p, uint32_t shopid, uint32_t complete)
{
	int idx = sizeof(tr_proto_t);
	pack_h(trpkgbuf, p->sell_cnt, idx);
	pack_h(trpkgbuf, shopid, idx);	
	init_tr_proto_head(trpkgbuf, trd_drop_shop, idx);
	return send_to_player(p, trpkgbuf, idx, complete);
}


/**
* @brief enter market
*/
int enter_market_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	player_info_t * pkg = reinterpret_cast<player_info_t *>(body);
	KDEBUG_LOG(pkg->id, "ENTER MARKET\t[%u %u] WITH COIN [%u]", pkg->id, pkg->market_id, pkg->coins); 
	CHECK_VAL_EQ(bodylen, sizeof(player_info_t) + (pkg->clothes_cnt)* sizeof(clothes_base_t) +  (pkg->summon_cnt)*sizeof(summon_t));
	p->init_player_info(pkg);
	MarketIterator iter = allMarket.find(pkg->market_id);
	if ((iter != allMarket.end()) && !(iter->second->is_full()))
	{
		Market * market = iter->second;
		
		market->Player_Enter(p);
		TRACE_LOG("player enter mark[%u %u]", p->id, pkg->market_id);
		int idx = sizeof(tr_proto_t);
		pack_h(trpkgbuf, pkg->market_id, idx);
	    pack_h(trpkgbuf, (uint32_t)(sSvr.server_id), idx); 	
		init_tr_proto_head(trpkgbuf, p->waitcmd, idx);
		send_to_player(p, trpkgbuf, idx, 1);

	//	market->Player_Enter(p);
		KDEBUG_LOG(p->id, "ENTERED MARKET\t[%u %u %u]", p->id, sSvr.server_id, p->p_market->market_id);
		return 0;

	} else {
		WARN_LOG("ERROR MARKET\t[%u %u]", p->id, pkg->market_id);
		send_header_to_player(p, p->waitcmd, cli_err_market_full, 1);
		del_player(p);
		return 0;
	}
}


/** 
 * @brief get all the usr info in the market
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int list_user_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	if (p->p_market) {
		return p->p_market->list_users(p);
	} else {
		return send_header_to_player(p, p->waitcmd, cli_err_wrong_market_id, 1);
	}
}

/**
* @brief change market in the same trade svr
*/
int change_market_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t market_id = 0;
	int idx = 0;
	unpack_h(body, market_id, idx);
	MarketIterator iter = allMarket.find(market_id);
	if ((iter != allMarket.end()) && !(iter->second->is_full()))
	{
		Market * market = iter->second;
		p->p_market->Player_Leave(p);
		market->Player_Enter(p);
		p->p_market = market;
		
		int ifx = sizeof(tr_proto_t);
		pack_h(trpkgbuf, market->market_id, ifx);
		pack_h(trpkgbuf, sSvr.server_id, ifx);
		init_tr_proto_head(trpkgbuf, p->waitcmd, ifx);
		send_to_player(p, trpkgbuf, ifx, 1);

		return 0;
	}
	else {
		send_header_to_player(p, p->waitcmd, cli_err_market_full, 1);
		return 0;
	}
}

/** 
 * @brief player leave  trade server
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int leave_market_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{		
	send_header_to_player(p, p->waitcmd, 0, 1);
	del_player(p);
	return 0;
}


/**
* @brief set up a shop
*/
int setup_shop_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t shop_id = 0;
	int idx = 0;
	unpack_h(body, shop_id, idx);
	
	if (p->p_market && !p->p_shop) {
		Shop * i_shop  = p->p_market->get_unused_shop(shop_id);
        if (i_shop && i_shop->setupShop(p)) {
			p->p_market->shop_status_change(shop_id);
			p->player_setup_shop();

			int idx_1 = 0;
			pack_h(dbpkgbuf, setup_shop_cost, idx_1); 

			send_request_to_db(0, p->id, p->role_tm, dbproto_setup_shop, dbpkgbuf, idx_1);
			uint32_t left_tm = max_shop_keep_time_limit;
            int idx = sizeof(tr_proto_t);
            pack_h(trpkgbuf, shop_id, idx);
			pack_h(trpkgbuf, left_tm, idx);
			pack_h(trpkgbuf, setup_shop_cost, idx);
            init_tr_proto_head(trpkgbuf, trd_setup_shop, idx);
            return send_to_player(p, trpkgbuf, idx, 1);
		} else {
            TRACE_LOG("shop is in time limit. uid=[%u] shopid=[%u]",p->id,shop_id);
            return send_header_to_player(p, p->waitcmd, cli_err_shop_in_tmlimit, 1);
        }
	} else {
		KDEBUG_LOG(p->id, "U ALREADY HAVE SHOP\t[uid=%u shopid=%u:%u", p->id, p->p_shop->get_shop_id(), shop_id);
        return send_header_to_player(p, p->waitcmd, cli_err_shop_in_tmlimit, 1);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
}

/**
* @brief update shop
*/
int update_shop_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	update_shop_t* p_header = reinterpret_cast<update_shop_t *>(body);

	CHECK_VAL_EQ(bodylen, sizeof(update_shop_t) + 
		p_header->itm_cnt * sizeof(update_shop_item_in_t) + 
		p_header->attire_cnt * sizeof(update_shop_attire_in_t));

	if (p->p_market && p->p_shop) {
		p->p_shop->clear_all_goods();
		update_shop_item_in_t* p_item_in = reinterpret_cast<update_shop_item_in_t *>(body + sizeof(update_shop_t));
		for (uint32_t i = 0; i < p_header->itm_cnt; i++) {
			goods_info_t goods;
			goods.item_type = item_type_normal;
			goods.item_id = p_item_in->item_id;
			goods.cnt = p_item_in->item_cnt;
			goods.price = p_item_in->price;
			if (!(p->p_shop->set_grid_goods(p_item_in->grid_idx, goods)) || !(p_item_in->item_cnt) ||
				!(p_item_in->price)) {
				ERROR_LOG("grid id err=[%u] gid=[%u]",p->id, p_item_in->grid_idx);
       			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
			}
			p_item_in++;
		}

		update_shop_attire_in_t* p_attire_in = 
			reinterpret_cast<update_shop_attire_in_t *>(body + sizeof(update_shop_t) + sizeof(update_shop_item_in_t) * p_header->itm_cnt);
		for (uint32_t i = 0; i < p_header->attire_cnt; i++) {
			goods_info_t goods;
			goods.item_type = item_type_clothes;
			goods.item_id = p_attire_in->attire_id;
			goods.unique_id = p_attire_in->unique_id;
			goods.item_lv = p_attire_in->lv;
			goods.price = p_attire_in->price;
			goods.duration = p_attire_in->duration;
            goods.cnt = 1;//for attire

           TRACE_LOG("%u %u %u %u %u %u", p_attire_in->grid_idx, p_attire_in->attire_id, p_attire_in->unique_id, p_attire_in->lv, p_attire_in->duration, p_attire_in->price);
			if (!(p->p_shop->set_grid_goods(p_attire_in->grid_idx, goods)) ||
				!(p_attire_in->attire_id) || !(p_attire_in->unique_id) ||
				!(p_attire_in->price)) {
				ERROR_LOG("grid id err=[%u] gid=[%u]",p->id, p_attire_in->grid_idx);
       			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
			}
			p_attire_in++;
		}
		p->p_shop->set_shop_open();
		//send to all player int this market shop change status
		p->p_market->shop_status_change(p->p_shop->get_shop_id());
		return send_header_to_player(p, p->waitcmd, 0, 1);
	} else {
		TRACE_LOG("player enter mark error market id[%u %u %u]", p->id, p_header->market_id, p_header->market_id);
		return send_header_to_player(p, p->waitcmd, cli_err_have_no_shop, 1);
	}

}



/**
 * @brief player buy goods action
 */
int buy_goods_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
    int ret = 0;
    buy_goods_rsp_t *rsp = reinterpret_cast<buy_goods_rsp_t*>(body);
    Shop *shop = p->p_market->get_shop(rsp->shop_id);
    if (shop == NULL) {
        ERROR_LOG("ship_id=[%u] not exist",rsp->shop_id);
        return -1;
    }

    Player* shopkeeper = shop->get_shopKeeper();
    if ( shopkeeper == NULL ) {
        TRACE_LOG("shopid=[%u] shopkeeper not exist", rsp->shop_id);
        return send_header_to_player(p, p->waitcmd, cli_err_wrong_shop_id, 1);
    }
	if (shop->is_shop_constructing()) {
		TRACE_LOG("shop is constructing uid=[%u] gid=[%u]", p->id, shop->get_shop_id());
        return send_header_to_player(p, p->waitcmd, cli_err_shop_not_open, 1);
	}
    goods_info_t* my_goods = shop->get_goods(rsp->goods_id);
	if (!my_goods || !(rsp->coins) || !(rsp->goods_cnt)) {
		ERROR_LOG("grid id err=[%u] gid=[%u]",p->id, rsp->goods_id);
        return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
    //my_goods->cnt = rsp->goods_cnt;
    TRACE_LOG("uid=[%u] gid=[%u] [%u %u %u %u]",p->id,rsp->goods_id,
        my_goods->item_id, my_goods->unique_id, my_goods->price, my_goods->cnt);
    if (rsp->itemid != my_goods->item_id || rsp->uniqueid != my_goods->unique_id 
        || rsp->coins != my_goods->price || rsp->goods_cnt > my_goods->cnt) {
        TRACE_LOG("SHOP grid has outdated! shopid=[%u] grid=[%u]",rsp->shop_id, rsp->goods_id);
        db_buy_goods_rsp_t out = {0};
        int idx = sizeof(tr_proto_t);
        pack(trpkgbuf, static_cast<uint8_t>(1), idx);
        memcpy(trpkgbuf + idx, (char *)(&out), sizeof(db_buy_goods_rsp_t));
        idx += sizeof(db_buy_goods_rsp_t);
        init_tr_proto_head(trpkgbuf, trd_buy_goods, idx);
        return send_to_player(p, trpkgbuf, idx, 1);
    }
    if (shop->is_locked_goods(rsp->goods_id, rsp->goods_cnt)) {
        TRACE_LOG("goods is locked uid=[%u] gid=[%u]",p->id,rsp->goods_id);
        return send_header_to_player(p, p->waitcmd, cli_err_item_in_tradeing, 1);
    }
    //lock ...
    shop->lock_goods(rsp->goods_id, rsp->goods_cnt);
    //unlock ...

	//stat log
	do_stat_log(stat_log_shop_daily_exchange_coins, my_goods->price * rsp->goods_cnt);
	
    int idx_1 = 0, idx_2 = 0;
    if (my_goods->unique_id != 0) {
        pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx_1);
        pack_h(trpkgbuf, static_cast<uint32_t>(1), idx_2);
    } else {
        pack_h(dbpkgbuf, static_cast<uint32_t>(2), idx_1);
        pack_h(trpkgbuf, static_cast<uint32_t>(2), idx_2);
    }
	pack_h(dbpkgbuf, rsp->goods_id, idx_1);
    pack_h(dbpkgbuf, my_goods->item_id, idx_1);
    pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx_1);
    pack_h(dbpkgbuf, rsp->goods_cnt, idx_1);
    pack_h(dbpkgbuf, my_goods->duration, idx_1);
    pack_h(dbpkgbuf, static_cast<uint32_t>(my_goods->item_lv), idx_1);
    pack_h(dbpkgbuf, static_cast<uint32_t>(time(NULL)), idx_1);
    pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx_1);
    pack_h(dbpkgbuf, my_goods->price, idx_1);
    pack_h(dbpkgbuf, rsp->max_bag, idx_1);
    ret = send_request_to_db(p, p->id, p->role_tm, dbproto_buy_goods, dbpkgbuf, idx_1);
    if (ret != 0) {
        return ret;
    }

	pack_h(trpkgbuf, shop->shop_start_tm, idx_2);
    pack_h(trpkgbuf, rsp->shop_id, idx_2);
    pack_h(trpkgbuf, rsp->goods_id, idx_2);
    pack_h(trpkgbuf, my_goods->item_id, idx_2);
    pack_h(trpkgbuf, my_goods->unique_id, idx_2);
	pack_h(trpkgbuf, static_cast<uint32_t>(my_goods->item_lv), idx_2);
    pack_h(trpkgbuf, rsp->goods_cnt, idx_2);
    pack_h(trpkgbuf, my_goods->price, idx_2);
   	KDEBUG_LOG(p->id, "BUY GOODS\t[buyer=%u seller=%u item=%u %u %u %u price=%u",
        p->id, shopkeeper->id, my_goods->item_id,rsp->goods_id,my_goods->unique_id,rsp->goods_cnt,my_goods->price);
    return send_request_to_db(shopkeeper, shopkeeper->id, shopkeeper->role_tm, dbproto_sell_goods, trpkgbuf, idx_2);
}

/**
* @brief walk
*/
int walk_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y, flag;
	unpack_h(body, x, i);
	unpack_h(body, y, i);
	unpack_h(body, flag, i);

	if (p->p_market) {
		p->walk(x, y, flag);
	}
	p->waitcmd = 0;
	return 0;

}

/**
* @brief walk keyboard
*/
int walk_keyboard_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y;
	uint8_t dir;
	uint8_t state;
	unpack_h(body, x, i);
	unpack_h(body, y, i);
	unpack_h(body, dir, i);
	unpack_h(body, state, i);

	if (p->p_market) {
		p->walk_keyboard(x, y, dir, state);
	}
	p->waitcmd = 0;
	return 0;
}


/**
* @brief stand
*/
int stand_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y;
	uint8_t dir;
	unpack_h(body, x, i);
	unpack_h(body, y, i);
	unpack_h(body, dir, i);

	if (p->p_market) {
		p->stand(x, y, dir);
	}
	p->waitcmd = 0;
	return 0;
}


/**
* @brief jump
*/
int jump_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t x, y;
	unpack_h(body, x, i);
	unpack_h(body, y, i);

	if (p->p_market) {
		p->jump(x, y);
	}
	p->waitcmd = 0;
	return 0;
}

/**
* @brief talk
*/
int talk_cmd(Player* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	userid_t id;
	uint32_t len;
	unpack_h(body, id, i);
	unpack_h(body, len, i);

	if (p->p_market) {
		p->talk(body + 8, len, id);
	}
	p->waitcmd = 0;
	return 0;
}

int  connect_to_trade_switch()
{
	if (trade_switch_fd == -1) {
		trade_switch_fd = connect_to_service(config_get_strval("trade_switch_name"), 0, 65535, 1);
		if (trade_switch_fd == -1) {
			return -1;
		}
		send_init_pkg();
	}
	return 0;
}

inline void init_tr_sw_proto_head(Player *p, uint8_t * buf, uint32_t len,  uint16_t cmd, uint32_t ret)
{
	trade_sw_proto_t * pkg = reinterpret_cast<trade_sw_proto_t*>(buf);
	pkg->len = len;
	pkg->seq = (p? ((p->fd)<<16) | (p->waitcmd) : 0); 
	pkg->ret = ret;
	pkg->cmd = cmd;
	pkg->sender_id = (p? p->id : 0); 
}	

int send_to_trade_switch(uint8_t *buf, uint32_t len)
{
	if (trade_switch_fd == -1) {
		if (connect_to_trade_switch() == -1) {
			ERROR_LOG("send to trade switch failed");
			return -1;
		}
	}

	return 	net_send(trade_switch_fd, buf, len);
}

/** 
 * @brief send server id and market player count to trade switch when the frist connecting
 * 
 * @return 
 */
int send_init_pkg()
{	
	int idx = sizeof(trade_sw_proto_t);
 	pack_h(trpkgbuf, sSvr.server_id, idx);
	for (MarketIterator iter = allMarket.begin(); iter != allMarket.end(); ++iter) {
			pack_h(trpkgbuf, iter->second->player_count, idx);
	}
	init_tr_sw_proto_head(0, trpkgbuf, idx, tr_sw_init_pkg, 0); 
 	return send_to_trade_switch(trpkgbuf, idx);   // sender id 0
}

/*
int trade_sw_keep_trade_alive()
{
	int idx = sizeof(trade_sw_proto_t);
	init_tr_sw_proto_head(0, trpkgbuf, idx, tr_sw_keep_trade_alive, 0);
	return send_to_trade_switch(trpkgbuf, idx); 
}*/	

/** 
 * @brief send to trade switch player enter a market
 * 
 * @param p
 * @param market_id
 * 
 * @return 
 */
int trade_sw_player_enter_market(Player *p, uint32_t market_id)
{
	int idx = sizeof(trade_sw_proto_t);
	pack_h(trpkgbuf, sSvr.server_id, idx);
	pack_h(trpkgbuf, market_id, idx);
	TRACE_LOG("send to trade switch player enter market %u %u", p->id, market_id);
	init_tr_sw_proto_head(p, trpkgbuf, idx, tr_sw_player_enter_market, 0);
	return  send_to_trade_switch(trpkgbuf, idx);
}

/** 
 * @brief send to trade switch player leave current market
 * 
 * @param p
 * @param market_id
 * 
 * @return 
 */
int trade_sw_player_leave_market(Player *p, uint32_t market_id)
{
	int idx = sizeof(trade_sw_proto_t);
	pack_h(trpkgbuf, sSvr.server_id, idx); 
	pack_h(trpkgbuf, market_id, idx); 
	TRACE_LOG("send to trade switch player leave market %u %u", p->id, market_id);
	init_tr_sw_proto_head(p, trpkgbuf, idx, tr_sw_player_leave_market, 0);
	return  send_to_trade_switch(trpkgbuf, idx);
}

/*int trade_sw_player_change_market(Player *p, uint32_t market_id)
{
	int idx = sizeof(trade_sw_proto_t);
	pack_h(trpkgbuf, sSvr.server_id, idx); 
	pack_h(trpkgbuf, market_id, idx); 
	init_tr_sw_proto_head(p, trpkgbuf, idx, tr_sw_player_change_market, 0);
	return  send_to_trade_switch(trpkgbuf, idx);
} */

/**
 * @brief player buy goods db callback
 * @return 0 on success, -1 on error
 */
int db_buy_goods_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    if (ret != 0) {
        WARN_LOG("SELLCALLBACK BUYER\t[%u %u]",id, ret);
        CHECK_DBERR(p, ret);
    }
    
    db_buy_goods_rsp_t * rsp = reinterpret_cast<db_buy_goods_rsp_t*>(body);
    KDEBUG_LOG(id, "SELLCALLBACK BUYER\t[%u item=%u %u %u xiaomee=%u]", id, rsp->grid_id, rsp->itemid, rsp->cnt, rsp->leftcoins);
	int idx = sizeof(tr_proto_t);
    pack(trpkgbuf, static_cast<uint8_t>(0), idx);
    memcpy(trpkgbuf + idx, body, bodylen);
    idx += bodylen;
    init_tr_proto_head(trpkgbuf, trd_buy_goods, idx);
    return send_to_player(p, trpkgbuf, idx, 1);
}

/**
 * @brief player sell goods db callback
 * @return 0 on success, -1 on error
 */
int db_sell_goods_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
	do_stat_log(stat_log_shop_exchange_times, 1);
	do_stat_log2(stat_log_shop_daily_exchange_times, 1, p->id);
	
    Shop* shop = p->p_shop;
    db_sell_goods_rsp_t * rsp = reinterpret_cast<db_sell_goods_rsp_t*>(body);

	if (!shop) {
		return send_header_to_player(p, trd_sell_goods, cli_err_have_no_shop, 0);
	}

	if (rsp->error != 0) {
		WARN_LOG("SELLCALLBACK SELLER\t[%u %u %u %u]", id, ret, rsp->goods_id, rsp->cnt);
		//lock ...
		shop->unlock_goods(rsp->goods_id, rsp->cnt);
		//unlock ...
		CHECK_DBERR(p, rsp->error);
	}

	
	
	int idx = sizeof(tr_proto_t);
    memcpy(trpkgbuf + idx, body, bodylen);
    idx += bodylen;
    init_tr_proto_head(trpkgbuf, trd_sell_goods, idx);
	send_to_player(p, trpkgbuf, idx, 0);
	
	p->sell_cnt++;
    KDEBUG_LOG(id, "SELLCALLBACK SELLER\t[%u item=%u %u xiaomee=%u]", id, rsp->itemid, rsp->cnt, rsp->leftcoins);
//	uint32_t price = shop->get_goods_price(rsp->goods_id);
	if (rsp->type == 1) {
		shop->delete_goods(rsp->goods_id, 1);
	    //lock ...
	    shop->unlock_goods(rsp->goods_id, 1);
	    //unlock ...
	} else {
		shop->delete_goods(rsp->goods_id, rsp->cnt);
	    //lock ...
	    shop->unlock_goods(rsp->goods_id, rsp->cnt);
	    //unlock ...
	}

//	save_sell_goods_log(p, shop->shop_start_tm, rsp->type, rsp->itemid, rsp->uniquekey, rsp->cnt, price);
    return 0;
}

/** 
 * @brief handle the cmd player get all the shop simple info
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int list_shop_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	TRACE_LOG("player [%u list shop ]", p->id);
	if ( p->p_market )
	{
		return	p->p_market->list_shops(p);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
}

/** 
 * @brief handle pkg decorate a shop
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int decorate_shop_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t shop_id = 0;
	uint32_t decorate_id = 0;
	unpack_h(body, shop_id, idx);
	unpack_h(body, decorate_id, idx);

	if (p->p_shop && p->p_market && p->p_market->get_shop(shop_id) == p->p_shop)
	{
		p->p_shop->decorate(decorate_id);
		p->p_market->shop_status_change(shop_id);

		int idx = sizeof(tr_proto_t);
	    pack_h(trpkgbuf, shop_id, idx); 	
		pack_h(trpkgbuf, decorate_id, idx); 
		KDEBUG_LOG(p->id, "DECORATE SHOP\t[%u %u %u]", p->id, shop_id, decorate_id);
		init_tr_proto_head(trpkgbuf, p->waitcmd, idx);
		return send_to_player(p, trpkgbuf, idx, 1);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_have_no_shop, 1);
}

/** 
 * @brief playe get all the goods info in a shop
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int get_shop_detail_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	uint32_t shopid = 0;
	int idx = 0;
	unpack_h(body, shopid, idx);
	Shop * shop = p->p_market->get_shop(shopid);
	if ( p->p_market && shop && !(shop->is_shop_constructing())) {
		return shop->list_shop_goods_info(p);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_shop_not_open, 1);
}

/** 
 * @brief pause a shop
 * 
 * @return 
 */
int pause_shop_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	uint32_t shopid = 0;
	int idx = 0;
	unpack_h(body, shopid, idx);
	if (p->p_market && p->p_shop && p->p_shop == p->p_market->get_shop(shopid))
	{ 
		p->p_shop->pauseShop();
		//p->p_shop->pauseShop();
		p->p_market->shop_status_change(shopid);
		int idx = sizeof(tr_proto_t);
	    pack_h(trpkgbuf, shopid, idx); 	
		KDEBUG_LOG(p->id, "SHOP PAUSE [%u %u]", p->id, shopid);
		init_tr_proto_head(trpkgbuf, p->waitcmd, idx);
		return send_to_player(p, trpkgbuf, idx, 1);
	}

	return send_header_to_player(p, p->waitcmd, cli_err_have_no_shop, 1);
} 

/** 
 * @brief  drop shop
 * 
 * @return 
 */
int drop_shop_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	uint32_t shopid = 0;
	int idx = 0;
	unpack_h(body, shopid, idx);
	if (p->p_market && p->p_shop == p->p_market->get_shop(shopid))
	{
		p->p_shop->leaveShop(p);
		p->player_drop_shop(shopid);

		return send_drop_shop_rsp(p, shopid);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_have_no_shop, 1);
}

/** 
 * @brief change shop name
 * 
 */
int change_shop_name_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	uint32_t shopid = 0;
	int idx = 0;
	unpack_h(body, shopid, idx);
	if (p->p_market && p->p_shop == p->p_market->get_shop(shopid))
	{
		KDEBUG_LOG(p->id, "CHANGE SHOP NAME\t[%u %u]", p->id, shopid);
		p->p_shop->change_name(body + idx, bodylen - idx);
		p->p_market->shop_status_change(shopid);
		int ifx = sizeof(tr_proto_t);
	    pack_h(trpkgbuf, shopid, ifx); 	
		pack(trpkgbuf, body + idx, bodylen - idx, ifx);
		init_tr_proto_head(trpkgbuf, p->waitcmd, ifx);
		return send_to_player(p, trpkgbuf, ifx, 1);
	}
	return send_header_to_player(p, p->waitcmd, cli_err_have_no_shop, 1);

}

/** 
 * @brief update usr's clothes info from online to trade 
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int wear_clothes_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	wear_clothes_rsp_t* rsp = reinterpret_cast<wear_clothes_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(wear_clothes_rsp_t) + rsp->clothes_cnt * sizeof(clothes_rsp_t));

	KDEBUG_LOG(p->id, "WEAR CLOTHES\t uid=[%u]", p->id);
	p->clothes_cnt = rsp->clothes_cnt;
	for (uint32_t i = 0; i < p->clothes_cnt; ++i) {
		p->clothes_arr[i].id = rsp->clothes[i].clothes_id;
		p->clothes_arr[i].lv = rsp->clothes[i].attirelv;
	}

	//send to market player wear clothes
	int idx = sizeof(tr_proto_t);
	idx += sizeof(cli_proto_t);
	pack(trpkgbuf, rsp->clothes_cnt, idx);
	for (uint32_t i = 0; i < rsp->clothes_cnt; ++i) {
		clothes_rsp_t* elem = &(rsp->clothes[i]);                                      
		pack(trpkgbuf, elem->clothes_id, idx);                                                    
		pack(trpkgbuf, elem->unique_id, idx);                                                     
		pack(trpkgbuf, elem->duration, idx);                                                      
		pack(trpkgbuf, elem->attirelv, idx); 
		TRACE_LOG("playe wear clothes[%u %u]", p->id, elem->clothes_id);
	}

	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, idx, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), p->id, cli_proto_wear_clothes, 0, idx-sizeof(tr_proto_t));
	if (p->p_market)
	{
		p->p_market->send_to_market(0, trpkgbuf, idx, 0);
		p->waitcmd = 0;
	}
	return 0;
}

/** 
 * @brief syn usr's fight summon, and send to all the player in this market
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int change_summon_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	change_summon_rsp_t * rsp = reinterpret_cast<change_summon_rsp_t*>(body);
	p->set_fight_summon(rsp->mon_tm);
	int idx = sizeof(tr_proto_t);
	idx += sizeof(cli_proto_t);
	pack(trpkgbuf, rsp->mon_type, idx);
	pack(trpkgbuf, rsp->mon_lv, idx);
	pack(trpkgbuf, rsp->mon_nick, max_nick_size, idx);
	KDEBUG_LOG(p->id, "CHANGE SUMMON [%u %u]", p->id, rsp->mon_type);
	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, idx, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), p->id, cli_proto_summon_change, 0, idx-sizeof(tr_proto_t));
	if (p->p_market)
	{
		p->p_market->send_to_market(p, trpkgbuf, idx, 0);
		p->waitcmd = 0;
	}
	return 0;
}

/** 
 * @brief set usr's fight summon call_flag and also syn to all the player in market
 * 
 * @param p
 * @param body
 * @param bodylen
 * 
 * @return 
 */
int call_summon_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	uint8_t call_flag = 0;
	int ix = 0;
	unpack_h(body, call_flag, ix);
	if (p->fight_mon) {
		p->fight_mon->call_flag = call_flag;
	}

	int idx = sizeof(tr_proto_t);
	idx += sizeof(cli_proto_t);
	pack(trpkgbuf, call_flag, idx);
	KDEBUG_LOG(p->id, "CALL SUMMON\t uid=[%u]", p->id);
	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, idx, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), p->id, cli_proto_call_summon, 0, idx-sizeof(tr_proto_t));
	if (p->p_market)
	{
		p->p_market->send_to_market(0, trpkgbuf, idx, 0);
		p->waitcmd = 0;
	}
	return 0;

}


