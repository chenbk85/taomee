#ifdef __cplusplus
extern "C"
{
#endif

#include <fcntl.h>
#include <sys/mman.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
//#include  <statistic_agent/msglog.h>
#include "libtaomee/project/stat_agent/msglog.h"
#ifdef __cplusplus
}
#endif

#include "utils.h"
#include "bt_online.h"
#include "bt_skill.h"
#include "bt_pet_skill.h"
#include "bt_bat_check.h"
#include "global.h"

skill_mp_exp_t* get_skill_mp_exp(uint32_t skill_type, uint8_t prof)
{
	if (skill_type == 0 || skill_type >= MAX_SKILL_NUM || prof >= MAX_PROF_NUM) {
		//ERROR_LOG("invalid skill or prof\t[%u %u]", skill_type, prof);
		return NULL;
	}

	return &gsump[skill_type][prof];
}

int load_skill_use_mp(xmlNodePtr cur)
{
	int skill_id;
	memset(gsump, 0, sizeof(gsump));
	cur = cur->xmlChildrenNode; 
	
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Skill")) { 
			DECODE_XML_PROP_INT (skill_id, cur, "ID");
			xmlNodePtr chl = cur->xmlChildrenNode;
			while (chl) {
				if (!xmlStrcmp(chl->name, (const xmlChar *)"Job")) { 
					uint32_t prof_power;
					DECODE_XML_PROP_INT (prof_power, chl, "ID");
					uint8_t prof_id = -1;
					while (prof_power) {
						prof_power = prof_power >> 1;
						prof_id ++;
					}
					
					if (skill_id >= MAX_SKILL_NUM_W_MIN){
						chl = chl->next;
						continue;
					}
					
					skill_mp_exp_t* sa = get_skill_mp_exp(skill_id, prof_id);
					if (!sa) {
						chl = chl->next;
						continue;
					}
					sa->id = skill_id;
					sa->prof = prof_id;
					decode_xml_prop_uint16_default ((uint16_t*)&sa->mp_a, chl, "MpA", 0);
					decode_xml_prop_uint16_default ((uint16_t*)&sa->mp_b, chl, "MpB", 0);
					decode_xml_prop_uint32_default (&sa->exp_para, chl, "Exp", 0);
					TRACE_LOG("JOBS id=%u job=%u mpa=%d mpb=%d exp:%u ", sa->id, sa->prof, sa->mp_a, sa->mp_b,sa->exp_para );
				}

				if (!xmlStrcmp(chl->name, (const xmlChar *)"Lv")) {
					uint32_t level;
					DECODE_XML_PROP_INT (level, chl, "SkillLevel");
					if (level < 1 || level > 10) {
						chl = chl->next;
						continue;
					}
					for (uint32_t loop = 0; loop < MAX_PROF_NUM; loop ++) {
						skill_mp_exp_t* sa = get_skill_mp_exp(skill_id, loop);
						if (!sa) continue;
						decode_xml_prop_uint32_default (&sa->lvinfo[level - 1].cool_round, chl, "CoolRound", 0);
					}
				}
				chl = chl->next;
			}
		}
		cur = cur->next;
	}

	return 0;
}

beast_topic_t* get_beast_topic(uint32_t type, uint32_t topicid)
{
	return type < MAX_TOPIC_TYPE && topicid < MAX_TOPIC_NUM ? &gtopic[type][topicid] : NULL;
}

int get_rand_beast_topic_id(uint8_t type)
{
	int rate = rand() % 1000;
	int rate_range = 0;
	for (int i = 0; i < MAX_TOPIC_NUM; i++){
		beast_topic_t* topic = &(gtopic[type][i]);
		if (topic->topic_type == 0 || topic->topic_rate == 0){
			return 0;
		}
		
		if (rate >= rate_range && rate < (rate_range + topic->topic_rate)){
			return type * 1000 + i;
		}

		rate_range += topic->topic_rate;
	}
	
	return 0;
}

int load_beast_topic(xmlNodePtr cur)
{
	memset(gtopic, 0, sizeof(gtopic));
	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Talk")) { 
			uint32_t topic_id, topic_type;

			DECODE_XML_PROP_UINT32(topic_type, cur, "Type");
			DECODE_XML_PROP_UINT32(topic_id, cur, "ID");

			TRACE_LOG("type=%u topicid=%u", topic_type, topic_id);
			beast_topic_t* sa = get_beast_topic(topic_type, topic_id);
			if (!sa) return -1;

			sa->topic_id = topic_id;
			sa->topic_type = topic_type;
			DECODE_XML_PROP_UINT32(sa->topic_rate, cur, "Probability");
		}

		cur = cur->next;
	}
	return 0;
}

skill_attr_t* get_skill_attr(uint32_t skill_type, uint32_t weapon)
{
	if (skill_type == 0 || skill_type >= MAX_SKILL_NUM || weapon >= MAX_WEAPON_NUM) {
		ERROR_LOG("invalid skill or weapon\t[%u %u]", skill_type, weapon);
		return NULL;
	}

	return &gsa[skill_type][weapon];
}

int load_skill_attr(xmlNodePtr cur)
{
	int skill_id;
	int weap_type[MAX_WEAPON_NUM]={ };
   	bool tag[MAX_WEAPON_NUM]={ };
	memset(gsa, 0, sizeof(gsa));
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)"Action")) { 
			DECODE_XML_PROP_INT (skill_id, cur, "ID");
			//DECODE_XML_PROP_INT (weap_type, cur, "Weapon");
			uint32_t cnt=decode_xml_prop_arr_int_default(weap_type,MAX_WEAPON_NUM,cur,(char*)"Weapon",0);
			memset(tag, 0, sizeof(tag));
			if (skill_id >= MAX_SKILL_NUM_W_MIN){
				cur = cur->next;
				continue;
			}
			if(cnt==1 && weap_type[0]==-1){
				memset(tag,1,sizeof(tag));
			}else{
				for(uint8_t loop=0;loop<cnt;loop++){
					if( weap_type[loop] <8 && weap_type[loop]>=0){
						tag[weap_type[loop]]=true;
					}
				}
			}	
			for( uint8_t loop=0 ; loop<MAX_WEAPON_NUM ; loop++ ){
				if( tag[loop] ){
					//TRACE_LOG("skillid=%u wt=%u", skill_id, loop);
					//DEBUG_LOG("skillid=%u wt=%u", skill_id, loop);
					skill_attr_t* sa = get_skill_attr(skill_id, loop);
					if (!sa) return -1;
					sa->id = skill_id;
					sa->weapon = loop;
					DECODE_XML_PROP_INT (sa->skill_type, cur, "Type");
					DECODE_XML_PROP_INT (sa->target_scale, cur, "TargetAmount");
					DECODE_XML_PROP_INT (sa->fantan, cur, "ApplyBack");
					DECODE_XML_PROP_INT (sa->fanji, cur, "AttackBack");
					DECODE_XML_PROP_INT (sa->target_type, cur, "ApplyTarget");
					DECODE_XML_PROP_INT (sa->distance, cur, "Distance");
					DECODE_XML_PROP_INT (sa->applystatus, cur, "ApplyStatus");
					DECODE_XML_PROP_INT_EX (sa->huiji, cur, "KnockBack", 0);
					DECODE_XML_PROP_INT_EX (sa->needshield, cur, "Shield", no_need_shield);
					DECODE_XML_PROP_INT_EX (sa->usr_type, cur, "User", user_ren_only);
					DECODE_XML_PROP_INT_EX (sa->earth, cur, "Earth", 0);
					DECODE_XML_PROP_INT_EX (sa->water, cur, "Water", 0);
					DECODE_XML_PROP_INT_EX (sa->fire, cur, "Fire", 0);
					DECODE_XML_PROP_INT_EX (sa->wind, cur, "Wind", 0);
					DECODE_XML_PROP_INT_EX (sa->use_person, cur, "Element", 0);
				}	
			}
		}
		cur = cur->next;
	}
	return 0;
}



void change_attr_before_attack(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	if (before_round_sp_atk_func[ara->atk_type]) 
		before_round_sp_atk_func[ara->atk_type](p, ara, abi);
}

void change_attr_after_attack(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	if (after_round_sp_atk_func[ara->atk_type])
		after_round_sp_atk_func[ara->atk_type](p, ara, abi);
	
}

void set_fangyu_state_before_round_attack(battle_info_t* abi, warrior_t* p_fighter, atk_info_t* p_atk_info)
{
	if (p_atk_info->atk_seq == 1){
		return;
	}
	set_fangyu_state_after_first_attack(p_fighter,p_atk_info );

	calc_warrior_prop_for_prepare_skill(p_fighter, p_atk_info);
}

void set_fangyu_state_after_first_attack(warrior_t* p_fighter, atk_info_t* p_atk_info)
{
	switch ( p_atk_info->atk_type ){
		case skill_pd_fangyu :
		case skill_pd_pet_fangyu:{
			KDEBUG_LOG(p_fighter->userid,"set_fangyu");
			SET_WARRIOR_STATE(p_fighter->p_waor_state->state, fangyu_bit);
			break;
		}
		case skill_pd_huandun:
		case skill_pd_pet_huandun:{
			SET_WARRIOR_STATE(p_fighter->p_waor_state->state, huandun_bit);
			p_fighter->p_waor_state->set_hundun_level( p_atk_info->atk_level);
			break;
		}
		case skill_md_mokang:
		case skill_md_pet_mokang:{
			SET_WARRIOR_STATE(p_fighter->p_waor_state->state, mokang_bit);
			break;
		}
		case skill_pd_huiji:
		case skill_pd_pet_huiji:{
			SET_WARRIOR_STATE(p_fighter->p_waor_state->state, huiji_bit);
			break;
		}

		case skill_pa_meiying:
		case skill_pa_pet_meiying:{
			SET_WARRIOR_STATE(p_fighter->p_waor_state->state, meiying_bit);
			p_fighter->p_waor_state->set_meiyin_level (p_atk_info->atk_level);
			break;
		}

		case skill_muyedun:
		case skill_pet_muyedun:{
			p_fighter->p_waor_state->set_muyedun_level (p_atk_info->atk_level);
			SET_WARRIOR_STATE(p_fighter->p_waor_state->state, muyedun_bit);
			break;
		}

		case skill_hw_huwei:
		case skill_hw_pet_huwei:{
			add_warrior_to_huweilist(p_fighter, p_atk_info->atk_pos);
			break;
		}
		case skill_mifanquan:{
			DEBUG_LOG("mifanquan set level");
			p_fighter->p_waor_state->set_state_info(huiji_bit,  2, 100 );
			p_fighter->p_waor_state->set_mifanquan_level(p_atk_info->atk_level);
			break;
		}
		case skill_jueqiquan:{
			p_fighter->p_waor_state->set_jueqiquan_level(p_atk_info->atk_level);
			break;
		}
		default :
			break;
	}
}

