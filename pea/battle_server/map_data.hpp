#ifndef MAP_DATA_HPP_
#define MAP_DATA_HPP_

#include <map>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pea_common.hpp"

using namespace taomee;
using namespace std;

enum
{
	no_file_type = 0,
	map_file_type,
	bullet_crater_file_type
};

class Object;

typedef struct map_data
{
	map_data(uint32_t count)
	{
		map_id = 0;
		grids = new char[count];
		grid_count = count;
		width = 0;
		height = 0;
		pixel_x = 0;
		pixel_y = 0;
	}
	~map_data()
	{
		map_id = 0;
		delete grids;
		grids = NULL;
		grid_count = 0;
		width = 0;
		height = 0;
		pixel_x = 0;
		pixel_y = 0;
	}

	map_data(const map_data& data)
	{
		map_id = data.map_id;
		grid_count = data.grid_count;
		width = data.width;
		height = data.height;
		pixel_x = data.pixel_x;
		pixel_y = data.pixel_y;
		grids = new char[grid_count];
		memcpy(grids, data.grids, grid_count);
	}

	bool check_valid_index(int32_t index)
	{
		return (index >= 0 && index < (int32_t)grid_count);
	}

	union
	{
		uint32_t map_id;
		uint32_t bullet_craters_id;
	};
	char     *grids;
	uint32_t width;
	uint32_t height;
	uint32_t pixel_x;
	uint32_t pixel_y;
	uint32_t grid_count;
}map_data;

typedef map_data bullet_crater_data;

class map_data_mgr
{
private:
	map_data_mgr(){}
	~map_data_mgr(){}
public:
	static map_data_mgr* getInstance();
	bool   init(const char* file_name, uint32_t type);
	bool   init_all(const char* dir_name);
	bool   final();
public:
	bool   is_map_data_exist(uint32_t map_id);
	bool   add_map_data(map_data* data);
	bool   del_map_data(uint32_t map_id);
	map_data* get_map_data(uint32_t map_id);
	int32_t gen_random_map_id();
public:
	bool   is_bullet_crater_data_exist(uint32_t bullet_crater_id);
	bool   add_bullet_crater_data(bullet_crater_data* data);
	bool   del_bullet_crater_data(uint32_t bullet_crater_id);
	bullet_crater_data*  get_bullet_crater_data_by_id(uint32_t bullet_crater_id);
private:
	std::map<uint32_t, map_data*> map_desc_datas;
	std::map<uint32_t, bullet_crater_data*> bullet_crater_desc_datas;
	std::vector<uint32_t>         map_ids; 
};

bool is_file(const char* name);

bool out_of_area(map_data* data, Object* obj);

bool collision_area(map_data* data, Object* obj, void* para);

bool collision_area_redress(map_data* data, Object* obj, void* para);

bool merge_area(bullet_crater_data* data,  uint32_t x, uint32_t y,  map_data* map);

int32_t get_index_from_pos(map_data* map, uint32_t x, uint32_t y);

bool   check_map_data_fall_down(map_data* map, uint32_t begin_x, uint32_t end_x, uint32_t y);
#endif
