/**
 *============================================================
 *  @file     item_manager.hpp
 *  @brief    用于解析Item表的公共部分。公共部分的格式必须统一，详见items.xml\n
 *            使用方法：请参阅conf_parser/item.cpp和items.xml\n
 *            g++ item.cpp -I/usr/include/libxml2 -lxml2 -ltaomee++
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */
#ifndef ITEM_MANAGER_HPP_
#define ITEM_MANAGER_HPP_

#include <string>

#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>

extern "C" {
#include <stdint.h>
}
#include <map>
#include <vector>


//using namespace taomee;
//namespace taomee {

/**
 * @brief 解析item配置文件出错时抛出的异常。继承自XmlParseError
 */
class ItemParseError : public taomee::XmlParseError {
public:
	explicit ItemParseError(const std::string& errmsg) : XmlParseError(errmsg)
		{ }
};

/**
 * @brief 保存Item的公共数据
 */
class Item {
public:
	/**
	 * @brief 默认构造函数，初始化Item内部成员
	 */
	Item() { init(); }

	/**
	 * @brief 返回Item ID
	 * @return item ID
	 */
	uint32_t id() const { return id_; }
	/**
	 * @brief 返回Item类别 
	 * @return item类别
	 */
	uint32_t category() const { return cat_; }
	/**
	 * @brief 返回Item在DB中的类别（有些物品，在online分类不同，但db都是保存在同一张表）。
	 * @return item在DB中的类别
	 */
	uint32_t db_category() const { return dbcat_; }
	/**
	 * @brief 返回用户最多能同时拥有的该Item的数量。某些物品，用户同时只能拥有1件，有些却可以N件。
	 * @return 用户最多能同时拥有的该Item的数量
	 */
	uint32_t max() const { return max_; }
	/**
	 * @brief 返回该Item的购买价格
	 * @return item的购买价格
	 */
	uint32_t price() const { return price_; }
	/**
	 * @brief 返回该Item的售卖价格
	 * @return item的售卖价格
	 */
	uint32_t sellprice() const { return sellprice_; }	
    /**
	 * @brief 返回该Item的寿命
	 * @return item的寿命
	 */
	uint32_t lifetime() const { return lifetime_; }	
	/**
	 * @brief 返回该Item是否全员可买
	 * @return true or false
	 */
	bool is_item_all_buyable() const { return taomee::test_bit_on(flag_, item_all_buyable); }
	/**
	 * @brief 返回该Item是否全员可卖
	 * @return true or false
	 */
	bool is_item_all_sellable() const { return taomee::test_bit_on(flag_, item_all_sellable); }
	/**
	 * @brief 返回该Item是否vip可买
	 * @return true or false
	 */
	bool is_item_vip_buyable() const { return taomee::test_bit_on(flag_, item_vip_buyable); }
	/**
	 * @brief 返回该Item是否vip可卖
	 * @return true or false
	 */
	bool is_item_vip_sellable() const { return taomee::test_bit_on(flag_, item_vip_sellable); }
	/**
	 * @brief 返回该Item是否只有vip可用
	 * @return true or false
	 */
	bool is_item_vip_only() const { return taomee::test_bit_on(flag_, item_vip_only); }
    /**
	 * @brief 返回该Item是否为商城出售物品
	 * @return true or false
	 */
	bool is_item_shop_sale() const { return taomee::test_bit_on(flag_, item_shop_sale); }

	/**
	 * @brief 返回buf 物品的类型
	 * @return 
	 */
	uint32_t buf_type() const { return buf_type_;}
	uint32_t buff_target() const { return buff_target_;}
	
	uint32_t keep_tm() const { return keep_tm_;}

	bool is_item_buf_type_appearance() const {return buf_type_ == item_buf_type_app ;}

	/**
	 * @brief 返回buf 物品的持续时间
	 * @return 
	 */
	uint32_t buf_duration() const {return buf_duration_;}

	uint32_t per_trim() const {return per_trim_;}
	/**
	 * @brief 返回buf 物品的额外id
	 * @return 
	 */
	uint32_t buf_ex_id1() const {return buf_ex_id1_;}

	/**
	 * @brief return the mutex_type_
	 */	
	uint32_t mutex_type() const { return mutex_type_; }

	bool is_storage_decompose_type() const { return decompose_ == 2; }
	/** 
	 * @brief check for item can be traded
	 * 
	 * @return 1 ok , 0 cann't be traded 
	 */
	bool is_tradable()const {return (tradable_ != 0);}

	bool is_tradable_for_all()const 
	{
		return (tradable_ == 1);
	}

	bool is_tradable_for_vip()const
	{
		return (tradable_ >= 1);
	}

protected:
	/**
	 * @brief 初始化Item内部成员
	 */
	void init() { memset(this, 0, sizeof(*this)); }

private:
	enum {
		item_all_buyable   = 1,
		item_all_sellable  = 2,
		item_vip_buyable   = 3,
		item_vip_sellable  = 4,
		item_vip_only	   = 5,
		item_shop_sale	   = 6,
		item_buf_type_app = 10,

        //strengthen_material_base_id_1 = 1540000,
        //strengthen_material_base_id_2 = 1740000
	};

