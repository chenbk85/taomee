#include "vip_config_data_mgr.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
using namespace taomee;

bool vip_config_data_mgr::init()
{
	add_vip_config_data(1, 12, 25, 0);
	add_vip_config_data(2, 12, 25, 30);
	add_vip_config_data(3, 24, 50, 60);
	add_vip_config_data(4, 36, 75, 61);	
	add_vip_config_data(5, 48, 100 ,304);
	add_vip_config_data(6, 60, 125, 304);
	add_vip_config_data(7, 72, 150, 304);
	return true;
}


bool vip_config_data_mgr::init(const char* xml_name)
{
	if(xml_name == NULL){
		return false;
	}
	xmlDocPtr  doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr vip_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml_name);
	if(doc == NULL){
		return false;
	}
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		return false;
	}
	bool ret = true;
	vip_node = root->xmlChildrenNode;

	while(vip_node)
	{
		uint32_t level = 0;
		uint32_t warehouse_grid_count  = 0;
		uint32_t item_bag_grid_count = 0;
		uint32_t vip_point = 0;

		get_xml_prop_def(level, vip_node, "level", 0);
		get_xml_prop_def(warehouse_grid_count, vip_node, "warehouse_count", 0);
		get_xml_prop_def(item_bag_grid_count, vip_node, "item_bag_count", 0);
		get_xml_prop_def(vip_point, vip_node, "vip_point", 0);

		ret = add_vip_config_data(level,  warehouse_grid_count,  item_bag_grid_count, vip_point);
		if(ret == false){
			throw XmlParseError(std::string("level has existed!"));
			return ret;
		}
		vip_node = vip_node->next;
	}


	xmlFreeDoc(doc);
	xmlCleanupParser();
	return ret;
}

bool vip_config_data_mgr::final()
{
	m_data_maps.clear();
	return true;
}

uint32_t  calc_player_vip_level(vip_config_data_mgr* mgr, uint32_t vip_point, uint32_t* show_point)
{
	*show_point = 0;
	if(vip_point  <= 0){
		return 1;
	}
	uint32_t level = 1;
	uint32_t next_point = mgr->get_vip_point_by_level(level + 1);
	uint32_t total_point = vip_point;
	while(total_point >= next_point)
	{
		level ++;
		total_point -= next_point;
		next_point = mgr->get_vip_point_by_level(level + 1);
		if(level == MAX_VIP_LEVEL){
			total_point = 0;
			break;
		}
	}
	*show_point = total_point;
	return level;	
}

/**
 * @brief calculate vip level
 * @param begin_tm: vip begin time
 * @param current_tm: system current time
 * @param vip_type: 1:vip 2:timeout vip 3:vip 9: year vip
 */
uint32_t  calc_player_vip_level(vip_config_data_mgr* mgr, uint32_t begin_tm, uint32_t current_tm,  uint32_t vip_point, uint32_t vip_type = 3)
{
	if(begin_tm > current_tm )return 0;
    if (vip_type == 0) return 0;
	uint32_t extern_point = (current_tm - begin_tm ) / (60*60*24);	
    if (vip_type == 9) {
        extern_point = extern_point * 3;
    }
	uint32_t show_point = 0;
	return calc_player_vip_level(mgr,  vip_point + extern_point, &show_point);
}
