#include "map_data.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


extern "C"
{
#include <glib.h>
#include <assert.h>
#include <libtaomee/timer.h>
#include <libtaomee/tlog/tlog.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/log.h>

}
#include "object.hpp"
#include "battle_impl.hpp"
#include "pea_common.hpp"


static int get_map_id_from_name(const char* file_name);
static int get_bullet_crater_id_from_name(const char* file_name);
static int get_type_from_dir_name(const char* dir_name);


int get_type_from_dir_name(const char* dir_name)
{
	if(dir_name == NULL)return no_file_type;
	if(strstr(dir_name, "maps") != NULL) return map_file_type;
	if(strstr(dir_name, "bullet_craters") != NULL) return bullet_crater_file_type;
	return no_file_type;
}

int get_map_id_from_name(const char* file_name)
{
	if(file_name == NULL)return -1;
	int map_id = -1;
	sscanf(file_name, "./maps/%u.map", &map_id);
	return map_id;
}

int get_bullet_crater_id_from_name(const char* file_name)
{
	if(file_name == NULL)return -1;
	int bullet_crater_id = -1;
	sscanf(file_name, "./bullet_craters/%u.bullet_crater", &bullet_crater_id);
	return bullet_crater_id;
}

bool is_file(const char* name)
{
	struct stat st;
	lstat(name, &st);
	return S_ISREG(st.st_mode);
}
/////////////////////////////////////////////////////////////////

map_data_mgr* map_data_mgr::getInstance()
{
	static map_data_mgr obj;
	return &obj;
}

bool  map_data_mgr::init_all(const char* dir_name)
{
	DIR* dir = NULL;
	struct dirent* dirp = NULL;

	dir = opendir(dir_name);
	if(dir == NULL)
	{
		return false;	
	}

	while( (dirp = readdir(dir)) != NULL)
	{
		if( strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)continue;
        if (dirp->d_name[0] == '.') continue;
		char path_name[1024] = {0};
		strcpy(path_name, dir_name);
		strcat(path_name, dirp->d_name);
		if( !is_file(path_name))continue;

		TRACE_TLOG("load map file name = %s", path_name);
		int type = get_type_from_dir_name(path_name);
		init(path_name, type);
	}
	return true;
}

bool   map_data_mgr::init(const char* file_name, uint32_t type)
{
	FILE* fp = fopen(file_name, "rb");
	if(fp == NULL)
	{
		ERROR_TLOG("map_data_mgr::init fail file_name =%s", file_name);
		fclose(fp);
		return false;
	}

	int id = -1;

	if(type == map_file_type)
	{
		id = get_map_id_from_name(file_name);	
	}
	else if(type == bullet_crater_file_type)
	{
		id = get_bullet_crater_id_from_name(file_name);
	}
		
	if(id == -1)
	{
		ERROR_TLOG("map_data_mgr::init fail invalid_file_name =%s", file_name);	
		fclose(fp);
		return false;
	}

	if(type == map_file_type && is_map_data_exist(id))
	{
		ERROR_TLOG("map_data_mgr::init fail map_id = %u is exist", id);
		fclose(fp);
		return false;
	}

	if(type == bullet_crater_file_type && is_bullet_crater_data_exist(id))
	{
		ERROR_TLOG("map_data_mgr::init fail bullet_crater_id = %u is exist", id);
		fclose(fp);
		return false;
	}

	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t pixel_x = 0;
	uint32_t pixel_y = 0;

	
	char buf[16] = {0};
	fread(buf, sizeof(char), 16, fp);
	
	width    = *((uint32_t*)buf);
	height   = *((uint32_t*)(buf+4));
	
	//pixel_x  = *((uint32_t*)(buf+8));
	//pixel_y  = *((uint32_t*)(buf+12));
	pixel_x = 1;
	pixel_y = 1;
	

	width    = width;
	height   = height;

	if(width == 0 || height == 0 || pixel_x == 0 || pixel_y == 0)
	{
		ERROR_TLOG("map_data_mgr::init fail width=%u, height=%u, pixel_x=%u, pixel_y=%u", width, height, pixel_x, pixel_y);
		fclose(fp);
		return false;
	}


	long int cur_pos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	long int end_pos = ftell(fp);
	int len = end_pos - cur_pos;

	fseek(fp, cur_pos, SEEK_SET);

	if((uint32_t)len != width* height)
	{
		ERROR_TLOG("map_data_mgr::init fail width = %u, height= %u, grid_count = %u", width, height, len);
		fclose(fp);
		return false;
	}

	map_data* data = new map_data(len);
	data->map_id = id;
	data->width = width;
	data->height = height;
	data->pixel_x = pixel_x;
	data->pixel_y = pixel_y;

	fread(data->grids, sizeof(char), len, fp);

	fclose(fp);

	if(type == map_file_type)
	{
		add_map_data(data);
	}
	if(type == bullet_crater_file_type)
	{
		add_bullet_crater_data(data);	
	}
	return true;
}

