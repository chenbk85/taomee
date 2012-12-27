#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
using namespace taomee;
extern "C" {
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
#include <libtaomee/timer.h>
}
#include "fwd_decl.hpp"

#include "shop.hpp"
#include "market.hpp"


Shop::Shop()
{
	shop_status = shop_status_null;
	shopKeeper = 0;
	goods = std::vector<goods_info_t>(max_shop_sell_item_cnt);
	clear_all_goods();

	decoration_id = 0;
	lock_num_ = 0;
	time_limit_ev = 0;
	memset(shop_name, 0, max_shop_name_size);
}

/** 
 * @brief player drop a shop
 * 
 * @param p
 */
void Shop::leaveShop(Player* p)
{
	KDEBUG_LOG(p->id, "LEAVE SHOP\t[%u %u]", p->id, get_shop_id());
	do_stat_log2(stat_log_shop_last_time, p->id, get_now_tv()->tv_sec - shop_start_tm);
	
	historyShopKeeper keeper;
	keeper.uid = shopKeeper->id; 
	keeper.history_tm = get_now_tv()->tv_sec;

	oldKeeper.push_back(keeper);
	while ( oldKeeper.begin() != oldKeeper.end() && oldKeeper.front().history_tm < (get_now_tv()->tv_sec - max_interval_for_setup_shop))
	{
		oldKeeper.pop_front();
	}
	set_shop_close();

	p->p_shop = 0;
//	p->invisible = 0;
	delete_keep_limit_timer();
	shopKeeper = 0;
	shop_start_tm = 0;
	clear_all_goods();

	p->p_market->shop_status_change(get_shop_id());
	return;
};

/** 
 * @brief player setup a shop 
 * 
 * @param p
 * 
 * @return 
 */
bool Shop::setupShop(Player * p)
{
	if (shopKeeper == 0 && check_history_keeper(p->id))
	{
		KDEBUG_LOG(p->id, "SET UP SHOP\t[uid=%u shop=%u]", p->id, get_shop_id());
		do_stat_log(stat_log_setup_shop_times, 1);
		
		shop_start_tm = get_now_tv()->tv_sec;
		shopKeeper = p;

		p->coins -= setup_shop_cost;
		set_shop_constructing();
		p->p_shop = this;
		p->sell_cnt = 0;
//		p->invisible = 1;
		add_keep_limit_timer();
		return true;
	}
	return false;
}

void Shop::startShop()
{
	shop_status = shop_status_open; 
}

void Shop::pauseShop()
{
	shop_status = shop_status_constructing;
}

bool Shop::check_history_keeper(uint32_t uid)
{
	for(iterator iter = oldKeeper.begin(); iter != oldKeeper.end(); ++iter)
	{
		if (iter->uid == uid)
		{
			return false;
		}
	}
	return true;
}

void Shop::add_keep_limit_timer()
{
	// test a player's latency at an interval of 1 hour
	uint32_t interval = max_shop_keep_time_limit;
	timeval tv = *get_now_tv();
	tv.tv_sec += interval;
	uint8_t type = 0;
	TRACE_LOG("%u %u", id_, shopKeeper->id);
	time_limit_ev = ev_mgr.add_event(*this, &Shop::proc_keeper_time_limit, type, tv);
}

void Shop::delete_keep_limit_timer()
{
	// test a player's latency at an interval of 1 hour
	if (time_limit_ev) {
		TRACE_LOG("delete timer:%u %u %p", id_, shopKeeper->id, time_limit_ev);
		ev_mgr.remove_event(time_limit_ev);
		time_limit_ev = 0;
	}
}

int Shop::proc_keeper_time_limit(uint8_t& type)
{
	Player* p = shopKeeper;
	//timeval now_tv = *get_now_tv();
	time_limit_ev = 0;
	if (!type) {
	//set shop pause and try to close shop 
		TRACE_LOG("first:%u %u", id_, p->id);
		set_shop_constructing();
		if (has_locked_goods()) {
			uint32_t interval = max_check_keep_time_interval;
			timeval tv = *get_now_tv();
			tv.tv_sec += interval;
			uint8_t type = 1;
			time_limit_ev = ev_mgr.add_event(*this, &Shop::proc_keeper_time_limit, type, tv, interval); 		
		} else {
			p->player_drop_shop(get_shop_id());
			send_drop_shop_rsp(p, get_shop_id(), 0);
			leaveShop(p);
		}
	} else {
	//try to close shop
		TRACE_LOG("second:%u %u", id_, p->id);
		if (has_locked_goods()) {
			WARN_LOG("locket goods too long shop:%u p:%u", id_, p->id);
		} else {
			p->player_drop_shop(get_shop_id());
			send_drop_shop_rsp(p, get_shop_id(), 0);
			leaveShop(p);	
		}
	}
	return 0;
}


goods_info_t* Shop::get_goods(uint32_t grid_idx)
{
	if (grid_idx >= max_shop_sell_item_cnt) {
		return 0;
	}
	return &(goods[grid_idx]);
}

Player *  Shop::get_shopKeeper()
{
	return shopKeeper;
}