static void decre_attack_use_mp(warrior_t* p, atk_info_t* p_atk_info)
{
	if (p_atk_info->atk_type == 0){
		return;
	}

	switch(p_atk_info->atk_type){
		case skill_run_away:
		case skill_user_break_off:
		case skill_pet_break_off:
		case skill_use_chemical:
		case skill_pet_use_chemical:
		case skill_chg_position:
		case skill_pet_chg_position:
		case skill_recall_pet:
		case skill_pa_dai_ji:
		case skill_pa_pet_dai_ji:
			return;
		default:
			break;
	}
	
	skill_mp_exp_t* psu = get_skill_mp_exp(p_atk_info->atk_type, p->prof);
	if (!psu || !psu->id) {
		ERROR_LOG("invalid attack type\t[atk_type=%u id=%u petid=%u prof=%u atk_type=%u]", p_atk_info->atk_type, p->userid, p->petid, p->prof, p_atk_info->atk_type);
		return;
	}
	p->mp = p->mp - (psu->mp_a * p_atk_info->atk_level + psu->mp_b);
	DEBUG_LOG("decr mp %u %u %u %u",(psu->mp_a * p_atk_info->atk_level + psu->mp_b),psu->mp_a,p_atk_info->atk_level,psu->mp_b);
}

static void incre_skill_exp(warrior_t* p, atk_info_t* p_atk_info)
{
	if (p_atk_info->atk_type == 0 || IS_BEAST(p)){
		return;
	}
	switch(p_atk_info->atk_type){
		case skill_run_away:
		case skill_user_break_off:
		case skill_pet_break_off:
		case skill_chg_position:
		case skill_pet_chg_position:
		case skill_recall_pet:
		case skill_pa_dai_ji:
		case skill_pa_pet_dai_ji:
			return ;
		default:
			break;
	}
	skill_mp_exp_t* psu = get_skill_mp_exp(p_atk_info->atk_type, p->prof);
	if (!psu || !psu->id) {
		ERROR_LOG("invalid attack type\t[uid=%u petid=%u prof=%u atk_type=%u]", p->userid, p->petid, p->prof, p_atk_info->atk_type);
		return;
	}
	int loop;
	for (loop = 0; loop < MAX_SKILL_NUM_PER_PERSON; loop++) {
		if (p_atk_info->atk_type == p->skills[loop].skill_id) {
			if (p->skills[loop].skill_level != 0){
				DEBUG_LOG("INCRE SKILL EXP\t[%u %u %u %u %u]", p->userid, p->petid, p_atk_info->atk_type, p_atk_info->atk_level, p->skills[loop].skill_level);
				if(IS_BEAST_GRP_ID(p->enemy_team->teamid)) {
					p->skills[loop].skill_exp +=  psu->exp_para * p_atk_info->atk_level / p->skills[loop].skill_level;
				}
				//使用个数加1
				p->skills_use_count[loop]++;
			}else{
				ERROR_LOG("INVALID SKILL LEVEL\t[%u %u]", p->skills[loop].skill_id, p->skills[loop].skill_level);
			}
			
			break;
		}
	}
}

//减少武器的耐久度
void decre_weapon_durable_value(warrior_t* p, atk_info_t* p_atk_info)
{
	if (IS_BEAST(p)) {
		return;
	}
	
	skill_attr_t* psa = get_skill_attr(p_atk_info->atk_type, p->weapon_type);
	if (!psa){
		return;
	}

	int i = 0;

	switch (psa->skill_type) {
		case physi_attack:
		case mp_attack:
		case fh_magic:
		case magic_attack:
		case abnormal_attack:
		case xixue_gongji:
			for (i = 0; i < p->cloth_cnt; i++){
				if(p->clothes[i].equip_type == part_weapon){
					p->clothes[i].durable_val++;
					return;
				}
			}
			break;
		default: return;
	}
}

//在攻击前计算武器属性
void calc_warrior_prop_for_prepare_skill(warrior_t* p, atk_info_t* p_atk_info)
{
	if (!p || CANNOT_ATTACK(p) || p_atk_info->confrm_decre_mp == confrm_decre_mp_when_set_state || p->enemy_team->ren_alive_count <= 0){
		return;
	}

	uint8_t decre_durable = 0;
	switch(p_atk_info->atk_type){
		case skill_pd_huandun:
		case skill_pd_pet_huandun:
		case skill_hw_huwei:
		case skill_hw_pet_huwei:
			break;
		case skill_pa_meiying:
		case skill_pa_pet_meiying:
		case skill_pd_huiji:
		case skill_pd_pet_huiji:
		case skill_mifanquan:
			decre_durable = 1;
			break;
		default:
			return;
	}
	
	/*
	DEBUG_LOG("DECRE MP & INCRE EXP FOR PREP SKILL\t[uid=%u petid=%u atk_seq=%d atk_type=%u atk_level=%u decre_durable=%u]", p_atk_info->wrior->userid, p_atk_info->wrior->petid, p_atk_info->atk_seq, p_atk_info->atk_type, p_atk_info->atk_level, decre_durable);
	*/
	p_atk_info->confrm_decre_mp = confrm_decre_mp_when_set_state;
	decre_attack_use_mp(p_atk_info->wrior, p_atk_info);
	incre_skill_exp(p_atk_info->wrior, p_atk_info);
	if (decre_durable){
		decre_weapon_durable_value(p_atk_info->wrior, p_atk_info);
	}
}
//一次行动后清理状态位
static void clean_state_after_step_attack(warrior_t* p)
{
	DEBUG_LOG("clean_state_after_step_attack");
	if (p->p_waor_state->state && tuoli_bit) {
		clean_one_state(p, tuoli_bit, 1);
	}
	if (p->p_waor_state->state && shufu_bit) {
		clean_one_state(p, shufu_bit, 1);
	}
	if (p->p_waor_state->state && xuanyun_bit) {
		clean_one_state(p, xuanyun_bit, 1);
	}
}
//技能的一次攻击（有可能攻击多个人 每攻击一次 调用函数calc_warrior_single_step_battle_result)
void resolve_and_calc_step_battle_result(battle_info_t* abi, atk_info_t* p_atk_info)
{	
	warrior_t* p = p_atk_info->wrior;
	KDEBUG_LOG(p->userid,"**********resolve_and_calc_step_battle_result:技能%u 开始攻击*************",p_atk_info->atk_type);
	//战斗结束条件判断  2012-3-22更改 由ren_alive_count to alive_count
	if(p->enemy_team->alive_count <= 0){
		DEBUG_LOG("All Enemy Dead\t[batid=%lu btl_loop=%u]", abi->batid, abi->battle_loop);
		return;
	}
	
	check_attack_type_by_mp(p_atk_info->wrior, p_atk_info);

	resolved_atk_t* pra_calc = NULL;
	state_effect_hpmp(abi, p, p_atk_info);
	DEBUG_LOG("xxxx %u",p->p_waor_state->check_state( attacked_out_bit));
	//2012-3-26 增加if(!NONEED_ATTACK(p)) 判断  解决被异常状态打死却不死的bug
	if(!(RUN_OR_OUT(p) || WARRIOR_DEAD(p) || WARRIOR_CATCHED_PET(p) || p->pet_not_in_battle_mode())){
		if (resolve_sp_atk_func[p_atk_info->atk_type]) {
			//自定义技能解决函数 一般攻击多个人或者效果有特殊要求的需自定义  
			//pra_calc返回NULL 因为函数里调用过calc_warrior_single_step_battle_result
			KDEBUG_LOG(p->userid,"special resolve skill=%u",p_atk_info->atk_type);
			pra_calc = resolve_sp_atk_func[p_atk_info->atk_type](p, p_atk_info, abi, p->enemy_team);
		} else {//通用的技能函数
			KDEBUG_LOG(p->userid,"normal resolve skill=%u",p_atk_info->atk_type);
			pra_calc = normal_resolve_sprite_atk(p, p_atk_info, abi, NULL);
		}
		
		if (pra_calc){//通用的技能调用函数的返回进入
			//KDEBUG_LOG(p->userid,"CALL SINGLE FUN[batid=%lu btl_loop=%u %d]", abi->batid, abi->battle_loop,  p_atk_info->atk_type);
			calc_warrior_single_step_battle_result(abi, pra_calc, p_atk_info);
		}
	}
	
	if (p_atk_info->confrm_decre_mp == confrm_decre_mp_after_atk){//扣魔 加经验
		KDEBUG_LOG(p_atk_info->wrior->userid,"DECRE MP & INCRE EXP [batid=%lu btl_loop=%u petid=%u atk_seq=%d atk_type=%u atk_level=%u]", 
			abi->batid, abi->battle_loop,p_atk_info->wrior->petid, p_atk_info->atk_seq, p_atk_info->atk_type, p_atk_info->atk_level);
		decre_attack_use_mp(p_atk_info->wrior, p_atk_info);
		incre_skill_exp(p_atk_info->wrior, p_atk_info);
		decre_weapon_durable_value(p_atk_info->wrior, p_atk_info);
	}

	p_atk_info->atk_deal = 1;

	warrior_t* lp = p_atk_info->wrior;
	if (p_atk_info->atk_seq == 0 && lp->atk_info[1].atk_seq == 1){
		//第一招攻击后设置状态位
		switch(lp->atk_info[1].atk_type){
			case skill_pd_fangyu:
			case skill_pd_pet_fangyu:
			case skill_md_mokang:
			case skill_md_pet_mokang:
			case skill_pd_huandun:
			case skill_pd_pet_huandun:
			case skill_pa_meiying:
			case skill_pa_pet_meiying:
			case skill_pd_huiji:
			case skill_pd_pet_huiji:
			case skill_hw_huwei:
			case skill_hw_pet_huwei:
				if (check_mp_enough( lp, &lp->atk_info[1])){
					set_fangyu_state_after_first_attack(lp, &lp->atk_info[1]);
					calc_warrior_prop_for_prepare_skill(lp, &lp->atk_info[1]);
				}
				break;
		}
	}
	//行动结束后清理状态位
	clean_state_after_step_attack(p);
	KDEBUG_LOG(p->userid,"**********resolve_and_calc_step_battle_result:攻击结束*************");
}

//XXXX  调整攻击
int gaopinzhenji_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	DEBUG_LOG("gaopinzhenji hurt value rate %f",(0.65 + 0.05 * (ara->atk_level - 1)));
	p->attack_value *= (0.65 + 0.05 * (ara->atk_level - 1));
	return 0;
}
//XXXX  还原攻击
int gaopinzhenji_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value /= (0.65 + 0.05 * (ara->atk_level - 1));
	skill_info_t* pskill = get_beast_skill_by_atktype(p, ara->atk_type);
	skill_use_mp* psu = get_skill_mp_exp(ara->atk_type, p->prof);
	if (pskill && psu && !pskill->cool_update) {
		pskill->cool_update = 1;
		pskill->cool_round = psu->lvinfo[ara->atk_level - 1].cool_round + 1;
	}
	return 0;
}
//
int skill_jintouquan_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	//p->attack_value *= (0.2+ 0.3 * (ara->atk_level ));
	p->p_waor_state->set_jintouquan_level(ara->atk_level);
	DEBUG_LOG(" ==skill_jintouquan_Begin== attack_valude=%u ",p->attack_value);
	return 0;
}
int skill_jintouquan_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	//p->attack_value /= (0.2+ 0.3 * (ara->atk_level ));
	DEBUG_LOG(" ==skill_jintouquan_end== attack_valude=%u ",p->attack_value);
	return 0;
}
//惊魂拳
int skill_jinghunquan_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{

	p->attack_value *= (1+ 0.05 * (ara->atk_level ));
	p->p_waor_state->set_jinghunquan_level(ara->atk_level);
	DEBUG_LOG(" ==skill_jinghunquan_Begin== attack_valude=%u ",p->attack_value);
	return 0;
}
int skill_jinghunquan_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value /= (1+ 0.05 * (ara->atk_level ));
	DEBUG_LOG(" ==skill_jinghunquan_end== attack_valude=%u ",p->attack_value);
	return 0;
}
//
int skill_qidouquan_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{

	p->attack_value *= (1.15 - 0.05 * (ara->atk_level-1 ));
	DEBUG_LOG(" ==skill_qidouquan_Begin== attack_valude=%u ",p->attack_value);
	return 0;
}
int skill_qidouquan_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value /= (1.15 - 0.05 * (ara->atk_level-1 ));
	DEBUG_LOG(" ==skill_qidouquan_end== attack_valude=%u ",p->attack_value);
	return 0;
}



