/** 
 * ========================================================================
 * @file player_data.hpp
 * @brief 
 * @version 1.0
 * @date 2012-06-14
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */



#ifndef H_PLAYER_DATA_H_2012_04_13
#define H_PLAYER_DATA_H_2012_04_13

void inline printf_attr(uint32_t type, obj_attr& attr)
{
//	printf("type:%u [ %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u] \n", type, attr.id2value(OBJ_ATTR_HP), 
//		attr.id2value(OBJ_ATTR_MAGIC), 
//		attr.id2value(OBJ_ATTR_AGILITY), 
//		attr.id2value(OBJ_ATTR_LUCK), 
//		attr.id2value(OBJ_ATTR_ANGER), 
//		attr.id2value(OBJ_ATTR_STRENGTH), 
//		attr.id2value(OBJ_ATTR_ATK), 
//		attr.id2value(OBJ_ATTR_DEF), 
//		attr.id2value(OBJ_ATTR_HAPPY), 
//		attr.id2value(OBJ_ATTR_SOUL), 
//		attr.id2value(OBJ_ATTR_INTENSITY), 
//		attr.id2value(OBJ_ATTR_CRIT), 
//		attr.id2value(OBJ_ATTR_DOUBLE), 
//		attr.id2value(OBJ_ATTR_SPEED), 
//		attr.id2value(OBJ_ATTR_CRIT_DAMAGE), 
//		attr.id2value(OBJ_ATTR_TENACITY), 
//		attr.id2value(OBJ_ATTR_PHYSIQUE));
}

// 人物的属性
class c_player_attr 
{
    public:

        c_player_attr()
        {
            reset_all_attr();
        }
        
        void reset_player_attr()
        {
            owner_attr.reset();    
        }
        void reset_pet_attr()
        {
            owner_attr.reset();    
        }
        void reset_merge_attr()
        {
            merge_attr.reset();    
        }
        void reset_all_attr()
        {
            owner_attr.reset();
            reset_pet_attr();
            reset_merge_attr();
        }
        // 设置玩家属性，不设置经验，金币等之类特殊属性
        void set_player_attr(attr_config_data& obj)
        {
            owner_attr.set_attr(OBJ_ATTR_HP, obj.hp_);          //血量
		    owner_attr.set_attr(OBJ_ATTR_MAGIC, obj.magic_);       //魔力
		    owner_attr.set_attr(OBJ_ATTR_AGILITY, obj.agility_);     //敏捷
		    owner_attr.set_attr(OBJ_ATTR_LUCK, obj.luck_);        //幸运
		    owner_attr.set_attr(OBJ_ATTR_ANGER, obj.anger_);       //怒气
		    owner_attr.set_attr(OBJ_ATTR_STRENGTH, obj.strength_);    //体力
		    owner_attr.set_attr(OBJ_ATTR_ATK, obj.atk_);         //攻击力
		    owner_attr.set_attr(OBJ_ATTR_DEF, obj.def_);         //防御力
		    owner_attr.set_attr(OBJ_ATTR_HAPPY, obj.happy_);       //快乐
		    owner_attr.set_attr(OBJ_ATTR_SOUL, obj.soul_);        //灵魂值
		    owner_attr.set_attr(OBJ_ATTR_INTENSITY, obj.intensity_);   //强度系数
		    owner_attr.set_attr(OBJ_ATTR_CRIT, obj.crit_);        //暴击
		    owner_attr.set_attr(OBJ_ATTR_DOUBLE, obj.double_hit_);  //连击
		    owner_attr.set_attr(OBJ_ATTR_SPEED, obj.speed_);       //移动速度
		    owner_attr.set_attr(OBJ_ATTR_CRIT_DAMAGE, obj.crit_damage_); //暴击伤害
		    owner_attr.set_attr(OBJ_ATTR_TENACITY, obj.tenacity_);    //韧性,减少暴击伤害
		    owner_attr.set_attr(OBJ_ATTR_PHYSIQUE, obj.physique_);    //体质
		    printf_attr(1, owner_attr);
        }

		void set_player_attr_by_type(uint32_t type, uint32_t value)
        {
            owner_attr.set_attr(type, value);
        }
		uint32_t get_player_attr_by_type(uint32_t type)
        {
            return owner_attr.id2value(type);
        }
		int add_player_attr(uint32_t type, int value)
		{
			return owner_attr.add_attr(type, value);
		}
        void set_pet_attr(obj_attr& obj)
        {
            pet_attr.copy(obj);
			printf_attr(2, pet_attr);
        }


        // 计算属性
        void calc_merge_attr()
        {
            reset_merge_attr();
			printf_attr(1, owner_attr);
			printf_attr(2, pet_attr);
            merge_attr.accumulate(owner_attr);
			printf_attr(3, merge_attr);
            merge_attr.accumulate(pet_attr);
			printf_attr(3, merge_attr);
            merge_attr.calc();
			printf_attr(3, merge_attr);
        }

	public:
		uint32_t get_merge_attr_by_type(uint16_t type_id)
		{
			return merge_attr.id2value(type_id);
		}
    private:
        obj_attr owner_attr;
        obj_attr pet_attr;
        obj_attr merge_attr;
};


#endif
