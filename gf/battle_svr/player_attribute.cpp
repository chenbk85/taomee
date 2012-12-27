#include "player_attribute.hpp"
#include "item.hpp"
#include "title_attribute.hpp"

static uint32_t calc_player_strength_by_clothes( Player* p);
static uint32_t calc_player_agility_by_clothes( Player* p);
static uint32_t calc_player_body_quality_by_clothes( Player* p);
static uint32_t calc_player_stamina_by_clothes( Player* p);
static uint32_t calc_player_hp_by_clothes( Player* p);
static uint32_t calc_player_mp_by_clothes( Player* p);
static float    calc_player_crit_rate_by_clothes( Player* p);
static float    calc_player_dodge_rate_by_clothes( Player* p);
static float    calc_player_hit_rate_by_clothes( Player* p);


////-----------------------------------static function----------------------------------///
float calc_player_hit_rate_by_clothes(Player* p)
{
	float add_hit_rate = 0.0;
	for(uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++)
	{
		const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothes->clothes_id);
		add_hit_rate += item->hit();
		if( items->is_atk_clothes(item->equip_part()) && clothes->lv >= 4 ){
			add_hit_rate += calc_weapon_add_hit(item->use_lv(), clothes->lv, item->quality_lv);
		}
	}
	add_hit_rate += p->suit_add_attr.hit;
	return add_hit_rate;
}


float    calc_player_dodge_rate_by_clothes(Player* p)
{
	float add_dodge_rate = 0.0;
	for(uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++)
	{
		const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothes->clothes_id);
		add_dodge_rate += item->dodge();
		if( items->is_def_clothes(item->equip_part()) && clothes->lv >= 1 ){
			add_dodge_rate += calc_clothes_add_dodge(item->use_lv(), clothes->lv, item->quality_lv);
		}
	}
	add_dodge_rate += p->suit_add_attr.dodge;
	return add_dodge_rate;
}

float    calc_player_crit_rate_by_clothes(Player* p)
{
	float add_crit_rate = 0.0;
	for(uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++)
	{
		const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothes->clothes_id);
		add_crit_rate += item->crit();
	}
	add_crit_rate += p->suit_add_attr.crit;
	return add_crit_rate;
}


float    calc_player_def_rate_by_clothes(Player* p )
{
	float  add_def_rate = 0.0;
	add_def_rate += p->def_value  /  static_cast<double>(p->lv + 50) * 0.09;
	add_def_rate = 1 - add_def_rate;

	return add_def_rate;
}



uint32_t calc_player_def_value_by_clothes( Player* p)
{
	uint32_t add_def_value = 0;
	for(uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++)
	{
		const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothes->clothes_id);
		add_def_value +=  item->def - item->def * clothes->duration_state/100;
		if( items->is_def_clothes(item->equip_part()) && clothes->lv >= 1){
			add_def_value += calc_clothes_add_defense(clothes->lv)* clothes->duration_state/100;
		}
	}
	add_def_value += p->suit_add_attr.def;
	return add_def_value;
}


uint32_t calc_player_atk_by_clothes(Player* p)
{
	uint32_t add_atk = 0;
	uint32_t low_atk = 0;
	uint32_t high_atk = 0;

	p->weapon_atk[0] = 0;
	p->weapon_atk[1] = 0;
	

	for(uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++)
	{
		const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothes->clothes_id);
		if( items->is_atk_clothes(item->equip_part()) ){
			if( clothes->lv >= 1){
				add_atk += calc_weapon_add_atk(item->use_lv(), clothes->lv, item->quality_lv);
			}
			low_atk = item->atk[0] + add_atk;
			high_atk = item->atk[1] + add_atk;
			p->weapon_atk[0] = low_atk - low_atk * clothes->duration_state / 100;
			p->weapon_atk[1] = high_atk - high_atk * clothes->duration_state / 100;
		}
	}
	return  0;
}


uint32_t calc_player_mp_by_clothes( Player* p)
{
	uint32_t add_mp = 0;

	for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i)
	{
		const player_clothes_info_t* clothe = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothe->clothes_id);
		add_mp += item->mp();
	}
	add_mp += p->suit_add_attr.mp;	
	return add_mp;
}

