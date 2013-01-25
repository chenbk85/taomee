#ifndef BT_SKILL_H
#define BT_SKILL_H
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

enum{
	near_attack = 1,
	remote_attack = 2,
};

typedef struct skill_lv_info {
	uint32_t			cool_round;
}__attribute__((packed)) skill_lv_info_t;

typedef struct skill_use_mp {
	uint32_t			id;
	uint8_t				prof;
	int16_t				mp_a;
	int16_t				mp_b;
	uint32_t			exp_para;
	skill_lv_info_t		lvinfo[MAX_SKILL_LEVEL];
}__attribute__((packed))skill_mp_exp_t;

typedef struct skill_attr {
	uint32_t			id;
	uint8_t				weapon;
	uint8_t				skill_type;
	uint8_t				target_scale;
	uint8_t				target_type;
	uint8_t				fantan;
	uint8_t				fanji;
	uint8_t             distance;
	uint8_t             applystatus;//ApplyStatus指令选中对象状态， 0正常，1击倒，2全部
	uint8_t             needshield;
	uint8_t             huiji;
	uint8_t             usr_type;

	uint8_t				earth;
	uint8_t				water;
	uint8_t				fire;
	uint8_t				wind;
	uint8_t             use_person;
}__attribute__((packed))skill_attr_t;

enum{
	topic_type_beitouxi = 1,
	topic_type_touxi=2,
	topic_type_hp=3,
	topic_type_usr_define = 10,
	
	topic_type_phy_atk=(topic_type_usr_define + 1),
	topic_type_mp_atk=(topic_type_usr_define + 2),
};

typedef struct beast_topic{
	uint8_t  topic_type;
	uint32_t topic_id;
	uint16_t topic_rate;
}__attribute__((packed))beast_topic_t;

typedef struct pet_skill_attr{
	uint32_t          id;
	uint32_t          lv;
}__attribute__((packed))pet_skill_attr_t;


skill_mp_exp_t* get_skill_mp_exp(uint32_t skill_type, uint8_t prof);
skill_attr_t* get_skill_attr(uint32_t skill_type, uint32_t weapon);
beast_topic_t* get_beast_topic(uint32_t type, uint32_t topicid);

int load_skill_use_mp(xmlNodePtr cur);
int load_skill_attr(xmlNodePtr cur);
int load_beast_topic(xmlNodePtr cur);
int get_rand_beast_topic_id(uint8_t type);


void init_attck_proc_func();
void init_attck_proc_pet_func();
void get_valid_alive_enemy_pos(battle_info_t* abi, warrior_t* p, atk_info_t* aai, skill_attr_t* psa);
warrior_t* get_valid_attack_warrior(battle_info_t* abi, warrior_t* p, atk_info_t* aai, skill_attr_t* psa);
warrior_t* get_valid_attack_warrior_ex(battle_info_t* abi, warrior_t* p, atk_info_t* aai);

void resolve_and_calc_step_battle_result(battle_info_t* abi, atk_info_t* aai);
void set_fangyu_state_after_first_attack(warrior_t* p_fighter, atk_info_t* aai);

void change_attr_before_attack(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
void change_attr_after_attack(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int xianfazhiren_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int xianfazhiren_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);

int skill_jinghunquan_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int skill_jinghunquan_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);

int gaopinzhenji_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int gaopinzhenji_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);

int kuangji_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int kuangji_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int jizhonggongji_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int jizhonggongji_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int huishang_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int huishang_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int ninshengyiji_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int ninshengyiji_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);
int zhiyuhuahuan_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi);

void calc_warrior_prop_for_prepare_skill(warrior_t* p, atk_info_t* aai);

int get_valid_t_attack_cross_pos(int curpos, battle_users_t* aui);
int get_valid_t_attack_cross_pos_ex(int curpos, battle_users_t* aui);
int get_attack_hurthp_rate(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* normal_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* xian_zhi_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* run_away_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* fang_yu_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* dai_ji_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* base_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* zhuiji_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* bengya_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* chuanci_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* duochongjian_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* kuangji_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* shunsha_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* meiying_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* huishang_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* jizhonggongji_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* douqi_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* shuangxingjian_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* ninshengyiji_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* t_attacked_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* all_attacked_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* pohun_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* mingsi_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* callback_fuzhu_resolve_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* zhiyuzhizhen_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* zhiyulingyu_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* skill_qiliaodun_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* skill_jintouquan_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* kuangyezhifun_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* zhiyujiejie_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* zhiyuhuahuan_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* zaishengzhizhen_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* zaishenglingyu_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* zaishengjiejie_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* jiushu_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* jiushu_all_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* bingqingzhizhen_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* bingqinglingyu_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* bingqingjiejie_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* chg_pos_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* catch_pet_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* recall_pet_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* shengmingchouqu_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* sanyeshi_resolve_sprite_atk(warrior_t * p,atk_info_t * aai,battle_info_t * abi,battle_users_t * enemy);

resolved_atk_t* mengshouliexi_resolve_sprite_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);

resolved_atk_t* gaopinzhenji_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* dubaoshu_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
resolved_atk_t* ruodiantouxi_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, battle_users_t* enemy);
static inline resolved_atk_t* alloc_resolve_atk(warrior_t* p, atk_info_t* aai, battle_info_t* abi, int inc_seq)
{
	KDEBUG_LOG(p->userid,"petid=%d action:atk=(%u,%d,%d,%u)",
			p->petid,aai->atk_type,aai->atk_level,aai->atk_pos,aai->atk_mark);
	resolved_atk_t* pra = (resolved_atk_t*)g_slice_alloc0(sizeof(resolved_atk_t));
	pra->seq     = (inc_seq) ? (++abi->seq_in_round) : abi->seq_in_round;
	pra->fighter_id = p->userid;
	pra->petid = p->petid;
	pra->atk_type = aai->atk_type;
	pra->atk_level = aai->atk_level;
	pra->heji_seq = aai->heji_seq;
	pra->change_hp=0;
	pra->change_mp=0;
	
	list_add_tail(&pra->atk_list, &abi->attack_list);
	return pra;
}


#endif
