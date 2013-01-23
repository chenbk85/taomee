#include <libtaomee++/conf_parser/xmlparser.hpp>

#include "bullet.hpp"
#include "player.hpp"
#include "player_attribute.hpp"
#include "battle_impl.hpp"
#include "player.hpp"
using namespace taomee;

bullet::bullet(skill_data*	skill_data, uint32_t owner_id, uint32_t bullet_id, uint32_t team, uint32_t bullet_times, uint32_t syn_number)
{
    id_ 	  		 = Object::id();		
    owner_id_ 		 = owner_id;
    team_            = team;
    p_data           = bullet_mgr::get_instance()->get_bullet_data_by_id(bullet_id);
    p_crater_data    = map_data_mgr::getInstance()->get_bullet_crater_data_by_id(p_data->bullet_crater_id);
	bullet_times_ 	 = bullet_times;
	syn_number_      = syn_number;
	p_skill_data	 = skill_data;
}

void bullet::init_bullet_pos(uint32_t x, uint32_t y, double x_speed, double y_speed, uint32_t dir)
{
    set_x(x);
    set_y(y);
    set_dir(dir);
    set_speed(x_speed, y_speed);	
}

bool bullet::merge_bullet_crater_to_map(map_data* data, Battle* btl)
{
    if(p_crater_data == NULL) return false;
    return merge_area(p_crater_data, get_x(), get_y(), data);
}

bool bullet::collision(Object* p_obj)
{
	Player* target = dynamic_cast<Player*>(p_obj);
	if(target)
	{
		return  in_area(target);
	}
	return false;
}

bool bullet::before_collision(Object* p_obj)
{
    return true;	
}

bool bullet::after_collision(Object* p_obj)
{	
    Player* target = dynamic_cast<Player*>(p_obj);
	if(target)
    { 
		ERROR_TLOG("bullet hit player bullet (%f %f)  player (%f %f )", get_x(), get_y(),  target->get_x(), target->get_y());
        ERROR_TLOG("notify bullet hit after player---- (%f,%f)", get_x(), get_y());

		target->btl->notify_all_bullet_hit_delay(p_data->bullet_id, syn_number_, bullet_times_, get_x(), get_y());

		Player* atker = target->btl->get_player_by_id(owner_id_);
		target->on_hit(this, atker);
		atker->on_hit_target(this, target);
    }
    return true;
}

bool bullet::line_collision(Object* p_obj)
{
	return false;
}

bool bullet::before_collision(map_data* data, void* para)
{
    return true;	
}

bool bullet::after_collision(map_data* data, void* para)
{
    Battle* btl = (Battle*)para;
    if(btl == NULL)return false;

	YJ_DEBUG_LOG("before notify bullet hit after map---- (%f,%f)", get_x(), get_y());

	btl->notify_all_bullet_hit_delay(p_data->bullet_id, syn_number_, bullet_times_, get_x(), get_y());

	check_blow_area(data, btl);

    if( merge_bullet_crater_to_map(btl->get_btl_map_data(), btl) )
	{
		btl->all_fall_down(syn_number_, bullet_times_);			
	}
    YJ_DEBUG_LOG("notify bullet hit after map---- (%f,%f)", get_x(), get_y());

    return true;	
}

bool bullet::line_collision(map_data* data, void* para)
{
	double x_ex = get_x() - get_old_x();
	double y_ex = get_y() - get_old_y();
	double slope = 0.0;
	if (x_ex != 0.0 && y_ex != 0.0) {
		slope = y_ex / x_ex;
	}
	

	YJ_DEBUG_LOG("slope %.3f  old %.3f %.3f now %.3f %.3f", slope, get_old_x(), get_old_y(), get_x(), get_y());

	double distance = pow(x_ex, 2) + pow(y_ex, 2);
	for (double i = 1.0; i < distance; i++) {
		double tmp_x = 0.0;
		double tmp_y = 0.0;

		double x_slope = 0.0;
		double y_slope = 0.0;
		if (x_ex == 0.0) {
			tmp_x = 0;
			tmp_y = 1;
		} else if (y_ex == 0.0) {
			tmp_x = 1;
			tmp_y = 0;
		} else {
			x_slope = 1 + pow(slope, 2);
			tmp_x = pow(i / x_slope, 0.5);

			y_slope = 1 + pow(1/slope, 2);
			tmp_y = pow(i / y_slope, 0.5);
		}
		

		double x_pos = x_ex > 0 ? (get_old_x() + tmp_x) : (get_old_x() - tmp_x) ;
		double y_pos = y_ex > 0 ? (get_old_y() + tmp_y) : (get_old_y() - tmp_y) ;

		YJ_DEBUG_LOG("line to %.3f %.3f ", x_pos, y_pos);
		int32_t index = get_index_from_pos(data, x_pos, y_pos);
		if( !data->check_valid_index(index))return false;
		if( data->grids[index] != 0 )
		{
			YJ_DEBUG_LOG("collisioned");
			set_x(x_pos);
			set_y(y_pos);
			return true;
		}	
	}
	return false;
}