uint32_t calc_player_hp_by_clothes( Player* p)
{
	uint32_t add_hp = 0;

	for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i)
	{
		const player_clothes_info_t* clothe = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothe->clothes_id);
		add_hp += item->hp();
	}
	add_hp += p->suit_add_attr.hp;	
	return add_hp;
}

uint32_t  calc_player_recover_hp_attr(Player* p)
{
	p->clothes_addhp = 0;
	for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i)
	{
		const player_clothes_info_t* clothe = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothe->clothes_id);
		p->clothes_addhp += item->add_hp();
        //这里漏了强化的加的回血
	}	
	p->clothes_addhp += p->suit_add_attr.add_hp;
	p->auto_add_hpmp.hp_chg = 5 + p->body_quality() * 0.225 + p->clothes_addhp + calc_title_recover_hp_attr(p);
	return p->auto_add_hpmp.hp_chg;
}

uint32_t  calc_player_recover_mp_attr(Player* p)
{
	p->clothes_addmp = 0;
	for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i)
	{
		const player_clothes_info_t* clothe = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothe->clothes_id);
		p->clothes_addmp += item->add_mp();
	}
	p->clothes_addmp += p->suit_add_attr.add_mp;
	p->auto_add_hpmp.mp_chg = 2 + p->stamina * 0.125 + p->clothes_addmp + calc_title_recover_mp_attr(p);
	return p->auto_add_hpmp.mp_chg;
}

uint32_t calc_player_stamina_by_clothes(Player* p)
{
	uint32_t add_stamina = 0;
	for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i)
	{
		const player_clothes_info_t* clothe = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothe->clothes_id);
		add_stamina += item->stamina;
	}
	add_stamina += p->suit_add_attr.stamina;
	return add_stamina;
}



uint32_t calc_player_body_quality_by_clothes( Player* p)
{
	uint32_t add_body_quality = 0;
	for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i)
	{
		const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothes->clothes_id);
		add_body_quality += item->body_quality;
		 if( items->is_def_clothes(item->equip_part()) && clothes->lv >= 1) {
			 add_body_quality += calc_clothes_add_body(item->use_lv(), clothes->lv, item->quality_lv);
		 }
	}
	add_body_quality += p->suit_add_attr.body_quality;
	return add_body_quality;
}


uint32_t calc_player_strength_by_clothes( Player* p)
{
	uint32_t add_strength = 0;
	for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i)
	{
		const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothes->clothes_id);
		//装备自带的属性
		add_strength += item->strength;
		//装备强化属性
		if( items->is_atk_clothes(item->equip_part()) && clothes->lv >= 4) {
			add_strength += calc_weapon_add_strength(item->use_lv(), clothes->lv, item->quality_lv);
		}
	}
	add_strength += p->suit_add_attr.strength;
	return add_strength;
}

uint32_t calc_player_agility_by_clothes( Player* p)
{
	uint32_t add_agility = 0;
	for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i)
	{
		const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothes->clothes_id);
		add_agility += item->agility;
		if( items->is_atk_clothes(item->equip_part()) && clothes->lv >= 4) {
			add_agility += calc_weapon_add_strength(item->use_lv(), clothes->lv, item->quality_lv);
		}
	}
	add_agility += p->suit_add_attr.agility;
	return add_agility;
}

