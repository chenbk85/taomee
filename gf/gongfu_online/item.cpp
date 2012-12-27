#include <libtaomee++/inet/pdumanip.hpp>
#include <kf/player_attr.hpp>
#include <kf/item_manager.hpp>
extern "C" {
#include <async_serv/mcast.h>
#include <arpa/inet.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/dataformatter/bin_str.h>

#include <libtaomee/project/utilities.h>
}
using namespace taomee;

#include "cli_proto.hpp"
#include "player.hpp"
#include "item.hpp"
#include "utils.hpp"
#include "task.hpp"
#include "battle.hpp"
#include "login.hpp"
#include "restriction.hpp"
#include "warehouse.hpp"
#include "ambassador.hpp"
#include "mail.hpp"
#include "skill.hpp"
#include "trade.hpp"
#include "global_data.hpp"
#include "buy_item_limit.hpp"
#include "mcast_proto.hpp"
#include "message.hpp"
#include "common_op.hpp"
//#include "achievement_logic.hpp"
#include "check_shop.hpp"
#include "limit.hpp"
#include "card.hpp"
#include "fight_team.hpp"
#include "stat_log.hpp"
#include "ambassador.hpp"
#include "rank_top.hpp"
#include "player_attribute.hpp"
#include "master_prentice.hpp"
#include "switch.hpp"
#include "other_active.hpp"
#include "temporary_team.hpp"

/*! the pointer to ItemManager for handling all items*/
//GfItemManager* items;
/*! for handling all compose product*/
//std::map<uint32_t, method_t*> methods_map_;
/*! for unique get item*/
//std::map<uint32_t, unique_item_pos_t> unique_item_map;
/*! for save all strengthen material*/
//std::map<uint32_t, strengthen_t> material_map;
/*! for strengthen fail */
/*static uint32_t strengthen_fail_rule [max_attire_level][2] = {
    {0, 100}, {0, 100}, {8, 92}, {8, 92}, {65, 35}, {92, 8}, {92, 8},
    {92, 8}, {99, 1}, {99, 1}, {99, 1}, {99, 1}};
static uint32_t strengthen_consume_coins[max_attire_level] = {
    3000, 3000, 3000, 5000, 5000, 6000, 6000, 7000, 8000, 8000, 8000, 8000};

suit_t suit_arr[max_suit_id + 1];
*/

uint32_t g_swap_action_cnt_1 =  0 ;
uint32_t g_swap_action_cnt_2 =  0 ;
uint32_t g_swap_action_cnt_3 =  0 ;
uint32_t g_swap_action_cnt_4 =  0 ;



int db_upgrade_item(player_t *p, items_upgrade_data* p_xml_data, uint32_t unique_id);
int send_compose_to_world(player_t *p, uint32_t attire_id);


//--------------------------------------------------------------------------------

/**
 * @brief add here for customer service
 */
void report_add_to_monitor(player_t* p, uint32_t opt_type, 
    uint32_t exv2, uint32_t exv3, uint32_t exv4)
{
    int idx_udp = 0;
    uint8_t buf[dbproto_max_len] = {0};
    pack_h(buf, static_cast<uint32_t>(6), idx_udp);
    pack_h(buf, p->id, idx_udp);
    pack_h(buf, static_cast<int32_t>(get_now_tv()->tv_sec), idx_udp);
    pack_h(buf, opt_type, idx_udp);
    pack_h(buf, static_cast<int32_t>(p->role_type), idx_udp);
    pack_h(buf, static_cast<int32_t>(exv2), idx_udp);
    pack_h(buf, static_cast<int32_t>(exv3), idx_udp);
    pack_h(buf, static_cast<int32_t>(exv4), idx_udp);
    TRACE_LOG("add item monitor:uid=%u, opt_type=%u, exv2=%u, exv3=%u, exv4=%u",
        p->id, opt_type, exv2, exv3, exv4);
    send_udp_request_to_db(p, p->id, dbproto_report_add_info, buf, idx_udp, 1);
}

/**
 * @brief report user get shop item 
 */
void report_user_get_shop_item(player_t* p, uint32_t item_id, uint32_t cnt)
{
    int idx_udp = 0;
    uint8_t buf[dbproto_max_len] = {0};
	pack_h(buf, p->role_tm, idx_udp);
    pack_h(buf, item_id, idx_udp);
    pack_h(buf, cnt, idx_udp);

    WARN_LOG("GET SHOP ITEM\t[%u %u %u]", p->id, item_id, cnt);
    send_udp_request_to_db(p, p->id, dbproto_notify_get_shopitem_log, buf, idx_udp, 2);
}

/**
 * @brief report user useshop item 
 */
void report_user_use_shop_item(player_t* p, uint32_t item_id, uint32_t cnt)
{
    int idx_udp = 0;
    uint8_t buf[dbproto_max_len] = {0};
	pack_h(buf, p->role_tm, idx_udp);
    pack_h(buf, item_id, idx_udp);
    pack_h(buf, cnt, idx_udp);

    WARN_LOG("USE SHOP ITEM\t[%u %u %u]", p->id, item_id, cnt);
    send_udp_request_to_db(p, p->id, dbproto_notify_use_shopitem_log, buf, idx_udp, 2);
}

void add_role_online_data(player_t* p, uint32_t type, uint32_t itemid, uint32_t cnt, const char* channel_str)
{
    //player attr
    if (type == give_type_player_attr) {
        switch (itemid) {
        case give_type_exp:
            p->exp += cnt;
            break;
        case give_type_xiaomee:
            p->coins += cnt;
            do_stat_log_coin_add_reduce(p, cnt, (uint32_t)0, channel_str);
            break;
        case give_type_skillpoint:
            p->skill_point += cnt;
            break;
        case give_type_fumo_point:
            p->fumo_points_total += cnt;
            do_stat_log_fumo_add_reduce(cnt, (uint32_t)0);
            break;
        case give_type_expliot:
            p->exploit += cnt;
            break;
        case give_type_double_tm:
            break;
        case give_type_honor:
            p->honor += cnt;
            break;
        case give_type_apothecary_exp:
            add_player_secondary_professoin_exp(p, apothecary_profession_type, cnt, &(cnt));
            break;
        case give_type_summon_exp:
            break;
        case give_type_sword_value:
            p->vip_sword_value += cnt;
            calc_player_vip_info(p, true);
            break;
        case give_type_vip_qualify:
            do_swap_vip_qualify(p, p->id, cnt);
            break;
        default:
            break;
        }
    } else if (type == give_type_normal_item) {
        p->my_packs->add_item(p, itemid, cnt, channel_str);
    } else if (type == give_type_clothes) {
        p->my_packs->add_clothes(p, itemid, cnt, 100, channel_str, 0, 0);
    }

}

int add_item_to_player(player_t* p, uint32_t type, uint32_t itemid, uint32_t cnt, uint32_t notify2player, const char* channel_str)
{
    if (cnt == 0) return 0; 
    int ifx = 0;
    uint32_t max_bag_grid_count = 500;
    pack_h(dbpkgbuf, type, ifx);
    pack_h(dbpkgbuf, itemid, ifx);
    pack_h(dbpkgbuf, cnt, ifx);
    pack_h(dbpkgbuf, max_bag_grid_count, ifx);
    send_request_to_db(0, p->id, p->role_tm, dbproto_add_item, dbpkgbuf, ifx);

    add_role_online_data(p, type, itemid, cnt, channel_str);

    if (notify2player) {
        int idx = sizeof(cli_proto_t);;
        pack(pkgbuf, type, idx);
        pack(pkgbuf, itemid, idx);
        pack(pkgbuf, cnt, idx);
		init_cli_proto_head(pkgbuf, p, cli_proto_player_gain_item, idx);
		send_to_player(p, pkgbuf, idx, 1);	
    }
    return 0;
}

/**
 * is_callback: false--不返回 true--返回
 */
int player_gain_item(player_t* p, uint32_t type, uint32_t itemid, uint32_t cnt, const char* channel_str, bool is_callback)
{
    if (cnt == 0) return 0; 
    int ifx = 0;
    uint32_t max_bag_grid_count = 500;
    /*
    uint32_t max_bag_grid_count = get_player_total_item_bag_grid_count(p); 
    if (!p->my_packs->check_enough_bag_item_grid(p, item_id, add_num)) {
        return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1); 
    }*/
    pack_h(dbpkgbuf, type, ifx);
    pack_h(dbpkgbuf, itemid, ifx);
    pack_h(dbpkgbuf, cnt, ifx);
    pack_h(dbpkgbuf, max_bag_grid_count, ifx);
    if (is_callback) {
        return send_request_to_db(p, p->id, p->role_tm, dbproto_add_item, dbpkgbuf, ifx);
    } else {
        send_request_to_db(0, p->id, p->role_tm, dbproto_add_item, dbpkgbuf, ifx);

        add_role_online_data(p, type, itemid, cnt, channel_str);
            /*
        if (type == 1) { //装备
            p->my_packs->add_clothes(p, itemid, cnt, 0, channel_str);
        } else if (type == 2){ //普通物品
            p->my_packs->add_item(p, itemid, cnt, channel_str);
        }*/
        int idx = sizeof(cli_proto_t);
        pack(pkgbuf, p->id, idx);
        pack(pkgbuf, itemid, idx);
        pack(pkgbuf, cnt, idx);
        init_cli_proto_head_full(pkgbuf, p->id, p->seqno, cli_proto_player_gain_item, idx, 0);
        //ERROR_LOG("ADD ITETM %u %u %u", p->id, pkg->item_id, pkg->add_info);
        return send_to_player(p, pkgbuf, idx, 0);
    }
}

int pack_app_buf_rsp(player_t* p, void* buf, buf_skill_t* p_buf)
{
	int idx = 0;
	
	if (!p_buf) {
		pack(buf, 0, idx);
	} else {
		pack(buf, p_buf->ex_id1, idx);
	}
	return idx;
}

void send_appearance_buf(player_t* p, buf_skill_t* p_buf)
{
	int idx = sizeof(cli_proto_t);
	idx += pack_app_buf_rsp(p, pkgbuf + idx, p_buf);
	init_cli_proto_head(pkgbuf, p, cli_proto_chg_app, idx);
	TRACE_LOG("uid=%u appid=%u buf_type=%u ",p->id, p_buf ? p_buf->ex_id1 : 0,p_buf ? p_buf->buf_type : 0);
	return send_to_map(p, pkgbuf, idx, 0);
}


/**
  * @brief a timer function to disable player's app buf
  * @return 0
  */
int set_app_buf_end(void* owner, void* data)
{
	player_t* player = reinterpret_cast<player_t*>(owner);
	if (player->battle_grp || player->trade_grp || player->home_grp) {
		int idx = sizeof(cli_proto_t);
		idx += pack_app_buf_rsp(player, pkgbuf + idx, 0);
		init_cli_proto_head(pkgbuf, player, cli_proto_chg_app, idx);
		send_to_player(player, pkgbuf, idx, 0);
	} else {
		send_appearance_buf(player, 0);
	}
    //------ get rich -------
    //ERROR_LOG("app buff end %u %u %u", player->id, player->app_mon, get_swap_action_times(player, 1277));
    if (player->app_mon == get_rich_monster_id && get_swap_action_times(player, 1277)) {
        player_gain_item(player, 2, get_rich_item_id, 2 * get_swap_action_times(player, 1277), channel_string_use_tongbao_item);
        clear_swap_action_times(player, 1277);
        player->reward_role_vec->clear();
    }

	player->app_mon = 0;
	player->app_t = 0;
	TRACE_LOG("%p %u %p", player->battle_grp, player->app_mon, player->app_t);

    
	return 0;
}

/**
  * @brief return bitpos of unique items 
  * @return 0 none
  */
uint32_t get_unique_item_bitpos(uint32_t item_id)
{
	std::map<uint32_t, unique_item_pos_t>::iterator iter = unique_item_map.find(item_id);
	if (iter == unique_item_map.end()) {
		return 0;
	}
	return iter->second.bit_pos;
	
}

int repair_all_attire(player_t* p)
{
	int db_idx = 0;
	int cnt_idx = 0;
	uint32_t clothes_cnt = p->clothes_num;
	uint32_t repair_cnt = clothes_cnt;
	db_idx += 4;
	
	for (uint32_t i = 0; i < clothes_cnt; i++) {
		player_clothes_info_t* p_cloth = &(p->clothes[i]);
		const GfItem* itm = items->get_item(p_cloth->clothes_id);
		if (!itm) {
			WARN_LOG("player:[%u] not found attire:[%u]", p->id, p_cloth->clothes_id);
			return send_header_to_player(p, p->waitcmd, cli_err_no_clothes, 1);
		}
		if (p_cloth->duration >= itm->duration * 50) {
			repair_cnt --;
			continue;
		}
		pack_h(dbpkgbuf, p_cloth->clothes_id, db_idx);
		pack_h(dbpkgbuf, p_cloth->unique_id, db_idx);
		pack_h(dbpkgbuf, static_cast<uint32_t>(0), db_idx); //per_price 
		pack_h(dbpkgbuf, static_cast<uint32_t>(itm->duration), db_idx);			
	
		TRACE_LOG("uid=[%u],id=[%d],unique=[%d], duration=[%u], repair_price=[%u]",
				p->id, p_cloth->clothes_id, p_cloth->unique_id, itm->duration, static_cast<uint32_t>(itm->repair_price()));
	}
	pack_h(dbpkgbuf, repair_cnt, cnt_idx);
	if (repair_cnt == 0 && p->battle_grp) {
		uint32_t* p_itemid = (uint32_t*)(p->session);
		return btlsvr_player_use_item(p, *p_itemid);
	}
	KDEBUG_LOG(p->id, "ROT REPAIR ALL CLOTHES\t[uid=%u repair_cnt=%u ]", p->id, clothes_cnt);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_repair_all_clothes, dbpkgbuf, db_idx);	
}

int send_use_item_rsp(player_t* p, uint32_t item_id, int completed)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, item_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	TRACE_LOG("uid=%u itmid=%u",p->id, item_id);
	return send_to_player(p, pkgbuf, idx, completed);
}

//--------------------------------------------------------------------------
// Cmds
/**
  * @brief buy (an) item(s)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int buy_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t itmid, cnt, npc_id;

	unpack(body, npc_id, idx);
	unpack(body, itmid, idx);
	unpack(body, cnt, idx);

	if (p->battle_grp) {
		ERROR_LOG("can not buy item in btl %u", p->id);
		return -1;
	}


	bool ret = check_npc_item(npc_id, itmid);
	if( !ret )
	{
		WARN_LOG("buy item npc error![%u %u %u %u]", p->id, npc_id, itmid, cnt);
		return send_header_to_player(p, p->waitcmd, cli_err_failed_to_buy, 1);
	}

	const GfItem* itm = items->get_item(itmid);
	if (!itm || (cnt > itm->max())) {
		WARN_LOG("buy item error![%u %u %u]", p->id, itm ? itm->id() : 0, cnt);
		return send_header_to_player(p, p->waitcmd, cli_err_failed_to_buy, 1);
	}
    if (!is_item_buyable(p, itm)) {
        WARN_LOG("buy item error![%u %u]", p->id, itm ? itm->id() : 0);
        do_stat_log_use_plugin(stat_log_use_plugin, p->id, 1);
        return send_header_to_player(p, p->waitcmd, cli_err_failed_to_buy, 1);
    }
	if ( !is_enough_to_buy(p, itm->price(), cnt) ) {
		TRACE_LOG("%u %u", itm->price(), p->coins);
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
	}

	if( !check_player_buy_item_limit_data( get_buy_item_limit_mgr(), p, itmid, cnt) ){
		return send_header_to_player(p, p->waitcmd, cli_err_limit_to_buy, 1);
	}	

	return db_buy_item(p, itm, cnt);
}

/**
  * @brief buy an clothes
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int buy_clothes_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	int db_idx_tmp = 1;
	int db_idx = 0;
	uint32_t count;
	uint8_t	 type = 0;
	uint32_t npc_id = 0;

	unpack(body, npc_id, idx);
	unpack(body, type, idx);
	unpack(body, count, idx);

	pack_h(dbpkgbuf, type, db_idx);
	db_idx += 4;
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), db_idx);
	pack_h(dbpkgbuf, count, db_idx);

	CHECK_VAL_EQ(bodylen, 9 + 4 * count);

	if (p->battle_grp) {
		ERROR_LOG("can not buy cloth in btl %u", p->id);
		return -1;
	}

	uint32_t all_cost = 0;
	uint32_t clothes_level = 0;
	uint32_t now = get_now_tv()->tv_sec;

	for ( uint32_t i = 0; i < count; i++ ) {
		uint32_t clothes_id;
		unpack(body, clothes_id, idx);
		TRACE_LOG("clothes_id=%u",clothes_id);

		if(type == buy_with_coins)
		{
			bool ret = check_npc_item(npc_id, clothes_id);
			if(!ret)
			{		
				WARN_LOG("clothes_id error![%u %u %u]",p->id, npc_id, clothes_id);
				return send_header_to_player(p, p->waitcmd, cli_err_failed_to_buy, 1);
			}
		}

		const GfItem* clothes = items->get_item(clothes_id);
		if ( !clothes || !items->is_clothes(clothes->category()) ){
			WARN_LOG("clothes_id error![%u %u]",p->id, clothes_id);
			return send_header_to_player(p, p->waitcmd, cli_err_no_clothes, 1);
		}
		if ( !is_item_buyable(p, clothes) ) {
			WARN_LOG("clothes_id buy error![%u %u]",p->id,clothes_id);
            do_stat_log_use_plugin(stat_log_use_plugin, p->id, 1);
			return send_header_to_player(p, p->waitcmd, cli_err_failed_to_buy, 1);
		}

		if (clothes->swap_action_id()) {
			if (!is_player_have_swap_action_times(p, clothes->swap_action_id())) {
				return send_header_to_player(p, p->waitcmd, cli_err_month_limit_time_act, 1);
			}
		}
		
		uint32_t duration = clothes->duration * clothes_duration_ratio;
		//pack_h(dbpkgbuf, clothes->price(), idx);
		pack_h(dbpkgbuf, clothes->id(), db_idx);
		pack_h(dbpkgbuf, now, db_idx);
		pack_h(dbpkgbuf, clothes_level, db_idx);
		pack_h(dbpkgbuf, duration, db_idx);
		
		if (type == buy_with_exploit) {
			if (clothes->exploit_value() == 0) {
				TRACE_LOG("clothes buy not enough exploit error!");
				return send_header_to_player(p, p->waitcmd, cli_err_buy_with_exploit, 1);				
			}
			if (clothes->honor_level() && !is_user_info_meet_honor_request(p, clothes->honor_level() - 1)) {
				TRACE_LOG("clothes buy not enough exploit error!");
				return send_header_to_player(p, p->waitcmd, cli_err_buy_with_exploit, 1);	
			}
			all_cost += clothes->exploit_value();
		} else if (type == buy_with_fumo_points) {
			uint32_t fumo_price = get_fumo_price(clothes);
			if (!fumo_price) {
				TRACE_LOG("clothes buy not enough fumo error!");
				return send_header_to_player(p, p->waitcmd, cli_err_buy_with_fumo, 1);				
			}
			all_cost += fumo_price;
		} else if (type == buy_with_coins){
			all_cost += clothes->price();
		} else {
			WARN_LOG("wai gua zhu!!! uid=%u", p->id);
			return -1;
		}
		TRACE_LOG("buy type:%u id:%u cost:%u", type, clothes->id(), all_cost);
	}


	if (type == buy_with_exploit && !is_exploit_enough_to_buy(p, all_cost, 1) ) {
		TRACE_LOG("clothes buy not enough money error! %u", type);
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_exploit, 1);
		
	} 
	if (type == buy_with_fumo_points && !is_fumo_enough_to_buy(p, all_cost, 1) ) {
		TRACE_LOG("clothes buy not enough money error! %u %u %u", p->coins, all_cost, type);
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_fumo, 1);
		
	} 
	if (type == buy_with_coins && !is_enough_to_buy(p, all_cost, 1) ) {
		TRACE_LOG("clothes buy not enough money error! %u %u %u", p->coins, all_cost, type);
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
	}

	pack_h(dbpkgbuf, all_cost, db_idx_tmp);

	KDEBUG_LOG(p->id, "BUY CLOTHES\t[uid=%u]", p->id);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_buy_clothes, dbpkgbuf, db_idx);
	//return db_buy_clothes(p, clothes);
}

/**
  * @brief sell (an) item(s)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int sell_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t itmid, cnt;

	unpack(body, itmid, idx);
	unpack(body, cnt, idx);

	if (p->trade_grp) {
		ERROR_LOG("%u in trade cannot sell_item", p->id);
		return -1;
	}

	const GfItem* itm = items->get_item(itmid);
	if (!itm || !is_item_sellable(p, itm)) {
		return send_header_to_player(p, p->waitcmd, cli_err_failed_to_sell, 1);
	}
	
	KDEBUG_LOG(p->id, "SELL ITEM\t[uid=%u itm=%u cnt=%u]", p->id, itmid, cnt);
	return db_sell_item(p, itm, cnt);
}

/**
  * @brief sell an clothes
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int sell_clothes_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t itmid, unique_id;

	unpack(body, itmid, idx);
	unpack(body, unique_id, idx);

	if (p->trade_grp) {
		ERROR_LOG("%u in trade cannot sell_clothes", p->id);
		return -1;
	}

	if (!is_player_checked_pay_passwd(p)) {
		ERROR_LOG("%u havnot check pay passwd", p->id);
		return -1;
	}

	const GfItem* itm = items->get_item(itmid);
	if (!itm || !is_item_sellable(p, itm) || is_clothes_weared(p, unique_id)) {
		return send_header_to_player(p, p->waitcmd, cli_err_failed_to_sell, 1);
	}
	
	KDEBUG_LOG(p->id, "SELL CLOTHES\t[uid=%u itm=%u unique_id=%u]", p->id, itmid, unique_id);
	return db_sell_clothes(p, itm, unique_id);
}

/**
  * @brief sell (some) item(s)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int batch_sell_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int u_idx = 0;
    uint32_t type = 0, item_cnt = 0; // 1-->gold coin; 2--> gongfu bean 
    uint32_t itemid = 0, cnt = 0;
    uint32_t coin = 0;

    if (p->trade_grp || p->home_grp || p->battle_grp) {
        ERROR_LOG("%u in trade/home/battle cannot sell_item", p->id);
        return -1;
    }

    unpack(body, type, u_idx);
    unpack(body, item_cnt, u_idx);

    TRACE_LOG(" IN header %u %u", type, item_cnt);

    CHECK_VAL_EQ(bodylen, 8 + item_cnt * 8);

    int idx = 0;
    pack_h(dbpkgbuf, type, idx);
    pack_h(dbpkgbuf, coin, idx); //money
    pack_h(dbpkgbuf, item_cnt, idx);
    for (uint32_t i = 0; i < item_cnt; i++) {
        unpack(body, itemid, u_idx);
        unpack(body, cnt, u_idx);

        if ( !p->my_packs->check_bag_item_count(itemid, cnt) ) {
            ERROR_LOG("uid [%u] item not exist !!!! [%u %u]", p->id, itemid, cnt);
            return -1;
        }

        KDEBUG_LOG(p ? p->id : 0, "BATCH SELL ITEMS\t[uid=%u itm=%u %u]", p ? p->id : 0, itemid, cnt);
        if (type == 1) {
            coin += (get_item_gold_coin_price(itemid) * cnt);
        } else if (type == 2) {
            const GfItem* itm = items->get_item(itemid);
            if (!itm || !is_item_sellable(p, itm)) {
                return send_header_to_player(p, p->waitcmd, cli_err_failed_to_sell, 1);
            }
            coin += (itm->sellprice() * cnt);
        }

        pack_h(dbpkgbuf, itemid, idx);
        pack_h(dbpkgbuf, cnt, idx);
    }

    *(reinterpret_cast<uint32_t*>(dbpkgbuf + sizeof(uint32_t))) = coin; 
	
	KDEBUG_LOG(p->id, "BATCH SELL ITEM\t[uid=%u coin=%u type=%u cnt=%u]", p->id, coin, type, item_cnt);
    return send_request_to_db(p, p->id, p->role_tm, dbproto_batch_sell_item, dbpkgbuf, idx);
}


/**
  * @brief repair all clothes include used, unused
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int repair_all_clothes_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t clothes_cnt, clothes_id, unique_id;
	int idx = 0, db_idx = 0;
	
	unpack(body, clothes_cnt, idx);
	CHECK_VAL_EQ(bodylen, 4 + clothes_cnt * 8);
	CHECK_VAL_LE(clothes_cnt, max_repair_cnt);
	
	pack_h(dbpkgbuf, clothes_cnt, db_idx);
	for (uint32_t i = 0; i < clothes_cnt; i++) {
		unpack(body, clothes_id, idx);
		unpack(body, unique_id, idx);
		const GfItem* itm = items->get_item(clothes_id);
		if (!itm) {
			WARN_LOG("player:[%u] not found attire:[%u]", p->id, clothes_id);
			return send_header_to_player(p, p->waitcmd, cli_err_no_clothes, 1);
		}
		pack_h(dbpkgbuf, clothes_id, db_idx);
		pack_h(dbpkgbuf, unique_id, db_idx);
		pack_h(dbpkgbuf, static_cast<uint32_t>(itm->repair_price()), db_idx); //per_price 
		pack_h(dbpkgbuf, static_cast<uint32_t>(itm->duration), db_idx);			
	
		TRACE_LOG("uid=[%u],id=[%d],unique=[%d], duration=[%u], repair_price=[%u]",
				p->id, clothes_id, unique_id, itm->duration, static_cast<uint32_t>(itm->repair_price()));
	}
	if (clothes_cnt == 0) {
		return send_header_to_player(p, p->waitcmd, cli_err_not_need_repair, 1);
	}
			
	KDEBUG_LOG(p->id, "REPAIR ALL CLOTHES\t[uid=%u repair_cnt=%u ]", p->id, clothes_cnt);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_repair_all_clothes, dbpkgbuf, db_idx);
}


/**
  * @brief player wear clothes
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int wear_clothes_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	//err if in battle
	if (p->battle_grp) {
		return send_header_to_player(p, p->waitcmd, cli_err_failed_wear_inbtl, 1);
	}

	TRACE_LOG("player %u .......................wear clothes", p->id);

	uint32_t clothes_cnt;
	int idx1 = 0, idx2 = 0;

	unpack(body, clothes_cnt, idx1);

	CHECK_VAL_EQ(bodylen, 4 + clothes_cnt * 8);
	CHECK_VAL_LE(clothes_cnt, max_clothes_on);

	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx2);
	pack_h(dbpkgbuf, clothes_cnt, idx2);
	TRACE_LOG("id=[%u],count=[%u]",p->id,clothes_cnt);

	std::vector<const GfItem*> going_wear_clothes;
	for ( uint32_t i = 0; i < clothes_cnt; i++) {
		//uint32_t time;
		uint32_t unique_id;
		uint32_t clothes_id;
		unpack(body, clothes_id, idx1);
		unpack(body, unique_id, idx1);
		//unpack(body, time, idx1);
		/*test if this clothes can wear*/
		const GfItem* clothes = items->get_item(clothes_id);
		if ( !clothes || !is_clothes_can_wear(p, clothes)) {
			WARN_LOG("cant wear this clothes[uid=%u, role_type=%u, clothesid=%u]",p->id, p->role_type, clothes_id);
			return send_header_to_player(p, p->waitcmd, cli_err_clothes_cannot_wear, 1);
		}
		if (!(p->my_packs->is_clother_item_exist(unique_id, clothes_id)) && !is_clothes_weared(p, unique_id, clothes_id)) {
			ERROR_LOG("%u WEAR CLOTEHS NOT EXIST or WITH SAME EQUIP PART %u ", p->id, clothes_id);
			p->waitcmd = 0;
			return 0;
		}

		going_wear_clothes.push_back(clothes);
		
		if (clothes->use_lv() > p->lv) {
			ERROR_LOG("lv less\t[uid=%u clothes_id=%u plv=%u uselv=%u]",p->id, clothes_id, p->lv, clothes->use_lv());
			return send_header_to_player(p, p->waitcmd, cli_err_less_lv, 1);
		}
        if (clothes->is_item_vip_only() && !is_vip_player(p)) {
            TRACE_LOG("VIP ONLY CLOTHES\t [uid=%u clothes_id=%u vip=%u flag=%u]", p->id, clothes_id, p->vip, clothes->is_item_vip_only());
            return send_header_to_player(p, p->waitcmd, cli_err_vip_only, 1);
        }

		//pack_h(dbpkgbuf, time, idx2);
		pack_h(dbpkgbuf, unique_id, idx2);
		pack_h(dbpkgbuf, static_cast<uint32_t>(clothes->equip_part()), idx2);
		TRACE_LOG("wear_clothes pack:id=[%u],unique=[%u]",clothes_id, unique_id);
	}

	for (uint32_t i  = 1; i < clothes_cnt; i++) {
		const GfItem * clothes = going_wear_clothes.at(i);
		for (uint32_t j = 0; j < i; j++) {
			const GfItem * pre_clothes = going_wear_clothes.at(j);
			if (clothes->equip_part() == pre_clothes->equip_part() && clothes->equip_part() <= 10) {
				ERROR_LOG("%u WEAR CLOTEHS %u AND %u WITH SAME EQUIP PART %u", p->id, i,  j, clothes->equip_part());
				p->waitcmd = 0;
				return 0;
			}
		}
	}

	return send_request_to_db(p, p->id, p->role_tm, dbproto_wear_clothes, dbpkgbuf, idx2);
}

/**
  * @brief get player's clothes unused
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_clothes_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return db_get_clothes_list(p);
}

/**
  * @brief get player's item 
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_item_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = sizeof(cli_proto_t);
	uint32_t max_pack_item = get_player_total_item_bag_grid_count(p);

	pack(pkgbuf, max_pack_item, idx);
	TRACE_LOG("%u", max_pack_item);
	idx += p->my_packs->pack_all_item_info(pkgbuf + idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return  send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief get player's strengthen material 
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_strengthen_material_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    return send_request_to_db(p, p->id, p->role_tm, dbproto_get_strengthen_material_list, 0, 0);
}
/**
  * @brief use a skill book
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int use_skill_book_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t book_id;
	unpack(body, book_id, idx);

	const GfItem* skill_book = items->get_item(book_id);
	if ( !skill_book || !items->is_skill_book(skill_book->category()) ) {
		TRACE_LOG("no such skill book\t[uid=%u bookid=%u]",p->id, book_id);
		return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
	} 
	/*else if ( skill_book->use_lv() > p->lv ) {
		TRACE_LOG("have less lv for using this book\t[uid=%u lv=%u bookid=%u booklv=%u]",
				p->id, p->lv, book_id, skill_book->use_lv());
		return send_header_to_player(p, p->waitcmd, cli_err_less_lv, 1);
	}*/
	//TODO: judge id have enough skill point
	

	idx = 0;
	//pack_h(dbpkgbuf, skill_book->skill_id(), idx);
	//pack_h(dbpkgbuf, (uint32_t)skill_book->use_lv(), idx);
	pack_h(dbpkgbuf, book_id, idx);//skill book
	pack_h(dbpkgbuf, (uint32_t)1, idx);//number of skill points got
	
	KDEBUG_LOG(p->id, "USE SKILL BOOK\t[uid=%u bookid=%u skillid=%u]",p->id, book_id, skill_book->skill_id());
	
	return send_request_to_db(p, p->id, p->role_tm, dbproto_use_skill_book, dbpkgbuf, idx);
}

