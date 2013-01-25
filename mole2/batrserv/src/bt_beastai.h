#ifndef __BATRSER_BEAST_AI_H__
#define __BATRSER_BEAST_AI_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <libxml/tree.h>
#include <async_serv/net_if.h>

#include "utils.h"
#include "proto.h"

#ifdef __cplusplus
}
#endif

#include "bt_online.h"

// void get_beast_attack_skill(battle_info_t* abi, warrior_t* lp);
void new_round_battle_beast(battle_info_t* abi);

void get_beast_attack_skill_by_lua(battle_info_t* abi, warrior_t* lp, atk_info_t* aai, int seq);
void register_beast_ai_function();
skill_id_level_t get_auto_pk_atk_for_pet(warrior_t * ppet);
auto_skill_info_t  get_auto_pk_atk_for_user(warrior_t * p,bool can_use_medical);
#endif
