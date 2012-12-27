/**
 *============================================================
 *  @file      item.hpp
 *  @brief    item related functions are declared here. (talk_cmd, ...)
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_ITEM_HPP_
#define KF_ITEM_HPP_

extern "C" {
#include <libtaomee/project/stat_agent/msglog.h>
}

#include <kf/item_impl.hpp>
#include <kf/player_attr.hpp>

#include "player.hpp"
#include "battle.hpp"
#include "fwd_decl.hpp"
#include <map>
#include "vip_impl.hpp"


struct pack_item_info_t;
struct pack_clothes_info_t;
typedef std::map<uint32_t, pack_item_info_t> 	PackItemMap;
typedef std::map<uint32_t, pack_clothes_info_t> PackClothesMap;

enum {
    get_rich_monster_id = 10190,
	reset_skills_item_id = 1703001,
	reset_skills_item_id_2 = 1500387,
	repair_clothes_item_id = 1703003,

    gold_coin_item_id = 1740029,
    get_rich_item_id = 1500577,
	
	double_exp_buff_item_id = 1703002,
	double_exp_buff_item_id_ex = 1700906,
	double_exp_buff_item_id_2 = 1700906,
	double_exp_buff_item_id_3 = 1703005,
	double_player_only_exp_buff_item_id = 1700019,
	
	double_mon_only_exp_buff_item_id = 1700020,
	double_mon_only_exp_buff_60min_item_id = 1703004,
	double_mon_only_exp_buff_60min_item_id_ex = 1700904,
	double_mon_only_exp_buff_60min_item_id_2 = 1700904,
	double_mon_only_exp_buff_30min_item_id	= 1703006,

	trade_mcast_item_id_1 = 1303015, 
	trade_mcast_item_id_2 = 1560004,
	world_notice_item_id = 1700021, 

	contest_flower_item_id = 1303014,

	player_only_get_exp_buff_daily_id = 103,
	mon_only_get_exp_buff_daily_id = 104,
	
};

 struct db_del_item_elem_t {
 	uint32_t item_id;
	uint32_t count;
};


struct product_item_t {
	uint32_t item_id;
	uint32_t cnt;
};

struct need_method_item_t{
	uint32_t item_id;
	uint32_t cnt;
	uint32_t begin_odds[10];
	uint32_t end_odds[10];
	uint32_t odds[10];
};

struct method_t {
	/*! product id */
	uint32_t id;
	/*! need coins to product this*/
	uint32_t coins;
	/*! need item cnt*/
	uint32_t in_cnt;
	uint32_t out_cnt;
	/*! need vitality point*/
	uint32_t need_vitality_point;
	/*! need second pro lv*/
	uint32_t need_pro_lv;
	/*! add  second pro exp*/
	uint32_t add_pro_exp;
	/*! need item info*/
	uint32_t broadcast;
	
	product_item_t 		product_item[max_output_compose_attire];
	need_method_item_t	method_need_item[max_product_need_item];
};

struct strengthen_t {
    /*! Material ID */
    uint32_t id;
    uint32_t quality[2];
    uint32_t attirelv[2];
    uint32_t lucky;
    uint32_t type;
    uint32_t odds[max_attire_level+1];
    uint32_t zero[max_attire_level+1];
};
struct suit_step_t {
	uint32_t attire_num;
	//base attr
	uint32_t strength;
	uint32_t agility;
	uint32_t body_quality;
	uint32_t stamina;
	//attr level 2
	uint32_t atk;
	uint32_t def;
	uint32_t hit;
	uint32_t dodge;
	uint32_t crit;
	uint32_t hp;
	uint32_t mp;
	uint32_t add_hp;
	uint32_t add_mp;
	
	uint32_t skill_atk;
};

struct suit_t {
	uint32_t id;
	uint32_t cnt;
	suit_step_t suit_step[max_suit_step + 1];
};

struct pack_clothes_info_t {
	uint32_t	clothes_id;
	uint32_t 	unique_id;
	uint32_t	duration;
    uint32_t    lv;
	uint32_t	gettime;
	uint32_t	timelag;
};

struct pack_item_info_t {
	uint32_t	item_id;
	uint32_t	item_cnt;
};

struct unique_item_pos_t {
	uint32_t	item_id;
	uint8_t		bit_pos;
	unique_item_pos_t(uint32_t id, uint32_t pos): item_id(id), bit_pos(pos) {
	}
};

struct strengthen_attire_t
{
    uint32_t    attireid;
    uint32_t    uniquekey;
    uint32_t    attirelv;
    uint32_t    material_id;
    uint32_t    material_cnt;
    uint32_t    stoneid_1;
    uint32_t    stoneid_1_cnt;
    uint32_t    stoneid_2;
};
struct db_strengthen_attire_rsp_t
{
    uint32_t    leftcoin;
    uint32_t    strengthen_cnt;//byte Alignment
    uint32_t    attireid;
    uint32_t    uniquekey;
    uint32_t    attirelv;
    uint32_t    material_id;
    uint32_t    material_cnt;
    uint32_t    stoneid_1;
    uint32_t    stone_1_cnt;
    uint32_t    stoneid_2;
};

//added by cws 0620
struct db_strengthen_without_material_rsp_t
{
    uint32_t    uniquekey;
    uint32_t    attirelv;
};

#define strengthen_material_base_1 1540000
#define strengthen_material_base_2 1740000


struct items_upgrade_elem_t {
	uint32_t type;
	uint32_t id[2];
	uint32_t count;

	items_upgrade_elem_t()
	{
		type = 0;
		id[0] = 0;
		id[1] = 0;
		count = 0;
	}
};

class items_upgrade_data {
	public:
		uint32_t id;
		uint32_t odds;
		uint32_t vip_lv;
		uint32_t source_cnt;
		items_upgrade_elem_t source_data[items_upgrade_elem_max_cnt];
		uint32_t material_cnt;
		items_upgrade_elem_t material_data[items_upgrade_elem_max_cnt];
		uint32_t dest_cnt;
		items_upgrade_elem_t dest_data[items_upgrade_elem_max_cnt];
	public:
		items_upgrade_data()
		{
			id = 0;
			odds = 0;
			source_cnt = 0;
			material_cnt = 0;
			dest_cnt = 0;
		}
		
};