uint32_t mutex_type_err_code(uint32_t mutex_type)
{
    switch (mutex_type) {
        case 1:
        case 2:
            return cli_err_cannot_already_have_dexp;
            break;
        case 3:
            return cli_err_cannot_already_have_bless;
            break;
        default:
            break;
    }
    return 0;
}

/**
  * @brief use a item
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int use_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t item_id;
	unpack(body, item_id, idx);
	const GfItem* itm = items->get_item(item_id);
	if ( !itm ) { //|| !items->can_use(itm->category()) ) {
		TRACE_LOG("no such item tonic\t[uid=%u item_id=%u]",p->id, item_id);
		return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
	}
	TRACE_LOG("%u %u %u", item_id, itm->mutex_type(), itm->buf_type());

	//judge if cannot be used in battle
	if (p->battle_grp) {
		switch (item_id) {
			case reset_skills_item_id:
			case double_exp_buff_item_id:
			case double_exp_buff_item_id_ex:
			case double_mon_only_exp_buff_item_id:
			case double_player_only_exp_buff_item_id:
			case double_mon_only_exp_buff_60min_item_id:
			case double_mon_only_exp_buff_60min_item_id_ex:
				TRACE_LOG("no pvp or pve available\t[uid=%u item_id=%u]",p->id, item_id);
				return send_header_to_player(p, p->waitcmd, cli_err_cannot_use_in_btl, 1);
				break;
			default:
				break;
		}
	}
	//judge use item times limit
	restriction_t* p_restr = 0;
	switch (item_id) {
		case double_mon_only_exp_buff_item_id:
			p_restr  = &(g_all_restr[mon_only_get_exp_buff_daily_id - 1]);
			if (p->daily_restriction_count_list[mon_only_get_exp_buff_daily_id - 1] >= p_restr->toplimit) {
				TRACE_LOG("times limit\t[uid=%u item_id=%u]",p->id, item_id);
				return send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act, 1);
			}
			break;
		case double_player_only_exp_buff_item_id:
			p_restr  = &(g_all_restr[player_only_get_exp_buff_daily_id - 1]);
			if (p->daily_restriction_count_list[player_only_get_exp_buff_daily_id - 1] >= p_restr->toplimit) {
				TRACE_LOG("times limit\t[uid=%u item_id=%u]",p->id, item_id);
				return send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act, 1);
			}
			break;
		case trade_mcast_item_id_1:
		case trade_mcast_item_id_2:
			if (!p->trade_grp) {
				return send_header_to_player(p, p->waitcmd, cli_err_trade_not_in_market, 1);
			}
			break;
			
		default:
			break;
	}


	if (itm->mutex_type()) {
		BufSkillMap::iterator it = p->buf_skill_map->begin();
		for (; it != p->buf_skill_map->end(); ++it) {
			if (it->second.mutex_type == itm->mutex_type()) {
				TRACE_LOG("already have skill buff \t[uid=%u item_id=%u %u]",p->id, item_id, it->second.buf_type);
				return send_header_to_player(p, p->waitcmd, mutex_type_err_code(itm->mutex_type()), 1);
			}
		}
	}

	//judge if vip only
    if (itm->is_item_vip_only() && !is_vip_player(p)) {
        TRACE_LOG("VIP ONLY ITEM\t [uid=%u item_id=%u vip=%u]", p->id, item_id, p->vip);
        return send_header_to_player(p, p->waitcmd, cli_err_vip_only, 1);
    }

    if (item_id == get_rich_item_id) {
        if (get_swap_action_times(p, 1275) >= 30) {
            return send_header_to_player(p, p->waitcmd, cli_err_pvp_not_available, 1);
        }
    }

	if (item_id == repair_clothes_item_id && p->battle_grp && is_stage_tower(p->battle_grp->stage_id)) {
	//if in tower stage ,can use repair matchine
	} else {
		if (p->battle_grp && 
				((p->battle_grp->battle_mode == btl_mode_lv_matching && !itm->pvp_available()) ||
					(is_stage_tower(p->battle_grp->stage_id) && !is_relive_item(item_id)))) {
            if (p->battle_grp->pvp_btl_type != pvp_monster_game && 
                p->battle_grp->pvp_btl_type != pvp_monster_practice && 
                p->battle_grp->pvp_btl_type != pvp_contest_final &&
                p->battle_grp->pvp_btl_type != pvp_ghost_game) {
                TRACE_LOG("no pvp_available\t[uid=%u item_id=%u]",p->id, item_id);
                return send_header_to_player(p, p->waitcmd, cli_err_pvp_not_available, 1);
            }
		}
	}
	if (itm->use_lv() > p->lv) {
			TRACE_LOG("lv less\t[uid=%u item_id=%u plv=%u uselv=%u]",p->id, item_id, p->lv, itm->use_lv());
			return send_header_to_player(p, p->waitcmd, cli_err_less_lv, 1);
	}
	bool is_shop_item = itm->is_item_shop_sale();
	if (is_storage_relive_item(item_id)) {
		is_shop_item = true;
		stat_log_relive_item(item_id, 1, 0);
	}
	if (is_relive_item(item_id)) {
		do_stat_log_universal_interface_1(stat_log_relive_item_use_lv, p->lv, 1);
	}
	return db_use_item(p, item_id, 1, is_shop_item);
}


/**
  * @brief set item bind key
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int set_itembind_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	TRACE_LOG("p->itembind,%s", (char*)body);
	return db_set_itembind(p, max_itembind_len, body);
}

/**
  * @brief discard (an) item(s)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int discard_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t itmid, cnt;

	unpack(body, itmid, idx);
	unpack(body, cnt, idx);

	const GfItem* itm = items->get_item(itmid);
	if (!itm || items->is_skill_book(itm->category())) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
	}
	
	KDEBUG_LOG(p->id, "DISCARD ITEM\t[uid=%u itm=%u cnt=%u]", p->id, itmid, cnt);

	//send to db
	idx = 0;
	pack_h(dbpkgbuf, itm->id(), idx);
	pack_h(dbpkgbuf, cnt, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_discard_item, dbpkgbuf, idx);
}

/**
  * @brief discard a clothes
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int discard_clothes_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t itmid, unique_id;

	unpack(body, itmid, idx);
	unpack(body, unique_id, idx);
	TRACE_LOG("[%u %u %u]",p->id, itmid, unique_id);

	if (!is_player_checked_pay_passwd(p)) {
		ERROR_LOG("%u havnot check pay passwd", p->id);
		return -1;
	}
	const GfItem* itm = items->get_item(itmid);
	pack_clothes_info_t * clothes = p->my_packs->get_clothes_info_by_unique_id(unique_id);

	if (!clothes) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
	}

	if (!itm && is_clothes_weared(p, unique_id) && !clothes) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
	}


	KDEBUG_LOG(p->id, "DISCARD CLOTHES\t[uid=%u clothes=%u uniqueid=%u lv=%u]", p->id, itmid, unique_id, clothes->lv);

	//send to db
	idx = 0;
	pack_h(dbpkgbuf, itmid, idx);
	pack_h(dbpkgbuf, unique_id, idx);
	return send_request_to_db(p, p->id, p->role_tm, dbproto_discard_clothes, dbpkgbuf, idx);
}

/**
  * @brief get player's clothes duration
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_clothes_duration_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (p->battle_grp != 0) {
		int idx = sizeof(btl_proto_t);
		init_btl_proto_head(p, p->id, btlpkgbuf, idx, btl_player_clothes_duration);
		return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->clothes_num, idx);
	TRACE_LOG("clothes cnt[%u %u]",p->id, p->clothes_num);
	for (uint32_t i = 0; i < p->clothes_num; i++) {
		pack(pkgbuf, p->clothes[i].unique_id, idx);
		pack(pkgbuf, static_cast<uint32_t>(p->clothes[i].duration), idx);
		TRACE_LOG("clothes duration[%u %u %u]",p->id, p->clothes[i].unique_id, p->clothes[i].duration);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);

}

/**
  * @brief decompose attire
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int decompose_attire_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t attire_id, unique_id;

	if (!is_player_checked_pay_passwd(p)) {
		ERROR_LOG("%u havnot check pay passwd", p->id);
		return -1;
	}


	unpack(body, attire_id, idx);
	unpack(body, unique_id, idx);

	pack_clothes_info_t * clothes = p->my_packs->get_clothes_info_by_unique_id(unique_id);
	if (!clothes) {
		ERROR_LOG("%u not have this clothes %u %u", p->id, attire_id, unique_id);
		return -1;
	}

	KDEBUG_LOG(p->id, "TRY DECOMPOSE ATTR\t[%u attire=%u unique=%u lv=%u]", p->id, attire_id, unique_id, clothes->lv);
	return do_decompose_attire(p, attire_id, unique_id);
}

/**
  * @brief compose attire
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int compose_attire_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t method_id;
	uint32_t addition_item_id;

	unpack(body, method_id, idx);
	unpack(body, addition_item_id, idx);
	
	return do_compose_attire(p, method_id, addition_item_id);
}

/**
  * @brief strengthen attire
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int strengthen_attire_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    strengthen_attire_t strengthen_obj = {0};
    unpack(body, strengthen_obj.attireid, idx);
    unpack(body, strengthen_obj.uniquekey, idx);
    //unpack(body, strengthen_obj.attirelv, idx);
    unpack(body, strengthen_obj.material_id, idx);
    unpack(body, strengthen_obj.material_cnt, idx);
    unpack(body, strengthen_obj.stoneid_1, idx);
    unpack(body, strengthen_obj.stoneid_1_cnt, idx);
    unpack(body, strengthen_obj.stoneid_2, idx);
    TRACE_LOG("attire strengthen[%u %u %u %u %u %u %u]",strengthen_obj.attireid, 
        strengthen_obj.uniquekey,strengthen_obj.material_id,strengthen_obj.material_cnt,
        strengthen_obj.stoneid_1, strengthen_obj.stoneid_1_cnt, strengthen_obj.stoneid_2);

	uint32_t tmp_id = strengthen_obj.material_id;
	uint32_t tmp_cnt = strengthen_obj.material_cnt;
    pack_clothes_info_t *clothse = p->my_packs->get_clothes_info_by_unique_id(strengthen_obj.uniquekey);
    if (clothse == NULL) {
        goto _ERR_END_;
    }
    strengthen_obj.attireid = clothse->clothes_id;

	if (tmp_id && p->my_packs->get_item_cnt(tmp_id) < tmp_cnt) {
		goto _ERR_END_;
	}

	tmp_id = strengthen_obj.stoneid_1;
	tmp_cnt = strengthen_obj.stoneid_1_cnt;
	if (tmp_id && p->my_packs->get_item_cnt(tmp_id) < tmp_cnt) {
		goto _ERR_END_;
	}

	tmp_id = strengthen_obj.stoneid_2;
	tmp_cnt = 1;
	if (tmp_id && p->my_packs->get_item_cnt(tmp_id) < tmp_cnt) {
		goto _ERR_END_;
	}
	
    return do_strengthen_attire(p, &strengthen_obj);

_ERR_END_:
	ERROR_LOG("STRENTH ATTIRE ERR uid=%u item=%u cnt=%u", p->id, tmp_id, tmp_cnt);
	p->waitcmd = 0;
	return 0;

}
//--------------------------------------------------------------------------------
// Requests to Dbproxy
//--------------------------------------------------------------------------------
/**
  * @brief buy items
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_buy_item(player_t* p, const GfItem* itm, uint32_t cnt, uint32_t ret_flg)
{
	int idx = 0;
    uint32_t costs = itm->price() * cnt;
	pack_h(dbpkgbuf, itm->id(), idx);
	pack_h(dbpkgbuf, cnt, idx);
	pack_h(dbpkgbuf, itm->max(), idx);	
	pack_h(dbpkgbuf, costs, idx);
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);

	if (ret_flg)
		return send_request_to_db(p, p->id, p->role_tm, dbproto_buy_item, dbpkgbuf, idx);
	else 
		return send_request_to_db(0, p->id, p->role_tm, dbproto_buy_item, dbpkgbuf, idx);
}
/**
  * @brief sell items
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_sell_item(player_t* p, const GfItem* itm, uint32_t cnt)
{
	int idx = 0;
	pack_h(dbpkgbuf, itm->id(), idx);
	pack_h(dbpkgbuf, cnt, idx);
	pack_h(dbpkgbuf, itm->sellprice() * cnt, idx);

	return send_request_to_db(p, p->id, p->role_tm, dbproto_sell_item, dbpkgbuf, idx);
}

/**
  * @brief use item
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_use_item_ex(player_t* p, uint32_t userid, uint32_t role_tm, uint32_t item_id, uint32_t count, bool is_shop_sale, bool callback)
{
	int idx = 0;
	pack_h(dbpkgbuf, item_id, idx);
	pack_h(dbpkgbuf, count, idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(is_shop_sale), idx);
	KDEBUG_LOG(p ? p->id : 0, "USE ITEM\t[uid=%u itm=%u %u shop=%d]", p ? p->id : 0, item_id, count, is_shop_sale);
	return send_request_to_db( callback? p: NULL, userid, role_tm, dbproto_use_item, dbpkgbuf, idx);
}

/**
  * @brief use item
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_use_item(player_t* p, uint32_t item_id, uint32_t count, bool is_shop_sale, bool callback)
{
	return db_use_item_ex(p, p->id, p->role_tm, item_id, count, is_shop_sale, callback);	
}

/**
  * @brief del item
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */

int player_del_items(player_t* p, db_del_item_elem_t* arr, uint32_t count, bool notify_2_player, const char* channel_str)
{
	int idx = 0;
	int idx_client = sizeof(cli_proto_t);
	
	pack_h(dbpkgbuf, count, idx);
	pack(pkgbuf, count, idx_client);
	for (uint32_t i = 0; i < count; i++) {
		pack_h(dbpkgbuf, arr[i].item_id, idx);
		pack_h(dbpkgbuf, arr[i].count, idx);

		p->my_packs->del_item(p, arr[i].item_id, arr[i].count, channel_str);
		
		KDEBUG_LOG(p ? p->id : 0, "DEL ITEMS\t[uid=%u itm=%u %u]", 
			p ? p->id : 0, arr[i].item_id, arr[i].count);

		pack(pkgbuf, arr[i].item_id, idx_client);
		pack(pkgbuf, arr[i].count, idx_client);
	}
	
	send_request_to_db( 0, p->id, p->role_tm, dbproto_player_del_items, dbpkgbuf, idx);

	if (notify_2_player) {
		init_cli_proto_head(pkgbuf, p, cli_proto_use_items, idx_client);
		send_to_player(p, pkgbuf, idx_client, 1);	
	}
	return 0;
}

/**
  * @brief buy clothes
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_buy_clothes(player_t* p, const GfItem* itm)
{
	uint32_t clothes_level = 0;
	uint32_t now = get_now_tv()->tv_sec;
	
	int idx = 0;
	uint32_t duration = itm->duration;
	pack_h(dbpkgbuf, itm->price(), idx);
	pack_h(dbpkgbuf, itm->id(), idx);
	pack_h(dbpkgbuf, now, idx);
	pack_h(dbpkgbuf, clothes_level, idx);
	pack_h(dbpkgbuf, duration, idx);
	
	return send_request_to_db(p, p->id, p->role_tm, dbproto_buy_clothes, dbpkgbuf, idx);
}

/**
  * @brief sell clothes
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_sell_clothes(player_t* p, const GfItem* itm, uint32_t unique_id)
{
	int idx = 0;

	pack_h(dbpkgbuf, itm->id(), idx);
	pack_h(dbpkgbuf, unique_id, idx);
	pack_h(dbpkgbuf, itm->sellprice(), idx);
	
	return send_request_to_db(p, p->id, p->role_tm, dbproto_sell_clothes, dbpkgbuf, idx);
}

/**
 * @brief get clothes list
 * @return 0 on success, -1 on error
 */
int db_get_clothes_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_clothes_list, 0, 0);
}



/**
 * @brief set item bind key
 * @return 0 on success, -1 on error
 */
int db_set_itembind(player_t* p, uint32_t len, uint8_t* bindbuf)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_set_itembind, bindbuf, len);
}

/**
 * @brief decompose attire
 * @return 0 on success, -1 on error
 */
int do_decompose_attire(player_t* p, uint32_t attire_id, uint32_t unique_id)
{
	const GfItem* itm = items->get_item(attire_id);
	if (!itm || !items->is_clothes(itm->category()) || is_clothes_weared(p, unique_id) 
			|| itm->quality_lv < 2 || itm->quality_lv > 4) {
		WARN_LOG("not exsit this attire:[%u %u %u]", p->id, attire_id, itm ? itm->quality_lv : 0);
		return send_header_to_player(p, p->waitcmd, cli_err_failed_to_decompose, 1);
	}
	if ( itm->is_item_shop_sale() && !(itm->is_storage_decompose_type())) {
		return send_header_to_player(p, p->waitcmd, cli_err_cannot_dec_attire, 1);
	}

	if (!(p->my_packs->is_clother_item_exist(unique_id, attire_id))) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_clothes, 1);
	}

	if (!(p->my_packs->is_clothes_forever(unique_id))) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_clothes, 1);
	}
	
	uint32_t lv = itm->use_lv();

	int idx = 0;

	if (itm->is_storage_decompose_type()) {
		uint32_t itemid = 0;
		uint32_t itemcnt = 0;
		
		uint32_t mid = 1540007;
		uint32_t mcnt = 0;

		if (lv <= 55) {
			itemcnt = (lv-5)/10;
			mcnt    = (lv-5)/10;
		} else {
			itemcnt = (lv-30)/5;
			mcnt    = (lv-30)/5;
		}
		switch (itm->equip_part()) {
			case 1:
				itemid = 1500909;
				break;
			case 2:
				break;
			case 3:
				itemid = 1500911;
				break;
			case 4:
				itemid = 1500913;
				break;
			case 5:
				break;
			case 6:
				itemid = 1500907;
				break;
			case 7:
				itemid = 1500910;
				break;
			case 8:
				itemid = 1500908;
				break;
			case 9:
				itemid = 1500912;
				break;
			case 10:
				break;
			default:
				break;
		}

		uint32_t item_type_cnt = 1;
		if (itemid) {
			item_type_cnt++;
		}
		pack_h(dbpkgbuf, unique_id, idx);
		pack_h(dbpkgbuf, item_type_cnt, idx); // mess of item type
		pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);

		if (itemid) {
			pack_h(dbpkgbuf, itemid, idx); 
			pack_h(dbpkgbuf, itemcnt, idx);
		}
		pack_h(dbpkgbuf, mid, idx); 
		pack_h(dbpkgbuf, mcnt, idx);

		
	} else {
		uint32_t chip_cnt = 0;//itm->use_lv() == 0 ? 1 : (itm->use_lv() + 9) / 10;
		uint32_t chip_id = 0;
		// calc num
		if (itm->quality_lv == 4) {
			if (lv >= 0 && lv <= 45) {
				chip_cnt = 1;
			} else {
				chip_cnt = (lv - 35 - 1) / 10 + 1;
			}
		} else {
			if (lv > 0 && lv <= 40) {
				chip_cnt = (lv + 9) / 10;
			} else if (lv == 0) {
				chip_cnt = 1;
			} else {
				chip_cnt = 4;
			}
		}

		// calc id
		chip_id = 1500901 + itm->quality_lv -2;

		pack_h(dbpkgbuf, unique_id, idx);
		pack_h(dbpkgbuf, 1, idx); // mess of item type
		pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);

		pack_h(dbpkgbuf, chip_id, idx); 
		pack_h(dbpkgbuf, chip_cnt, idx);
	}

	return send_request_to_db(p, p->id, p->role_tm, dbproto_decompose_attire, dbpkgbuf, idx);
}


uint32_t get_addition_odds(uint32_t addition_item_id)
{
	uint32_t tmp_per = 1;
	switch (addition_item_id) {
		case 2400001://ye ming zhu
			tmp_per = 50;
			break;
		case 2400002://long lin
			tmp_per = 5;
			break;
		case 2400003://ye guang zhu
			tmp_per = 3;
			break;
		case 2400004://jing gang li
			tmp_per = 2;
			break;
		case 2400005://jing hua
			tmp_per = 2;
			break;
			
	}
	return tmp_per;
}

uint32_t get_compose_attire_id(method_t* p_method, uint32_t pro_lv, uint32_t addition_item_id)
{
	uint32_t odds = ranged_random(1, 100);	
	
	uint32_t max_quality_per = 1;
	
	if (addition_item_id) {
		max_quality_per = get_addition_odds(addition_item_id);
	}

	bool b_first = true;
	uint32_t total_odds = 0;
	for(uint32_t i = max_output_compose_attire - 1; i>=0; i--)
	{
		need_method_item_t* p_need = &p_method->method_need_item[i];
		if (!(p_need->item_id)) {
			if (i == 0) {
				break;
			}
			continue;
		}
		if( odds <= total_odds + p_need->odds[pro_lv -1 ])
		{
			return p_need->item_id;		
		}
		if (b_first) {
			//p_need->odds[pro_lv -1 ];
			if (odds <= total_odds + p_need->odds[pro_lv -1 ] * max_quality_per) {
				return p_need->item_id;	
			}
			b_first = false;
		}
		total_odds += p_need->odds[pro_lv -1 ];
		if (i == 0) {
			break;
		}
	}
	return 0;
}
/**
 * @brief compose attire
 * @return 0 on success, -1 on error
 */
int do_compose_attire(player_t* p, uint32_t method_id, uint32_t addition_item_id)
{
	method_t* p_method = get_compose_method(method_id);
	if (!p_method) {
		ERROR_LOG("get_compose_method fail [%u %u]", p->id, method_id);
		return send_header_to_player(p, p->waitcmd, cli_err_failed_to_compose, 1);
	}
	if (addition_item_id && !(p->my_packs->is_item_exist(addition_item_id))) {
		return send_header_to_player(p, p->waitcmd, cli_err_failed_to_compose, 1);
	}
	//检查活力点	
	if(p_method->need_vitality_point > p->vitality_point){
		return send_header_to_player(p, p->waitcmd, cli_err_secondary_pro_not_enough_point, 1);		
	}
	//检查铁匠级别
	secondary_profession_data* pProData = get_player_secondary_profession_by_type(p, iron_man_profession_type);
	if(pProData == NULL){
		return send_header_to_player(p, p->waitcmd, cli_err_secondary_pro_not_exist, 1);
	}
	if( pProData->get_profession_level() < p_method->need_pro_lv){
		return send_header_to_player(p, p->waitcmd, cli_err_secondary_pro_low_level, 1);	
	}
	
	uint32_t attire_id = get_compose_attire_id(p_method, pProData->get_profession_level(), addition_item_id);
	const GfItem* itm = items->get_item(attire_id);
	if (!itm ) 
	{
		ERROR_LOG("get_compose_attire_id fail [%u %u]", p->id, attire_id);
		return send_header_to_player(p, p->waitcmd, cli_err_failed_to_compose, 1);
	}

	if (p_method->coins > p->coins) {
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
	}
	uint32_t is_clothes = 0;
	if (items->is_clothes(itm->category())) {
		is_clothes = 1;
	}
	uint32_t clothes_level = 0;
	uint32_t now = get_now_tv()->tv_sec;
	uint32_t duration = 0;
	if (is_clothes) {
		duration = itm->duration * clothes_duration_ratio;
	} else {
		duration = 1;
	}
	uint32_t max_num = itm->max();
	int idx = 0;

	pack_h(dbpkgbuf, method_id, idx);
	pack_h(dbpkgbuf, get_player_total_item_bag_grid_count(p), idx);
	pack_h(dbpkgbuf, p_method->coins, idx);// reduce coins
	pack_h(dbpkgbuf, is_clothes, idx);
	pack_h(dbpkgbuf, max_num, idx);
	pack_h(dbpkgbuf, attire_id, idx);

	pack_h(dbpkgbuf, now, idx);
	pack_h(dbpkgbuf, clothes_level, idx);
	pack_h(dbpkgbuf, duration, idx);

	pack_h(dbpkgbuf, addition_item_id, idx);
	uint32_t odds = ranged_random(0, 100);	
	if (odds < 1) {
		pack_h(dbpkgbuf, 2400005, idx);
	} else {
		pack_h(dbpkgbuf, 0, idx);
	}
	//pack_h(dbpkgbuf, p_method->in_cnt + 1, idx); 
	pack_h(dbpkgbuf, p_method->in_cnt, idx); 

	/*
	const GfItem* itm_method = items->get_item(p_method->id);
	if (itm_method && items->is_clothes(itm_method->category())) 
	{
		pack_h(dbpkgbuf, 1, idx);
	} 
	else 
	{
		pack_h(dbpkgbuf, 0, idx);
	}
	pack_h(dbpkgbuf, p_method->id, idx);
	pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);
	*/

	for (uint32_t i = 0; i < p_method->in_cnt; ++i) 
	{
		const GfItem* itm = items->get_item(p_method->product_item[i].item_id);
		if (itm && items->is_clothes(itm->category())) 
		{
			pack_h(dbpkgbuf, 1, idx);
			uint32_t unique_id = p->my_packs->get_lowest_quality_attire(p_method->product_item[i].item_id);
			if (!unique_id) 
			{
				ERROR_LOG("%u do not have clothes: %u", p->id, p_method->product_item[i].item_id);
				return send_header_to_player(p, p->waitcmd, cli_err_failed_to_compose, 1);
			}
			pack_h(dbpkgbuf, unique_id, idx);
		} 
		else 
		{
			pack_h(dbpkgbuf, 0, idx);
			pack_h(dbpkgbuf, p_method->product_item[i].item_id, idx);
		}
		pack_h(dbpkgbuf, p_method->product_item[i].cnt, idx);
	}	
	return send_request_to_db(p, p->id, p->role_tm, dbproto_compose_attire, dbpkgbuf, idx);
}
/**
 * @different quality attire has it's oneself toplimit
 */
bool is_attire_can_strengthen(uint32_t lv, uint32_t quality)
{
    return lv < 12;
    /*
    switch (quality) {
        case 1:
        case 2:
            return lv < 3;
        case 3:
            return lv < 6;
        case 4:
            return lv < 12;
    }
    return false;
    */
}
/**
 * @brief do strengthen attire
 */
