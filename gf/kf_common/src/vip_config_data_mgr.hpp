#ifndef _VIP_CONFIG_DATA_MGR_H_
#define _VIP_CONFIG_DATA_MGR_H_
#include <map>
#include <stdint.h>

#define MAX_VIP_LEVEL 7

typedef struct vip_config_data
{
		vip_config_data()
		{
			add_warehouse_grid_count = 0;
			add_item_bag_grid_count = 0;
			vip_level = 0;
			vip_point = 0;										    
		}
		uint32_t   vip_level;
		uint32_t   add_warehouse_grid_count;
		uint32_t   add_item_bag_grid_count;
		uint32_t   vip_point;
}vip_config_data;

class vip_config_data_mgr
{
public:
	    vip_config_data_mgr(){}
		~vip_config_data_mgr(){}
public:
	    /*
		static vip_config_data_mgr* getInstance()
		{
			static vip_config_data_mgr obj;
			return &obj;									    
		}
		*/
public:
		bool  init(const char* xml_name);
		bool  init();
		bool  final();

		uint32_t get_vip_config_data_count()
		{
			return m_data_maps.size();
		}
		bool add_vip_config_data(uint32_t level,  uint32_t warehouse_grid_count, uint32_t item_bag_grid_count, uint32_t vip_point)
		{
			if( is_vip_config_data_exist(level)){
				return false;
			}
			vip_config_data data;
			data.vip_level = level;
			data.add_item_bag_grid_count =  item_bag_grid_count;
			data.add_warehouse_grid_count = warehouse_grid_count;
			data.vip_point = vip_point;
			m_data_maps[level] = data;
			return true;
		}

		bool del_vip_config_data(uint32_t level)
		{
			std::map<uint32_t,  vip_config_data>::iterator pItr = m_data_maps.find(level);
			if(pItr == m_data_maps.end()){
				return false;
			}
			m_data_maps.erase(pItr);
			return true;
		}
		uint32_t  get_vip_point_by_level(uint32_t level)
		{
			std::map<uint32_t,  vip_config_data>::iterator pItr = m_data_maps.find(level);
			if(pItr == m_data_maps.end()){
				return 0;
			}
			return pItr->second.vip_point;
		}

		uint32_t  get_add_warehouse_count_by_level(uint32_t level)
		{
			std::map<uint32_t,  vip_config_data>::iterator pItr = m_data_maps.find(level);
			if(pItr == m_data_maps.end()){
				return 0;
			}
			return pItr->second.add_warehouse_grid_count;
		}
		uint32_t  get_add_item_bag_count_by_level(uint32_t level)
		{
			std::map<uint32_t,  vip_config_data>::iterator pItr = m_data_maps.find(level);
			if(pItr == m_data_maps.end()){
				return 0;
			}
			return pItr->second.add_item_bag_grid_count;
		}

		bool  is_vip_config_data_exist(uint32_t level)
		{
			return m_data_maps.find(level) !=  m_data_maps.end();
		}
private:
		std::map<uint32_t,  vip_config_data> m_data_maps;

};



uint32_t  calc_player_vip_level(vip_config_data_mgr* mgr,  uint32_t vip_point, uint32_t* show_point);

uint32_t  calc_player_vip_level(vip_config_data_mgr* mgr, uint32_t begin_tm, uint32_t current_tm,  uint32_t vip_point, uint32_t vip_type);
#endif