typedef std::map<uint32_t, items_upgrade_data> ItemsUpgradeDataMap;

class items_upgrade_mrg {
	public:
		items_upgrade_data* get_upgrade_data(uint32_t id)
		{
			ItemsUpgradeDataMap::iterator it = items_upgrade_data_map.find(id);
			if (it == items_upgrade_data_map.end()) {
				return 0;
			}
			return &(it->second);
		}
	public:
		void init(const char* xml_file);
	private:
		ItemsUpgradeDataMap items_upgrade_data_map;
	
};


class swap_action_mrg {
	public:
		swap_action_data* get_swap_action(uint32_t id)
		{
			SwapActionDataMap::iterator it = swap_action_data_map.find(id);
			if (it == swap_action_data_map.end()) {
				return 0;
			}
			return &(it->second);
		}
		bool check_limit(player_t* p, uint32_t id);

		uint32_t get_top_limit(uint32_t id);

		uint32_t get_cd_time(uint32_t id);
        uint32_t get_buff_id(uint32_t id);
	public:
		void init(const char* xml_file);
	private:
		SwapActionDataMap swap_action_data_map;
};

enum {
	max_summon_dragon_list_cnt = 1000,
};

#pragma pack(1)

struct summon_dragon_elem_t {
	uint32_t uid;
	char nick[max_nick_size];
	uint32_t nimbus;
	uint32_t tm;
};

struct summon_dragon_list_rsp {
	uint32_t count;
	summon_dragon_elem_t elem[];
};
#pragma pack(0)

class Summon_dragon_list_mrg {
public:
	Summon_dragon_list_mrg() { update_tm_ = 0; count = 0; }
	bool if_need_update();
	void update_tm() { update_tm_ = get_now_tv()->tv_sec; }
	int pack_list(uint8_t* buf) {
		int idx = 0;
		pack(buf, count, idx);

		for (uint32_t i = 0; i < count; i++) {
			TRACE_LOG("%u %s %u %u", summon_dragon_list[i].uid, summon_dragon_list[i].nick,
				summon_dragon_list[i].nimbus, summon_dragon_list[i].tm);
			pack(buf, summon_dragon_list[i].uid, idx);
			pack(buf, summon_dragon_list[i].nimbus, idx);
			pack(buf, summon_dragon_list[i].nick, sizeof(summon_dragon_list[i].nick), idx);
			pack(buf, summon_dragon_list[i].tm, idx);
		}
		return idx;
	}
	void update_list(summon_dragon_list_rsp* rsp) {
		if (count > max_summon_dragon_list_cnt) {
			return;
		}
		count = rsp->count;
		for (uint32_t i = 0; i < count; i++) {
			summon_dragon_list[i] = rsp->elem[i];
		}
		update_tm();
	}
	private:
		summon_dragon_elem_t summon_dragon_list[max_summon_dragon_list_cnt];
		uint32_t count;
		uint32_t update_tm_;
};
/**
 * @brief judge item whether is strengthen material or not according to the items_shop.xml and items_nor.xml
 */
inline bool is_strengthen_material(uint32_t id)
{
    if ((id>=strengthen_material_base_1 && id<strengthen_material_base_1+10000)
        || (id>=strengthen_material_base_2 && id<strengthen_material_base_2+10000)) {
        return true;
    }
    return false;
}


inline bool is_relive_item(uint32_t item_id)
{
	return (item_id == 1302000 || item_id == 1302001);
}

inline bool is_storage_relive_item(uint32_t item_id)
{
	return (item_id == 1302000 || item_id == 1302001);
}

inline bool is_all_server_limit_item(uint32_t swap_id, uint32_t item_id)
{
    if (swap_id != 1466) {
        return false;
    }
    return (item_id == 1410017 || item_id == 1740023 || item_id == 1740024);
}

/**
 * @brief report user get shop item 
 */
void report_user_get_shop_item(player_t* p, uint32_t item_id, uint32_t cnt);

/**
 * @brief report user useshop item 
 */
void report_user_use_shop_item(player_t* p, uint32_t item_id, uint32_t cnt);

//--------------------------------------------------------------------------------
// global 
//--------------------------------------------------------------------------------
/*! the pointer to ItemManager for handling all items*/
extern GfItemManager* items;
/*! for handling all compose product*/
extern std::map<uint32_t, method_t*> methods_map_;
/*! suit info*/
extern suit_t suit_arr[];

extern uint32_t g_swap_action_cnt_1;
extern uint32_t g_swap_action_cnt_2;
extern uint32_t g_swap_action_cnt_3;
extern uint32_t g_swap_action_cnt_4;


struct player_packs {
private:
	PackItemMap 	items_map;
	PackClothesMap	clothes_map;
	PackItemMap 	material_map;
	player_t* owner;
public:
	player_packs(player_t* p) {
		owner = p;
	}
	uint32_t all_items_cnt() {
		return items_map.size();
	}
	
	uint32_t all_clothes_cnt() {
		return clothes_map.size();
	}

	pack_clothes_info_t * get_clothes_info_by_unique_id(uint32_t unique_id)
	{
		PackClothesMap::iterator it = clothes_map.find(unique_id);
		if (it != clothes_map.end()) {
			return &(it->second);
		}
		return NULL;
	}

	uint32_t get_item_cnt(uint32_t item_id) {
		if (is_strengthen_material(item_id)) {
			return get_material_cnt(item_id);
		} else {
			PackItemMap::iterator it = items_map.find(item_id);
			if (it != items_map.end()) {
				return it->second.item_cnt;
			}
		}
		return 0;
	}
	uint32_t get_material_cnt(uint32_t item_id) {
		PackItemMap::iterator it = material_map.find(item_id);
		if (it != material_map.end()) {
			return it->second.item_cnt;
		}

		return 0;
	}

	bool check_enough_bag_item_grid(player_t* p, uint32_t item_id,  uint32_t item_count)
	{
		const GfItem *pItem = items->get_item(item_id);
		if( !pItem){
			WARN_LOG("check_enough_bag_item_grid error: [%u]", item_id);
			return false;
		}
		PackItemMap::iterator it = items_map.find(item_id);
		if( it == items_map.end() ){
			return items_map.size() < get_player_total_item_bag_grid_count(p);			
		}
		return (it->second.item_cnt + item_count) <= pItem->max();
	}
	bool check_bag_item_count(uint32_t item_id, uint32_t item_count)
	{
		uint32_t count = get_item_cnt(item_id);
		if(count >= item_count && item_count > 0){
			return true;
		}
		return false;
	}