uint32_t bullet::is_crit(Player* target)
{
	Player* atker = target->btl->get_player_by_id(owner_id_);
	if (!atker) {
		return 0;
	}
	float lucky_crit_rate = 0.0;
	if (target->get_merge_attr_value(OBJ_ATTR_LEVEL) <= 60) {
		uint32_t target_lv = target->get_merge_attr_value(OBJ_ATTR_LEVEL);
		lucky_crit_rate = atker->get_merge_attr_value(OBJ_ATTR_LUCK) * atker->get_merge_attr_value(OBJ_ATTR_CRIT) * 5.4;
		lucky_crit_rate /= (target_lv + 4);
		//lucky_crit_rate /= 100;
	} else {
		uint32_t target_lv = target->get_merge_attr_value(OBJ_ATTR_LEVEL);
		lucky_crit_rate = atker->get_merge_attr_value(OBJ_ATTR_LUCK) * atker->get_merge_attr_value(OBJ_ATTR_CRIT) * 5.4;
		lucky_crit_rate /= 
			(target_lv + 2 * (target_lv - 59));
		//lucky_crit_rate /= 100;
	}
	
	uint32_t rand_num = ranged_random(0, 100);
	if (rand_num <= lucky_crit_rate) {
		return 1;
	}
	return 0;
}

uint32_t bullet::calc_damage(Player* target, uint32_t& crit_flg)
{
	Player* atker = target->btl->get_player_by_id(owner_id_);
	uint32_t damage = 0;
	crit_flg = 0;
	if (atker) {
		uint32_t atl_ex = ranged_random(0, 20);
		uint32_t real_atk = atker->get_merge_attr_value(OBJ_ATTR_ATK) * (90 + atl_ex) / 100;
		uint32_t target_def = target->get_merge_attr_value(OBJ_ATTR_DEF);
		uint32_t tmp = real_atk > target_def ? (real_atk - target_def) : 0;

		uint32_t happy = atker->get_merge_attr_value(OBJ_ATTR_HAPPY);
		happy = happy > 100 ? 100 : happy;
		damage = tmp * happy /100;

		// calc crit flag
		crit_flg = is_crit(target);
		if (crit_flg) {
			//add damage
			damage = damage * (1 + atker->get_merge_attr_value(OBJ_ATTR_CRIT_DAMAGE));
		}

		//add skill damage
		if (p_skill_data) {
			damage = damage * p_skill_data->damage_percent / 100;
		}
		damage *= atker->player_state_mrg->damage_trim();
		YJ_DEBUG_LOG("calc damage skill=%u atk=%u targetdef=%u happy=%u crit=%u crit_damage=%u", 
			p_skill_data->skill_id, atker->get_merge_attr_value(OBJ_ATTR_ATK), target_def, 
			atker->get_merge_attr_value(OBJ_ATTR_HAPPY), crit_flg, atker->get_merge_attr_value(OBJ_ATTR_CRIT_DAMAGE));
	}
	return damage;
}

bool bullet::check_blow_area(map_data* data, Battle* btl)
{
	std::list<Player*>::iterator pItr = btl->playerlist.begin();

	for(; pItr != btl->playerlist.end(); ++pItr)
	{
		Player* target = *pItr;
		if(target->is_dead())continue;
		if( distance(target) > p_data->damage_radius)continue;

		Player* atker = target->btl->get_player_by_id(owner_id_);
		target->on_hit(this, atker);
		atker->on_hit_target(this, target);
	}
	return true;
}
////////////////////////////////////////////////////////////////

bullet_mgr* bullet_mgr::get_instance()
{
    static bullet_mgr obj;
    return &obj;
}