int do_strengthen_attire(player_t* p, strengthen_attire_t* p_strengthen)
{
    const GfItem* itm = items->get_item(p_strengthen->attireid);
    uint32_t cur_lv = p->my_packs->get_clothes_lv_by_id(p_strengthen->uniquekey);

	if (!(p->my_packs->is_clother_item_exist(p_strengthen->uniquekey))) {
		WARN_LOG("strengthen attire not exist: %u %u ", p->id, p_strengthen->attireid);
		return send_header_to_player(p, p->waitcmd, cli_err_attire_cannot_strengthen, 1);
	}
    if ( !itm || !items->is_clothes(itm->category()) || items->is_jew_clothes(itm->equip_part()) 
        || ( !items->is_def_clothes(itm->equip_part()) && !items->is_atk_clothes(itm->equip_part()) )
        || !is_attire_can_strengthen(cur_lv, itm->quality_lv) ) {
        ERROR_LOG("attire not exist or cannot strengthen uid=[%u] attireid=[%u]",p->id,p_strengthen->attireid);
        return send_header_to_player(p, p->waitcmd, cli_err_attire_cannot_strengthen, 1);
    }
    //gongfu dou
    if ( !is_enough_to_buy(p, strengthen_consume_coins[cur_lv], 1) ) {
		TRACE_LOG("uid=[%u] [%u %u]", p->id, strengthen_consume_coins[cur_lv], p->coins);
		return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
	}

    //weapon
    if (items->is_atk_clothes(itm->equip_part())) {
        if (p_strengthen->material_cnt < 3) {
            TRACE_LOG("strengthen weapon material not enough. uid=[%u] attireid=[%u]",p->id,p_strengthen->attireid);
            return send_header_to_player(p, p->waitcmd, cli_err_strengthen_material_notenough, 1);
        }
        p_strengthen->material_cnt = 3;
    } else {
        if (p_strengthen->material_cnt < 1) {
            TRACE_LOG("strengthen clothes material not enough. uid=[%u] attireid=[%u]",p->id,p_strengthen->attireid);
            return send_header_to_player(p, p->waitcmd, cli_err_strengthen_material_notenough, 1);
        }
        p_strengthen->material_cnt = 1;
    }

    std::map<uint32_t, strengthen_t>::iterator miter;
    std::map<uint32_t, strengthen_t>::iterator iter;
    miter = material_map.find(p_strengthen->material_id);
    if ( miter == material_map.end() ) {
        WARN_LOG("strengthen material not enough. uid=[%u] materialid=[%u]",p->id,p_strengthen->material_id);
        return send_header_to_player(p, p->waitcmd, cli_err_strengthen_material_notenough, 1);
    }
    if (itm->quality_lv < miter->second.quality[0] || itm->quality_lv > miter->second.quality[1]) {
        TRACE_LOG("strengthen material quality error. uid=[%u] materialid=[%u]",p->id,p_strengthen->material_id);
        return send_header_to_player(p, p->waitcmd, cli_err_strengthen_material_quality, 1);
    }
    p_strengthen->attirelv = cur_lv;
    if ( cur_lv < miter->second.attirelv[0] || cur_lv >= miter->second.attirelv[1] ) {
        TRACE_LOG("strengthen material level error. uid=[%u] materialid=[%u]",p->id,p_strengthen->material_id);
        return send_header_to_player(p, p->waitcmd, cli_err_strengthen_material_level, 1);
    }

    if (p_strengthen->stoneid_1 != 0) {
        iter = material_map.find(p_strengthen->stoneid_1);
        if ( iter->second.type != 2 || iter == material_map.end() ) {
            WARN_LOG("strengthen stone type=2 error. uid=[%u] stoneid=[%u]",p->id,p_strengthen->stoneid_1);
            return send_header_to_player(p, p->waitcmd, cli_err_strengthen_stone_type, 1);
        }
        if (itm->quality_lv < iter->second.quality[0] || itm->quality_lv > iter->second.quality[1]) {
            TRACE_LOG("strengthen stone quality error. uid=[%u] stoneid=[%u]",p->id,p_strengthen->stoneid_1);
            return send_header_to_player(p, p->waitcmd, cli_err_strengthen_stone_quality, 1);
        }
        if ( cur_lv < iter->second.attirelv[0] || cur_lv >= iter->second.attirelv[1] ) {
            TRACE_LOG("strengthen stone level error. uid=[%u] stoneid=[%u]",p->id,p_strengthen->stoneid_1);
            return send_header_to_player(p, p->waitcmd, cli_err_strengthen_stone_level, 1);
        }

        //use_lv
        if (p_strengthen->stoneid_1 == 1740028) {
            uint32_t store_1_cnt = p->my_packs->get_material_cnt(p_strengthen->stoneid_1);
            if (itm->use_lv() <= 45) {
                if (p_strengthen->stoneid_1_cnt < 1 || store_1_cnt < 1) {
                    TRACE_LOG("strengthen stone count [%u] lv [%u]", p_strengthen->stoneid_1_cnt, itm->use_lv());
                    return send_header_to_player(p, p->waitcmd, cli_err_strengthen_stone_count, 1);
                }
                p_strengthen->stoneid_1_cnt = 1;
            } else if (itm->use_lv() <= 60 && itm->use_lv() > 45) {
                if (p_strengthen->stoneid_1_cnt < 2 || store_1_cnt < 2) {
                    TRACE_LOG("strengthen stone count [%u] lv [%u]", p_strengthen->stoneid_1_cnt, itm->use_lv());
                    return send_header_to_player(p, p->waitcmd, cli_err_strengthen_stone_count, 1);
                }
                p_strengthen->stoneid_1_cnt = 2;
            } else if (itm->use_lv() > 60) {
                if (p_strengthen->stoneid_1_cnt < 3 || store_1_cnt < 3) {
                    TRACE_LOG("strengthen stone count [%u] lv [%u]", p_strengthen->stoneid_1_cnt, itm->use_lv());
                    return send_header_to_player(p, p->waitcmd, cli_err_strengthen_stone_count, 1);
                }
                p_strengthen->stoneid_1_cnt = 3;
            } else {
                return send_header_to_player(p, p->waitcmd, cli_err_strengthen_stone_count, 1);
            }
        }
    }
    if (p_strengthen->stoneid_2 != 0) {
        iter = material_map.find(p_strengthen->stoneid_2);
        if (iter->second.type != 3 || iter == material_map.end() ) {
            WARN_LOG("strengthen stone type=3 error. uid=[%u] stoneid=[%u]",p->id,p_strengthen->stoneid_2);
            return send_header_to_player(p, p->waitcmd, cli_err_strengthen_stone_type, 1);
        }
        if (itm->quality_lv < iter->second.quality[0] || itm->quality_lv > iter->second.quality[1]) {
            TRACE_LOG("strengthen stone quality error. uid=[%u] stoneid=[%u]",p->id,p_strengthen->stoneid_1);
            return send_header_to_player(p, p->waitcmd, cli_err_strengthen_stone_quality, 1);
        }
        if ( cur_lv < iter->second.attirelv[0] || cur_lv >= iter->second.attirelv[1] ) {
            TRACE_LOG("strengthen stone level error. uid=[%u] stoneid=[%u]",p->id,p_strengthen->stoneid_1);
            return send_header_to_player(p, p->waitcmd, cli_err_strengthen_stone_level, 1);
        }
    }

    KDEBUG_LOG(p->id, "STRENGTHEN ATTIRE\t uid=[%u] [%u %u %u %u %u %u %u]",p->id,p_strengthen->attireid, p_strengthen->uniquekey,
        p_strengthen->attirelv,p_strengthen->material_id,p_strengthen->material_cnt,p_strengthen->stoneid_1,
        p_strengthen->stoneid_2);
    uint32_t rand_num = rand() % 10000;
    uint32_t odds = 0;
    uint32_t fail_flag = 1; //0--fail; 1--success

    if (miter->second.lucky == 0) {
        if (( cur_lv < 3 && p->strengthen_cnt != 0 && p->strengthen_cnt % 6 == 0)
            || p_strengthen->stoneid_2 != 0 
            || rand_num < miter->second.odds[cur_lv + 1]) {
            KDEBUG_LOG(p->id, "STRENGTHEN SUCCESS [%u %u %u] odds[%u %u]", 
                cur_lv, p->strengthen_cnt, p_strengthen->stoneid_2,
                rand_num, miter->second.odds[cur_lv + 1]);
            p_strengthen->attirelv += 1;
            do_stat_log_universal_interface_1(stat_log_strengthen_succ, cur_lv + 1, 1);
        } else {
            rand_num = rand() % 10000;
            if (p_strengthen->stoneid_1 == 0 && rand_num < scut_level_rate[cur_lv]) {
                p_strengthen->attirelv -= 1;
				
				uint32_t zero_rand = rand() % 10000;
				if (zero_rand < reset_level_rate[cur_lv]) {
					p_strengthen->attirelv = 0;
				} 
            } 
            if (cur_lv < 3) {
                p->strengthen_cnt += 1;
                fail_flag = 0;
            }
            do_stat_log_universal_interface_1(stat_log_strengthen_fail, p_strengthen->attirelv, 1);
        }
    } else if (miter->second.lucky == 1) {
        for (uint32_t i = 0; i < max_attire_level+1; i++) {
            odds += miter->second.odds[i];
            if ( rand_num < odds ) {
                p_strengthen->attirelv = i;
                break;
            }
        }
    }
    uint32_t stat_id = ((p_strengthen->material_cnt % 3) * 2 + (itm->quality_lv - 3)) << 8;
    do_stat_log_universal_interface_1(stat_log_strengthen_level, stat_id + p_strengthen->attirelv, 1);
    int idx = 0;
    pack_h(dbpkgbuf, strengthen_consume_coins[cur_lv], idx);// reduce coins
    pack_h(dbpkgbuf, fail_flag, idx);
    pack_h(dbpkgbuf, p_strengthen->attireid, idx);
    pack_h(dbpkgbuf, p_strengthen->uniquekey, idx);
    pack_h(dbpkgbuf, p_strengthen->attirelv, idx);
    pack_h(dbpkgbuf, p_strengthen->material_id, idx);
    pack_h(dbpkgbuf, p_strengthen->material_cnt, idx);
    pack_h(dbpkgbuf, p_strengthen->stoneid_1, idx);
    pack_h(dbpkgbuf, p_strengthen->stoneid_1_cnt, idx);
    pack_h(dbpkgbuf, p_strengthen->stoneid_2, idx);
    return send_request_to_db(p, p->id, p->role_tm, dbproto_strengthen_attire, dbpkgbuf, idx);
}
/**
 * @brief check if the attire can be strengthen
 * @return 0 on success, -1 on error
 */
int is_attire_without_material_ok(player_t* p, uint32_t uniquekey)
{
	if (!(p->my_packs->is_clother_item_exist(uniquekey))) {
		TRACE_LOG("strengthen attire not exist: %u %u ", p->id, uniquekey);
        return 0;
	}
	pack_clothes_info_t *att = p->my_packs->get_clothes_info_by_unique_id(uniquekey);
    if (!att) {
        return 0;
    }
  	const GfItem* itm = items->get_item(att->clothes_id);
	  if ( !itm || !items->is_clothes(itm->category()) || items->is_jew_clothes(itm->equip_part()) 
        || !is_attire_can_strengthen(att->lv, itm->quality_lv) || itm->quality_lv >= 5 ) {
        ERROR_LOG("attire not exist or cannot strengthen uid=[%u] attireid=[%u]",p->id,att->clothes_id);
        return 0;
    }
	  return 1;
}


/**
 * @brief do strengthen attire without material
 * @return 0 on success, -1 on error
 */
int do_strengthen_attire_without_material(player_t* p, uint32_t uniquekey){
	pack_clothes_info_t *att = p->my_packs->get_clothes_info_by_unique_id(uniquekey);
	int idx = 0;
	att->lv += 1;
	pack_h(dbpkgbuf, uniquekey,idx);
	pack_h(dbpkgbuf, att->lv, idx);
	TRACE_LOG("cws strengthen_attire_without_material to db");
	return send_request_to_db(0, p->id, p->role_tm, dbproto_strengthen_without_material, dbpkgbuf, idx);
}
/**
 * @brief do appearance buf
 * @return 0 on success, -1 on error
 */
void do_item_buf(player_t* p, const GfItem* itm)
{
	BufSkillMap::iterator it = p->buf_skill_map->find(itm->buf_type());
	if (it == p->buf_skill_map->end()) {
		
		buf_skill_t buf_skill = {0};
		buf_skill.buf_duration = itm->buf_duration();
		buf_skill.buf_type = itm->buf_type();
		buf_skill.per_trim = itm->per_trim();
		buf_skill.ex_id1 = itm->buf_ex_id1();
		buf_skill.db_buff_flg = 1;
		send_appearance_buf(p, &buf_skill);
		p->buf_skill_map->insert(BufSkillMap::value_type(itm->buf_type(), buf_skill));
		
		TRACE_LOG("INSERT APP BUF: %u %u %u %u", buf_skill.buf_type, buf_skill.buf_duration, 
			buf_skill.per_trim, buf_skill.ex_id1);
	} else {
		TRACE_LOG("BEFORE UPDATE: %u %u %u %u", it->second.buf_type, it->second.buf_duration, 
					it->second.per_trim, it->second.ex_id1);
		
		it->second.buf_duration = itm->buf_duration();
		it->second.buf_type = itm->buf_type();
		it->second.per_trim = itm->per_trim();
		it->second.ex_id1 = itm->buf_ex_id1();
		send_appearance_buf(p, &(it->second));
		
		TRACE_LOG("AFTER UPDATE: %u %u %u %u", it->second.buf_type, it->second.buf_duration, 
					it->second.per_trim, it->second.ex_id1);
	}
	//set mon id
	p->app_mon = itm->buf_ex_id1();
	//set end timer
	if (p->app_t) {
		mod_expire_time(p->app_t, get_now_tv()->tv_sec + itm->keep_tm());
	} else {
		p->app_t = ADD_TIMER_EVENT_EX(p, set_app_buf_end_idx, 0, get_now_tv()->tv_sec + itm->keep_tm());
	}
	TRACE_LOG("%u %u", p->app_mon, itm->keep_tm());

}


//simply change appearance with no item used 
//added by cws 0611
void chg_app_buff(player_t* p, uint32_t buff_id)
{
   	 BufSkillMap::iterator it = p->buf_skill_map->find(buff_id);
	buf_skill_t buf_skill = {0};
    uint32_t duration = 0, ex_id = 0;
    if (it != p->buf_skill_map->end()) 
    {
        TRACE_LOG("already have buff,chg app again");
        it->second.ex_id1 = 10403 ;
        it->second.buf_duration = 3600 * 24 * 7 ;
        duration = it->second.buf_duration;
        ex_id = it->second.ex_id1;
        TRACE_LOG("cws %d and %d",it->second.ex_id1,it->second.buf_duration);
        send_appearance_buf(p, &it->second);
    }

    else {


        buf_skill.buf_duration = 3600 * 24 * 7;
        buf_skill.buf_type = buff_id;//id for test
        buf_skill.per_trim = 0;
		buf_skill.ex_id1 = 10403;
		buf_skill.db_buff_flg = 0;
 	       buf_skill.start_tm = get_now_tv()->tv_sec;
		duration= buf_skill.buf_duration;
		ex_id = buf_skill.ex_id1;
		send_appearance_buf(p, &buf_skill);
		p->buf_skill_map->insert(BufSkillMap::value_type(buff_id, buf_skill));
		
		TRACE_LOG("CWS INSERT APP BUF: %u %u %u %u", buf_skill.buf_type, buf_skill.buf_duration, 
			buf_skill.per_trim, buf_skill.ex_id1);
	} 
		//set mon id
	p->app_mon = ex_id;
    //TRACE_LOG("%u %u", p->app_mon, buf_skill.buf_duration);
}

void chg_app_buff_back(player_t* p, uint32_t buff_id)
{
    BufSkillMap::iterator it = p->buf_skill_map->find(buff_id);
    if (it != p->buf_skill_map->end())  {
        p->buf_skill_map->erase(buff_id);
        TRACE_LOG("ERASE BUF: %u", buff_id);
        send_appearance_buf(p, 0);
        p->app_mon = 0;
        p->app_t = 0;
    }
}

/////
/**
 * @brief offline will be cut
 */
void add_empty_app_buff(player_t* p, uint32_t buff_id)
{
    BufSkillMap::iterator it = p->buf_skill_map->find(buff_id);
	if (it == p->buf_skill_map->end()) {
		
		buf_skill_t buf_skill = {0};
		buf_skill.buf_duration = 3600 * 12;
		buf_skill.buf_type = buff_id;
		buf_skill.per_trim = 0;
		buf_skill.ex_id1 = 0;
		buf_skill.db_buff_flg = 1;
        buf_skill.start_tm = get_now_tv()->tv_sec;
		//send_appearance_buf(p, &buf_skill);
		p->buf_skill_map->insert(BufSkillMap::value_type(buff_id, buf_skill));
		
		TRACE_LOG("INSERT APP BUF: %u %u %u %u", buf_skill.buf_type, buf_skill.buf_duration, 
			buf_skill.per_trim, buf_skill.ex_id1);
	} 
}

/**
 * @brief
 * @result true-->exist false-->not exist;
 */
bool is_app_buff_exist(player_t* p, uint32_t buff_id)
{
    BufSkillMap::iterator it = p->buf_skill_map->find(buff_id);
	if (it != p->buf_skill_map->end()) {
        return true;
    }
    return false;
}

uint32_t get_app_buff_passtime(player_t* p, uint32_t buff_id)
{
    BufSkillMap::iterator it = p->buf_skill_map->find(buff_id);
	if (it != p->buf_skill_map->end()) {
		return (get_now_tv()->tv_sec - it->second.start_tm);
    }
    return 0;
}

/**
 * @brief query restriction
 * @return 0 on success, -1 on error
 */
int db_query_restriction_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_query_restriction_list, 0, 0);
}

/**
  * @brief callback for buying items
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_buy_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	int idx = 0;
	const buy_item_rsp_t* rsp = reinterpret_cast<buy_item_rsp_t*>(body);
    uint32_t costs = p->coins - rsp->coins;
	p->coins = rsp->coins;

	if (p->coins > rsp->coins) {
		do_stat_log_universal_interface_1(stat_log_destroy_coins, 0, p->coins - rsp->coins);
	}
	do_stat_log_coin_add_reduce(p, (uint32_t)0, costs, channel_string_npc_shop);


	//stat log :got skill book
	do_stat_log_amber( stat_log_got_amber, rsp->itmid, p->role_type);

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->coins, idx);
	pack(pkgbuf, rsp->itmid, idx);
	pack(pkgbuf, rsp->cnt, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);

	//add to player's pack
	if (p->my_packs->add_item(p, rsp->itmid, rsp->cnt, channel_string_npc_shop) < 0) {
		WARN_LOG("add item error![%u %u]", p->id, p->my_packs->all_items_cnt());
	}

	//monitor
    report_add_to_monitor(p, 1, rsp->itmid, rsp->cnt, costs);
	//save buy item limit data
	buy_item_limit_data* pdata = get_buy_item_limit_mgr()->get_limit_data_by_item_id(rsp->itmid);
	if(pdata)
	{
		add_player_buy_item_limit_data( get_buy_item_limit_mgr(), p, rsp->itmid, rsp->cnt);
		player_item_limit_data* ppdata = get_player_item_limit_data_by_item_id(p, rsp->itmid);
		db_set_buy_item_limit_data(p, ppdata);
	}	

	KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s]", 
				1, rsp->itmid, rsp->cnt, channel_string_npc_shop);

	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief callback for buying clothes
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_buy_clothes_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	const buy_clothes_rsp_t* rsp = reinterpret_cast<buy_clothes_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(buy_clothes_rsp_t) + rsp->count * sizeof(db_clothes_elem_t));
	
    uint32_t cost_fumo = 0;
	if (rsp->type == buy_with_exploit) {
		p->exploit = rsp->coins;
	} else if (rsp->type == buy_with_coins){
		if (p->coins > rsp->coins) {
			do_stat_log_universal_interface_1(stat_log_destroy_coins, 0, p->coins - rsp->coins);
		}
		do_stat_log_coin_add_reduce(p, rsp->coins);
		p->coins = rsp->coins;
	} else if (rsp->type == buy_with_fumo_points){
        cost_fumo = p->fumo_points_total - rsp->coins;
		do_stat_log_fumo_add_reduce((uint32_t)0, cost_fumo);
		p->fumo_points_total = rsp->coins;
	}
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->type, idx);
	pack(pkgbuf, rsp->coins, idx);
	pack(pkgbuf, rsp->count, idx);
    const GfItem* clothes;
	for (uint32_t i = 0; i < rsp->count; i++) {
		pack(pkgbuf, rsp->clothes[i].clothes_id, idx);
		pack(pkgbuf, rsp->clothes[i].unique_id, idx);
		KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s mytype=%u]", 
			2, rsp->clothes[i].clothes_id, 0, channel_string_npc_shop, rsp->type);
		//add to player's pack
		if (p->my_packs->add_clothes(p, rsp->clothes[i].clothes_id, rsp->clothes[i].unique_id, 0, channel_string_npc_shop) < 0) {
			WARN_LOG("add clothes error![%u %u]", p->id, p->my_packs->all_clothes_cnt());
		}

        //send to monitor
        clothes = items->get_item(rsp->clothes[i].clothes_id);

		if (clothes->swap_action_id()) {
			add_swap_action_times(p, clothes->swap_action_id());
		}
        report_add_to_monitor(p, 1, rsp->clothes[i].clothes_id, 1, clothes->price());
        if (rsp->type == buy_with_fumo_points) {
            do_stat_log_fumo_consume(rsp->clothes[i].clothes_id, p->lv, p->id, cost_fumo/rsp->count);
        }
	}
	//pack(pkgbuf, rsp->get_time, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief callback for selling item
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_sell_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	const sell_item_rsp_t* rsp = reinterpret_cast<sell_item_rsp_t*>(body);

	do_stat_log_coin_add_reduce(p, rsp->coins, channel_string_npc_shop);
    uint32_t costs = rsp->coins - p->coins;
	if (rsp->coins > p->coins) {
		do_stat_log_universal_interface_1(stat_log_produce_coins, 0, rsp->coins - p->coins);
	}
	KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s]", 
				3, 1, costs, channel_string_npc_shop);
	do_stat_log_coin_add_reduce(p, rsp->coins, channel_string_npc_shop);
	p->coins = rsp->coins;

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->coins, idx);
	pack(pkgbuf, rsp->item_id, idx);
	pack(pkgbuf, rsp->item_cnt, idx);

	TRACE_LOG("coins:%u itemid=%u cnt=%u", rsp->coins, rsp->item_id, rsp->item_cnt);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	
	
	//add to player's pack
	if (p->my_packs->del_item(p, rsp->item_id, rsp->item_cnt, channel_string_npc_shop) < 0) {
		WARN_LOG("del item error![%u %u]", p->id, p->my_packs->all_items_cnt());
	}

	//send to monitor
    report_add_to_monitor(p, 2, rsp->item_id, rsp->item_cnt, costs);
	return send_to_player(p, pkgbuf, idx, 1);
}


/**
  * @brief callback for selling clothes
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_sell_clothes_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	const sell_clothes_rsp_t* rsp = reinterpret_cast<sell_clothes_rsp_t*>(body);
	do_stat_log_coin_add_reduce(p, rsp->coins);
    uint32_t costs = rsp->coins - p->coins;
	if (rsp->coins > p->coins) {
		do_stat_log_universal_interface_1(stat_log_produce_coins, 0, rsp->coins - p->coins);
	}
	KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s]", 
				3, 1, costs, channel_string_npc_shop);
	p->coins = rsp->coins;

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->coins, idx);
	pack(pkgbuf, rsp->clothes_id, idx);
	pack(pkgbuf, rsp->unique_id, idx);

	TRACE_LOG("coins:%u clothesid=%u uniqueid=%u", rsp->coins, rsp->clothes_id, rsp->unique_id);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	
	//add to player's pack
	if (p->my_packs->del_clothes(rsp->unique_id, channel_string_npc_shop) < 0) {
		WARN_LOG("del clothes error![%u %u]", p->id, p->my_packs->all_clothes_cnt());
	}

	//send to monitor
    report_add_to_monitor(p, 2, rsp->clothes_id, 1, costs);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief callback for selling item
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_batch_sell_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

    batch_sell_item_rsp_t* rsp = reinterpret_cast<batch_sell_item_rsp_t*>(body);
    CHECK_VAL_EQ(bodylen, sizeof(batch_sell_item_rsp_t) + rsp->cnt * sizeof(db_item_elem_t));

    if (rsp->type == 2) {
        do_stat_log_coin_add_reduce(p, rsp->coins);
        uint32_t costs = rsp->coins - p->coins;
        if (rsp->coins > p->coins) {
            do_stat_log_universal_interface_1(stat_log_produce_coins, 0, rsp->coins - p->coins);
        }
        KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s]", 
            3, 1, costs, channel_string_npc_shop);
        p->coins = rsp->coins;
    } else if (rsp->type == 1) {
        do_stat_log_universal_interface_2(0x09526205, 0, p->id, rsp->coins);
        p->my_packs->add_item(p, gold_coin_item_id, rsp->coins, channel_string_use_tongbao_item);
    }

    db_item_elem_t * p_item = rsp->item;

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->type, idx);
	pack(pkgbuf, rsp->coins, idx);
	pack(pkgbuf, rsp->cnt, idx);
    for (uint32_t i = 0; i < rsp->cnt; i++) {
        pack(pkgbuf, p_item->item_id, idx);
        pack(pkgbuf, p_item->count, idx);

        p->my_packs->del_item(p, p_item->item_id, p_item->count, channel_string_npc_shop);
        TRACE_LOG("del itemid=%u cnt=%u", p_item->item_id, p_item->count);
	    //send to monitor
        //report_add_to_monitor(p, 2, p_item->item_id, p_item->item_cnt, costs);
        p_item++;
    }

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


/**
  * @brief callback for repairing all clothes
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_repair_all_clothes_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	repair_all_clothes_rsp_t* rsp = reinterpret_cast<repair_all_clothes_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, 8 + rsp->clothes_cnt * sizeof(db_repair_all_clothes_elemt_t));

	do_stat_log_coin_add_reduce(p, rsp->left_coins);
	do_stat_log_universal_interface_1(stat_log_destroy_coins, 0, p->coins > rsp->left_coins ? p->coins - rsp->left_coins : 0);
	p->coins = rsp->left_coins;
	db_repair_all_clothes_elemt_t* p_elem = rsp->clothes;
	
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->left_coins, idx);
	pack(pkgbuf, rsp->clothes_cnt, idx);
	for (uint32_t i = 0; i < rsp->clothes_cnt; i++) {
		pack(pkgbuf, p_elem->clothes_id, idx);
		pack(pkgbuf, p_elem->unique_id, idx);
		pack(pkgbuf, p_elem->duration, idx);
		
		update_used_clothes_duration(p, p_elem->unique_id, p_elem->duration * 50);
		
		TRACE_LOG("coins:%u clothesid=%u uniqueid=%u duration=%u", rsp->left_coins, 
				p_elem->clothes_id, p_elem->unique_id, p_elem->duration);
		p_elem ++;
	}
	if (p->waitcmd == cli_proto_use_item) {
		uint32_t* p_itemid = (uint32_t*)(p->session);
		if (p->battle_grp) {
			return btlsvr_player_use_item(p, *p_itemid);
		}
		return send_use_item_rsp(p, *p_itemid, 1);
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
 * @brief callback for getting clothes unused list
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_get_clothes_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	get_clothes_list_rsp_t* rsp = reinterpret_cast<get_clothes_list_rsp_t*>(body);
	
	CHECK_VAL_EQ(bodylen, sizeof(get_clothes_list_rsp_t) + rsp->clothes_cnt * sizeof(db_get_clothes_list_elem_t));

	int idx = sizeof(cli_proto_t);
	//uint32_t max_pack_clothes = max_pack_clothes_cnt;
	uint32_t max_pack_clothes = get_player_total_item_bag_grid_count(p);
	pack(pkgbuf, max_pack_clothes, idx);
	pack(pkgbuf, rsp->clothes_cnt, idx);
	for ( uint32_t i = 0; i < rsp->clothes_cnt; i++) {
		db_get_clothes_list_elem_t* elem = &(rsp->clothes[i]);
		uint32_t duration = elem->duration;
		pack(pkgbuf, elem->clothes_id, idx);
		pack(pkgbuf, elem->unique_id, idx);
		pack(pkgbuf, duration, idx);
		pack(pkgbuf, elem->attirelv, idx);
		pack(pkgbuf, elem->gettime, idx);
        pack(pkgbuf, elem->timelag, idx);
		pack(pkgbuf, elem->usedflag, idx);
		TRACE_LOG("uid=[%u],id=[%d],unique=[%d],duration=[%u],attirelv=[%u],gettime=[%u],timelag=[%x],usedflag=[%u]",
				p->id,elem->clothes_id,elem->unique_id, elem->duration,elem->attirelv,elem->gettime,elem->timelag,elem->usedflag);
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}
/**
 * @brief callback for getting strengthen material list
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_get_strengthen_material_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
    CHECK_DBERR(p, ret);
    get_item_list_rsp_t* rsp = reinterpret_cast<get_item_list_rsp_t*>(body);
    CHECK_VAL_EQ(bodylen, sizeof(get_item_list_rsp_t) + rsp->item_cnt * sizeof(db_item_elem_t));

    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->item_cnt, idx);
    for (uint32_t i = 0; i < rsp->item_cnt; i++) {
        db_item_elem_t* elem = &(rsp->item[i]);
        //update player strengthen material pack
        p->my_packs->add_item(p, elem->item_id, elem->count, 0);

        pack(pkgbuf, elem->item_id, idx);
        pack(pkgbuf, elem->count, idx);
        TRACE_LOG("uid=[%u] material_id=[%u] material_cnt=[%u]",p->id,elem->item_id,elem->count);
    }
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx); 
    return send_to_player(p, pkgbuf, idx, 1);
}
/**
 * @brief callback for getting clothes list
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_wear_clothes_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	wear_clothes_rsp_t* rsp = reinterpret_cast<wear_clothes_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(wear_clothes_rsp_t) + rsp->clothes_cnt * sizeof(db_wear_clothes_elem_t));

	//update player's pack
	for ( uint32_t i = 0; i < rsp->clothes_cnt; i++) {
		const db_wear_clothes_elem_t* elem = &(rsp->clothes[i]);
		p->my_packs->del_clothes(elem->unique_id, 0);
	}
	for ( uint32_t i = 0; i < p->clothes_num; i++) {
		bool add_flag = true;
		for ( uint32_t j = 0; j < rsp->clothes_cnt; j++) {
			const db_wear_clothes_elem_t* elem = &(rsp->clothes[j]);
			if (p->clothes[i].unique_id == elem->unique_id) {
				add_flag = false;
				break;
			}
		}
		if (add_flag) {
			p->my_packs->add_clothes(p, p->clothes[i].clothes_id, p->clothes[i].unique_id, p->clothes[i].duration, 0,
                p->clothes[i].gettime,p->clothes[i].timelag,p->clothes[i].lv);
		}
	}
	
	//set player's clothes num
	if ( rsp->clothes_cnt <= max_clothes_on ) {
		p->clothes_num = rsp->clothes_cnt;
	} else {
		WARN_LOG("too many clothes on: uid=%u cnt=%u", p->id, rsp->clothes_cnt);
		p->clothes_num = max_clothes_on;
	}

//	if(check_all_attire_quality_level(p, 4) && !is_player_achievement_data_exist(p, 71))
//	{
//		do_achieve_type_logic(p, 71, false);
//	}



	if (p->trade_grp) {
		KDEBUG_LOG(p->id, "PLAYER WEAR CLOTHES TO TRADE \t[%u]", p->id);
	  	p->waitcmd = 0;
		int idx = sizeof(tr_proto_t);
		pack_h(pkgbuf, p->clothes_num, idx);
		for ( uint32_t i = 0; i < p->clothes_num; i++) {
			db_wear_clothes_elem_t* elem = &(rsp->clothes[i]);
			pack_h(pkgbuf, elem->clothes_id, idx);
			pack_h(pkgbuf, elem->unique_id, idx);
			pack_h(pkgbuf, elem->duration, idx);
			pack_h(pkgbuf, elem->attirelv, idx);
			//pack(pkgbuf, elem->get_time, idx);
			p->clothes[i].clothes_id = elem->clothes_id;
			p->clothes[i].unique_id = elem->unique_id;
			p->clothes[i].duration = elem->duration;
			p->clothes[i].lv = elem->attirelv;
			p->clothes[i].gettime = elem->gettime;
			p->clothes[i].timelag = elem->timelag;
			TRACE_LOG("db wear clothes list:uid=[%u],clothes_id=[%d],unique=[%d],lv=[%u],duration=[%u],gettime=[%u],endtime=[%u]",
				p->id, p->clothes[i].clothes_id, p->clothes[i].unique_id, p->clothes[i].lv,p->clothes[i].duration,
                p->clothes[i].gettime,p->clothes[i].timelag);
		}

		init_tr_proto_head(p, p->id, pkgbuf, idx, tr_player_wear_clothes);

		if (send_to_trade_svr(p, pkgbuf, idx, p->trade_grp->fd_idx) < 0)
		{
			ERROR_LOG("TRADE SVR SEND ERROR [%u %u]", p->id, p->waitcmd);
			send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}

		/*recalc the attr of player*/
        calc_and_save_player_attribute(p, 0);
		return 0;
	}

	//send to player
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->clothes_num, idx);
	for ( uint32_t i = 0; i < p->clothes_num; i++) {
		db_wear_clothes_elem_t* elem = &(rsp->clothes[i]);
		pack(pkgbuf, elem->clothes_id, idx);
		pack(pkgbuf, elem->unique_id, idx);
		pack(pkgbuf, elem->duration, idx);
		pack(pkgbuf, elem->attirelv, idx);
		//pack(pkgbuf, elem->get_time, idx);
		p->clothes[i].clothes_id = elem->clothes_id;
		p->clothes[i].unique_id = elem->unique_id;
		p->clothes[i].duration = elem->duration;
		p->clothes[i].lv = elem->attirelv;
		p->clothes[i].gettime = elem->gettime;
		p->clothes[i].timelag = elem->timelag;
		TRACE_LOG("db wear clothes list:uid=[%u],clothes_id=[%d],unique=[%d],lv=[%u],duration=[%u],gettime=[%u],endtime=[%u]",
				p->id, p->clothes[i].clothes_id, p->clothes[i].unique_id, p->clothes[i].lv,p->clothes[i].duration,
                p->clothes[i].gettime,p->clothes[i].timelag);
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_map(p, pkgbuf, idx, 1);
	
	/*recalc the attr of player*/
    calc_and_save_player_attribute(p, 0);
	proc_player_clothes_logic(p);
	return 0;
}

