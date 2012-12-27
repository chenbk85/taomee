#include "god_guard.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "summon_monster.hpp"
#include "player.hpp"
#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "global_data.hpp"
#include "numen.hpp"

using namespace taomee;

//#define MAX(a,b) ((a)>(b))?(a):(b)
//#define MIN(a,b) ((a)<(b))?(a):(b)

bool god_guard_mgr::is_god_exist(uint32_t id)
{
	std::map<uint32_t, god_factor*>::iterator pItr = god_datas.find(id);
	if(pItr != god_datas.end())return true;
	return false;
}

bool god_guard_mgr::add_god_data(god_factor* p_god)
{
	if( is_god_exist(p_god->id))return false;
	god_datas[ p_god->id ] = p_god;
	return true;
}

bool god_guard_mgr::is_summon_exist(uint32_t id)
{
	std::map<uint32_t, summon_factor*>::iterator pItr = summon_datas.find(id);
	if(pItr != summon_datas.end())return true;
	return false;
}

bool god_guard_mgr::add_summon_data(summon_factor* p_sum)
{
	if( is_summon_exist(p_sum->id))return false;
	summon_datas[ p_sum->id ] = p_sum;
	return true;
}

uint32_t god_guard_mgr::summon_arithmetic(uint32_t base_value, uint32_t grow_rate, uint32_t lv)
{
    return ((float)base_value / 100.0 * (float)grow_rate / 100.0 * (float)lv);
}

uint32_t god_guard_mgr::group_arithmetic(uint32_t base_value, uint32_t add_value, uint32_t grow_rate, uint32_t base_lv, uint32_t add_lv)
{
    float llv = MIN(base_lv, add_lv);
    float hlv = MAX(base_lv, add_lv);
    return ((float)base_value / 100.0 * (float)grow_rate / 100.0 * (float)base_lv * (float)add_value / 100.0 * llv / hlv);
}

uint32_t god_guard_mgr::god_arithmetic(uint32_t base_value, uint32_t lv)
{
    return ((float)base_value / 100.0 * (float)lv);
}

void print_add_quality(uint32_t type, quality_t *q)
{
    TRACE_LOG("%u | %u %u %u %u %u %u %u %u %u", type, q->hp, q->mp, q->addhp, q->addmp, 
        q->atk, q->def, q->hit, q->dodge, q->crit);
}

void god_guard_mgr::calc_summon_add_quality(player_t* p, uint32_t *sum, uint32_t sum_cnt, quality_t *add_quality)
{
    for (uint32_t i = 0; i < sum_cnt; i++) {
        summon_mon_t* s = get_player_summon(p, sum[i]);
        summon_factor * f = get_summon_factor(get_summon_base_type(p, sum[i]));
        if (s && f) {
            //TRACE_LOG(" uid summonid [%u %u|%u] [%u %u]", p->id, s->mon_type, sum[i], s->attr_per, s->lv);
            add_quality->hp += summon_arithmetic(f->quality.hp, s->attr_per, s->lv);
            add_quality->mp += summon_arithmetic(f->quality.mp, s->attr_per, s->lv);
            add_quality->atk += summon_arithmetic(f->quality.atk, s->attr_per, s->lv);
            add_quality->def += summon_arithmetic(f->quality.def, s->attr_per, s->lv);
            add_quality->hit += summon_arithmetic(f->quality.hit, s->attr_per, s->lv);
            add_quality->addhp += summon_arithmetic(f->quality.addhp, s->attr_per, s->lv);
            add_quality->addmp += summon_arithmetic(f->quality.addmp, s->attr_per, s->lv);
            add_quality->dodge += summon_arithmetic(f->quality.dodge, s->attr_per, s->lv);
            add_quality->crit += summon_arithmetic(f->quality.crit, s->attr_per, s->lv);

            print_add_quality(s->mon_type, add_quality);
            for (uint32_t j = 0; j < sum_cnt; j++) {
                if (sum[i] == sum[j]) continue;

                summon_mon_t* gs = get_player_summon(p, sum[j]);
                summon_factor * gf = get_summon_factor(get_summon_base_type(p, sum[j]));
                if (gs && gf) {
                    //TRACE_LOG(" uid summonid [%u %u|%u]", p->id, gs->mon_type, sum[j]);
                    add_quality->hp += group_arithmetic(gf->quality.hp, f->group_quality.hp, 
                        gs->attr_per, gs->lv, s->lv);
                    add_quality->mp += group_arithmetic(gf->quality.mp, f->group_quality.mp, 
                        gs->attr_per, gs->lv, s->lv);
                    add_quality->atk += group_arithmetic(gf->quality.atk, f->group_quality.atk, 
                        gs->attr_per, gs->lv, s->lv);
                    add_quality->def += group_arithmetic(gf->quality.def, f->group_quality.def, 
                        gs->attr_per, gs->lv, s->lv);
                    add_quality->hit += group_arithmetic(gf->quality.hit, f->group_quality.hit, 
                        gs->attr_per, gs->lv, s->lv);
                    add_quality->addhp += group_arithmetic(gf->quality.addhp, f->group_quality.addhp, 
                        gs->attr_per, gs->lv, s->lv);
                    add_quality->addmp += group_arithmetic(gf->quality.addmp, f->group_quality.addmp, 
                        gs->attr_per, gs->lv, s->lv);
                    add_quality->dodge += group_arithmetic(gf->quality.dodge, f->group_quality.dodge, 
                        gs->attr_per, gs->lv, s->lv);
                    add_quality->crit += group_arithmetic(gf->quality.crit, f->group_quality.crit, 
                        gs->attr_per, gs->lv, s->lv);

                    print_add_quality(gs->mon_type, add_quality);
                }

            }
        }
    }
}