	/**
	 * @brief 设置item ID
	 * @param id item ID
	 */
	void set_id(uint32_t id) { id_ = id; }
	/**
	 * @brief 设置item类别
	 * @param cat item 类别
	 */
	void set_category(uint32_t cat) { cat_ = cat; }
	/**
	 * @brief 设置item在db中的类别
	 * @param dbcat item在db中的类别
	 */
	void set_db_category(uint32_t id, uint32_t dbcat) { dbcat_ = dbcat;
        /*
        if ((id >=strengthen_material_base_id_1 && id <strengthen_material_base_id_1+10000)
            || (id >=strengthen_material_base_id_2 && id < strengthen_material_base_id_2+10000))
            dbcat_ = 3;
            */
    }
	/**
	 * @brief 设置用户能够拥有的该item的最大个数
	 * @param max 用户能够拥有的该item的最大个数
	 */
	void set_max(uint32_t max) { max_ = max; }
	/**
	 * @brief 设置item的购买价格 
	 * @param price item购买价格
	 */
	void set_price(uint32_t price) { price_ = price; }
	/**
	 * @brief 设置item的售卖价格
	 * @param sellprice item的售卖价格
	 */
	void set_sellprice(uint32_t sellprice) { sellprice_ = sellprice; }
    /**
	 * @brief 设置item的寿命
	 * @param lifetime item的寿命
	 */
    void set_lifetime(uint32_t lifetime) {lifetime_ = lifetime; }
	/**
	 * @brief 设置item是否全员可买
	 */
	void set_item_all_buyable() { flag_ = taomee::set_bit_on(flag_, item_all_buyable); }
	/**
	 * @brief 设置item是否全员可卖
	 */
	void set_item_all_sellable() { flag_ = taomee::set_bit_on(flag_, item_all_sellable); }
	/**
	 * @brief 设置item是否vip可买
	 */
	void set_item_vip_buyable() { flag_ = taomee::set_bit_on(flag_, item_vip_buyable); }
	/**
	 * @brief 设置item是否vip可卖
	 */
	void set_item_vip_sellable() { flag_ = taomee::set_bit_on(flag_, item_vip_sellable); }
	/**
	 * @brief 设置item是否只有vip可用
	 */
	void set_item_vip_only()	{ flag_ = taomee::set_bit_on(flag_, item_vip_only); }
    /**
     * @brief 设置item是否为商城道具
     */
    void set_item_shop_sale() { flag_ = taomee::set_bit_on(flag_, item_shop_sale);}
	/**
	 * @brief 设置buf物品的类型
	 */
	void set_item_buf_type(uint32_t type) { buf_type_ = type;}

	void set_item_buf_target(uint32_t type) { buff_target_ = type;}
	
	/**
	 * @brief 设置buf物品的类型
	 */
	void set_item_keep_tm(uint32_t tm) { keep_tm_ = tm;}

	/**
	 * @brief 设置buf物品的持续时间
	 */
	void set_item_buf_duration(uint32_t duration) { buf_duration_ = duration;}

	void set_item_buf_per_trim(uint32_t per_trim) { per_trim_ = per_trim;}
	/**
	 * @brief 设置buf物品的额外id
	 */
	void set_item_buf_ex_id1(uint32_t id) { buf_ex_id1_ = id;}
	/**
	* @brief set the mutex type on using
	*/
	void set_mutex_type(uint32_t value) { mutex_type_ = value; };

	/**
	* @brief set the decompose_
	*/
	void set_decompose(uint32_t value) { decompose_ = value; };

	/** 
	 * @brief set item tradable
	 * 
	 * @param value
	 */
	void set_tradable(uint32_t value) { tradable_ = value;}

	/* item id */
	uint32_t	id_;
	/* flag: tradability, viptradability, viponly */
	uint8_t		flag_;
	/* category of an item */
	uint8_t		cat_;
	/* category id for db */
	uint8_t		dbcat_;
	/* max number of an item a player can own */
	uint32_t	max_;
	/* coins needed to buy an item */
	uint32_t	price_;
	/* coins received after selling an item */
	uint32_t	sellprice_;
    /* lifetime */
    uint32_t    lifetime_;
	/* type of buf skill */
	uint32_t	buf_type_;
	uint32_t 	buff_target_;
	/* time of appeare */
	uint32_t	keep_tm_;
	/* time continue */
	uint32_t 	buf_duration_;
	/* trim percent */
	uint32_t	per_trim_;
	/* id */
	uint32_t	buf_ex_id1_;
	/* mutex type */
	uint32_t    mutex_type_;

	/* can be composed*/
	uint32_t    decompose_;
	/** 
	 * @brief 1 tradable_, 0 untradable
	 */
	uint32_t    tradable_;

	/**
	 * @brief 必须把ItemManager声明为Item类的友元类，因为ItemManager里面要用到Item里的私有函数。
	 */
	template <typename ITEM, int MAX_CAT, int MAX_ELEM>
	friend class ItemManager;
};

/**
 * @brief GfItem
 */