int skill_jueqiquan_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value /= (0.75+ 0.05 * (ara->atk_level ));
	DEBUG_LOG(" ==skill_jueqiquan_end== attack_valude=%u ",p->attack_value);
	return 0;
}
int skill_jueqiquan_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{

	p->attack_value *= (0.75+ 0.05 * (ara->atk_level ));
	DEBUG_LOG(" ==skill_jueqiquan_Begin== attack_valude=%u ",p->attack_value);
	return 0;
}

int skill_zaiezhizhao_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value *= (1 + 0.05 * (ara->atk_level ));
	DEBUG_LOG(" ==skill_zaiezhizhao_Begin== attack_valude=%u ",p->attack_value);
	return 0;
}

int skill_zaiezhizhao_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value /= (1 + 0.05 * (ara->atk_level ));
	DEBUG_LOG(" ==skill_zaiezhizhao_end== attack_valude=%u ",p->attack_value);
	return 0;
}

int skill_hundunzhiren_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value *= (1 + 0.05 * (ara->atk_level ));
	DEBUG_LOG(" ==skill_hundunzhiren_Begin== attack_valude=%u ",p->attack_value);
	return 0;
}

int skill_hundunzhiren_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value /= (1 + 0.05 * (ara->atk_level ));
	DEBUG_LOG(" ==skill_hundunzhiren_end== attack_valude=%u ",p->attack_value);
	return 0;
}


int skill_shenpanzhilun_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value *= (0.65 + 0.05 * (ara->atk_level ));
	DEBUG_LOG(" ==skill_shenpanzhilun_Begin== attack_valude=%u ",p->attack_value);
	return 0;
}
int skill_shenpanzhilun_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value /= (0.65 + 0.05 * (ara->atk_level ));
	DEBUG_LOG(" ==skill_shenpanzhilun_end== attack_valude=%u ",p->attack_value);
	return 0;
}




int xianfazhiren_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value *= (1.6 + 0.1 * (ara->atk_level - 1));
	return 0;
}
int xianfazhiren_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->attack_value /= (1.6 + 0.1 * (ara->atk_level - 1));
	return 0;
}

int kuangji_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->hit_rate *= (1 - 0.03 * ara->atk_level);
	return 0;
}

int kuangji_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->hit_rate /= (1 - 0.03 * ara->atk_level);
	return 0;
}

int jizhonggongji_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->bisha *= (1.38 + 0.02 * ara->atk_level);
	return 0;
}

int jizhonggongji_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->bisha /= (1.38 + 0.02 * ara->atk_level);
	return 0;
}

int huishang_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	DEBUG_LOG("PET_HUI_SHANG BEFORE");
	p->defense_value *= (1 - 0.05 * ara->atk_level);
	p->spirit *= (1 - 0.05 * ara->atk_level);
	p->attack_value *= (1 + 0.07 * ara->atk_level);
	return 0;
}

int huishang_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	DEBUG_LOG("PET_HUI_SHANG AFTER");
	p->defense_value /= (1 - 0.05 * ara->atk_level);
	p->spirit /= (1 - 0.05 * ara->atk_level);
	p->attack_value /= (1 + 0.07 * ara->atk_level);
	return 0;
}

int ninshengyiji_sprite_before_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->bisha *= 1.3;
	return 0;
}

int ninshengyiji_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	p->bisha /= 1.3;
	return 0;
}

int zhiyuhuahuan_sprite_after_round_atk(warrior_t* p, resolved_atk_t* ara, battle_info_t* abi)
{
	skill_info_t* pskill = get_beast_skill_by_atktype(p, ara->atk_type);
	skill_use_mp* psu = get_skill_mp_exp(ara->atk_type, p->prof);
	if (pskill && psu && !pskill->cool_update) {
		pskill->cool_update = 1;
		pskill->cool_round = psu->lvinfo[ara->atk_level - 1].cool_round + 1;
	}
	
	return 0;
}

int get_attack_hurthp_rate(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	switch(p_atk_info->atk_type){
		case skill_ma_jushizhou:
		case skill_ma_hanbingzhou:
		case skill_ma_liehuozhou:
		case skill_ma_kuangfengzhou:
		case skill_ma_liuxingyu:
		case skill_ma_youyuanbing:
		case skill_ma_diyuhuo:
		case skill_ma_longjuanren:
			return 40;
		case skill_ma_pet_jushizhou:
		case skill_ma_pet_hanbingzhou:
		case skill_ma_pet_liehuozhou:
		case skill_ma_pet_kuangfengzhou:
			return 60;
		case skill_ma_pet_liuxingyu:
		case skill_ma_pet_youyuanbing:
		case skill_ma_pet_diyuhuo:
		case skill_ma_pet_longjuanren:
			return 40;
	}
	uint32_t atk_level=p_atk_info->atk_level;
	//??
	if (atk_level>10 )  atk_level=10;

	int basehurt_rate = 100;
	//DEBUG_LOG("lllll %u", p_atk_info->atk_type );
	switch ( p_atk_info->atk_type ){
		case skill_pa_zhuiji:
		case skill_pa_pet_zhuiji:{
			float rate_opt[]={0.7,0.5,0.4,0.34,0.3 };
			basehurt_rate *= rate_opt[ (atk_level-1)/2];
			break;
		}
		case skill_shengtanglingyu:{
			basehurt_rate *= 2.5* atk_level;
			break;
		}
		case skill_shenenjiejie:{
			basehurt_rate *= 1.5*atk_level;
			//DEBUG_LOG("lllll2 %u",basehurt_rate);
			break;
		}
		case skill_pa_bengya :
		case skill_pa_pet_bengya:
			basehurt_rate = 99 + 20 * atk_level + atk_level * atk_level;
			if (atk_level == 10){
				basehurt_rate += 1;
			}
			break;
		case skill_pa_chuanci:
		case skill_pa_pet_chuanci:
			basehurt_rate = (45 + 10 * atk_level);
			break;
		case skill_pa_duochongjian:
			basehurt_rate = 40;
			break;
		case skill_pa_kuangji:
		case skill_pa_pet_kuangji:
			basehurt_rate = 100 + 10 * atk_level;
			break;
		case skill_pa_jizhonggongji:
		case skill_pa_pet_jizhonggongji:
			basehurt_rate = 100 + 5 * atk_level;
			break;
		case skill_pa_shuangxingjian:
		case skill_pa_pet_shuangxingjian:
			basehurt_rate = 35 + 5 * atk_level;
			break;

		case skill_pa_ninshenjian:
		case skill_pa_pet_ninshenjian :
			if (atk_level==1 ){
				basehurt_rate = 101;
			}else{
				basehurt_rate = 97 + 3 * atk_level;
			}
			break;
		case skill_rh_mingsi:
		case skill_rh_pet_mingsi :
			if (atk_level==0 ){
				basehurt_rate = (1 + rand() % 6) * 10;
			}else{
				basehurt_rate = 5 + (rand() % 6) * atk_level;
			}
			break;

		default :
			break;
	}


	return basehurt_rate;
}

void get_valid_alive_enemy_pos(battle_info_t* abi, warrior_t* p, atk_info_t* p_atk_info, skill_attr_t* psa)
{
	warrior_t* lp = NULL;	
	switch (psa->distance){
	case near_attack:
		break;
	case remote_attack:
		if (p_atk_info->atk_pos == -1){
			p_atk_info->atk_pos = get_rand_alive_warrior_pos(p->enemy_team,p->pet_state);
		}else{
			lp = get_attacked_warrior(p, p_atk_info);
			if (!lp || lp->is_not_in_battle_mode() || NONEED_ATTACK(lp)){
				p_atk_info->atk_pos = get_rand_alive_warrior_pos(p->enemy_team,p->pet_state);
			}
		}
		return;
	}
	
	// first find self pos
	int pos = get_warrior_pos(p->self_team, p->userid, p->petid);

	// if myself in the front  row
	if (pos >= 0 && pos < MAX_PLAYERS_PER_TEAM){
		lp = get_attacked_warrior(p, p_atk_info);
		if (!lp || NONEED_ATTACK(lp)|| lp->is_not_in_battle_mode()){
			p_atk_info->atk_pos = get_rand_alive_warrior_pos(p->enemy_team,p->pet_state);
		}
		return;
	}

	// first i want to attack the enemy's front row ok
	if(p_atk_info->atk_pos >= 0 && p_atk_info->atk_pos < MAX_PLAYERS_PER_TEAM){
		lp = get_attacked_warrior(p, p_atk_info);
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			p_atk_info->atk_pos = get_near_rand_alive_warrior_pos(p, pos);
		}
	}else{// then i want to attack enemy's back row
		if (p_atk_info->atk_pos < 0 || p_atk_info->atk_pos >= (MAX_PLAYERS_PER_TEAM * 2)){// [0, 9]
			p_atk_info->atk_pos = get_near_rand_alive_warrior_pos(p, pos);
			return;
		}else{
			lp = get_attacked_warrior(p, p_atk_info);
			//DEBUG_LOG("111get_valid_alive_enemy_pos %u",p_atk_info->atk_pos);
			if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
				p_atk_info->atk_pos = get_near_rand_alive_warrior_pos(p, pos);
				return;
			}
			//DEBUG_LOG("222get_valid_alive_enemy_pos %u",p_atk_info->atk_pos);
			int enemy_front = p_atk_info->atk_pos - MAX_PLAYERS_PER_TEAM;
			warrior_t* lp_enemy_front = p->enemy_team->players_pos[enemy_front];
			if (!lp_enemy_front || NONEED_ATTACK(lp_enemy_front) || lp_enemy_front->is_not_in_battle_mode()){
				return;
			}

			int self_front = pos - MAX_PLAYERS_PER_TEAM;
			warrior_t* lp_self_front = p->self_team->players_pos[self_front];
			if (!lp_self_front || CANNOT_ATTACK(lp_self_front) || lp_self_front->is_not_in_battle_mode()){
				return;
			}
			p_atk_info->atk_pos = get_near_rand_alive_warrior_pos(p, pos);
			//DEBUG_LOG("222get_valid_alive_enemy_pos %u",p_atk_info->atk_pos);
		}
	}

	return;
}

static inline warrior_t* get_attacked_warrior_by_distance(battle_info_t * abi, warrior_t* attacker, atk_info_t* p_atk_info, skill_attr_t* psa)
{

	//DEBUG_LOG("get_attacked_warrior_by_distance%u %u",p_atk_info->atk_mark,psa->distance);
	if (p_atk_info->atk_mark){
		switch(psa->distance){
		case near_attack:
			get_valid_alive_enemy_pos(abi, attacker, p_atk_info, psa);
			return get_warrior_by_pos(attacker->enemy_team, p_atk_info->atk_pos);
		default:
			return get_warrior_by_pos(attacker->enemy_team, p_atk_info->atk_pos);
		}
	}
	else{
		return get_warrior_by_pos(attacker->self_team, p_atk_info->atk_pos);
	}
}