	uint32_t get_remain_clothes_bag_grid(player_t* p)
	{
		//return max_pack_clothes_cnt - clothes_map.size();
		return get_player_total_item_bag_grid_count(p) - clothes_map.size();
	}

	uint32_t get_remain_item_bag_grid(player_t *p)
	{
		return get_player_total_item_bag_grid_count(p) - items_map.size();
	}

	bool     is_clother_item_exist(uint32_t id, uint32_t item_id = 0, uint32_t item_lv = 0)
	{
		TRACE_LOG("%u %u %lu", id, item_id, clothes_map.size());
		PackClothesMap::iterator pItr = clothes_map.find(id);
		if(pItr == clothes_map.end()){
			return false;
		}
		if(item_id && pItr->second.clothes_id != item_id) {
			return false;
		}
        if(item_lv && pItr->second.lv != item_lv) {
			return false;
		}

		return true;
	}

	bool is_clothes_forever(uint32_t id)
	{
		PackClothesMap::iterator pItr = clothes_map.find(id);
		if(pItr == clothes_map.end()){
			return false;
		}
		if (pItr->second.timelag == 0xffffffff || pItr->second.timelag == 0) {
			return true;
		}
		return false;
	}
	bool 	is_item_exist(uint32_t item_id)
	{
		PackItemMap::iterator it = items_map.find(item_id);
		if (it == items_map.end()) {
			return false;
		}
		return true;
	}

	bool 	is_material_exist(uint32_t item_id)
	{
		PackItemMap::iterator it = material_map.find(item_id);
		if (it == material_map.end()) {
			return false;
		}
		return true;
	}

	bool	is_item_full(player_t* p, uint32_t item_id, uint32_t cnt)
	{
		uint32_t max_bag = get_player_total_item_bag_grid_count(p);
        if ( p->my_packs->all_items_cnt() + 1 > max_bag ) {
            return true;
        }
		return false;
	}

    int add_material(player_t* p ,uint32_t id, uint32_t cnt) {
		if (!(items->get_item(id))) {
			WARN_LOG("add error1:[%u]", id);
			return -1;
		}

		PackItemMap::iterator it = material_map.find(id);
		if (it != material_map.end()) {
			it->second.item_cnt += cnt;
			TRACE_LOG("add:[%u %u %u]:[%lu]",id, it->second.item_cnt, cnt, material_map.size());
			return 0;
		}

		pack_item_info_t item;
		item.item_id = id;
		item.item_cnt = cnt;

		material_map.insert(PackItemMap::value_type(id, item));
		TRACE_LOG("[%u %u]:[%lu]",id, cnt, material_map.size());
		return 0;
	}

	int add_item(player_t* p ,uint32_t id, uint32_t cnt, const char* channel_str, bool report_flg = true, int channel = 0) {
        if (is_strengthen_material(id)) {
            add_material(p, id , cnt);
        } else {
			if (!(items->get_item(id))) {
				WARN_LOG("add error1:[%u]", id);
				return -1;
			}

			PackItemMap::iterator it = items_map.find(id);
			if (it != items_map.end()) {
				it->second.item_cnt += cnt;
				TRACE_LOG("add:[%u %u %u]:[%lu]",id, it->second.item_cnt, cnt, items_map.size());
			} else {
				pack_item_info_t item;
				item.item_id = id;
				item.item_cnt = cnt;

				items_map.insert(PackItemMap::value_type(id, item));
				TRACE_LOG("[%u %u]:[%lu]",id, cnt, items_map.size());
			}
        }
        if ( (channel != 0) && (id > 1300000 || id < 1800000) ) {
            do_stat_log_item_universal_interface(id, channel, cnt);
        }
		if (report_flg) {
			const GfItem* itm = items->get_item(id);
			if (is_storage_relive_item(id) || itm->is_item_shop_sale()) {
				report_user_get_shop_item(p, id, cnt);
			}
		}
		if (channel_str) {
			LONG_LOG("add_item\t%u\titemid:%u,cnt:%u,channel:%s", p->id, id, cnt, channel_str);
		}
		return 0;
	}

	int add_clothes(player_t* p, uint32_t id, uint32_t u_id, uint32_t duration, const char* channel_str, uint32_t gettime = time(NULL), uint32_t timelag = 0, uint32_t lv = 0) {
		//if (clothes_map.size() >= max_pack_clothes_cnt) {
		/*
		if( clothes_map.size() >= get_player_total_item_bag_grid_count(p)){
			TRACE_LOG("[>= %u %u %u %u]",id, u_id, get_player_total_item_bag_grid_count(p), get_player_extern_item_bag_grid_count(p));
			return -1;
		}*/

		pack_clothes_info_t clothes;
		clothes.clothes_id = id;
		clothes.unique_id = u_id;
		clothes.duration = duration;
        clothes.lv = lv;
		clothes.gettime = gettime;
		clothes.timelag = timelag;
		TRACE_LOG("[%u %u %u]",id, u_id, duration);

		clothes_map.insert(PackClothesMap::value_type(u_id, clothes));

		if (channel_str) {
			LONG_LOG("add_clothes\t%u\titemid:%u,cnt:%u,unique_id:%u,channel:%s,level=%u", p->id, id, 1, u_id, channel_str, lv);
		}
		return 0;
	}
     
	int del_material(uint32_t id, uint32_t cnt) {
		PackItemMap::iterator it = material_map.find(id);
		if (it == material_map.end()) {
			WARN_LOG("del error:[%u]", id);
			return -1;
		}

		if (it->second.item_cnt < cnt) {
			material_map.erase(it);
			WARN_LOG("erase:[%u %u %u],[%lu]", id, it->second.item_cnt, cnt, material_map.size());
			return -1;
		} else if (it->second.item_cnt == cnt) {
			material_map.erase(it);
			TRACE_LOG("erase:[%u %u],[%lu]", id, cnt, material_map.size());
		} else {
			it->second.item_cnt -= cnt;
			TRACE_LOG("[%u %u],[%lu]", id, cnt, material_map.size());
		}

		return 0;
	}
   