class GfItem : public Item {
public:
	/**
	 * @brief constructor
	 */
	GfItem() { init(); }
	void init();
	/**
	 * @brief return the eqiup part
	 */
	uint16_t equip_part() const { return equip_part_; }
	/**
	 * @brief return the lv of user
	 */
	uint16_t use_lv() const { return use_lv_; }
	/**
	 * @brief return the hit
	 */
	uint16_t hit() const { return hit_; }
	/**
	 * @brief return the dodge
	 */
	uint16_t dodge() const { return dodge_; }
	/**
	 * @brief return the crit
	 */
	uint16_t crit() const { return crit_; }
	/**
	 * @brief return the hp
	 */
	uint16_t hp() const { return hp_; }
	/**
	 * @brief return the mp
	 */
	uint16_t mp() const { return mp_; }
	/**
	 * @brief return the add_hp
	 */
	uint16_t add_hp() const { return add_hp_; }
	/**
	 * @brief return the add_mp
	 */
	uint16_t add_mp() const { return add_mp_; }
	/**
	 * @brief return the add_hp
	 */
	uint16_t add_per_hp() const { return add_per_hp_; }
	/**
	 * @brief return the add_mp
	 */
	uint16_t add_per_mp() const { return add_per_mp_; }	
	/**
	 * @brief return the slot
	 */
	uint16_t slot() const { return slot_; }
	/**
	 * @brief return the skill_id
	 */
	uint32_t skill_id() const { return skill_id_; }
	/**
	 * @brief return the repair_price
	 */
	uint32_t repair_price() const { return repair_price_; }
	/**
	 * @brief return the pvp_available
	 */
	uint32_t pvp_available() const { return pvp_available_; }
	/**
	 * @brief return the fight_value
	 */
	uint32_t fight_value() const { return fight_value_; }
	/**
	 * @brief return the summon_id
	 */
	uint32_t summon_id() const { return summon_id_; }

	uint32_t pet_attr_id() const { return pet_attr_id_;}
	/**
	 * @brief return the exploit_value
	 */
	uint32_t exploit_value() const { return exploit_value_; }

	uint32_t swap_action_id() const { return swap_action_id_; };
	/**
	 * @brief return the exploit_value
	 */
	uint32_t honor_level() const { return honor_level_; }

	
	/**
	 * @brief return the suit_id_
	 */
	uint32_t suit_id() const { return suit_id_; }
	
	/**
	 * @brief return the effect_type_
	 */
	uint16_t effect_type() const { return effect_type_; }
	
	/**
	 * @brief return the effect_value_
	 */
	uint16_t effect_value() const { return effect_value_; }

	const char* get_name() const { return itm_name; }

public:
	/**
	 * @brief clothes_lv info
	 */
	uint16_t	strength;
	uint16_t	agility;
	uint16_t	body_quality;
	uint16_t	quality_lv;
	uint16_t	drop_lv;
	uint16_t	stamina;
	uint16_t	atk[2];
	uint16_t	def;
	uint16_t 	duration;
	uint16_t	interval;
    uint16_t    speed_ratio;
//	gf_clothes_t clothes_info;

public:
	/**
	 * @brief set the equip part
	 */
	void set_equip_part(uint16_t equip_part) { equip_part_ = equip_part; }
	/**
	 * @brief set the lv of user
	 */
	void set_use_lv(uint16_t use_lv) { use_lv_ = use_lv; }
	/**
	 * @brief set the hit
	 */
	void set_hit(uint16_t hit) { hit_ = hit; }
	/**
	 * @brief set the dodge
	 */
	void set_dodge(uint16_t dodge) { dodge_ = dodge; }
	/**
	 * @brief set the crit
	 */
	void set_crit(uint16_t crit) { crit_ = crit; }
	/**
	 * @brief set the hp
	 */
	void set_hp(uint16_t hp) { hp_ = hp; }
	/**
	 * @brief set the mp
	 */
	void set_mp(uint16_t mp) { mp_ = mp; }
	/**
	 * @brief set the add_hp
	 */
	void set_add_hp(uint16_t add_hp) { add_hp_ = add_hp; }
	/**
	 * @brief set the add_mp
	 */
	void set_add_mp(uint16_t add_mp) { add_mp_ = add_mp; }

	/**
	 * @brief set the add_hp
	 */
	void set_add_per_hp(uint16_t add_per_hp) { add_per_hp_ = add_per_hp; }
	/**
	 * @brief set the add_mp
	 */
	void set_add_per_mp(uint16_t add_per_mp) { add_per_mp_ = add_per_mp; }

	/**
	 * @brief set the slot
	 */
	void set_slot(uint16_t slot) { slot_ = slot; }
	/**
	 * @brief set the skill_id
	 */
	void set_skill_id(uint32_t skill_id) { skill_id_ = skill_id; }
	/**
	 * @brief set the repair_price
	 */
	void set_repair_price(uint32_t repair_price) { repair_price_ = repair_price; }
	/**
	 * @brief set the pvp_available_
	 */
	void set_pvp_available(uint32_t pvp_available) { pvp_available_ = pvp_available; }
	/**
	 * @brief set the fight_value_
	 */
	void set_fight_value(uint32_t fight_value) { fight_value_ = fight_value; }
	/**
	 * @brief set the summon_id
	 */
	void set_summon_id(uint32_t summon_id) { summon_id_ = summon_id; }
	/**
	 * @brief set the exploit_value_
	 */
	void set_exploit_value(uint32_t exploit_value) { exploit_value_ = exploit_value; }
	/**
	 * @brief set the exploit_value_
	 */
	void set_swap_action_id(uint32_t id) { swap_action_id_ = id; }
	/**
	 * @brief set the exploit_value_
	 */
	void set_honor_level(uint32_t honor_level) { honor_level_ = honor_level; }

