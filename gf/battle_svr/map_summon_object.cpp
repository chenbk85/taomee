#include "base_effect.hpp"
#include "skill_imme_effect.hpp"
#include "map_summon_object.hpp"
#include "battle_impl.hpp"
#include "player_status.hpp"
map_summon_object::map_summon_object()
{
	id_ = 0;
	owner_id_ = 0;
	model_ = 0;
	team_  = 0;
	action_radius_ = 0;
	action_x_ =0;
	action_y_ = 0;
	target_type_   = 0;
	action_type_ = 0;
	action_times_ = 0;
	memset(nick, 0, sizeof(nick));
	effect_ = NULL;
	battle_ = NULL;
	buff_id_ = 0;
	delay_time_ = 0;
	call_skill_id_ = 0;
}

map_summon_object::~map_summon_object()
{
	id_ = 0;
	owner_id_ = 0;
	model_ = 0;
	team_ = 0;
	action_radius_ = 0;
	action_x_ = 0;
	action_y_ = 0;
	target_type_   = 0;
	action_type_ = 0;
	action_times_ = 0;
	effect_ = NULL;
	battle_ = NULL;
	cur_map_ = NULL;
	buff_id_ = 0;
	delay_time_ = 0;
	call_skill_id_ = 0;
}

bool map_summon_object::init( uint32_t type_id, uint32_t model, uint32_t action_radius, uint32_t action_x, uint32_t action_y, uint32_t target_type, uint32_t action_type, uint32_t skill_id )
{
	id_ = Object::id();
	Object::set_type(type_id);
	model_ = model;
	action_radius_ = action_radius;
	target_type_ = target_type;
	action_type_ = action_type;
	skill_id_ = skill_id;
	action_x_ = action_x;
	action_y_ = action_y;
	return true;
}

void map_summon_object::set_buff_id(uint32_t buff_id)
{
	buff_id_ = buff_id;
}

bool map_summon_object::final()
{
	id_ = 0;
	owner_id_ = 0;
	model_ = 0;
	action_radius_ = 0;
	target_type_   = 0;
	action_type_ = 0;
	action_times_ = 0;
	battle_ = NULL;
	if( effect_ != NULL)
	{
		delete effect_;
		effect_ = NULL;
	}
	cur_map_ = NULL;
	return true;	
}

void map_summon_object::set_pos(const Vector3D& v, map_t* map)
{
	Object::set_pos(v);
	cur_map_ = map;
}

void map_summon_object::set_range(uint32_t radius)
{
	set_region(radius, radius);
}

void map_summon_object::set_range(uint32_t x, uint32_t y)
{
	set_region(x, y);
}

void map_summon_object::set_owner_id(uint32_t owner_id, uint32_t team, Battle* battle)
{
	owner_id_ = owner_id;
	battle_ = battle;
	team_ = team;
}

void map_summon_object::set_base_effect(base_effect* effect)
{
	effect_ = effect;
}

void map_summon_object::set_times(uint32_t time)
{
	action_times_ = time;
}

void map_summon_object::set_delay_time(uint32_t time)
{
	delay_time_ = time;
}

void map_summon_object::set_call_skill_id(uint32_t skill_id)
{
	call_skill_id_ = skill_id;
}

bool map_summon_object::check_action_radius(Player* p)
{
	return collision(p);
}

bool map_summon_object::check_target_type(Player* p)
{
	if( target_type_ == 1)//敌人
	{
		return p->team != team_;
	}
	else if( target_type_ == 2)//友方
	{
		return p->team == team_;
	}
	else if(target_type_ == 3)//无视敌友
	{
		return true;
	}
	return true;
}	

bool map_summon_object::check_trigger( struct timeval  next_tm)
{
	return true;
}

bool map_summon_object::check_delete(struct timeval next_tm)
{
	if( action_type_ == action_condition_times_type )
	{
		return action_times_ <=0 || (m_duration_time && is_timer_finish(next_tm));
	}
	if( action_type_ == action_times_type)
	{
		return action_times_ <=0;
	}
	else if( action_type_ == action_continue_type)
	{
		return is_timer_finish(next_tm);
	}
	else if(action_type_ == action_dead_type )
	{
		return is_timer_finish(next_tm);
	}
	return false;
}

bool map_summon_object::check_delay_time( struct timeval  next_tm)
{
	return taomee::timediff2(next_tm, m_begin_time) > (int)delay_time_;
}

bool map_summon_object::process(Player* p, struct timeval next_tm)
{
	if(effect_ == NULL)return false;

	switch(action_type_)
	{
		case action_condition_times_type:
		{
			effect_->process_effect(p, next_tm, NULL);
			action_times_--;
		}
		break;

		case action_continue_type:
		case action_dead_type:
		case action_times_type:
		{
			effect_->process_effect(p, next_tm, NULL);
		}
		break;

	}

	return true;
}

