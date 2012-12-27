/*
 * =========================================================================
 *
 *        Filename: Cmap_conf.h
 *
 *        Version:  1.0
 *        Created:  2011-07-22 14:31:44
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#ifndef  CMAP_CONF_H
#define  CMAP_CONF_H
#include <stdint.h>
#include <map>
struct map_conf_t {
	uint32_t mapid;	
	uint32_t multi;	//多人模式
};

class Cisland_time {
	bool						is_open;
	uint32_t					next_state_time;
	std::map<uint32_t, bool>	time_map[7];
public:
	void add_time_map(uint32_t day, const std::map<uint32_t, uint32_t>& time_map);
	void reset_island_timer(uint32_t islandid);
	bool get_is_open() { return is_open; }
	uint32_t get_next_state_time();
};

#include  <vector>
class Cmap_conf_map {
	private:
	public:
		std::map<uint32_t, Cisland_time> island_time_map;
		//kindid , mapid, map_conf_t 
		std::map<uint32_t, std::map<uint32_t,  map_conf_t> > conf_map;
		void add(uint32_t islandid, map_conf_t & map_conf );
		bool check_multi(uint32_t islandid,uint32_t mapid )   ;
		bool check_valid(uint32_t islandid, uint32_t mapid);

		void clear_island_time() { island_time_map.clear(); }
		void add_island_time(uint32_t islandid, std::map<uint32_t, uint32_t>* time_map);
		void reset_island_timer(uint32_t islandid = uint32_t(-1));
		bool check_island_open(uint32_t islandid);
};



#endif  /*CMAP_CONF_H*/