	int del_item(player_t* p, uint32_t id, uint32_t cnt, const char* channel_str, bool report_flg = true) {
        if (is_strengthen_material(id)) {
            del_material(id, cnt);
        } else {
			PackItemMap::iterator it = items_map.find(id);
			if (it == items_map.end()) {
				WARN_LOG("del error:[%u]", id);
				return -1;
			}

			if (it->second.item_cnt < cnt) {
				items_map.erase(it);
				WARN_LOG("erase:[%u %u %u],[%lu]", id, it->second.item_cnt, cnt, items_map.size());
				return -1;
			} else if (it->second.item_cnt == cnt) {
				items_map.erase(it);
				TRACE_LOG("erase:[%u %u],[%lu]", id, cnt, items_map.size());
			} else {
				it->second.item_cnt -= cnt;
				TRACE_LOG("[%u %u],[%lu]", id, cnt, items_map.size());
			}
        }

        if (id > 1300000 || id < 1800000) {
            do_stat_log_item_universal_interface(id, item_cost_enum, cnt);
        }
		if (report_flg) {
			const GfItem* itm = items->get_item(id);
			if (itm && itm->is_item_shop_sale()) {
				report_user_use_shop_item(p, id, cnt);
			}
		}
		if (channel_str) {
			LONG_LOG("del_item\t%u\titemid:%u,cnt:%u,channel:%s", p->id, id, cnt, channel_str);
		}
		return 0;
	}

	int del_clothes(uint32_t unique_id, const char* channel_str) {
		PackClothesMap::iterator it = clothes_map.find(unique_id);
		if (it == clothes_map.end()) {
			return -1;
		}
		if (channel_str) {
			LONG_LOG("del_clothes\t%u\titemid:%u,cnt:%u,unique_id:%u,channel:%s,level=%u", owner->id, it->second.clothes_id, 1, unique_id, channel_str, it->second.lv);
		}
		clothes_map.erase(it);
		return 0;
	}

	int del_clothes_by_id(uint32_t id, const char* channel_str) {
		PackClothesMap::iterator it = clothes_map.begin();
		for (; it != clothes_map.end(); ++it) {
			if (it->second.clothes_id == id) {
				del_clothes(it->second.unique_id, channel_str);
				//clothes_map.erase(it);
				return 0;
			}
		}
		return 0;
	}

	int pack_all_item_info(void* buf) {
		int idx = 0;
		uint32_t item_cnt = items_map.size();
		taomee::pack(buf, item_cnt, idx);
		TRACE_LOG("item cnt:[%u]", item_cnt);

		for (PackItemMap::iterator it = items_map.begin(); it != items_map.end(); ++it) {
			taomee::pack(buf, it->second.item_id, idx);
			taomee::pack(buf, it->second.item_cnt, idx);
			TRACE_LOG("item:[%u %u]", it->second.item_id, it->second.item_cnt);
		}

		return idx;
	}

    int get_clothes_lv_by_id(uint32_t id) {
        PackClothesMap::iterator it = clothes_map.begin();
        for (; it != clothes_map.end(); ++it) {
            if (it->second.unique_id == id) {
                return it->second.lv;
            }
        }
        return 0;
    }

	int get_clothes_duration(uint32_t id) {
        PackClothesMap::iterator it = clothes_map.begin();
        for (; it != clothes_map.end(); ++it) {
            if (it->second.unique_id == id) {
                return it->second.duration;
            }
        }
        return 0;
    }
    void update_clothes_lv_by_id(uint32_t id, uint32_t lv) {
        PackClothesMap::iterator it = clothes_map.begin();
        for (; it != clothes_map.end(); ++it) {
            if (it->second.unique_id == id) {
                it->second.lv = lv;
            }
        }
    }

	uint32_t get_lowest_quality_attire(uint32_t attire_id)
	{
		uint32_t duration = 0xffffffff;
		uint32_t lowest_lv = 0xffffffff;
		uint32_t lowest_id = 0;
		PackClothesMap::iterator it = clothes_map.begin();
		for (; it != clothes_map.end(); ++it) {
            if (it->second.clothes_id == attire_id) {
                if (it->second.lv < lowest_lv) {
					lowest_id = it->second.unique_id;
					lowest_lv = it->second.lv;
					duration = it->second.duration;
					TRACE_LOG("%u %u %u %u", attire_id, it->second.unique_id, it->second.lv, it->second.duration);
                } else if (it->second.lv == lowest_lv && it->second.duration < duration) {
                	lowest_id = it->second.unique_id;
					lowest_lv = it->second.lv;
					duration = it->second.duration;
					TRACE_LOG("%u %u %u %u", attire_id, it->second.unique_id, it->second.lv, it->second.duration);
                }
            }
        }
		TRACE_LOG("ret %u", lowest_id);
		return lowest_id;
	}
};


//--------------------------------------------------------------------------------
// commands
//--------------------------------------------------------------------------------
/**
 * @brief add here for platform of statstics
 */
inline void do_stat_log_fumo_consume(uint32_t id, uint32_t lv, uint32_t userid, uint32_t cnt)
{
    uint32_t cmd = 0;
    uint32_t buf[2] = { 0};
    buf[0] = cnt;
    buf[1] = userid;
    if (id > 1300000 && id < 1400000) {
        cmd = stat_log_fumo_swap_drug + (id % 1300000) + (0x1000 * lv);
    } else if (id == 1550024 || id == 1550025 || id == 1550026) {
        cmd = stat_log_fumo_swap_drawing_40 + lv;
    } else if (id == 1550021 || id == 1550022 || id == 1550023) {
        cmd = stat_log_fumo_swap_drawing_41 + lv;
    } else {
        cmd = stat_log_fumo_swap_attire + (id % 10000) + (0x1000 * lv);
    }

	msglog(statistic_logfile, cmd, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [itemid=%u] [lv=%u] buf[%u, %u]",
        cmd, id, lv, buf[0], buf[1]);
    buf[0] = 0;
    buf[1] = cnt;
	msglog(statistic_logfile, stat_log_fumo_consume + lv, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [itemid=%u] [lv=%u] buf[%u, %u]",
        cmd, id, lv, buf[0], buf[1]);
}

