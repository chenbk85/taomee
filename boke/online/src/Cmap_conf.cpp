/*
 * =========================================================================
 *
 *        Filename: Cmap_conf.cpp
 *
 *        Version:  1.0
 *        Created:  2011-07-22 14:36:46
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#include    "util.h"
#include    "Cmap_conf.h"
#include	"global.h"
#include 	"Ctimer.h"
#include 	"timer_func.h"

void Cmap_conf_map::add(uint32_t islandid, map_conf_t & map_conf )
{
	this->conf_map[islandid][map_conf.mapid ]=map_conf;
}

bool Cmap_conf_map::check_multi(uint32_t islandid,uint32_t mapid ) 
{
	std::map<uint32_t, std::map<uint32_t, map_conf_t> >::iterator it;
	it = this->conf_map.find(islandid);
	
	if (it == conf_map.end())
		return false;

	std::map<uint32_t, map_conf_t>::iterator it_map;
	it_map = it->second.find(mapid);
	if (it_map == it->second.end())
		return false;

	return it_map->second.multi == 1;
}

bool Cmap_conf_map::check_valid(uint32_t islandid, uint32_t mapid)
{
	std::map<uint32_t, std::map<uint32_t, map_conf_t> >::iterator it;
	it = this->conf_map.find(islandid);
	
	if (it == conf_map.end())
		return false;

	std::map<uint32_t, map_conf_t>::iterator it_map;
	it_map = it->second.find(mapid);
	if (it_map == it->second.end())
		return false;

	return true;
}
void Cmap_conf_map::add_island_time(uint32_t islandid, std::map<uint32_t, uint32_t>* time_map)
{
	Cisland_time& island_time = this->island_time_map[islandid];
	for (uint32_t day = 0; day < 7; day ++) {
		island_time.add_time_map(day, time_map[day]);
	}
}

void Cmap_conf_map::reset_island_timer(uint32_t islandid)
{
	std::map<uint32_t, Cisland_time>::iterator it;
	if (islandid == uint32_t(-1)) {
		g_timer_map->del_by_key(timer_key_map_time);
		for (it = island_time_map.begin(); it != island_time_map.end(); it ++) {
			it->second.reset_island_timer(it->first);
		}
	} else {
		it = island_time_map.find(islandid);
		if (it != island_time_map.end()) {
			it->second.reset_island_timer(it->first);
		}
	}
}
bool Cmap_conf_map::check_island_open(uint32_t islandid)
{
	std::map<uint32_t, Cisland_time>::iterator it;
	it = island_time_map.find(islandid);
	if (it == island_time_map.end())
		return true;
	return it->second.get_is_open();
}

void Cisland_time::reset_island_timer(uint32_t islandid)
{
	time_t tnow = time(NULL);
	struct tm tmnow;	
	localtime_r(&tnow, &tmnow);
	std::map<uint32_t, bool>& day_island = this->time_map[tmnow.tm_wday];
	uint32_t day_second = get_day_second(tnow);
	for (std::map<uint32_t, bool>::iterator it = day_island.begin();
			it != day_island.end(); it ++) {
	}
	std::map<uint32_t, bool>::iterator it = day_island.upper_bound(day_second);
	
	if (it == day_island.end()) {
		this->next_state_time = 24 * 3600;
	} else {
		this->next_state_time = it->first;
	}

	if (it == day_island.begin()) {
		this->is_open = false;
	} else {
		it --;
		this->is_open = it->second;
	}

	g_timer_map->add_timer(TimeVal(this->next_state_time - day_second), n_reset_island_time, timer_key_map_time, &islandid, 4);
}

void Cisland_time::add_time_map(uint32_t day, const std::map<uint32_t, uint32_t>& time_map)
{
	std::map<uint32_t, uint32_t>::const_iterator it;
	for (it = time_map.begin(); it != time_map.end(); it ++) {
		this->time_map[day][it->first] = true;
		this->time_map[day][it->second] = false;
	}
}

uint32_t Cisland_time::get_next_state_time()
{
	time_t tnow = time(NULL);
	if (this->next_state_time == 24 * 3600) {
		// 明天
		struct tm tmnow;	
		localtime_r(&tnow, &tmnow);

		int day = (tmnow.tm_wday + 1) % 7;
		for (int i = 0; i < 7; i ++) {
			if (this->time_map[day].size()) {
				std::map<uint32_t, bool>::iterator it;
				it = this->time_map[day].begin();
				return this->next_state_time - get_day_second(tnow) + it->first;
			}
			
			day = (day + 1) % 7;
		}
		return 24 * 3600;
	} else {
		return this->next_state_time - get_day_second(tnow);
	}
}