///---------------------------------extern function---------------------------------------///
uint32_t calc_player_attr_by_suit(Player* p)
{
    addition_attr_t skill_attr = {0};
	weared_suit_t weared_suit_arr[max_suit_step];
	memset( weared_suit_arr, 0, sizeof(weared_suit_arr));
	memset(&(p->suit_add_attr), 0, sizeof(p->suit_add_attr));
	
	for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i)
	{
		const player_clothes_info_t* clothe = &(p->clothes_info.clothes[i]);
		const GfItem* item = items->get_item(clothe->clothes_id);
		uint32_t suit_id =  item->suit_id();
		if( !suit_id ){
			continue;
		}

		for(uint32_t j =0; j < max_suit_step; j++)
		{
			if(! (weared_suit_arr[j].suit_id)){
				weared_suit_arr[j].suit_id = suit_id;
				weared_suit_arr[j].suit_cnt = 1;
				break;
			}

			if (weared_suit_arr[j].suit_id == suit_id){
				weared_suit_arr[j].suit_cnt ++;
				break;
			}
		}
	}

	p->reset_skill_addition_attr();
	for( uint32_t j = 0; j < max_suit_step; j++)
	{
		uint32_t suit_id = weared_suit_arr[j].suit_id;
		uint32_t suit_cnt = weared_suit_arr[j].suit_cnt;
		if( !suit_id ){
			break;
		}	
		for( uint32_t n = suit_cnt; n> 0; n--)
		{
            memset(&skill_attr, 0x00, sizeof(addition_attr_t));
			if (suit_arr[suit_id].suit_step[n].attire_num)
			{
				suit_step_t* p_step = &(suit_arr[suit_id].suit_step[n]);
				p->suit_add_attr.hp      += p_step->hp;
				p->suit_add_attr.mp          += p_step->mp;
				p->suit_add_attr.atk         += p_step->atk;
				p->suit_add_attr.crit        += p_step->crit / 10;
				p->suit_add_attr.hit         += p_step->hit / 2000.0;
				p->suit_add_attr.dodge       += p_step->dodge / 1000.0;
				p->suit_add_attr.strength    += p_step->strength;
				p->suit_add_attr.agility     += p_step->agility;
				p->suit_add_attr.body_quality += p_step->body_quality;
				p->suit_add_attr.stamina     += p_step->stamina;
				p->suit_add_attr.add_hp      += p_step->add_hp;
				p->suit_add_attr.add_mp      += p_step->add_mp;
				p->suit_add_attr.skill_atk   += p_step->skill_atk;
				p->suit_add_attr.def         += p_step->def;
                
                //skill addition attr
                if (p_step->addition.skill_id != 0) {
                    skill_attr.cut_cd    += p_step->addition.cut_cd;
                    skill_attr.cut_mp    += p_step->addition.cut_mp;
                    skill_attr.attr_dmg  += p_step->addition.attr_dmg;
                    skill_attr.extra_dmg += p_step->addition.extra_dmg;
                    p->set_skill_addition_attr(p_step->addition.skill_id, &skill_attr);
                    TRACE_LOG("addition [%u] [%u %u %u %u]",p_step->addition.skill_id, skill_attr.cut_cd,skill_attr.cut_mp, skill_attr.attr_dmg, skill_attr.extra_dmg);
                }
			}
		}

	}
	return 0;
}

uint32_t  calc_player_stamina_attr(Player* p)
{
	uint32_t  total_stamina = 0;
	total_stamina += calc_stamina[p->role_type](p->lv);
	total_stamina += calc_player_stamina_by_clothes(p);
	total_stamina += calc_title_stamina_attr(p);
	return total_stamina;
}


uint32_t  calc_player_strength_attr(Player* p)
{
 	uint32_t total_strength = 0;
	total_strength += calc_strength[p->role_type](p->lv);
	total_strength += calc_player_strength_by_clothes(p);
	total_strength += calc_title_strength_attr(p);
	return total_strength;
}

uint32_t  calc_player_agility_attr(Player* p)
{
	uint32_t total_agility = 0;
	total_agility += calc_agility[p->role_type](p->lv);
	total_agility += calc_player_agility_by_clothes(p);
	total_agility += calc_title_agility_attr(p);
	return total_agility;
}

uint32_t  calc_player_body_quality_attr(Player* p)
{
	uint32_t total_body_quality = 0;
	total_body_quality += calc_body_quality[p->role_type](p->lv);
	total_body_quality += calc_player_body_quality_by_clothes(p);
	total_body_quality += calc_title_body_quality_attr(p);
	return total_body_quality;
}

uint32_t  calc_player_hp_attr(Player* p)
{
	uint32_t total_max_hp = 0;
	total_max_hp += calc_hp[p->role_type](p->body_quality());
	total_max_hp += calc_player_hp_by_clothes(p);
	total_max_hp += calc_title_hp_attr(p);
	return total_max_hp;	
}


uint32_t  calc_player_mp_attr(Player* p)
{
	uint32_t total_max_mp = 0;
	total_max_mp += calc_mp[p->role_type](p->stamina);
	total_max_mp += calc_player_mp_by_clothes(p);
	total_max_mp += calc_title_mp_attr(p);
	return total_max_mp;
}


uint32_t  calc_player_atk_attr(Player* p)
{
	uint32_t total_atk  = 0;
	total_atk += calc_atk(p->strength);
	total_atk += calc_player_atk_by_clothes(p);	
	total_atk += calc_title_atk_attr(p);	
	return total_atk;
}