bool   map_data_mgr::final()
{
	std::map<uint32_t, map_data*>::iterator pItr = map_desc_datas.begin();
	for( ; pItr != map_desc_datas.end(); ++pItr)
	{
		map_data* data = pItr->second;
		delete data;
		data = NULL;
	}
	map_desc_datas.clear();

	std::map<uint32_t, bullet_crater_data*>::iterator pItr2 = bullet_crater_desc_datas.begin();
	for( ; pItr2 != bullet_crater_desc_datas.end(); ++pItr2 )
	{
		bullet_crater_data* data = pItr2->second;
		delete data;
		data = NULL;
	}
	bullet_crater_desc_datas.clear();
	
	return true;
}

bool   map_data_mgr::is_map_data_exist(uint32_t map_id)
{
	std::map<uint32_t, map_data*>::iterator pItr = map_desc_datas.find(map_id);
	return pItr != map_desc_datas.end();
}

bool   map_data_mgr::add_map_data(map_data* data)
{
	if( is_map_data_exist(data->map_id) )return false;
	map_desc_datas[data->map_id] = data;
	map_ids.push_back(data->map_id);
	DEBUG_LOG("add map data mid=%u gridcnt=%u pixel x y =%u %u", data->map_id, data->grid_count, data->pixel_x, data->pixel_y);
	return true;
}

bool   map_data_mgr::del_map_data(uint32_t map_id)
{
	std::map<uint32_t, map_data*>::iterator pItr = map_desc_datas.find(map_id); 
	if(pItr != map_desc_datas.end())
	{
		map_ids.erase( std::find(map_ids.begin(), map_ids.end(), map_id));
		map_data* data = pItr->second;
		map_desc_datas.erase(pItr);
		delete data;
		data = NULL;
		return true;
	}
	return false;
}

map_data* map_data_mgr::get_map_data(uint32_t map_id)
{
	std::map<uint32_t, map_data*>::iterator pItr = map_desc_datas.find(map_id);
	if(pItr == map_desc_datas.end())return NULL;
	return pItr->second;
}

bool   map_data_mgr::is_bullet_crater_data_exist(uint32_t bullet_crater_id)
{
	return bullet_crater_desc_datas.find(bullet_crater_id) != bullet_crater_desc_datas.end();	
}

bool   map_data_mgr::add_bullet_crater_data(bullet_crater_data* data)
{
	if(is_bullet_crater_data_exist(data->bullet_craters_id))return false;
	bullet_crater_desc_datas[ data->bullet_craters_id ] = data;
	DEBUG_LOG("add bullet crater data mid=%u gridcnt=%u pixel x y =%u %u", data->map_id, data->grid_count, data->pixel_x, data->pixel_y);
	return true;
}

bool   map_data_mgr::del_bullet_crater_data(uint32_t bullet_crater_id)
{
	std::map<uint32_t, bullet_crater_data*>::iterator pItr = bullet_crater_desc_datas.find(bullet_crater_id);
	if(pItr != bullet_crater_desc_datas.end())
	{
		bullet_crater_data* data = pItr->second;
		bullet_crater_desc_datas.erase(pItr);
		delete data;
		data = NULL;
		return true;
	}
	return false;
}

int32_t map_data_mgr::gen_random_map_id()
{
	uint32_t count = map_ids.size();
	if(count == 0)return -1;
	uint32_t index = ranged_random(0, count -1);
	return map_ids[index];
}


bullet_crater_data*  map_data_mgr::get_bullet_crater_data_by_id(uint32_t bullet_crater_id)
{
	std::map<uint32_t, bullet_crater_data*>::iterator pItr = bullet_crater_desc_datas.find(bullet_crater_id);
	if(pItr != bullet_crater_desc_datas.end())
	{
		return pItr->second;	
	}
	return NULL;
}

uint32_t get_x_index_from_pix_pos(map_data* map, uint32_t x)
{
	return x / map->pixel_x +  x % map->pixel_x;
}

uint32_t get_y_index_from_pix_pos(map_data* map, uint32_t y)
{
	return y / map->pixel_y +  y % map->pixel_y;
}

int32_t get_index_from_pos(map_data* map, uint32_t x, uint32_t y)
{
	if(  x > map->width * map->pixel_x &&  y > map->height* map->pixel_y )return -1;		
	
	uint32_t x_index = 	get_y_index_from_pix_pos(map, x);
	uint32_t y_index = get_y_index_from_pix_pos(map, y);
	int32_t  index = x_index + y_index * map->width;
	
	return index;
}