warrior_t* get_valid_attack_warrior(battle_info_t* abi, warrior_t* p, atk_info_t* p_atk_info, skill_attr_t* psa)
{
	warrior_t* p_enemy = NULL;
	//检查目标
	//DEBUG_LOG("get_valid_attack_warrior %u %u %u %u",p->userid,p_atk_info->atk_type,p_atk_info->atk_pos,psa->target_type);
	switch(psa->target_type){
	case self_only:
		p_atk_info->atk_mark = 0;
		p_atk_info->atk_pos = get_warrior_pos(p->self_team, p->userid, p->petid);
		return p;
	case except_self:
		//DEBUG_LOG("0000get_valid_attack_warrior %u %u %u ",p_atk_info->atk_mark,!p_enemy , p_enemy == p );
		if (p_atk_info->atk_mark == 0){
			// check attack obj is myself or is noneed attack
			p_enemy = get_attacked_warrior(p, p_atk_info);
			if (!p_enemy || p_enemy == p || NONEED_ATTACK(p_enemy)){
				p_atk_info->atk_pos = get_rand_alive_warrior_pos_ex(p->self_team, p);
				//DEBUG_LOG("111 pos %u",p_atk_info->atk_pos);
				if (p_atk_info->atk_pos == -1){
					p_atk_info->atk_mark = 1;
					get_valid_alive_enemy_pos(abi, p, p_atk_info, psa);
				}
			}
		}else{
			//DEBUG_LOG("000 pos %u %u",p_atk_info->atk_pos,!p_enemy);
			p_enemy = get_attacked_warrior_by_distance(abi, p, p_atk_info, psa);
			//DEBUG_LOG("111 pos %u %u",p_atk_info->atk_pos,!p_enemy);
			if (!p_enemy || NONEED_ATTACK(p_enemy)){
				get_valid_alive_enemy_pos(abi, p, p_atk_info, psa);
			}

			//DEBUG_LOG("222 pos %u %u",p_atk_info->atk_pos,!p_enemy);
		}
		break;
	case self_team:
		if (p_atk_info->atk_mark){
			p_atk_info->atk_mark = 0;
			get_valid_alive_enemy_pos(abi, p, p_atk_info, psa);
		}else{
			p_enemy = get_attacked_warrior(p, p_atk_info);
			if (!p_enemy || (NONEED_ATTACK(p_enemy) && psa->applystatus == attack_only_alive) 
				|| (!(NONEED_ATTACK(p_enemy)) && psa->applystatus == attack_only_dead)){
				switch(psa->applystatus){
				case attack_only_alive:
					p_atk_info->atk_pos = get_rand_alive_warrior_pos(p->self_team,p->pet_state);
					break;
				case attack_only_dead:
					p_atk_info->atk_pos = get_rand_disable_warrior_pos(p->self_team);
					break;
				}
			}
		}
		break;
	case enemy_team:
		if (p_atk_info->atk_mark == 0){
			p_atk_info->atk_mark = 1;
			get_valid_alive_enemy_pos(abi, p, p_atk_info, psa);
		}else{
			p_enemy = get_attacked_warrior_by_distance(abi, p, p_atk_info, psa);
			if (!p_enemy || NONEED_ATTACK(p_enemy)){
				get_valid_alive_enemy_pos(abi, p, p_atk_info, psa);
			}
		}
		break;
	case all_attacked:
		if (p_atk_info->atk_mark == 0){
			p_enemy = get_attacked_warrior(p, p_atk_info);
			if (!p_enemy || NONEED_ATTACK(p_enemy)){
				p_atk_info->atk_pos = get_rand_alive_warrior_pos(p->self_team,p->pet_state);
				if (p_atk_info->atk_pos == -1){
					p_atk_info->atk_mark = 1;
					get_valid_alive_enemy_pos(abi, p, p_atk_info, psa);
				}
			}
		}else{
			get_valid_alive_enemy_pos(abi, p, p_atk_info, psa);
		}
		break;
	case self_team_except_self:
		if (p_atk_info->atk_mark == 0){
			// check attack obj is myself or is noneed attack
			p_enemy = get_attacked_warrior(p, p_atk_info);
			if (!p_enemy || p_enemy == p || NONEED_ATTACK(p_enemy)){
				p_atk_info->atk_pos = get_rand_alive_warrior_pos_ex(p->self_team, p);
				if (p_atk_info->atk_pos == -1){
					p_atk_info->atk_mark = 1;
					get_valid_alive_enemy_pos(abi, p, p_atk_info, psa);
				}
			}
		}else{
			p_atk_info->atk_mark = 0;
			p_atk_info->atk_pos = get_rand_alive_warrior_pos_ex(p->self_team, p);
		}
		break;
	}
	DEBUG_LOG("get_valid_attack_warrior %u %u %u",p->userid,p_atk_info->atk_type,p_atk_info->atk_pos);
	if (p_atk_info->atk_pos == -1){
		return NULL;
	}

	return get_attacked_warrior(p, p_atk_info);
}

warrior_t* get_valid_attack_warrior_ex(battle_info_t* abi, warrior_t* p, atk_info_t* p_atk_info)
{
	//DEBUG_LOG("get_valid_attack_warrior_ex");
	skill_attr_t* psa = get_skill_attr(p_atk_info->atk_type, p->weapon_type);
	//DEBUG_LOG("get_valid_attack_warrior_ex111 %u %u %u",psa!=NULL,psa->id,p_atk_info->atk_type);
	if (!psa || !psa->id){
		return NULL;
	}

	if (IS_BEAST(p)){
		switch(psa->usr_type){
		case user_ren_only:
		case user_beast_only:
			if (p->petid){// pet
				ERROR_LOG("CANNOT USE THIS SKILL<USER>\t[batid=%lu uid=%u petid=%u atk_type=%u prof=%u]", abi->batid, p->userid, p->petid, p_atk_info->atk_type, p->prof);
				p_atk_info->atk_pos  = get_warrior_pos(p->self_team, p->userid, p->petid);
				p_atk_info->atk_mark = 1; // enemy
				p_atk_info->atk_type = (IS_BEAST(p)) ? skill_pa_pet_dai_ji : skill_pa_dai_ji;
				p_atk_info->atk_level = 1;
			}
			break;
		}
	}

	if ((psa->needshield == must_have_shield && !p->shield) || (psa->needshield == must_not_shield && p->shield)) {
		ERROR_LOG("shield invalid\t[uid=%u petid=%u atk_type=%u %u %u]", p->userid, p->petid, p_atk_info->atk_type, p_atk_info->atk_mark, p->shield);
		p_atk_info->atk_type = (IS_BEAST(p)) ? skill_pa_pet_dai_ji : skill_pa_dai_ji;
		p_atk_info->atk_level = 1;
		psa = get_skill_attr(p_atk_info->atk_type, p->weapon_type);
		if (!psa || !psa->id){
			return NULL;
		}
	}
	//DEBUG_LOG("get_valid_attack_warrior_ex %u %u %u",p->userid,p_atk_info->atk_type,p_atk_info->atk_pos);
	return get_valid_attack_warrior(abi, p, p_atk_info, psa);
}

//单人操作处理(通常)
resolved_atk_t* normal_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	//得到被攻击者
	warrior_t* pwar = get_attacked_warrior(p, p_atk_info);

	//得到攻击属性
	skill_attr_t* psa = get_skill_attr(p_atk_info->atk_type, p->weapon_type);

	
	if (!psa || !psa->id) {//技能不存在
		ERROR_LOG("CANNOT USE THIS SKILL<RESOLVE STEP BATTLE RESULT>\t[batid=%lu %u %u %u %u]", abi->batid, p->userid, p->petid, p_atk_info->atk_type, p->weapon_type);
		//设置为待机
		p_atk_info->atk_pos  = get_warrior_pos(p->self_team, p->userid, p->petid);
		p_atk_info->atk_mark = 1;
		p_atk_info->atk_type = (IS_BEAST(p)) ? skill_pa_pet_dai_ji : skill_pa_dai_ji;
		p_atk_info->atk_level = 1;
		psa = get_skill_attr(p_atk_info->atk_type, p->weapon_type);
	}

	//得到技能施加对象
	if (!pwar || NONEED_ATTACK(pwar)){
		//如果是使用复活药 则可以对死人使用
		if(pwar && !IS_REVIVING_MEDICAL(p->p_waor_state->get_use_item_itemid())){
			if (p_atk_info->atk_mark){
				pwar = get_rand_alive_warrior(p->enemy_team,p->pet_state );
			}else{
				pwar = get_rand_alive_warrior(p->self_team,p->pet_state );
			}
		}
		
		if (!pwar){
			return NULL;
		}
	}
	//设置技能施加信息
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = pwar->userid;
	p_resolved_atk->atk_petid = pwar->petid;
	p_resolved_atk->hurthp_rate = get_attack_hurthp_rate(p, p_atk_info, abi, enemy);
	DEBUG_LOG("RESOLVE NORMAL[fiter=%u sorp=%u atk_uid=%u atk_petid=%u atk_type=%u atk_mark=%u]", p_resolved_atk->fighter_id, p_resolved_atk->petid, p_resolved_atk->atk_uid, p_resolved_atk->atk_petid, p_resolved_atk->atk_type, p_atk_info->atk_mark);
	return p_resolved_atk;
}

resolved_atk_t* shunsha_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	
	if (!lp){
		return NULL;
	}

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = get_attack_hurthp_rate(p, p_atk_info, abi, enemy);
	return p_resolved_atk;
}

resolved_atk_t* skill_lunzhuanfengbao_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	std::list <warrior_t*> warrior_list;
	p->get_t_user_list(p_atk_info->atk_pos, p_atk_info->atk_mark==0, warrior_list );
	std::list <warrior_t*>::iterator it;
	for (it=warrior_list.begin(); it!=warrior_list.end();++it ){
		warrior_t* lp=*it;
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		//KDEBUG_LOG(p->userid,"lunzhuanfengbao fightee:%u %u",lp->userid,lp->petid);
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* ruodiantouxi_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	DEBUG_LOG("use ruodiantouxi");
	std::list <warrior_t*> warrior_list;
	p->get_t_user_list(p_atk_info->atk_pos, true, warrior_list );
	std::list <warrior_t*>::iterator it;
	for (it=warrior_list.begin(); it!=warrior_list.end();++it ){
		warrior_t* lp=*it;
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
		lp->ruodiantouxi_lv=p_atk_info->atk_level;	
	}
	return NULL;
}

resolved_atk_t* dubaoshu_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	DEBUG_LOG("use dubaoshu");
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	
	if (!lp){
		return NULL;
	}
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = 100;
	return p_resolved_atk;
}
//XXX
resolved_atk_t* gaopinzhenji_resolve_sprite_pet_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	DEBUG_LOG("use gaopinzhenji");
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	//if(p->p_waor_state->effect_cnt[gaopin_zhenji_bit]>0){
		//KDEBUG_LOG(p->userid,"gaopinzhenji error,effect cnt=%u",p->p_waor_state->effect_cnt[gaopin_zhenji_bit]>0);
		//p_atk_info->atk_type=10;	
	//}	
	if (!lp){
		return NULL;
	}

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = 100;
	return p_resolved_atk;
}


resolved_atk_t* fang_yu_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	return NULL;
}