uint32_t  calc_player_def_value_attr(Player* p)
{
	uint32_t total_def_value = 0;
	total_def_value += calc_def_value(p->lv);
	total_def_value += calc_player_def_value_by_clothes(p);
	total_def_value += calc_title_def_value_attr(p);
	return total_def_value;
}

float     calc_player_def_rate_attr(Player* p)
{
	float total_def_rate = 0.0;
	total_def_rate += calc_def_rate(p->lv);
	total_def_rate += calc_player_def_rate_by_clothes(p);
	return total_def_rate;
}


float     calc_player_crit_attr(Player* p)
{
	float crit_para = ( 4500.0 ) / pow(p->lv + 35, 2.5);
	float total_crit = 0.0;
	
	total_crit  = p->agility() * 2 / 3 * crit_para;
	if (is_valid_uid(p->id)) {
		total_crit += crit_adjs[p->role_type];					    
	}	
	total_crit += calc_player_crit_rate_by_clothes(p) * crit_para;
	total_crit += calc_title_crit_attr(p) * crit_para;
	return total_crit;
}

float     calc_player_dogde_attr(Player* p)
{
	float dodge_para = ( 4500.0 ) / pow(p->lv + 35, 2.5) / 100;
	float total_dodge_rate = 0.0;
	total_dodge_rate += p->agility() / 3.0 * dodge_para;
	total_dodge_rate += calc_player_dodge_rate_by_clothes(p) * dodge_para;
	total_dodge_rate += calc_title_dodge_attr(p) * dodge_para;
	return 1 - total_dodge_rate;
}

float     calc_player_hit_attr(Player* p)
{
	float hit_para = ( 4500.0 ) / pow(p->lv + 35, 2.5) / 100;
	float total_hit_rate = 0.0;
	total_hit_rate  += hit_rates[p->role_type];	
	total_hit_rate  += calc_player_hit_rate_by_clothes(p) * hit_para;
	total_hit_rate  += calc_title_hit_attr(p) * hit_para;
	return total_hit_rate;
}

//-------------------NEW calculate split line --------------------------------------//
uint32_t get_base_hp(Player *p)
{
    uint32_t body = calc_body_quality[p->role_type](p->lv);
    return calc_hp[p->role_type](body);
}

uint32_t get_base_atk(Player *p)
{
    uint32_t strength = calc_strength[p->role_type](p->lv);
    return calc_atk(strength);
}

void calc_player_base_attr(Player *p, base_attr_t *attr)
{
    attr->agility   =   calc_agility[p->role_type](p->lv);
    attr->strength  =   calc_strength[p->role_type](p->lv);
    attr->body      =   calc_body_quality[p->role_type](p->lv);
    attr->stamina   =   calc_stamina[p->role_type](p->lv);
}

void calc_player_attire_base_attr(Player *p, base_attr_t *attr)
{
    for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i) {
        const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
        const GfItem* itm = items->get_item(clothes->clothes_id);
        if (itm == NULL) {
            WARN_LOG("clothes id error[uid=%u clothesid=%u]",p->id,clothes->clothes_id);
            continue;
        }
        attr->agility   +=   itm->agility;
        attr->strength  +=   itm->strength;
        attr->body      +=   itm->body_quality;
        attr->stamina   +=   itm->stamina;
        //follow strength level
        if ( items->is_atk_clothes(itm->equip_part()) && clothes->lv >= 4 ) {
            attr->agility   +=  calc_weapon_add_agility(itm->use_lv(), clothes->lv, itm->quality_lv);
            attr->strength  +=  calc_weapon_add_strength(itm->use_lv(), clothes->lv, itm->quality_lv);
        }
        if ( items->is_def_clothes(itm->equip_part()) && clothes->lv >= 1 ) {
            attr->body      +=  calc_clothes_add_body(itm->use_lv(), clothes->lv, itm->quality_lv);
        }
    }
}