/**
 * @brief callback for getting item list
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_get_item_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	get_item_list_rsp_t* rsp = reinterpret_cast<get_item_list_rsp_t*>(body);
	
	CHECK_VAL_EQ(bodylen, sizeof(get_item_list_rsp_t) + rsp->item_cnt * sizeof(db_item_elem_t));

	int idx = sizeof(cli_proto_t);
	uint32_t max_pack_item = get_player_total_item_bag_grid_count(p);
	if( is_vip_player(p))
	{
		max_pack_item += get_player_extern_item_bag_grid_count(p);
	}

	pack(pkgbuf, max_pack_item, idx);
	pack(pkgbuf, rsp->item_cnt, idx);
	for ( uint32_t i = 0; i < rsp->item_cnt; i++) {
		db_item_elem_t* elem = &(rsp->item[i]);
		pack(pkgbuf, elem->item_id, idx);
		pack(pkgbuf, elem->count, idx);
		TRACE_LOG("uid=[%u],id=[%d],count=[%d]", p->id,elem->item_id,elem->count);
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}




/**
 * @brief callback for getting player pack
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_get_packs_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	get_packs_rsp_t* rsp = reinterpret_cast<get_packs_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(get_packs_rsp_t) + rsp->items_cnt * sizeof(db_item_elem_t) 
			+ rsp->clothes_cnt * sizeof(db_get_clothes_list_elem_t));



	if (rsp->items_cnt > get_player_total_item_bag_grid_count(p) || 
			rsp->clothes_cnt > get_player_total_item_bag_grid_count(p)) {
		WARN_LOG("pack too large:[%u %u %u]", p->id, rsp->items_cnt, rsp->clothes_cnt);
	}


	db_get_clothes_list_elem_t* cloths = reinterpret_cast<db_get_clothes_list_elem_t*>(rsp->body);
	db_item_elem_t* itms = reinterpret_cast<db_item_elem_t*>(rsp->body + rsp->clothes_cnt * sizeof(db_get_clothes_list_elem_t));

	TRACE_LOG("player packs:[%u %u %u]", p->id, rsp->items_cnt, rsp->clothes_cnt);
	for (uint32_t i = 0; i < rsp->clothes_cnt; i++) {
		db_get_clothes_list_elem_t* elem = &(cloths[i]);
		p->my_packs->add_clothes(p, elem->clothes_id, elem->unique_id, elem->duration, 0, elem->gettime, elem->timelag);
	}
	TRACE_LOG("pack clothes cnt[%u %u]",p->id, p->my_packs->all_clothes_cnt());
	
	for (uint32_t i = 0; i < rsp->items_cnt; i++) {
		db_item_elem_t* elem = &(itms[i]);
		p->my_packs->add_item(p, elem->item_id, elem->count, 0, false);
	}
	TRACE_LOG("pack items cnt[%u %u]",p->id, p->my_packs->all_items_cnt());
	
	if ( p->waitcmd == cli_proto_login ) {
		return send_request_to_db(p, p->id, p->role_tm, dbproto_query_restriction_list, 0, 0);
	}

	return 0;
}

/**
 * @brief callback for using a skill book
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_use_skill_book_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	use_skill_book_rsp_t* rsp = reinterpret_cast<use_skill_book_rsp_t*>(body);

	//del from player's packs
	if (p->my_packs->del_item(p, rsp->book_id, 1, channel_string_other) < 0) {
		WARN_LOG("del item error![%u %u]", p->id, p->my_packs->all_items_cnt());
	}

	p->skill_point = rsp->left_points;
	//stat log :use skill book
//	do_stat_log_amber(stat_log_used_amber,rsp->book_id, p->role_type);

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->book_id, idx);
	pack(pkgbuf, rsp->left_points, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	KDEBUG_LOG(p->id, "USE SKILL BOOK CB\t[uid=%u bookid=%u leftsp=%u]",p->id, rsp->book_id, rsp->left_points);
	return send_to_player(p, pkgbuf, idx, 1);
}


/**
 * @brief callback for using a item
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_use_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	use_item_rsp_t* rsp = reinterpret_cast<use_item_rsp_t*>(body);


	//add to player's pack
	if (p->my_packs->del_item(p, rsp->item_id, 1, channel_string_other) < 0) {
		WARN_LOG("del item error![%u %u]", p->id, p->my_packs->all_items_cnt());
	}
    
	if (p->waitcmd == cli_proto_double_exp_action_use_item) {
		return db_set_double_exp_data(p, g_all_restr[double_exp_daily_use_item - 1].rewards[0].count);
	}
	const GfItem* itm = items->get_item(rsp->item_id);
	if (itm->buf_ex_id1()) {
		TRACE_LOG("appearance buf\t[uid=%u item_id=%u]",p->id, rsp->item_id);
        if (p->app_mon == get_rich_monster_id) {
            player_gain_item(p, 2, get_rich_item_id, 2 * get_swap_action_times(p, 1277), channel_string_use_tongbao_item);
            clear_swap_action_times(p, 1277);
            p->reward_role_vec->clear();
        }

		do_item_buf(p, itm);

        if (rsp->item_id == get_rich_item_id)
            add_swap_action_times(p, 1275);
	}


	if ( (is_trade_bcast_item(rsp->item_id) || rsp->item_id == world_notice_item_id) 
        &&  p->waitcmd == cli_proto_player_mcast_message) {
		KDEBUG_LOG(p->id, "PLAYER USE TRADE MCAST [%u] " ,rsp->item_id);

		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, rsp->item_id, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		TRACE_LOG("uid=%u itmid=%u",p->id, rsp->item_id);
		send_to_player(p, pkgbuf, idx, 1);

        uint32_t front_id = 0;
        int ifx = 0;
        unpack_h(p->session, front_id,  ifx);
        //for trade players in this online
        idx = sizeof(cli_proto_t);
        TRACE_LOG("mcast info %s | %s ", p->nick, p->session + ifx);  

        player_gain_item(p, 2, gold_coin_item_id, get_feedback_coin_cnt(rsp->item_id), channel_string_use_tongbao_item, false);
		return 0;
	}

	p->sesslen = 0;
	pack_h(p->session, rsp->item_id, p->sesslen);

    player_gain_item(p, 2, gold_coin_item_id, get_feedback_coin_cnt(rsp->item_id), channel_string_use_tongbao_item, false);

	//log it's used times
	restriction_t* p_restr = 0;
	switch (rsp->item_id) {
		case double_player_only_exp_buff_item_id:
			p_restr  = &(g_all_restr[player_only_get_exp_buff_daily_id - 1]);
			db_add_action_count(0, p->id, p->role_tm, player_only_get_exp_buff_daily_id, p_restr->restr_flag, 
				p_restr->toplimit, p_restr->userflg);
			p->daily_restriction_count_list[player_only_get_exp_buff_daily_id - 1] ++;
			break;
		case double_mon_only_exp_buff_item_id:
			p_restr  = &(g_all_restr[mon_only_get_exp_buff_daily_id - 1]);
			db_add_action_count(0, p->id, p->role_tm, mon_only_get_exp_buff_daily_id, p_restr->restr_flag, 
				p_restr->toplimit, p_restr->userflg);
			p->daily_restriction_count_list[mon_only_get_exp_buff_daily_id - 1] ++;
			break;
		default:
			//return repair_all_attire(p);
			break;
	}

	//carry out it's effect
	switch (rsp->item_id) {
		case reset_skills_item_id:
		case reset_skills_item_id_2:
			return reset_all_skills(p, 0);//db_get_skills(p);
			break;
		case repair_clothes_item_id:
			return repair_all_attire(p);
			break;
		default:
			break;
	}

	proc_use_item_achieve_logic(p, rsp->item_id, 1, normal_use_item);

	if (itm->buf_type() && !(p->battle_grp)) {
		return db_add_buff(p, itm->buf_type(), itm->buf_duration(), itm->mutex_type());
	}

	//use item in a battle
	if ( p->battle_grp != 0) {
		return btlsvr_player_use_item(p, rsp->item_id);
	}
	
	send_use_item_rsp(p, rsp->item_id, 1);
	return 0;
}

/**
 * @brief callback for swap item
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_swap_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	/*int idx = 0;
	unpack_h(body, p->coins, idx);
	unpack_h(body, p->exp, idx);*/
	swap_item_rsp_t* rsp = reinterpret_cast<swap_item_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(swap_item_rsp_t) + rsp->clothes_cnt * sizeof(db_swap_clothes_elem_t));

	uint32_t cur_exp = p->exp;
	uint32_t cur_sp  = p->skill_point;
	uint32_t cur_honor = p->honor;
	uint32_t old_coins = p->coins;
	p->coins = rsp->coins;
	p->exp = rsp->exp;
	p->allocator_exp = rsp->allocator_exp;
	p->skill_point = rsp->skill_point;
	p->honor = rsp->honor;

	do_stat_log_coin_add_reduce(p, rsp->coins, channel_string_active);
	if (p->fumo_points_total < rsp->fumo_point) {
		uint32_t buf[2] = {rsp->fumo_point - p->fumo_points_total, 0};
		do_stat_item_log(stat_log_fumo_gain, p->lv, 0, buf, 2);
	}
	p->fumo_points_total = rsp->fumo_point;
	TRACE_LOG("swap attr[%u %u %u %u %u]",p->id, p->coins, p->exp, p->skill_point, p->fumo_points_total);

	//player lv update
	calc_player_level(p);

	if (cur_honor != p->honor) {
		//noti to map , honor up
		int idx = sizeof(cli_proto_t);
		idx += pack_player_honor_up_noti(pkgbuf + idx, static_cast<uint32_t>(p->honor));
		init_cli_proto_head_full(pkgbuf, p->id, 0, cli_proto_player_honor_up_noti, idx, 0);
		send_to_map(p, pkgbuf, idx, 0);
		TRACE_LOG("honor update :uid=%u %u %u %u",p->id, p->lv, cur_honor, p->honor);
	}

	//if any change
	if (p->waitcmd != cli_proto_restrict_action && p->waitcmd != cli_proto_restrict_action_ex) {
	//if dailyaction then do not nodify  client
		if (cur_exp < p->exp || cur_sp < p->skill_point) {
			//noti to the player
			int idx = sizeof(cli_proto_t);
			idx += pack_player_base_attr(pkgbuf + idx, p, old_coins);
			init_cli_proto_head(pkgbuf, p, cli_proto_player_attr_chg, idx);
			send_to_player(p, pkgbuf, idx, 0);
			
			TRACE_LOG("exp or sp update :uid=%u %u %u %u %u",p->id, p->lv, cur_exp, p->exp, p->skill_point);
		}
	}

	if (p->waitcmd == cli_proto_restrict_action || p->waitcmd == cli_proto_recv_restrict_reward ||
		p->waitcmd == cli_proto_restrict_action_ex) {

		return db_swap_res_item_callback(p, rsp, bodylen);
	} else if (p->waitcmd == cli_proto_get_amb_reward) {
		return db_amb_swap_item_callback(p, rsp, bodylen);
	} 

	//any clothes
	fin_tsk_session_t* fin_sess = reinterpret_cast<fin_tsk_session_t *>(p->session);
	if (fin_sess->out_clothes_num != rsp->clothes_cnt) {
		WARN_LOG("tskid:[%u] clothes cnt error:[%u %u]", fin_sess->tsk_id, fin_sess->out_clothes_num, rsp->clothes_cnt);
		//return -1;
	}
	
	for ( uint32_t i = 0; i < rsp->clothes_cnt; i++) {
		fin_sess->clothes[i].clothes_id = rsp->clothes[i].clothes_id;
		fin_sess->clothes[i].unique_id = rsp->clothes[i].unique_id;
		TRACE_LOG("clothes[%u %u]",fin_sess->clothes[i].clothes_id, fin_sess->clothes[i].unique_id);
	}
	TRACE_LOG("uid=%u coins=%u exp=%u skill_pt=%u clothes_cnt=%u",p->id, p->coins, p->exp, p->skill_point, rsp->clothes_cnt);

	switch (p->waitcmd) {
		case cli_proto_finish_task:
			tmp_log_for_task_bug(p, 1);
			return finish_task_step_over(p, finish_task_added_items);
		default:
			ERROR_LOG("unsupported cmd:%u", p->waitcmd);
			return -1;
	}
	
	return 0;
}


/**
 * @brief callback for set_itembind
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_set_itembind_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	set_itembind_rsp_t* rsp = reinterpret_cast<set_itembind_rsp_t*>(body);
	int idx = sizeof(cli_proto_t);
	
	pack(pkgbuf, rsp->buf, sizeof(set_itembind_rsp_t), idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	TRACE_LOG("set itembind callback,%s", (char*)&(rsp->buf));
	return send_to_player(p, pkgbuf, idx, 1); 
}

/**
  * @brief callback for discarding items
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_discard_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	db_item_elem_t* rsp = reinterpret_cast<db_item_elem_t*>(body);
	
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->item_id, idx);
	pack(pkgbuf, rsp->count, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	TRACE_LOG("discard item[%u %u %u]", p->id, rsp->item_id, rsp->count);

	//add to player's pack
	if (p->my_packs->del_item(p, rsp->item_id, rsp->count, channel_string_discard) < 0) {
		WARN_LOG("del item error![%u %u]", p->id, p->my_packs->all_items_cnt());
	}

    report_add_to_monitor(p, 41, rsp->item_id, rsp->count, 0);
	send_to_player(p, pkgbuf, idx, 1); 

    player_gain_item(p, 2, gold_coin_item_id, get_feedback_coin_cnt(rsp->item_id) * rsp->count, channel_string_use_tongbao_item, false);
    return 0;
}

/**
  * @brief callback for discarding clothes
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_discard_clothes_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	db_clothes_elem_t* rsp = reinterpret_cast<db_clothes_elem_t*>(body);
	
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->clothes_id, idx);
	pack(pkgbuf, rsp->unique_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	TRACE_LOG("discard clothes[%u %u %u]", p->id, rsp->clothes_id, rsp->unique_id);

	//add to player's pack
	if (p->my_packs->del_clothes(rsp->unique_id, channel_string_discard) < 0) {
		WARN_LOG("del clothes error![%u %u]", p->id, p->my_packs->all_clothes_cnt());
	}
    //report_add_to_monitor(p, 41, rsp->clothes_id, 1, 0);
	send_to_player(p, pkgbuf, idx, 1); 

    player_gain_item(p, 2, gold_coin_item_id, get_feedback_coin_cnt(rsp->clothes_id), channel_string_use_tongbao_item, false);
    return 0;
}

/**
  * @brief callback fordecompose attire
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_decompose_attire_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int idx = sizeof(cli_proto_t);
	
	decompose_attire_rsp_t* rsp = reinterpret_cast<decompose_attire_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(decompose_attire_rsp_t) + rsp->cnt * sizeof(decompose_attire_item_t));
	
	pack(pkgbuf, rsp->unique_id, idx);
	pack(pkgbuf, rsp->cnt, idx); // mess of item type

	//add to player's pack
	if (p->my_packs->del_clothes(rsp->unique_id, channel_string_decompose) < 0) {
		WARN_LOG("del clothes error![%u %u]", p->id, p->my_packs->all_clothes_cnt());
	}



	


	for (uint32_t i = 0; i < rsp->cnt; ++i) {
		pack(pkgbuf, rsp->item[i].item_id, idx);
		pack(pkgbuf, rsp->item[i].cnt, idx);
		TRACE_LOG("got item[%u %u %u]", p->id, rsp->item[i].item_id, rsp->item[i].cnt);
		
		//add to player's pack
		if (p->my_packs->add_item(p, rsp->item[i].item_id, rsp->item[i].cnt, channel_string_decompose) < 0) {
			WARN_LOG("add item error![%u %u]", p->id, p->my_packs->all_items_cnt());
		}
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	KDEBUG_LOG(p->id, "DECOMPOSE ATTR OVER\t[%u %u %u]", p->id, rsp->unique_id, rsp->cnt);
	return send_to_player(p, pkgbuf, idx, 1); 	
}

/**
  * @brief callback for compose attire
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_compose_attire_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int idx = sizeof(cli_proto_t);
	
	compose_attire_rsp_t* rsp = reinterpret_cast<compose_attire_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(compose_attire_rsp_t) + rsp->cnt * sizeof(compose_attire_item_t));

	do_stat_log_coin_add_reduce(p, rsp->left_coins, channel_string_compose);
	p->coins = rsp->left_coins;
	
	uint8_t type = !(rsp->is_clothes);
	pack(pkgbuf, type, idx);
	pack(pkgbuf, rsp->method_id, idx);
	if (rsp->is_clothes) {
		p->my_packs->add_clothes(p, rsp->attire_id, rsp->unique_id, 0, channel_string_compose);
		pack(pkgbuf, rsp->attire_id, idx);
		pack(pkgbuf, rsp->unique_id, idx);
	} else {
		p->my_packs->add_item(p, rsp->attire_id, rsp->duration, channel_string_compose);
		pack(pkgbuf, rsp->attire_id, idx);
		pack(pkgbuf, rsp->duration, idx);
	}

	if (rsp->del_item) {
		p->my_packs->del_item(p, rsp->del_item, 1, channel_string_compose);
	}
	if (rsp->random_item) {
		p->my_packs->add_item(p, rsp->random_item, 1, channel_string_compose);
	}
	pack(pkgbuf, rsp->del_item, idx);
	pack(pkgbuf, rsp->random_item, idx);

	//update player's packs
	int del_clothes_idx = idx;
	idx += 4;
	
	uint32_t del_clothes_cnt = 0;
	for (uint32_t i = 0; i < rsp->cnt; ++i) {
		if (rsp->item[i].is_clothes) {
			p->my_packs->del_clothes(rsp->item[i].item_id, channel_string_compose);
			pack(pkgbuf, rsp->item[i].item_id, idx);
			del_clothes_cnt ++;
		} else {
			p->my_packs->del_item(p, rsp->item[i].item_id, rsp->item[i].cnt, channel_string_compose);
		}
		TRACE_LOG("%u %u", rsp->item[i].item_id, rsp->item[i].cnt);
	}
	pack(pkgbuf, del_clothes_cnt, del_clothes_idx);
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	KDEBUG_LOG(p->id, "COMPOSE ATTR\t[%u %u %u %u %u %u]", p->id, rsp->method_id, rsp->attire_id, rsp->unique_id, rsp->duration, p->coins);
	send_to_player(p, pkgbuf, idx, 1); 
	
	method_t* p_method = get_compose_method(rsp->method_id);
	if(p_method)
	{
		//增加铁匠经验
		uint32_t fact_add_exp = 0;
		//寒假经验翻倍
		uint32_t exp_per = 2;
		//同步内存 
		add_player_secondary_professoin_exp(p, iron_man_profession_type, exp_per*p_method->add_pro_exp, &fact_add_exp);
		//同步DB
		secondary_profession_data* p_data = get_player_secondary_profession_by_type(p, iron_man_profession_type);
		if(p_data != NULL)
		{
			db_set_player_second_pro_exp(p, iron_man_profession_type, p_data->get_profession_exp());
		}
		//同步客户端
		notify_player_second_pro_exp_lv_change(p, iron_man_profession_type);	
		//扣除活力点,同步内存，同步DB，通知客户端都在这个函数
		dec_player_vitality_point(p, p_method->need_vitality_point);
	}
	//如果是紫色装备则广播
	if(p_method->broadcast)
	{
		send_compose_to_world(p, rsp->attire_id);
	}
	return 0;
}

/**
  * @brief callback for strengthen attire
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_strengthen_attire_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	int idx = sizeof(cli_proto_t);
	
	db_strengthen_attire_rsp_t* rsp = reinterpret_cast<db_strengthen_attire_rsp_t*>(body);

	p->coins = rsp->leftcoin;

    uint32_t gold_coin_count = 0;
	//update player's packs
    p->my_packs->del_item(p, rsp->material_id, rsp->material_cnt, channel_string_strengthen);

    gold_coin_count += (get_feedback_coin_cnt(rsp->material_id) * rsp->material_cnt);

    uint32_t stat_id = rsp->material_id - strengthen_material_base_1;
    if (stat_id < 10000) {
        do_stat_log_universal_interface_1(stat_log_material_consume_1, stat_id + 0x1000 * rsp->attirelv, rsp->material_cnt);
    } else {
        stat_id = rsp->material_id - strengthen_material_base_2;
        do_stat_log_universal_interface_1(stat_log_material_consume_2, stat_id + 0x1000 * rsp->attirelv, rsp->material_cnt);
        do_stat_log_consume_material(rsp->material_id, rsp->material_cnt);
    }
    if (rsp->stoneid_1 != 0) {
        p->my_packs->del_item(p, rsp->stoneid_1, rsp->stone_1_cnt, channel_string_strengthen);
        stat_id = rsp->stoneid_1 - strengthen_material_base_2;
        do_stat_log_universal_interface_1(stat_log_material_consume_2, stat_id + 0x1000 * rsp->attirelv, rsp->stone_1_cnt);

        gold_coin_count += (get_feedback_coin_cnt(rsp->stoneid_1) * rsp->stone_1_cnt);
    }
    if (rsp->stoneid_2 != 0) {
        p->my_packs->del_item(p, rsp->stoneid_2, 1, channel_string_strengthen);
        stat_id = rsp->stoneid_2 - strengthen_material_base_2;
        do_stat_log_universal_interface_1(stat_log_material_consume_2, stat_id + 0x1000 * rsp->attirelv, 1);
        do_stat_log_consume_material(rsp->stoneid_2, 1);

        gold_coin_count += (get_feedback_coin_cnt(rsp->stoneid_2));
    }

	p->my_packs->update_clothes_lv_by_id(rsp->uniquekey, rsp->attirelv);

	pack(pkgbuf, rsp->leftcoin, idx);
	pack(pkgbuf, rsp->attireid, idx);
	pack(pkgbuf, rsp->uniquekey, idx);
	pack(pkgbuf, rsp->attirelv, idx);
	pack(pkgbuf, rsp->material_id, idx);
	pack(pkgbuf, rsp->material_cnt, idx);
	pack(pkgbuf, rsp->stoneid_1, idx);
	pack(pkgbuf, rsp->stone_1_cnt, idx);
	pack(pkgbuf, rsp->stoneid_2, idx);
	
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	KDEBUG_LOG(p->id, "STRENGTHEN ATTR\t uid=[%u] [%u %u %u %u]",p->id,rsp->leftcoin,rsp->attireid,rsp->uniquekey,rsp->attirelv);
	KDEBUG_LOG(p->id, "STRENGTHEN ATTR DEL MATERIAL\t uid=[%u] [%u %u %u %u %u]",p->id,rsp->material_id,rsp->material_cnt,rsp->stoneid_1,rsp->stone_1_cnt,rsp->stoneid_2);
	send_to_player(p, pkgbuf, idx, 1); 
       player_gain_item(p, 2, gold_coin_item_id, gold_coin_count, channel_string_use_tongbao_item, false);

	proc_gain_clothes_achieve_logic(p, rsp->attireid, rsp->attirelv, strengthen_update_attire);
   	return 0;
}

//added by cws 0620
int db_strengthen_attire_without_material_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	//int idx = sizeof(cli_proto_t);
	db_strengthen_without_material_rsp_t* rsp = reinterpret_cast<db_strengthen_without_material_rsp_t*>(body);
	p->my_packs->update_clothes_lv_by_id(rsp->uniquekey, rsp->attirelv);
	//pack(pkgbuf, rsp->uniquekey, idx);	
	//pack(pkgbuf, rsp->attirelv, idx);
	//init_cli_proto_head(pkgbuf, p, cli_proto_strengthen_witout_material, idx);	
	//send_to_player(p, pkgbuf, idx, 1); 	
	TRACE_LOG("cws strengthen_attire_without_material success!");
  	return 0;
}



/**
  * @brief get player's clothes duration
  */
