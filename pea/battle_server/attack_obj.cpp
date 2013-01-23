#include "attack_obj.hpp"


attack_obj::attack_obj(uint32_t skill_id, uint32_t skill_lv, uint32_t angle, uint32_t velocity, Player* p, Battle* p_btl)
{
	p_skill_data = skill_data_mgr::get_instance()->get_skill_data_by_id(skill_id, skill_lv);
	gun_angle = angle;
	muzzle_velocity = velocity;
	owner = p;
	btl   = p_btl;
	hit_result = false;
	remain_bullet_times = p_skill_data->bullet_times;
}

attack_obj::~attack_obj()
{
	
}


bool attack_obj::init_attack_obj()
{
	for(uint32_t i = 0; i< p_skill_data->bullet_count_per_time; i++)
	{
		uint32_t cur_angle = gun_angle - (p_skill_data->bullet_count_per_time /2 - i)* p_skill_data->bullet_excursion_angle;
		
		uint32_t syn_number = i + 1;
		uint32_t bullet_times = p_skill_data->bullet_times - remain_bullet_times + 1;

		bullet* p_bullet = create_bullet(p_skill_data, p_skill_data->bullet_id, owner->id, owner->team, syn_number, bullet_times );
		double x_speed = muzzle_velocity * cos(angle2radian(cur_angle));
		double y_speed = muzzle_velocity * sin(angle2radian(cur_angle));

		p_bullet->init_bullet_pos(owner->get_x(), owner->get_y(),  x_speed, y_speed, owner->get_dir());
		p_bullet->enable_simulate();
		YJ_DEBUG_LOG("init line: %.0lf, %.0lf", owner->get_x(), owner->get_y());
		btl->add_phy_model_obj(p_bullet->get_phy_model());
		bullet_list.push_back(p_bullet);

		ERROR_TLOG("bullet init pos = (%f, %f) speed = (%f %f) angle = %u, velocity = %u" , p_bullet->get_x(), p_bullet->get_y(), x_speed, y_speed, gun_angle, muzzle_velocity);
	}
	remain_bullet_times--;
	return true;
}


bool attack_obj::final_attack_obj()
{
	p_skill_data = NULL;
	owner = NULL;
	std::list<bullet*>::iterator pItr = bullet_list.begin();
	for(; pItr != bullet_list.end(); ++pItr)
	{
		bullet* p_bullet = *pItr;
		btl->del_phy_model_obj(p_bullet->get_phy_model());
		destroy_bullet(p_bullet);	
		p_bullet = NULL;
	}
	return true;
}


bool attack_obj::get_hit_result()
{
	return hit_result;	
}

void attack_obj::set_hit_result(bool result)
{
	hit_result = result;	
}


attack_obj* create_attack_obj(uint32_t skill_id, uint32_t skill_lv, uint32_t angle, uint32_t velocity, struct timeval  cur_time,  Player* p)
{
	attack_obj* p_obj = new attack_obj(skill_id, skill_lv, angle, velocity, p, p->btl);	
	return p_obj;
}

void        destroy_attack_obj(attack_obj* obj)
{
	obj->final_attack_obj();
	delete obj;
}


bool attack_obj::process_attack_obj(struct timeval cur_time,  uint32_t& process_time)
{
	process_time = 0;
	while( remain_bullet_times > 0)
	{
		init_attack_obj();

		while(bullet_list.size()) {
			std::list<bullet*>::iterator bullet_itr = bullet_list.begin();
			while( bullet_itr != bullet_list.end() )
			{
				bullet* p_bullet = *bullet_itr;
		
				btl->container.simulate_models(PHY_MODEL_SIMULATE_TIME, p_bullet->get_phy_model());
			
	            YJ_DEBUG_LOG("objid:%u fly line: %.0lf, %.0lf", p_bullet->id_, p_bullet->get_x(), p_bullet->get_y());
				if( out_of_area( btl->get_btl_map_data(), p_bullet))
				{
					ERROR_TLOG("bullet out of area pos = %f %f", p_bullet->get_x(), p_bullet->get_y());
					process_time = PEA_MAX(process_time, p_bullet->get_simulate_time());
					bullet_itr = bullet_list.erase(bullet_itr);
					btl->del_phy_model_obj(p_bullet->get_phy_model());
					destroy_bullet(p_bullet);
					p_bullet = NULL;
					continue;
				}

				bool ret = p_bullet->process_collision(btl->get_btl_map_data(),  (void*)btl);
				if(ret)
				{	
					ERROR_TLOG("bullet model = %p, pos = (%f, %f)", p_bullet->get_phy_model(), p_bullet->get_x(), p_bullet->get_y()); 
					ERROR_TLOG("1.delete bullet because hit envirment pos = %f, %f", p_bullet->get_x(), p_bullet->get_y());
						
					process_time = PEA_MAX(process_time, p_bullet->get_simulate_time());
					bullet_itr = bullet_list.erase(bullet_itr);
					btl->del_phy_model_obj(p_bullet->get_phy_model());
					destroy_bullet(p_bullet);
					p_bullet = NULL;
					continue;
				}
			
				std::list<Player*>::iterator player_itr = btl->playerlist.begin();
				for(; player_itr != btl->playerlist.end(); ++player_itr) 
				{ 
					Player* p = *player_itr; 
					if(p == owner)continue;
					if(p->is_dead())continue;

					ret = p_bullet->process_collision(p); 
					if(ret) 
					{
						set_hit_result(true);
					} 
				}

				if(get_hit_result())
				{
					if( p_bullet->merge_bullet_crater_to_map(btl->get_btl_map_data(), btl) )
					{
						btl->all_fall_down( p_bullet->syn_number_, p_bullet->bullet_times_);
					}
					ERROR_TLOG("bullet model = %p,pos = (%f, %f)", p_bullet->get_phy_model(), p_bullet->get_x(), p_bullet->get_y());
					ERROR_TLOG("2.delete bullet because hit player pos = %f, %f", p_bullet->get_x(), p_bullet->get_y());
					process_time = PEA_MAX(process_time, p_bullet->get_simulate_time());
					bullet_itr = bullet_list.erase(bullet_itr);
					btl->del_phy_model_obj(p_bullet->get_phy_model());
					destroy_bullet(p_bullet);
					p_bullet = NULL;	
					continue;
				}
				bullet_itr++;
			}
		}
	}

	return get_hit_result();
}

