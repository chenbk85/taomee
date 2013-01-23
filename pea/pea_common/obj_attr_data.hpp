#ifndef PLAYER_ATTR_DATA_H_
#define PLAYER_ATTR_DATA_H_

#include <map>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>


struct attr_config_data;

enum
{
    OBJ_ATTR_HP = 0,           		//血量
    OBJ_ATTR_MAX_HP_LIMIT = 1, 		//血量上限
    OBJ_ATTR_EXP = 2,          		//经验
    OBJ_ATTR_LEVEL = 3,        		//级别
    OBJ_ATTR_MAGIC = 4,        		//魔力
    OBJ_ATTR_AGILITY = 5,      		//敏捷
    OBJ_ATTR_LUCK = 6,        		//幸运
    OBJ_ATTR_ANGER = 7,        		//怒气
    OBJ_ATTR_ATK = 8,          		//攻击力
    OBJ_ATTR_DEF = 9,          		//防御力
    OBJ_ATTR_STRENGTH = 10,     	//体力
    OBJ_ATTR_HAPPY  = 11,       	//快乐度
    OBJ_ATTR_MAX_HAPPY_LIMIT = 12,  //快乐值上限
    OBJ_ATTR_SOUL   = 13,      		//灵魂值
    OBJ_ATTR_MAX_SOUL_LIMIT = 14,   //灵魂值上限
    OBJ_ATTR_INTENSITY = 15,   		//强度系数
    OBJ_ATTR_CRIT = 16,         	//暴击
    OBJ_ATTR_DOUBLE = 17,      		//连击
    OBJ_ATTR_SPEED = 18,       		//速度
    OBJ_ATTR_CRIT_DAMAGE = 19, 		//暴击伤害倍率
    OBJ_ATTR_TENACITY = 20,    		//韧性，减少暴击伤害
    OBJ_ATTR_PHYSIQUE = 21,         //体质
    OBJ_ATTR_GOLD     = 22,         //金钱
    OBJ_ATTR_COUPON_POINT = 23,     //点券
    OBJ_ATTR_MAX = 64
};

class obj_attr
{
    public:
        obj_attr()
        {
            memset(attr_arr, 0, sizeof(attr_arr));
        }

        virtual ~obj_attr()
        {

        }

        obj_attr(attr_config_data& data);

        virtual void reset()
        {
            memset(attr_arr, 0, sizeof(attr_arr));
        }

        // 属性累加
        virtual int accumulate(obj_attr& data)
        {

//        	attr_arr[OBJ_ATTR_HP] += data.id2value(OBJ_ATTR_HP);
//			attr_arr[OBJ_ATTR_MAX_HP_LIMIT] += data.id2value(OBJ_ATTR_MAX_HP_LIMIT);
//            attr_arr[OBJ_ATTR_MAGIC] += data.id2value(OBJ_ATTR_MAGIC);
//            attr_arr[OBJ_ATTR_AGILITY] += data.id2value(OBJ_ATTR_AGILITY);
//            attr_arr[OBJ_ATTR_LUCK] += data.id2value(OBJ_ATTR_LUCK);
//            attr_arr[OBJ_ATTR_PHYSIQUE] += data.id2value(OBJ_ATTR_PHYSIQUE);
//            attr_arr[OBJ_ATTR_ATK] += data.id2value(OBJ_ATTR_ATK);
//            attr_arr[OBJ_ATTR_DEF] += data.id2value(OBJ_ATTR_DEF);
//            attr_arr[OBJ_ATTR_CRIT] += data.id2value(OBJ_ATTR_CRIT);
//            attr_arr[OBJ_ATTR_DOUBLE] += data.id2value(OBJ_ATTR_DOUBLE);
//            attr_arr[OBJ_ATTR_SPEED] += data.id2value(OBJ_ATTR_SPEED);
//            attr_arr[OBJ_ATTR_TENACITY] += data.id2value(OBJ_ATTR_TENACITY);
//            attr_arr[OBJ_ATTR_GOLD] += data.id2value(OBJ_ATTR_GOLD);
//            attr_arr[OBJ_ATTR_COUPON_POINT] += data.id2value(OBJ_ATTR_COUPON_POINT);

			for (uint32_t i = 0; i < OBJ_ATTR_MAX; i++) {
				attr_arr[i] += data.attr_arr[i];
			}
            return 0;
        }

		virtual int decrease(obj_attr& data)
		{
			for (uint32_t i = 0; i < OBJ_ATTR_MAX; i++) {
				if (attr_arr[i] < data.attr_arr[i]) {
					attr_arr[i] = 0;
				} else {
					attr_arr[i] -= data.attr_arr[i];
				}
			}
            return 0;
		}
		virtual int set_attr(uint32_t attr_id, uint32_t value)
        {
        	 if( !(attr_id >= OBJ_ATTR_HP && attr_id < OBJ_ATTR_MAX )) return 0;
        	(*id2p(attr_id)) = value;
            return 0;
        }
		
		virtual int add_attr(uint32_t attr_id, int value)
        {
        	(*id2p(attr_id)) = id2value(attr_id) + value;
            return 0;
        }
        // 复制
        virtual int copy(obj_attr& data)
        {
            for(uint32_t i= OBJ_ATTR_HP; i<= OBJ_ATTR_COUPON_POINT; i++)
            {
                *(id2p(i)) = data.id2value(i);
            }
            return 0;
        }

		// 计算属性
        virtual void calc()
        {
            attr_arr[OBJ_ATTR_HP] += attr_arr[OBJ_ATTR_PHYSIQUE] * 5;
            attr_arr[OBJ_ATTR_ATK] += attr_arr[OBJ_ATTR_MAGIC] * 0.18;
        }

        uint32_t * id2p(uint32_t attr_id)
        {
            if( !(attr_id >= OBJ_ATTR_HP && attr_id < OBJ_ATTR_MAX ))return NULL;
            return attr_arr + attr_id;
        }

        uint32_t id2value(uint32_t attr_id)
        {
            if( !(attr_id >= OBJ_ATTR_HP && attr_id < OBJ_ATTR_MAX )) return 0;
            return attr_arr[attr_id];
        }
    public:
		uint32_t	attr_arr[OBJ_ATTR_MAX];
};

#endif