void calc_player_attire_btl_attr(Player *p, btl_attr_t *attr)
{
    uint32_t low_atk = 0, high_atk = 0, add_atk = 0;
    uint32_t def = 0;
    for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i) {
        const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
        const GfItem* itm = items->get_item(clothes->clothes_id);
        if (itm == NULL) {
            WARN_LOG("clothes id error[uid=%u clothesid=%u]",p->id, clothes->clothes_id);
            continue;
        }
        uint8_t duration_state = get_cloth_duration_state(clothes->duration, itm->duration * clothes_duration_ratio);
        def = itm->def - itm->def * duration_state / 100;

        attr->maxhp   +=   itm->hp();
        attr->maxmp   +=   itm->mp();
        attr->addhp   +=   itm->add_hp();
        attr->addmp   +=   itm->add_mp();
        //attr->atk       +=  itm->atk[0]; //weapon atk
        attr->def       +=  def;
        attr->hit       +=  itm->hit();
        attr->dodge     +=  itm->dodge();
        attr->crit      +=  itm->crit();
        //follow strength level
        if ( items->is_atk_clothes(itm->equip_part()) ) {
			if (clothes->lv >= 4) {
				add_atk     +=  calc_weapon_add_atk(itm->use_lv(), clothes->lv, itm->quality_lv); //weapon atk
				attr->hit   +=  calc_weapon_add_hit(itm->use_lv(), clothes->lv, itm->quality_lv);
			}
			//save weapen atk
			low_atk     = itm->atk[0] + add_atk;
			high_atk    = itm->atk[1] + add_atk;
			p->weapon_atk[0] = low_atk - low_atk * clothes->duration_state / 100;
			p->weapon_atk[1] = high_atk - high_atk * clothes->duration_state / 100;
        }
        if ( items->is_def_clothes(itm->equip_part()) && clothes->lv >= 1 ) {
            attr->addhp     +=  calc_clothes_add_hp(itm->use_lv(), clothes->lv, itm->quality_lv);
            attr->def       +=  calc_clothes_add_defense(clothes->lv);
            attr->dodge     +=  calc_clothes_add_dodge(itm->use_lv(), clothes->lv, itm->quality_lv);
        }
    }
}

/**
 * @input array
 */
void get_player_suit_info(Player *p, weared_suit_t *suit_arr)
{
    for (uint32_t i = 0; i != p->clothes_info.clothes_cnt; ++i) {
        const GfItem* itm = items->get_item(p->clothes_info.clothes[i].clothes_id);
        if (itm == NULL) {
            WARN_LOG("clothes id error[uid=%u clothesid=%u]",p->id, p->clothes_info.clothes[i].clothes_id);
            continue;
        }
        uint32_t suit_id = itm->suit_id(); 
        if (suit_id) {
            for (uint32_t j = 0; j < max_suit_step; ++j) {
                if (suit_arr[j].suit_id == suit_id) {
                    suit_arr[j].suit_cnt++;
                    break;
                }
                if (suit_arr[j].suit_id == 0) {
                    suit_arr[j].suit_id = suit_id;
                    suit_arr[j].suit_cnt = 1;
                    break;
                }
            }
        }
    }
}

void calc_player_suit_base_attr(Player *p,  weared_suit_t *p_suit_arr, base_attr_t *attr)
{
    uint32_t suit_id = 0;
    for (uint32_t i = 0; i < max_suit_step; i++) {
        suit_id = p_suit_arr[i].suit_id;
        if (suit_id == 0) break;
        for (uint32_t n = p_suit_arr[i].suit_cnt; n > 0; --n) {
            if (suit_arr[suit_id].suit_step[n].attire_num) {
                suit_step_t* p_step = &(suit_arr[suit_id].suit_step[n]);
                attr->agility   +=   p_step->agility;
                attr->strength  +=   p_step->strength;
                attr->body      +=   p_step->body_quality;
                attr->stamina   +=   p_step->stamina;

            }
        }
    }
}

void calc_player_suit_btl_attr(Player *p, weared_suit_t *p_suit_arr, btl_attr_t *attr)
{
    uint32_t suit_id = 0;
    for (uint32_t i = 0; i < max_suit_step; i++) {
        suit_id = p_suit_arr[i].suit_id;
        if (suit_id == 0) break;
        for (uint32_t n = p_suit_arr[i].suit_cnt; n > 0; --n) {
            if (suit_arr[suit_id].suit_step[n].attire_num) {
                suit_step_t* p_step = &(suit_arr[suit_id].suit_step[n]);
                attr->maxhp   +=   p_step->hp;
                attr->maxmp   +=   p_step->mp;
                attr->addhp   +=   p_step->add_hp;
                attr->addmp   +=   p_step->add_mp;
                attr->atk       +=  p_step->atk;
                attr->def       +=  p_step->def;
                attr->hit       +=  p_step->hit;
                attr->dodge     +=  p_step->dodge;
                attr->crit      +=  p_step->crit;
            }
        }
    }
}