bool   bullet_mgr::init_xml(const char* xml)
{	
    xmlDocPtr   doc = NULL;
    xmlNodePtr  root = NULL;
    xmlNodePtr  bullet_node = NULL;
    xmlNodePtr  range_node = NULL;

    char err_msg[4096] = {0};

    doc = xmlParseFile(xml);
    if(doc == NULL)
    {
        sprintf(err_msg, "xml file %s is not exist", xml);
        throw XmlParseError(err_msg);
        return false;
    }

    root = xmlDocGetRootElement(doc);
    if(root == NULL)
    {
        xmlFreeDoc(doc);
        sprintf(err_msg, "xml file %s content is empty", xml);
        throw XmlParseError(err_msg);
        return false;
    }

    bullet_node = root->xmlChildrenNode;
    while(bullet_node)
    {
        if( xmlStrcmp(bullet_node->name, reinterpret_cast<const xmlChar*>("bullet")) == 0)	
        {
            bullet_data* p_data = new bullet_data();

            get_xml_prop_def(p_data->bullet_id,  bullet_node, "id", 0);			
            get_xml_prop_def(p_data->bullet_type,  bullet_node, "type", 0);
            get_xml_prop_def(p_data->bullet_width,  bullet_node, "width", 0);
            get_xml_prop_def(p_data->bullet_height,  bullet_node, "height", 0);
            get_xml_prop_def(p_data->max_damage,  bullet_node, "max_damage", 0);
            get_xml_prop_def(p_data->damage_radius, bullet_node, "damage_radius", 0);
            get_xml_prop_def(p_data->bullet_crater_id, bullet_node, "bullet_crater_id", 0);

            if(p_data->bullet_id == 0)
            {
                xmlFreeDoc(doc);
                sprintf(err_msg, "the bullet_id = 0");
                throw XmlParseError(err_msg);
                return false;
            }

            if( is_bullet_data_exist(p_data) )
            {
                xmlFreeDoc(doc);
                sprintf(err_msg, "the bullet_id = %u has existed", p_data->bullet_id);
                throw XmlParseError(err_msg);
                return false;
            }

            range_node = bullet_node->xmlChildrenNode;
            while(range_node)
            {
                damage_range  range;
                char range_str[1024] = {0};

                get_xml_prop_def(range.damage_percent,  range_node, "damage_percent", 0);
                get_xml_prop_raw_str_def(range_str, range_node, "radius_percent", "");	
                sscanf(range_str, "%u-%u", &range.begin_odds, &range.end_odds);

                p_data->ranges.push_back(range);

                range_node = range_node->next;
            }

            if(p_data->ranges.size() == 0)
            {
                xmlFreeDoc(doc);
                sprintf(err_msg, "the bullet_id = %u has no range damage node ", p_data->bullet_id);
                throw XmlParseError(err_msg);
                return false;
            }
            add_bullet_data(p_data);
        }
        bullet_node = bullet_node->next;		
    }

    xmlCleanupParser();
    xmlFreeDoc(doc);
    return true;
}

bool   bullet_mgr::final()
{
    std::map<uint32_t, bullet_data*>::iterator pItr = data_maps.begin();
    for(; pItr != data_maps.end(); ++pItr)
    {
        bullet_data* p_data = pItr->second;
        delete p_data;
        p_data = NULL;
    }
    return true;
}

bool   bullet_mgr::add_bullet_data(bullet_data* data)
{
    if(is_bullet_data_exist(data))return false;
    data_maps[data->bullet_id] = data;
    return true;
}

bullet_data* bullet_mgr::get_bullet_data_by_id(uint32_t bullet_id)
{
    std::map<uint32_t, bullet_data*>::iterator pItr = data_maps.find(bullet_id);
    if(pItr == data_maps.end())return NULL;
    return pItr->second;
}

bool   bullet_mgr::is_bullet_data_exist(bullet_data* data)
{
    std::map<uint32_t, bullet_data*>::iterator pItr = data_maps.find(data->bullet_id);
    return pItr != data_maps.end();
}

bool   bullet_mgr::is_bullet_data_exist(uint32_t bullet_id)
{
    std::map<uint32_t, bullet_data*>::iterator pItr = data_maps.find(bullet_id);	
    return pItr != data_maps.end();
}


bullet* create_bullet(skill_data* p_skill_data, uint32_t id, uint32_t owner_id, uint32_t team, uint32_t syn_number, uint32_t bullet_times)
{
    bullet_data* data = bullet_mgr::get_instance()->get_bullet_data_by_id(id);
    if(data == NULL)return NULL;
    bullet *p_bullet = new bullet(p_skill_data, owner_id, id, team, syn_number, bullet_times);		
    p_bullet->set_rectangle(data->bullet_width, data->bullet_height);
    return p_bullet;
}

void    destroy_bullet(bullet* p_bullet)
{
    if(p_bullet != NULL)	
    {
        delete p_bullet;	
    }
}