int Shop::delete_goods(uint32_t idx, uint32_t cnt)
{
	if (goods[idx].cnt == 0 || goods[idx].cnt < cnt) {
		return -1;
	} 
	if (goods[idx].cnt == cnt) {
		goods[idx].item_id = 0;
		goods[idx].item_tm = 0;
		goods[idx].item_lv = 0;
		goods[idx].price   = 0;
	}
	goods[idx].cnt -= cnt;
	return 0;
}

uint32_t Shop::get_goods_price(uint32_t idx)
{
	if (goods[idx].cnt == 0) {
		return 0;
	} 
	return goods[idx].price;	
}

/** 
 * @brief lock the goods cnt before db sell callback
 * 
 * @param idx the grid of the shop goods
 * @param cnt 
 */
void Shop::lock_goods(uint32_t idx, uint32_t cnt)
{
	goods[idx].lock.flg = 1;
	goods[idx].lock.cnt += cnt;
	lock_num_ ++;
}

/** 
 * @brief unlock the goods cnt from db sell callback
 * 
 * @param idx the grid of the shop goods

 * @param cnt
 */
void Shop::unlock_goods(uint32_t idx, uint32_t cnt)
{
	lock_num_--; //num of locks --

	if (goods[idx].lock.cnt > cnt) {
		goods[idx].lock.cnt -= cnt;
	} else {
		goods[idx].lock.flg = 0;
		goods[idx].lock.cnt = 0;
	}
	
}

/** 
 * @brief check whether  goods has been locked
 * 
 * @param idx
 * @param cnt
 * 
 * @return 
 */
bool Shop::is_locked_goods(uint32_t idx, uint32_t cnt) 
{
	if (goods[idx].item_tm != 0 && goods[idx].lock.flg == 1) {
		return true;
	}
	if (goods[idx].item_tm == 0 && goods[idx].lock.flg == 1 
		&& (goods[idx].cnt - goods[idx].lock.cnt < cnt)) {
		return true;
	}
	return false;
}

/** 
 * @brief check is there a lock in the shop
 * 
 * @return 
 */
bool Shop::has_locked_goods()
{
	return (lock_num_ > 0);
}



/** 
 * @brief clear all the goods info when usr drop the shop
 */
void Shop::clear_all_goods()
{
	for (uint32_t i = 0; i < max_shop_sell_item_cnt; i++) {
		memset(&goods[i], 0, sizeof(goods_info_t));
	}
}


bool Shop::set_grid_goods(uint32_t grid_idx, goods_info_t& goods_in)
{
	TRACE_LOG("%u %u %u %u", goods_in.item_id, goods_in.unique_id, goods_in.cnt, goods_in.price);
	if (grid_idx < max_shop_sell_item_cnt) {
		goods[grid_idx] = goods_in;
		return true;
	}
	return false;
}

void Shop::pack_shop_info(uint8_t * body, int &idx)
{
	uint32_t keeper = 0;
	uint32_t role_type = 0;
	uint32_t role_tm = 0;
	if (shopKeeper != 0)
	{
		keeper = shopKeeper->id;
		role_tm = shopKeeper->role_tm;
		role_type = shopKeeper->role_type;
	}			
	pack(body, id_, idx);
	pack(body, keeper, idx);
	pack(body, role_tm, idx);
	pack(body, role_type, idx);
	pack(body, shop_status, idx);
	pack(body, decoration_id, idx);
	pack(body, shop_name, max_shop_name_size, idx);

	TRACE_LOG("%u %u %u %u %u %u %s", id_, keeper, role_tm, role_type, shop_status, decoration_id, shop_name);
}

void Shop::decorate(uint32_t decoration)
{
	decoration_id = decoration;
}

int Shop::list_shop_goods_info(Player *p)
{
	uint32_t use_tm = get_now_tv()->tv_sec - shop_start_tm;
	uint32_t left_tm = use_tm > max_shop_keep_time_limit ? 0 : max_shop_keep_time_limit - use_tm;
	int idx = sizeof(tr_proto_t);
	uint32_t goods_c = 0;
	pack(trpkgbuf, left_tm, idx);
	idx += 4;
	for ( uint32_t i = 0 ; i < max_shop_sell_item_cnt; ++i) {
		if (goods[i].item_id != 0)
		{
			goods_c++;
			pack(trpkgbuf, i, idx);
			pack(trpkgbuf, (goods[i]).item_id, idx); 
			pack(trpkgbuf, (goods[i]).unique_id, idx);
			pack(trpkgbuf, (goods[i]).item_lv, idx);
			pack(trpkgbuf, (goods[i]).duration, idx);
			pack(trpkgbuf, (goods[i]).price, idx);
			pack(trpkgbuf, (goods[i]).cnt, idx);
			TRACE_LOG(" goods %u %u %u %u %u %u %u", i, goods[i].item_id, goods[i].unique_id, goods[i].item_lv, goods[i].duration, goods[i].price, goods[i].cnt);
		}	

	}
	KDEBUG_LOG(p->id, "LIST GOOD\t[%u %u %u]", p->id, left_tm, goods_c);
	int ix = 0;
	pack(trpkgbuf + sizeof(tr_proto_t) + 4, goods_c, ix);
	init_tr_proto_head_full(trpkgbuf, p->waitcmd, idx, 0);
	return send_to_player(p, trpkgbuf, idx, 1); 			

}

void Shop::change_name(uint8_t * buf, uint32_t len)
{
	memcpy(shop_name, buf, sizeof(shop_name));
}



