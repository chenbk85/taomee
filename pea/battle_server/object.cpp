#include "object.hpp"
#include "battle_manager.hpp"

uint32_t Object::s_next_id_ = base_guest_id;



void update_all_objects()
{
	struct timeval cur_time = *get_now_tv();
	
	battle_mgr::get_instance()->on_battle_mgr_time(cur_time);	
}