int btlsvr_player_clothes_duration_callback(player_t* p, btl_proto_t* pkg)
{
	clothes_duration_rsp_t* rsp = reinterpret_cast<clothes_duration_rsp_t*>(pkg->body);
	CHECK_VAL_EQ(pkg->len - sizeof(btl_proto_t), 4 + rsp->clothes_cnt * sizeof(clothes_duration_elem_t));

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->clothes_cnt, idx);
	TRACE_LOG("clothes cnt[%u %u]",p->id, rsp->clothes_cnt);
	clothes_duration_elem_t* cloth = reinterpret_cast<clothes_duration_elem_t*>(rsp->body);
	for (uint32_t i = 0; i < rsp->clothes_cnt; i++) {
		pack(pkgbuf, cloth->unique_id, idx);
		pack(pkgbuf, static_cast<uint32_t>(cloth->duration), idx);
		TRACE_LOG("clothes duration[%u %u %u]",p->id, cloth->unique_id, cloth->duration);
		cloth++;
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

/**
  * @brief upgrade item (items_upgrade.xml)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int upgrade_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t upgrade_id = 0;
	uint32_t unique_id = 0;

	unpack(body, upgrade_id, idx);
	unpack(body, unique_id, idx);

//	upgrade_id = 1001;
//	unique_id = 3346 ;
	items_upgrade_data xml_data_tmp = *(g_items_upgrade_mrg.get_upgrade_data(upgrade_id));
	items_upgrade_data* p_xml_data = &xml_data_tmp;
	
	if (!p_xml_data) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);	
	}

	if (p_xml_data->vip_lv && (!is_vip_player(p) || p->vip_level < p_xml_data->vip_lv) ) {
		return send_header_to_player(p, p->waitcmd, cli_err_less_vip_lv, 1);
	}
	for (uint32_t i = 0; i < p_xml_data->source_cnt; i++) {
		items_upgrade_elem_t* p_elem = &(p_xml_data->source_data[i]);
		if (p_elem->type == give_type_clothes && unique_id) {
			if (!(p->my_packs->is_clother_item_exist(unique_id, p_elem->id[0]))) {
				ERROR_LOG("source attire id error %u %u %u", p->id, unique_id, p_elem->id[0]);
				return -1;
			}
		} else {
			ERROR_LOG("source give type error %u %u %u", p->id, upgrade_id, p_elem->type);
			return -1;
		}
	}

	uint32_t material_cnt = p_xml_data->material_cnt;
	for (uint32_t i = 0; i < material_cnt; i++) {
		items_upgrade_elem_t* p_elem = &(p_xml_data->material_data[i]);
		if (p_elem->type == give_type_normal_item) {

			uint32_t item1_cnt = p->my_packs->get_item_cnt(p_elem->id[0]);
			uint32_t item2_cnt = p->my_packs->get_item_cnt(p_elem->id[1]);
			uint32_t have_cnt = item1_cnt + item2_cnt;
			if (have_cnt < p_elem->count) {
				ERROR_LOG("material item id error %u %u %u", p->id, unique_id, p_elem->id[0]);
				return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
			}

			if (p_elem->id[0] && p_elem->id[1]) {
			//have two items
				uint32_t item2_real_cnt = 0;
				uint32_t item1_real_cnt = 0;
				if (item2_cnt < p_elem->count) {
					item2_real_cnt = item2_cnt;
				} else {
					item2_real_cnt = p_elem->count;
				}
				item1_real_cnt = p_elem->count - item2_real_cnt;
				if (item1_real_cnt) {
					if (item2_real_cnt) {
						p_xml_data->material_data[p_xml_data->material_cnt].type = p_elem->type;
						p_xml_data->material_data[p_xml_data->material_cnt].id[0] = p_elem->id[1];
						p_xml_data->material_data[p_xml_data->material_cnt].count = item2_real_cnt;
						p_xml_data->material_cnt++;
					}
					p_elem->id[1] = 0;
					p_elem->count = item1_real_cnt;
					
				} else {
					p_elem->id[0] = p_elem->id[1];
					p_elem->count = item2_real_cnt;
					p_elem->id[1] = 0;
				}
			}
			
		} else if (p_elem->type == give_type_player_attr) {
			if (p_elem->id[0] == give_type_xiaomee) {
				if (p->coins < p_elem->count) {
					return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
				}
			} else {
				ERROR_LOG("source give type error %u %u %u %u", p->id, upgrade_id, p_elem->type, p_elem->id[0]);
				return -1;
			}
		} else {
			ERROR_LOG("source give type error %u %u %u", p->id, upgrade_id, p_elem->type);
			return -1;
		}
	}

	for (uint32_t i = 0; i < p_xml_data->dest_cnt; i++) {
		items_upgrade_elem_t* p_elem = &(p_xml_data->dest_data[i]);
		const GfItem* clothes = items->get_item(p_elem->id[0]);
		if (!clothes) {
			return send_header_to_player(p, p->waitcmd, cli_err_no_clothes, 1);	
		}
	}
	
	return db_upgrade_item(p, p_xml_data, unique_id);
}

int db_upgrade_item(player_t *p, items_upgrade_data* p_xml_data, uint32_t unique_id)
{
	uint32_t upgrade_succ = 0;
	uint32_t rd_num = random() % 100;
	if (rd_num < p_xml_data->odds) {
		upgrade_succ = 1;
	}
	int db_idx = 0;
	pack_h(dbpkgbuf, upgrade_succ, db_idx);

	int del_idx = db_idx;
	int del_cnt = 0;
	pack_h(dbpkgbuf, 0, db_idx);

	int add_idx = db_idx;
	int add_cnt = 0;
	pack_h(dbpkgbuf, 0, db_idx);

	
	if (upgrade_succ) {
		for (uint32_t i = 0; i < p_xml_data->source_cnt; i++) {
			items_upgrade_elem_t* p_elem = &(p_xml_data->source_data[i]);
			pack_h(dbpkgbuf, p_elem->type, db_idx);
			pack_h(dbpkgbuf, p_elem->id[0], db_idx);
			if (p_elem->type == give_type_clothes) {
				pack_h(dbpkgbuf, unique_id, db_idx);
			}  else {
				pack_h(dbpkgbuf, p_elem->count, db_idx);
			}
			pack_h(dbpkgbuf, 0, db_idx);
			del_cnt ++;
		}
	}

	for (uint32_t i = 0; i < p_xml_data->material_cnt; i++) {
		items_upgrade_elem_t* p_elem = &(p_xml_data->material_data[i]);
		pack_h(dbpkgbuf, p_elem->type, db_idx);
		pack_h(dbpkgbuf, p_elem->id[0], db_idx);
		pack_h(dbpkgbuf, p_elem->count, db_idx);
		pack_h(dbpkgbuf, 0, db_idx);
		del_cnt ++;
	}

	if (upgrade_succ) {
		for (uint32_t i = 0; i < p_xml_data->dest_cnt; i++) {
			items_upgrade_elem_t* p_elem = &(p_xml_data->dest_data[i]);
			pack_h(dbpkgbuf, p_elem->type, db_idx);
			pack_h(dbpkgbuf, p_elem->id[0], db_idx);
			pack_h(dbpkgbuf, p_elem->count, db_idx);
			if (p_elem->type == give_type_clothes) {
				const GfItem* clothes = items->get_item(p_elem->id[0]);
				uint32_t duration = clothes->duration * clothes_duration_ratio;
				pack_h(dbpkgbuf, duration, db_idx);
			} else {
				pack_h(dbpkgbuf, 0, db_idx);
			}
			add_cnt ++;
		}
	}
	pack_h(dbpkgbuf, del_cnt, del_idx);
	pack_h(dbpkgbuf, add_cnt, add_idx);
	
	return send_request_to_db(p, p->id, p->role_tm, dbproto_upgrade_item, dbpkgbuf, db_idx);
}


int db_upgrade_item_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	const upgrade_item_head_rsp_t* rsp = reinterpret_cast<upgrade_item_head_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(upgrade_item_head_rsp_t) + 
							rsp->add_cnt * sizeof(upgrade_item_elem_rsp_t) +
							rsp->del_cnt * sizeof(upgrade_item_elem_rsp_t));

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->upgrade_succ, idx);
	pack(pkgbuf, rsp->del_cnt, idx);
	pack(pkgbuf, rsp->add_cnt, idx);

	KDEBUG_LOG(p->id, "UPGRADE ITEM\t[%u %u %u]", rsp->upgrade_succ, rsp->del_cnt, rsp->add_cnt);
	upgrade_item_elem_rsp_t* p_elem = reinterpret_cast<upgrade_item_elem_rsp_t*>(reinterpret_cast<upgrade_item_head_rsp_t*>(body) + 1);
	for (uint32_t i = 0; i < rsp->del_cnt; i++) {
		if (p_elem->type == give_type_clothes) {
			p->my_packs->del_clothes(p_elem->count, channel_string_upgrade_item);
		} else if (p_elem->type == give_type_normal_item) {
			p->my_packs->del_item(p, p_elem->id, p_elem->count, channel_string_upgrade_item);
		} else if (p_elem->type == give_type_player_attr) {
			if (p_elem->id == give_type_xiaomee) {
				p->coins -= p_elem->count;
				do_stat_log_coin_add_reduce(p, (uint32_t)0, p_elem->count, channel_string_upgrade_item);
			}
		}
		pack(pkgbuf, p_elem->type, idx);
		pack(pkgbuf, p_elem->id, idx);
		pack(pkgbuf, p_elem->count, idx);
		p_elem++;
	}

	for (uint32_t i = 0; i < rsp->add_cnt; i++) {
		if (p_elem->type == give_type_clothes) {
			p->my_packs->add_clothes(p, p_elem->id, p_elem->count, p_elem->duration, channel_string_upgrade_item);
		} else if (p_elem->type == give_type_normal_item) {
			p->my_packs->add_item(p, p_elem->id, p_elem->count, channel_string_upgrade_item);
		} else if (p_elem->type == give_type_player_attr) {
			if (p_elem->id == give_type_xiaomee) {
				p->coins += p_elem->count;
			}
		}

		pack(pkgbuf, p_elem->type, idx);
		pack(pkgbuf, p_elem->id, idx);
		pack(pkgbuf, p_elem->count, idx);
		p_elem++;
	}
	init_cli_proto_head_full(pkgbuf, p->id, p->seqno, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}


void proc_extra_swap_xiaomee_logic(player_t * p , uint32_t resid,  swap_action_elem_t *swap_rsp)
{
	if (resid == 1420) {
		uint32_t today_cnt = get_swap_action_times(p, resid);
		uint32_t xiaomee_cost = p->lv * ( 4 + today_cnt + 1) * 10; 
		if (xiaomee_cost < 1000) {
			xiaomee_cost = 1000;
		}
		swap_rsp->count = xiaomee_cost;
	}
}

bool pack_swap_action_item(player_t* p, uint32_t resid, uint32_t role_type, swap_action_data* p_out_res, swap_action_data* p_out_mail)
{
	swap_action_data res_total;
	swap_action_data* p_total = &res_total;
	swap_action_data* p_res = g_swap_action_mrg.get_swap_action(resid);
	
	memcpy(p_out_res, p_res, sizeof(*p_out_res));
	memcpy(p_out_mail, p_res, sizeof(*p_out_mail));
	memcpy(p_total, p_res, sizeof(*p_total));
	TRACE_LOG("%u %u", p_out_res->id, p_out_res->user_flg);
	
	p_out_res->rew_count = 0;
	p_total->rew_count = 0;
	p_out_mail->rew_count = 0;
	
	uint32_t rand_num = rand() % 1000;
	uint32_t odds = 0;
	uint32_t j = 0;
	uint32_t reward_item_cnt = 0;
	uint32_t reward_attire_cnt = 0;

    if (p_res->cost_odds != 0) {
        p_total->cost_count = 1;//p_res->cost_odds;
        get_odds_cost_elem( p_res, p_total->costs, rand_num );
    }

	for (uint32_t i = 0; i < p_total->cost_count; i++) {
		swap_action_elem_t* swap_item_rsp = &(p_total->costs[i]);
		if (swap_item_rsp->give_type == give_type_normal_item) {
			if (swap_item_rsp->count > p->my_packs->get_item_cnt(swap_item_rsp->give_id)) {
				send_header_to_player(p, p->waitcmd, 103122, 1);
			 	return false;
			}
		} else if (swap_item_rsp->give_type == give_type_clothes) {
		//attires
		
		} else if (swap_item_rsp->give_type == give_type_player_attr) {
		//attr
			switch (swap_item_rsp->give_id) {
				case give_type_exp:
					break;
				case give_type_xiaomee:
					if (p->coins < swap_item_rsp->count) {
						send_header_to_player(p, p->waitcmd, cli_err_not_enough_money, 1);
			 			return false;
					}
					proc_extra_swap_xiaomee_logic(p, resid,  swap_item_rsp);
					break;
			    case give_type_skillpoint:
					if (p->skill_point < swap_item_rsp->count) {
						send_header_to_player(p, p->waitcmd, cli_err_no_enough_point, 1);
			 			return false;
					}
					break;
			    case give_type_fumo_point:
					if (p->fumo_points_total < swap_item_rsp->count) {
						send_header_to_player(p, p->waitcmd, cli_err_no_enough_fumo, 1);
			 			return false;
					}
					break;
				case give_type_expliot:
					if (p->exploit < swap_item_rsp->count) {
						send_header_to_player(p, p->waitcmd, cli_err_no_enough_exploit, 1);
			 			return false;
					}
					break;
				case give_type_double_tm:
					break;
				case give_type_honor:
					if (p->honor < swap_item_rsp->count) {
						send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
			 			return false;
					}
					break;
				case give_type_apothecary_exp:
					break;
				case give_type_summon_exp:
					break;
                case give_type_sword_value:
                    break;
				default:
					break;
			}
		}	
	}

    //bool double_flag = true;
    for (uint32_t i = 0; i < p_res->rew_count; i++) {
        TRACE_LOG("role_type odds : [%u %u]",p_res->rewards[i].role_type, p_res->rewards[i].odds);
        // role_type limit but wrong role_type
        if (((p_res->rewards[i].role_type != 0) && (p_res->rewards[i].role_type != role_type)) ||
				(p->lv >= max_exp_lv && p_res->rewards[i].give_type == give_type_player_attr && p_res->rewards[i].give_id == give_type_exp)) {
			 if (p->lv >= max_exp_lv && p_res->rewards[i].give_type == give_type_player_attr && p_res->rewards[i].give_id == give_type_exp) {
			 	send_header_to_player(p, p->waitcmd, cli_err_max_exp_lv, 1);
			 	return false;
			 }
			 continue;
        } else {

			if (p_res->rewards[i].give_type == give_type_player_attr) {
	            if (p_res->rewards[i].give_id == give_type_xiaomee && p->coins > 4000000000) {
					send_header_to_player(p, p->waitcmd, cli_err_coins_too_much, 1);
					return false;
	            }
				if (p_res->rewards[i].give_id == give_type_summon_exp && p->allocator_exp > 4000000000) {
					send_header_to_player(p, p->waitcmd, cli_err_max_exp_lv, 1);
					return false;
				}
	        }
            //odds += p_res->rewards[i].odds;
            if (p_res->rewards[i].odds != 0) {
                odds += p_res->rewards[i].odds;
                TRACE_LOG("rand item1 :[%u %u %u]", rand_num, odds, p_res->rewards[i].odds);
                if (rand_num < odds) {
					uint32_t idx = i;
					if (resid == 1412) {
					//hard code : zhua xiao gui huodong
						if (idx <= 2) {
							if (get_swap_action_times(p, 1413 + idx)) {
								continue;
							} else {
								add_swap_action_times(p, 1413 + idx);
							}
						}
					}
					if (p_res->rewards[idx].give_type == give_type_normal_item) {
						reward_item_cnt ++;
					} else if (p_res->rewards[idx].give_type == give_type_clothes) {
						reward_attire_cnt ++;
					}
                    p_total->rew_count++;

					p_total->rewards[j].give_id = p_res->rewards[idx].give_id;
                    p_total->rewards[j].role_type = p_res->rewards[idx].role_type;
                    p_total->rewards[j].give_type = p_res->rewards[idx].give_type;
                    p_total->rewards[j].count = p_res->rewards[idx].count;
                    memcpy(p_total->rewards[j].name, p_res->rewards[idx].name, sizeof(p_res->rewards[idx].name));
                    p_total->rewards[j].is_bcast = p_res->rewards[idx].is_bcast;

/*
                    const GfItem* itm = items->get_item(p_res->rewards[idx].give_id);
					if (itm && is_valid_montype(itm->summon_id()) ) {
						if (check_summon_type_exist(p, itm->summon_id()) || 
							p->my_packs->is_item_exist(p_res->rewards[idx].give_id)) {
							p_total->rewards[j].give_id = 1540005;
                            p_total->rewards[j].is_bcast = 0;
						}
					}
					*/
                    if ( is_all_server_limit_item(resid, p_res->rewards[idx].give_id) ) {
                        uint32_t server_limit_id = 1467;
                        uint32_t insteat_item = 1549001;
                        if (p_res->rewards[idx].give_id == 1410017) {
                            server_limit_id = 1469;
                            insteat_item = 1410009;
                        } else if (p_res->rewards[idx].give_id == 1740024) {
                            server_limit_id = 1468;
                            insteat_item = 2300006;
                        }
                        global_limit_data_t* p_limit_data = g_limit_data_mrg.get_limit_data(server_limit_id);
                        if (p_limit_data && p_limit_data->left_cnt > 0) {
                            g_limit_data_mrg.reduce_item_cnt(server_limit_id, 1, true);
                        } else {
                            p_total->rewards[j].give_id = insteat_item; 
                        }
                    }

                    if (p_res->rewards[idx].give_type == give_type_player_attr 
                        && p_res->rewards[idx].give_id == give_type_sword_value && !is_vip_player(p)) {
                        p_total->rewards[j].give_type = give_type_normal_item;
                        p_total->rewards[j].give_id = 1302001;
                        p_total->rewards[j].count = 1;
                        reward_item_cnt++;
                    }

                    TRACE_LOG("rand item2 :[%u %u %u]",p_total->rewards[j].give_id, 
                        p_total->rewards[j].give_type, p_total->rewards[j].count);
                    //ERROR_LOG("Pack: [%u][%s]", p_total->rewards[j].give_id, p_total->rewards[j].name);
                    j++;
                    /*// 
                    if (double_flag && is_vip_player(p) && is_double_random(resid)) {
                        double_flag = false;
                        odds = 0;
                        rand_num = rand() % 1000;
                        i = 0;
                        continue;
                    }*/
                    break;
                } else {
                    continue;
                }
            } else {
				if (p_res->rewards[i].give_type == give_type_normal_item) {
					reward_item_cnt ++;
				} else if (p_res->rewards[i].give_type == give_type_clothes) {
					reward_attire_cnt ++;
				}

                p_total->rew_count++;
                p_total->rewards[j].role_type = p_res->rewards[i].role_type;
				p_total->rewards[j].give_id = p_res->rewards[i].give_id;
                p_total->rewards[j].give_type = p_res->rewards[i].give_type;
                p_total->rewards[j].count = p_res->rewards[i].count;
                memcpy(p_total->rewards[j].name, p_res->rewards[i].name, sizeof(p_res->rewards[i].name));
			    p_total->rewards[j].is_bcast = p_res->rewards[i].is_bcast;	

/*
                if (p_res->rewards[i].give_type == give_type_normal_item) {
					const GfItem* itm = items->get_item(p_res->rewards[i].give_id);
					if (itm && is_valid_montype(itm->summon_id()) ) {
						if (check_summon_type_exist(p, itm->summon_id()) ||
							p->my_packs->is_item_exist(p_res->rewards[i].give_id)) {
							p_total->rewards[j].give_id = 1540005;
                            p_total->rewards[j].is_bcast = 0;
						}
					}
					
				}
*/

                if (p_res->rewards[i].give_type == give_type_player_attr 
                    && p_res->rewards[i].give_id == give_type_sword_value && !is_vip_player(p)) {
                    p_total->rewards[j].give_type = give_type_normal_item;
                    p_total->rewards[j].give_id = 1302001;
                    p_total->rewards[j].count = 1;
                    reward_item_cnt++;
                }

                j++;
            }
        }
    }

	uint32_t all_item_cnt = get_player_total_item_bag_grid_count(p);
	uint32_t cur_item_cnt = p->my_packs->all_items_cnt();
	uint32_t free_item_bag = all_item_cnt > cur_item_cnt ? all_item_cnt - cur_item_cnt : 0;
	uint32_t all_attire_cnt = get_player_total_item_bag_grid_count(p);
	uint32_t cur_attire_cnt = p->my_packs->all_clothes_cnt();
	uint32_t free_attire_bag = all_attire_cnt > cur_attire_cnt ? all_attire_cnt - cur_attire_cnt : 0;

	TRACE_LOG("%u %u %u %u %u", free_item_bag, free_attire_bag, all_attire_cnt, reward_item_cnt, reward_attire_cnt);
	if ( free_item_bag < reward_item_cnt || free_attire_bag < reward_attire_cnt ) {
		uint32_t mail_flag = 1;

		if (mail_flag) {
			for (uint32_t i = 0; i < p_total->rew_count; i++) {
				if (((p_total->rewards[i].give_type != give_type_normal_item && 
					p_total->rewards[i].give_type != give_type_clothes)) ||
					(p_total->rewards[i].give_id >= 1700009 && p_total->rewards[i].give_id <= 1700018)
                    || is_strengthen_material(p_total->rewards[i].give_id) ) {
					
					p_out_res->rewards[p_out_res->rew_count].role_type = p_total->rewards[i].role_type;
					p_out_res->rewards[p_out_res->rew_count].give_id = p_total->rewards[i].give_id;
					p_out_res->rewards[p_out_res->rew_count].give_type = p_total->rewards[i].give_type;
					p_out_res->rewards[p_out_res->rew_count].count = p_total->rewards[i].count;
					
					TRACE_LOG("RESOUT : %u %u", p_out_res->rewards[p_out_res->rew_count].give_id,
						p_out_res->rewards[p_out_res->rew_count].count);
					p_out_res->rew_count++;
				} else {		
					p_out_mail->rewards[p_out_mail->rew_count].role_type = p_total->rewards[i].role_type;
					p_out_mail->rewards[p_out_mail->rew_count].give_id = p_total->rewards[i].give_id;
					p_out_mail->rewards[p_out_mail->rew_count].give_type = p_total->rewards[i].give_type;
					p_out_mail->rewards[p_out_mail->rew_count].count = p_total->rewards[i].count;
					
					TRACE_LOG("MAIL : %u %u", p_out_mail->rewards[p_out_mail->rew_count].give_id,
						p_out_mail->rewards[p_out_mail->rew_count].count);
					p_out_mail->rew_count++;
				}
			}
		} else {
			memcpy(p_out_res, p_total, sizeof(*p_out_res));
		}

	} else {
		memcpy(p_out_res, p_total, sizeof(*p_out_res));
	}
	return true;
}

uint32_t get_active_start_time(uint32_t swap_id)
{
    uint32_t ret = 0;
    switch (swap_id) {
        case 1032: 
            ret = get_utc_second(2011, 7, 10, 0, 0, 0);
            break;
        default:
            break;
    }
    return ret;
}

int db_swap_action(player_t* p, const swap_action_data* p_restr, const swap_action_data* p_res_mail, bool ret_flg, uint32_t add_times)
{
	char tmp_str[8024] = "";
	swap_action_data* p_restr_sess = 0;
	//pack type in player's session
	if (ret_flg) {
		p->sesslen = 0;
		pack(p->session, p_restr, sizeof(*p_restr) , p->sesslen);
		pack(p->session, p_res_mail, sizeof(*p_res_mail) , p->sesslen);
		p_restr_sess = (swap_action_data*)(p->session);
	} else {
        int tmp_idx = 0;
		pack(tmp_str, p_restr, sizeof(*p_restr) , tmp_idx);
		pack(tmp_str, p_res_mail, sizeof(*p_res_mail) , tmp_idx);
		p_restr_sess = (swap_action_data*)(tmp_str);
	}
	

	TRACE_LOG("%u %u", p->id, p_restr_sess->user_flg);
	
	uint32_t cost_cnt = p_restr_sess->cost_count;
	uint32_t rew_cnt = p_restr_sess->rew_count;
	
	int idx = 0;
	pack_h(dbpkgbuf, p_restr_sess->id, idx);
	pack_h(dbpkgbuf, p_restr_sess->type, idx);
	pack_h(dbpkgbuf, add_times, idx);
	pack_h(dbpkgbuf, p_restr_sess->user_flg, idx);

    // deal reuse swap id
    //uint32_t active_time = get_active_start_time(p_restr_sess->id);
	//pack_h(dbpkgbuf, active_time, idx);
	pack_h(dbpkgbuf, cost_cnt, idx);
	pack_h(dbpkgbuf, rew_cnt, idx);
	TRACE_LOG("restr :[%u %u %u]",p->id, cost_cnt, rew_cnt);
	for (uint32_t i = 0; i < cost_cnt; i++) {
		pack_h(dbpkgbuf, p_restr_sess->costs[i].give_type, idx);
		pack_h(dbpkgbuf, p_restr_sess->costs[i].give_id, idx);
		pack_h(dbpkgbuf, p_restr_sess->costs[i].count, idx);
		pack_h(dbpkgbuf, 0, idx);
		pack_h(dbpkgbuf, 0, idx);
		TRACE_LOG("cost item :[%u %u %u %u]",p->id, p_restr_sess->costs[i].give_type, p_restr_sess->costs[i].give_id,
								p_restr_sess->costs[i].count);
	}
	for (uint32_t i = 0; i < rew_cnt; i++) {
		uint32_t duration = 0;
		uint32_t lifetime = 0;
		uint32_t item_max = 999;

		if (p_restr_sess->rewards[i].give_type == give_type_normal_item
				 || p_restr_sess->rewards[i].give_type == give_type_clothes) {
			const GfItem* itm = items->get_item(p_restr_sess->rewards[i].give_id);
			duration = itm->duration * clothes_duration_ratio;
            lifetime = itm->lifetime();
			item_max = itm->max();
		} else if (p_restr_sess->rewards[i].give_type == give_type_player_attr && 
					p_restr_sess->rewards[i].give_id == give_type_apothecary_exp) {
			secondary_profession_data* p_sec_data;
			p_sec_data = get_player_secondary_profession_by_type(p, apothecary_profession_type);
			if (p_sec_data) {
				uint32_t cur_exp = p_sec_data->get_profession_exp();
				uint32_t max_exp = p_sec_data->m_pCalc->get_max_exp();
				uint32_t add_exp = p_restr_sess->rewards[i].count;
				if (cur_exp < max_exp) {
					p_restr_sess->rewards[i].count = (add_exp + cur_exp) > max_exp ? (max_exp - cur_exp) : add_exp;
				} else {
					p_restr_sess->rewards[i].count = 0;
				}
				item_max = max_exp;
			} else {
				return send_header_to_player(p, p->waitcmd, cli_err_secondary_pro_not_exist, 1);
			}
			
		}
		
		pack_h(dbpkgbuf, p_restr_sess->rewards[i].give_type, idx);
		pack_h(dbpkgbuf, p_restr_sess->rewards[i].give_id, idx);
		pack_h(dbpkgbuf, p_restr_sess->rewards[i].count, idx);
		pack_h(dbpkgbuf, 0, idx);
		pack_h(dbpkgbuf, duration, idx);
		
        if (i+1 >= 454 && i+1 <= 469) {
            do_stat_log_exchange(p->id, p->role_type, i+1, p_restr_sess->rewards[i].give_id);
        } else if (i+1 == 663 || i+1 == 664) {
            do_stat_log_get_material(p_restr_sess->rewards[i].give_id, p_restr_sess->rewards[i].count);
        }
		TRACE_LOG("reward item :[%u %u %u %u %u %u %u]",p->id, p_restr_sess->rewards[i].give_type, p_restr_sess->rewards[i].give_id,
								p_restr_sess->rewards[i].count, duration, lifetime, item_max);
        //ERROR_LOG("REWARD: [%u] [%s]", p_restr_sess->rewards[i].give_id, p_restr_sess->rewards[i].name);
	}
	if (ret_flg)
		return send_request_to_db(p, p->id, p->role_tm, dbproto_swap_action, dbpkgbuf, idx);
	else 
		return send_request_to_db(0, p->id, p->role_tm, dbproto_swap_action, dbpkgbuf, idx);
}


int db_add_swap_action_cnt(player_t* p, uint32_t userid, uint32_t role_tm, uint32_t swap_id)
{
	swap_action_data* p_res = g_swap_action_mrg.get_swap_action(swap_id);
	if (!p_res) {
		return 0;
	}
	int idx = 0;
	pack_h(dbpkgbuf, p_res->id, idx);
	pack_h(dbpkgbuf, p_res->type, idx);
	pack_h(dbpkgbuf, p_res->user_flg, idx);

	pack_h(dbpkgbuf, 0, idx);
	pack_h(dbpkgbuf, 0, idx);

	if (p)
		return send_request_to_db(p, userid, role_tm, dbproto_swap_action, dbpkgbuf, idx);
	else 
		return send_request_to_db(0, userid, role_tm, dbproto_swap_action, dbpkgbuf, idx);
}


int db_swap_action_empty(player_t* p, uint32_t resid, uint32_t times)
{
	swap_action_data restr, res_mail;
	swap_action_data* p_res = g_swap_action_mrg.get_swap_action(resid);

	if (!p_res) {
		return -1;
	}
	if (pack_swap_action_item(p, resid, p->role_type, &restr, &res_mail)) {
		return db_swap_action(p, &restr, &res_mail, false, times);
	}
	return 0;
}

uint32_t get_swap_action_times(player_t* p, uint32_t resid)
{
	PlayerSwapActionDataMap::iterator it = p->m_swap_action_data_maps->find(resid);
	if (it != p->m_swap_action_data_maps->end()) {
		return it->second.count;
	}
	return 0;
}

bool is_player_have_swap_action_times(player_t* p, uint32_t resid)
{
	uint32_t have_cnt = get_swap_action_times(p, resid);
	swap_action_data* p_res = g_swap_action_mrg.get_swap_action(resid);
	if (!(p_res->top_limit)) {
		return true;
	}
	if (have_cnt < p_res->top_limit) {
		return true;
	}
	return false;
}

uint32_t get_player_swap_action_left_times(player_t* p, uint32_t resid)
{
	uint32_t have_cnt = get_swap_action_times(p, resid);
	swap_action_data* p_res = g_swap_action_mrg.get_swap_action(resid);

	if (have_cnt >= p_res->top_limit) {
		return 0;
	}
	return (p_res->top_limit - have_cnt);
}

uint32_t get_swap_action_times_by_bound(player_t* p, uint32_t resid_low, uint32_t resid_high)
{
	uint32_t ret = 0;
	PlayerSwapActionDataMap::iterator it = p->m_swap_action_data_maps->begin();
	for ( ; it != p->m_swap_action_data_maps->end(); ++it) {
		if (it->second.id >= resid_low && it->second.id <= resid_high) {
			ret += it->second.count;
		}
	}
	return ret;
}


uint32_t get_swap_action_last_tm(player_t* p, uint32_t resid)
{
	PlayerSwapActionDataMap::iterator it = p->m_swap_action_data_maps->find(resid);
	if (it != p->m_swap_action_data_maps->end()) {
		return it->second.tm;
	}
	return 0;
}
/**
 * @param 
 */
bool is_swap_action_in_new_day(player_t* p, uint32_t resid)
{
    uint32_t last_tm = get_swap_action_last_tm(p, resid);

    time_t new_tm = last_tm + 86400;
    struct tm *ptm = localtime( &(new_tm) ); 
    ptm->tm_hour = 0;
    ptm->tm_min  = 0;
    ptm->tm_sec  = 0;

    time_t new_day = mktime(ptm);

    if (get_now_tv()->tv_sec > new_day) {
        return true;
    }
    return false;
}

bool if_have_get_swap_action(player_t* p, uint32_t resid_from, uint32_t resid_to)
{
	PlayerSwapActionDataMap::iterator it = p->m_swap_action_data_maps->begin();

	for (; it != p->m_swap_action_data_maps->end(); ++it) {
		uint32_t tmp_id = it->second.id;
		if (tmp_id >= resid_from && tmp_id <= resid_to && it->second.count) {
			return true;
		}
	}

	return false;
}


int add_swap_action_times(player_t* p, uint32_t resid, uint32_t times)
{
	PlayerSwapActionDataMap::iterator it = p->m_swap_action_data_maps->find(resid);
	if (it == p->m_swap_action_data_maps->end()) {
		player_swap_action_data data = { 0 };
		data.id = resid;
		data.type = 0;
		data.count = times;
		data.tm = get_now_tv()->tv_sec;
		p->m_swap_action_data_maps->insert(PlayerSwapActionDataMap::value_type(data.id, data));
	} else {
		it->second.count += times;
		it->second.tm = get_now_tv()->tv_sec;
	}

	return db_swap_action_empty(p, resid, times);
}

int clear_swap_action_times(player_t* p, uint32_t resid)
{
	PlayerSwapActionDataMap::iterator it = p->m_swap_action_data_maps->find(resid);
	if (it != p->m_swap_action_data_maps->end()) {
        it->second.count = 0;

        int idx = 0;
        uint8_t buf[1024] = {0};
        pack_h(buf, resid, idx);
        return send_request_to_db( 0, p->id, p->role_tm, dbproto_clear_swap_action, buf, idx);
	}

    return 0;
}

void calc_player_level(player_t *p)
{
	uint16_t pre_lv = p->lv;
	if ( calc_lv_add_exp(0, p->exp, p->lv, player_max_level, p->role_type, p->id) ) {
		if (p->lv >= min_hero_top_lv)
		{
			db_set_player_hero_top_info(p);
		}
		//calc_player_attr_ex(p);
        calc_and_save_player_attribute(p);
		p->hp = p->maxhp;
		p->mp = p->maxmp;
		p->skill_point += (p->lv > pre_lv ? (p->lv - pre_lv) : 0);
		//noti to db to save
		db_set_role_base_info(p);
		//noti to db to add amb achieve num
		if (p->lv == 5 && p->parentid && !is_achieve_amb_lv(p)) {
			db_add_amb_achieve_num(p->parentid);
			set_once_bit_on(p, flag_pos_achieve_lv);
			db_set_user_flag(p, 1);
			do_stat_log_achieve_child_num(p);
		}
        do_stat_log_role_exp(p->id, p->role_type, p->exp);
		//noti to map , lv up
		int idx = sizeof(cli_proto_t);
		idx += pack_player_lvup_noti(pkgbuf + idx, static_cast<uint32_t>(p->lv), p->hp, p->mp);
		init_cli_proto_head_full(pkgbuf, p->id, 0, cli_proto_player_lvup_noti, idx, 0);
		send_to_map(p, pkgbuf, idx, 0);
		//检查成就

		TRACE_LOG("lv update noti:uid=%u %u",p->id, p->lv);
        //stat_log_use_lv_change
        do_stat_log_role_lv_change(p->lv - 1, p->lv);
		
		if(p->lv %5 == 0 && p->lv <45 && is_finished_task(p, 1231)){
			send_lv_up_mail(p, mail_templet_level_up);
		}

        if (p->lv == 30 || p->lv == 40) {
            db_set_prentice_grade(p);
        }

		if (p->lv == 40) {
			get_invitee_player(p);
		}
	} 

}