resolved_atk_t* run_away_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = p->userid;
	p_resolved_atk->atk_petid = p->petid;

	warrior_t* lp = get_rand_alive_warrior(p->enemy_team,p->pet_state );
	if (lp == NULL){
		return NULL;
	}
	
	return p_resolved_atk;
}

resolved_atk_t* xian_zhi_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* pwar = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!pwar){
		return NULL;
	}

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = pwar->userid;
	p_resolved_atk->atk_petid = pwar->petid;
	p_resolved_atk->hurthp_rate = 100;
	return p_resolved_atk;

}

resolved_atk_t* base_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* pwar = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!pwar){
		return NULL;
	}
	
	DEBUG_LOG("base_resolve_sprite_atk: uid=%u,petid=%u ; pwar->userid=%u pwar->petid=%u", 
			 p->userid,p->petid, pwar->userid,pwar->petid);
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = pwar->userid;
	p_resolved_atk->atk_petid = pwar->petid;
	p_resolved_atk->hurthp_rate = 100;
	return p_resolved_atk;

}

//救所有的人
resolved_atk_t* jiushu_all_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	//设置成
	if (IS_BEAST( p)) {
		p_atk_info->atk_type= skill_rl_pet_jiushu;
	}else{
		p_atk_info->atk_type= skill_rl_jiushu;
	}

	//处理2008
	if (abi->is_pk_beast_2008())	{
		bool is_jiushu_all=false;	
		if (p->type_id==TYPEID_2008 ){
			 is_jiushu_all=true;
		}

		for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM; loop++) {
			warrior_t* lp = p->self_team->players_pos[loop];
			if (!lp || lp->is_not_in_battle_mode() ){
				continue;
			}

			if (!is_jiushu_all && lp->type_id!=TYPEID_2008  ){
				continue;
			}

			if (!( WARRIOR_DEAD(lp) || WARRIOR_ATTACKED_OUT(lp) )) {//没有死或打飞
				continue;
			}
			if (WARRIOR_ATTACKED_OUT(lp)){//
				RESET_WARRIOR_STATE(lp->p_waor_state->state,attacked_out_bit  );
				SET_WARRIOR_STATE(lp->p_waor_state->state,dead_bit );
			}
			KDEBUG_LOG( lp->userid, "jiushu_all: petid=%u", lp->petid);
			resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
			p_resolved_atk->atk_uid = lp->userid;
			p_resolved_atk->atk_petid = lp->petid;

			p_resolved_atk->hurthp_rate = lp->hp_max*100 ;
			p_resolved_atk->hurtmp_rate= lp->mp_max*100 ;
			calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
		}

	}else{//2004||2005
		//只救三只出来
		 int deal_count=0;
		for (int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM && deal_count<3 ; loop++) {
			warrior_t* lp = p->self_team->players_pos[loop];
			if (!lp || lp->is_not_in_battle_mode() ){
				continue;
			}
			if (!( WARRIOR_DEAD(lp)||  WARRIOR_ATTACKED_OUT(lp) )) {//没有死或打飞
				continue;
			}
			if (WARRIOR_ATTACKED_OUT(lp)){//
				RESET_WARRIOR_STATE(lp->p_waor_state->state,attacked_out_bit  );
				SET_WARRIOR_STATE(lp->p_waor_state->state,dead_bit );
			}

			lp->p_waor_state->state &= ~abnormal_states;
			KDEBUG_LOG( lp->userid, "2004||2005 jiushu_all: petid=%u", lp->petid);
			resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
			p_resolved_atk->atk_uid = lp->userid;
			p_resolved_atk->atk_petid = lp->petid;

			p_resolved_atk->hurthp_rate = lp->hp_max*100 ;
			p_resolved_atk->hurtmp_rate= lp->mp_max*100 ;
			calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
			deal_count++;
		}
	}
	return NULL;
}

resolved_atk_t* sanyeshi_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	int loop;
	warrior_t* lp[4];
	resolved_atk_t* p_resolved_atk;
	int cnt = (p_atk_info->atk_level + 2) / 3;
	
	if (CANNOT_ATTACK(p)) return NULL;

	for (loop=0; loop < cnt; loop++) {
		lp[loop] = get_valid_attack_warrior_ex(abi, p, p_atk_info);
		if (lp[loop] && !NONEED_ATTACK(lp[loop])) {
			lp[loop]->flag_set_bit(bit_attacked,1);

			p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
			p_resolved_atk->atk_uid = lp[loop]->userid;
			p_resolved_atk->atk_petid = lp[loop]->petid;
			p_resolved_atk->hurthp_rate = 100;
			float rate_opt[]={1.1,1.2,1.3,0.7,0.75,0.8,0.6,0.65,0.7,0.6 };
			p_resolved_atk->hurthp_rate *= rate_opt[p_resolved_atk->atk_level-1];
			calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
			p_atk_info->atk_pos = -1;
		}
	}

	for (loop=0; loop < cnt; loop++) {
		if (lp[loop] && !NONEED_ATTACK(lp[loop])) {
			lp[loop]->flag_set_bit(bit_attacked,0);
		}
	}

	return NULL;
}

resolved_atk_t* mengshouliexi_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (lp == NULL || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode() ){
		return NULL;
	}

	p_atk_info->atk_step = 2;
	RESET_WARRIOR_STATE(p->p_waor_state->one_loop_state, one_loop_bit_huiji);

	int loop;
	for (loop = 0; loop < 2; loop++) {
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		switch ( loop ){
			case 0:
				p_resolved_atk->hurthp_rate = 100+ p_atk_info->atk_level*3-2;
				break;
			case 1:
				p_resolved_atk->hurthp_rate = 55+ p_atk_info->atk_level*5;
				break;
			default :
				break;
		}
		p_atk_info->atk_step--;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);

		if (lp->hp!=0 ){//没有死，直接返回
			break;
		}
		
		if (abi->challger.ren_alive_count <= 0 || abi->challgee.ren_alive_count <= 0){
			break;
		}

		if (CANNOT_ATTACK(p) || CHK_WARRIOR_STATE(p->p_waor_state->one_loop_state, one_loop_bit_huiji)){
			break;
		}

		if ((loop + 1) < 2) {
			p_atk_info->atk_pos = -1;
			lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
			if (lp == NULL || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode() ){
				break; // all ememy are died!
			}
		}
	}
	return NULL;
}



resolved_atk_t* zhuiji_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	int cnt = (p_atk_info->atk_level - 1) / 2  + 2;
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (lp == NULL || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode() ){
		return NULL;
	}

	p_atk_info->atk_step = cnt;
	RESET_WARRIOR_STATE(p->p_waor_state->one_loop_state, one_loop_bit_huiji);
	int loop;
	for (loop = 0; loop < cnt; loop++) {
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = 100;
		switch(p_atk_info->atk_level){
			case 1:
			case 2:
				p_resolved_atk->hurthp_rate *= 0.7;
				break;
			case 3:
			case 4:
				p_resolved_atk->hurthp_rate *= 0.5;
				break;
			case 5:
			case 6:
				p_resolved_atk->hurthp_rate *= 0.4;
				break;
			case 7:
			case 8:
				p_resolved_atk->hurthp_rate *= 0.34;
				break;
			case 9:
			case 10:
				p_resolved_atk->hurthp_rate *= 0.3;
				break;
		}
		p_atk_info->atk_step--;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
		
		if (abi->challger.ren_alive_count <= 0 || abi->challgee.ren_alive_count <= 0){
			break;
		}

		if (CANNOT_ATTACK(p) || CHK_WARRIOR_STATE(p->p_waor_state->one_loop_state, one_loop_bit_huiji)){
			break;
		}

		if ((loop + 1) < cnt && (p_atk_info->atk_level % 2 || NONEED_ATTACK(lp))) {
			p_atk_info->atk_pos = -1;
			lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
			if (lp == NULL || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode() ){
				break; // all ememy are died!
			}
		}
	}
	return NULL;
}

resolved_atk_t* bengya_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
	
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	
	p_resolved_atk->hurthp_rate = 99 + 20 * p_atk_info->atk_level + p_atk_info->atk_level * p_atk_info->atk_level;
	if (p_atk_info->atk_level == 10){
		p_resolved_atk->hurthp_rate += 1;
	}
	return p_resolved_atk;
}

resolved_atk_t* chuanci_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}

	int pos = get_warrior_pos(lp->self_team, lp->userid, lp->petid);
	int enem_pos[2] = {0};
	enem_pos[0] = (pos < MAX_PLAYERS_PER_TEAM)?pos:(pos - MAX_PLAYERS_PER_TEAM);
	enem_pos[1] = (pos < MAX_PLAYERS_PER_TEAM)?(pos + MAX_PLAYERS_PER_TEAM):pos;

	uint8_t chuanchi = 0;
	p_atk_info->atk_pos = enem_pos[0];
	warrior_t* pw_front = get_attacked_warrior(p, p_atk_info);
	if (!pw_front || NONEED_ATTACK(pw_front) || pw_front->is_not_in_battle_mode()){
	}else{
		chuanchi++;
	}

	p_atk_info->atk_pos = enem_pos[1];
	warrior_t* pw_back = get_attacked_warrior(p, p_atk_info);
	if (!pw_back || NONEED_ATTACK(pw_back) || pw_back->is_not_in_battle_mode()){
	}else{
		chuanchi++;
	}

	if (!pw_front || NONEED_ATTACK(pw_front) || pw_front->is_not_in_battle_mode()){
	}else{
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = pw_front->userid;
		p_resolved_atk->atk_petid = pw_front->petid;
		if (chuanchi == 1){
			p_resolved_atk->hurthp_rate = (45 + 10 * p_atk_info->atk_level);
		}else{
			p_resolved_atk->hurthp_rate = (enem_pos[0] < MAX_PLAYERS_PER_TEAM) ? 45 : (45 + 10 * p_atk_info->atk_level);
		}
		
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}


	if (!pw_back || NONEED_ATTACK(pw_back) || pw_back->is_not_in_battle_mode()){
	}else{
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = pw_back->userid;
		p_resolved_atk->atk_petid = pw_back->petid;
		if (chuanchi == 1){
			p_resolved_atk->hurthp_rate = (45 + 10 * p_atk_info->atk_level);
		}else{
			p_resolved_atk->hurthp_rate = (enem_pos[1] < MAX_PLAYERS_PER_TEAM) ? 45 : (45 + 10 * p_atk_info->atk_level);
		}
		
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}
	
	return NULL;
}

resolved_atk_t* duochongjian_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	int counts[MAX_PLAYERS_PER_TEAM * 2] = {0};
	int cnt = p_atk_info->atk_level + 2;
	int loop;
	
	int hurthp_rate[3] = {40,25,15};
	
	warrior_t* lpememy = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lpememy){
		return NULL;
	}
	
	for (loop = 0; loop < cnt; loop++) {
		warrior_t* lp = (!loop) ? lpememy :	get_valid_attack_warrior_ex(abi, p, p_atk_info);
		if (lp == NULL){
			break;
		}

		int idx = get_warrior_pos(enemy, lp->userid, lp->petid);

		counts[idx]++;
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = (counts[idx] < 4) ? hurthp_rate[counts[idx]-1] : 5;

		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
		p_atk_info->atk_pos = -1;
	}
	return NULL;
}

resolved_atk_t* kuangji_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	
	if (!lp){
		return NULL;
	}

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = 100 + 10 * p_atk_info->atk_level;
	return p_resolved_atk;
}

