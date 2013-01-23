#ifndef _ITEM_DATA_HPP_
#define _ITEM_DATA_HPP_

#include<map>
#include<vector>

using namespace std;

enum
{
	none_equip_pos = 0,
	head_equip_pos = 100000,
	eye_equip_pos,
	glass_equip_pos,
	body_equip_pos,
	tail_equip_pos,
	suit_equip_pos
};

#define CHEKC_VALID_EQUIP_POS( pos )  \
	(pos >= head_equip_pos && pos <= suit_equip_pos)

enum
{
	task_item_category = 1<<0,   		//任务道具
	equip_item_category = 1<<1, 		//装备类
	consume_item_category = 1<<2,		//消耗类
	formula_item_catetory = 1<<3,		//配方材料
	material_item_catetory = 1<<4,		//材料类
	nomarl_item_catetory  = 1<<5,       //普通物品
	static_period_item_catetory = 1<<6, //不能修改有效期的物品
	dynamic_period_item_catetory = 1<<7 //可以修改有效期的物品
};

enum
{
	normal_buy_flag  = 1<< 0,
	normal_sell_falg = 1<< 1,
	vip_buy_flag     = 1<< 2,
	vip_sell_flag    = 1<< 3
};

enum 
{
	discard_flag = 1<< 0,
	destroy_flag = 1<< 1
};


struct item_effect_t
{
    item_effect_t()
    {
        effect_id = 0;
        memset(effect_value, 0, sizeof(effect_value));
    }

    uint32_t effect_id;
    uint32_t effect_value[2];
};

typedef struct item_data
{
	item_data()
    {
        item_id_ = 0;
        memset(name_, 0, sizeof(name_));
        category_ = 0;
        sell_price_ = 0;
        buy_price_ = 0;
        discard_flag_ = 0;
        trade_flag_ = 0;
        max_heap_num_ = 0;
        max_num_ = 0;
        equip_pos_ = 0;
        duration_time_ = 0;
        end_time_ = 0;

        magic_ = 0;
        agility_ = 0;
        luck_ = 0;
        strength_ = 0;
        crit_ = 0;
        crit_damage_ = 0;
        physique_ = 0;
        tenacity_ = 0;
        hp_ = 0;
        atk_ = 0;
        def_ = 0;

    }

	bool check_trade_flag(uint32_t flag)
	{
		return trade_flag_ & flag;	
	}

	bool check_discard_flag(uint32_t flag)
	{
		return discard_flag_ & flag;	
	}

	bool check_category_flag(uint32_t flag)
	{
		return category_ & flag;	
	}

	uint32_t max_heap_num()
	{
		return max_heap_num_;	
	}

	bool check_category(uint32_t cat)
	{
		return category_ & cat;
	}

    item_effect_t * get_item_effect()
    {
        return &effect;
    }
	
	uint32_t item_id_;       //物品ID
	uint32_t name_[1024];    //物品名字
	uint32_t category_;	 	 //类别 见XML
	uint32_t sell_price_; 	 //卖出价格
	uint32_t buy_price_;   	 //买入价格
	uint32_t discard_flag_;  //丢弃FLAG
	uint32_t trade_flag_;    //交易FLAG
	uint32_t max_heap_num_;  //最大堆叠数量
	uint32_t max_num_;       //最大拥有量
	uint32_t equip_pos_;     //装备位置
	uint32_t duration_time_; //有效期时间段
	uint32_t end_time_;      //有效期时间点

	uint32_t magic_;            //附加魔力
	uint32_t agility_;          //附加敏捷
	uint32_t luck_;             //附加幸运
	uint32_t strength_;         //附加体力
	uint32_t crit_;             //附加暴击率
	uint32_t crit_damage_;      //附加暴击伤害
	uint32_t physique_;         //附加体质
	uint32_t tenacity_;         //附加韧性
	uint32_t hp_;               //附加血量
	uint32_t atk_;              //附加攻击
	uint32_t def_;              //附加防御

    item_effect_t effect;
}item_data;


class item_data_mgr
{
private:
	item_data_mgr(){}
	~item_data_mgr(){}
public:
	static item_data_mgr* get_instance();
public:
	bool init_xml(const char* xml);
	bool final();
	bool add_item_data(item_data* data);
	bool del_item_data(uint32_t item_id);
	bool is_item_data_exist(uint32_t item_id);
	item_data* get_item_data_by_id(uint32_t item_id);
private:
	std::map<uint32_t, item_data*> data_maps;
};


