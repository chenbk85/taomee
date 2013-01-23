extern "C"
{
#include <libtaomee/log.h>
}
#include <libtaomee++/conf_parser/xmlparser.hpp>


#include "skill.hpp"
#include "bullet.hpp"

using namespace taomee;


skill_data_mgr* skill_data_mgr::get_instance()
{
	static skill_data_mgr object;
	return &object;
}


bool   skill_data_mgr::init_xml(const char* xml)
{
	xmlDocPtr   doc = NULL;
	xmlNodePtr  root = NULL;
	xmlNodePtr  bullet_node = NULL;
	xmlNodePtr  skill_node = NULL;
	
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
		
	skill_node = root->xmlChildrenNode;
	while(skill_node)
	{
		if( xmlStrcmp(skill_node->name, reinterpret_cast<const xmlChar*>("skill")) == 0)
		{
			skill_data* p_data = new skill_data();		
	
			get_xml_prop_def(p_data->skill_id,  skill_node, "id", 0);
			get_xml_prop_def(p_data->skill_lv,  skill_node, "lv", 0);
			get_xml_prop_def(p_data->skill_type, skill_node, "type", 0);
			get_xml_prop_def(p_data->skill_consume_strength, skill_node, "consume_strength", 0);
			get_xml_prop_def(p_data->skill_consume_anger, skill_node, "consume_anger", 0);
			get_xml_prop_def(p_data->skill_cd_time, skill_node, "skill_cd_time", 0);

			if(p_data->skill_id == 0 || p_data->skill_lv == 0)
			{
				xmlFreeDoc(doc);
				sprintf(err_msg, "the skill_id = %u, skill_lv = %u", p_data->skill_id, p_data->skill_lv);
				throw XmlParseError(err_msg);
				return false;
			}

			if(is_skill_data_exist(p_data))
			{
				xmlFreeDoc(doc);
				sprintf(err_msg, "the skill_id = %u has existed", p_data->skill_id);
				throw XmlParseError(err_msg);
				return false;
			}
				
			bullet_node  = skill_node->xmlChildrenNode;
			while(bullet_node)
			{
				if( xmlStrcmp(bullet_node->name, reinterpret_cast<const xmlChar*>("bullet")) == 0)
				{	
					get_xml_prop_def(p_data->bullet_id, bullet_node, "id", 0);
					if( !bullet_mgr::get_instance()->is_bullet_data_exist(p_data->bullet_id))
					{
						sprintf(err_msg, "the bullet_id %u is not exist in skill %u", p_data->bullet_id, p_data->skill_id);
						throw XmlParseError(err_msg);
						return false;
					}
					get_xml_prop_def(p_data->bullet_times, bullet_node, "bullet_times", 0);
					get_xml_prop_def(p_data->bullet_count_per_time, bullet_node, "bullet_count_per_time", 0);
					get_xml_prop_def(p_data->bullet_excursion_angle, bullet_node, "bullet_excursion_angle", 0);
	    			get_xml_prop_def(p_data->skill_target, bullet_node, "target", 4);
		        	get_xml_prop_def(p_data->damage_percent, bullet_node, "damage_percent", 100);
        			get_xml_prop_def(p_data->recove_hp, bullet_node, "recove_hp", 0);
			        get_xml_prop_def(p_data->recove_hp_target, bullet_node, "recove_hp_target", 1);
			    }   
				bullet_node = bullet_node->next;
			}
			add_skill_data(p_data);
		}
		skill_node = skill_node->next;
	}
	
	xmlCleanupParser();
	xmlFreeDoc(doc);
	return true;	
}


bool   skill_data_mgr::final()
{
	std::map<uint32_t, skill_data*>::iterator pItr = data_maps.begin();
	for(; pItr != data_maps.end(); ++pItr)
	{
		skill_data* p_data = pItr->second;
		delete p_data;
		p_data = NULL;
	}
	return true;	
}

bool   skill_data_mgr::add_skill_data(skill_data* data)
{
	if(is_skill_data_exist(data))return false;
	YJ_DEBUG_LOG("add skill data %u %u %u", data->skill_id * SKILL_ODDS + data->skill_lv,
		data->skill_lv, data->skill_type);
	data_maps[ data->skill_id * SKILL_ODDS + data->skill_lv ] = data;
	return true;
}


skill_data* skill_data_mgr::get_skill_data_by_id(uint32_t skill_id, uint32_t skill_lv)
{
	uint32_t skill_index = skill_id * SKILL_ODDS + skill_lv;
	std::map<uint32_t, skill_data*>::iterator pItr = data_maps.find(skill_index);
	if(pItr == data_maps.end())return NULL;
	return pItr->second;
}
	    
bool   skill_data_mgr::is_skill_data_exist(skill_data* data)
{
	uint32_t skill_index = data->skill_id * SKILL_ODDS + data->skill_lv;
	return data_maps.find(skill_index) != data_maps.end();	
}

//-----------------------------------------------------------------------//


