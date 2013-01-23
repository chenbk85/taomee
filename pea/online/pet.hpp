/** 
 * ========================================================================
 * @file pet.hpp
 * @brief 
 * @version 1.0
 * @date 2012-01-09
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_PET_H_2012_01_09
#define H_PET_H_2012_01_09


#include "pea_common.hpp"
#include <set>
#include "constant.hpp"
#include "proto.hpp"

class player_t;
class c_pet;


// 精灵品质
enum
{
    // 普通精灵
    PET_NORMAL = 1,
    // 稀有精灵
    PET_RARE = 2,
    // 精英精灵
    PET_ELITE = 3,
};




///////////////////////////////////////////
// 精灵技能
///////////////////////////////////////////

class c_pet_skill
{
	public:

		uint32_t m_skill_1;
		uint32_t m_skill_2;
		uint32_t m_skill_3;
		uint32_t m_uni_skill;

};



///////////////////////////////////////////////
// 精灵天赋
///////////////////////////////////////////////



struct pet_gift_t
{
    uint32_t gift_no;
	uint32_t gift_id;
	// 天赋等级
	uint32_t gift_level;
};



class c_pet_gift
{
	public:

        int init(db_pet_info_t * p_info);

        int export_info(std::vector<db_pet_gift_info_t> & vec);

		inline uint32_t get_count()
		{
			return ((uint32_t)(m_data.size()));
		}

        inline uint32_t get_max_count()
        {
            return m_max_count;
        }

        inline bool is_full()
        {
            return (m_data.size() >= m_max_count);
        }

		pet_gift_t * get_gift(uint32_t gift_no);

        inline pet_gift_t * get_gift_by_index(uint32_t index)
        {
            return &m_data[index];
        }

		int calc_attr(c_pet_attr * p_attr);

        int update(db_update_pet_gift_t * p_info);

	protected:

		int find(uint32_t id)
		{
			for (uint32_t i = 0; i < get_count(); i++)
			{
				const pet_gift_t * p_gift = &(m_data[i]);
				if (p_gift->gift_id == id)
				{
					return i;
				}
			}

			return -1;
		}

	private:

		typedef std::vector<pet_gift_t> gift_vec_t;

		gift_vec_t m_data;

        uint32_t m_max_count;
};



/////////////////////////////////////////////////////
// 精灵血脉
/////////////////////////////////////////////////////

// struct pet_badge_t
// {
	// uint32_t badge_id;
	// uint16_t badge_step;
	// uint16_t max_step;

	// inline int is_finished()
	// {
		// return (badge_step >= max_step);
	// }

	// inline int step()
	// {
		// if (is_finished())
		// {
			// return -1;
		// }

		// badge_step++;
		// return 0;
	// }

// };

// struct pet_badge_info_t
// {
	// uint32_t badge_id;

	// // TODO 达成精灵血脉的好处
// };


// class c_pet_badge : public c_id_vec
// {
	// public:

		// uint32_t get_count() const
		// {
			// return ((uint32_t)(m_data.size()));
		// }

		// uint32_t get_id(uint32_t index) const
		// {
			// index %= get_count();
			// return (m_data[index].badge_id);
		// }

		// const pet_badge_t * get_badge(uint32_t index) const
		// {
			// index %= get_count();
			// return &(m_data[index]);
		// }

		// int insert(uint32_t id, void * p_data)
		// {
			// pet_badge_t data;
			// data.badge_id = id;
			// data.badge_step = 0;
			// data.max_step = *((uint16_t *)p_data);

			// m_data.push_back(data);
			// return 0;
		// }

		// int erase(uint32_t index)
		// {
			// m_data.erase(m_data.begin() + index);
			// return 0;
		// }

		// int badge_step(uint32_t badge_id)
		// {
			// int i = find(badge_id);
			// if (-1 == i)
			// {
				// return -1;
			// }

			// pet_badge_t * p_badge = &(m_data[i]);
			// return p_badge->step();

		// }

	// protected:


		// int find(uint32_t id) const
		// {
			// for (uint32_t i = 0; i < get_count(); i++)
			// {
				// const pet_badge_t * p_badge = &(m_data[i]);
				// if (p_badge->badge_id == id)
				// {
					// return i;
				// }
			// }

			// return -1;
		// }

	// private:

		// typedef std::vector<pet_badge_t> badge_vec_t;

		// badge_vec_t m_data;

// };











// 精灵
class c_pet
{
	public:

		c_pet(player_t * p);

		int init(db_pet_info_t * p_info);

        int uninit();

        int export_info(simple_pet_info_t * p_info);

        int export_info(btl_pet_info_t * p_info);

        int export_info(detail_pet_info_t * p_info);

		// 计算精灵的总属性
		int calc_attr();
		// 计算精灵的天赋属性
		int calc_gift_attr();
		// 计算精灵的血脉属性
		int calc_badge_attr();

		// 确认培养数据
		int confirm_train_grow();
		// 取消培养
		int cancel_train_grow();

		// 获得exp
		int gain_exp(uint32_t add_exp);

		// 等级提升
		int level_up(uint32_t new_level);
        
        // 获取宠物的属性
		uint32_t get_attr_value(uint16_t attr_type);
	private:

		// 精灵归属的玩家
		player_t * m_player;

	public:

		inline void set_player(player_t * p_player)
		{
			m_player = p_player;
		}

		inline player_t * get_player()
		{
			return m_player;
		}

	public:

		// 精灵对应的数据
		c_pet_data * m_pet_data;

		// id
		uint32_t m_id;
		// 编号
		uint32_t m_no;
		// 状态
		uint32_t m_status;

		// 经验等级表
		base_exp2level_calculator * m_exp2level_calc;
		// 等级
		uint32_t m_level;
		// 经验
		uint32_t m_exp;
        // 品质
        uint32_t m_quality;

        uint32_t m_iq;

		// 技能
		c_pet_skill m_skill;

		// 血脉
		// c_pet_badge m_badge;

        // 天赋
        c_pet_gift m_gift;

		// 精灵的总属性
		c_pet_attr m_attr;

		// 精灵等级对应的基本属性
		c_pet_attr m_base_attr;

        // 辅助精灵加成的
        uint32_t m_assist_value[PET_ASSIST_ATTR_NUM];
        c_pet_attr m_assist_attr;

		// 随机成长
		uint32_t m_rand_value[PET_RAND_ATTR_NUM];

		// 随机成长获得的属性
		c_pet_attr m_rand_attr;
		// 随机成长计算器
		c_pet_rand_calc * m_rand_calc;

        // 精灵培养
        uint32_t m_train_value[PET_TRAIN_ATTR_NUM];
        uint32_t m_try_train_value[PET_TRAIN_ATTR_NUM];

		// 精灵培养已获得的属性
		c_pet_attr m_train_attr;
        // 培养的计算器
        c_pet_train_calc * m_train_calc;


		// 精灵天赋附加的属性
		c_pet_attr m_gift_attr;

		// 精灵血脉提供的属性
		c_pet_attr m_badge_attr;






};



typedef std::vector<c_pet *> pet_vec_t;



//////////////////////////////////////////////////////////////////
// 函数
//
//////////////////////////////////////////////////////////////////


int init_player_pet(player_t * p);

int final_player_pet(player_t * p);

int is_pet_pocket_full(player_t * p_player);

bool is_fight_pet(player_t * p, uint32_t pet_no);

bool is_assist_pet(player_t * p, uint32_t pet_no);


int add_player_pet(player_t * p, c_pet * p_pet);
int del_player_pet(player_t * p, c_pet * p_pet);

c_pet * find_player_pet(player_t * p, uint32_t pet_no);

int set_fight_pet(player_t * p, c_pet * p_pet);

int add_assist_pet(player_t * p, c_pet * p_pet);

int del_assist_pet(player_t * p, c_pet * p_pet);


int pack_assist_pet_info(player_t * p, Cmessage * c_out);

void print_pet_log(player_t * p);

int calc_assist_attr(player_t * p);

#endif