resolved_atk_t* meiying_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
	
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = 100;
	return p_resolved_atk;
}
resolved_atk_t* callback_fuzhu_resolve_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	DEBUG_LOG("===callback_fuzhu_resolve_atk ");
	p_atk_info->atk_type = skill_callback_fuzhu;
	if (!abi->is_one_person_pk_beast())
			return NULL;
	
	return alloc_resolve_atk(p, p_atk_info, abi, 0);
}



resolved_atk_t* huishang_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
		
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = 100;
	return p_resolved_atk;
}

resolved_atk_t* jizhonggongji_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	
	if (!lp){
		return NULL;
	}

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = 100 + 5 * p_atk_info->atk_level;
	return p_resolved_atk;
}

resolved_atk_t* douqi_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}


	int cnt = (p_atk_info->atk_level + 1) / 2;
	if (p_atk_info->atk_level % 2 == 0) {
		cnt += rand() % 2;
	}
	if (p_atk_info->atk_level == 10) cnt++;
	

	int deal_count=0;
	for ( int loop = 0; loop < 2 * MAX_PLAYERS_PER_TEAM && deal_count<cnt ; 
			loop++) {
		warrior_t* lp = p->enemy_team->players_pos[loop];
		if (!lp){
			continue;
		}
		
		if (NONEED_ATTACK(lp) || lp->is_not_in_battle_mode() ) {
			continue;
		}
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = 100;
		p_atk_info->atk_pos=-1;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);

		deal_count++;	
	}

	return NULL;
}

resolved_atk_t* shuangxingjian_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}

	int cnt = 2;
	int loop;
	for (loop = 0; loop < cnt; loop++) {
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = (loop) ? (35 + 5 * p_atk_info->atk_level) : (55 + 5 * p_atk_info->atk_level);
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);

		if (NONEED_ATTACK(lp)){
			p_atk_info->atk_pos = -1;
			lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
			if (!lp){
				break;
			}
		}
	}
	return NULL;
}

resolved_atk_t* ninshengyiji_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;

	switch(p_atk_info->atk_level){
	case 1:
		p_resolved_atk->hurthp_rate = 101;
		break;
	default:
		p_resolved_atk->hurthp_rate = 97 + 3 * p_atk_info->atk_level;
		break;
	}
	
	return p_resolved_atk;
}

//劲透拳	
resolved_atk_t* skill_jintouquan_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = 100;

	return p_resolved_atk;
}

resolved_atk_t* pohun_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	
	p_resolved_atk->hurtmp_rate = 5 * p_atk_info->atk_level;
	return p_resolved_atk;
}

resolved_atk_t* skill_qiliaodun_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{

	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
	
	resolved_atk_t* pra = alloc_resolve_atk(p, p_atk_info, abi, 0);
	pra->atk_uid = lp->userid;
	pra->atk_petid = lp->petid;
	
		
	pra->hurthp_rate = 4+p_atk_info->atk_level
		+rand()%(4+p_atk_info->atk_level*2);
	DEBUG_LOG("skill_qiliaodun hp_rate=%u",pra->hurthp_rate);

	return pra;
}

resolved_atk_t* mingsi_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
	
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	
	switch(p_atk_info->atk_level){
	case 10:
		p_resolved_atk->hurthp_rate = (1 + rand() % 6) * 10;
		break;
	default:
		p_resolved_atk->hurthp_rate = 5 + (rand() % 6) * p_atk_info->atk_level;
		break;
	}

	return p_resolved_atk;
}

resolved_atk_t* zhiyuzhizhen_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = 300 * p_atk_info->atk_level;
	
	return p_resolved_atk;
}


int get_valid_t_attack_cross_pos_ex(int curpos, battle_users_t* aui)
{
	if (curpos < 0 || curpos >= (MAX_PLAYERS_PER_TEAM * 2)){
		return -1;
	}

	int lpos = 0;
	uint8_t haswarrior = 0;
	for (lpos = 0; lpos < 4; lpos++){
		int war_pos = t_users[curpos][lpos];
		if (war_pos < 0 || war_pos >= (MAX_PLAYERS_PER_TEAM * 2)){
			continue;
		}
		
		warrior_t* lp = aui->players_pos[war_pos];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}
		
		haswarrior = 1;
		break;
	}

	if (haswarrior){
		return curpos;
	}

	return get_valid_t_attack_cross_pos(curpos, aui);
}

int get_valid_t_attack_cross_pos(int curpos, battle_users_t* aui)
{
	if (curpos < 0 || curpos >= (MAX_PLAYERS_PER_TEAM * 2)){
		return -1;
	}
	
	int lpos = t_users[curpos][0];
	warrior_t* lp = aui->players_pos[lpos];
	if (lp && !NONEED_ATTACK(lp) && !lp->is_not_in_battle_mode()){
		return curpos;
	}

	int loop = 0;
	for (loop = 0; loop < curpos; loop++){
		lpos = t_users[loop][0];
		lp = aui->players_pos[lpos];
		if (lp && !NONEED_ATTACK(lp) && !lp->is_not_in_battle_mode()){
			return loop;
		}
	}

	int cnt = MAX_PLAYERS_PER_TEAM * 2;
	for (loop = curpos + 1; loop < cnt; loop++){
		lpos = t_users[loop][0];
		lp = aui->players_pos[lpos];
		if (lp && !NONEED_ATTACK(lp) && !lp->is_not_in_battle_mode()){
			return loop;
		}
	}

	return -1;
}

resolved_atk_t* zhiyulingyu_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	//TODO
	std::list <warrior_t*> warrior_list;
	p->get_t_user_list(p_atk_info->atk_pos, true, warrior_list );
	std::list <warrior_t*>::iterator it;
	for (it=warrior_list.begin(); it!=warrior_list.end();++it ){
		warrior_t* lp=*it;
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = 180 * p_atk_info->atk_level;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	};

	return NULL;
}

resolved_atk_t* kuangyezhifun_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	std::list <warrior_t*> warrior_list;
	p->get_t_user_list(p_atk_info->atk_pos, true, warrior_list );
	std::list <warrior_t*>::iterator it;
	for (it=warrior_list.begin(); it!=warrior_list.end();++it ){
		warrior_t* lp=*it;
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	};

	return NULL;
	
}
resolved_atk_t* zhiyuhuahuan_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	std::list <warrior_t*> warrior_list;
	p->get_t_user_list(p_atk_info->atk_pos,true, warrior_list );
	std::list <warrior_t*>::iterator it;
	for (it=warrior_list.begin(); it!=warrior_list.end();++it ){
		warrior_t* lp=*it;
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = 100 * p_atk_info->atk_level;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	};


	return NULL;
	
}


resolved_atk_t* zhiyujiejie_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	battle_users_t* ally = p->self_team;
	if (p_atk_info->atk_mark){
		ally = p->enemy_team;
	}
	int loop;
	for (loop = 0; loop < MAX_PLAYERS_PER_TEAM * 2; loop++) {
		warrior_t* lp = ally->players_pos[loop];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}

		// alloc resolve attack info
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = 120 * p_atk_info->atk_level;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* zaishengzhizhen_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* pwar = get_attacked_warrior(p, p_atk_info);
	if (!pwar || NONEED_ATTACK(pwar)){
		if (p_atk_info->atk_mark){
			pwar = get_rand_alive_warrior(p->enemy_team,p->pet_state );
		}else{
			pwar = get_rand_alive_warrior(p->self_team,p->pet_state );
		}
			
		if (!pwar){
			return NULL;
		}
	}
		

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = pwar->userid;
	p_resolved_atk->atk_petid = pwar->petid;
	p_resolved_atk->hurthp_rate = 100*p_atk_info->atk_level;
	return p_resolved_atk;

}

resolved_atk_t* zaishenglingyu_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{

	std::list <warrior_t*> warrior_list;
	p->get_t_user_list(p_atk_info->atk_pos, p_atk_info->is_opt_self_team() , warrior_list );
	std::list <warrior_t*>::iterator it;
	for (it=warrior_list.begin(); it!=warrior_list.end();++it ){
		warrior_t* lp=*it;
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = 60*p_atk_info->atk_level;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	};

	return NULL;

}

resolved_atk_t* zaishengjiejie_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	DEBUG_LOG("zaishengjiejie");
	battle_users_t* ally = p->self_team;
	if (p_atk_info->atk_mark){
		ally = p->enemy_team;
	}
	int loop;
	for (loop = 0; loop < MAX_PLAYERS_PER_TEAM * 2; loop++) {
		warrior_t* lp = ally->players_pos[loop];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}
	
		// alloc resolve attack info
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = 120*p_atk_info->atk_level;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* jiushu_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
	DEBUG_LOG("jiushu_resolve_sprite_atk:%u",lp->check_state( person_break_off));
	if (lp->check_state( person_break_off) ||
		lp->check_state( attacked_out_bit) ){
		int pos = get_rand_alive_warrior_pos(enemy,p->pet_state);
		if (pos == -1){
			return NULL;
		}
		
		p_atk_info->atk_pos = pos;
		p_atk_info->atk_mark = 1;
		p_atk_info->atk_type = (IS_BEAST(p)) ? skill_pa_pet_dai_ji : skill_pa_dai_ji;
		p_atk_info->atk_level = 1;
		lp = get_attacked_warrior(p, p_atk_info);
	}

	// alloc battle info
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	switch (p_atk_info->atk_type){
		case skill_pa_pet_base_fight:
		case skill_pa_base_fight:
			p_resolved_atk->hurthp_rate = 100;
			break;
		default:
			p_resolved_atk->hurthp_rate = 30000 * p_atk_info->atk_level;
			break;
	}

	return p_resolved_atk;
}
//气斗拳
resolved_atk_t* skill_qidouquan_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	DEBUG_LOG("skill_qidouquan_resolve_sprite_atk");
	skill_attr_t* psa = get_skill_attr(p_atk_info->atk_type, p->weapon_type);
	if (!psa || !psa->id){
		ERROR_LOG("invalid t attack skill\t[batid=%lu uid=%u petid=%u atk_type=%u weapon_type=%u]", abi->batid, p->userid, p->petid, p_atk_info->atk_type, p->weapon_type);
		return NULL;
	}

	battle_users_t* enemy_team = NULL;
	if (p_atk_info->atk_mark){
		enemy_team = p->enemy_team;
	}else{
		enemy_team = p->self_team;
	}
	
	uint32_t loop;
	uint32_t count=(1+p_atk_info->atk_level)/2 +((p_atk_info->atk_level%2==1)?0:(rand()%2));
	//DEBUG_LOG("count: %u %u %u %u",count,p_atk_info->atk_level,(1+p_atk_info->atk_level)/2,(p_atk_info->atk_level%2==1));
	std::vector<int> pos_index_list;
	//得到可攻击列表

	bool add_select_pos=false;
	for (loop = 0; loop <  MAX_PLAYERS_PER_TEAM * 2; loop++) {
		warrior_t* lp = enemy_team->players_pos[loop];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}
		if ((int)loop!=p_atk_info->atk_pos) {
			if (pos_index_list.size() <count){
				pos_index_list.push_back(loop);
			}else{//多出了
				pos_index_list[rand()%(count)]=loop;
			}
		}else{//是选中的目标
			add_select_pos=true;
		}
	}
	if (add_select_pos){
		if (pos_index_list.size() <count ){
			pos_index_list.push_back(p_atk_info->atk_pos);
		}else{
			pos_index_list[0]=p_atk_info->atk_pos;
		}

	}
	DEBUG_LOG("pos_index_list.size():%u %u",(uint32_t)pos_index_list.size(),count);
	uint32_t hurthp_rate=  120-5*p_atk_info->atk_level ; 
	for (uint32_t i=0;i<pos_index_list.size();i++ ){
		warrior_t* lp = enemy_team->players_pos[pos_index_list[i]];
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = hurthp_rate; 
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}

	return NULL;
}

