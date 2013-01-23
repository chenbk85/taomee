#ifndef ATTR_CONFIG_DATA_HPP_
#define ATTR_CONFIG_DATA_HPP_
#include <string.h>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "singleton.hpp"
#include "container.hpp"


struct attr_config_data
{
    attr_config_data()
    {
        reset();
    }
    void reset()
    {
        memset(this, 0, sizeof(*this));
    }


    uint32_t   id_;          //ID
    uint32_t   hp_;          //血量
    uint32_t   exp_;         //经验
    uint32_t   level_;       //级别
    uint32_t   magic_;       //魔力
    uint32_t   agility_;     //敏捷
    uint32_t   luck_;        //幸运
    uint32_t   anger_;       //怒气
    uint32_t   strength_;    //体力
    uint32_t   atk_;         //攻击力
    uint32_t   def_;         //防御力
    uint32_t   happy_;       //快乐
    uint32_t   soul_;        //灵魂值
    uint32_t   intensity_;   //强度系数
    uint32_t   crit_;        //暴击
    uint32_t   double_hit_;  //连击
    uint32_t   speed_;       //移动速度
    uint32_t   crit_damage_; //暴击伤害
    uint32_t   tenacity_;    //韧性,减少暴击伤害
    uint32_t   physique_;    //体质
};

typedef std::map<uint32_t, attr_config_data *> attr_config_data_map_t;

int parse_attr_config_data(xmlNodePtr node, attr_config_data * data);



//////////////////////////////////////////////////////////////////////
// 玩家
//



class c_attr_config_data_mgr : public c_object_container<uint32_t, attr_config_data>
{

};

typedef singleton_default<c_attr_config_data_mgr> attr_config_data_mgr;




int init_player_attr_config_data(xmlNodePtr root);



#endif