/**
  * @brief get swap action
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_swap_action_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return db_get_swap_action(p);
}


int db_get_swap_action(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_swap_action, 0, 0);
}

int send_get_swap_action_rsp(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	uint32_t cnt = p->m_swap_action_data_maps->size();
	PlayerSwapActionDataMap::iterator it = p->m_swap_action_data_maps->begin();

	pack(pkgbuf, cnt, idx);
	for (; it != p->m_swap_action_data_maps->end(); ++it) {
		pack(pkgbuf, it->second.id, idx);
		pack(pkgbuf, it->second.count, idx);
		TRACE_LOG("%u %u", it->second.id, it->second.count);
	}
	init_cli_proto_head_full(pkgbuf, p->id, p->seqno, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}

int db_get_swap_action_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	const get_swap_action_header_rsp_t* rsp = reinterpret_cast<get_swap_action_header_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(get_swap_action_header_rsp_t) + 
							rsp->cnt * sizeof(get_swap_action_item_rsp_t));
	get_swap_action_item_rsp_t* p_data = reinterpret_cast<get_swap_action_item_rsp_t*>(reinterpret_cast<get_swap_action_header_rsp_t*>(body) + 1);
	p->m_swap_action_data_maps->clear();
	for (uint32_t i = 0; i < rsp->cnt; i++) {
		player_swap_action_data data = {0};
		data.id    =    p_data->id;
		data.type  =  p_data->type;
		data.count = p_data->count;
		data.tm    =    p_data->tm;
		if (p->m_swap_action_data_maps->find(data.id) == p->m_swap_action_data_maps->end()) {
			p->m_swap_action_data_maps->insert(PlayerSwapActionDataMap::value_type(data.id, data));
		}
		p_data++;
	}
	
    login_send_mail_action(p);

	if (p->waitcmd == cli_proto_login) {
		return db_get_card_list(p);
	} else if (p->waitcmd == cli_proto_get_swap_action_detail_info) {
		int idx = 0;
		uint32_t swap_id = 0;
		unpack_h(p->session, swap_id, idx);
		return do_get_swap_action_detail_info(p, swap_id);
	} else {
		return send_get_swap_action_rsp(p);
	}
}

int send_swap_action_result_to_world(player_t *p, char *active_name, char *item_name, int loop_mode = 0)
{
    char words[max_trade_mcast_size] = {0};
    int len = snprintf(words, max_trade_mcast_size, "%s%s,%s%s", char_content[2].content,active_name,char_content[3].content,item_name);
    TRACE_LOG("mcast title msg[%s][%d]",words,len);

    player_mcast_t info = {0};
    memset(&info, 0x00, sizeof(player_mcast_t)); 
    info.type = 2; //:player_market 2:player_world
    info.front_id = 2;
    memcpy(&(info.nick), g_title_condition.pve_note2, sizeof(info.nick));
    memcpy(&(info.info), words, max_trade_mcast_size);

    if (loop_mode) {
        char loop_words[max_trade_mcast_size] = {0};
        snprintf(loop_words, max_trade_mcast_size, "[%s]%s%s,%s%s", p->nick, 
            char_content[2].content,active_name, char_content[3].content,item_name);
        notify_official_mcast_info(p->id, cli_proto_mcast_official_notice, loop_words);
    } else {
        notify_player_mcast_info(p, mcast_trade_message, &info);
    }
    return 0;
}

int send_got_gold_dragon_to_world(player_t *p)
{
    char words[max_trade_mcast_size] = {0};
    int len = snprintf(words, max_trade_mcast_size, "%s", char_content[5].content);
    TRACE_LOG("mcast title msg[%s][%d]",words,len);

    player_mcast_t info = {0};
    memset(&info, 0x00, sizeof(player_mcast_t)); 
    info.type = 2; //:player_market 2:player_world
    info.front_id = 2;
    memcpy(&(info.nick), g_title_condition.pve_note2, sizeof(info.nick));
    memcpy(&(info.info), words, max_trade_mcast_size);

    notify_player_mcast_info(p, mcast_trade_message, &info);
    return 0;
}

int send_compose_to_world(player_t *p, uint32_t attire_id)
{

	const GfItem* itm = items->get_item(attire_id);

	if (itm->quality_lv < 4) {
		return 0;
	}
    char words[max_trade_mcast_size] = {0};
    int len = snprintf(words, max_trade_mcast_size, "%s%s", char_content[6].content, itm->get_name());
    TRACE_LOG("mcast title msg[%s][%d]",words,len);

    player_mcast_t info = {0};
    memset(&info, 0x00, sizeof(player_mcast_t)); 
    info.type = 2; //:player_market 2:player_world
    info.front_id = 2;
    memcpy(&(info.nick), g_title_condition.pve_note2, sizeof(info.nick));
    memcpy(&(info.info), words, max_trade_mcast_size);

    notify_player_mcast_info(p, mcast_trade_message, &info);
    return 0;
}


int db_swap_action_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	const swap_action_header_rsp_t* rsp = reinterpret_cast<swap_action_header_rsp_t*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(swap_action_header_rsp_t) + 
							rsp->add_cnt * sizeof(swap_action_item_rsp_t) +
							rsp->del_cnt * sizeof(swap_action_item_rsp_t));
	uint32_t restr_id = rsp->id;

	

    int idx = sizeof(cli_proto_t);
	
	swap_action_data* p_restr = (swap_action_data*)(p->session);//&(g_all_restr[restr_id - 1]);
	swap_action_item_rsp_t* swap_item_rsp = 
		reinterpret_cast<swap_action_item_rsp_t*>(reinterpret_cast<swap_action_header_rsp_t*>(body) + 1);

	if (p_restr->id == 1158 ) {
		g_swap_action_cnt_1 ++;
	}
	if (p_restr->id == 1159 ) {
		g_swap_action_cnt_2 ++;
	}
	if ( p_restr->id == 1162 ) {
		g_swap_action_cnt_3 ++;
	}
	if ( p_restr->id == 1163 ) {
		g_swap_action_cnt_4 ++;
	}

//	if ( p_restr->id == 1173 ) {
//		p->player_show_state = player_city_team(p->id, p->role_tm);
//		//ERROR_LOG("P CITY TEAM %u %u-----------", p->id, p->player_show_state);
//	}
	if (p_restr->id == 1297) {
		set_player_other_info(p, 6, 0);
	}

	if ((p_restr->id >= 1409 && p_restr->id <= 1411) || p_restr->id ==  1416 || p_restr->id == 1427 || p_restr->id == 1511) {
		player_gain_team_exp_by_swap(p, p_restr->id);
	}

    if (p_restr->id >= 1451 && p_restr->id <= 1455) {
        do_2v2_title(p, p_restr->id);
    }

	pack(dbpkgbuf, p_restr->type, idx);
	pack(dbpkgbuf, rsp->id, idx);
	pack(dbpkgbuf, rsp->del_cnt, idx);
	pack(dbpkgbuf, rsp->add_cnt, idx);
	
    uint32_t gold_coin_count = 0;
	//update my packs
	for (uint32_t i = 0; i < rsp->del_cnt; i++) {
		//items
		if (swap_item_rsp->type == give_type_normal_item) {
			p->my_packs->del_item(p, swap_item_rsp->id, swap_item_rsp->count, channel_string_active);

			if (swap_item_rsp->id == 1500340) { 
                do_stat_log_universal_interface_1(stat_log_element_cost, 0, swap_item_rsp->count);
            }
			if (swap_item_rsp->id == 1500344) { 
                do_stat_log_universal_interface_2(stat_log_lingli_neihe, 0, 0, swap_item_rsp->count);
            }
            if (swap_item_rsp->id == 1500362) {
                do_stat_log_universal_interface_2(stat_log_qiannian_jinghun, 0, 0, swap_item_rsp->count);
            }
		} else if (swap_item_rsp->type == give_type_clothes) {
		//attires
			p->my_packs->del_clothes_by_id(swap_item_rsp->id, channel_string_active);
		} else if (swap_item_rsp->type == give_type_player_attr) {
		//attr
			switch (swap_item_rsp->id) {
				case give_type_exp:
					break;
				case give_type_xiaomee:
					do_stat_log_coin_add_reduce(p, (uint32_t)0, swap_item_rsp->count);
					p->coins = p->coins > swap_item_rsp->count ? 
						p->coins - swap_item_rsp->count : 0;
					break;
			    case give_type_skillpoint:
					p->skill_point = p->skill_point > swap_item_rsp->count ? 
						p->skill_point - swap_item_rsp->count : 0;
					break;
			    case give_type_fumo_point:
					do_stat_log_fumo_add_reduce((uint32_t)0, swap_item_rsp->count);
					p->fumo_points_total = p->fumo_points_total > swap_item_rsp->count ? 
						p->fumo_points_total - swap_item_rsp->count : 0;
					break;
				case give_type_expliot:
					p->exploit = p->fumo_points_total > swap_item_rsp->count ? 
						p->exploit - swap_item_rsp->count : 0;
					break;
				case give_type_double_tm:
					break;
				case give_type_honor:
					p->honor = p->fumo_points_total > swap_item_rsp->count ? 
						p->honor - swap_item_rsp->count : 0;
					break;
				case give_type_apothecary_exp:
					break;
				case give_type_summon_exp:
					break;
                case give_type_sword_value:
                    break;
				default:
					break;
			}
		}

		pack(dbpkgbuf, swap_item_rsp->type, idx);
		pack(dbpkgbuf, swap_item_rsp->id, idx);
		pack(dbpkgbuf, swap_item_rsp->count, idx);
		
		KDEBUG_LOG(p->id, "ACTION COST\t[type=%u id=%u cnt=%u channel=%s res_id=%u]", 
				swap_item_rsp->type, swap_item_rsp->id, swap_item_rsp->count, channel_string_active, p_restr->id);
        gold_coin_count += (get_feedback_coin_cnt(swap_item_rsp->id) * swap_item_rsp->count);

		swap_item_rsp++;
	}
	
	for (uint32_t i = 0; i < rsp->add_cnt; i++) {
		//player attr
		if (swap_item_rsp->type == give_type_player_attr) {
			switch (swap_item_rsp->id) {
				case give_type_exp:
					p->exp += swap_item_rsp->count;
					break;
				case give_type_xiaomee:
					p->coins += swap_item_rsp->count;
					do_stat_log_coin_add_reduce(p, swap_item_rsp->count, (uint32_t)0, channel_string_active);
					break;
			    case give_type_skillpoint:
					p->skill_point += swap_item_rsp->count;
					break;
			    case give_type_fumo_point:
					p->fumo_points_total += swap_item_rsp->count;
					do_stat_log_fumo_add_reduce(swap_item_rsp->count, (uint32_t)0);
					break;
				case give_type_expliot:
					p->exploit += swap_item_rsp->count;
					break;
				case give_type_double_tm:
					break;
				case give_type_honor:
					p->honor += swap_item_rsp->count;
					break;
				case give_type_apothecary_exp:
					add_player_secondary_professoin_exp(p, apothecary_profession_type, swap_item_rsp->count, &(swap_item_rsp->count));
					break;
				case give_type_summon_exp:
					break;
                case give_type_sword_value:
                    p->vip_sword_value += swap_item_rsp->count;
                    calc_player_vip_info(p, true);
                    break;
                case give_type_vip_qualify:
                    do_swap_vip_qualify(p, p->id, swap_item_rsp->count);
                    break;
				default:
					break;
			}

			// - -! - -! - -! - -! - -! - -! - -! - -! - -!//
			do_stat_log_fumo_consume(swap_item_rsp->id, p->lv, p->id, swap_item_rsp->count);
		}

		//player item
		if (swap_item_rsp->type == give_type_normal_item) {
			p->my_packs->add_item(p, swap_item_rsp->id, swap_item_rsp->count, channel_string_active, true, swap_action);

			// - -! - -! - -! - -! - -! - -! - -! - -! - -!//
			//if the item is relive item then log it
			stat_log_relive_item(swap_item_rsp->id, swap_item_rsp->count);
			
            // if the item is coupons item then log it
            //if (swap_item_rsp->id == 1700061) { 
            //    do_stat_log_universal_interface_1(stat_log_get_coupons_item, 0, swap_item_rsp->count);
            //}
			if (swap_item_rsp->id == 1500339) { 
                do_stat_log_universal_interface_2(stat_log_peach_got, 0, swap_item_rsp->count, 0);
            }
			do_stat_log_fumo_consume(swap_item_rsp->id, p->lv, p->id, swap_item_rsp->count);
		}

		//player attire
		if (swap_item_rsp->type == give_type_clothes) {
			p->my_packs->add_clothes(p, swap_item_rsp->id, swap_item_rsp->unique_id, swap_item_rsp->duration, channel_string_active, 0, 0);

			// - -! - -! - -! - -! - -! - -! - -! - -! - -!//
	        do_stat_log_fumo_consume(swap_item_rsp->id, p->lv, p->id, 1);
		}

		pack(dbpkgbuf, swap_item_rsp->type, idx);
		pack(dbpkgbuf, swap_item_rsp->id, idx);
        if (swap_item_rsp->type == give_type_clothes) {
            pack(dbpkgbuf, swap_item_rsp->unique_id, idx);
        } else {
            pack(dbpkgbuf, swap_item_rsp->count, idx);
        }

		KDEBUG_LOG(p->id, "ACTION REWARD\t[type=%u id=%u cnt=%u channel=%s res_id=%u]", 
			swap_item_rsp->type, swap_item_rsp->id, swap_item_rsp->count, channel_string_active, p_restr->id);

        // -- broadcast -- //
        for (uint32_t i = 0; i < p_restr->rew_count; i++) {
            if (swap_item_rsp->id == p_restr->rewards[i].give_id && p_restr->rewards[i].is_bcast) {
                //ERROR_LOG("BROADCAST: %u [%u] [%s]", i, swap_item_rsp->id, p_restr->rewards[i].name);
                int loop_mode = 0;
                if (is_all_server_limit_item(p_restr->id, p_restr->rewards[i].give_id)) {
                    loop_mode = 1;
                }
                const GfItem* itm = items->get_item(p_restr->rewards[i].give_id);
                if (itm) {
                    send_swap_action_result_to_world(p, p_restr->name, (char*)(itm->get_name()), loop_mode);
                } else {
                    send_swap_action_result_to_world(p, p_restr->name, p_restr->rewards[i].name, loop_mode);
                }
            }
        }

		swap_item_rsp++;
	}

	init_cli_proto_head(dbpkgbuf, p, p->waitcmd, idx);
	send_to_player(p, dbpkgbuf, idx, 1);

    //-------- gold coin feedback ---------
    player_gain_item(p, 2, gold_coin_item_id, gold_coin_count, channel_string_active, false);

	calc_player_level(p);

	if (p_restr->cd_time) {
		db_add_buff(p, max_nor_buff_type + restr_id, p_restr->cd_time, 0);
	}

	PlayerSwapActionDataMap::iterator it = p->m_swap_action_data_maps->find(restr_id);
	if (it == p->m_swap_action_data_maps->end()) {
		player_swap_action_data data = { 0 };
		data.id = restr_id;
		data.type = p_restr->type;
		data.count = 1;
        data.tm = get_now_tv()->tv_sec;
		p->m_swap_action_data_maps->insert(PlayerSwapActionDataMap::value_type(data.id, data));
	} else {
		it->second.count++;
        it->second.tm = get_now_tv()->tv_sec;
	}

    if (restr_id >= 1090 && restr_id <= 1109) {
        //add message: finger guessing clear zero 
        clear_swap_action_times(p, 1088);
        clear_swap_action_times(p, 1089);
        add_swap_action_times(p, 1087);
    }
//added by cws 20120515
    if (restr_id >= 1444 && restr_id <= 1446) {
        add_swap_action_times(p, 1447,1);
    }
   
    if (restr_id == 1442 || restr_id == 1443) {
        add_swap_action_times(p, 1447,2);
    }
	//times of rewards for escort
    if (restr_id >= 1486 && restr_id <= 1489) {
        add_swap_action_times(p, 1490,1);
    }
	//see strengthen_attire_without_material
    if (restr_id == 1510) {
        do_strengthen_attire_without_material(p, p_restr->ex_field_1);
    }
    if (restr_id == 1509) {
        add_exp_to_summon_rudely(p, 49720);
    }
	
////
    if (restr_id == 1259) {
        //make_fireworks_item_id:
        notify_player_play_fireworks(p);
    }

    if (restr_id == 1256) {
        notify_event_to_seer(p, 1);
    }


	///////////////---end and start log//////
    if (restr_id > 1000) {
        do_stat_log_universal_interface_1(0x09620000 + restr_id - 1000, 0, 1);
    }
	if (restr_id > 208 && restr_id < 214) {
		do_stat_log_universal_interface_1(stat_log_element_daily, restr_id, 1);
		do_stat_log_universal_interface_1(stat_log_element_join, restr_id, p->id);
	} else if (restr_id > 227 && restr_id < 235) {
		do_stat_log_universal_interface_2(stat_log_peach_daily, p_restr->id, 1, p->id);
	} else if ((restr_id > 234 && restr_id < 241) || 
		(restr_id > 241 && restr_id < 262)) {
		do_stat_log_universal_interface_1(stat_log_exchange_times_1, p_restr->id, 1);
	} else if (restr_id == 1031) {
        do_stat_log_universal_interface_1(0x09526002, 0, 1);
    } else if (restr_id >= 1366 && restr_id <= 1369) {
        do_stat_log_universal_interface_1(0x0952620D, 0, 1);
    } else if (restr_id >= 1507 && restr_id <= 1510) {
        do_stat_log_universal_interface_1(0x09060000, restr_id - 1507, p->id);
    }

	///////////////---end and start send mail//////
	uint32_t duration = 0, lifetime = 0;
	char title[MAX_MAIL_TITLE_LEN + 1] = {0};
	char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
	std::vector<mail_item_enclosure> item_vec;
	std::vector<mail_equip_enclosure> equips;
	const swap_action_data* p_res_mail = (swap_action_data*)(p->session + sizeof(swap_action_data));
	if (p_res_mail->rew_count) {
		send_header_to_player(p,p->waitcmd, cli_err_bag_no_space_2_mail, 0);
	}
	strncpy(title, p_res_mail->name, sizeof(title) - 1);
	strcpy(content,"");
	TRACE_LOG("mail: %s ===== %s", title, content);
	for (uint32_t i = 0; i < p_res_mail->rew_count; i++) {
		if (p_res_mail->rewards[i].give_type == give_type_normal_item) {
			item_vec.push_back(mail_item_enclosure(p_res_mail->rewards[i].give_id, p_res_mail->rewards[i].count));
			//if the item is relive item then log it
			stat_log_relive_item(p_res_mail->rewards[i].give_id, p_res_mail->rewards[i].count);
			
		} else if (p_res_mail->rewards[i].give_type == give_type_clothes) {
			const GfItem* itm = items->get_item(p_res_mail->rewards[i].give_id);
			duration = itm->duration * clothes_duration_ratio;
			lifetime = itm->lifetime();
            //TRACE_LOG("send mail:[%u %u %u]",p_res_mail->rewards[i].give_id,duration,lifetime);
			equips.push_back(mail_equip_enclosure(p_res_mail->rewards[i].give_id, 
				get_now_tv()->tv_sec, 0, duration, lifetime));
		} 
		if (item_vec.size() == 3) {
			db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, 0);
			item_vec.clear();
		}
		if (equips.size() == 3) {
			db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, 0, &equips);
			equips.clear();
		}
	}
	if (item_vec.size() || equips.size()) {
		db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, 
			item_vec.size() ? &item_vec : 0, equips.size() ? &equips : 0);
		item_vec.clear();
		equips.clear();
	}

    ///////////---------achieve and title-----------------/////////////
    if ( (restr_id == 1078 || restr_id == 1079) && p->contest_final_rank == 1 ) {
//        if (!is_player_achievement_data_exist(p, 111))
//            do_achieve_type_logic(p, 111, false); //天下第一
        do_special_title_logic(p, 104);
    }
    if (restr_id == 1082 ) {
        //if (!is_player_achievement_data_exist(p, 113))
        //    do_achieve_type_logic(p, 113, false);//优秀队长
        //do_special_title_logic(p, 106);
    }
    if ((restr_id == 1075 || restr_id == 1076) ) {
//        if ( !is_player_achievement_data_exist(p, 110))
//            do_achieve_type_logic(p, 110, false);//白大侠是
        do_special_title_logic(p, 103);
    }

	//summon dragon
	switch (rsp->id) {
		case 1110:
			g_limit_data_mrg.add_item_cnt(100000001, 1, true);
			break;
		case 1111:
			g_limit_data_mrg.add_item_cnt(100000002, 1, true);
			break;
		case 1112:
			g_limit_data_mrg.add_item_cnt(100000003, 1, true);
			break;
		case 1113:
			g_limit_data_mrg.add_item_cnt(100000004, 1, true);
			break;
		case 1114:
			g_limit_data_mrg.add_item_cnt(100000005, 1, true);
			break;
		default:
			break;
	}

	proc_gain_item_achieve_logic(p, 0, 0, gain_item_by_swap, p_restr->id);
	return 0;

}

bool check_finger_guessing(player_t* p, uint32_t swap_id)
{
    bool ret = false;
    uint32_t victor_times = get_swap_action_times(p, 1089);

    if (swap_id == 1109) {
        uint32_t limit_times = p->act_record[act_record_pos] + (is_vip_player(p) ? 5 : 2);
        uint32_t finger_time = get_swap_action_times(p, 1087);
        uint32_t gift_get = get_swap_action_times(p, 1109); 
        if (gift_get <= finger_time + 1 && gift_get <= limit_times) 
            return true;
    }
    switch (swap_id) {
        case 1090:
        case 1091:
            if (victor_times >= 5) ret = true;
            break;
        case 1092:
        case 1093:
        case 1094:
        case 1095:
        case 1096:
            if (victor_times >= 6) ret = true;
            break;
        case 1097:
            if (victor_times >= 7) ret = true;
            break;
        case 1098:
        case 1099:
            if (victor_times >= 8) ret = true;
            break;
        case 1100:
        case 1101:
        case 1102:
        case 1103:
        case 1104:
        case 1105:
        case 1106:
            if (victor_times >= 9) ret = true;
            break;
        default:
            if (victor_times >= 10) ret = true;
            break;
    }
    return ret;
}

bool judge_if_can_get_swap_fumo_attire(player_t* p, uint32_t swap_id)
{
    if ( (swap_id == p->role_type + 1259) || (swap_id == p->role_type + 1263) ) {
        uint32_t cnt1 = get_swap_action_times(p, 1259 + p->role_type);
        uint32_t cnt2 = get_swap_action_times(p, 1263 + p->role_type);
        if ( (cnt1 + cnt2) < (uint32_t)(1 + is_vip_player(p) ? 1 : 0) ) {
            return true;
        }
    }
    return false;
}

bool is_in_reward_list(player_t* p, uint32_t id)
{
    std::vector<role_info_t>::iterator it = p->reward_role_vec->begin();
    for (; it != p->reward_role_vec->end(); ++it) {
        //ERROR_LOG(" -----> %u %u", it->userid, id);
        if (it->userid == id) {
            return true;
        }
    }
    return false;
}

uint32_t judge_if_can_get_dragon_reward(player_t* p, uint32_t swap_id)
{
	if (swap_id < 1117 || swap_id > 1132) {
		return 0;
	}

	if (if_have_get_swap_action(p, 1117, 1132)) {
		return cli_err_have_get_summoned_dragon_rewards;
	}
	
	if (!get_swap_action_times(p, 1110) && 
		!get_swap_action_times(p, 1111) &&
		!get_swap_action_times(p, 1112) &&
		!get_swap_action_times(p, 1113) &&
		!get_swap_action_times(p, 1114)) {
		return cli_err_not_give_nimbus;
	}

	uint32_t dragon_uid = g_limit_data_mrg.get_item_cnt(100000011);
	uint32_t nimbus = g_limit_data_mrg.get_item_cnt(100000010);
	uint32_t reward_type = 0;

	if (!nimbus) {
		return cli_err_have_not_summoned_dragon;
	}
	if (g_limit_data_mrg.get_item_cnt(100000010) < 3000) {
		reward_type = 1;
	} else if (g_limit_data_mrg.get_item_cnt(100000010) < 6000) {
		reward_type = 2;
	} else if (g_limit_data_mrg.get_item_cnt(100000010) >= 6000) {
		reward_type = 3;
	} else {
		return cli_err_have_not_summoned_dragon;
	}

	if (p->id == dragon_uid || get_swap_action_times(p, 1196)) {
		if (reward_type == 1) {
			//1117 1118 
			if (swap_id == 1117 || swap_id == 1118) {
			} else {
				return cli_err_summoned_dragon_rewards_type_err;
			}
		}

		if (reward_type == 2) {
			//1119-1122 1123
			if ((swap_id <= 1122 && swap_id >= 1119) ||
				swap_id == 1123 ||
				swap_id == 1117 || swap_id == 1118) {
			} else {
				return cli_err_summoned_dragon_rewards_type_err;
			}
		}

		if (reward_type == 3) {
			//1124 - 1127
			if (swap_id >= 1124 || swap_id <= 1127 ||
				(swap_id <= 1122 && swap_id >= 1119) ||
				swap_id == 1123 ||
				swap_id == 1117 || swap_id == 1118) {
			} else {
				return cli_err_summoned_dragon_rewards_type_err;
			}
		}

	} else {
		if (reward_type == 1) {
			//1128 1129
			if (swap_id == 1128 || swap_id == 1129) {
			} else {
				return cli_err_summoned_dragon_rewards_type_err;
			}
		}

		if (reward_type == 2) {
			//1130 1131
			if (swap_id == 1130 || swap_id == 1131 ||
				swap_id == 1128 || swap_id == 1129) {
			} else {
				return cli_err_summoned_dragon_rewards_type_err;
			}
		}

		if (reward_type == 3) {
			//1132
			if (swap_id == 1132 || 
				swap_id == 1130 || swap_id == 1131 || 
				swap_id == 1128 || swap_id == 1129) {
			} else {
				return cli_err_summoned_dragon_rewards_type_err;
			}
		}
	}
	return 0;
}

int swap_action_ex_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	return swap_action_cmd(p, body, bodylen);
}
//
bool is_swap_conflict(player_t* p, uint32_t resid){
	time_t cur_time = time(NULL);
	time_t time_cof = get_swap_action_last_tm(p, resid);
	struct tm  _tm, _tmcof;
	localtime_r(&cur_time, &_tm);
	localtime_r(&time_cof, &_tmcof);	
	//TRACE_LOG("cws cur time %d, %d, %d",_tm.tm_year ,_tm.tm_mon,_tm.tm_mday);
	if(_tm.tm_year == _tmcof.tm_year && _tm.tm_mon== _tmcof.tm_mon
		&& _tm.tm_mday == _tmcof.tm_mday) {
		//TRACE_LOG("cws time conflict %d time_cof %d, %d, %d",resid,_tmcof.tm_year ,_tmcof.tm_mon,_tmcof.tm_mday);
		return 1;
	}
	//TRACE_LOG("cws time no conflict %d time_cof %d, %d, %d",resid,_tmcof.tm_year ,_tmcof.tm_mon,_tmcof.tm_mday);
	return 0;
}
int add_exp_to_summon_rudely(player_t *p, uint32_t allocated_exp)
{
	summon_mon_t * p_mon = p->fight_summon;
	if(!p_mon) {
		ERROR_LOG("cws allocated exp noexit summon");
		return send_header_to_player(p, p->waitcmd,	cli_err_allocate_exp_summon_not_exit, 1); 
	}

    uint32_t gain_exp = 0;
    if(p_mon->lv >= p->lv) {
        gain_exp = 0;
    } else {
        if (p_mon->mon_type % 10 == 6) {
            gain_exp = calc_exp(p->lv, true) - p_mon->exp;
        } else {
            uint32_t next_lv = get_next_gradation_lv(p_mon->mon_type);
            gain_exp = next_lv ? (calc_exp(next_lv, true) + 1 - p_mon->exp) : 0;;
        }
    }

    uint32_t pre_lv = p_mon->lv;
	calc_lv_add_exp(gain_exp, p_mon->exp, p_mon->lv, p->lv, p->role_type, p->id, true);  
	//TRACE_LOG("allocated_exp is %u", gain_exp);
	if (pre_lv != p_mon->lv) {
		attr_data * data = new attr_data;
	   	calc_summon_attr(data, get_summon_attr_type(p_mon->mon_type), p_mon->attr_per, p_mon->lv);
		set_summon_attr(p_mon, data);
	}

	//TRACE_LOG("cws db_allocate_exp_to_summon,allocated_exp is %d", gain_exp);
	db_allocate_exp_to_summon(p, p_mon, 0);
	return 0;
}
/**
  * @brief upgrade item (items_upgrade.xml)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int swap_action_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t swap_id = 0, ex_field_1 = 0, ex_field_2 = 0;

	unpack(body, swap_id, idx);
	unpack(body, ex_field_1, idx);
	unpack(body, ex_field_2, idx);

	if (!swap_id) {
		p->waitcmd = 0;
		return 0;
	}
    if (swap_id < 1422 || swap_id >1425) {
        if (is_time_limit_3(p)) {
            return send_header_to_player(p, p->waitcmd, cli_err_action_time_limited, 1);
        }
    }
	
	switch (swap_id) {
        case 1422:
        case 1423:
        case 1424:
        case 1425:
            if (!check_login_box(p, swap_id, ex_field_1)) {
                return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
            }
            break;
		case 1231:
			break;
		case 1232:
			{
				uint32_t parent_id = ex_field_1;
				if (!is_valid_uid(parent_id) || p->id == ex_field_1) {
					return send_header_to_player(p, p->waitcmd, cli_err_userid_error, 1);
				}
				if (get_swap_action_times(p, 1232)) {
					return send_header_to_player(p, p->waitcmd, cli_err_tmp_baotu_have_regedit, 1);
				}
			}

			break;
		case 1233:
			{
				uint32_t total_times = get_swap_action_times(p, 1231);
				uint32_t done_times = get_swap_action_times(p, 1233);
				if (done_times >= total_times / 6) {
					return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
				}
			}
			break;
           //added by cws 20120515
 		case 1448:
			{
				uint32_t lottery_times = 1 + get_swap_action_times(p, 1447) 
                    + (p->act_record[act_record_pos] ? 15 : 0);
				uint32_t lottery_used = get_swap_action_times(p, 1448);
				if (lottery_used > lottery_times) {
					return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
				}
			}
			break;  
		case 1471:
			{
				if(get_swap_action_times(p, 1471) >=1){
					return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
				}
			}
			break;
        case 1482:
        case 1483:
        case 1484:	
        case 1485:
            {	
                global_limit_data_t* p_limit_pick = g_limit_data_mrg.get_limit_data(swap_id + 4);
                if (p_limit_pick && p_limit_pick->left_cnt == 0) {
                    return send_header_to_player(p, p->waitcmd, cli_err_action_global_limited, 1);
                }				
                chg_app_buff(p, 3000);
                do_event_trigger_logic(p, swap_id - 1481, p->trigger_times);
            }
            break;
        case 1486:
        case 1487:
        case 1488:
        case 1489:
            {
                chg_app_buff_back(p,3000);
                del_event_trigger_timer(p);
                if (p->trigger_times <= 0 || p->trigger_block != p->trigger_victory) {
                    return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
                }
                set_player_event_trigger(p, 0, 0, 0);
			}
            break;
        case 1499:
            chg_app_buff_back(p,3000);
            del_event_trigger_timer(p);
            set_player_event_trigger(p, 0, 0, 0);
            break;
            //for lottery of escort
        case 1490:
            ERROR_LOG("%u use plugin !!!!!!!!", p->id);
            return -1;
            break;

	case 1491:
			{
				uint32_t lottery_times4escort = get_swap_action_times(p, 1490); 
				uint32_t lottery_times4used = get_swap_action_times(p, 1491);
				if (lottery_times4used >= lottery_times4escort) {
					return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
				}
			}
		break;


	case 1507:
		{
			if(get_swap_action_times(p, 1507) >= get_swap_action_times(p, 1506)){
					return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
			}
		}
		break;
	case 1508:
		{
			if( is_swap_conflict(p, 1509) || is_swap_conflict(p, 1510) ){ 
				return send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act, 1);
			}
		}
		break;	
	case 1509:
		{
			if( is_swap_conflict(p, 1508) || is_swap_conflict(p, 1510) ){ 
				return send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act, 1);
			}
		}
		break;
	case 1510:
		{
			if(is_in_active_time_section(38) != 0)
			{
				TRACE_LOG("cws not in time");
				return send_header_to_player(p, p->waitcmd, cli_err_swap_action_not_in_time, 1);
			}
			if( is_swap_conflict(p, 1508) || is_swap_conflict(p, 1509) ) {
				TRACE_LOG("cws 1510 swap conflict with the other two");
				return send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act, 1);
            }
			if(is_attire_without_material_ok(p, ex_field_1) != 1)
				return send_header_to_player(p, p->waitcmd, cli_err_attire_cannot_strengthen, 1);
		}
		break;

		///
        case 1466:
			{
				if (get_swap_action_times(p, 1466) >= get_swap_action_times(p, 1465)) {
					return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
				}
			}
			break;  
        case 1259:
            {
                if (p->cur_map->id != 7) {
                    return send_header_to_player(p, p->waitcmd, cli_err_not_in_correct_map, 1);
                }
            }
            break;
        case 1276:
            {
                player_t* master = get_player(ex_field_1);
                if (!master || !is_valid_uid(ex_field_1) || p->id == ex_field_1) {
                    return send_header_to_player(p, p->waitcmd, cli_err_userid_error, 1);
                }
                //ERROR_LOG("swap 1276 %u %u %u| %u", ex_field_1, master->app_mon, get_swap_action_times(master, 1277), p->id);
                if (master->app_mon != get_rich_monster_id || is_in_reward_list(master, p->id)) {
                    return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
                }
                // save player list
                role_info_t role = {0};
                role.userid = p->id;
                role.roletm = p->role_tm;
                master->reward_role_vec->push_back(role);
                add_swap_action_times(master, 1277);
                if (get_swap_action_times(master, 1277) == 10) {
                    set_app_buf_end(master, NULL); // 1:clear buf 2:reward 3:clear times 4:clear role 
                    //player_gain_item(master, 2, get_rich_item_id, 20);
                    //clear player list
                    //master->reward_role_vec->clear();
                }
            }
            break;
        case 1286:
            {
                if (!is_power_user_player(p)) {
                    return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
                }
            }
            break;
        case 1285:
            {
                if (get_swap_action_times(p, 1285) >= p->vip_level) {
                    return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
                }
            }
            break;
        case 1290:
            {
                if (!is_vip_year_player(p)) {
                    return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
                }
            }
            break;
		case 1297:
			{
				uint32_t other_score = get_player_other_info_value(p, 6);
				if (other_score < 9) {
                    return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
				}
			}
			break;
        case 1312:
        case 1313:
            if (!is_player_checked_pay_passwd(p)) {
                return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
            }
            break;
        case 1364:
            if (!is_vip_player(p) && p->lv < 45) {
                return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
            }
            break;
        case 1392:
            if (p->act_record[act_record_pos] < 3) {
                return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
            }
            break;
		case 1393:
			if (p->oltoday < 3600 && p->coins >= 5000000) {
                return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
			}
            break;
        case 1394:
            if (get_swap_action_times(p, 1395) + get_swap_action_times(p, 1396) >= 1) {
                return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);	
            }
            break;
		case 1395:
            if (get_swap_action_times(p, 1394) + get_swap_action_times(p, 1396) >= 1) {
                return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);	
            }
			if (p->oltoday + (get_now_tv()->tv_sec - p->login_tm) < 3600) {
                return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
			}
            break;
        case 1421:
            if (get_swap_action_times(p, 1421) >= get_swap_action_times(p, 1420) / 3) {
                return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
            }
            break;
        case 1451:
        case 1452:
        case 1453:
        case 1454:
        case 1455:
            if (!judge_player_catch_2v2_rewards(p, swap_id)) {
                return send_header_to_player(p, p->waitcmd, cli_err_2v2_reward_err, 1);
            }
            break;
        default:
			break;
	}
	
	if ((swap_id >= 1234 && swap_id <= 1253) || (swap_id >= 1268 && swap_id <= 1272)) {
		uint32_t tmp_cnt = get_swap_action_times_by_bound(p, 1234, 1253);
		tmp_cnt += get_swap_action_times_by_bound(p, 1268, 1272);
		if (tmp_cnt) {
			return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);	
		}
	}

	if (swap_id >= 1398 && swap_id <= 1402) {
		uint32_t need_score[5] = {10, 50, 100, 200, 300};
		if (del_player_red_blue_score(p, need_score[swap_id - 1398]) == -1) {
			return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
		}
	}

	if (swap_id >= 1494 && swap_id <= 1496) {
		uint32_t invitee_succ_stat = 8;
		uint32_t invitee_succ_cnt =  get_player_stat_info(p, invitee_succ_stat);
		if (invitee_succ_cnt < (swap_id - 1493) * 10) {
			p->waitcmd = 0;
			return 0;
		}
	}

	if (swap_id == 1403) {
		uint32_t win_team = blue_team;
		if (get_red_banner_cnt() > get_blue_banner_cnt()) {
			win_team = red_team;
		}
		if ((p->p_banner_info->protect_banner == 0 && p->p_banner_info->rob_banner == 0) || 
			   win_team != p->p_banner_info->random_master) {
			return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
		}	
	}

	if (swap_id >= 1409 && swap_id < 1411) {
		if (check_fight_team_exp_swap(p, swap_id) != 0) {
			return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
		}
	}

	if (swap_id == 1511) {
		if (p->team_info.captain_uid == p->id && p->team_info.captain_role_tm) {
			int rank = get_player_ranker_range(p, 10001);
			if (rank == -1 || rank >= 100) {
				return send_header_to_player(p, p->waitcmd, cli_err_not_in_ranking_list, 1); 
			}
		} else {
			return send_header_to_player(p, p->waitcmd, cli_err_not_team_captain, 1);
		}
	}

	if (swap_id >= 1435 && swap_id <= 1438) {
		if (can_player_get_contest_reward(p)) {
			reward_player_team_contest_win(p);
		} else {
			return send_header_to_player(p, p->waitcmd, cli_err_no_reward_item_left, 1);
		}
	}

//	//tmp for city_team rand prepare
//	if (swap_id == 1173) {
//		uint32_t rand_team_time_id = 6;
//		if (!get_now_active_data_by_active_id(rand_team_time_id)) {
//			return send_header_to_player(p, p->waitcmd, cli_err_swap_action_not_in_time, 1); 
//		}
//	}
	if (g_swap_action_mrg.check_limit(p, swap_id)) {
		p->waitcmd = 0;
		return 0;
	}

    if (swap_id == 1396) {
        if (get_swap_action_times(p, 1394) + get_swap_action_times(p, 1395) >= 1) {
			return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);	
        }
        if (!get_player_other_info_value(p, 8)) {
            if (is_in_active_time_section(22) != 0) {
                return send_header_to_player(p, p->waitcmd, cli_err_swap_action_not_in_time, 1); 
            }
            uint32_t value = p->oltoday + (get_now_tv()->tv_sec - p->login_tm);
            set_player_other_info(p, 8, value);
            return send_header_to_player(p, p->waitcmd, cli_err_accept_vip_reward_task, 1);
        } else {
            if (get_player_other_info_value(p, 8) + 900 > p->oltoday + (get_now_tv()->tv_sec - p->login_tm)) {
                return send_header_to_player(p, p->waitcmd, cli_err_action_cd_limited, 1);
            }
        }
    }

	uint32_t cd_time = g_swap_action_mrg.get_cd_time(swap_id);
	if (cd_time) {
		if (is_buff_alive(p,  swap_id + max_nor_buff_type)) {
			if (swap_id == 1259) {
				return send_header_to_player(p, p->waitcmd, cli_err_yanhua_cdtime, 1);
			} else if (swap_id > 1000) {
				return send_header_to_player(p, p->waitcmd, cli_err_nangua_cd_tm, 1);
			} else {
				return send_header_to_player(p, p->waitcmd, cli_err_action_cd_limited, 1);
			}
		}
	}

    if (swap_id >= 1069 && swap_id <= 1082) {
        if (!check_contest_game_limit(p, swap_id)) {
            form_contest_game_errcode(swap_id);
            return send_header_to_player(p, p->waitcmd, cli_err_not_in_ranking_list, 1);
        }
    }

    if (swap_id >= 1090 && swap_id <= 1109) {
        if ( !check_finger_guessing(p, swap_id) ) {
            return send_header_to_player(p, p->waitcmd, cli_err_not_in_ranking_list, 1);
        }
    }

    //
    if (swap_id == 1140) {
        return -1;
    }
	if (swap_id == 1147) {
		uint32_t pre_summon_tm = get_summon_tm_by_one_type(p, 1111);
		if (!pre_summon_tm) {
			return send_header_to_player(p, p->waitcmd, cli_err_summon_cannot_evolve, 1);
		}
		pre_summon_tm = get_summon_tm_by_one_type(p, 1001);
		if (pre_summon_tm) {
			return send_header_to_player(p, p->waitcmd, cli_err_summon_cannot_evolve, 1);
		}
	}
    if (swap_id == 1141 && !can_get_summon_item(p)) {
        return send_header_to_player(p, p->waitcmd, cli_err_not_in_ranking_list, 1);
    }

    if (swap_id == 1197 && 
        ( !is_player_team_in_top100(p) || !is_active_time_member(p) ) ) {
        return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
    }

    if (swap_id == 1198 ) { 
        if ( p->team_info.active_score <= 100 || is_player_team_in_top100(p) || !is_active_time_member(p) ) {
            return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
        }
    }

	uint32_t err_ret = judge_if_can_get_dragon_reward(p, swap_id);
	if (err_ret) {
		return send_header_to_player(p, p->waitcmd, err_ret, 1);
	}
	
	global_limit_data_t* p_limit_data = g_limit_data_mrg.get_limit_data(swap_id);
	if (p_limit_data && p_limit_data->left_cnt == 0) {
        if (swap_id >= 1486 && swap_id <= 1489) {
            return send_header_to_player(p, p->waitcmd, cli_err_celebration_goods_full, 1);
        } else {
            return send_header_to_player(p, p->waitcmd, cli_err_action_global_limited, 1);
        }
	}
	KDEBUG_LOG(p->id, "ADD ACTION CNT\t[id=%u id=%u limit=%u]", p->id, swap_id, 
		g_swap_action_mrg.get_top_limit(swap_id));
	//return db_add_action_count(p, &(g_all_restr[i]));

    //  master prentice reward
    if (swap_id >= 1212 && swap_id <= 1215) {
        if (!is_player_have_master(p)) {
            return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
        }
    }
    if (swap_id >= 1217 && swap_id <= 1218) {
        if (!is_can_get_prentice_reward(p, ex_field_1, ex_field_2, swap_id)) {
            return send_header_to_player(p, p->waitcmd, cli_err_swap_action_illegal, 1);
        }
        db_master_set_prentice_grade(p, ex_field_1, ex_field_2, swap_id);
    }

	if (swap_id >= 1187 && swap_id <= 1191) {
		if (!judge_if_can_get_fight_team_reward(p, swap_id)) {
			return send_header_to_player(p, p->waitcmd, cli_err_cannot_take_gift_taotai_game, 1);
		}
	}

    if (swap_id >= 1260 && swap_id <= 1267) {
        if (!judge_if_can_get_swap_fumo_attire(p, swap_id)) {
            return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
        }
    }

    if (swap_id == 1224) {
        if (!is_player_can_get_reward(p, ex_field_1)) {
            return send_header_to_player(p, p->waitcmd, cli_err_cannot_take_gift_taotai_game, 1);
        }
        db_set_player_reward_flag(p, ex_field_1);
    }
	if (swap_id == 1311) {
		char qq_str[128] = "";
		sprintf(qq_str, "%u%08u", ex_field_1, ex_field_2);
		WARN_LOG("QQ %u %s", p->id, qq_str);
	}

    swap_action_data res;
	swap_action_data res_mail;

    if (pack_swap_action_item(p, swap_id, p->role_type, &res, &res_mail)) {
		if (p_limit_data) {
			g_limit_data_mrg.reduce_item_cnt(swap_id, 1, true);
		}
		if (swap_id == 1232) {
			db_add_swap_action_cnt(0, ex_field_1, 0, 1231);
		}
		res.ex_field_1 = ex_field_1;
		res.ex_field_2 = ex_field_2;
		TRACE_LOG("cws again ex_field_1 is %d, ex_field_2 is %d",res.ex_field_1,res.ex_field_2 );
	    return db_swap_action(p, &res, &res_mail);
    }
	return 0;
}

int do_get_swap_action_detail_info(player_t* p, uint32_t swap_id)
{
    PlayerSwapActionDataMap::iterator it = p->m_swap_action_data_maps->find(swap_id);
	player_swap_action_data* p_my_data = 0;
	if (it != p->m_swap_action_data_maps->end()) {
		p_my_data = &(it->second);
	}
	swap_action_data* p_data = g_swap_action_mrg.get_swap_action(swap_id);
	if (!p_data) {
		return 0;
	}

    uint32_t limit_times = p_data->top_limit;
    switch (swap_id) {
        case 1032:
            limit_times = p->act_record[act_record_pos]; 
            break;
        case 1031:
            limit_times = p->act_record[act_lefanfan_pos] * 5 + (is_vip_player(p) ? 5 : 2);
            break;
        case 1087:
            limit_times = limit_times > 17 ? 17 : 0;
            break;
        case 1448:
            limit_times = get_swap_action_times(p, 1447) + 1 + (p->act_record[act_record_pos] ? 15 : 0);
            break;
        case 1466:
            limit_times = get_swap_action_times(p, 1465);
            break;
	//added by cws 0612
	 case 1491:
            limit_times = get_swap_action_times(p, 1490);
            break;
	case 1507:
            limit_times = get_swap_action_times(p, 1506);
            break;
    }

    uint32_t done_times = 0;
    if (p_my_data) {
        done_times = p_my_data->count;
    }
    
    int idx = sizeof(cli_proto_t);
    pack(pkgbuf, swap_id, idx);
    pack(pkgbuf, limit_times, idx);
    pack(pkgbuf, done_times, idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);

}

int get_swap_action_detail_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
    uint32_t swap_id = 0;
    unpack(body, swap_id, idx);

	idx = 0;
	pack_h(p->session, swap_id, idx);
	return db_get_swap_action(p);
}

int set_app_buff_end_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    send_appearance_buf(p, 0);
    return send_header_to_player(p, p->waitcmd, 0, 1);
}


void judge_nimbus_type(uint32_t nimbus, uint32_t& type1, uint32_t& type2, uint32_t& type3)
{
	if (nimbus >= 1000 && nimbus < 3000) {
		type1 ++;
	} else if (nimbus >= 3000 && nimbus < 6000) {
		type2 ++;
	} else if (nimbus >= 6000) {
		type3 ++;
	}
}

uint32_t get_summon_dragon_nimbus()
{
	uint32_t condition_type1 = 0;
	uint32_t condition_type2 = 0;
	uint32_t condition_type3 = 0;

	uint32_t tmp = g_limit_data_mrg.get_item_cnt(100000001);
	judge_nimbus_type(tmp, condition_type1, condition_type2, condition_type3);

	tmp = g_limit_data_mrg.get_item_cnt(100000002);
	judge_nimbus_type(tmp, condition_type1, condition_type2, condition_type3);

	tmp = g_limit_data_mrg.get_item_cnt(100000003);
	judge_nimbus_type(tmp, condition_type1, condition_type2, condition_type3);

	tmp = g_limit_data_mrg.get_item_cnt(100000004);
	judge_nimbus_type(tmp, condition_type1, condition_type2, condition_type3);

	tmp = g_limit_data_mrg.get_item_cnt(100000005);
	judge_nimbus_type(tmp, condition_type1, condition_type2, condition_type3);

	if ((condition_type1 + condition_type2 + condition_type3) < 3) {
		return 0;
	}
	if (condition_type3 >= 3) {
		return 6000;
	}
	if (condition_type2 + condition_type3 >= 3) {
		return 3000;
	}
	if (condition_type1 + condition_type2 + condition_type3 >= 3) {
		return 1000;
	}
	return 0;
}


uint32_t reduce_dragon_numbus_by_order(uint32_t top_order, uint32_t reduce_cnt)
{
	g_limit_data_mrg.order_datas();

	uint32_t tmp_cnt = 0;
	
	if (g_limit_data_mrg.reduce_item_cnt_with_order(100000001, reduce_cnt, 3, true)) {
		tmp_cnt++;
		if (tmp_cnt >= 3) {
			return 0;
		}
	}
	if (g_limit_data_mrg.reduce_item_cnt_with_order(100000002, reduce_cnt, 3, true)) {
		tmp_cnt++;
		if (tmp_cnt >= 3) {
			return 0;
		}
	}
	if (g_limit_data_mrg.reduce_item_cnt_with_order(100000003, reduce_cnt, 3, true)) {
		tmp_cnt++;
		if (tmp_cnt >= 3) {
			return 0;
		}
	}
	if (g_limit_data_mrg.reduce_item_cnt_with_order(100000004, reduce_cnt, 3, true)) {
		tmp_cnt++;
		if (tmp_cnt >=3) {
			return 0;
		}
	}
	if (g_limit_data_mrg.reduce_item_cnt_with_order(100000005, reduce_cnt, 3, true)) {
		tmp_cnt++;
		if (tmp_cnt >= 3) {
			return 0;
		}
	}
	
	return 0;
}

uint32_t reduce_all_dragon_nimbus()
{
	uint32_t nimbus = get_summon_dragon_nimbus();
	reduce_dragon_numbus_by_order(3, nimbus);
	return nimbus;
}

int db_add_summon_dragon_list(player_t* p)
{
	int idx = 0;
	pack_h(dbpkgbuf, p->id, idx);
	pack(dbpkgbuf, p->nick, sizeof(p->nick), idx);
	pack_h(dbpkgbuf, get_summon_dragon_nimbus(), idx);
	pack_h(dbpkgbuf, (uint32_t)(get_now_tv()->tv_sec), idx);
	
	return send_request_to_db(0,  p->id,  p->role_tm, dbproto_add_summon_dragon, dbpkgbuf, idx);	
}

int send_summon_dragon_to_world(player_t *p)
{
    char words[max_trade_mcast_size] = {0};
    int len = sprintf(words, "%s", char_content[4].content);
    TRACE_LOG("mcast title msg[%s][%d]", words, len);

    player_mcast_t info = {0};
    memset(&info, 0x00, sizeof(player_mcast_t)); 
    info.type = 2; //:player_market 2:player_world
    info.front_id = 2;
    memcpy(&(info.nick), p->nick, sizeof(p->nick));
    memcpy(&(info.info), words, max_trade_mcast_size);

    notify_player_mcast_info(p, mcast_trade_message, &info);
    return 0;
}

int summon_dragon_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	struct tm tm_tmp = *get_now_tm();
	if (tm_tmp.tm_hour < 12 || tm_tmp.tm_hour > 21) {
		return send_header_to_player(p, p->waitcmd, cli_err_cannot_summoned_dragon_err_time, 1);
	}
	if (!(p->my_packs->is_item_exist(1500564)) ||
		!(p->my_packs->is_item_exist(1500565)) ||
		!(p->my_packs->is_item_exist(1500566)) ||
		!(p->my_packs->is_item_exist(1500567)) ||
		!(p->my_packs->is_item_exist(1500568))) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
	uint32_t reduce_nimbus = get_summon_dragon_nimbus();
	if (!reduce_nimbus) {
		return send_header_to_player(p, p->waitcmd, cli_err_cannot_summoned_dragon, 1);
	}
	
	if (g_limit_data_mrg.get_item_cnt(100000010)) {
		return send_header_to_player(p, p->waitcmd, cli_err_have_summoned_dragon, 1);
	}
	db_add_summon_dragon_list(p);
	
	reduce_all_dragon_nimbus();
	g_limit_data_mrg.add_item_cnt(100000010, reduce_nimbus, true);
	g_limit_data_mrg.add_item_cnt(100000011, p->id, true);
	send_header_to_player(p, p->waitcmd, 0, 1);

	send_summon_dragon_to_world(p);

	add_swap_action_times(p, 1196);
	
	db_del_item_elem_t item_arr[5] = { {1500564, 1}, {1500565, 1}, {1500566, 1}, {1500567, 1}, {1500568, 1}};
	player_del_items(p, item_arr, 5);
	return 0;
}

int db_get_summon_dragon_list(player_t* p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_summon_dragon_list, 0, 0);
}

int db_get_summon_dragon_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);

	summon_dragon_list_rsp* rsp = reinterpret_cast<summon_dragon_list_rsp*>(body);

	CHECK_VAL_EQ(bodylen, sizeof(summon_dragon_list_rsp) + rsp->count * sizeof(summon_dragon_elem_t));

	g_summon_dragon_list.update_list(rsp);

	int idx = sizeof(cli_proto_t);
	idx += g_summon_dragon_list.pack_list(pkgbuf + idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int get_summon_dragon_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (g_summon_dragon_list.if_need_update()) {
		return db_get_summon_dragon_list(p);
	}
	int idx = sizeof(cli_proto_t);
	idx += g_summon_dragon_list.pack_list(pkgbuf + idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

bool Summon_dragon_list_mrg::if_need_update() 
{
	return (update_tm_ + 3 < get_now_tv()->tv_sec); 
}

//---------------------------------------------------------------------
/**
 *
 */