resolved_atk_t* shenyuhuhuan_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	skill_attr_t* psa = get_skill_attr(p_atk_info->atk_type, p->weapon_type);
	if (!psa || !psa->id ||  p_atk_info->atk_pos < 0 || p_atk_info->atk_pos >= 10 ){
		ERROR_LOG("[batid=%lu uid=%u petid=%u atk_type=%u weapon_type=%u]",
			   	abi->batid, p->userid, p->petid, p_atk_info->atk_type, p->weapon_type);
		return NULL;
	}
	DEBUG_LOG("shenyuhuhuan");//
	for (uint32_t loop=0; loop < 4 ; loop++ ){
		int idx=t_users[p_atk_info->atk_pos][loop];
		if (idx == -1 ){
			continue;
		}
		warrior_t* lp = p->self_team->players_pos[idx];
		if (!lp || lp->is_not_in_battle_mode() ){
			continue;
		}

		if (lp->check_state( person_break_off) || lp->check_state( attacked_out_bit) ) {//没有死或打飞
			continue;
		}

		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		// alloc battle info
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate =  (200+100 * p_atk_info->atk_level)*100;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* t_attacked_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	skill_attr_t* psa = get_skill_attr(p_atk_info->atk_type, p->weapon_type);
	if (!psa || !psa->id){
		ERROR_LOG("invalid t attack skill\t[batid=%lu uid=%u petid=%u atk_type=%u weapon_type=%u]", abi->batid, p->userid, p->petid, p_atk_info->atk_type, p->weapon_type);
		return NULL;
	}
	std::list <warrior_t*> warrior_list;
	switch ( psa->target_scale ){
		case 1 :
			p->get_t_user_list(p_atk_info->atk_pos, p_atk_info->atk_mark==0, warrior_list );
			break;
		case 2 :
			p->get_col_user_list(p_atk_info->atk_pos, p_atk_info->atk_mark==0, warrior_list );
			break;
		case 3:
			p->get_row_user_list(p_atk_info->atk_pos, p_atk_info->atk_mark==0, warrior_list );
			break;
		default :
			return NULL;
	}
	DEBUG_LOG("t_attacked_resolve_sprite_atk skill=%u target_scale=%u size=%lu",psa->id,psa->target_scale,warrior_list.size());
	std::list <warrior_t*>::iterator it;
	for (it=warrior_list.begin(); it!=warrior_list.end();++it ){
		warrior_t* lp=*it;
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);

		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = get_attack_hurthp_rate(p, p_atk_info, abi, enemy);
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* all_attacked_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	battle_users_t* enemy_team = NULL;
	if (p_atk_info->atk_mark){
		enemy_team = p->enemy_team;
	}else{
		enemy_team = p->self_team;
	}
	int loop;
	for (loop = 0; loop < MAX_PLAYERS_PER_TEAM * 2; loop++) {
		warrior_t* lp = enemy_team->players_pos[loop];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = get_attack_hurthp_rate(p, p_atk_info, abi, enemy_team);
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* bingqingzhizhen_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}

	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = 100;
	
	return p_resolved_atk;
}

resolved_atk_t* bingqinglingyu_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	std::list <warrior_t*> warrior_list;
	p->get_t_user_list(p_atk_info->atk_pos, true, warrior_list );
	std::list <warrior_t*>::iterator it;
	for (it=warrior_list.begin(); it!=warrior_list.end();++it ){
		warrior_t* lp=*it;
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);

		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = 100;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* bingqingjiejie_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	if (p_atk_info->atk_mark){
		enemy = p->enemy_team;
	}else{
		enemy = p->self_team;
	}
	
	for (int loop = 0; loop < MAX_PLAYERS_PER_TEAM * 2; loop++) {
		warrior_t* lp = enemy->players_pos[loop];
		if (!lp || NONEED_ATTACK(lp) || lp->is_not_in_battle_mode()){
			continue;
		}
		resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
		p_resolved_atk->hurthp_rate = 100;
		calc_warrior_single_step_battle_result(abi, p_resolved_atk, p_atk_info);
	}
	return NULL;
}

resolved_atk_t* chg_pos_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);

	warrior_t* lp = get_warrior_by_pos(p->self_team, p_atk_info->atk_pos);
	p_resolved_atk->atk_uid = (lp) ? lp->userid : 0;
	//如果宠物和人都使用换位，则直接如下处理
	if (p->petid){
		//如果宠物操作,对方就是人
		p_resolved_atk->atk_petid = 0;
	}else{//反之设置petid
		p_resolved_atk->atk_petid = (lp) ? lp->petid : 0;
	}

	return p_resolved_atk;
}

resolved_atk_t* catch_pet_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_warrior_by_pos(p->enemy_team, p_atk_info->atk_pos);
	if (!lp || NONEED_ATTACK(lp)){

		notify_catch_pet(abi, p, (lp)?lp->userid:0, 2, (lp)?lp->level:0);//success is 2 means the pet has been caught by someone else
		return NULL;
	}
	
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	if (lp){
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
	}

	return p_resolved_atk;
}

resolved_atk_t* recall_pet_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);

	if (p_atk_info->atk_pos != -1){
		warrior_t* lp = &(p->self_team->players[p_atk_info->atk_pos]);
		p_resolved_atk->atk_uid = lp->userid;
		p_resolved_atk->atk_petid = lp->petid;
	}

	return p_resolved_atk;

}

resolved_atk_t* dai_ji_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	return NULL;
}

resolved_atk_t* shengmingchouqu_resolve_sprite_atk(warrior_t* p, atk_info_t* p_atk_info, battle_info_t* abi, battle_users_t* enemy)
{
	warrior_t* lp = get_valid_attack_warrior_ex(abi, p, p_atk_info);
	if (!lp){
		return NULL;
	}
	
	resolved_atk_t* p_resolved_atk = alloc_resolve_atk(p, p_atk_info, abi, 0);
	p_resolved_atk->atk_uid = lp->userid;
	p_resolved_atk->atk_petid = lp->petid;
	p_resolved_atk->hurthp_rate = 100;

	return p_resolved_atk;
}

