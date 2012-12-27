/*
 * =====================================================================================
 *
 *       Filename:  data.cpp
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
#include "data.h"
using namespace std;

map<uint16_t, PROTO_PROCESS_FUNC> g_cli_cmd_map;
map<uint16_t, DB_PROCESS_FUNC> g_db_cmd_map;


CONF_PARSE_FUNC g_conf_parse_func[MAX_CONF_FILE_NUM] = {NULL};
char g_allowed_ip[16] = {0};

UserManager g_user_mng;                      //管理所有在线用户的缓存
c_server *g_p_db_proxy_svr;                  //管理到db-proxy的连接
c_server *g_p_ucount_svr;                    //管理到唯一数服务的连接
c_server *g_p_switch_svr;                    //管理到switch服务的连接
c_server *g_p_multi_server_svr;              //管理到多人益智游戏服务的连接
c_server *g_p_share_svr;                     //管理到分享服务的连接
c_server *g_p_db_cache_svr;                 //管理到db-cache_server的连接
int g_user_index = 0;                            //记录第几个登录的用户，每100个用户给share-svr发一次

uint8_t g_tel_or_net;            //电信(0)or网通(1)
uint32_t g_errno;                            //协议处理的错误码
c_pack g_send_msg;                           //用来打包数据的类
char g_send_buffer[MAX_SEND_PKG_SIZE];       //发送数据的缓存
list<int> g_waiting_user_list;          //保存有请求正在等待处理的用户fd

char g_log_file[PATH_MAX];
map<uint8_t, maintain_type_t> g_maintain_map;

int g_maintain_plant_time;                   //对植物进行维护的间隔时间
int g_max_exp_in_plant;                      //在种植园中领取阳关奖励每天可以获得的最大经验值
int g_plant_time;                            //进入种植园可以增加成长值的最短时间
int g_extra_growth;                          //进入种植园额外的成长值
int g_shop_update_time;                      //商店更新的时间
int g_street_normal_num;                     //街道上普通用户的数量
int g_street_vip_num;                        //街道上vip用户的数量
int g_health_desc_offline;                   //离线时健康值减少1点需要的时间
int g_health_desc_slow;                       //在线时健康值高时减少1点需要的时间
int g_health_desc_middle;                    //在线时健康值中时减少1点需要的时间
int g_health_desc_high;                      //在线时健康值低时减少1点需要的时间
int g_happy_desc_time;                       //愉悦值减少1点需要的时间,分钟
int g_happy_growth_time;                     //愉悦时植物成长1点需要的时间,秒
int g_unhappy_growth_time;                   //不愉悦时成长1点需要的时间,秒
int g_max_game_score;                        //小游戏每天最多获得的金币值
int g_max_game_item;                   	    //小游戏每天最多获得道具次数
int g_max_day_exp;                           //每天最多可以获得经验值
int g_max_day_stuff_exp;                     //买物品每天最多可以获得经验值
int g_max_day_puzzle_exp;                    //益智游戏每天最多可以获得经验值

int g_max_pet_id;                   	    //精灵的最大id

int g_current_paper_term;
int g_bobo_reward_min;
int g_bobo_reward_max;

map<uint32_t, map<int, game_item_t> > g_game_reward_map;          //小游戏的道具奖励
map<uint32_t, game_level_item_t> g_game_level_map;          //小游戏关卡信息
map<uint32_t, uint32_t> g_game_map;

map<uint32_t, donate_info_t> g_donate_map;//募捐计划

map<uint32_t, std::map<uint32_t, uint8_t> > g_game_change_stuff_map; //小游戏兑换奖励物品map key:game_id value:stuff_list
map<uint32_t, uint32_t> g_game_change_period_map;//记录小游戏兑换奖励的当前期数


std::map<uint32_t, uint32_t> g_item_npc_map;//保存商品对应的npc信息 key:item_id value:npc_id
std::vector<uint32_t> g_npc_vec;//保存所有的商店npc_id
int g_npc_num;//商店npc的个数

//XML解析出来的数据
map<uint32_t, item_t> g_item_map;       //保存所有商品购买信息
map<uint32_t, vector<uint32_t> > g_shop_item_map;  //商店的物品列表

map<uint32_t, badge_xml_info_t> g_badge_item_map;//成就项列表

map<uint32_t, uint32_t> g_pet_series_map;//记录精灵对应的系别

//商店各种稀有度物品的数量
map<shop_key_t, shop_num_t, c_shop_cmp> g_shop_num_map;
map<uint32_t, int> g_shop_type_map;

map<uint32_t, map<seed_match_t, int, c_seed_cmp> > g_common_pet;
map<uint32_t, map<seed_match_t, int, c_seed_cmp> > g_good_pet;
map<uint32_t, map<seed_match_t, int, c_seed_cmp> > g_rare_pet;
map<uint32_t, map<seed_match_t, int, c_seed_cmp> > g_vip_pet;


map<uint32_t, task_xml_t> g_task_map; //记录配表中的所有任务

map<uint32_t, activity_info_t> g_activity_map;//记录配表中的所有活动信息


vector<seed_reward_t> g_seed_reward;

//保存每个等级对应的经验
int g_level[MAX_LEVEL];
int g_level_num;
map<uint32_t, interactive_rewards_t> g_interactive_reward_map; //保存互动元素奖励信息

map<uint32_t, vector<factory_cost_t> > g_factory_cost_map;     //工厂消耗的物品
map<uint32_t, vector<uint32_t> > g_factory_create_map;         //工厂生成的物品
map<uint32_t, uint32_t> g_factory_cost_time_map;               //工厂生成的物品的消耗时间
map<uint32_t, factory_unlock_t> g_factory_unlock_map;          //工厂图纸的解锁条件

map<uint32_t, uint8_t> g_museum_map;						   //博物馆属性
map<uint32_t, level_item_t> g_museum_level_map;				   //博物馆关卡奖品

set<uint32_t> g_depversion_id_set;  //版署米米号集合

std::vector<user_name_t> g_male_name_vec; //男孩姓名集合
std::vector<user_name_t> g_female_name_vec;//女孩姓名集合
int g_male_name_num;
int g_female_name_num;


online_timer_t g_register_to_switch_timer;//定时检测到switch的连接，如果已断开，发起重连，并发送初始信息给swtich
online_timer_t g_keepalive_timer;//定时发送指定报文给switch和multi表示online还是活跃的
online_timer_t g_register_to_multi_timer;//定时检测到multi的连接，如果已断开，发起重连，并发送初始信息给multi
