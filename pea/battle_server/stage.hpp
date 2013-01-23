#ifndef _STAGE_HPP_
#define _STAGE_HPP_
#include <vector>
#include <map>
#include <libtaomee++/random/random.hpp>
#include "pea_common.hpp"

using namespace taomee;
using namespace std;

typedef struct stage_data
{
	stage_data()
	{
		map_id = 0;	
	}

	pos*   get_rand_red_pos()
	{
		uint32_t count = red_pos.size();
		if(count == 0)return NULL;

		uint32_t index = ranged_random(0, count-1);
		return &red_pos[index];
	}
	pos*   get_rand_blue_pos()
	{
		uint32_t count = blue_pos.size();
		if(count == 0)return NULL;

		uint32_t index = ranged_random(0, count-1);
		return &blue_pos[index];
	}


	uint32_t map_id;
	vector<pos>  red_pos;
	vector<pos>  blue_pos;
}stage_data;


class stage_data_mgr
{
private:
	stage_data_mgr(){}
	~stage_data_mgr(){}
public:
	static stage_data_mgr* get_instance();
public:
	bool init_xml(const char* xml);
	bool init_all_xmls(const char* dir_name);
	bool final();
public:
	bool is_stage_data_exist(uint32_t map_id);
	stage_data* get_stage_data_by_id(uint32_t map_id);
	bool add_stage_data(stage_data* data);
private:
	map<uint32_t, stage_data*>  datas;
};














#endif