inline void do_stat_log(uint32_t id, uint32_t num)
{
	msglog(statistic_logfile, id, get_now_tv()->tv_sec, &num, sizeof(num));
}
/**
 * @brief add here for platform of statstics
 */
inline void do_stat_log_role_exp(uint32_t uid, uint32_t role_type, uint32_t exp)
{
    uint32_t buf[2] = {0};
    buf[0] = uid;
    buf[1] = exp;
    uint32_t cmd = stat_log_role_exp;
    msglog(statistic_logfile, cmd + role_type, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [type=%u] buf[%u, %u]", cmd, role_type, buf[0], buf[1]);
}
/**
 * @brief for platform of statstics
 */
inline void do_stat_log_role_lv_change(uint32_t slv, uint32_t clv)
{
    uint32_t buf[2] = {0};
    buf[0] = slv;
    buf[1] = clv;
    uint32_t cmd = stat_log_use_lv_change;
    msglog(statistic_logfile, cmd + clv, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] buf[%u, %u]", cmd, buf[0], buf[1]);
}
/**
 * @brief for platform of statstics
 */
inline void do_stat_log_get_material(uint32_t id, uint32_t cnt)
{
    uint32_t buf[2] = {0};
    uint32_t cmd = 0;
    buf[0] = cnt;
    buf[1] = 0;
    switch (id) {
        case 1740006:
        case 1740007:
		case 1540907:
            cmd = 0x0902D001;
            break;
        case 1740014:
            cmd = 0x0902D002;
            break;
        default:
            return;
            break;
    }
    msglog(statistic_logfile, cmd, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] buf[%u, %u]", cmd, buf[0], buf[1]);
}

/**
 * @brief for platform of statstics
 */
inline void do_stat_log_consume_material(uint32_t id, uint32_t cnt)
{
    uint32_t cmd = 0;
    uint32_t buf[2] = {0};
    buf[0] = 0;
    buf[1] = cnt;
    switch (id) {
        case 1740006:
        case 1740007:
            cmd = 0x0902D001;
            break;
        case 1740014:
            cmd = 0x0902D002;
            break;
        default:
            return;
            break;
    }
    msglog(statistic_logfile, cmd, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] buf[%u, %u]", cmd, buf[0], buf[1]);
}

/**
  * @brief buy (an) item(s)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int buy_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief sell (an) item(s)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int sell_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief buy an clothes
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int buy_clothes_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief sell (some) item(s)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int batch_sell_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief sell an clothes
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int sell_clothes_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief repair all clothes include used, unused
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int repair_all_clothes_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
  * @brief player wear clothes
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int wear_clothes_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief get player's clothes unused
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_clothes_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief get player's strengthen material 
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_strengthen_material_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief get player's items
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_item_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief use a skill book
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int use_skill_book_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief use a item
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int use_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief set item bind key
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int set_itembind_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief discard (an) item(s)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int discard_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief discard a clothes
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int discard_clothes_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief get player's clothes duration
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_clothes_duration_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief decompose attire
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int decompose_attire_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief compose attire
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int compose_attire_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief strengthen attire
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int strengthen_attire_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
  * @brief upgrade item (items_upgrade.xml)
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int upgrade_item_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int swap_action_ex_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int swap_action_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
  * @brief get swap action
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_swap_action_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief get swap action times
 * @return 0 on success, -1 on error
 */
int get_swap_action_detail_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int set_app_buff_end_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
 *
 */
bool get_odds_cost_elem(swap_action_data* p_res, swap_action_elem_t * cost_elem, uint32_t odds_val);

int db_add_summon_dragon_list(player_t* p);

int summon_dragon_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int db_get_summon_dragon_list(player_t* p);

int db_get_summon_dragon_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int get_summon_dragon_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

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
int db_buy_item(player_t* p, const GfItem* itm, uint32_t cnt, uint32_t ret_flg = true);

/**
  * @brief buy items
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_buy_clothes(player_t* p, const GfItem* itm);

/**
  * @brief sell items
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_sell_item(player_t* p, const GfItem* itm, uint32_t cnt);

/**
  * @brief use item
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_use_item_ex(player_t* p, uint32_t userid, uint32_t role_tm, uint32_t item_id, uint32_t count, bool is_shop_sale, bool callback = true);


/**
  * @brief use item
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_use_item(player_t* p, uint32_t item_id, uint32_t count, bool is_shop_sale = false, bool callback = true);

/**
  * @brief sell clothes
  * @param p the buyer
  * @param itm item to buy
  * @param cnt number of item to buy
  * @return 0 on success, -1 on error
  */
int db_sell_clothes(player_t* p, const GfItem* itm, uint32_t cnt);

/**
 * @brief get clothes list
 * @return 0 on success, -1 on error
 */
int db_get_clothes_list(player_t* p);




/**
 * @brief set item bind key
 * @return 0 on success, -1 on error
 */
int db_set_itembind(player_t* p, uint32_t len, uint8_t* bindbuf);

/**
 * @brief decompose attire
 * @return 0 on success, -1 on error
 */
int do_decompose_attire(player_t* p, uint32_t attire_id, uint32_t unique_id);

/**
 * @brief compose attire
 * @return 0 on success, -1 on error
 */
int do_compose_attire(player_t* p, uint32_t method_id, uint32_t addition_item_id);


uint32_t get_compose_attire_id(method_t* p_method, uint32_t pro_lv, uint32_t addition_item_id);
/**
 * @brief do strengthen attire
 * @return 0 on success, -1 on error
 */
int do_strengthen_attire(player_t* p, strengthen_attire_t* p_strengthen);
/**
 * @brief do strengthen attire without material
 * @return 0 on success, -1 on error
 */
int do_strengthen_attire_without_material(player_t* p, uint32_t uniquekey);
/**
 * @brief add allocated_exp to summon
 * @return 0 on success, -1 on error
 */
int add_exp_to_summon_rudely(player_t *p, uint32_t allocated_exp);
/**
 * @brief do appearance buf
 * @return 0 on success, -1 on error
 */
void do_item_buf(player_t* p, const GfItem* itm);

/**
 * @brief chg appearance buf rudely
 */
void chg_app_buff(player_t* p, uint32_t buff_id);
void chg_app_buff_back(player_t* p, uint32_t buff_id);

/**
 * @brief decompose attire
 * @return 0 on success, -1 on error
 */
int do_decompose_attire(player_t* p, uint32_t attire_id, uint32_t unique_id);

/**
 * @brief query restriction
 * @return 0 on success, -1 on error
 */
int db_query_restriction_list(player_t* p);

/**
  * @brief callback for buying items
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_buy_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for buying clothes
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_buy_clothes_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for repairing clothes
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_repair_all_clothes_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for selling item
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_sell_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for selling clothes
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_sell_clothes_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for selling item
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_batch_sell_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for getting clothes list
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_get_clothes_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for getting strengthen material list
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_get_strengthen_material_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for getting item list
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_get_item_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for getting player pack
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_get_packs_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for getting item list
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_wear_clothes_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for using a skill book
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_use_skill_book_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for using a item
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_use_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for swap item
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_swap_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 * @brief callback for set_itembind
 * @param p the player who initiated the request to dbproxy
 * @param id the requester id
 * @param body
 * @param bodylen
 * @param ret
 * @return 0 on success, -1 on error
 */
int db_set_itembind_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for discarding items
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_discard_item_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for discarding clothes
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_discard_clothes_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
/**
  * @brief callback fordecompose attire
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_decompose_attire_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for compose attire
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_compose_attire_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
  * @brief callback for strengthen attire
  * @param p the player who initiated the request to dbproxy
  * @param id the buyer
  * @param body
  * @param bodylen
  * @param ret
  * @return 0 on success, -1 on error
  */
int db_strengthen_attire_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
//added by cws0620
int db_strengthen_attire_without_material_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);
/**
  * @brief get player's clothes duration
  */