	/**
	 * @brief set the equipment_set_id
	 */
	void set_suit_id(uint32_t id) { suit_id_ = id; };

	/**
	 * @brief set the effect_type_
	 */
	void set_effect_type(uint16_t type) { effect_type_ = type; };

	/**
	 * @brief set the effect_value_
	 */
	void set_effect_value(uint16_t value) { effect_value_ = value; };

	void set_item_name(char* name) { strncpy(itm_name, name, sizeof(itm_name));}

	void set_petattr(uint32_t petattr_id)
	{
		pet_attr_id_ = petattr_id;
	}
private:
	uint16_t equip_part_;
	uint16_t use_lv_;
	uint16_t hit_;
	uint16_t dodge_;
	uint16_t crit_;
	uint16_t hp_;
	uint16_t mp_;
	uint16_t add_hp_;
	uint16_t add_mp_;
	uint16_t add_per_hp_;
	uint16_t add_per_mp_;	
	uint16_t slot_;
	uint32_t skill_id_;
	uint32_t repair_price_;
	uint16_t fight_value_;
	uint32_t summon_id_;
	uint32_t pet_attr_id_;
	uint32_t pvp_available_;
	uint32_t exploit_value_;
	uint32_t swap_action_id_;
	uint32_t honor_level_;
	uint32_t suit_id_;
	uint16_t effect_type_;
	uint16_t effect_value_;
	char	 itm_name[128];
public:
	friend class GfItemManager;
};


/**
 * @brief init of GfItem
 */
inline void 
GfItem::init() 
{
	Item::init();
	equip_part_ = 0;
	use_lv_ = 0;
	hit_ = 0;
	dodge_ = 0;
	crit_ = 0;
	hp_ = 0;
	mp_ = 0;
	add_hp_ = 0;
	add_mp_ = 0;
	slot_ = 0;
	strength = 0;
	agility = 0;
	body_quality = 0;
	drop_lv = 0;
	stamina = 0;
	atk[0] = 0;
	atk[1] = 0;
	def = 0;
	duration = 0;
	interval = 0;
    speed_ratio = 0;
	
	repair_price_ = 0;
	fight_value_ = 0;
	summon_id_ = 0;
	pvp_available_ = 0;
	exploit_value_ = 0;
	swap_action_id_ = 0;
	suit_id_ = 0;
	memset(itm_name, 0, sizeof(itm_name));
}

/**
 * @brief 解析item配置文件，管理item。\n
 *        ITEM：可用Item或者继承自Item的派生类。\n
 *        MAX_CAT：指定ItemManager最大支持的item种类数目。\n
 *        MAX_ELEM：指定ItemManager每个item类别下最大支持的item个数。
 */
template <typename ITEM, int MAX_CAT, int MAX_ELEM>
class ItemManager {
public:
	/**
	 * @brief 构造函数，指定需要解析的item配置文件
	 */
	ItemManager(const char* file) : filename_1(file) { }
	/**
	 * @brief 构造函数，指定需要解析的item配置文件
	 */
	ItemManager(const char* file1, const char* file2, const char* file3) 
				: filename_1(file1), filename_2(file2), filename_3(file3) { }

	/**
	 * @brief 返回item_id指定的Item的指针
	 * @param item_id
	 * @return 如果找到item_id对应的物品，则返回指向该ITEM的指针，
	 *         如果找不到，则放回0。
	 */
	const ITEM* get_item(uint32_t item_id);
	/**
	 * @brief 对每个item执行func
	 */
	void for_each_item(int (*func)(const ITEM& item, void* in), void* in);
	/**
	 * @brief 解析item配置文件
	 */
	void load_items() 
	{ 
		if (filename_1.size()) load_item_file(filename_1); 
		if (filename_2.size()) load_item_file(filename_2); 
		if (filename_3.size()) load_item_file(filename_3); 
	}
	/**
	 * @brief 重新解析加载item配置文件
	 */
	void reload_items()
	{
		init_all_items();
		load_items();
	}
	/**
	 * @brief 重新解析item配置文件
	 */
	void load_item_file(std::string& filename_);
private:
	enum {
		/* base item id */
		item_base_item_id  = 100000
	};

	/**
	 * @brief 初始化items_
	 */
	void init_all_items();
	/**
	 * @brief 解析一条item配置
	 */
	void load_an_item(xmlNodePtr cur, uint32_t cat_id, uint32_t dbcat, uint32_t max);
	/**
	 * @brief 解析非公共的item配置。如果item配置表中有扩展的配置，
	 *        可以通过继承ItemManager，并且覆盖该函数来实现。
	 * @return 如果解析配置文件出错,则返回-1；成功则返回0。
	 */
	//virtual int load_item_extended_info(ITEM* item, xmlNodePtr cur) { return 0; }

	//ITEM items_[MAX_CAT][MAX_ELEM];

	std::map<uint32_t, ITEM> items_map;

