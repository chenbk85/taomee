/*
 * =====================================================================================
 *
 *       Filename:  home_impl.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/04/2011 06:35:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus (), plus@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
extern "C" {
//#include <libtaomee/project/utilities.h>
}
#include "home_attr.hpp"
#include "home.hpp"
#include "home_impl.hpp"
#include "plant.hpp"
#include "decorate.hpp"

SummonHome::SummonHome(Home* p_home)
{
	updated_flg = false;
	tm_flag = 0;
	summon_cnt = 0;
	memset(homesummon, 0, sizeof(summon_t) * max_summon_num );
	p_base_home = p_home;
	Plants_ = std::vector<Plant*>(8, reinterpret_cast<Plant*>(0));

    p_decorate = new HomeDecorate();
}

SummonHome::~SummonHome()
{
    delete p_decorate;
}

void SummonHome::PlayerEnter(Player * p) 
{

	if (is_valid_uid(p->id)) {
     	//int idx = sizeof(home_proto_t);
     //	idx += sizeof(cli_proto_t);
	//	p->CurHome->pack_all_home_player_info(pkgbuf, idx);
    //	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
    //	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_list_user, 0, idx-sizeof(home_proto_t));
	//	send_to_player(p, pkgbuf, idx, 0);

		if (p->id == p->CurHome->OwnerUserId && 
			p->role_tm == p->CurHome->OwnerRoleTime) { //更新小屋内的主人灵兽
			this->summon_cnt = p->summon_cnt;
			memset(this->homesummon, 0, max_summon_num * sizeof(summon_t));
			memcpy(this->homesummon, p->allsummon, max_summon_num * sizeof(summon_t));
		}

	    //宠物主人不在打包发送主人的灵兽信息
		//if (p->CurHome && !p->CurHome->Owner) { 
	//	if (true) {
	//	   int idx = sizeof(home_proto_t);
     //      idx += sizeof(cli_proto_t);
	//	   p->CurHome->pack_all_home_pet_info(pkgbuf, idx);
    //	   init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
    //	   init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_list_home_pet, 0, idx-sizeof(home_proto_t));
	//	   send_to_player(p, pkgbuf, idx, 0);
	//	}

	    Players_.insert(PlayerMap::value_type(p->id, p));

		//ERROR_LOG("HAS %d player in this home after %u enter!", (int)(Players_.size()), p->id);

	} 

}

void SummonHome::syn_pet_info(Player *p)
{
	if (p == p->CurHome->Owner) { //更新小屋内的主人灵兽
		this->summon_cnt = p->summon_cnt;
		memset(this->homesummon, 0, max_summon_num * sizeof(summon_t));
		memcpy(this->homesummon, p->allsummon, max_summon_num * sizeof(summon_t));
	}
}

void SummonHome::PlayerLeave(Player *p)
{
	PlayerMap::iterator iter = Players_.find(p->id);
	if (iter != Players_.end()) {
		Players_.erase(iter);
	}
	//ERROR_LOG("HAS %d player in this home after %u leave!", (int)(Players_.size()), p->id);

}

Plant * SummonHome::add_plant(plant_elem_t * plant_in, bool notify)
{
	Plant * p_plant = new Plant(this, plant_in);

	//Plants_.insert(PlantMap::value_type(plant_in->field, p_plant));
	Plants_[plant_in->field - 1] = p_plant;

	if (notify) {
		p_plant->notify_plant_status_changed();
	}
	return p_plant;
}	

void SummonHome::clear_plant()
{
	std::vector<Plant*>::iterator it = Plants_.begin();
		
	for (; it != Plants_.end(); ++it) {
		if (*it) {
			Plant * t_p = *it;
			delete t_p;
		}
	}
	Plants_.clear();
}


void SummonHome::pack_all_plants_info(uint8_t * pkg, int & idx)
{
	
	int plants_cnt = 0;
//	pack(pkg, plants_cnt, idx);
	int ifx = idx + 4;
	std::vector<Plant*>::iterator it = Plants_.begin();
	for (; it != Plants_.end(); ++it) {
		Plant* p_plant = *it;
		if (p_plant) {
			p_plant->pack_plant_info(pkg, ifx);
			plants_cnt ++;
		}
	}
	pack(pkg, plants_cnt, idx);
	idx = ifx;
}

Plant* SummonHome::get_plant(uint32_t field_id)
{
//	PlantMap::iterator it = Plants_.find(field_id);
//	if (it != Plants_.end()) {
//		Plant *p_plant = it->second;
//		return p_plant;
//	}
//	
	return Plants_[field_id - 1];
}


void SummonHome::set_plant_status(Player* p, uint32_t field_id, uint32_t status)
{
	
//	PlantMap::iterator it = Plants_.find(field_id);
//	if (it != Plants_.end()) {
//		Plant *p_plant = it->second;
//		if (p_plant->field_id() == field_id && p_plant->is_cur_status_user_active()) {
//			p_plant->change_grow_status(status);
//		}
//	}
}

bool SummonHome::pick_fruit(Player* p, uint32_t field_id, uint32_t fruit_id)
{
	if (Plants_[field_id - 1]) {
		Plant * p_plant = Plants_[field_id - 1];
		if (fruit_id != p_plant->fruit()) {//果实ID 不对
			return false;
		}
		if (p_base_home && p_plant->can_pick_fruit(p)) {
			plant_t * p_xml = g_plants->get_plant(p_plant->plant_id());

			if (p_xml->e_item && p_xml->d_odds) { //采集额外产物
				uint32_t x = rand() % 1000;
				if (x < p_xml->d_odds) {
					int idx = sizeof(home_proto_t);
					taomee::pack_h(pkgbuf, p_plant->field_id(), idx);
					taomee::pack_h(pkgbuf, p_xml->e_item, idx);
					uint32_t default_cnt = 1;
					taomee::pack_h(pkgbuf, default_cnt, idx);
					init_home_proto_head(pkgbuf, p->waitcmd, idx);
					send_to_player(p, pkgbuf, idx, 1); 
					return true;
				}
			}

			int idx = sizeof(home_proto_t);
			taomee::pack_h(pkgbuf, p_plant->field_id(), idx);
			taomee::pack_h(pkgbuf, p_plant->fruit(), idx);
			uint32_t default_cnt = 1;
			if (p->CurHome->IsOwner(p)) {
				default_cnt = p_plant->get_fruit_cnt();
			}
			taomee::pack_h(pkgbuf, default_cnt, idx);
			ERROR_LOG("USER PICK ID CNT %u ", default_cnt);
			init_home_proto_head(pkgbuf, p->waitcmd, idx);
			send_to_player(p, pkgbuf, idx, 1); 
			//	after picked_cmd	
			p_plant->be_picked(default_cnt); //更新果实数量 和状态
			if (!p_plant->fruit_cnt) {
				delete p_plant;
				Plants_[field_id - 1] = NULL;
			}
			db_add_home_log(p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, p,
				home_log_type_steal, home_log_access_type_all, 0);
			return true;
		}
	} 
	return false;
}

bool SummonHome::sow_seed(uint32_t field_id, plant_t * p_in)
{
//	PlantMap::iterator it = Plants_.find(field_id);
//	if (it != Plants_.end()) { //已经有植物了
//		return false;
//	} 
//
	plant_elem_t elem(field_id, p_in->id);
	this->add_plant(&elem, true); 
//
	return true;
}


void SummonHome::ferilizer_home(Ferilizer_t* p_ferilizer)
{
//	PlantMap::iterator it = Plants_.begin();
//	for (; it != Plants_.end(); ++it) {
//		Plant* p_plant = it->second;
//		p_plant->ferilizer_plant(p_ferilizer->home_speed_per, p_ferilizer->home_output_per);
//	}
//	db_add_effect_to_all_plants(p_ferilizer);
}

void SummonHome::db_add_effect_to_all_plants(Ferilizer_t* p_ferilizer)
{
	uint32_t ownerid = p_base_home->OwnerUserId;
	uint32_t ownertm = p_base_home->OwnerRoleTime;

	int idx = 0;
	pack_h(dbpkgbuf, p_ferilizer->home_speed_per, idx);
	pack_h(dbpkgbuf, p_ferilizer->home_output_per, idx);
		
	send_request_to_db(0, ownerid, ownertm, dbproto_add_effect_to_all_plants, dbpkgbuf, idx);
}

void SummonHome::Update()
{
    uint32_t now = get_now_tv()->tv_sec;
	if (now - tm_flag > 1) {
		tm_flag = now;

		for (std::vector<Plant*>::iterator it = Plants_.begin(); 	
				it != Plants_.end(); ++it) {
			if (*it) {
				(*it)->update(0);
			}
		}
	}

	if (is_plants_db_updated() 
			&& home_data.last_randseed_tm 
			&& now - this->home_data.last_randseed_tm > 100) {
		
		int times = (now - this->home_data.last_randseed_tm) / 1800;
		if (times > 5) {
			times = 5;
		}
		this->home_data.last_randseed_tm = now;
		RandSowSeed(times);

		db_set_home_update_tm(p_base_home->OwnerUserId, p_base_home->OwnerRoleTime,
				home_data.last_randseed_tm);
	}
}

void SummonHome::RandSowSeed(uint32_t odds_times)
{
	static int randseedinfo[12] = {
		90025, 90026, 90027, 90028, 90029, 90030, 90031, 90032
	};

	static int odds_info[12] = {
		5, 4, 3, 2, 5, 4, 3, 2
	};

	for (int field = 0; field < 8; ++field) {
		//int rand_plant = 0;
		if (Plants_[field] == 0) {
			for (int i = 12; i > 0; i--) {
				uint32_t rand_val = rand() % 100;
				if (rand_val < odds_info[i] * odds_times) {
					plant_t * plant_info = g_plants->get_plant(randseedinfo[i]);
					if (plant_info->home_lv[0] <= this->get_home_level()) {
						sow_seed(field + 1, plant_info);
					//	ERROR_LOG("SOW SEED %u %u", (field + 1), plant_info->id);  
						break;
					}
				}
			}
		}
	}
}