int btlsvr_player_clothes_duration_callback(player_t* p, btl_proto_t* pkg);

int db_add_item_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret);

int db_upgrade_item_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret);

int db_get_swap_action_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret);

int db_swap_action_callback(player_t *p, userid_t id, void *body, uint32_t bodylen, uint32_t ret);

bool pack_swap_action_item(player_t* p, uint32_t resid, uint32_t role_type, swap_action_data* p_out_res, swap_action_data* p_out_mail);

int db_swap_action(player_t* p, const swap_action_data* p_restr, const swap_action_data* p_res_mail, bool ret_flg=true, uint32_t add_times = 1);

int db_swap_action_empty(player_t* p, uint32_t resid, uint32_t times = 1);

uint32_t get_swap_action_times(player_t* p, uint32_t resid);

bool is_player_have_swap_action_times(player_t* p, uint32_t resid);

uint32_t get_player_swap_action_left_times(player_t* p, uint32_t resid);

uint32_t get_swap_action_last_tm(player_t* p, uint32_t resid);

bool is_swap_action_in_new_day(player_t* p, uint32_t resid);

int add_swap_action_times(player_t* p, uint32_t resid, uint32_t times = 1);

int clear_swap_action_times(player_t* p, uint32_t resid);

void calc_player_level(player_t *p);
int player_del_items(player_t* p, db_del_item_elem_t* arr, uint32_t count, bool notify_2_player = false, const char* channel_str = channel_string_other);

/**
 * @brief report public information to server for monitor
 */
void report_add_to_monitor(player_t* p, uint32_t opt_type, uint32_t cnt, uint32_t id, uint32_t val);


void send_appearance_buf(player_t* p, buf_skill_t* p_buf);

int send_got_gold_dragon_to_world(player_t *p);

/**
 * @brief load strengthen material configure from the xml file
 * @return 0 on success, -1 on error
 */
int load_strengthen_material(xmlNodePtr cur);
/**
  * @brief load compose configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_compose(xmlNodePtr cur);

/**
  * @brief load suit configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int load_suit(xmlNodePtr cur);

/**
  * @brief check the suit id in item.xml
  * @return 0 on success, -1 on error
  * @see unload_skills
  */
int check_item_suit(const GfItem& itm, void* in);

/**
  * @brief load unique items from an xml file
  * @return 0 on success, -1 on error
  */
int load_unique_items(xmlNodePtr cur);
/**
  * @brief give presents to all online usr
  * @return 0 on success, -1 on error
  */



//--------------------------------------------------------------------------------
// inline function
//--------------------------------------------------------------------------------
/**
  * @brief determine if 'p' can buy 'itm'
  * @param p
  * @param itm
  * @return true or false
  */
inline bool is_item_buyable(player_t* p, const GfItem* itm)
{
	return ( (itm->is_item_all_buyable()) || (p->vip && itm->is_item_vip_buyable()) );
}

/**
  * @brief determine if 'p' can sell 'itm'
  * @param p
  * @param itm
  * @return true or false
  */
inline bool is_item_sellable(player_t* p, const GfItem* itm)
{
	return ( (itm->is_item_all_sellable()) || (p->vip && itm->is_item_vip_sellable()) );
}

/**
  * @brief determine if 'p' have enough cions to buy 'itm'
  * @param p
  * @param itm
  * @return true or false
  */
inline bool is_enough_to_buy(player_t* p, const uint32_t price, uint32_t cnt)
{
	return ( p->coins >= price * cnt  );
}

/**
  * @brief determine if this clothes weared
  * @param p
  * @param itm
  * @return true or false
  */
inline bool is_clothes_weared(player_t* p, uint32_t unique_id)
{
	for (uint32_t i = 0; i < p->clothes_num; i++) {
		if (p->clothes[i].unique_id == unique_id) {
			return true;
		}
	}

	return false;
}

/**
  * @brief determine if this clothes weared
  * @param p
  * @param itm
  * @return true or false
  */
inline bool is_clothes_weared(player_t* p, uint32_t unique_id, uint32_t clothes_id)
{
	for (uint32_t i = 0; i < p->clothes_num; i++) {
		if (p->clothes[i].unique_id == unique_id && p->clothes[i].clothes_id == clothes_id) {
			return true;
		}
	}

	return false;
}


/**
  * @brief determine if 'p' have enough cions to buy 'itm'
  * @param p
  * @param itm
  * @return true or false
  */
inline bool is_exploit_enough_to_buy(player_t* p, const uint32_t price, uint32_t cnt)
{
	return ( p->exploit >= price * cnt  );
}

