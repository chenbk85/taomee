/** 
 * ========================================================================
 * @file pet_data.hpp
 * @brief 
 * @version 1.0
 * @date 2012-04-13
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#ifndef H_PET_DATA_H_2012_04_13
#define H_PET_DATA_H_2012_04_13

#include "pea_utils.hpp"
#include "container.hpp"
#include "calculator.hpp"
#include "constant.hpp"
#include "obj_attr_data.hpp"
#include "exp2level.hpp"
#include "singleton.hpp"


/////////////////////////// xml 数据解析 ///////////////////////////////
////////////////////////////////////////////////////////////////////////


///////////////////// 精灵的数据 //////////////////////////////

struct pet_attr_config_t
{
    pet_attr_config_t()
    {
        reset();
    }
    void reset()
    {
        memset(this, 0, sizeof(*this));
    }

    void copy(pet_attr_config_t * p_data)
    {
        memcpy(this, p_data, sizeof(pet_attr_config_t));
    }

    void accumulate(pet_attr_config_t * p_data)
    {
        hp_ += p_data->hp_;
        magic_ += p_data->magic_;
        agility_ += p_data->agility_;
        luck_ += p_data->luck_;
        physique_ += p_data->physique_;
        anger_ += p_data->anger_;
        atk_ += p_data->atk_;
        def_ += p_data->def_;
        strength_ += p_data->strength_;
        happy_ += p_data->happy_;
        soul_ += p_data->soul_;
        intensity_ += p_data->intensity_;
        crit_ += p_data->crit_;
        double_hit_ += p_data->double_hit_;
        speed_ += p_data->speed_;
        crit_damage_ += p_data->crit_damage_;
        tenacity_ += p_data->tenacity_;

    }

    float   hp_;          //血量
    uint32_t exp_;         //经验
    uint32_t level_;       //级别
    float   magic_;       //魔力
    float   agility_;     //敏捷
    float   luck_;        //幸运
    float   anger_;       //怒气
    float   strength_;    //体力
    float   atk_;         //攻击力
    float   def_;         //防御力
    float   happy_;       //快乐
    float   soul_;        //灵魂值
    float   intensity_;   //强度系数
    float   crit_;        //暴击
    float   double_hit_;  //连击
    float   speed_;       //移动速度
    float   crit_damage_; //暴击伤害
    float   tenacity_;    //韧性,减少暴击伤害
    float   physique_;    //体质

    // 精灵天赋数量
    uint32_t gift_count;
    // 精灵血脉数量
    uint32_t badge_count;
};




// 精灵等级为key
class c_pet_config : public c_object_container<uint32_t, pet_attr_config_t>
{
    public:
		uint32_t width;
		uint32_t height;

		uint32_t skill_1;
        uint32_t skill_2;
        uint32_t skill_3;
        // 绝招
        uint32_t uni_skill;

};

// 精灵id为key
class c_pet_config_mgr : public c_object_container<uint32_t, c_pet_config>
{

};

typedef singleton_default<c_pet_config_mgr> pet_config_mgr;


int init_pet_config_data(xmlNodePtr root);






/////////////// iq 成长 /////////////////


struct pet_iq_config_t
{
    pet_iq_config_t()
    {
        cost = 0;
        prob = 0;
    }

    // 聪明豆消耗
    uint32_t cost;
    // 成功概率
    uint32_t prob;
};

// iq为key
class c_pet_iq_config_mgr : public c_object_container<uint32_t, pet_iq_config_t>
{

};

typedef singleton_default<c_pet_iq_config_mgr> pet_iq_config_mgr;


int init_pet_iq_config_data(xmlNodePtr root);




/////////////////// 精灵培养 ///////////////////////


struct pet_train_consume_config_t
{
    pet_train_consume_config_t()
    {
        gold = 0;
        item_id = 0;
        item_num = 0;
    }
    // 培养消耗金币
    uint32_t gold;
    // 培养消耗的道具和数量
    uint32_t item_id;
    uint32_t item_num;

};

struct pet_train_attr_config_t
{
    pet_train_attr_config_t()
    {
        attr_id = 0;
        min_step = 0;
        max_step = 0;
        max_rate = 0;
    }

    uint32_t attr_id;
    // 每次培养随机到的附加值的下限
    int32_t min_step;
    // 每次培养随机到的附加值的上限
    int32_t max_step;
    // 培养获得的附加值的总和的倍数上限
    // 基数是精灵基础属性
    float max_rate;
};

struct pet_train_config_t
{
    pet_train_consume_config_t * get_consume_config(uint32_t level)
    {
        if (level < MAX_PET_LEVEL)
        {
            return consume_config + level;
        }

        return NULL;
    }

    pet_train_attr_config_t * get_attr_config(uint32_t index)
    {
        if (index < PET_TRAIN_ATTR_NUM)
        {
            return attr_config + index;
        }

        return NULL;
    }


    pet_train_consume_config_t consume_config[MAX_PET_LEVEL];

    pet_train_attr_config_t attr_config[PET_TRAIN_ATTR_NUM];

};

class c_pet_train_config_mgr : public c_object_container<uint32_t, pet_train_config_t>
{

};

typedef singleton_default<c_pet_train_config_mgr> pet_train_config_mgr;



int init_pet_train_config_data(xmlNodePtr root);




////////////////////////// 精灵天赋 /////////////////////////


struct pet_gift_config_t
{

};

class c_pet_gift_config_mgr : public c_object_container<uint32_t, pet_gift_config_t>
{

};


typedef singleton_default<c_pet_gift_config_mgr> pet_gift_config_mgr;


int init_pet_gift_config_data(xmlNodePtr root);



/////////////// 精灵天赋升级 ////////////////////

struct pet_gift_upgrade_config_t
{
    uint32_t item_id;
    uint32_t item_num;
};


class c_pet_gift_upgrade_config_mgr : public c_object_container<uint32_t, pet_gift_upgrade_config_t>
{

};

typedef singleton_default<c_pet_gift_upgrade_config_mgr> pet_gift_upgrade_config_mgr;

int init_pet_gift_upgrade_config_data(xmlNodePtr root);


/////////////// 精灵天赋随机 ////////////////

struct pet_gift_rand_config_t
{
    uint32_t item_id;
    uint32_t item_num;
};

class c_pet_gift_rand_config_mgr : public c_object_container<uint32_t, pet_gift_rand_config_t>
{

};

typedef singleton_default<c_pet_gift_rand_config_mgr> pet_gift_rand_config_mgr;

int init_pet_gift_rand_config_data(xmlNodePtr root);

class c_pet_gift_rand_prob_config_mgr : public c_object_container<uint32_t, uint32_t>
{

};

typedef singleton_default<c_pet_gift_rand_prob_config_mgr> pet_gift_rand_prob_config_mgr;

int init_pet_gift_rand_prob_config_data(xmlNodePtr root);

//////////////// 精灵天赋消耗 //////////////


int init_pet_gift_consume_config_data(xmlNodePtr);





//////////////////////// 精灵幻化 /////////////////////////

struct pet_merge_prob_config_t
{
    pet_merge_prob_config_t()
    {
        base_prob = 0;
        assist_prob = 0;
    }
    uint32_t base_prob;
    uint32_t assist_prob;
};


struct pet_merge_compensation_config_item_t
{
    pet_merge_compensation_config_item_t()
    {
        item_id = 0;
        item_num = 0;
        expire_time = 0;
        rate = 0;
    }
    uint32_t item_id;
    uint32_t item_num;
    uint32_t expire_time;
    uint32_t rate;
};



struct pet_merge_compensation_config_t
{
    std::vector<pet_merge_compensation_config_item_t> items;

};


class c_pet_merge_prob_config_mgr : public c_object_container<uint32_t, pet_merge_prob_config_t>
{

};


class c_pet_merge_compensation_config_mgr : public c_object_container<uint32_t, pet_merge_compensation_config_t>
{

};


typedef singleton_default<c_pet_merge_prob_config_mgr> pet_merge_prob_config_mgr;

typedef singleton_default<c_pet_merge_compensation_config_mgr> pet_merge_compensation_config_mgr;



int init_pet_merge_prob_config_data(xmlNodePtr root);

int init_pet_merge_compensation_config_data(xmlNodePtr root);

int init_pet_merge_config_data(xmlNodePtr root);

/////////////////////////// 以上是xml读取 //////////////////////////////






///////////////////////////// 精灵数据 /////////////////////////////////











class c_pet_data;

// 精灵状态
enum
{
    PET_STATUS_BEGIN = 0,
    // 出战
    PET_STATUS_FIGHT = 1,
    // 辅助
    PET_STATUS_ASSIST = 2,
    // 背包
    PET_STATUS_BAG = 3,

    PET_STATUS_END
};


// 精灵的属性
class c_pet_attr : public obj_attr
{
    public:

        c_pet_attr()
            : obj_attr()
        {

        }

        c_pet_attr(pet_attr_config_t & data)
            : obj_attr()
        {
			attr_arr[OBJ_ATTR_HP] = data.hp_;
			attr_arr[OBJ_ATTR_EXP] = data.exp_;
			attr_arr[OBJ_ATTR_LEVEL] = data.level_;
			attr_arr[OBJ_ATTR_MAGIC] = data.magic_;
			attr_arr[OBJ_ATTR_AGILITY] = data.agility_;
			attr_arr[OBJ_ATTR_LUCK] = data.luck_;
			attr_arr[OBJ_ATTR_ANGER] = data.anger_;
			attr_arr[OBJ_ATTR_STRENGTH] = data.strength_;
			attr_arr[OBJ_ATTR_ATK] = data.atk_;
			attr_arr[OBJ_ATTR_DEF] = data.def_;
			attr_arr[OBJ_ATTR_HAPPY] = data.happy_;
			attr_arr[OBJ_ATTR_SOUL] = data.soul_;
			attr_arr[OBJ_ATTR_INTENSITY] = data.intensity_;
			attr_arr[OBJ_ATTR_CRIT] = data.crit_;
			attr_arr[OBJ_ATTR_DOUBLE] = data.double_hit_;
			attr_arr[OBJ_ATTR_SPEED] = data.speed_;
			attr_arr[OBJ_ATTR_CRIT_DAMAGE] = data.crit_damage_;
			attr_arr[OBJ_ATTR_TENACITY] = data.tenacity_;
			attr_arr[OBJ_ATTR_PHYSIQUE] = data.physique_;
            m_gift_count = data.gift_count;
            m_badge_count = data.badge_count;
        }

        // 计算属性
        void calc()
        {
            attr_arr[OBJ_ATTR_HP] += attr_arr[OBJ_ATTR_PHYSIQUE] * 5;
            attr_arr[OBJ_ATTR_ATK] += attr_arr[OBJ_ATTR_MAGIC] * 0.18;


        }


    public:

        // 解锁的天赋数量
        uint32_t m_gift_count;
        // 血脉数量
        uint32_t m_badge_count;
};


typedef std::map<uint32_t, c_pet_attr *> c_pet_attr_map;




class c_pet_skill_data
{
    public:

        uint32_t m_skill_1;
        uint32_t m_skill_2;
		uint32_t m_skill_3;
        // 绝招
        uint32_t m_uni_skill;

};


struct pet_iq_data_t
{
    pet_iq_data_t()
    {
        item_id = 0;
        item_num = 0;
        prob = 0;
    }

    // 聪明豆id
    uint32_t item_id;
    // 聪明豆消耗
    uint32_t item_num;
    // 成功概率
    uint32_t prob;
};

class c_pet_iq_data_mgr : public c_object_container<uint32_t, pet_iq_data_t>
{

};

typedef singleton_default<c_pet_iq_data_mgr> pet_iq_data_mgr;


int init_pet_iq_data();




class c_pet_rand_calc
{

    public:

        int init(c_pet_data * p_pet, uint32_t level, c_pet_attr * p_attr, c_pet_attr * p_base_attr);

        // 根据p_value中给出的值，计算出p_attr
        int calc_attr(obj_attr * p_attr, uint32_t * p_value);


        // 计算出新的属性值，存放于p_des中
        // p_src用于提供老的属性值用于计算上限的参考
        int calc_new_attr(uint32_t iq, obj_attr * p_des, obj_attr * p_src);


        uint32_t get_max_value(uint32_t iq, uint32_t attr_id);

        int get_max_value_list(uint32_t iq, uint32_t * p_des);

    protected:

        uint32_t calc_max(uint32_t attr_id, c_pet_attr * p_attr, c_pet_attr * p_base_attr);

    private:


        c_pet_data * m_pet;

        uint32_t m_level;


    private:


        struct attr_data_t
        {
            // 成长的属性id
            uint32_t attr_id;
            // 属性值上限
            uint32_t max;
            // 每次成长的数值
            uint32_t step;
        };


        attr_data_t m_data[PET_RAND_ATTR_NUM];
};



struct pet_train_consume_data_t
{
    pet_train_consume_data_t()
    {
        gold = 0;
        item_id = 0;
        item_num = 0;
    }
    uint32_t gold;
    uint32_t item_id;
    uint32_t item_num;

};


class c_pet_train_calc
{

    public:

        int init(c_pet_data * p_pet, uint32_t level, c_pet_attr * p_attr);

        // 根据p_value中给出的值，计算出p_attr
        int calc_attr(obj_attr * p_attr, uint32_t * p_value);

        // 给定train_id，计算出培养的值，p_src中的值作为计算上限的参考
        int calc(uint32_t train_id, uint32_t * p_des, obj_attr * p_src);

        pet_train_consume_data_t * get_consume_data(uint32_t train_id);

    private:


        c_pet_data * m_pet;

        uint32_t m_level;



    private:

        struct attr_data_t
        {
            uint32_t attr_id;
            int32_t min_step;
            int32_t max_step;
            uint32_t max_value;
        };


        struct train_data_t
        {
            pet_train_consume_data_t consume;
            attr_data_t attr[PET_TRAIN_ATTR_NUM];

        };



        c_object_container<uint32_t, train_data_t> m_data;

};



////////////////////////// 精灵天赋 /////////////////////////////


struct pet_gift_consume_data_t
{
    uint32_t item_id;
    uint32_t item_num;
};


class c_pet_gift_upgrade_data_mgr : public c_object_container<uint32_t, pet_gift_consume_data_t>
{

};

typedef singleton_default<c_pet_gift_upgrade_data_mgr> pet_gift_upgrade_data_mgr;



class c_pet_gift_rand_data_mgr : public c_object_container<uint32_t, pet_gift_consume_data_t>
{

};

typedef singleton_default<c_pet_gift_rand_data_mgr> pet_gift_rand_data_mgr;


class c_pet_gift_rand_prob_calc : public c_prob_calculator<uint32_t>
{

};


typedef singleton_default<c_pet_gift_rand_prob_calc> pet_gift_rand_prob_calc;

int init_pet_gift_consume_data();


// 精灵相关的天赋数据，比如最多拥有的天赋数量等
class c_pet_gift_data
{
    public:

        // 最多拥有的天赋数量
        uint32_t m_gift_count;
};


// 天赋自身的数据，比如产生什么效果等
class c_pet_gift_info
{

};

class c_pet_gift_info_mgr : public c_object_container<uint32_t, c_pet_gift_info>
{

    public:

        int get_rand_gift(uint32_t * p_des_gift_id, uint32_t * p_des_gift_level, std::vector<uint32_t> & exclude_gift);
};


typedef singleton_default<c_pet_gift_info_mgr> pet_gift_info_mgr;


int init_pet_gift_info();


/////////////////////////////// 精灵幻化 //////////////////////////////


struct pet_merge_prob_data_t
{
    pet_merge_prob_data_t()
    {
        base_prob = 0;
        assist_prob = 0;
    }
    uint32_t base_prob;
    uint32_t assist_prob;
};


struct pet_merge_compensation_item_t
{
    pet_merge_compensation_item_t()
    {
        item_id = 0;
        item_num = 0;
        expire_time = 0;
        rate = 0;
    }
    uint32_t item_id;
    uint32_t item_num;
    uint32_t expire_time;
    uint32_t rate;
};



struct pet_merge_compensation_data_t
{
    std::vector<pet_merge_compensation_item_t> items;

};


class c_pet_merge_prob_data_mgr : public c_object_container<uint32_t, pet_merge_prob_data_t>
{

};


class c_pet_merge_compensation_data_mgr : public c_object_container<uint32_t, pet_merge_compensation_data_t>
{

};


typedef singleton_default<c_pet_merge_prob_data_mgr> pet_merge_prob_data_mgr;

typedef singleton_default<c_pet_merge_compensation_data_mgr> pet_merge_compensation_data_mgr;


int init_pet_merge_data();


/////////////////////////////// 精灵数据 //////////////////////////////


class c_pet_data
{
    public:

        c_pet_data(uint32_t id);

        int init();

        base_exp2level_calculator * get_exp2level_calc()
        {
            return &m_exp2level_calc;
        }

        c_pet_skill_data * get_skill_data()
        {
            return &m_skill_id_data;
        }

        c_pet_attr * get_base_attr(uint32_t level)
        {
            return m_base_attr_map.get_object(level);
        }


        c_pet_rand_calc * get_rand_calc(uint32_t level)
        {
            return m_rand_calc_map.get_object(level);
        }


        c_pet_train_calc * get_train_calc(uint32_t level)
        {
            return m_train_calc_map.get_object(level);
        }


        c_pet_gift_data * get_gift_data(uint32_t level)
        {
            return m_gift_data_map.get_object(level);
        }



        // c_pet_badge_set * get_badge_set(uint32_t quality)
        // {
        // c_id_set * p_set = m_badge_id_map.get_object(quality);
        // if (NULL == p_set)
        // {
        // return NULL;
        // }
        // else
        // {
        // c_pet_badge_set * p_badge_set = dynamic_cast<c_pet_badge_set *>(p_set);
        // return p_badge_set;
        // }
        // }

    public:

        typedef c_object_container<uint32_t, c_pet_attr> attr_map_t;


        uint32_t m_id;
		uint32_t height;
		uint32_t width;


    protected:

        uint32_t max_level()
        {
            return m_exp2level_calc.get_max_level();
        }

        int init_skill_data();

        int init_base_attr_data();

        int init_rand_calc();

        int init_train_calc();

        int init_gift_data();

    private:

        // 各种计算器和表

        // 等级经验表
        pet_exp2level m_exp2level_calc;

        // 技能
        c_pet_skill_data m_skill_id_data;


        // 等级、基础属性表
        attr_map_t m_base_attr_map;

        // 等级、随机成长的计算器表
        c_object_container<uint32_t, c_pet_rand_calc> m_rand_calc_map;

        // 等级、培养计算器表
        c_object_container<uint32_t, c_pet_train_calc> m_train_calc_map;

        // 等级、天赋表
        c_object_container<uint32_t, c_pet_gift_data> m_gift_data_map;

        // // 品质、血脉表
        // id_set_map_t m_badge_id_map;

};

class c_pet_data_mgr : public c_object_container<uint32_t, c_pet_data>
{

};


typedef singleton_default<c_pet_data_mgr> pet_data_mgr;


int init_pet_data();

#endif