bool map_summon_object::process_buff(Player* p)
{
	if(buff_id_ != 0 && !is_player_buff_exist(p, buff_id_)){
		add_player_buff(p, buff_id_, 0);
		if(action_type_ == action_condition_times_type ){
			action_times_--;
		}
	}
	return true;
}

uint32_t map_summon_object::get_buff_id()
{
	return buff_id_;
}

void notify_add_map_summon_object( map_summon_object* obj, Player* p)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);	
	
    pack(pkgbuf, static_cast<uint32_t>(0), idx);

	pack(pkgbuf, static_cast<uint32_t>(obj->id_), idx);           //id
	pack(pkgbuf, static_cast<uint32_t>(0), idx);                  //role_tm
	pack(pkgbuf, static_cast<uint32_t>(obj->model_), idx);        //role_type
	pack(pkgbuf, 0, idx);                                         //power_user
	pack(pkgbuf, 0, idx);                                         //show state
	pack(pkgbuf, 0, idx);                                         //VIP
	pack(pkgbuf, 0, idx);                                         //VIP LEVE
	pack(pkgbuf, static_cast<uint8_t>(0), idx);                   //team
	pack(pkgbuf, obj->nick, sizeof(obj->nick), idx);               //nick
	pack(pkgbuf, 0, idx);                                          //lv
	pack(pkgbuf, 0, idx);                                          //max_hp
	pack(pkgbuf, 0, idx);                                          //hp
	pack(pkgbuf, 0, idx);                                          //max_mp
	pack(pkgbuf, 0, idx);                                          //mp
	pack(pkgbuf, 0, idx);                                          //exp
	pack(pkgbuf, 0, idx);                                          //honor
	pack(pkgbuf, static_cast<uint32_t>(obj->pos().x()), idx);      //x 
	pack(pkgbuf, static_cast<uint32_t>(obj->pos().y()), idx);      //y
	pack(pkgbuf, static_cast<uint8_t>(1), idx);                    //dir
	pack(pkgbuf, 0, idx);										   //speed
	pack(pkgbuf, 0, idx);                                          //clothes_count
	pack(pkgbuf, 0, idx);                                          //summon count 
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_realtm_birth_mon_info, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	
	if(p){
		send_to_player(p, pkgbuf, idx, 0);
	}
	else
	{
		obj->cur_map_->send_to_map(pkgbuf, idx, 0);
	}
}


void notify_del_map_summon_object( map_summon_object* obj)
{
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, obj->id_, idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_del_map_summon_obj, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	obj->cur_map_->send_to_map(pkgbuf, idx, 0);
}

void notify_strigger_map_summon_object(map_summon_object* obj)
{
	if( obj->skill_id_ == 0)return;
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, obj->skill_id_, idx);
	pack(pkgbuf, obj->pos().x(), idx);
	pack(pkgbuf, obj->pos().y(), idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_skill_effect_noti, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	obj->cur_map_->send_to_map(pkgbuf, idx, 0); 
}

void notify_strigger_map_summon_object_by_call(map_summon_object* obj)
{
	if( obj->call_skill_id_ == 0)return;
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, obj->call_skill_id_, idx);
	pack(pkgbuf, obj->pos().x(), idx);
	pack(pkgbuf, obj->pos().y(), idx);
	init_cli_proto_head(pkgbuf + sizeof(btl_proto_t), cli_proto_skill_effect_noti, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);
	obj->cur_map_->send_to_map(pkgbuf, idx, 0);
}


map_summon_object*  create_map_summon_object( uint32_t id)
{
	map_summon_data* pdata = map_summon_data_mgr::getInstance()->get_map_summon_data(id);
	if (pdata == NULL) {
		return NULL;
	}
	uint32_t effect_id = pdata->effect_type*1000 + pdata->effect_id;
	
	effect_data* pEffectdata = effect_data_mgr::getInstance()->get_effect_data_by_id(effect_id);
	base_effect* pEffect = NULL;
	struct timeval begin_time = *get_now_tv();
	if(pEffectdata != NULL){
		pEffect = new base_effect();
		pEffect->init_base_effect(pEffectdata, begin_time);
	}

	map_summon_object *p_summon = new map_summon_object();
	
	p_summon->init(pdata->type, pdata->model,  pdata->action_radius, pdata->action_x, pdata->action_y, pdata->action_target, pdata->action_type, pdata->show_id);
	p_summon->set_base_effect( pEffect);
	if(pdata->action_radius)
	{
		p_summon->set_range( pdata->action_radius);
	}
	else 
	{
		p_summon->set_range(pdata->action_x, pdata->action_y);
	}
	
	p_summon->set_times( pdata->action_times);
	p_summon->set_delay_time(pdata->delay_time);
	p_summon->init_base_duration_timer(begin_time , pdata->duration_time);
	p_summon->set_buff_id( pdata->buff_id);
	p_summon->set_call_skill_id(pdata->call_show_id);
	return p_summon;
}

