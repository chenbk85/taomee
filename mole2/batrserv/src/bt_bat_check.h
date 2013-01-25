#ifndef BATRSERV_CHECK_H
#define BATRSERV_CHECK_H
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


void init_attck_check_func() ;

void check_user_step_attack_and_insert_atk_list(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* jiushu_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* pet_jiushu_all_only_svr_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* run_away_check_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* chg_pos_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* pet_chg_pos_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* catch_pet_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* recall_pet_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* daiji_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);

atk_info_t* huwei_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);

atk_info_t* chemical_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* fang_yu_check_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* xianfazhiren_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* zhuiji_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* huiji_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* jizhonggongji_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* ninshenjian_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);

atk_info_t* t_attacked_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* all_attacked_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);

atk_info_t* xixuegongji_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);
atk_info_t* normal_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);

void check_attack_type_by_mp(warrior_t* p, atk_info_t* aai);
uint8_t check_mp_enough( warrior_t* p, atk_info_t* aai);

atk_info_t* zaishengzhizhen_check_warrior_atk(battle_info_t* abi, warrior_t* p, atk_info_t* pai);


#endif