int load_strengthen_material(xmlNodePtr cur)
{
    // load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Material"))) {
			//struct strengthen_t* p_method = reinterpret_cast<struct method_t*>(malloc(sizeof(struct method_t)));
			//memset(p_method, 0x00, sizeof(struct method_t));
            struct strengthen_t material_obj = {0};
			get_xml_prop(material_obj.id, cur, "ID");
            const GfItem* itm = items->get_item(material_obj.id);
            if (!itm) {
                ERROR_LOG("Material ID:[%u] not exist in items.xml",material_obj.id);
                throw XmlParseError(std::string("Material ID not exist in item.xml"));
                return -1;
            }
            if (get_xml_prop_arr_def(material_obj.quality, cur, "Quality", 0) != 2) {
                ERROR_LOG("invalid material quality in parsing strengthenMaterial: ID=%u", material_obj.id);
                return -1;
            }
            if (get_xml_prop_arr_def(material_obj.attirelv, cur, "StrengthenLevel", 0) != 2) {
                ERROR_LOG("invalid material level in parsing strengthenMaterial: ID=%u", material_obj.id);
                return -1;
            }
            get_xml_prop_def(material_obj.lucky, cur, "Lucky", 0);
            get_xml_prop_def(material_obj.type, cur, "Type", 1);
            if (get_xml_prop_arr_def(material_obj.odds, cur, "Probability", 0) != 13) {
                ERROR_LOG("invalid material odds in parsing strengthenMaterial: ID=%u", material_obj.id);
                return -1;
            }

            TRACE_LOG("StrengthenMaterial:[%u [%u %u] [%u %u] %u]",material_obj.id,
                material_obj.quality[0],material_obj.quality[1],material_obj.attirelv[0],material_obj.attirelv[1],
                material_obj.lucky);
            //material_map.insert(std::map<uint32_t, strengthen_t>::value_type(material_obj.id, material_obj));
            material_map[material_obj.id] = material_obj;
        } else if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("ResetLevelRate"))) {
        	if (get_xml_prop_arr_def(reset_level_rate, cur, "RateArray", 0) != 12) {
                ERROR_LOG("rate array error");
            }
			TRACE_LOG("reset lv:%u %u %u %u %u %u %u %u %u %u %u %u", reset_level_rate[0], reset_level_rate[1]
				, reset_level_rate[2], reset_level_rate[3], reset_level_rate[4], reset_level_rate[5]
				, reset_level_rate[6], reset_level_rate[7], reset_level_rate[8], reset_level_rate[9]
				, reset_level_rate[10], reset_level_rate[11]);
			
        } else if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("CutLevelRate"))) {
            if (get_xml_prop_arr_def(scut_level_rate, cur, "RateArray", 0) != 12) {
                ERROR_LOG("level cut rate array error");
            }
        }
		cur = cur->next;
	}
	return 0;
}
/**
  * @brief load product configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_an_product(xmlNodePtr cur, struct method_t* p_method)
{
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t nor_item_j = 0;
	int total_odds[5] = {0};
	while (cur) 
	{
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("InItem"))) 
		{
			if (i >= max_product_need_item) {
				ERROR_LOG("productid[%u] method[0]", p_method->id);
				return -1;
			}
			get_xml_prop(p_method->product_item[i].item_id, cur, "ID");
			get_xml_prop(p_method->product_item[i].cnt, cur, "Cnt");
			if (!p_method->product_item[i].item_id ||
				!p_method->product_item[i].cnt) {
				ERROR_LOG("itemid[%u] cnt[%u]", p_method->product_item[i].item_id, p_method->product_item[i].cnt);
				return -1;	
			}
			const GfItem* itm = items->get_item(p_method->product_item[i].item_id);
			if (!itm) {
				ERROR_LOG("product id:[%u] not exist in item.xml", p_method->product_item[i].item_id);
				return -1;
			}
			i++;
		}
		else if(!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("OutItem")))
		{
			
			uint32_t item_id  = 0;
			get_xml_prop(item_id, cur, "ID");
			const GfItem* clothes = items->get_item(item_id);
			if (!clothes) {
				ERROR_LOG("%u not exist", item_id);
			}
			if (items->is_clothes(clothes->category())) {
				j = 10 + clothes->quality_lv;
			} else {
				j = nor_item_j;
				nor_item_j ++;
			}
			if( j >= max_output_compose_attire || p_method->method_need_item[j].item_id)
			{
				ERROR_LOG("productid[%u] method[0] %u %u ", p_method->id, clothes->quality_lv, p_method->method_need_item[j].item_id);
				return -1;
			}

			
			get_xml_prop(p_method->method_need_item[j].item_id, cur, "ID");
			p_method->method_need_item[j].cnt = 1;
			uint32_t  rate_array[max_ironsmith_lv];
			get_xml_prop_arr_def(rate_array, cur, "Odds", 0);

			for(uint32_t m =0; m < max_ironsmith_lv; m++)
			{
				uint32_t odds = rate_array[m];
				p_method->method_need_item[j].begin_odds[m] = total_odds[m];
				p_method->method_need_item[j].end_odds[m] = total_odds[m] + odds;
				p_method->method_need_item[j].odds[m] = odds;
				total_odds[m] += odds;
			}
		
			const GfItem* itm = items->get_item(p_method->method_need_item[j].item_id);
			if(!itm)
			{
				ERROR_LOG("product id:[%u] not exist in compose.xml", p_method->method_need_item[j].item_id);
				return -1;
			}
		}
		cur = cur->next;
	}
	if (i == 0) {
		ERROR_LOG("productid[%u] item[0]", p_method->id);
		return -1;
	}
	p_method->in_cnt = i;
	p_method->out_cnt = j;
	return 0;
}


/**
  * @brief load compose configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_compose(xmlNodePtr cur)
{
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Method"))) {
			struct method_t* p_method = reinterpret_cast<struct method_t*>(malloc(sizeof(struct method_t)));
			memset(p_method, 0x00, sizeof(struct method_t));
			get_xml_prop(p_method->id, cur, "MethodID");
			//get_xml_prop(p_method->attire_id, cur, "ProductID");
			get_xml_prop_def(p_method->coins, cur, "Coins", 0);
			get_xml_prop_def(p_method->need_vitality_point, cur, "NeedVitality", 0);
			get_xml_prop_def(p_method->need_pro_lv, cur, "NeedSecondProLv", 0);
			get_xml_prop_def(p_method->add_pro_exp, cur, "SecondProExp", 0);
			get_xml_prop_def(p_method->broadcast, cur, "Broadcast", 0);	
			
			if (load_an_product(cur->xmlChildrenNode, p_method) == -1) {
				throw XmlParseError(std::string("fail to load_an_product"));
				return -1;
			}
			methods_map_.insert(std::map<uint32_t, struct method_t*>::value_type(p_method->id, p_method));
		}
		cur = cur->next;
	}
	return 0;
}

void clear_suit_arr()
{
	memset(suit_arr, 0x0, sizeof(suit_arr));
}


/**
  * @brief load suit step configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_an_suit(xmlNodePtr cur, struct suit_t* p_suit)
{
	uint32_t i = 0;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Step"))) {
			if (i >= max_suit_step) {
				ERROR_LOG("suit step id[%u] steps too many", p_suit->id);
				return -1;
			}
			uint32_t step_cnt = 0;
			get_xml_prop(step_cnt, cur, "Cnt");
			if (step_cnt > max_suit_step) {
				ERROR_LOG("itemid[%u]", p_suit->id);
				return -1;	
			}
			suit_step_t* p_step = &(p_suit->suit_step[step_cnt]);
			p_step->attire_num = step_cnt;
			//base attr
			get_xml_prop_def(p_step->strength, cur, "Strength", 0);
			get_xml_prop_def(p_step->agility, cur, "Agility", 0);
			get_xml_prop_def(p_step->body_quality, cur, "BodyQuality", 0);
			get_xml_prop_def(p_step->stamina, cur, "Stamina", 0);
			//attr level 2
			get_xml_prop_def(p_step->atk, cur, "Atk", 0);
			get_xml_prop_def(p_step->def, cur, "Def", 0);
			get_xml_prop_def(p_step->hit, cur, "Hit", 0);
			get_xml_prop_def(p_step->dodge, cur, "Dodge", 0);
			get_xml_prop_def(p_step->crit, cur, "Crit", 0);
			get_xml_prop_def(p_step->hp, cur, "Hp", 0);
			get_xml_prop_def(p_step->mp, cur, "Mp", 0);
			get_xml_prop_def(p_step->add_hp, cur, "AddHp", 0);
			get_xml_prop_def(p_step->add_mp, cur, "AddMp", 0);
			get_xml_prop_def(p_step->skill_atk, cur, "SkillAtk", 0);
	
			TRACE_LOG("suit id %u [%u %u %u %u %u %u %u %u %u %u %u %u %u]", 
				p_suit->id, 
				p_step->attire_num, 
				p_step->strength, 
				p_step->agility, 
				p_step->body_quality, 
				p_step->stamina, 
				p_step->atk, 
				p_step->def, 
				p_step->hit, 
				p_step->dodge, 
				p_step->crit, 
				p_step->add_hp, 
				p_step->add_mp, 
				p_step->skill_atk);
			
			i++;
		}
		cur = cur->next;
	}
	if (i == 0) {
		ERROR_LOG("suit[%u] step is 0", p_suit->id);
		return -1;
	}
	p_suit->cnt = i;
	return 0;
}

/**
  * @brief load suit configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_suit(xmlNodePtr cur)
{
	clear_suit_arr();
		
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Item"))) {
			uint32_t id = 0;
			get_xml_prop(id, cur, "ID");
			if (id > max_suit_id) {
				ERROR_LOG("suit id:[%u] max[%u]", id, max_suit_id);
				throw XmlParseError(std::string("suit id invaild"));
			}
			suit_t* p_suit = suit_arr + id;
			p_suit->id = id;
			// load configs for each step
			if (load_an_suit(cur->xmlChildrenNode, p_suit) == -1) {
				throw XmlParseError(std::string("failed to load an suit"));
			}
		}
		cur = cur->next;
	}
	return 0;
}

int check_item_suit(const GfItem& itm, void* in)
{
	uint32_t set_id = itm.suit_id();
	
	if (set_id > 0) {
		if (set_id > max_suit_id || !(suit_arr[set_id].id)) {
			ERROR_LOG("suit id not found: %u %u ", itm.id(), set_id);
			return -1;
		}
	}
	TRACE_LOG("<item id=\"%u\" name=\"%s\" />", itm.id(), itm.get_name());
	return 0;
}

/**
  * @brief load unique items from an xml file
  * @return 0 on success, -1 on error
  */