	std::string filename_1;
	std::string filename_2;
	std::string filename_3;


public:
	/**
	  * @brief item category
	  */
	enum ItemCat {
		item_cat_monkey	= 1,
		item_cat_rabbit	= 2,
		item_cat_panda	= 3,

		/*! auxiliary equipment such as rings, necklace */
		item_cat_aux	= 11,

		/*! skill book*/
		item_skill_book	= 12,
		item_tonic	  	= 13,
		item_task	  		= 14,
		
		summon_food	  	= 16,

		item_shop		= 17,
	};

	/**
	 * @brief judge if the item is atk clothes
	 */
	bool is_atk_clothes(uint32_t equip_part)
		{ return equip_part == 4; }

	/**
	 * @brief judge if the item is def clothes
	 */
	bool is_def_clothes(uint32_t equip_part)
		{ 
			return ( equip_part == 1 || equip_part == 3  || equip_part == 6 ||
			 					equip_part == 7 || equip_part == 8  || equip_part == 9 ); 
		}

    /**
     * @brief judge if the item is mount
     */
    bool is_mount_clothes(uint32_t equip_part)
        { return equip_part == 11; }

	/**
	 * @brief judge if the item is jewelry clothes
	 */
	bool is_jew_clothes(uint32_t equip_part)
		{ return ( equip_part == 2 || equip_part == 5 ); }

	/**
	 * @brief judge if the item is clothes
	 */
	bool is_clothes(uint32_t category)
		{ return ( category >= item_cat_monkey && category <= item_cat_aux ); }
		
	/**
	 * @brief judge if the item is clothes
	 */
	bool is_clothes_can_wear(uint32_t category, uint32_t role_type)
		{ return ( category == role_type || category == item_cat_aux ); }
		
	/**
	 * @brief judge if the item is skill book
	 */
	bool is_skill_book(uint32_t category)
		{ return ( category == item_skill_book ); }

  /**
	 * @brief judge if the item is tonic
	 */
	bool is_tonic(uint32_t category)
		{ return ( category == item_tonic ); }
  
 	/**
	 * @brief judge if the item is tonic
	 */
	bool can_use(uint32_t category)
		{ return (category == item_tonic || category == item_shop || category == summon_food); };
	/**
	 * @brief judge if the item is item_task
	 */
	bool is_task_item(uint32_t category)
		{ return ( category == item_task ); }
		
	/**
	 * @brief judge if the item is summon food
	 */
	bool is_summon_food(uint32_t category)
		{ return ( category == summon_food ); }
	
  	/**
	 * @brief get o drop item's id
	 */
	uint32_t get_drop_itemid(uint32_t dp_lv);

  /**
	 * @brief get o drop clothes's id
	 */
	uint32_t get_drop_clothesid(uint32_t dp_lv, uint32_t quality, uint32_t part = 0, uint32_t role_type = 0);

private:
	enum {
		max_role_type = 3,
		max_drop_lv = 100,
		max_drop_quality = 6,
		max_equip_part = 10,
	};

	std::vector<uint32_t> drop_itm_vec[max_drop_lv];
	
	std::vector<uint32_t> drop_clothes_vec[max_role_type][max_drop_lv][max_drop_quality];
		
	std::vector<uint32_t> drop_clothes_vec2[max_role_type][max_drop_lv][max_drop_quality][max_equip_part];

	int load_item_extended_info(GfItem* item, xmlNodePtr cur);	
};

template <typename ITEM, int MAX_CAT, int MAX_ELEM>
inline void
ItemManager<ITEM, MAX_CAT, MAX_ELEM>::init_all_items()
{
	items_map.clear();
	/*int i, j;
	for (i = 0; i != MAX_CAT; ++i) {
		for (j = 0; j != MAX_ELEM; ++j) {
			items_[i][j].init();
		}
	}*/
}

template <typename ITEM, int MAX_CAT, int MAX_ELEM>
inline const ITEM*
ItemManager<ITEM, MAX_CAT, MAX_ELEM>::get_item(uint32_t item_id)
{
	/*uint32_t idx1 = ((item_id - 1) / item_base_item_id);
	uint32_t idx2 = ((item_id - 1) % item_base_item_id);

	if ((idx1 < MAX_CAT) && (idx2 < MAX_ELEM) && (items_[idx1][idx2].id() == item_id)) {
		return &(items_[idx1][idx2]);
	} else {
		return 0;
	}*/
	

	typename std::map<uint32_t, ITEM>::iterator it = items_map.find(item_id);
	if (it != items_map.end()) {
		return&(it->second);
	}

	return 0;
}

template <typename ITEM, int MAX_CAT, int MAX_ELEM>
void
ItemManager<ITEM, MAX_CAT, MAX_ELEM>::for_each_item(int (*func)(const ITEM& item, void* in), void* in)
{
	typename std::map<uint32_t, ITEM>::iterator it = items_map.begin();
	for (; it != items_map.end(); ++it) {
		int err = func(it->second, in);
		if (err) {
			char msg[256];
			sprintf(msg, "for_each_item itemid [%u]", it->second.id());
			throw ItemParseError(std::string(msg));
		}
	}
}

