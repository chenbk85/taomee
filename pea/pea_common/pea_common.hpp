#ifndef PEA_COMMON_HPP
#define PEA_COMMON_HPP

extern "C"
{
#include <libtaomee/tlog/tlog.h>
}
#include "log.hpp"
#include "app_log.hpp"
#include "singleton.hpp"
#include "rc4.hpp"
#include "exp2level.hpp"
#include "physical_engine.hpp"
#include "obj_attr_data.hpp"
#include "net_cmd_def.hpp"
#include "base_timer.hpp"
#include "item_data.hpp"
#include "attr_config_data.hpp"
#include "pea_utils.hpp"
#include "effect_id.hpp"

// 定义游戏内一些常数
#include "constant.hpp"

// 各种计算器
#include "calculator.hpp"

// 各种容器
#include "container.hpp"

// 精灵
#include "pet_data.hpp"

// 人物属性
#include "player_data.hpp"

#endif
