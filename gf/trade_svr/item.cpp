#include <libtaomee++/inet/pdumanip.hpp>
using namespace taomee;
#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "utils.hpp"
#include "item.hpp"
#include "player.hpp"
#include "trade.hpp"

/*! the pointer to ItemManager for handling all items*/
GfItemManager* items;

void save_sell_goods_log(Player* p, uint32_t shop_start_tm, uint32_t type, uint32_t itemid, uint32_t uniquekey, uint32_t cnt, uint32_t price)
{
	int idx = 0;
	uint32_t sell_tm = get_now_tv()->tv_sec;
	pack_h(dbpkgbuf, shop_start_tm, idx);
	pack_h(dbpkgbuf, type, idx);
	pack_h(dbpkgbuf, itemid, idx);
	pack_h(dbpkgbuf, uniquekey, idx);
	pack_h(dbpkgbuf, cnt, idx);
	pack_h(dbpkgbuf, price, idx);
	pack_h(dbpkgbuf, sell_tm, idx);
	TRACE_LOG("id=%u tm=%u type=%u itemid=%u unique=%u cnt=%u price=%u",
	  p->id, shop_start_tm, type, itemid, uniquekey, cnt, price);
	send_request_to_db(0, p->id, p->role_tm, dbproto_save_sell_goods_log, dbpkgbuf, idx);
}