bool out_of_area(map_data* data, Object* obj)
{
	if( obj->get_y() > data->height * data->pixel_y )return true;
	
	if( obj->get_x() < 0 || obj->get_x() > data->width * data->pixel_x)return true;

	return false;	
}

bool collision_area_redress(map_data* data, Object* obj, void* para)
{
	Battle* btl = (Battle*)para;
	
	int32_t cur_simulate_time = obj->get_simulate_time();
	if(cur_simulate_time < 33.0)return false;
	

	double x = obj->get_x();
	double y = obj->get_y();

	double last_x = btl->container.calc_x_by_time(obj->get_phy_model(), cur_simulate_time - 33);
	double last_y = btl->container.calc_y_by_time(obj->get_phy_model(), cur_simulate_time - 33);

	double min_x = PEA_MIN(last_x, x);
	double max_x = PEA_MAX(last_x, x);
	
	for(double i = min_x; i<= max_x; i++)
	{
		double j = (y - last_y)/(x-last_x)*(i-last_x) + last_y;
		int32_t index = get_index_from_pos(data, i, j);
		if( !data->check_valid_index(index))return false;

		if( data->grids[index] != 0 )
		{
			obj->redress_x(i);
			obj->redress_y(j);
			ERROR_TLOG("--- redress x = %f, y= %f----", i, j);
			return true;
		}												        
	}
	/*
	for(int32_t i = cur_simulate_time - 1 ; i > 0; i--)
	{
		double x = 	btl->container.calc_x_by_time(obj->get_phy_model(), i);
		double y =  btl->container.calc_y_by_time(obj->get_phy_model(), i);
		
		int32_t index = get_index_from_pos(data, x, y);
	
		if( !data->check_valid_index(index))return false;

		if(index == -1)return false;

		if( data->grids[index] != 0 )
		{
			obj->redress_x(x);
			obj->redress_y(y);
			ERROR_TLOG("--- redress x = %f, y= %f, i = %u----", x, y, i);
			continue;
		}
	}
	*/
	return false;
}

bool collision_area(map_data* data, Object* obj, void* para)
{	
	ERROR_TLOG("-----------collision_area-----------------------");
	ERROR_TLOG("bullet pos = %f %f", obj->get_x(), obj->get_y());
	

	double x = obj->get_x();
	double y = obj->get_y();

	int32_t index = get_index_from_pos(data, x, y);

	if( !data->check_valid_index(index))return false;

	if(index == -1)return false;

	ERROR_TLOG("---------check ----------index = %d, x = %d, y = %d, grids = %d-------------",
			  index, (int)x, (int)y, (int)data->grids[index]);
	
	if( data->grids[index] != 0)
	{
		//坐标矫正，往前推33毫秒，计算33次，碰见碰撞点连续点，就做下矫正
		collision_area_redress(data, obj, para);
		return true;	
	}
	ERROR_TLOG("------------------------------------------------");
	return false;
}

bool  merge_area(bullet_crater_data* data,  uint32_t x, uint32_t y,  map_data* map)
{
	uint32_t left_top_x = x - data->width / 2;
	uint32_t left_top_y = y - data->height / 2;
	
	for(uint32_t i = left_top_x;  i < left_top_x + data->width; i++)
	{
		for(uint32_t j = left_top_y;  j< left_top_y + data->height; j++)
		{
			int map_index = get_index_from_pos(map, i, j);		
			if( !map->check_valid_index(map_index) )continue;
			
			int data_index = get_index_from_pos( data, i - left_top_x, j - left_top_y);
			if( !data->check_valid_index(data_index))continue;

			if(data->grids[data_index] == 0)continue;
			map->grids[map_index] = 0;
		}
	}
	YJ_DEBUG_LOG(" merge_area pix=(%u,%u) grid=(%u %u), \
		width = %u, height = %u left_top_x_y=[%u, %u] \
		pix_x,_y=[%u, %u]", 
		x, y, get_x_index_from_pix_pos(map, x), get_x_index_from_pix_pos(map, y), 
		data->width, data->height, left_top_x, left_top_y, 
		map->pixel_x, map->pixel_y);

	return true;
}

bool   check_map_data_fall_down(map_data* map, uint32_t begin_x, uint32_t end_x, uint32_t y)
{
	for(uint32_t i = begin_x; i<= end_x; i++)
	{
		int32_t index  = get_index_from_pos(map, i, y );	
		if( !map->check_valid_index(index))continue;	
		if(map->grids[index] != 0)return false;
	}
	return true;
}