void god_guard_mgr::calc_god_add_quality(uint32_t* god_id, uint32_t lv, quality_t *add_quality)
{
    god_factor * f = get_god_factor(*god_id);
    if (f) {
        add_quality->hp += god_arithmetic(f->quality.hp, lv);
        add_quality->mp += god_arithmetic(f->quality.mp, lv);
        add_quality->atk += god_arithmetic(f->quality.atk, lv);
        add_quality->def += god_arithmetic(f->quality.def, lv);
        add_quality->hit += god_arithmetic(f->quality.hit, lv);
        add_quality->addhp += god_arithmetic(f->quality.addhp, lv);
        add_quality->addmp += god_arithmetic(f->quality.addmp, lv);
        add_quality->dodge += god_arithmetic(f->quality.dodge, lv);
        add_quality->crit += god_arithmetic(f->quality.crit, lv);
    }
}

uint32_t god_guard_mgr::calc_group_quality_and_power(player_t* p, uint32_t lv, quality_t *add_quality)
{
    memset(add_quality, 0x00, sizeof(quality_t));
    uint32_t *p_member = p->group_datas->member;
    //calc level 1
    calc_summon_add_quality(p, p_member, 1, add_quality);
    //print_add_quality(0, add_quality);
    //calc level 2
    calc_summon_add_quality(p, p_member + 1, 2, add_quality);
    //print_add_quality(0, add_quality);
    //calc level 3
    calc_summon_add_quality(p, p_member + 4, 2, add_quality);
    //print_add_quality(0, add_quality);
    calc_god_add_quality(p_member + 3, lv, add_quality);
    //print_add_quality(0, add_quality);
    //calc level 4
    calc_summon_add_quality(p, p_member + 7, 3, add_quality);
    //print_add_quality(0, add_quality);
    calc_god_add_quality(p_member + 6, lv, add_quality);
    //print_add_quality(0, add_quality);

    return (add_quality->hp * 0.2 + add_quality->mp * 0.3 + add_quality->atk * 10.0 + (add_quality->addhp + add_quality->addmp + add_quality->def + add_quality->hit + add_quality->dodge + add_quality->crit) * 5.0);
}

