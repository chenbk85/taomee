#ifndef BT_PET_SKILL_H
#define BT_PET_SKILL_H

#include "bt_skill.h"

int kuangji_pet_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int kuangji_pet_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int huishang_pet_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int huishang_pet_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);

resolved_atk_t* base_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* kuangji_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* meiying_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* huishang_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* chanshentengman_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* zhuiji_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* bengya_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* pohun_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* mingsi_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* zhiyuzhizhen_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* zhiyujiejie_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* t_attacked_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* all_attacked_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* zaishengzhizhen_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* zaishenglingyu_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* zaishengjiejie_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* xixuegongji_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* shengmingchouqu_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* duochongjian_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);




#endif