void destroy_map_summon_object(map_summon_object* p)
{
	p->final();
	delete p;
}


void notify_map_summon_object(Player* p)
{
	Battle* btl = p->btl;
	std::list<map_summon_object*>::iterator pItr = btl->map_summon_list.begin();
	for( ; pItr != btl->map_summon_list.end(); ++pItr)
	{
		if( (*pItr)->cur_map_ != p->cur_map ){
			notify_add_map_summon_object(*pItr, p);
		}
	}
}
//-----------------------------------------------------------------------//
map_summon_data_mgr::map_summon_data_mgr()
{

}

map_summon_data_mgr::~map_summon_data_mgr()
{

}


map_summon_data_mgr* map_summon_data_mgr::getInstance()
{
	static map_summon_data_mgr obj;
	return &obj;
}

bool map_summon_data_mgr::init(const char* xml_name)
{
	if(xml_name == NULL){
		return false;
	}
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr map_summon_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml_name);
	if(doc == NULL){
		throw XmlParseError(std::string("the xml file is not exist"));
		return false;
	}
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the xml file content is empty"));
		return false;
	}
	bool ret = true;
	
	map_summon_node = root->xmlChildrenNode;
	while(map_summon_node)
	{
		if(  xmlStrcmp(map_summon_node->name, BAD_CAST"summon") == 0 )		
		{
			uint32_t id = 0;
			get_xml_prop_def(id, map_summon_node, "id", 0);
			if(  is_map_summon_data_exist(id) ){
				ret = false;
				throw XmlParseError(std::string("id exist"));
				goto ERR_LOGIC;
			}
			map_summon_data* pdata = new map_summon_data();
			pdata->id = id;

			get_xml_prop_def(pdata->type, map_summon_node, "type", 0); 
			get_xml_prop_def(pdata->model, map_summon_node, "model", 0); 
			get_xml_prop_def(pdata->action_radius, map_summon_node, "action_radius", 0); 	
			get_xml_prop_def(pdata->action_type, map_summon_node, "action_type", 0); 
			get_xml_prop_def(pdata->action_target, map_summon_node, "action_target", 0); 
			get_xml_prop_def(pdata->action_times, map_summon_node, "action_times", 0); 
			get_xml_prop_def(pdata->show_id,  map_summon_node, "show_id", 0);
			get_xml_prop_def(pdata->duration_time,  map_summon_node, "duration_time", 0);
			get_xml_prop_def(pdata->effect_id,  map_summon_node, "effect_id", 0);
			get_xml_prop_def(pdata->effect_type,  map_summon_node, "effect_type", 0);	
			get_xml_prop_def(pdata->buff_id, map_summon_node, "buff_id", 0);
			get_xml_prop_def(pdata->delay_time, map_summon_node, "delay_time", 0);
			get_xml_prop_def(pdata->call_show_id, map_summon_node, "call_show_id", 0);
			add_map_summon_data(pdata);
		}
		map_summon_node = map_summon_node->next;
	}


ERR_LOGIC:
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return ret;
}

bool map_summon_data_mgr::final()
{
	std::map<uint32_t, map_summon_data*>::iterator pItr = m_data_maps.begin();
	for( ; pItr != m_data_maps.end(); ++pItr)
	{
		map_summon_data* data = pItr->second;
		delete data;
		data = NULL;	
	}
	m_data_maps.clear();
	return true;
}

bool map_summon_data_mgr::add_map_summon_data(  map_summon_data* data)
{
	std::map<uint32_t, map_summon_data*>::iterator pItr = m_data_maps.find(data->id);
	if(pItr == m_data_maps.end()){
		m_data_maps[data->id] = data;
	}
	return true;
}

bool map_summon_data_mgr::is_map_summon_data_exist( uint32_t id)
{
	std::map<uint32_t, map_summon_data*>::iterator pItr = m_data_maps.find(id);
	if(pItr == m_data_maps.end()){
		return false;
	}
	return true;
}

map_summon_data* map_summon_data_mgr::get_map_summon_data(uint32_t id)
{
	std::map<uint32_t, map_summon_data*>::iterator pItr = m_data_maps.find(id);
	if(pItr == m_data_maps.end()){
		return NULL;
	}
	return pItr->second;
}