bool god_guard_mgr::init(const char* xml)
{
	if(xml == NULL){
		return false;
	}

	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr cur = NULL;

	xmlKeepBlanksDefault(0);

	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the god guard xml file is not exist"));
		return false;
	}

	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the god guard xml file content is empty"));
		return false;
	}
	cur = root->xmlChildrenNode;
	while(cur) {

		if( xmlStrcmp( cur->name, BAD_CAST"Numens") == 0 ) {
            xmlNodePtr child_node = cur->xmlChildrenNode;
            while( child_node ) {
                if ( xmlStrcmp( child_node->name, BAD_CAST"Numen") == 0 ) {
                    god_factor *p_god = new god_factor;
                    get_xml_prop_def(p_god->id, child_node, "ID", 0);

                    xmlNodePtr grandson_node = child_node->xmlChildrenNode;
                    while ( grandson_node ) {
                        if ( xmlStrcmp( grandson_node->name, BAD_CAST"Quality") == 0 ) {
                            get_xml_prop_def( p_god->quality.hit,      grandson_node, "Hit",    0);
                            get_xml_prop_def( p_god->quality.dodge,    grandson_node, "Dodge",  0);
                            get_xml_prop_def( p_god->quality.crit,     grandson_node, "Crit",   0);
                            get_xml_prop_def( p_god->quality.atk,      grandson_node, "Atk",    0);
                            get_xml_prop_def( p_god->quality.def,      grandson_node, "Def",    0);
                            get_xml_prop_def( p_god->quality.hp,       grandson_node, "Hp",     0);
                            get_xml_prop_def( p_god->quality.mp,       grandson_node, "Mp",     0);
                            get_xml_prop_def( p_god->quality.addhp,    grandson_node, "AddHp",  0);
                            get_xml_prop_def( p_god->quality.addmp,    grandson_node, "AddMp",  0);
                        }
                        grandson_node = grandson_node->next;
                    }
                    //print_add_quality(p_god->id, &(p_god->quality));
                    bool ret = add_god_data(p_god);
                    if(!ret) {
                        throw XmlParseError(std::string("numen_type has been existed"));
                        return false;
                    }
                }
                child_node = child_node->next;
            }
        }

        if( xmlStrcmp( cur->name, BAD_CAST"Summons") == 0 ) {
            xmlNodePtr child_node = cur->xmlChildrenNode;
            while( child_node ) {
                if ( xmlStrcmp( child_node->name, BAD_CAST"Summon") == 0 ) {
                    summon_factor *p_sum = new summon_factor;
                    get_xml_prop_def(p_sum->id, child_node, "ID", 0);

                    xmlNodePtr grandson_node = child_node->xmlChildrenNode;
                    while ( grandson_node ) {
                        if ( xmlStrcmp( grandson_node->name, BAD_CAST"Quality") == 0 ) {
                            get_xml_prop_def( p_sum->quality.hit,      grandson_node, "Hit",    0);
                            get_xml_prop_def( p_sum->quality.dodge,    grandson_node, "Dodge",  0);
                            get_xml_prop_def( p_sum->quality.crit,     grandson_node, "Crit",   0);
                            get_xml_prop_def( p_sum->quality.atk,      grandson_node, "Atk",    0);
                            get_xml_prop_def( p_sum->quality.def,      grandson_node, "Def",    0);
                            get_xml_prop_def( p_sum->quality.hp,       grandson_node, "Hp",     0);
                            get_xml_prop_def( p_sum->quality.mp,       grandson_node, "Mp",     0);
                            get_xml_prop_def( p_sum->quality.addhp,    grandson_node, "AddHp",  0);
                            get_xml_prop_def( p_sum->quality.addmp,    grandson_node, "AddMp",  0);
                        }
                        if ( xmlStrcmp( grandson_node->name, BAD_CAST"GroupQuality") == 0 ) {
                            get_xml_prop_def( p_sum->group_quality.hit,      grandson_node, "Hit",    0);
                            get_xml_prop_def( p_sum->group_quality.dodge,    grandson_node, "Dodge",  0);
                            get_xml_prop_def( p_sum->group_quality.crit,     grandson_node, "Crit",   0);
                            get_xml_prop_def( p_sum->group_quality.atk,      grandson_node, "Atk",    0);
                            get_xml_prop_def( p_sum->group_quality.def,      grandson_node, "Def",    0);
                            get_xml_prop_def( p_sum->group_quality.hp,       grandson_node, "Hp",     0);
                            get_xml_prop_def( p_sum->group_quality.mp,       grandson_node, "Mp",     0);
                            get_xml_prop_def( p_sum->group_quality.addhp,    grandson_node, "AddHp",  0);
                            get_xml_prop_def( p_sum->group_quality.addmp,    grandson_node, "AddMp",  0);
                        }

                        grandson_node = grandson_node->next;
                    }
                    //print_add_quality(p_sum->id, &(p_sum->quality));
                    //print_add_quality(p_sum->id, &(p_sum->group_quality));
                    bool ret = add_summon_data(p_sum);
                    if(!ret) {
                        throw XmlParseError(std::string("summon has been existed"));
                        return false;
                    }
                }
                child_node = child_node->next;
            }
        }
        cur = cur->next;
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return true;
}

bool god_guard_mgr::final()
{
	std::map<uint32_t, god_factor*>::iterator gItr = god_datas.begin();
	for(; gItr != god_datas.end(); ++gItr)
	{
		god_factor* p_god = gItr->second;
		delete p_god;
		p_god = NULL;
	}
    std::map<uint32_t, summon_factor*>::iterator sItr = summon_datas.begin();
	for(; sItr != summon_datas.end(); ++sItr)
	{
		summon_factor* p_sum = sItr->second;
		delete p_sum;
		p_sum = NULL;
	}

	return true;
}



//----------------------------------------------------
//  protocol
//----------------------------------------------------
int save_player_god_guard_info(player_t *p, uint32_t money)
{
	int idx = 0;
    pack_h(dbpkgbuf, money, idx);
    pack(dbpkgbuf, p->group_datas, sizeof(group_t), idx);
    //memcpy(dbpkgbuf + idx, p->group_datas, sizeof(group_t));
    //idx += sizeof(group_t);
    return send_request_to_db(NULL, p->id, p->role_tm, dbproto_set_role_god_guard_info, dbpkgbuf, idx);
}