void init_attck_proc_func() 
{
	before_round_sp_atk_func[skill_pa_xianfazhiren] 	= xianfazhiren_sprite_before_round_atk;
	after_round_sp_atk_func[skill_pa_xianfazhiren] 		= xianfazhiren_sprite_after_round_atk;

	before_round_sp_atk_func[skill_jinghunquan] 	=  	skill_jinghunquan_before_round_atk;
	after_round_sp_atk_func[skill_jinghunquan] 	= skill_jinghunquan_after_round_atk;

	before_round_sp_atk_func[skill_jintouquan] 	=  	skill_jintouquan_before_round_atk;
	after_round_sp_atk_func[skill_jintouquan] 	= skill_jintouquan_after_round_atk;

	before_round_sp_atk_func[skill_jueqiquan] 	=  	skill_jueqiquan_before_round_atk;
	after_round_sp_atk_func[skill_jueqiquan] 	= skill_jueqiquan_after_round_atk;

	before_round_sp_atk_func[skill_qidouquan] 	=  	skill_qidouquan_before_round_atk;
	after_round_sp_atk_func[skill_qidouquan] 	= skill_qidouquan_after_round_atk;

	before_round_sp_atk_func[skill_pa_pet_xianfazhiren] = xianfazhiren_sprite_before_round_atk;
	after_round_sp_atk_func[skill_pa_pet_xianfazhiren] 	= xianfazhiren_sprite_after_round_atk;
	
	before_round_sp_atk_func[skill_pa_kuangji] 			= kuangji_sprite_before_round_atk;
	after_round_sp_atk_func[skill_pa_kuangji] 			= kuangji_sprite_after_round_atk;

	before_round_sp_atk_func[skill_zaiezhizhao] 			= skill_zaiezhizhao_before_round_atk;
	after_round_sp_atk_func[ skill_zaiezhizhao] 			= skill_zaiezhizhao_after_round_atk;

	before_round_sp_atk_func[skill_hundunzhiren] 			= skill_hundunzhiren_before_round_atk;
	after_round_sp_atk_func[ skill_hundunzhiren] 			= skill_hundunzhiren_after_round_atk;

	before_round_sp_atk_func[skill_shenpanzhilun] 			= skill_shenpanzhilun_before_round_atk;
	after_round_sp_atk_func[ skill_shenpanzhilun] 			= skill_shenpanzhilun_after_round_atk;
	
	
	before_round_sp_atk_func[skill_pa_jizhonggongji]	= jizhonggongji_sprite_before_round_atk;
	after_round_sp_atk_func[skill_pa_jizhonggongji] 	= jizhonggongji_sprite_after_round_atk;



	before_round_sp_atk_func[skill_pa_pet_jizhonggongji]	= jizhonggongji_sprite_before_round_atk;
	after_round_sp_atk_func[skill_pa_pet_jizhonggongji] 	= jizhonggongji_sprite_after_round_atk;	
	
	before_round_sp_atk_func[skill_pa_huishang] 		= huishang_sprite_before_round_atk;
	after_round_sp_atk_func[skill_pa_huishang] 			= huishang_sprite_after_round_atk;
	before_round_sp_atk_func[skill_pa_ninshenjian] 		= ninshengyiji_sprite_before_round_atk;
	after_round_sp_atk_func[skill_pa_ninshenjian] 		= ninshengyiji_sprite_after_round_atk;

	before_round_sp_atk_func[skill_pa_pet_ninshenjian]		= ninshengyiji_sprite_before_round_atk;
	after_round_sp_atk_func[skill_pa_pet_ninshenjian]		= ninshengyiji_sprite_after_round_atk;

	after_round_sp_atk_func[skill_rh_pet_zhiyuhuahuan]		= zhiyuhuahuan_sprite_after_round_atk;


	resolve_sp_atk_func[skill_run_away] 				= run_away_resolve_sprite_atk;
	resolve_sp_atk_func[skill_user_break_off] 			= run_away_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pet_break_off] 			= run_away_resolve_sprite_atk;
	resolve_sp_atk_func[skill_chg_position] 			= chg_pos_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pet_chg_position] 		= chg_pos_resolve_sprite_atk;
	resolve_sp_atk_func[skill_catch_pet]	 		  	= catch_pet_resolve_sprite_atk;
	resolve_sp_atk_func[skill_recall_pet]				= recall_pet_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_dai_ji]				= dai_ji_resolve_sprite_atk;

	resolve_sp_atk_func[skill_pa_base_fight]			= base_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_heji]					= base_resolve_sprite_atk;
	
	resolve_sp_atk_func[skill_pd_fangyu] 		        = fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_md_mokang] 			    = fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pd_huandun] 			    = fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pd_huiji] 			    = fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_mifanquan] 			    = fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_hw_huwei] 				= fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_shenpanzhilun] 			= t_attacked_resolve_sprite_atk;

	
	resolve_sp_atk_func[skill_pa_xianfazhiren]			= xian_zhi_resolve_sprite_atk;
	// 
	resolve_sp_atk_func[skill_pa_zhuiji] 				= zhuiji_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_bengya] 				= bengya_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_chuanci] 				= chuanci_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_duochongjian] 			= duochongjian_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_kuangji] 				= kuangji_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_meiying] 				= meiying_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_huishang] 				= huishang_resolve_sprite_atk;

	resolve_sp_atk_func[skill_shunsha]					= shunsha_resolve_sprite_atk;
	resolve_sp_atk_func[skill_lunzhuanfengbao]			= skill_lunzhuanfengbao_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_pet_shunsha]				= shunsha_resolve_sprite_atk;
	// 
	resolve_sp_atk_func[skill_pa_jizhonggongji] 		= jizhonggongji_resolve_sprite_atk;	
	resolve_sp_atk_func[skill_pa_douqi] 				= douqi_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_shuangxingjian] 		= shuangxingjian_resolve_sprite_atk;
	
	resolve_sp_atk_func[skill_pa_ninshenjian] 			= ninshengyiji_resolve_sprite_atk;
	resolve_sp_atk_func[skill_hm_pohun] 				= pohun_resolve_sprite_atk;
	resolve_sp_atk_func[skill_jintouquan] 				= skill_jintouquan_resolve_sprite_atk;
	resolve_sp_atk_func[skill_qidouquan] 				= skill_qidouquan_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rh_mingsi] 				= mingsi_resolve_sprite_atk;
	resolve_sp_atk_func[skill_qiliaodun] 				= skill_qiliaodun_resolve_sprite_atk;
	
	resolve_sp_atk_func[skill_rh_zhiyuzhizhen] 			= zhiyuzhizhen_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rh_zhiyulingyu] 			= zhiyulingyu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rh_zhiyujiejie] 			= zhiyujiejie_resolve_sprite_atk;
	
	resolve_sp_atk_func[skill_rs_bingqingzhizhen] 		= bingqingzhizhen_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rs_bingqinglingyu] 		= bingqinglingyu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rs_bingqingjiejie] 		= bingqingjiejie_resolve_sprite_atk;

	resolve_sp_atk_func[skill_rs_pet_bingqingzhizhen] 		= bingqingzhizhen_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rs_pet_bingqinglingyu] 		= bingqinglingyu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rs_pet_bingqingjiejie] 		= bingqingjiejie_resolve_sprite_atk;
	
	resolve_sp_atk_func[skill_rl_jiushu] 				= jiushu_resolve_sprite_atk;	

	resolve_sp_atk_func[skill_shenglingzhaohuan] 		= t_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ma_jushizhou] 			= t_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ma_hanbingzhou] 			= t_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ma_liehuozhou] 			= t_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ma_kuangfengzhou]			= t_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ma_liuxingyu] 			= all_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ma_youyuanbing] 			= all_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ma_diyuhuo] 				= all_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ma_longjuanren] 			= all_attacked_resolve_sprite_atk;
	
	resolve_sp_atk_func[skill_dadiciqiang] 			= t_attacked_resolve_sprite_atk;

	resolve_sp_atk_func[skill_anyunliejie ] 			= all_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_mas_zaishengzhizhen] 		= zaishengzhizhen_resolve_sprite_atk;
	resolve_sp_atk_func[skill_mas_zaishenglingyu]		= zaishenglingyu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_mas_zaishengjiejie]		= zaishengjiejie_resolve_sprite_atk;
	resolve_sp_atk_func[skill_zaishengjiejie]		= zaishengjiejie_resolve_sprite_atk;

	resolve_sp_atk_func[skill_ms_shihuazhizhen] 	= zaishengzhizhen_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_yiwangzhizhen]		= zaishengzhizhen_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_hunluanzhizhen]	= zaishengzhizhen_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_yuanlingzhifu] 	= zaishengzhizhen_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_juduzhizhen]		= zaishengzhizhen_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_shuimianzhizhen]	= zaishengzhizhen_resolve_sprite_atk;

	resolve_sp_atk_func[skill_ms_pet_shihualingyu] 		= zaishenglingyu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_pet_yiwanglingyu]		= zaishenglingyu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_pet_hunluanlingyu]		= zaishenglingyu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_pet_yuanlinglingyu] 	= zaishenglingyu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_pet_judulingyu]		= zaishenglingyu_resolve_sprite_atk;

	resolve_sp_atk_func[skill_ms_pet_shihuajiejie] 		= zaishengjiejie_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_pet_yiwangjiejie]		= zaishengjiejie_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_pet_hunluanjiejie]		= zaishengjiejie_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_pet_yuanlingjiejie] 	= zaishengjiejie_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_pet_judujiejie]		= zaishengjiejie_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ms_pet_shuimianjiejie]	= zaishengjiejie_resolve_sprite_atk;

	resolve_sp_atk_func[skill_fh_chouqu]				= shengmingchouqu_resolve_sprite_atk;
	
	resolve_sp_atk_func[skill_sanyeshi]						= sanyeshi_resolve_sprite_atk;
}

void init_attck_proc_pet_func()
{
	before_round_sp_atk_func[skill_pa_pet_kuangji]			= kuangji_pet_before_round_atk;
	after_round_sp_atk_func[skill_pa_pet_kuangji]			= kuangji_pet_after_round_atk;
	
	before_round_sp_atk_func[skill_pa_pet_huishang]			= huishang_pet_before_round_atk;
	after_round_sp_atk_func[skill_pa_pet_huishang]			= huishang_pet_after_round_atk;

	before_round_sp_atk_func[skill_ph_pet_gaopinzhenji] 	= gaopinzhenji_sprite_before_round_atk;
	after_round_sp_atk_func[skill_ph_pet_gaopinzhenji] 		= gaopinzhenji_sprite_after_round_atk;

	resolve_sp_atk_func[skill_pa_pet_base_fight]			= base_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_pa_pet_heji]					= base_resolve_sprite_pet_atk;
	
	resolve_sp_atk_func[skill_pa_pet_kuangji]				= kuangji_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_pa_pet_meiying] 				= meiying_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_pa_pet_huishang] 				= huishang_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ph_pet_chanshentengman]		= chanshentengman_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ma_pet_zhimianfenzhen]		= shengmingchouqu_resolve_sprite_pet_atk;

	resolve_sp_atk_func[skill_pa_pet_xianfazhiren]			= xian_zhi_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_pet_chuanci] 				= chuanci_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_pet_jizhonggongji]			= jizhonggongji_resolve_sprite_atk;
	resolve_sp_atk_func[skill_callback_fuzhu]				= callback_fuzhu_resolve_atk;
	resolve_sp_atk_func[skill_pa_pet_douqi] 				= douqi_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_pet_shuangxingjian] 		= shuangxingjian_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pa_pet_ninshenjian] 			= ninshengyiji_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rl_pet_jiushu] 				= jiushu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_shenyuhuhuan] 				= shenyuhuhuan_resolve_sprite_atk;
	resolve_sp_atk_func[skill_shengyanshipian] 				= t_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rl_pet_jiushu_all_only_svr] 	= jiushu_all_resolve_sprite_atk;
	
	resolve_sp_atk_func[skill_pa_pet_zhuiji]				= zhuiji_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_pa_pet_bengya]				= bengya_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_hm_pet_pohun]					= pohun_resolve_sprite_pet_atk;
	
	resolve_sp_atk_func[skill_pa_pet_dai_ji]				= dai_ji_resolve_sprite_atk;

	resolve_sp_atk_func[skill_pd_pet_fangyu] 		        = fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_md_pet_mokang] 			    = fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pd_pet_huandun] 			    = fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_pd_pet_huiji] 			    = fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_hw_pet_huwei] 			    = fang_yu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rh_pet_mingsi] 			    = mingsi_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_rh_pet_zhiyuzhizhen]		    = zhiyuzhizhen_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_rh_pet_zhiyulingyu]		    = zhiyulingyu_resolve_sprite_atk;
	resolve_sp_atk_func[skill_shengtanglingyu]		    = 	t_attacked_resolve_sprite_atk;
	resolve_sp_atk_func[skill_rh_pet_zhiyujiejie]		    = zhiyujiejie_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_shenenjiejie]		    = 	t_attacked_resolve_sprite_atk ;
	resolve_sp_atk_func[skill_zhouyuanzhimen]		    = 	all_attacked_resolve_sprite_atk ;
	resolve_sp_atk_func[skill_rh_pet_zhiyuhuahuan]		    = zhiyuhuahuan_resolve_sprite_atk;

	resolve_sp_atk_func[skill_rh_pet_kuangyezhifun]		    = kuangyezhifun_resolve_sprite_atk;

	resolve_sp_atk_func[skill_ma_pet_jushizhou] 			= t_attacked_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ma_pet_hanbingzhou] 			= t_attacked_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ma_pet_liehuozhou] 			= t_attacked_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ma_pet_kuangfengzhou]			= t_attacked_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ma_pet_liuxingyu] 			= all_attacked_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ma_pet_youyuanbing] 			= all_attacked_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ma_pet_diyuhuo] 				= all_attacked_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ma_pet_longjuanren] 			= all_attacked_resolve_sprite_pet_atk;

	resolve_sp_atk_func[skill_mas_pet_zaishengzhizhen] 		= zaishengzhizhen_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_mas_pet_zaishenglingyu]		= zaishenglingyu_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_mas_pet_zaishengjiejie]		= zaishengjiejie_resolve_sprite_pet_atk;

	resolve_sp_atk_func[skill_ms_pet_shihuazhizhen] 	= zaishengzhizhen_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_yiwangzhizhen]		= zaishengzhizhen_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_hunluanzhizhen]	= zaishengzhizhen_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_yuanlingzhifu] 	= zaishengzhizhen_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_juduzhizhen]		= zaishengzhizhen_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_shuimianzhizhen]	= zaishengzhizhen_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ma_pet_wanshoushishen]	= base_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ph_pet_mengshouliexi]		= mengshouliexi_resolve_sprite_atk ;

	resolve_sp_atk_func[skill_ms_pet_shihualingyu] 		= zaishenglingyu_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_yiwanglingyu]		= zaishenglingyu_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_hunluanlingyu]		= zaishenglingyu_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_yuanlinglingyu] 	= zaishenglingyu_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_judulingyu]		= zaishenglingyu_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_shuimianlingyu]	= zaishenglingyu_resolve_sprite_pet_atk;

	resolve_sp_atk_func[skill_ms_pet_shihuajiejie] 		= zaishengjiejie_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_yiwangjiejie]		= zaishengjiejie_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_hunluanjiejie]		= zaishengjiejie_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_yuanlingjiejie] 	= zaishengjiejie_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_judujiejie]		= zaishengjiejie_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ms_pet_shuimianjiejie]	= zaishengjiejie_resolve_sprite_pet_atk;


	resolve_sp_atk_func[skill_ph_pet_xixuegongji]			= xixuegongji_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_fh_pet_chouqu]				= shengmingchouqu_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_pa_pet_duochongjian]			= duochongjian_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_pet_sanyeshi]						= sanyeshi_resolve_sprite_atk;
	resolve_sp_atk_func[skill_ph_pet_gaopinzhenji]			= gaopinzhenji_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_ma_pet_dubaoshu]			= dubaoshu_resolve_sprite_pet_atk;
	resolve_sp_atk_func[skill_rh_pet_ruodiantouxi]			= ruodiantouxi_resolve_sprite_pet_atk;
	//
}