void calc_player_suit_skill_attr(Player *p,  weared_suit_t *p_suit_arr)
{
    addition_attr_t skill_attr = {0};
    uint32_t suit_id = 0;
	p->reset_skill_addition_attr();
    for (uint32_t i = 0; i < max_suit_step; i++) {
        suit_id = p_suit_arr[i].suit_id;
        if (suit_id == 0) break;
        for (uint32_t n = p_suit_arr[i].suit_cnt; n > 0; --n) {
            memset(&skill_attr, 0x00, sizeof(addition_attr_t));
            if (suit_arr[suit_id].suit_step[n].attire_num) {
                suit_step_t* p_step = &(suit_arr[suit_id].suit_step[n]);
                //skill addition attr
                if (p_step->addition.skill_id != 0) {
                    skill_attr.cut_cd    += p_step->addition.cut_cd;
                    skill_attr.cut_mp    += p_step->addition.cut_mp;
                    skill_attr.attr_dmg  += p_step->addition.attr_dmg;
                    skill_attr.extra_dmg += p_step->addition.extra_dmg;
                    p->set_skill_addition_attr(p_step->addition.skill_id, &skill_attr);
                    TRACE_LOG("addition [%u] [%u %u %u %u]",p_step->addition.skill_id, skill_attr.cut_cd,skill_attr.cut_mp, skill_attr.attr_dmg, skill_attr.extra_dmg);
                }

            }
        }
    }
}

void calc_player_strengthen_attire_base_attr(Player *p, base_attr_t *attr)
{
    for (uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++) {
        const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
        const GfItem* itm = items->get_item(clothes->clothes_id);
        if (!itm) continue;

        if ( items->is_atk_clothes(itm->equip_part()) && clothes->lv >= 4 ) {
            attr->agility   +=  calc_weapon_add_agility(itm->use_lv(), clothes->lv, itm->quality_lv);
            attr->strength  +=  calc_weapon_add_strength(itm->use_lv(), clothes->lv, itm->quality_lv);
        }
        if ( items->is_def_clothes(itm->equip_part()) && clothes->lv >= 1 ) {
            attr->body      +=  calc_clothes_add_body(itm->use_lv(), clothes->lv, itm->quality_lv);
        }
    }
}

void calc_player_strengthen_attire_btl_attr(Player *p, btl_attr_t *attr)
{
    for (uint32_t i = 0; i < p->clothes_info.clothes_cnt; i++) {
        const player_clothes_info_t* clothes = &(p->clothes_info.clothes[i]);
        const GfItem* itm = items->get_item(clothes->clothes_id);
        if (!itm) continue;

        if ( items->is_atk_clothes(itm->equip_part()) && clothes->lv >= 4 ) {
            attr->atk       +=  calc_weapon_add_atk(itm->use_lv(), clothes->lv, itm->quality_lv);
            attr->hit       +=  calc_weapon_add_hit(itm->use_lv(), clothes->lv, itm->quality_lv);
        }
        if ( items->is_def_clothes(itm->equip_part()) && clothes->lv >= 1 ) {
            attr->addhp     +=  calc_clothes_add_hp(itm->use_lv(), clothes->lv, itm->quality_lv);
            attr->def       +=  calc_clothes_add_defense(clothes->lv);
            attr->dodge     +=  calc_clothes_add_dodge(itm->use_lv(), clothes->lv, itm->quality_lv);
        }
    }
}

void calc_player_title_base_attr(Player *p, base_attr_t *attr)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) return;

    attr->agility   +=   pattr->agility;
    attr->strength  +=   pattr->strength;
    attr->body      +=   pattr->body;
    attr->stamina   +=   pattr->stamina;
}

void calc_player_title_btl_attr(Player *p, btl_attr_t *attr)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) return;
    attr->maxhp   +=   pattr->hp;
    attr->maxmp   +=   pattr->mp;
    attr->addhp   +=   pattr->addhp;
    attr->addmp   +=   pattr->addmp;
    attr->atk       +=  pattr->atk;
    attr->def       +=  pattr->def;
    attr->hit       +=  pattr->hit;
    attr->dodge     +=  pattr->dodge;
    attr->crit      +=  pattr->crit;
}

