/******************************************************
 *
 * @auth saga
 *
 *******************************************************/

#include <kf/player_attr.hpp>
#include <kf/fight_team.hpp>
#include "fwd_decl.hpp"
#include "global_data.hpp"
#include "item.hpp"
#include "fight_team.hpp"

#include "player_attribute.hpp"


uint32_t get_base_hp(player_t *p)
{
    uint32_t body = calc_body_quality[p->role_type](p->lv);
    return calc_hp[p->role_type](body);
}

uint32_t get_base_atk(player_t *p)
{
    uint32_t strength = calc_strength[p->role_type](p->lv);
    return calc_atk(strength);
}

void calc_player_base_attr(player_t *p, base_attr_t *attr)
{
    attr->agility   =   calc_agility[p->role_type](p->lv);
    attr->strength  =   calc_strength[p->role_type](p->lv);
    attr->body      =   calc_body_quality[p->role_type](p->lv);
    attr->stamina   =   calc_stamina[p->role_type](p->lv);
}

void calc_player_attire_base_attr(player_t *p, base_attr_t *attr)
{
    for (uint32_t i = 0; i != p->clothes_num; ++i) {
        const player_clothes_info_t* clothes = &(p->clothes[i]);
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

void calc_player_attire_btl_attr(player_t *p, btl_attr_t *attr)
{
    uint32_t def = 0;
    for (uint32_t i = 0; i != p->clothes_num; ++i) {
        const player_clothes_info_t* clothes = &(p->clothes[i]);
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
        attr->atk       +=  itm->atk[1]; //weapon high atk
        attr->def       +=  def;
        attr->hit       +=  itm->hit();
        attr->dodge     +=  itm->dodge();
        attr->crit      +=  itm->crit();
        //follow strength level
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

struct weared_suit_t {
    uint32_t    suit_id;
    uint32_t    suit_cnt;
};
/**
 * @input array
 */
void get_player_suit_info(player_t *p, weared_suit_t *suit_arr)
{
    for (uint32_t i = 0; i != p->clothes_num; ++i) {
        const GfItem* itm = items->get_item(p->clothes[i].clothes_id);
        if (itm == NULL) {
            WARN_LOG("clothes id error[uid=%u clothesid=%u]",p->id, p->clothes[i].clothes_id);
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

void calc_player_suit_base_attr(player_t *p,  weared_suit_t *p_suit_arr, base_attr_t *attr)
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

void calc_player_suit_btl_attr(player_t *p, weared_suit_t *p_suit_arr, btl_attr_t *attr)
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

void calc_player_strengthen_attire_base_attr(player_t *p, base_attr_t *attr)
{
    for (uint32_t i = 0; i < p->clothes_num; i++) {
        const player_clothes_info_t* clothes = &(p->clothes[i]);
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

void calc_player_strengthen_attire_btl_attr(player_t *p, btl_attr_t *attr)
{
    for (uint32_t i = 0; i < p->clothes_num; i++) {
        const player_clothes_info_t* clothes = &(p->clothes[i]);
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

void calc_player_title_base_attr(player_t *p, base_attr_t *attr)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->using_achieve_title);
    if (pattr == NULL) return;

    attr->agility   +=   pattr->agility;
    attr->strength  +=   pattr->strength;
    attr->body      +=   pattr->body;
    attr->stamina   +=   pattr->stamina;
}

void calc_player_title_btl_attr(player_t *p, btl_attr_t *attr)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->using_achieve_title);
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

void calc_player_god_guard_btl_attr(player_t *p, btl_attr_t *attr)
{
    quality_t *pattr = p->add_quality;
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

void calc_player_attr_ex(player_t *p)
{
    base_attr_t base_attr = {0};
    btl_attr_t btl_attr = {0};
    memset(&base_attr, 0x00, sizeof(base_attr_t));
    memset(&btl_attr, 0x00, sizeof(btl_attr_t));

    weared_suit_t weared_suit_arr[max_suit_step];
    memset(weared_suit_arr, 0x0, sizeof(weared_suit_t) * max_suit_step);
    get_player_suit_info(p, weared_suit_arr);

    //-------------基础属性---------------------//
	//更具等级计算基础的体力值
    calc_player_base_attr(p, &base_attr);
	//计算装备的体力值,包括强化
    calc_player_attire_base_attr(p, &base_attr);
	//计算套装奖励的体力值
    calc_player_suit_base_attr(p, weared_suit_arr, &base_attr);
	//计算称号奖励的体力值
    calc_player_title_base_attr(p, &base_attr);

    p->agility  =   base_attr.agility;
    p->strength =   base_attr.strength;
    p->body_quality =   base_attr.body;
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

    //---------------战队属性---------------------//
    if (is_player_have_team(p)) {
        fight_team_attr_add_t team_attr;
        calc_fight_team_attr_add(p, &team_attr);
        btl_attr.atk    += team_attr.atk;
        btl_attr.def    += team_attr.def_value;
        btl_attr.maxhp  += team_attr.hp;
        btl_attr.maxmp  += team_attr.mp;
    }

    btl_attr.crit = crit_adjs[p->role_type] + (base_attr.agility * 2.0 / 3.0 + btl_attr.crit);
    btl_attr.hit = hit_rates[p->role_type] * 100 + (btl_attr.hit);
    btl_attr.dodge = base_attr.agility / 3.0 + btl_attr.dodge;
    //float def_rate = 0.0, hit_rate = 0.0, dodge_rate = 0.0, crit_rate = 0.0;
    //float factor_para = ( 4500.0 ) / pow(p->lv + 35, 2.5) / 100; 
    //def_rate = calc_def_rate(p->lv) + (1 - btl_attr.def / (double)(p->lv + 50) * 0.09);
    //hit_rate = hit_rates[p->role_type] + (btl_attr.hit) * factor_para;
    //dodge_rate = 1 - (base_attr.agility / 3.0 + btl_attr.dodge) * factor_para;
    //crit_rate = crit_adjs[p->role_type] + (base_attr.agility * 2.0 / 3.0 + btl_attr.crit) * factor_para * 100;

    p->maxhp    =   btl_attr.maxhp;
    p->maxmp    =   btl_attr.maxmp;
	p->addhp	= 	btl_attr.addhp;
	p->addmp	= 	btl_attr.addmp;
    p->atk      =   btl_attr.atk;
    p->crit_rate    =   btl_attr.crit;
    p->def_rate     =   btl_attr.def;
    p->hit_rate     =   btl_attr.hit;
    p->dodge_rate   =   btl_attr.dodge;

    if (p->power_user) {
        p->maxhp    += get_base_hp(p) * 0.3;
        p->atk      += get_base_atk(p) * 0.3;
    }

    p->hp = p->maxhp;
    p->mp = p->maxmp;

    TRACE_LOG("hpmp: [%u] [%u %u]", p->id, p->hp, p->mp);
}
