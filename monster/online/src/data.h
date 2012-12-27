/*
 * =====================================================================================
 *
 *       Filename:  data.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月18日 14时16分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  luis (程龙), luis@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_DATA_H_20110718
#define H_DATA_H_20110718

#include <map>
#include <list>
#include <vector>

#include "c_pack.h"
#include "constant.h"
#include "data_structure.h"

#include "user_manager.h"
#include "c_server.h"
#include "online_structure.h"

typedef int (*PROTO_PROCESS_FUNC)(usr_info_t *p_user, char *p_body, uint16_t body_len);
typedef int (*DB_PROCESS_FUNC)(usr_info_t *p_user, svr_msg_header_t *p_db_pkg);
typedef int (*CONF_PARSE_FUNC)();//解析配置文件的func

extern std::map<uint16_t, PROTO_PROCESS_FUNC> g_cli_cmd_map;
extern std::map<uint16_t, DB_PROCESS_FUNC> g_db_cmd_map;
extern CONF_PARSE_FUNC g_conf_parse_func[MAX_CONF_FILE_NUM];
extern char g_allowed_ip[16];

extern UserManager g_user_mng;                      //管理所有在线用户的缓存
extern c_server *g_p_db_proxy_svr;                  //管理到db-proxy的连接
extern c_server *g_p_ucount_svr;                    //管理到唯一数服务的连接
extern c_server *g_p_switch_svr;                    //管理到switch服务的连接
extern c_server *g_p_multi_server_svr;              //管理到多人益智游戏服务的连接
extern c_server *g_p_share_svr;                     //管理到分享服务的连接
extern c_server *g_p_db_cache_svr;                 //管理到db-cache_server的连接
extern int g_user_index;                            //记录第几个登录的用户，每100个用户给share-svr发一次


extern uint8_t g_tel_or_net;            //电信(0)or网通(1)
extern uint32_t g_errno;                            //协议处理的错误码
extern c_pack g_send_msg;                           //用来打包数据的类
extern char g_send_buffer[MAX_SEND_PKG_SIZE];       //发送数据的缓存
extern std::list<int> g_waiting_user_list;          //保存有请求正在等待处理的用户fd

extern char g_log_file[PATH_MAX];

extern int g_maintain_plant_time;                   //对植物进行维护的间隔时间
extern int g_max_exp_in_plant;                      //在种植园中领取阳关奖励每天可以获得的最大经验值
extern int g_plant_time;                            //进入种植园可以增加成长值的最短时间
extern int g_extra_growth;                          //进入种植园额外的成长值
extern int g_shop_update_time;                      //商店更新的时间
extern int g_street_normal_num;                     //街道上普通用户的数量
extern int g_street_vip_num;                        //街道上vip用户的数量
extern int g_health_desc_offline;                   //离线时健康值减少1点需要的时间,分钟
extern int g_health_desc_slow;                      //在线时健康值高时减少1点需要的时间,分钟
extern int g_health_desc_middle;                    //在线时健康值中时减少1点需要的时间,分钟
extern int g_health_desc_high;                      //在线时健康值低时减少1点需要的时间,分钟
extern int g_happy_desc_time;                       //愉悦值减少1点需要的时间,分钟
extern int g_happy_growth_time;                     //愉悦时植物成长1点需要的时间,秒
extern int g_unhappy_growth_time;                   //不愉悦时成长1点需要的时间,秒
extern int g_max_game_score;                   	    //小游戏每天最多获得的金币值
extern int g_max_game_item;                   	    //小游戏每天最多获得道具次数
extern int g_max_day_stuff_exp;                     //买物品每天最多可以获得经验值
extern int g_max_day_puzzle_exp;                    //益智游戏每天最多可以获得经验值

extern int g_max_pet_id;                   	    //精灵的最大id


extern std::map<uint8_t, maintain_type_t> g_maintain_map;

extern std::map<uint32_t, std::map<int, game_item_t> > g_game_reward_map;//小游戏的道具奖励 key为game_id+level_id
extern std::map<uint32_t, game_level_item_t> g_game_level_map;//小游戏的关卡信息 key为game_id+level_id(1+1=11 1+10=110字符串相加)
extern std::map<uint32_t, uint32_t> g_game_map;//记录每个游戏有多少关卡 key为游戏id



extern std::map<uint32_t, std::map<uint32_t, uint8_t> > g_game_change_stuff_map; //小游戏兑换奖励物品map key:game_id value:stuff_list
extern std::map<uint32_t, uint32_t> g_game_change_period_map;//记录小游戏兑换奖励的当前期数

extern std::map<uint32_t, uint32_t> g_item_npc_map;//保存商品对应的npc信息 key:item_id value:npc_id
extern std::vector<uint32_t> g_npc_vec;//保存所有的商店npc_id
extern int g_npc_num;//商店npc的个数

//XML解析出来的数据
//
extern std::map<uint32_t, item_t> g_item_map;       //保存所有商品购买信息
extern std::map<uint32_t, std::vector<uint32_t> > g_shop_item_map;  //商店的物品列表

extern std::map<uint32_t, badge_xml_info_t> g_badge_item_map;//成就项列表

//对于成就项 收集齐某个系别的精灵，系别中的精灵数量配置在成就项的配置表中
extern std::map<uint32_t, uint32_t> g_pet_series_map;//记录精灵对应的系别

extern std::map<uint32_t, donate_info_t> g_donate_map;//募捐计划

extern std::map<uint32_t, uint8_t> g_museum_map;//博物馆
extern std::map<uint32_t, level_item_t> g_museum_level_map;//博物馆奖品

//商店各种稀有度物品的数量
class c_shop_cmp;
extern std::map<shop_key_t, shop_num_t, c_shop_cmp> g_shop_num_map;
extern std::map<uint32_t, int> g_shop_type_map;

//合成工厂的配置
extern std::map<uint32_t, std::vector<factory_cost_t> > g_factory_cost_map;     //工厂消耗的物品
extern std::map<uint32_t, std::vector<uint32_t> > g_factory_create_map;         //工厂生成的物品
extern std::map<uint32_t, uint32_t> g_factory_cost_time_map;                    //工厂生成的物品的消耗时间
extern std::map<uint32_t, factory_unlock_t> g_factory_unlock_map;                       //工厂图纸的解锁条件


extern std::map<uint32_t, task_xml_t> g_task_map; //记录配表中的所有任务

extern std::map<uint32_t, activity_info_t> g_activity_map;//记录配表中的所有活动信息


class c_seed_cmp;

extern std::map<uint32_t, std::map<seed_match_t, int, c_seed_cmp> > g_common_pet;
extern std::map<uint32_t, std::map<seed_match_t, int, c_seed_cmp> > g_good_pet;
extern std::map<uint32_t, std::map<seed_match_t, int, c_seed_cmp> > g_rare_pet;
extern std::map<uint32_t, std::map<seed_match_t, int, c_seed_cmp> > g_vip_pet;

extern std::vector<seed_reward_t> g_seed_reward;

//保存每个等级对应的经验
extern int g_level[MAX_LEVEL];
extern int g_level_num;

extern int g_current_paper_term;
extern int g_bobo_reward_min;
extern int g_bobo_reward_max;

extern std::map<uint32_t, interactive_rewards_t> g_interactive_reward_map; //保存互动元素奖励信息

typedef struct
{
    list_head_t timer_list;
}online_timer_t;

extern online_timer_t g_register_to_switch_timer;//定时检测到switch的连接，如果已断开，发起重连，并发送初始信息给swtich
extern online_timer_t g_keepalive_timer;//定时发送指定报文给switch和multi表示online还是活跃的
extern online_timer_t g_register_to_multi_timer;//定时检测到multi的连接，如果已断开，发起重连，并发送初始信息给multi

extern std::set<uint32_t> g_depversion_id_set;

extern std::vector<user_name_t> g_male_name_vec; //男孩姓名集合
extern std::vector<user_name_t> g_female_name_vec;//女孩姓名集合
extern int g_male_name_num;
extern int g_female_name_num;



class c_seed_cmp
{
public:
    bool operator()(const seed_match_t &key_a, const seed_match_t &key_b) const
    {
        if(key_a.seed_id < key_b.seed_id)
        {
            return true;
        }

        if(key_a.seed_id == key_b.seed_id)
        {
            if(key_a.color < key_b.color)
            {
                return true;
            }
        }

        return false;
    }
};

class c_shop_cmp
{
public:
    bool operator()(const shop_key_t &key_a, const shop_key_t &key_b) const
    {
        if(key_a.id < key_b.id)
        {
            return true;
        }

        if(key_a.id == key_b.id)
        {
            if(key_a.type < key_b.type)
            {
                return true;
            }
        }

        return false;
    }
};

#endif//H_DATA_H_20110718
