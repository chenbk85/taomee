#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "obj_attr_data.hpp"
#include "attr_config_data.hpp"



obj_attr::obj_attr(attr_config_data & data)
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

}