template <typename ITEM, int MAX_CAT, int MAX_ELEM>
void
ItemManager<ITEM, MAX_CAT, MAX_ELEM>::load_item_file(std::string& filename_)
{
	xmlDocPtr doc = xmlParseFile(filename_.c_str());
	if (!doc) {
		throw ItemParseError(std::string("failed to parse item file '") + filename_ + "'");
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		xmlFreeDoc(doc);
		throw ItemParseError(std::string("xmlDocGetRootElement error when loading item file '") + filename_ + "'");
	}

	// load items from an xml file
	uint32_t id, max, dbcatid;
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Cat"))) {
			// Category ID
			taomee::get_xml_prop(id, cur, "ID");
			taomee::get_xml_prop(dbcatid, cur, "DbCatID");
			taomee::get_xml_prop(max, cur, "Max");
			if (id >= MAX_CAT) {
				std::ostringstream oss;
				oss << "invalid category id: " << id;
				throw ItemParseError(oss.str());
			}
			// load configs for each item
			load_an_item(cur->xmlChildrenNode, id, dbcatid, max);
		}
		cur = cur->next;
	}

	xmlFreeDoc(doc);
}

/**
 * @brief get o drop item's id
 */
template <typename ITEM, int MAX_CAT, int MAX_ELEM>
uint32_t
ItemManager<ITEM, MAX_CAT, MAX_ELEM>::get_drop_itemid(uint32_t dp_lv)
{
	if (dp_lv > max_drop_lv || dp_lv ==0) {
		return 0;
	}

	uint32_t vec_size = drop_itm_vec[dp_lv - 1].size();
	if (vec_size == 0) {
		return 0;
	}

	uint32_t idx = rand() % vec_size;
	return drop_itm_vec[dp_lv - 1][idx];

}

/**
 * @brief get a drop clothes's id
 */
template <typename ITEM, int MAX_CAT, int MAX_ELEM>
uint32_t
ItemManager<ITEM, MAX_CAT, MAX_ELEM>::get_drop_clothesid(uint32_t dp_lv, uint32_t quality, uint32_t part, uint32_t role_type)
{
	//printf("[%u %u %u %u]\n",dp_lv,quality,part,role_type);
	if (dp_lv > max_drop_lv || quality > max_drop_quality ||
			dp_lv ==0 || quality == 0 || 
			(part !=0 && part > max_equip_part) || 
			(role_type !=0 && role_type > max_role_type) ) {
		return 0;
	}

	uint32_t role = role_type;
	if (role_type == 0) {
			role = rand() % max_role_type + 1;
	}
	
	if (part == 0) {
		uint32_t vec_size = drop_clothes_vec[role - 1][dp_lv - 1][quality - 1].size();
		//printf("vec_size[%u]\n",vec_size);
		if (vec_size == 0) {
			return 0;
		}

		uint32_t idx = rand() % vec_size;
		return drop_clothes_vec[role - 1][dp_lv - 1][quality - 1][idx];
	}
	
	//by equip_part
	uint32_t vec_size = drop_clothes_vec2[role - 1][dp_lv - 1][quality - 1][part - 1].size();
	//printf("vec_size[%u]\n",vec_size);
	if (vec_size == 0) {
		return 0;
	}

	uint32_t idx = rand() % vec_size;
	return drop_clothes_vec2[role - 1][dp_lv - 1][quality - 1][part - 1][idx];
}

/**
 * @brief load item extended info
 */