int load_unique_items(xmlNodePtr cur)
{
	// load unique items from xml file
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("UniqueDrop"))) {
			uint32_t id;
			uint32_t bit_pos;
			get_xml_prop(id, cur, "ID");
			get_xml_prop(bit_pos, cur, "Bit_Pos");

			if (unique_item_map.size() > 100) {
				ERROR_LOG("unique items to many %u ", (uint32_t)unique_item_map.size());
				return -1;
			}
			if (unique_item_map.find(id) != unique_item_map.end()) {
				ERROR_LOG("id:%u  defined twice", id);
				return -1;
			}
			unique_item_pos_t item(id, bit_pos);
			unique_item_map.insert(std::map<uint32_t, unique_item_pos_t>::value_type(id, item));
			TRACE_LOG("unique_item_map: %u %u %u", id, item.item_id, item.bit_pos);
		}
		cur = cur->next;
	}
	return 0;
}

int db_add_item_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret)
{
	struct add_item_rsp_t {
		uint32_t error_num;
		uint32_t type;
		uint32_t item_id;
		uint32_t add_info;
	} __attribute__((packed));


	add_item_rsp_t * pkg = reinterpret_cast<add_item_rsp_t *>(body);

	if (pkg->error_num) {
		return send_header_to_player(p, p->waitcmd, pkg->error_num, 0);
	}

	if (pkg->type == 1) { //装备
		p->my_packs->add_clothes(p, pkg->item_id, pkg->add_info, 0, channel_string_other);
		return 0;
	} else if (pkg->type == 2){ //普通物品
		p->my_packs->add_item(p, pkg->item_id, pkg->add_info, channel_string_other);

		if (p->waitcmd == cli_proto_player_gain_item) { //天下比武大会领取复活草
			set_bit_on(p->unique_item_bit, 68);
			db_set_unique_item_bit(p);				
			int idx = sizeof(cli_proto_t);
			pack(pkgbuf, p->id, idx);
			pack(pkgbuf, pkg->item_id, idx);
			pack(pkgbuf, pkg->add_info, idx);
			init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
			return send_to_player(p, pkgbuf, idx, 1);
		}

		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, p->id, idx);
		pack(pkgbuf, pkg->item_id, idx);
		pack(pkgbuf, pkg->add_info, idx);
		init_cli_proto_head_full(pkgbuf, p->id, p->seqno, cli_proto_player_gain_item, idx, 0);
		//ERROR_LOG("ADD ITETM %u %u %u", p->id, pkg->item_id, pkg->add_info);
		return send_to_player(p, pkgbuf, idx, 0);
	}
    return 0;
}

void load_items_upgrade_elem(xmlNodePtr cur, items_upgrade_elem_t* p_data, const char* cmpstr, uint32_t* p_count)
{
	if(cur)
	{
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>(cmpstr))) {
			xmlNodePtr chl_cur = cur->xmlChildrenNode;
			while (chl_cur) {
				if (*p_count >= items_upgrade_elem_max_cnt) {
					printf("%u\n", *p_count);
					throw XmlParseError(std::string("the item upgrade xml file *p_count >= items_upgrade_elem_max_cnt"));
					return ;
				}
				get_xml_prop_def(p_data->type, chl_cur, "type", 0);
				if (p_data->type == 2) {
					get_xml_prop_arr_def(p_data->id, chl_cur, "id", 0);
				} else {
					get_xml_prop_def(p_data->id[0], chl_cur, "id", 0);
				}
				get_xml_prop_def(p_data->count, chl_cur, "count", 0);
				
				if (p_data->type == 1) {
					const GfItem* clothes = items->get_item(p_data->id[0]);
					if (!clothes || p_data->count != 1 || p_data->id[1]) {
						throw XmlParseError(std::string("items_upgrade clothes not exist"));
						return ;
					}
				}
				if (p_data->type == 2) {
					const GfItem* itm1 = items->get_item(p_data->id[0]);
					const GfItem* itm2 = items->get_item(p_data->id[1]);
					if ((p_data->id[0] && !itm1) || (p_data->id[1] && !itm2)) {
						throw XmlParseError(std::string("items_upgrade item not exist"));
						return ;
					}
				}
				
				TRACE_LOG("%u %u %u %u %u", p_data->type, p_data->id[0], p_data->id[1], p_data->count, *p_count);
				(*p_count) ++;
				p_data++;
				chl_cur = chl_cur->next;
			}
		}
	}
}

void items_upgrade_mrg::init(const char * xml_file)
{
	if(xml_file == NULL)return;
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr master_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml_file);
	if(doc == NULL){
		throw XmlParseError(std::string("the item upgrade xml file is not exist"));
		return;
	}
	
	root = xmlDocGetRootElement(doc);
    if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the item upgrade file content is empty"));
		return;						    
	}

	master_node = root->xmlChildrenNode;
	while(master_node)
	{
		if (!xmlStrcmp(master_node->name, reinterpret_cast<const xmlChar*>("Index"))) {
			items_upgrade_data data;
			get_xml_prop_def(data.id, master_node, "ID", 0);
			get_xml_prop_def(data.odds, master_node, "Odds", 0);
			get_xml_prop_def(data.vip_lv, master_node, "VipLv", 1);

			xmlNodePtr chl_node = master_node->xmlChildrenNode;
			while(chl_node) {
				load_items_upgrade_elem(chl_node, data.source_data, "SourceItems", &(data.source_cnt));
				TRACE_LOG("SourceItems %u", data.source_cnt);
				load_items_upgrade_elem(chl_node, data.material_data, "MaterialItems", &(data.material_cnt));
				TRACE_LOG("SourceItems %u", data.material_cnt);
				load_items_upgrade_elem(chl_node, data.dest_data, "DestItems", &(data.dest_cnt));
				TRACE_LOG("SourceItems %u", data.dest_cnt);
				chl_node = chl_node->next;
			}
			items_upgrade_data_map.insert(ItemsUpgradeDataMap::value_type(data.id, data));
		}
		master_node = master_node->next;
	}
	TRACE_LOG("SourceItems end");
	return;	

}
enum need_type {
	need_rank_range = 1,
	need_active_score = 2,
	need_attr_other  = 3,
};

bool load_swap_action_reward_cost(xmlNodePtr cur, swap_action_data* p_data)
{
	uint32_t cnt1 = 0, cnt2 = 0;

	while (cur) {

		if ((!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("Needs")))) {
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if ((!xmlStrcmp(chl->name, reinterpret_cast<const xmlChar *>("Need")))) {
					get_xml_prop(p_data->needs.need_type, chl, "need_type");
					get_xml_prop(p_data->needs.active_id, chl, "active_id");
					if (p_data->needs.need_type == need_rank_range) {
						int arr_len = get_xml_prop_arr(p_data->needs.range, chl, "range");
						if (arr_len != 2) {
							ERROR_LOG("FATA swap Need range %u ", p_data->id);
							return false;
						}
					} else {
						get_xml_prop(p_data->needs.need_cnt, chl, "need_cnt");
					}
				} 
				chl = chl->next;
			}
		}

		if ((!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("Costs")))) {
            get_xml_prop_def(p_data->cost_odds, cur, "OddsFlg", 0);
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if ((!xmlStrcmp(chl->name, reinterpret_cast<const xmlChar *>("Cost")))) {
					get_xml_prop(p_data->costs[cnt1].give_type, chl, "give_type");
					get_xml_prop(p_data->costs[cnt1].give_id, chl, "give_id");
					get_xml_prop(p_data->costs[cnt1].count, chl, "count");
                    get_xml_prop_def(p_data->costs[cnt1].odds, chl, "Odds", 0); 
					TRACE_LOG("Cost:[%u %u %u]",p_data->costs[cnt1].give_type, 
											p_data->costs[cnt1].give_id, p_data->costs[cnt1].count);
					cnt1++;
				} 
				chl = chl->next;
			}
		}

		if ((!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("Rewards")))) {
			get_xml_prop_def(p_data->odds_flg, cur, "OddsFlg", 0);
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if ((!xmlStrcmp(chl->name, reinterpret_cast<const xmlChar *>("Reward")))) {
					get_xml_prop(p_data->rewards[cnt2].give_type, chl, "give_type");
                    get_xml_prop_raw_str_def(p_data->rewards[cnt2].name, chl, "Name", "神秘物品");
					get_xml_prop_def(p_data->rewards[cnt2].role_type, chl, "role_type", 0);
					get_xml_prop(p_data->rewards[cnt2].give_id, chl, "give_id");
                    if ( p_data->rewards[cnt2].give_type == 1 ) {
                        const GfItem* itm = items->get_item(p_data->rewards[cnt2].give_id);
                        if ( !itm || !items->is_clothes(itm->category()) ||  p_data->rewards[cnt2].give_id < 10000) {
                            ERROR_LOG("dailyAction.xml parameter err.[give_type=1 - give_id=%u]",
                                p_data->rewards[cnt2].give_id);
                            return false;
                        }
                    } else if ( p_data->rewards[cnt2].give_type == 2 ) {
                        const GfItem* itm = items->get_item(p_data->rewards[cnt2].give_id);
                        if ( !itm || items->is_clothes(itm->category()) ||  p_data->rewards[cnt2].give_id < 10000) {
                            ERROR_LOG("dailyAction.xml parameter err.[give_type=2 - give_id=%u]",
                                p_data->rewards[cnt2].give_id);
                            return false;
                        }
                    } else if ( p_data->rewards[cnt2].give_type == 3 ) {
                        if ( p_data->rewards[cnt2].give_id > 100000 ) {
                            ERROR_LOG("dailyAction.xml parameter err.[give_type=3 - give_id=%u]",
                                p_data->rewards[cnt2].give_id);
                            return false;
                        }
						if (p_data->rewards[cnt2].give_id == 2 && p_data->top_limit == 0) {
							ERROR_LOG("swap action unlimit %u coins %u",  p_data->id, p_data->rewards[cnt2].count);
						}
                    }
					get_xml_prop(p_data->rewards[cnt2].count, chl, "count");
					get_xml_prop_def(p_data->rewards[cnt2].odds, chl, "Odds", 0);
					get_xml_prop_def(p_data->rewards[cnt2].is_bcast, chl, "Bcast", 0);
					TRACE_LOG("Reward:[%u %u %u %u %u]",p_data->rewards[cnt2].give_type, 
                       p_data->rewards[cnt2].role_type,p_data->rewards[cnt2].give_id, 
                       p_data->rewards[cnt2].count, p_data->rewards[cnt2].odds);

					if ( p_data->rewards[cnt2].give_type == 3 && 
						 p_data->rewards[cnt2].give_id == 2 && 
						 p_data->top_limit == 0) {
						 ERROR_LOG("swap action unlimit %u coins %u",  p_data->id, p_data->rewards[cnt2].count);
                    }
                    //ERROR_LOG("Load: [%u] [%s]", p_data->rewards[cnt2].give_id, p_data->rewards[cnt2].name);
					cnt2++;
				}
				chl = chl->next;
			}
		}
	
		cur = cur->next;
	} //end while(cur)

	p_data->cost_count = cnt1;
	p_data->rew_count = cnt2;
	TRACE_LOG("restr cnt[%u %u]",p_data->cost_count, p_data->rew_count);
	return true;

}

bool can_reward_active(player_t * p, swap_other_ranker_t & t) 
{
	enum need_other_attr_t {
		need_attr_achieve_point = 1,
	};
	uint32_t user_info = 0;
	if (t.need_type == need_ranker_score) {
		user_info = get_player_other_info_value(p, t.active_id);
	} else if (t.need_type == need_ranker_range) {
		user_info = get_player_ranker_range(p, t.active_id);
	} else if (t.need_type == need_other_attr) {
		if (t.active_id == need_attr_achieve_point) {
			if (t.need_cnt <= p->achieve_point) {
				return true;
			}
		}
		return false;
	}


	if (user_info <= t.range[1] && user_info >= t.range[0]) {
		return true;
	}

	return false;
}

bool swap_action_mrg::check_limit(player_t* p, uint32_t id)
{
	PlayerSwapActionDataMap::iterator it = p->m_swap_action_data_maps->find(id);
	player_swap_action_data* p_my_data = 0;
	if (it != p->m_swap_action_data_maps->end()) {
		p_my_data = &(it->second);
	}
	swap_action_data* p_data = get_swap_action(id);
	if (!p_data) {
		return true;
	}

	if (p_data->start_end_time[0] && get_now_tv()->tv_sec < p_data->start_end_time[0]) {
		send_header_to_player(p, p->waitcmd, cli_err_swap_action_not_in_time, 1);
		return true;
	}
	if (p_data->start_end_time[1] && get_now_tv()->tv_sec > p_data->start_end_time[1]) {
		send_header_to_player(p, p->waitcmd, cli_err_swap_action_not_in_time, 1);
		return true;
	}

	if (p_data->time_range_id && !get_now_active_data_by_active_id(p_data->time_range_id)) {
		send_header_to_player(p, p->waitcmd, cli_err_swap_action_not_in_time, 1);
		return true;
	}	
		
	if (p_data->user_lv[0] > p->lv) {
		send_header_to_player(p, p->waitcmd, cli_err_less_lv, 1);
		return true;
	}
	if (p_data->user_lv[1] < p->lv) {
		send_header_to_player(p, p->waitcmd, cli_err_large_lv, 1);
		return true;
	}

	const struct tm* cur_tm = get_now_tm();
    //week
    if (p_data->week_limit[cur_tm->tm_wday] == 1) {
		send_header_to_player(p, p->waitcmd, cli_err_swap_action_not_in_time, 1);
		return true;
    }

    //hour
	uint32_t hour = static_cast<uint32_t>(cur_tm->tm_hour);
	if ((hour < p_data->tm_range[0]) || (hour >= p_data->tm_range[1])) {
		send_header_to_player(p, p->waitcmd, cli_err_swap_action_not_in_time, 1);
		return true;
	}
	
	if ( p_data->year_vip && (!is_vip_year_player(p)) ) {
		send_header_to_player(p, p->waitcmd, cli_err_vipyear_exclusive_right, 1);
		return true;
	}
	if (p_data->vip && (p_data->vip > p->vip_level || !(is_vip_player(p)))) { 
		send_header_to_player(p, p->waitcmd, cli_err_nonvip_operation_forbidden, 1);
		return true;
	} 

	if (p_data->cli_type == swap_action_card_set) {
		uint32_t set = get_set_id_by_swap_id(id);
		if (!can_reward_card_set(p, set)) {
			return true;
		}
	}

	if (p_data->cli_type == swap_active_player_reward || p_data->cli_type == swap_achieve_need_reward) {
		if (!can_reward_active(p, p_data->needs)) {
			return true;
		}
	}

	if (id == 1174) {

		if (!p->player_show_state || ((int)p->player_show_state != get_city_win_team())) {
			return true;
		}
	}


	if (id == 1032) {
		if (p_my_data) {
			if (p_my_data->count >= p->act_record[act_record_pos]) {
				send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
			    return true;
			}
		} else {
			if (p->act_record[act_record_pos] == 0) {
				send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
			    return true;
			}
		}
	}


	if (p_my_data && p_data->top_limit) {
        uint32_t limit_times = 0;
        if (id== 1031) {
            limit_times = p->act_record[act_lefanfan_pos] * 5 + (is_vip_player(p) ? 5 : 2);
            if (p_my_data->count >= limit_times) {
                send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
                return true;
            }
        }
        if (id >= 1090 && id <= 1109)  {
            limit_times = p->act_record[act_record_pos] + (is_vip_player(p) ? 5 : 2);
            if ( p_my_data->count > limit_times || p_my_data->count > 17) {
                send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
                return true;
            }
        }
        if (p_my_data->count >= p_data->top_limit) {
			if (p_data->type == 0) {
				send_header_to_player(p, p->waitcmd, cli_err_day_limit_time_act, 1);
				return true;
			} else if (p_data->type == 1) {
				send_header_to_player(p, p->waitcmd, cli_err_week_limit_time_act, 1);
				return true;
			} else if (p_data->type == 2) {
				send_header_to_player(p, p->waitcmd, cli_err_month_limit_time_act, 1);
				return true;
			} else if (p_data->type == 3) {
				send_header_to_player(p, p->waitcmd, cli_err_year_limit_time_act, 1);
				return true;
			} else if (p_data->type == 4) {
				send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
				return true;
			} 
		}
	}
	
	return false;
}

uint32_t swap_action_mrg::get_top_limit(uint32_t id)
{
	return 0;
}

uint32_t swap_action_mrg::get_cd_time(uint32_t id)
{
	swap_action_data* p_data = get_swap_action(id);
	if (!p_data) {
		return 0xffffffff;
	}
	return p_data->cd_time;
}

uint32_t swap_action_mrg::get_buff_id(uint32_t id)
{
	swap_action_data* p_data = get_swap_action(id);
	if (!p_data) {
		return 0;
	}
	return p_data->buff_id;
}

bool get_odds_cost_elem(swap_action_data* p_res, swap_action_elem_t * cost_elem, uint32_t odds_val)
{
    uint32_t odds = 0;
    uint32_t j = 0;
    for (uint32_t i = 0; i < p_res->cost_count; i++) {
        odds += p_res->costs[i].odds;
        if (odds >= odds_val) {
            memcpy(&(cost_elem[j++]), &(p_res->costs[i]), sizeof(swap_action_elem_t));
            return true;
        } else {
            continue;
        }
    }
    return false;
}

void swap_action_mrg::init(const char * xml_file)
{
	if(xml_file == NULL)return;
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr cur = NULL;

	swap_action_data_map.clear();
	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml_file);
	if(doc == NULL){
		throw XmlParseError(std::string("the item upgrade xml file is not exist"));
		return;
	}
	
	root = xmlDocGetRootElement(doc);
    if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the item upgrade file content is empty"));
		return;						    
	}

	

	cur = root->xmlChildrenNode; 
	while (cur) 
	{
		swap_action_data data;
		memset(&data, 0, sizeof(data));
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Restriction"))) 
		{	
			get_xml_prop(data.id, cur, "ID");	
			get_xml_prop(data.cli_type, cur, "Type");
			get_xml_prop(data.type, cur, "Restr_Flag");
			if (swap_action_data_map.find(data.id) != swap_action_data_map.end()) 
			{
				ERROR_LOG("invalid/duplicated restriction: id=%u %u", data.id, data.type);
				throw XmlParseError(std::string("error log"));
				return;
			}
			if (data.id < 1) 
			{
				ERROR_LOG("invalid/duplicated restriction: type=%u max=%u %u", data.id, max_restr_count, data.type);
				throw XmlParseError(std::string("error log"));
				return;
			}
			get_xml_prop_raw_str_def(data.name, cur, "Name", "兑换物品");
			get_xml_prop(data.type, cur, "Restr_Flag");
			get_xml_prop_def(data.top_limit, cur, "Toplimit", 999999);
			get_xml_prop_def(data.vip, cur, "Vip", 0);
			get_xml_prop_def(data.year_vip, cur, "YearVip", 0);
            if (get_xml_prop_arr_def(data.user_lv, cur, "UseLv", 0) != 2) 
			{
                ERROR_LOG("invalid appearlevel in parsing restriction UserLv: type=%u id=%u", data.type, data.id);
                throw XmlParseError(std::string("error log"));
				return;
            }
			get_xml_prop_def(data.user_flg, cur, "UserFlag", 0);
			get_xml_prop_def(data.buff_id, cur, "BuffID", 0);

            get_xml_prop_arr_def(data.week_limit, cur, "WeekLimit", 0);

			if (get_xml_prop_arr_def(data.tm_range, cur, "TimeRange", 0) != 2) {
				ERROR_LOG("invalid appeartime in parsing restriction TimeRange: type=%u", data.type);
				throw XmlParseError(std::string("error log"));
				return;
			}
			char fromstr[128] = "";
			time_t tmptime;
			get_xml_prop_raw_str_def(fromstr, cur, "From", "NULL");
			if (strcmp(fromstr, "NULL") != 0) 
			{
				if (!date2timestamp(fromstr, "%Y-%m-%d %H:%M:%S", tmptime)) 
				{
					ERROR_LOG("time format invalid[%s]", fromstr);
					throw XmlParseError(std::string("error log"));
					return;
				}
			} 
			else 
			{
				tmptime = 0;
			}
			data.start_end_time[0] = tmptime;

			get_xml_prop_raw_str_def(fromstr, cur, "To", "NULL");
			if (strcmp(fromstr, "NULL") != 0) 
			{
				if (!date2timestamp(fromstr, "%Y-%m-%d %H:%M:%S", tmptime)) 
				{
					ERROR_LOG("time format invalid[%s]", fromstr);
					throw XmlParseError(std::string("error log"));
					return;
				}
			} 
			else 
			{
				tmptime = 0;
			}
			data.start_end_time[1] = tmptime;

			get_xml_prop_def(data.time_range_id, cur, "Time_Range_Id", 0);

			get_xml_prop_def(data.cd_time, cur, "CDTime", 0);
			
            if (!load_swap_action_reward_cost(cur->xmlChildrenNode, &data)) 
			{
            	TRACE_LOG("restr:[%u %u %u %u [%u %u] %u %u %u]",data.id, data.type, 
                data.type, data.top_limit, 
                data.user_lv[0],data.user_lv[1],
                data.vip, data.user_flg, data.cost_odds);
            	if (!load_swap_action_reward_cost(cur->xmlChildrenNode, &data)) {
                	ERROR_LOG("Parse restriction costs and rewards error!");
					throw XmlParseError(std::string("error log"));
					return;
				}
			}	
		}
		swap_action_data_map.insert(SwapActionDataMap::value_type(data.id, data));
		cur = cur->next;
	}
	
	return;

}

int player_item_2_score_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	return -1;

	uint32_t active_id;
	uint32_t item_id;
	uint32_t item_cnt;
	int idx = 0;

	unpack(body, active_id, idx);
	unpack(body, item_id, idx);
	unpack(body, item_cnt, idx);
	//check is valid active_id 
	if (active_id != 1) {
		return -1;
	}
	
	//check has 
	//tmp hard code	
	uint32_t add_score = 0;
	if (item_id == 1500573 && p->my_packs->get_item_cnt(item_id) >= item_cnt) {
		add_score += item_cnt;
		p->my_packs->del_item(p, item_id, item_cnt, channel_string_other);
		db_use_item_ex(0, p->id, p->role_tm, item_id, item_cnt, false);
	} else {
		ERROR_LOG("ERROR ITEM OR CNT %u %u %u", p->id, item_id, item_cnt);
		return -1;
	}

	uint32_t new_score = get_player_other_info_value(p, active_id) + add_score;

	//save 2 db
	rank_info_t info; 
	info.userid = p->id;
	info.role_tm = p->role_tm;
	info.role_type = p->role_type;
	memcpy(info.nick_name, p->nick,  max_nick_size);
	info.score = new_score;
	
	db_insert_rank_info(p, active_id, &info);
	set_player_other_info(p, active_id, new_score);
	uint32_t max_exp = calc_exp(max_exp_lv);

	uint32_t add_exp = add_score * 500; 

	if (p->lv < max_exp_lv) {
		p->exp += add_exp;
		if (p->exp > max_exp) {
			add_exp = max_exp - p->exp;
			p->exp = max_exp;
		} 
		calc_player_level(p);
	}

	uint32_t add_coins = add_score * 100;
	p->coins += add_coins;

	db_set_role_base_info(p);	

	//send to clent;
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, active_id, idx);
	pack(pkgbuf, item_id, idx);
	pack(pkgbuf, item_cnt, idx);
	pack(pkgbuf, new_score, idx);
	pack(pkgbuf, add_exp, idx);
	pack(pkgbuf, add_coins, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int distract_check_status(pack_clothes_info_t * old_clothes, pack_clothes_info_t * new_clothes) 
{

	const GfItem * itm_old  = items->get_item(old_clothes->clothes_id);
	const GfItem * itm_new  = items->get_item(new_clothes->clothes_id);

	if (old_clothes->lv < 2) {
		return cli_err_old_clothes_lv_too_low;
	}

	if (itm_new->quality_lv != itm_old->quality_lv) {
		return cli_err_distract_quality_diff;
	}

	if (itm_new->equip_part() == 4 && itm_old->equip_part() != 4) {
		return cli_err_distract_weapon_2_clothes;
	} 

	if (itm_new->equip_part() != 4 && itm_old->equip_part() == 4) {
		return cli_err_distract_clothes_2_weapon;
	}

	if (old_clothes->lv < new_clothes->lv) {
		return cli_err_old_clothes_lv_too_low;
	}

	return 0;
}

uint32_t get_reduce_lv_by_stone_id(uint32_t stone_id)
{
	if (stone_id == 1549001) {
		return 1;
	} else if (stone_id == 1549002) {
		return 2;
	} else if (stone_id == 1549003) {
		return 3;
	}
	return 0;
}

uint32_t calc_new_clothes_lv(uint32_t stone_id, uint32_t old_lv)
{
	uint32_t reduce_lv = get_reduce_lv_by_stone_id(stone_id);
	if (old_lv + reduce_lv >  4) {
		return (old_lv + reduce_lv) - 4;
	}
	return 0;
}

uint32_t calc_disctract_coin_cost(pack_clothes_info_t * old_clothes, pack_clothes_info_t * new_clothes)
{
	const GfItem * itm_old  = items->get_item(old_clothes->clothes_id);
	const GfItem * itm_new  = items->get_item(new_clothes->clothes_id);

	uint32_t reduce_coin = old_clothes->lv * itm_new->use_lv() * itm_new->use_lv(); 
	//传承武器
	if (itm_old->equip_part() == 4) {
		reduce_coin *= 5;
	}
	//传承紫装
	if (itm_new->quality_lv == 4) {
		reduce_coin *= old_clothes->lv;
	}

	return reduce_coin;
}

uint32_t need_keep_old_clothes(uint32_t old_clothes_type)
{
	const GfItem * itm_old  = items->get_item(old_clothes_type);
	if (itm_old->is_item_vip_only()) {
		return 1;
	}
	return 0;
}



#define UNPACK_INT(buf, val, idx) \
	int val = 0; \
	unpack(buf, val, idx)

int distract_clothes_strength_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	int idx = 0;
	UNPACK_INT(body, old_clothes_id, idx);
	UNPACK_INT(body, new_clothes_id, idx);
	UNPACK_INT(body, stone_id, idx);

	pack_clothes_info_t * old_clothes =  p->my_packs->get_clothes_info_by_unique_id(old_clothes_id);
	pack_clothes_info_t * new_clothes =  p->my_packs->get_clothes_info_by_unique_id(new_clothes_id);
	if (old_clothes && new_clothes) {
		int error_no = distract_check_status(old_clothes, new_clothes);
		if (error_no) {
			return send_header_to_player(p, p->waitcmd, error_no, 1);
		}

		uint32_t reduce_coin =  calc_disctract_coin_cost(old_clothes, new_clothes);
		if (reduce_coin > p->coins) {
			return send_header_to_player(p, p->waitcmd, cli_err_no_enough_coins, 1);
		}

		if (stone_id && p->my_packs->get_item_cnt(stone_id) == 0) {
			ERROR_LOG("USER DISTRACT STRENTTH WITH NOT EXIST STONE %u", stone_id);
			return -1;
		}

		uint32_t keep_flag = need_keep_old_clothes(old_clothes->clothes_id);

		uint32_t lv = calc_new_clothes_lv(stone_id, old_clothes->lv);
		idx = 0;
		pack_h(dbpkgbuf, keep_flag, idx);
		pack_h(dbpkgbuf, old_clothes_id, idx);
		pack_h(dbpkgbuf, new_clothes_id, idx);
		pack_h(dbpkgbuf, lv, idx);
		pack_h(dbpkgbuf, stone_id, idx);
		pack_h(dbpkgbuf, reduce_coin, idx);
		return send_request_to_db(p, p->id, p->role_tm, dbproto_distract_clothes_strength, dbpkgbuf, idx);
	}

	
	ERROR_LOG("Player DISTRACT STRENGTH NOT EXIST CLOTHES UID %u %u", old_clothes_id, new_clothes_id);
	return -1;
}

int db_distract_clothes_strength_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	db_distract_clothes_strength_rsp_t * rsp = reinterpret_cast<db_distract_clothes_strength_rsp_t*>(body);

	pack_clothes_info_t * new_clothes =  p->my_packs->get_clothes_info_by_unique_id(rsp->new_clothes_id);
	if (new_clothes) {
		new_clothes->lv = rsp->new_level;
	}

	if (rsp->keep_flag) {
		pack_clothes_info_t * old_clothes = p->my_packs->get_clothes_info_by_unique_id(rsp->old_clothes_id);
		if (old_clothes) {
			KDEBUG_LOG(p->id, "SET VIP ONLY CLOTHES %u LV =0 BY DISTRACT", old_clothes->clothes_id); 
			old_clothes->lv = 0;
		}
	} else {
		p->my_packs->del_clothes(rsp->old_clothes_id, "channel distract strength");
	}

	if (p->coins >= rsp->reduce_coins) {
		p->coins -= rsp->reduce_coins;
	} else {
		p->coins = 0;
	}

	if (rsp->stone_id) {
		p->my_packs->del_item(p, rsp->stone_id, 1, "channel distract strength");
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->keep_flag, idx);
	pack(pkgbuf, rsp->old_clothes_id, idx);
	pack(pkgbuf, rsp->new_clothes_id, idx);
	pack(pkgbuf, rsp->new_level, idx);
	pack(pkgbuf, rsp->stone_id, idx);
	pack(pkgbuf, rsp->reduce_coins, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


//added by cws 0612
int present_to_online_usr(){ return 0;}
	


