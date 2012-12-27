#include "title_attr.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
using namespace taomee;

/*
title_attr_data_mgr* title_attr_data_mgr::getInstance()
{
	static  title_attr_data_mgr obj;
	return &obj;
}
*/
bool title_attr_data_mgr::is_title_id_exist(uint32_t id)
{
	std::map<uint32_t, title_attr_data*>::iterator pItr = datas.find(id);
	if(pItr != datas.end())return true;
	return false;
}

bool title_attr_data_mgr::add_title_attr_data(title_attr_data* p_data)
{
	if( is_title_id_exist(p_data->id))return false;
	datas[ p_data->id ] = p_data;
	return true;
}

title_attr_data* title_attr_data_mgr::get_title_attr_data(uint32_t id)
{
	std::map<uint32_t, title_attr_data*>::iterator pItr = datas.find(id);
	if(pItr == datas.end())return NULL;
	return pItr->second;
}

bool title_attr_data_mgr::init(const char* xml)
{
	if(xml == NULL){
		return false;
	}

	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr title_node = NULL;

	xmlKeepBlanksDefault(0);

	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the title attribute xml file is not exist"));
		return false;
	}

	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the title attribute xml file content is empty"));
		return false;
	}
	title_node = root->xmlChildrenNode;
	while(title_node)
	{
		if( xmlStrcmp( title_node->name, BAD_CAST"Title") != 0)
		{
			title_node = title_node->next;
			continue;
		}
	
		title_attr_data* p_data = new title_attr_data;
		get_xml_prop_def(p_data->id, title_node, "id", 0);

        get_xml_prop_def( p_data->agility,  title_node, "Agility",      0);
        get_xml_prop_def( p_data->strength, title_node, "Strength",     0);
        get_xml_prop_def( p_data->stamina,  title_node, "Stamina",      0);
        get_xml_prop_def( p_data->body,     title_node, "BodyQuality",  0);
        get_xml_prop_def( p_data->hit,      title_node, "Hit",          0);
        get_xml_prop_def( p_data->dodge,    title_node, "Dodge",        0);
        get_xml_prop_def( p_data->crit,     title_node, "Crit",         0);
        get_xml_prop_def( p_data->atk,      title_node, "Atk",          0);
        get_xml_prop_def( p_data->def,      title_node, "Def",          0);
        get_xml_prop_def( p_data->hp,       title_node, "Hp",           0);
        get_xml_prop_def( p_data->mp,       title_node, "Mp",           0);
        get_xml_prop_def( p_data->addhp,    title_node, "AddHp",        0);
        get_xml_prop_def( p_data->addmp,    title_node, "AddMp",        0);

        bool ret = add_title_attr_data(p_data);
		if(!ret)
		{
			throw XmlParseError(std::string("title_type has been existed"));
			return false;
		}
		title_node = title_node->next;
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return true;
}

bool title_attr_data_mgr::final()
{
	std::map<uint32_t, title_attr_data*>::iterator pItr = datas.begin();
	for(; pItr != datas.end(); ++pItr)
	{
		title_attr_data* p_data = pItr->second;
		delete p_data;
		p_data = NULL;
	}
	return true;
}

