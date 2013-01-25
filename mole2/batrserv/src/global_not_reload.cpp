/*
 * =====================================================================================
 *
 *       Filename:  global_not_reload.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/05/2011 11:45:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef MAX_FUNC_NUM
#define MAX_FUNC_NUM	MAX_SKILL_NUM
#endif

#include "bt_skill.h"
#include "bt_online.h"

typedef atk_info_t* (*check_warrior_attack_t)(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
check_warrior_attack_t check_sp_atk_func[MAX_FUNC_NUM] = {0};

typedef int (*func_t)(void* pkg, int bodylen, fdsession_t* fdsess);
func_t funcs[5500] = {0};



#define MAX_FUNC_NUM	MAX_SKILL_NUM
typedef int (*before_round_warrior_attack_t)(warrior_t*, resolved_atk_t*, battle_info_t*);
typedef int (*after_round_warrior_attack_t)(warrior_t*, resolved_atk_t*, battle_info_t*);
typedef resolved_atk_t* (*resolve_warrior_attack_t)(warrior_t*, atk_info_t*, battle_info_t*, battle_users_t*);

before_round_warrior_attack_t before_round_sp_atk_func[MAX_FUNC_NUM] = {0};
after_round_warrior_attack_t after_round_sp_atk_func[MAX_FUNC_NUM] = {0};
resolve_warrior_attack_t resolve_sp_atk_func[MAX_FUNC_NUM] = {0};
lua_State*	g_lua_state = NULL;

// 协议全局缓存
uint8_t g_proto_buff[10 * 1024 * 1024] = {0};

// T-Attack Position array T 字攻击列表
//     3  1  0  2  4 
//     8  6  5  7  9 
int t_users[10][4] = {{0, 1, 2, 5}, {1, 3, 0, 6}, {2, 0, 4, 7}, {3, -1, 1, 8}, {4, 2, -1, 9}, \
					  {5, 6, 7, 0}, {6, 8, 5, 1}, {7, 5, 9, 2}, {8, -1, 6, 3}, {9, 7, -1, 4}};
//纵向攻击列表
int col_users[10][2]={{0,5},{1,6},{2,7},{3,8},{4,9},{5,0},{6,1},{7,2},{8,3},{9,4}};
//横向攻击列表
int row_users[10][5]={ {3,1,0,2,4},{3,1,0,2,4},{3,1,0,2,4},{3,1,0,2,4},{3,1,0,2,4},
						{8,6,5,7,9}	,{8,6,5,7,9},{8,6,5,7,9},{8,6,5,7,9},{8,6,5,7,9}};

skill_mp_exp_t gsump[MAX_SKILL_NUM][MAX_PROF_NUM];
skill_attr_t gsa[MAX_SKILL_NUM][MAX_WEAPON_NUM];
beast_topic_t gtopic[MAX_TOPIC_TYPE][MAX_TOPIC_NUM];

