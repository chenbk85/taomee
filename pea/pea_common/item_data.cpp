#include <cstring>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "item_data.hpp"

extern "C"
{
#include <glib.h>
#include <assert.h>
#include <libtaomee/tlog/tlog.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
}


#include "pea_utils.hpp"


using namespace taomee;


item_data_mgr* item_data_mgr::get_instance()
{
	static item_data_mgr obj;
	return &obj;
}

bool item_data_mgr::init_xml(const char* xml)
{
	xmlDocPtr   doc = NULL;
	xmlNodePtr  root = NULL;
	xmlNodePtr  item_node = NULL;


	char err_msg[4096] = {0};

	doc = xmlParseFile(xml);
	if(doc == NULL)
	{
		sprintf(err_msg, "xml file %s is not exist", xml);
		throw XmlParseError(err_msg);
		return false;
	}

	root = xmlDocGetRootElement(doc);
	if(root == NULL)
	{
		xmlFreeDoc(doc);
		sprintf(err_msg, "xml file %s content is empty", xml);
		throw XmlParseError(err_msg);
		return false;
	}
	
	item_node = root->xmlChildrenNode;
	while(item_node)
	{
		if(xmlStrcmp(item_node->name, reinterpret_cast<const xmlChar*>("item")) == 0)
		{
			item_data* p_data = new item_data();
			get_xml_prop_def(p_data->item_id_, item_node, "item_id", 0);
			get_xml_prop_def(p_data->category_, item_node, "category", 0);
			get_xml_prop_def(p_data->sell_price_, item_node, "sell_price", 0);
			get_xml_prop_def(p_data->buy_price_, item_node, "buy_price", 0);
			get_xml_prop_def(p_data->discard_flag_, item_node, "discard_flag", 0);
			get_xml_prop_def(p_data->trade_flag_, item_node, "trade_flag", 0);
			get_xml_prop_def(p_data->max_heap_num_, item_node, "max_heap_num", 0);
			get_xml_prop_def(p_data->max_num_, item_node, "max_num", 0);
			get_xml_prop_def(p_data->equip_pos_, item_node, "equip_pos", 0);
			get_xml_prop_def(p_data->duration_time_, item_node, "duration_time", 0);
			char timestr[1024] = {0};
			get_xml_prop_raw_str_def(timestr, item_node, "end_time", "");
			p_data->end_time_ = str_2_unix_time(timestr);

			get_xml_prop_def(p_data->magic_, item_node, "magic", 0);
			get_xml_prop_def(p_data->agility_, item_node, "agility", 0);
			get_xml_prop_def(p_data->luck_, item_node, "luck", 0);
			get_xml_prop_def(p_data->strength_, item_node, "strength", 0);
			get_xml_prop_def(p_data->crit_, item_node, "crit", 0);
			get_xml_prop_def(p_data->crit_damage_, item_node, "crit_damage", 0);
			get_xml_prop_def(p_data->tenacity_, item_node, "tenacity", 0);
			get_xml_prop_def(p_data->tenacity_, item_node, "hp", 0);
			get_xml_prop_def(p_data->tenacity_, item_node, "atk", 0);
			get_xml_prop_def(p_data->tenacity_, item_node, "def", 0);

            get_xml_prop_def(p_data->effect.effect_id, item_node, "fun_id", 0);
            get_xml_prop_arr_def(p_data->effect.effect_value, item_node, "fun_value", 0);


			if(p_data->item_id_ == 0)
			{
				xmlFreeDoc(doc);
				sprintf(err_msg, "invalid item_id = %u", p_data->item_id_);
				throw XmlParseError(err_msg);
				return false;
			}

			if( is_item_data_exist(p_data->item_id_) )
			{
				xmlFreeDoc(doc);
				sprintf(err_msg, "item_id = %u is exist in file %s", p_data->item_id_, xml);
				throw XmlParseError(err_msg);
				return false;
			}
			add_item_data(p_data);
		}
		item_node = item_node->next;	
	}
	
	xmlCleanupParser();
	xmlFreeDoc(doc);
	return true;
}

bool item_data_mgr::final()
{
	std::map<uint32_t, item_data*>::iterator pItr = data_maps.begin();
	for(; pItr != data_maps.end(); ++pItr)
	{
		item_data* data = pItr->second;
		delete data;
		data = NULL;
	}
	return true;
}

bool item_data_mgr::add_item_data(item_data* data)
{
	if( is_item_data_exist(data->item_id_))return false;
	data_maps[data->item_id_] = data;
	return true;
}

bool item_data_mgr::del_item_data(uint32_t item_id)
{
	std::map<uint32_t, item_data*>::iterator pItr = data_maps.find(item_id);
	if(pItr == data_maps.end())return false;
	item_data* data = pItr->second;
	delete data;
	data = NULL;
	data_maps.erase(pItr);
	return true;
}

bool item_data_mgr::is_item_data_exist(uint32_t item_id)
{
	std::map<uint32_t, item_data*>::iterator pItr = data_maps.find(item_id);
	return pItr != data_maps.end();
}

item_data* item_data_mgr::get_item_data_by_id(uint32_t item_id)
{
	std::map<uint32_t, item_data*>::iterator pItr = data_maps.find(item_id);
	if( pItr == data_maps.end() )return NULL;
	item_data* data= pItr->second;
	return data;
}
/*-----------------------------------------------------------------------------------*/
item::item(uint32_t id, uint32_t item_id,  uint32_t heap_count, uint32_t index, uint32_t get_time)
{
	id_ = id;
	cur_heap_count_ = heap_count;
	bag_index_ = index;
	get_time_  = get_time;
	p_data = item_data_mgr::get_instance()->get_item_data_by_id(item_id);
	p_next_item = NULL;
	p_prev_item = NULL;
}

item::~item()
{
	memset(this, 0, sizeof(*this));
}


/*-----------------------------------------------------------------------------------*/

item_factory* item_factory::get_instance()
{
	static item_factory obj;
	return &obj;
}

item* item_factory::create_item(uint32_t id, uint32_t item_id, uint32_t heap_count, uint32_t index, uint32_t get_time, uint32_t expire_time)
{
	item* p_item = new item(id, item_id, heap_count, index, get_time);
	p_item->set_expire_time(expire_time);
	return p_item;
}

void item_factory::destroy_item(item* p_item)
{
	if(p_item)delete p_item;
}




uint32_t str_2_unix_time(const char* timestr)
{
	if(timestr == NULL)return 0;
	if(strlen(timestr) != 12)return 0;

	uint32_t year = 0;
	uint32_t month = 0;
	uint32_t day = 0;
	uint32_t hour = 0;
	uint32_t minute = 0;
	uint32_t second = 0;
	
	int ret = sscanf(timestr, "%4d%2d%2d%2d%2d%2d", &year, &month, &day, &hour, &minute, &second);
	if(ret != 5)return  0;
	
	struct tm  tmp;
	tmp.tm_year = year-1900;
	tmp.tm_mon  = month - 1;
	tmp.tm_mday = day;
	tmp.tm_hour = hour;
	tmp.tm_min = minute;
	tmp.tm_sec = second;

	return mktime(&tmp);
}