class item
{
public:
	item(uint32_t id, uint32_t item_id, uint32_t heap_count, uint32_t index, uint32_t get_time);
	~item();
public:
	void to_next(item* p_item)
	{
		p_next_item = p_item;	
	}
	void to_prev(item* p_item)
	{
		p_prev_item = p_item;
	}
	void clear_to_item()
	{
		if(p_prev_item == NULL && p_next_item == NULL)//唯一结点
		{

		}
		if( p_prev_item != NULL && p_next_item != NULL)//中间结点
		{
			item* prev = p_prev_item;
			item* next = p_next_item;

			prev->to_next(next);
			next->to_prev(prev);

			p_prev_item = NULL;
			p_next_item = NULL;
		}
		if( p_prev_item == NULL && p_next_item != NULL)//头节点
		{
			p_next_item->p_prev_item = NULL;
			p_next_item = NULL;
		}
		if( p_prev_item != NULL && p_next_item == NULL)//尾结点
		{
			p_prev_item->p_next_item = NULL;
			p_prev_item = NULL;
		}

	}
	item*    p_next_item;       //指向包裹内下个相同的堆叠物品	
	item*    p_prev_item;       //指向包裹内上个相同的堆叠物品
public:
	uint32_t item_id()
	{
		return p_data->item_id_;
	}

	uint32_t get_max_heap_count()
	{
		return p_data->max_heap_num_;	
	}
	
	uint32_t equip_pos()
	{
		return p_data->equip_pos_;	
	}

	uint32_t grid_index()
	{
		return bag_index_;	
	}

	void set_grid_index(uint32_t index)
	{
		bag_index_ = index;	
	}

	uint32_t get_cur_remain_heap_count()
	{
		return p_data->max_heap_num_ - cur_heap_count_;
	}

	uint32_t get_cur_heap_count()
	{
		return cur_heap_count_;	
	}

	bool inc_cur_heap_count(uint32_t count)
	{
		if(cur_heap_count_ + count <=  p_data->max_heap_num() )
		{
			cur_heap_count_ += count;
			return true;
		}
		return false;
	}

	bool dec_cur_heap_count(uint32_t count)
	{
		if(cur_heap_count_ >= count)
		{
			cur_heap_count_ -= count;
			return true;
		}
		return false;
	}

	bool  check_cur_heap_full()
	{
		return get_cur_remain_heap_count() == 0	;
	}
	
	uint32_t get_time()
	{
		return get_time_;	
	}

	item_data* get_item_data()
	{
		return p_data;	
	}

	uint32_t get_item_magic()
	{
		return p_data->magic_;	
	}

	uint32_t get_item_agility()
	{
		return p_data->agility_;
	}

	uint32_t get_item_luck()
	{
		return p_data->luck_;	
	}

	uint32_t get_item_strength()
	{
		return p_data->strength_;	
	}

	uint32_t get_item_crit()
	{
		return p_data->crit_;	
	}

	uint32_t get_item_crit_damage()
	{
		return p_data->crit_damage_;	
	}

	uint32_t get_item_physique()
	{
		return p_data->physique_;	
	}

	uint32_t get_item_tenacity()
	{
		return p_data->tenacity_;	
	}
	uint32_t get_item_hp()
	{
		return p_data->hp_;	
	}

	uint32_t get_item_atk()
	{
		return p_data->atk_;	
	}

	uint32_t get_item_def()
	{
		return p_data->def_;	
	}

	uint32_t get_expire_time()
	{
		return expire_time_;
	}

	void set_expire_time(uint32_t time)
	{
		expire_time_ = time;	
	}
public:
	uint32_t id_;               //唯一ID
	uint32_t cur_heap_count_;   //当前堆叠数量
	uint32_t bag_index_;        //在包包里的位置
	uint32_t get_time_;         //获取时间
	uint32_t expire_time_;      //过期时间，永久为0
	uint8_t hide;				//物品隐藏属性1隐藏，0显示
	item_data* p_data;
};

typedef item* item_ptr;

class item_factory
{
private:
	item_factory(){}
	~item_factory(){}
public:
	static item_factory* get_instance();
public:
	item*	create_item(uint32_t id, uint32_t item_id, uint32_t heap_count, uint32_t index, uint32_t get_time, uint32_t expire_time);
	void	destroy_item(item* p_item);
};


uint32_t str_2_unix_time(const char* timestr);












#endif
