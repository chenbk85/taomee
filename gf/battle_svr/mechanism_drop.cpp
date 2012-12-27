#include "mechanism_drop.hpp"

bool mechanism_drop_mgr::init(const char* xml_name)
{
	if(xml_name == NULL){
		return false;
	}
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;

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

	xmlNodePtr drop_node = NULL;
	xmlNodePtr drop_item_node = NULL;	

	drop_node = root->xmlChildrenNode;
	while(drop_node)
	{
		if( xmlStrcmp( drop_node->name, BAD_CAST"drop") != 0 )
		{
			drop_node = drop_node->next;
			continue;
		}		
		mechanism_drop_data* p_data= new mechanism_drop_data();
		get_xml_prop_def(p_data->drop_id, drop_node, "id", 0);	
		get_xml_prop_def(p_data->repeat_count, drop_node, "repeat", 1);		
		if(p_data->drop_id == 0)
		{
			throw XmlParseError(std::string("invalid drop_id = 0"));
			return false;
		}	
		drop_item_node = drop_node->xmlChildrenNode;
		
		int item_begin_odds =0;
		//int monster_begin_odds = 0;
		
		while(drop_item_node)
		{
			if( xmlStrcmp( drop_item_node->name, BAD_CAST"item") == 0 )
			{
				mechanism_item_drop_t item_data;
				get_xml_prop_def(item_data.item_id,  drop_item_node, "id", 0); 	
				item_data.begin_odds = item_begin_odds;
				uint32_t odds = 0;
				get_xml_prop_def(odds,  drop_item_node, "drop_odds", 0);
				if(odds == 0 || item_data.item_id == 0)
				{
					throw XmlParseError(std::string("invalid odds = 0"));
					return false;
				}
				item_data.end_odds = item_data.begin_odds + odds;
				item_begin_odds += odds;
				p_data->item_drops.push_back(item_data);
			}
			if( xmlStrcmp( drop_item_node->name, BAD_CAST"monster") == 0)
			{
				mechanism_monster_drop_t monster_data;
				get_xml_prop_def(monster_data.monster_id,  drop_item_node, "id", 0);
			   	monster_data.begin_odds = item_begin_odds;	
				uint32_t odds = 0;
				get_xml_prop_def(odds,  drop_item_node, "drop_odds", 0);
				if(odds == 0 || monster_data.monster_id == 0)
				{
					throw XmlParseError(std::string("invalid odds = 0"));
					return false;
				}
				monster_data.end_odds = monster_data.begin_odds + odds;
				item_begin_odds += odds;
				p_data->monster_drops.push_back(monster_data);
			}

			drop_item_node = drop_item_node->next;
		}		
		drop_node = drop_node->next;
		add_mechanism_drop_data(p_data);
	}
	return true;
}
bool mechanism_drop_mgr::final()
{
	std::map<uint32_t, mechanism_drop_data*>::iterator pItr = datas.begin();
	for(; pItr != datas.end(); ++pItr)
	{
		mechanism_drop_data* pdata = pItr->second;
		delete pdata;
		pdata= NULL;
	}
	return true;
}

mechanism_drop_data* mechanism_drop_mgr::get_mechanism_drop_by_id(uint32_t id)
{
	std::map<uint32_t, mechanism_drop_data*>::iterator pItr = datas.find(id);
	if(pItr == datas.end())return NULL;
	return pItr->second;
}

bool mechanism_drop_mgr::add_mechanism_drop_data(mechanism_drop_data* p)
{
	if(p == NULL)return false;
	if(is_mechanism_drop_id_exist(p->drop_id))return false;
	datas[p->drop_id] = p;
	return true;
}

bool mechanism_drop_mgr::is_mechanism_drop_id_exist(uint32_t id)
{
	std::map<uint32_t, mechanism_drop_data*>::iterator pItr = datas.find(id);
	return pItr != datas.end();
}


mechanism_drop_mgr* mechanism_drop_mgr::getInstance()
{
	static mechanism_drop_mgr obj;
	return &obj;
}












