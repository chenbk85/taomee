#include "title_attribute.hpp"
//#include <libtaomee++/conf_parser/xmlparser.hpp>

title_attr_data_mgr* get_title_attr_data_mgr()
{
	static title_attr_data_mgr  obj;
	return &obj;
}


uint32_t calc_title_strength_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return pattr->strength;
}

uint32_t calc_title_agility_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return pattr->agility;
}

uint32_t calc_title_body_quality_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return pattr->body;
}

uint32_t calc_title_stamina_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return pattr->stamina;
}

uint32_t calc_title_recover_hp_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return pattr->addhp;
}

uint32_t calc_title_recover_mp_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return pattr->addmp;
}


uint32_t calc_title_def_value_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return pattr->def;
}

float calc_title_crit_attr(Player* p)
{
    //float crit_para = ( 4500.0 ) / pow(p->lv + 35, 2.5);

    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return (float)(pattr->crit);
}

float calc_title_hit_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return (float)(pattr->hit);
}

uint32_t calc_title_hp_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return pattr->hp;
}

uint32_t calc_title_mp_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return pattr->mp;
}

uint32_t calc_title_atk_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return pattr->atk;
}

float calc_title_dodge_attr(Player* p)
{
    title_attr_data * pattr = get_title_attr_data_mgr()->get_title_attr_data(p->achieve_title);
    if (pattr == NULL) {
        return 0;
    }
    return (float)(pattr->dodge);
}