/**
  * @brief determine if 'p' have enough fumo points to buy 'itm'
  * @param p
  * @param itm
  * @return true or false
  */
inline bool is_fumo_enough_to_buy(player_t* p, const uint32_t price, uint32_t cnt)
{
	return ( p->fumo_points_total >= price * cnt );
}


/**
  * @brief determine if 'p' have enough cions to buy 'itm'
  * @param p
  * @param itm
  * @return true or false
  */
inline bool is_clothes_can_wear(player_t* p, const GfItem* itm)
{
	//return ( (p->role_type == itm->category()) || (itm->category() == 11) );
	return items->is_clothes_can_wear(itm->category(), p->role_type);
}

/**
 * @brief update duration of attire
 * @return true or false
 */
inline bool update_used_clothes_duration(player_t* p, uint32_t unique_id, uint32_t duration)
{
	for ( uint32_t cnt = 0; cnt < p->clothes_num; cnt++ ) {
		if (p->clothes[cnt].unique_id == unique_id) {
			p->clothes[cnt].duration = duration;
			return true;
		}
	}	
	return false;
}

/**
 * @brief If get amber do statistics
 */
inline void do_stat_log_amber(uint32_t cmdid, uint32_t item_id, uint32_t type)
{
	const GfItem* itm = items->get_item(item_id);
	if (!itm) {
		TRACE_LOG(" stat item not exist:%u ", item_id);
		return;
	}
    TRACE_LOG("teat stat item id %u", item_id);
	if (items->is_skill_book(itm->category())) {
        uint32_t buf[1] = { 1 };
        uint32_t amber_id = item_id - 1200000 - ((type - 1)* 200);
        uint32_t itype = ((type-1) << 12);
        msglog(statistic_logfile, cmdid + amber_id + itype, get_now_tv()->tv_sec, buf, sizeof(buf));
        TRACE_LOG("stat log cmdid:%x item_id:%u amber_id:%u type:[%u %x]",
            cmdid, item_id, amber_id, type, itype);
	}
}

/**
 * @brief as fourth interface of statistics
 * NOTE: for self define message
 */
inline void do_stat_item_log(uint32_t cmdid, uint32_t id, uint32_t type, uint32_t buf[], int len)
{
    msglog(statistic_logfile, cmdid + type + id , get_now_tv()->tv_sec, buf, sizeof(buf[0]) * len);
    TRACE_LOG("stat log cmid:%x id:%x type:%x buf para1:%u len:%lu",
        cmdid, id, type, buf[0], sizeof(buf[0]) * len);
}

/**
 * @brief use plugin buy or sell item
 */
inline void do_stat_log_use_plugin(uint32_t cmdid, uint32_t id, uint32_t cnt)
{
    uint32_t buf[2] = {0};
    buf[0] = cnt;
    buf[1] = id;
    msglog(statistic_logfile, cmdid, get_now_tv()->tv_sec, buf, sizeof(buf));
    TRACE_LOG("stat log cmdid:%x userid:%u", cmdid, id);
}

inline method_t* get_compose_method(uint32_t method_id)
{
	std::map<uint32_t, method_t*>::iterator it = methods_map_.find(method_id);
	if (it != methods_map_.end()) {
		return it->second;
	}
	return NULL;
}

inline uint32_t get_fumo_price(const GfItem* clothes)
{
	if (clothes->quality_lv == 2) {
		//green
		if (clothes->use_lv() < 10) {
			return 200;
		} else if (clothes->use_lv() >= 10) {
			return 1200;
		} 
	} else if (clothes->quality_lv == 3) {
		//blue
		if (clothes->use_lv() < 30) {
			return 3000;
		} else if (clothes->use_lv() >= 30) {
			return 5000;
		} 
	} 
	return 0;
}

inline bool is_user_info_meet_honor_request(player_t* p, uint16_t honor_lv)
{
	uint16_t user_need_lv = get_user_lv_needed_by_honor_lv(honor_lv);

	if (p->lv < user_need_lv) {
		return false;
	}

	uint32_t user_need_honor = calc_honor(honor_lv);
	if (p->honor < user_need_honor) {
		return false;
	}
	return true;
}

inline bool is_trade_bcast_item(uint32_t item_id)
{
	return (item_id == trade_mcast_item_id_1 || item_id == trade_mcast_item_id_2);
}

inline bool is_double_random(uint32_t id)
{
    return (id >= 1260 && id <= 1267);
}

int add_item_to_player(player_t* p, uint32_t type, uint32_t itemid, uint32_t cnt, uint32_t notify2player, const char* channel_str);

int player_gain_item(player_t* p, uint32_t type, uint32_t itemid, uint32_t cnt, const char* channel_str = channel_string_use_tongbao_item, bool is_call_back = true);
/**
  * @brief return bitpos of unique items 
  * @return 0 none
  */
uint32_t get_unique_item_bitpos(uint32_t item_id);

int set_app_buf_end(void *owner, void *data);

void chg_app_buff_back(player_t* p, uint32_t buff_id);

int send_use_item_rsp(player_t* p, uint32_t item_id, int completed = 1);

int db_get_swap_action(player_t* p);

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// struct
//--------------------------------------------------------------------------------

#pragma pack(1)

/**
  * @brief response from dbproxy to conform the request of buying items
  */
struct buy_item_rsp_t {
	uint32_t	coins;
	uint32_t	itmid;
	uint32_t	cnt;
};

/**
  * @brief response from dbproxy to conform the request of buying clothes
  */
struct db_clothes_elem_t {
	uint32_t	clothes_id;
	//uint32_t	get_time;
	uint32_t	unique_id;
};

struct db_wear_clothes_elem_t {
	uint32_t	clothes_id;
	uint32_t	unique_id;
	uint32_t 	duration;
	uint32_t 	attirelv;
	uint32_t	gettime;
	uint32_t	timelag;
};

struct db_get_clothes_list_elem_t {
	uint32_t	clothes_id;
	uint32_t	unique_id;
	uint32_t 	duration;
	uint32_t 	attirelv;
	uint32_t 	gettime;
	uint32_t 	timelag;
	uint32_t 	usedflag;
};

