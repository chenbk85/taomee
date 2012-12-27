#include <libtaomee++/conf_parser/xmlparser.hpp>

extern "C" {
#include <dirent.h>
#include <libtaomee/log.h>
}

using namespace taomee;

#include "skill_additional_damage.hpp"

#define MAX_DATA_COUNT  400

skill_add_damage_mgr::skill_add_damage_mgr()
{
	 m_rabbit_data.resize(MAX_DATA_COUNT);
	 m_panda_data.resize(MAX_DATA_COUNT);
	 m_monkey_data.resize(MAX_DATA_COUNT);
	 m_drogon_data.resize(MAX_DATA_COUNT);
}


skill_add_damage_mgr::~skill_add_damage_mgr()
{

}

bool skill_add_damage_mgr::init()
{
	init(RABBIT);
	init(PANDA);
	init(MONKEY);
	init(DROGON);
	return true;
}

bool skill_add_damage_mgr::init(const char* xml_name)
{
	std::vector<additional_data>* pdata = get_data_by_name(xml_name);
	if(pdata == NULL)return false;
	
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr level_node = NULL;
	
	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml_name);
	if(doc == NULL){
		throw XmlParseError(std::string("the xml file is not exist"));
		return false;
	}
	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the xml file content is empty"));
		return false;
	}
	bool ret = true;

	level_node = root->xmlChildrenNode;

	while(level_node)
	{
		uint32_t lv = 0;	
		get_xml_prop_def(lv, level_node, "lv", 0);
		if( !(lv >= 1 && lv <= MAX_DATA_COUNT))
		{
			throw XmlParseError(std::string("invalid lv in level node"));
			goto ERR_LOGIC;
		}
		
		get_xml_prop_def( (*pdata)[lv - 1].damage[0], level_node, "damage1", 0);
		get_xml_prop_def( (*pdata)[lv - 1].damage[1], level_node, "damage2", 0);
		get_xml_prop_def( (*pdata)[lv - 1].damage[2], level_node, "damage3", 0);
		get_xml_prop_def( (*pdata)[lv - 1].damage[3], level_node, "damage4", 0);
		get_xml_prop_def( (*pdata)[lv - 1].damage[4], level_node, "damage5", 0);		
		

		level_node = level_node->next;
	}

ERR_LOGIC:
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return ret;
}

bool skill_add_damage_mgr::final()
{
	return true;
}

skill_add_damage_mgr*  skill_add_damage_mgr::getInstance()
{
	static skill_add_damage_mgr obj;
	return &obj;
}

std::vector<additional_data>* skill_add_damage_mgr::get_data_by_name(const char* name)
{
	if(name == NULL)return NULL;
	if( strcmp(name, RABBIT) == 0)return &m_rabbit_data;
	if( strcmp(name, PANDA) == 0)return &m_panda_data;
	if( strcmp(name, MONKEY) == 0)return &m_monkey_data;
	if( strcmp(name, DROGON) == 0)return &m_drogon_data;
	return NULL;
}

std::vector<additional_data>* skill_add_damage_mgr::get_data_by_role_type(uint32_t role_type)
{
	switch(role_type)
	{
		case 1:
			return &m_monkey_data;	
		break;

		case 2:
			return &m_rabbit_data;
		break;

		case 3:
			return &m_panda_data;
		break;

		case 4:
			return &m_drogon_data;
		break;

		default:
		 	return NULL;
	}
	return NULL;
}
uint32_t get_addition_damage_by_index( uint32_t role_type,  uint32_t add_type,  uint32_t add_start, uint32_t add_step, uint32_t skill_lv)
{
	if( !(role_type >= 1 && role_type <= 4))return 0;
	if( !(add_type >= 1  && add_type <=5)) return 0;
	if( !(skill_lv >= 1)) return 0;
	std::vector<additional_data>* p_data = skill_add_damage_mgr::getInstance()->get_data_by_role_type(role_type);	
	if(p_data == NULL) return 0;
	
	uint32_t index = add_start -1 + (skill_lv - 1)*add_step;
	if( index >= MAX_DATA_COUNT )	return 0;
	return (*p_data)[index].damage[add_type - 1];
}