template <typename ITEM, int MAX_CAT, int MAX_ELEM>
int
ItemManager<ITEM, MAX_CAT, MAX_ELEM>::load_item_extended_info(GfItem* item, xmlNodePtr cur)
{
	if ( is_clothes(item->category()) ){
		uint16_t equip_part;
		uint16_t use_lv;
		uint16_t hit;
		uint16_t dodge;
		uint16_t crit;
		uint16_t hp;
		uint16_t mp;
		uint16_t add_hp;
		uint16_t add_mp;
		uint16_t add_per_hp;
		uint16_t add_per_mp;
		uint16_t slot;
		uint32_t repair_price;
		uint32_t exploit_value;
		uint32_t suit_id;
		uint32_t honor_level;
		uint32_t effect_type;
		uint32_t effect_value = 0;
		uint32_t swap_action_id = 0;
		
		
		taomee::get_xml_prop_def(equip_part, cur, "EquipPart", 0);
		taomee::get_xml_prop_def(use_lv, cur, "UseLv", 0);
		taomee::get_xml_prop_def(hit, cur, "Hit", 0);
		taomee::get_xml_prop_def(dodge, cur, "Dodge", 0);
		taomee::get_xml_prop_def(crit, cur, "Crit", 0);
		taomee::get_xml_prop_def(hp, cur, "Hp", 0);
		taomee::get_xml_prop_def(mp, cur, "Mp", 0);
		taomee::get_xml_prop_def(add_per_hp, cur, "AddPerHp", 0);
		taomee::get_xml_prop_def(add_per_mp, cur, "AddPerMp", 0);
		taomee::get_xml_prop_def(add_hp, cur, "AddHp", 0);
		taomee::get_xml_prop_def(add_mp, cur, "AddMp", 0);
		taomee::get_xml_prop_def(slot, cur, "Slot", 0);
		taomee::get_xml_prop_def(repair_price, cur, "RepairPrice", 0);
		taomee::get_xml_prop_def(exploit_value, cur, "ExploitValue", 0);
		taomee::get_xml_prop_def(honor_level, cur, "honorLevel", 0);
		taomee::get_xml_prop_def(suit_id, cur, "SetID", 0);
		taomee::get_xml_prop_def(effect_type, cur, "EffectType", 0);
		taomee::get_xml_prop_def(effect_type, cur, "EffectValue", 0);
		taomee::get_xml_prop_def(swap_action_id, cur, "SwapActionID", 0);
		//char tmp_name[12800] = "";
		//taomee::get_xml_prop_raw_str_def(tmp_name, cur, "Name", "");
		//TRACE_LOG("[%u %u %u %u %u %u %u %u %u]",use_lv,hit,dodge,crit,hp,mp,add_hp,add_mp,slot);

		item->set_equip_part(equip_part);
		item->set_use_lv(use_lv);
		item->set_hit(hit);
		item->set_dodge(dodge);
		item->set_crit(crit);
		item->set_hp(hp);
		item->set_mp(mp);
		item->set_add_hp(add_hp);
		item->set_add_mp(add_mp);
		item->set_add_per_hp(add_per_hp);
		item->set_add_per_mp(add_per_mp);

		item->set_slot(slot);
		item->set_repair_price(repair_price);
		item->set_exploit_value(exploit_value);
		item->set_honor_level(honor_level);
		item->set_suit_id(suit_id);
		item->set_effect_type(effect_type);
		item->set_effect_value(effect_value);
		//item->set_item_name(tmp_name);
		item->set_swap_action_id(swap_action_id);
		

		
		taomee::get_xml_prop_def(item->strength, cur, "Strength", 0);
		taomee::get_xml_prop_def(item->agility, cur, "Agility", 0);
		taomee::get_xml_prop_def(item->body_quality, cur, "BodyQuality", 0);
		taomee::get_xml_prop_def(item->quality_lv, cur, "QualityLevel", 0);
		taomee::get_xml_prop_def(item->drop_lv, cur, "DropLv", 0);
		taomee::get_xml_prop_def(item->stamina, cur, "Stamina", 0);
		int arr_len = taomee::get_xml_prop_arr_def(item->atk, cur, "Atk", 0);
		if( arr_len != 2 && arr_len != 0 ) {
			return -1;
		} else if( item->atk[0] > item->atk[1]){
			return -1;
		}
		taomee::get_xml_prop_def(item->def, cur, "Def", 0);
		taomee::get_xml_prop_def(item->duration, cur, "Duration", 0);
		taomee::get_xml_prop_def(item->speed_ratio, cur, "Speed", 0);

		//drop clothes
		if (item->quality_lv && item->drop_lv) {
			if (item->quality_lv > max_drop_quality || item->drop_lv > max_drop_lv || item->equip_part() > max_equip_part) {
				return -1;
			}
			
			if (item->category() <= max_role_type) {
				drop_clothes_vec[item->category()-1][item->drop_lv-1][item->quality_lv-1].push_back(item->id());
				drop_clothes_vec2[item->category()-1][item->drop_lv-1][item->quality_lv-1][item->equip_part()-1].push_back(item->id());
			} else {
				for (uint32_t i = 0; i < max_role_type; i++) {
					drop_clothes_vec[i][item->drop_lv-1][item->quality_lv-1].push_back(item->id());
					drop_clothes_vec2[i][item->drop_lv-1][item->quality_lv-1][item->equip_part()-1].push_back(item->id());
				}
			}
			
		}

	} else if ( is_skill_book(item->category()) ) {
		uint16_t use_lv;
		uint32_t skill_id;

		taomee::get_xml_prop_def(use_lv, cur, "UseLv", 0);
		taomee::get_xml_prop_def(skill_id, cur, "SkillID", 0);

		item->set_use_lv(use_lv);
		item->set_skill_id(skill_id);
	} else if ( is_tonic(item->category()) || is_summon_food(item->category()) ) {
		uint16_t use_lv;
		uint16_t add_hp;
		uint16_t add_mp;
		uint16_t add_per_hp;
		uint16_t add_per_mp;

		uint16_t fight_value;
		uint32_t pvp_available;
		
		
		taomee::get_xml_prop_def(item->drop_lv, cur, "DropLv", 0);
		taomee::get_xml_prop_def(item->duration, cur, "Duration", 0);
		taomee::get_xml_prop_def(item->interval, cur, "Interval", 1);
		taomee::get_xml_prop_def(use_lv, cur, "UseLv", 0);
		taomee::get_xml_prop_def(add_hp, cur, "AddHp", 0);
		taomee::get_xml_prop_def(add_mp, cur, "AddMp", 0);
		taomee::get_xml_prop_def(add_per_hp, cur, "AddPerHp", 0);
		taomee::get_xml_prop_def(add_per_mp, cur, "AddPerMp", 0);		
		taomee::get_xml_prop_def(fight_value, cur, "FightValue", 0);
		taomee::get_xml_prop_def(pvp_available, cur, "PvpAvailable", 0);
		

		item->set_use_lv(use_lv);
		item->set_add_hp(add_hp);
		item->set_add_mp(add_mp);
		item->set_add_per_hp(add_per_hp);
		item->set_add_per_mp(add_per_mp);

		item->set_fight_value(fight_value);
		item->set_pvp_available(pvp_available);
		
		
		//drop items
		if (item->drop_lv) {
			if (item->drop_lv > max_drop_lv) {
				return -1;
			}
			drop_itm_vec[item->drop_lv-1].push_back(item->id());
		}
	} else if ( is_task_item(item->category()) ) {
		
	}
	uint32_t summon_id;	
	taomee::get_xml_prop_def(summon_id, cur, "SummonID", 0);
	item->set_summon_id(summon_id);
	uint32_t petattr_id;
	taomee::get_xml_prop_def(petattr_id, cur, "PetAttrID", 0);
	item->set_petattr(petattr_id);

    char tmp_name[12800] = "";
    taomee::get_xml_prop_raw_str_def(tmp_name, cur, "Name", "");
    item->set_item_name(tmp_name);
    return 0;
}