struct buy_clothes_rsp_t {
	//uint32_t	clothes_id;
	//uint32_t	unique_id;
	//uint32_t	get_time;
	uint8_t		type;
	uint32_t	coins;
	uint32_t	count;
	db_clothes_elem_t clothes[];
};

struct sell_item_rsp_t {
	uint32_t	item_id;
	uint32_t	item_cnt;
	uint32_t	coins;
};

struct sell_clothes_rsp_t {
	uint32_t	clothes_id;
	uint32_t	unique_id;
	uint32_t	coins;
};

struct db_repair_all_clothes_elemt_t {
	uint32_t	clothes_id;
	uint32_t	unique_id;
	uint32_t	duration;
};

struct repair_all_clothes_rsp_t {
	uint32_t	left_coins;
	uint32_t	clothes_cnt;
	db_repair_all_clothes_elemt_t clothes[];
};

struct get_clothes_list_rsp_t {
	uint32_t			clothes_cnt;
	db_get_clothes_list_elem_t	clothes[];
};

struct db_item_elem_t {
	uint32_t	item_id;
	uint32_t	count;
};

struct batch_sell_item_rsp_t {
    uint32_t    type;
    uint32_t    coins;
    uint32_t    cnt;
    db_item_elem_t  item[];
};

struct get_item_list_rsp_t {
	uint32_t		item_cnt;
	db_item_elem_t	item[];
};

struct db_warehouse_clothes_item_elem_t
{
	uint32_t  id;
	uint32_t  attireid;
	uint32_t  get_time;
	uint32_t  attire_rank;
	uint32_t  duration;
	uint32_t  end_time;
	uint32_t  attire_lv;
};

struct get_warehouse_clothes_item_list_rsp_t
{
	uint32_t        item_cnt;
	db_warehouse_clothes_item_elem_t item[];
};


struct get_warehouse_item_list_rsp_t
{
	uint32_t                item_cnt;
	db_item_elem_t          item[];	
};

struct get_packs_rsp_t {
	uint32_t	clothes_cnt;
	uint32_t	items_cnt;
	uint8_t		body[];
};

struct wear_clothes_rsp_t {
	uint32_t				clothes_cnt;
	db_wear_clothes_elem_t	clothes[];
};


struct set_itembind_rsp_t{
	uint8_t buf[max_itembind_len];
};


struct get_itembind_rsp_t{
	uint8_t buf[max_itembind_len];
};

/**
  * @brief response from dbproxy to conform the request of using a skill book
  */
struct use_skill_book_rsp_t {
	uint32_t	book_id;
	uint32_t	left_points;
};

/**
  * @brief response from dbproxy to conform the request of using a item
  */
struct use_item_rsp_t {
	uint32_t	item_id;
	uint32_t    count;
};

struct db_swap_clothes_elem_t {
	uint32_t	clothes_id;
	//uint32_t	get_time;
	uint32_t	unique_id;
	uint32_t	gettime;
	uint32_t    endtime;
};

struct swap_item_rsp_t {
	uint32_t	coins;
	uint32_t	exp;
	uint32_t    allocator_exp;
	uint32_t	skill_point;
	uint32_t	fumo_point;
	uint32_t	honor;
	uint32_t	clothes_cnt;
	db_swap_clothes_elem_t clothes[];
};

struct get_killed_boss_rsp_t {
	uint32_t 	cnt;
	uint32_t	boss_id[];
};

struct clothes_duration_elem_t {
	uint32_t	unique_id;
	uint16_t	duration;
};

struct clothes_duration_rsp_t {
	uint32_t	clothes_cnt;
	uint8_t		body[];
};

struct decompose_attire_item_t {
	uint32_t	item_id;
	uint32_t	cnt;
};

struct decompose_attire_rsp_t {
	uint32_t	unique_id;
	uint32_t	cnt;
	uint32_t    max_item_bag_grid_count;
	decompose_attire_item_t	item[];
};

struct compose_attire_item_t {
	uint32_t 	is_clothes;
	uint32_t	item_id;
	uint32_t	cnt;
};

struct compose_attire_rsp_t {
	uint32_t 	left_coins;
	uint32_t	is_clothes;
	uint32_t	attire_id;
	uint32_t	unique_id;
	uint32_t 	duration;
	uint32_t    method_id;
	uint32_t 	del_item;
	uint32_t 	random_item;
	uint32_t	cnt;
	compose_attire_item_t	item[];
};

struct move_item_rsp_t
{
	uint32_t        item_id;
        uint32_t        cnt;
	uint32_t        left_coin;
};

struct move_clothes_item_rsp_t
{
	uint32_t        old_id;
	uint32_t        item_id;
    uint32_t        id;
	uint32_t        get_time;
	uint32_t        attire_rank;
	uint32_t        duration;
	uint32_t        end_time;
	uint32_t        attire_lv;
    uint32_t        left_coin;	
};

struct upgrade_item_head_rsp_t
{
	uint32_t upgrade_succ;
	uint32_t del_cnt;
	uint32_t add_cnt;
};

struct upgrade_item_elem_rsp_t
{
	uint32_t type;
	uint32_t id;
	uint32_t count;
	uint32_t duration;
};

struct swap_action_header_rsp_t
{
	uint32_t id;
	uint32_t type;
	uint32_t add_times;
	uint32_t user_flg;
	uint32_t del_cnt;
	uint32_t add_cnt;
};

struct swap_action_item_rsp_t
{
	uint32_t type;
	uint32_t id;
	uint32_t count;
	uint32_t unique_id;
	uint32_t duration;
};

struct get_swap_action_header_rsp_t
{
	uint32_t cnt;
};

struct get_swap_action_item_rsp_t
{
	uint32_t id;
	uint32_t type;
	uint32_t count;
	uint32_t tm;
};

struct db_distract_clothes_strength_rsp_t {
	uint32_t keep_flag;
	uint32_t old_clothes_id;
	uint32_t new_clothes_id;
	uint32_t new_level;
	uint32_t stone_id;
	uint32_t reduce_coins;
};

#pragma pack()

int player_item_2_score_cmd(player_t * player, uint8_t * body, uint32_t bodylen);
int do_get_swap_action_detail_info(player_t* p, uint32_t swap_id);

int distract_clothes_strength_cmd(player_t * p, uint8_t * body, uint32_t bodylen); 
int db_distract_clothes_strength_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

#endif
