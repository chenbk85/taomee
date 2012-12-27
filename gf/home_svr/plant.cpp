/*
 * =====================================================================================
 *
 *       Filename:  plant.cpp
 *
 *    Description:  :wq
 *
 *
 *        Version:  1.0
 *        Created:  07/05/2011 11:14:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include "home_attr.hpp"
#include "plant.hpp"
#include "player.hpp"
#include "home.hpp"
#include "home_impl.hpp"
#include "utils.hpp"

GobalPlants* g_plants;
FerilizerMrg* g_ferilizer_mrg;


Plant::Plant()
{
	status = 1;
	p_home = 0;
}


Plant::Plant(SummonHome* p_home_in, plant_elem_t * plant_in)
{
	p_home = p_home_in;
	plant_id_ = plant_in->plant_id;
	plant_tm_ = plant_in->plant_tm;

	status = plant_in->status;
	status_tm = plant_in->status_tm;
	field_id_ = plant_in->field;
	water_tm = plant_in->water_tm;
	speed_per = plant_in->speed_per;
	output_per = plant_in->output_per;
	
	status_duration = plant_in->status_tm + get_duration_time(); 

	fruit_cnt = plant_in->fruit_cnt;
}

Plant::~Plant()
{

}

uint32_t Plant::get_duration_time()
{
	plant_t * p_xml = g_plants->get_plant(plant_id_);

	//int per_lv = this->p_home->p_base_home->GetHomeDetail()->get_home_level();
//	return 10;
	return p_xml->status_arr[status - 1].duration; 
}

uint32_t Plant::get_total_fruit_cnt()
{
	plant_t * p_xml = g_plants->get_plant(plant_id_);

//	if (p_xml->status_arr[this->status - 1].fruit) {
//		return (uint32_t)(p_xml->status_arr[status - 1].fruit_cnt
//			   	* (double(100 + output_per) / 100.0));
//	}
//
	return p_xml->status_arr[this->status - 1].fruit_cnt;
}

bool Plant::can_pick_fruit(Player * p)
{

	plant_t * p_xml = g_plants->get_plant(plant_id_);
	if (p_xml->status_arr[this->status - 1].fruit && this->fruit_cnt) {
		//for owner
		if (p_home->p_base_home->OwnerUserId == p->id 
			 && p_home->p_base_home->OwnerRoleTime == p->role_tm) {
			return true;
		} 
		//for guest
		if (this->fruit_cnt > (get_total_fruit_cnt() / 2) ) {
			return true;
		}
	}
	return false;
}

void Plant::update(int time_elapsed) 
{
	//uint32_t duration = p_xml->status_arr[status - 1].duration;
	//uint32_t end_flg = p_xml->status_arr[status - 1].end_flg;
//	uint32_t user_active = p_xml->status_arr[status - 1].user_active;
	uint32_t now = get_now_tv()->tv_sec;
	plant_t* p_xml = g_plants->get_plant(plant_id_);


	if (p_xml->status_arr[this->status - 1].duration) { //改状态会主动变更
		
		if (now > status_tm + this->get_duration_time()) { //每次都算，，。

			this->status ++;

			if (p_xml->status_arr[this->status - 1].water_tm) {
				this->water_tm = p_xml->status_arr[this->status - 1].water_tm + now; //下次可浇水时间
			} else {
				this->water_tm = 0;
			}

			this->status_tm = now; //新状态时间

			if (p_xml->status_arr[this->status - 1].duration) {
				this->status_duration = get_duration_time() + now;
			} else {
				this->status_duration = 0;
			}

			this->fruit_cnt = get_total_fruit_cnt();

		//	ERROR_LOG("[%u now %u next %u water %u]", this->plant_id_, now, this->status_duration, this->water_tm);

			notify_plant_status_changed();
		} 
	}
}

uint32_t Plant::fruit()
{
	plant_t* p_xml = g_plants->get_plant(plant_id_);
	return p_xml->status_arr[status - 1].fruit; 
}

void Plant::be_picked(uint32_t cnt)
{
	if (fruit_cnt) {
		fruit_cnt = fruit_cnt - cnt; 
	}

	plant_t* p_xml = g_plants->get_plant(plant_id_);

	if (p_xml->status_cnt == status && fruit_cnt == 0) { //最终
		status = 0;
		status_duration = 0;
		water_tm = 0;
	}

	this->notify_plant_status_changed();
}

bool Plant::need_deleted()
{
	return (!status && !fruit_cnt && !status_duration); 
}


void Plant::change_grow_status(uint32_t status_in, uint32_t status_tm_passed) 
{
	uint32_t old_status = status;
	 if (status_in == plant_grow_status_dead) {
		 status = 0;
	 } else if (status_in == plant_grow_status_up) {   
 		 if (!(is_final_status())) {
			status ++;
		 }	 
	 } else if (status_in == plant_grow_status_down) { 

	 } else if (status_in == plant_grow_status_init) {
	 	status = 1;
	 }
	 if (old_status != status) {
	 	 status_tm = get_now_tv()->tv_sec + status_tm_passed;
		 notify_plant_status_changed();
	 }
 } 

plant_grow_status_t* Plant::get_cur_grow_statu()
{
	plant_t * p_xml = g_plants->get_plant(plant_id());
		
	return &(p_xml->status_arr[status - 1]);

}


bool Plant::is_cur_status_user_active() 
{
	 plant_t* p_xml = g_plants->get_plant(plant_id_);
	 return (p_xml->status_arr[status - 1].duration == 0); 
 } 


bool Plant::is_final_status() 
{
 	plant_t* p_plant = g_plants->get_plant(plant_id_);
	if (p_plant) {
		return (status >= p_plant->status_cnt) ? true : false;
	}
	return false;
}

bool Plant::if_can_be_watered()
{
	uint32_t now = get_now_tv()->tv_sec;
	return (water_tm && now >= water_tm);
}

void Plant::water_plant(Ferilizer_t* p_ferilizer)
{
	water_tm = get_now_tv()->tv_sec + get_cur_grow_statu()->water_tm;
	water_cnt ++;
	ferilizer_plant(p_ferilizer);
}

void Plant::ferilizer_plant(Ferilizer_t* p_ferilizer)
{
	speed_per += p_ferilizer->speed_per;
	output_per += p_ferilizer->output_per;
	notify_plant_status_changed();
}

void Plant::ferilizer_plant(uint32_t speed_per_in, uint32_t output_per_in)
{
	speed_per += speed_per_in;
	output_per += output_per_in;
}

int Plant::save_status_to_db()
{
	uint32_t ownerid = p_home->p_base_home->OwnerUserId;
	uint32_t ownertm = p_home->p_base_home->OwnerRoleTime;
	int idx = 0;
	pack_plant_info_h(dbpkgbuf, idx);
	return send_request_to_db(0, ownerid, ownertm, dbproto_set_plant_status, dbpkgbuf, idx);
}

int Plant::notify_plant_status_changed()
{
	TRACE_LOG("%u %u", plant_id(), status);
	if (p_home && p_home->Players_.size()) {
		Player* p = p_home->Players_.begin()->second;

		TRACE_LOG("%u %u %u", p->id, plant_id(), status);
		int idx = sizeof(home_proto_t);
		idx += sizeof(cli_proto_t);
		pack_plant_info(pkgbuf, idx);
		init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
		init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), 0, cli_proto_notify_plant_status, 0, idx-sizeof(home_proto_t));
		p->CurHome->send_to_home(pkgbuf, idx, 0, 0);
	}

	if (p_home && p_home->p_base_home && p_home->p_base_home->OwnerUserId && p_home->p_base_home->OwnerRoleTime) {
		save_status_to_db();
	}
	return 0;
}

void Plant::pack_plant_info(void * pkg, int & idx)
{

	pack(pkg, field_id_, idx);
	pack(pkg, plant_id_, idx);
	pack(pkg, plant_tm_, idx);
	pack(pkg, status, idx);
//	pack(pkg, status_tm, idx);
	pack(pkg, status_duration, idx);
	pack(pkg, water_tm, idx);
}

void Plant::pack_plant_info_h(void * pkg, int & idx)
{
	pack_h(pkg, plant_id_, idx);
	pack_h(pkg, plant_tm_, idx);
	pack_h(pkg, field_id_, idx);
	pack_h(pkg, status, idx);
	pack_h(pkg, status_tm, idx);
	//pack_h(pkg, status_duration, idx);
	pack_h(pkg, water_tm, idx);
	pack_h(pkg, fruit_cnt, idx);
	pack_h(pkg, speed_per, idx);
	pack_h(pkg, output_per, idx);
}

GobalPlants::GobalPlants(const char* xml_file)
{
	xml_file_name = xml_file;
	init_plants();
}

GobalPlants::~GobalPlants()
{
}

int GobalPlants::init_plants()
{
	xmlDocPtr doc = xmlParseFile(xml_file_name.c_str());
	if (!doc) {
		throw XmlParseError(std::string("failed to parse plant file '") + xml_file_name + "'");
		ERROR_LOG("failed to parse item file!");
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("xmlDocGetRootElement error when loading item file '") + xml_file_name + "'");
		ERROR_LOG("xmlDocGetRootElement error when loading plant file!");
	}
	
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Plant"))) {
			struct plant_t* p_plant = (struct plant_t*)malloc(sizeof(struct plant_t));
			memset(p_plant, 0x00, sizeof(struct plant_t));
			get_xml_prop(p_plant->id, cur, "ID");
			//get_xml_prop(p_plant->seed_id, cur, "SeedID");
			get_xml_prop_def(p_plant->e_item, cur, "e_item", 0);
			get_xml_prop_def(p_plant->d_odds, cur, "d_odds", 0);
			get_xml_prop_arr_def(p_plant->home_lv, cur, "HomeLv", 0);

			p_plant->status_cnt = 0;

			xmlNodePtr step_cur = cur->xmlChildrenNode;
			while(step_cur) {
				if (!xmlStrcmp(step_cur->name, reinterpret_cast<const xmlChar*>("Step"))) {
					get_xml_prop(p_plant->status_arr[p_plant->status_cnt].id, step_cur, "ID");
					get_xml_prop_def(p_plant->status_arr[p_plant->status_cnt].duration, step_cur, "Duration", 0);
					get_xml_prop_def(p_plant->status_arr[p_plant->status_cnt].fruit, step_cur, "Fruit", 0);
					get_xml_prop_def(p_plant->status_arr[p_plant->status_cnt].fruit_cnt, step_cur, "Count", 0);
					get_xml_prop_def(p_plant->status_arr[p_plant->status_cnt].water_tm, step_cur, "Water", 0);
					p_plant->status_cnt ++;
				}

				step_cur = step_cur->next;
			}

			plant_map_.insert(std::map<uint32_t, plant_t*>::value_type(p_plant->id, p_plant));
		}
		cur = cur->next;
	}
	std::map<uint32_t, plant_t*>::iterator it = plant_map_.begin();
	for (; it != plant_map_.end(); ++it) {
		TRACE_LOG("%u %u", it->second->id, it->second->status_cnt);
		for (uint32_t i = 0; i < it->second->status_cnt; i++) {
			TRACE_LOG("%u %u ", it->second->status_arr[i].id, it->second->status_arr[i].duration);
		}
	}
	return 0;
}

FerilizerMrg::FerilizerMrg(const char* xml_file)
{
	xml_file_name = xml_file;
	init_ferilizer();
}

FerilizerMrg::~FerilizerMrg()
{
	std::map<uint32_t, Ferilizer_t*>::iterator it = ferilizer_map_.begin();
	for (; it != ferilizer_map_.end(); ++it) {
		TRACE_LOG("%u %u %u", it->second->id, it->second->speed_per, it->second->output_per);
		delete (it->second);
	}
	ferilizer_map_.clear();
}

int FerilizerMrg::init_ferilizer()
{
	xmlDocPtr doc = xmlParseFile(xml_file_name.c_str());
	if (!doc) {
		throw XmlParseError(std::string("failed to parse plant file '") + xml_file_name + "'");
		ERROR_LOG("failed to parse item file!");
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (!cur) {
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("xmlDocGetRootElement error when loading item file '") + xml_file_name + "'");
		ERROR_LOG("xmlDocGetRootElement error when loading plant file!");
	}
	
	// load items from an xml file
	cur = cur->xmlChildrenNode; 
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Ferilizer"))) {
			struct Ferilizer_t* p_ferilizer = (struct Ferilizer_t*)malloc(sizeof(struct Ferilizer_t));
			memset(p_ferilizer, 0x00, sizeof(struct Ferilizer_t));
			get_xml_prop_def(p_ferilizer->id, cur, "ID", 0);
			get_xml_prop_def(p_ferilizer->speed_per, cur, "SpeedPer", 0);
			get_xml_prop_def(p_ferilizer->output_per, cur, "OutputPer", 0);
			get_xml_prop_def(p_ferilizer->home_speed_per, cur, "HomeSpeedPer", 0);
			get_xml_prop_def(p_ferilizer->home_output_per, cur, "HomeOutputPer", 0);

			ferilizer_map_.insert(std::map<uint32_t, Ferilizer_t*>::value_type(p_ferilizer->id, p_ferilizer));
		}
		cur = cur->next;
	}
	std::map<uint32_t, Ferilizer_t*>::iterator it = ferilizer_map_.begin();
	for (; it != ferilizer_map_.end(); ++it) {
		TRACE_LOG("%u %u %u", it->second->id, it->second->speed_per, it->second->output_per);
	}
	return 0;
}


//////////////////////////////////
//gobal func

int get_plants_list_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	if (p->CurHome && p->CurHome->IsSummerHome() && p->CurHome->GetHomeDetail()) {
		if (p->CurHome->GetHomeDetail()->is_plants_db_updated()) {
			int idx = sizeof(home_proto_t);
	     	idx += sizeof(cli_proto_t);
			p->CurHome->GetHomeDetail()->pack_all_plants_info(pkgbuf, idx);
	    	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	    	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_get_plants_list, 0, idx-sizeof(home_proto_t));
			send_to_player(p, pkgbuf, idx, 1);
			return 0;
		} else {
			db_get_plants_list(p, p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime);
			return 0;
		}
	}
	return 0;
}

int send_set_plant_status_rsp(Player* p, uint32_t field_id, uint32_t plant_id, uint32_t plant_tm, uint32_t type, uint32_t item_id)
{
	int idx = sizeof(home_proto_t);
	taomee::pack_h(pkgbuf, field_id, idx);
	taomee::pack_h(pkgbuf, plant_id, idx);
	taomee::pack_h(pkgbuf, plant_tm, idx);
	taomee::pack_h(pkgbuf, type, idx);
	taomee::pack_h(pkgbuf, item_id, idx);
	init_home_proto_head(pkgbuf, p->waitcmd, idx);
	DEBUG_LOG("%u field:%u plant_id:%u plant_tm:%u type:%u itemid:%u", p->id, field_id, plant_id, plant_tm, type, item_id);
	return send_to_player(p, pkgbuf, idx, 1); 
}

int set_plant_status_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
struct set_plant_status_rsp_t {
	uint32_t field_id;
	uint32_t plant_id;
	uint32_t plant_tm;
	uint32_t type;
	uint32_t item_id;
}__attribute__((packed));
	set_plant_status_rsp_t* p_rsp = (set_plant_status_rsp_t*)(body);

	if (!(p->CurHome) || !(p->CurHome->IsSummerHome()) || !(p->CurHome->GetHomeDetail())) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}

	Plant* p_plant = p->CurHome->GetHomeDetail()->get_plant(p_rsp->field_id);
	SummonHome* p_summ_home = p->CurHome->GetHomeDetail();
	if (p_rsp->type == 1) {
	//类型是浇水
		Ferilizer_t* p_ferilizer = g_ferilizer_mrg->get_ferilizer(0);
		if (!p_ferilizer) {
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		if (!p_plant || ! (p_plant->if_can_be_watered()) || !p_ferilizer) {
			return send_header_to_player(p, p->waitcmd, cli_err_cannot_water, 1);
		}

		if (!p->CurHome->IsOwner(p)) {
			p->add_curhome_exp(1);
		} 

		p_plant->water_plant(p_ferilizer);

		db_add_home_log(p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, p,
			home_log_type_water, home_log_access_type_all, 0);
		
	} else {
	//类型是施肥
		Ferilizer_t* p_ferilizer = g_ferilizer_mrg->get_ferilizer(p_rsp->item_id);
		if (!p_ferilizer) {
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}

		if (!p->CurHome->IsOwner(p)) {
			p->add_curhome_exp(2);
		} 

		if (p_ferilizer->output_per || p_ferilizer->speed_per) {
			if (p_plant) {
				p_plant->ferilizer_plant(p_ferilizer);
				db_add_home_log(p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, p,
					home_log_type_ferilizer, home_log_access_type_all, 0);
			}
		}
		if (p_ferilizer->home_output_per || p_ferilizer->home_speed_per) {
			p_summ_home->ferilizer_home(p_ferilizer);
			db_add_home_log(p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, p,
					home_log_type_ferilizer, home_log_access_type_all, 0);
		}
	}
	return send_set_plant_status_rsp(p, p_rsp->field_id, p_rsp->plant_id, p_rsp->plant_tm,
				p_rsp->type, p_rsp->item_id);
}
int db_get_plants_list(Player* p, uint32_t uid, uint32_t utm)
{
	return send_request_to_db(p, uid, utm, dbproto_get_plants_list, 0, 0);
}

int db_get_plants_list_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	if (!(p->CurHome) || !(p->CurHome->GetHomeDetail())) {
		p->waitcmd = 0;
		ERROR_LOG("%u get_plants_list", p->id);
		return 0;
	}
	
	if (!ret && !(p->CurHome->GetHomeDetail()->is_plants_db_updated())) {
		uint32_t cnt = *(uint32_t*)body;
		if (cnt) {
			plant_elem_t* p_elem = (plant_elem_t*)((uint32_t*)body + 1);
			for (uint32_t i = 0; i < cnt; i++) {
				p->CurHome->GetHomeDetail()->add_plant(p_elem, false);
				p_elem++;
			} 
			//p->CurHome->GetHomeDetail()->set_plants_db_updated();
		} 
		p->CurHome->GetHomeDetail()->set_plants_db_updated();
	} 

	if (p->waitcmd == home_get_plants_list) {
		int idx = sizeof(home_proto_t);
     	idx += sizeof(cli_proto_t);
		p->CurHome->GetHomeDetail()->pack_all_plants_info(pkgbuf, idx);
    	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
    	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_get_plants_list, 0, idx-sizeof(home_proto_t));
		return send_to_player(p, pkgbuf, idx, 1);
	}

	return 0;
}

int pick_fruit_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	struct pick_fruit_rsp_t {
		uint32_t field_id;
		uint32_t fruit_id;
	}__attribute__((packed));

	pick_fruit_rsp_t* p_rsp = (pick_fruit_rsp_t*)(body);
	if (p->CurHome && p->CurHome->IsSummerHome() && p->CurHome->GetHomeDetail()) {
		if (p->CurHome->GetHomeDetail()->pick_fruit(p, p_rsp->field_id, p_rsp->fruit_id)) {
//			if (!p->CurHome->IsOwner(p)) {
//				p->add_curhome_exp(3);
//			} 
			return 0;
		}	
	}

	return send_header_to_player(p, p->waitcmd, cli_err_cannot_pick, 1);

}

int db_pick_fruit(Player * p, Plant * p_plant, plant_t * plant_xml)
{
	int idx = 0;
	return	send_request_to_db(p, p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, dbproto_pick_fruit, dbpkgbuf, idx);
}

int db_pick_fruit_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	int idx = sizeof(home_proto_t);
	if (ret) {
		pack_h(dbpkgbuf, ret, idx);
		pack_h(dbpkgbuf, 0, idx);
		pack_h(dbpkgbuf, 0, idx);
		pack_h(dbpkgbuf, 0, idx);
	} else {
		taomee::pack_h(pkgbuf, ret, idx);
		taomee::pack(pkgbuf, body, bodylen, idx);
	}
	init_home_proto_head(pkgbuf, p->waitcmd, idx);
	DEBUG_LOG("PICK FRUIT! %u %u ret:%u", p->id, *(uint32_t*)body, ret);
	return send_to_player(p, pkgbuf, idx, 1); 
}

int sow_plant_seed_cmd(Player *p, uint8_t * body, uint32_t bodylen)
{
	return 0;
}