template <typename ITEM, int MAX_CAT, int MAX_ELEM>
void
ItemManager<ITEM, MAX_CAT, MAX_ELEM>::load_an_item(xmlNodePtr cur, uint32_t cat_id, uint32_t dbcat, uint32_t max)
{
	uint32_t id, idx, flag;
	uint32_t base_id = (cat_id * item_base_item_id) + 1;
	//ITEM*    items   = items_[cat_id];
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Item")) {
			taomee::get_xml_prop(id, cur, "ID");

			idx = id - base_id;
			/*if ((id < base_id) || ((idx) >= MAX_ELEM) || items[idx].id()) {
				std::ostringstream oss;
				oss << "invalid/duplicated item: id=" << id << " catid=" << cat_id;
				throw ItemParseError(oss.str());
			}*/
			
			typename std::map<uint32_t, ITEM>::iterator it = items_map.find(id);
			if ( it != items_map.end() || (id < base_id) || ((idx) >= MAX_ELEM) ) {
				std::ostringstream oss;
				oss << "invalid/duplicated item: id=" << id << " catid=" << cat_id;
				throw ItemParseError(oss.str());
			}

			ITEM  itm;
			ITEM* item = &(itm);

			item->set_id(id);
			item->set_category(cat_id);
			item->set_db_category(id, dbcat);

			uint32_t tmp;
			taomee::get_xml_prop_def(tmp, cur, "Max", max);
			item->set_max(tmp);
			taomee::get_xml_prop_def(tmp, cur, "Price", 0);
			item->set_price(tmp);
			taomee::get_xml_prop_def(tmp, cur, "SellPrice", 0);
			item->set_sellprice(tmp);
            taomee::get_xml_prop_def(tmp, cur, "LifeTime", 0);
			item->set_lifetime(tmp);
			
			//tradable between players	
			taomee::get_xml_prop_def(tmp, cur, "Tradable", 0);
			item->set_tradable(tmp);
			// decode flag
			taomee::get_xml_prop_def(flag, cur, "Tradability", 3);
			if (taomee::test_bit_on(flag, 1)) {
				item->set_item_all_buyable();
			}
			if (taomee::test_bit_on(flag, 2)) {
				item->set_item_all_sellable();
			}
			taomee::get_xml_prop_def(flag, cur, "VipTradability", 3);
			if (taomee::test_bit_on(flag, 1)) {
				item->set_item_vip_buyable();
			}
			if (taomee::test_bit_on(flag, 2)) {
				item->set_item_vip_sellable();
			}
			taomee::get_xml_prop_def(flag, cur, "VipOnly", 0);
			if (flag) {
				item->set_item_vip_only();
			}
            taomee::get_xml_prop_def(flag, cur, "Shop", 0);
            if (flag) {
                item->set_item_shop_sale();
            }
			taomee::get_xml_prop_def(flag, cur, "BuffID", 0);
			if (flag) {
				item->set_item_buf_type(flag);
			}
			taomee::get_xml_prop_def(flag, cur, "BuffTarget", 0);
			if (flag) {
				item->set_item_buf_target(flag);
			}
			taomee::get_xml_prop_def(flag, cur, "KeepTm", 20);
			if (flag) {
				item->set_item_keep_tm(flag);
			}
			taomee::get_xml_prop_def(flag, cur, "Duration", 0);
			if (flag) {
				item->set_item_buf_duration(flag);
			}
			taomee::get_xml_prop_def(flag, cur, "PerTrim", 0);
			if (flag) {
				item->set_item_buf_per_trim(flag);
			}
			taomee::get_xml_prop_def(flag, cur, "ExID1", 0);
			if (flag) {
				item->set_item_buf_ex_id1(flag);
			}
			taomee::get_xml_prop_def(flag, cur, "MutexType", 0);
			if (flag) {
				item->set_mutex_type(flag);
			}
			
			taomee::get_xml_prop_def(flag, cur, "decompose", 0);
			if (flag) {
				item->set_decompose(flag);
			}
			// load extended info of an item
			if (load_item_extended_info(item, cur) != 0) {
				std::ostringstream oss;
				oss << "error occur while loading extended item info: id=" << id;
				throw ItemParseError(oss.str());
			}

			items_map.insert(typename std::map<uint32_t, ITEM>::value_type(id, itm));
		}
	
		cur = cur->next;
	}
}



//} // end of namespace taomee

#endif // ITEM_MANAGER_HPP_