int upgrade_god_guard_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    static uint32_t limit[3][2] = {{40, 200000}, {60, 5000000}, {80, 25000000}};
    if (p->group_datas->_level == 0) p->group_datas->_level = 1;
    uint32_t cur_level = p->group_datas->_level; 

    if (p->lv < limit[cur_level - 1][0] || p->coins < limit[cur_level - 1][1]) {
        return send_header_to_player(p, p->waitcmd, cli_err_ever_limit_time_act, 1);
    }

    p->group_datas->_level += 1;
    p->coins -= limit[cur_level - 1][1];

    KDEBUG_LOG(p->id, "upgrade god guard [%u |%u]\n", p->group_datas->_level, limit[cur_level - 1][1]);

    save_player_god_guard_info(p, limit[cur_level - 1][1]);

    int idx = sizeof(cli_proto_t); 
    pack(pkgbuf, p->group_datas->_level, idx);
    pack(pkgbuf, limit[cur_level - 1][1], idx);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int set_god_guard_position_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    static uint32_t pos_limit[4] = {1, 3, 6, 10};
    uint32_t cur_level = p->group_datas->_level; 
    KDEBUG_LOG(p->id, "set god guard [%u]\n", p->group_datas->_level);
    int idx = 0;
    group_t group_obj;
    for (uint32_t i = 0; i < max_group_member_num && i < pos_limit[cur_level - 1]; i++) {
        unpack(body, group_obj.member[i], idx);
        TRACE_LOG("uid[%u] pos[%u] guard[%u]", p->id, i, group_obj.member[i]);
        if (group_obj.member[i] == 0) continue;
        if (get_player_summon(p, group_obj.member[i]) == NULL && !is_numen_exist(p, group_obj.member[i])) {
            return -1;
        }
        for (uint32_t j = 0; j < i; j++) {
            if (group_obj.member[i] == group_obj.member[j]) {
                return -1;
            }
        }
    }

    group_obj._level = p->group_datas->_level;
    memcpy(p->group_datas, &group_obj, sizeof(group_t));
    save_player_god_guard_info(p, 0);

    //uint32_t _power = get_god_guard_mgr()->calc_group_quality_and_power(p, p->lv, p->add_quality);
    uint32_t _power = calc_and_save_player_attribute(p, 1);

    idx = sizeof(cli_proto_t); 
    pack(pkgbuf, _power, idx);
    pack(pkgbuf, p->add_quality->hp, idx);
    pack(pkgbuf, p->add_quality->mp, idx);
    pack(pkgbuf, p->add_quality->addhp, idx);
    pack(pkgbuf, p->add_quality->addmp, idx);
    pack(pkgbuf, p->add_quality->atk, idx);
    pack(pkgbuf, p->add_quality->def, idx);
    pack(pkgbuf, p->add_quality->hit, idx);
    pack(pkgbuf, p->add_quality->dodge, idx);
    pack(pkgbuf, p->add_quality->crit, idx);

    //memcpy(pkgbuf + idx, p->add_quality, sizeof(quality_t));
    //idx += sizeof(quality_t);
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}

int get_god_guard_position_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
    if (p->lv < 20) {
		ERROR_LOG(" %u get god guard [%u]\n", p->id, p->group_datas->_level);
		return -1;
    }
    if (p->group_datas->_level == 0) {
        p->group_datas->_level = 1;
    }
    KDEBUG_LOG(p->id, "get god guard [%u]\n", p->group_datas->_level);

    //uint32_t _power = get_god_guard_mgr()->calc_group_quality_and_power(p, p->lv, p->add_quality);
    uint32_t _power = calc_and_save_player_attribute(p, 1);

    int idx = sizeof(cli_proto_t); 
    pack(pkgbuf, p->group_datas->_level, idx);
    for (uint32_t i = 0; i < max_group_member_num; i++) {
        TRACE_LOG("god guard: [%u]", p->group_datas->member[i]);
        pack(pkgbuf, p->group_datas->member[i], idx);
    }
    pack(pkgbuf, _power, idx);
    pack(pkgbuf, p->add_quality->hp, idx);
    pack(pkgbuf, p->add_quality->mp, idx);
    pack(pkgbuf, p->add_quality->addhp, idx);
    pack(pkgbuf, p->add_quality->addmp, idx);
    pack(pkgbuf, p->add_quality->atk, idx);
    pack(pkgbuf, p->add_quality->def, idx);
    pack(pkgbuf, p->add_quality->hit, idx);
    pack(pkgbuf, p->add_quality->dodge, idx);
    pack(pkgbuf, p->add_quality->crit, idx);

    /*
    pack(pkgbuf, p->group_datas, sizeof(group_t), idx);
    pack(pkgbuf, _power, idx);
    pack(pkgbuf, p->add_quality, sizeof(quality_t), idx);
    */
    init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}