void calc_player_god_guard_btl_attr(Player *p, btl_attr_t *attr)
{
    quality_t *pattr = &(p->add_quality);
    if (pattr == NULL) return;
    attr->maxhp   +=   pattr->hp;
    attr->maxmp   +=   pattr->mp;
    attr->addhp   +=   pattr->addhp;
    attr->addmp   +=   pattr->addmp;
    attr->atk       +=  pattr->atk;
    attr->def       +=  pattr->def;
    attr->hit       +=  pattr->hit;
    attr->dodge     +=  pattr->dodge;
    attr->crit      +=  pattr->crit;
}

void calc_player_attr_ex2(Player *p)
{
    base_attr_t base_attr = {0};
    btl_attr_t btl_attr = {0};
    memset(&base_attr, 0x00, sizeof(base_attr_t));
    memset(&btl_attr, 0x00, sizeof(btl_attr_t));

    weared_suit_t weared_suit_arr[max_suit_step];
    memset(weared_suit_arr, 0x0, sizeof(weared_suit_t) * max_suit_step);
    get_player_suit_info(p, weared_suit_arr);

    //--------------------------------------------//
    calc_player_suit_skill_attr(p, weared_suit_arr);

    //-------------基础属性---------------------//
    calc_player_base_attr(p, &base_attr);
    calc_player_attire_base_attr(p, &base_attr);
    calc_player_suit_base_attr(p, weared_suit_arr, &base_attr);
    calc_player_title_base_attr(p, &base_attr);

    p->agility_  =   base_attr.agility;
    p->strength =   base_attr.strength;
    p->body_quality_ =   base_attr.body;
    p->stamina  =   base_attr.stamina;

    //--------------战斗属性----------------------//
    calc_player_attire_btl_attr(p, &btl_attr);
    calc_player_suit_btl_attr(p, weared_suit_arr, &btl_attr);
    calc_player_title_btl_attr(p, &btl_attr);
    calc_player_god_guard_btl_attr(p, &btl_attr);

    btl_attr.maxhp  +=  calc_hp[p->role_type](base_attr.body);
    btl_attr.maxmp  +=  calc_mp[p->role_type](base_attr.stamina);
    btl_attr.addhp  +=  5 + base_attr.body * 0.225;
    btl_attr.addmp  +=  2 + base_attr.stamina * 0.125;
    btl_attr.atk    +=  calc_atk(base_attr.strength);
    btl_attr.def    +=  calc_def_value(p->lv);
    //btl_attr.hit    /=  2000.0;
    //btl_attr.dodge  /=  1000.0;
    //btl_attr.crit   /=  10.0;

    float def_rate = 0.0, hit_rate = 0.0, dodge_rate = 0.0, crit_rate = 0.0;
    float factor_para = ( 4500.0 ) / pow(p->lv + 35, 2.5) / 100; 
    def_rate = calc_def_rate(p->lv) + ( 1 -  btl_attr.def / (double)(p->lv + 50) * 0.09);
    hit_rate = hit_rates[p->role_type] + (btl_attr.hit) * factor_para;
    dodge_rate = 1 - (p->agility()/ 3.0 + btl_attr.dodge) * factor_para;
    crit_rate = crit_adjs[p->role_type] + (p->agility() * 2.0 / 3.0 + btl_attr.crit) * (factor_para * 100);

    p->maxhp    =   btl_attr.maxhp;
    p->maxmp    =   btl_attr.maxmp;
    p->auto_add_hpmp.hp_chg =   btl_attr.addhp;
    p->auto_add_hpmp.mp_chg =   btl_attr.addmp;
    p->atk      =   btl_attr.atk;
    p->def_value    =   btl_attr.def;
    p->def_rate     =   def_rate;
    p->hit_value    =   btl_attr.hit;
    p->hit_rate     =   hit_rate;
    p->dodge_value  =   btl_attr.dodge;
    p->dodge_rate   =   dodge_rate;
    p->crit_value   =   btl_attr.crit;
    p->crit_rate    =   crit_rate;

    if (p->power_user) {
        p->maxhp    += get_base_hp(p) * 0.3;
        p->atk      += get_base_atk(p) * 0.3;
    }
}


uint32_t calc_player_base_mp_ex(Player * p)
{
	return ::calc_player_base_mp(p->lv, p->role_type);
}


