#include "map_data.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>

using namespace taomee;



map_data_mgr* map_data_mgr::get_instance()
{
	static map_data_mgr obj;
	return &obj;
}
    
bool   map_data_mgr::init_xml(const char* xml)
{
	xmlDocPtr   doc = NULL;
	xmlNodePtr  root = NULL;
	xmlNodePtr  map_node = NULL;
	xmlNodePtr  transport_node = NULL;
	char err_msg[1024] = {0};

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

	map_node = root->xmlChildrenNode;
	while(map_node)
	{
		if( xmlStrcmp(map_node->name, reinterpret_cast<const xmlChar*>("map")) == 0)	
		{
			map_data* p_data = new map_data();
			get_xml_prop_def(p_data->map_id,  map_node, "id", 0);
			get_xml_prop_def(p_data->born_x,  map_node, "init_x", 0);
			get_xml_prop_def(p_data->born_y,  map_node, "init_y", 0);
			
			if(p_data->map_id == 0)
			{
				xmlFreeDoc(doc);
				sprintf(err_msg, "the map_id = 0");
				throw XmlParseError(err_msg);
				return false;
			}

			if( is_map_data_exist(p_data->map_id) )
			{
				xmlFreeDoc(doc);
				sprintf(err_msg, "the map_id:%u is exist", p_data->map_id);
				throw XmlParseError(err_msg);
				return false;
			}

			transport_node = map_node->xmlChildrenNode;
			while(transport_node)
			{
				if( xmlStrcmp(transport_node->name, reinterpret_cast<const xmlChar*>("transport")) == 0 )
				{
					map_transport_info* p_info = new map_transport_info();
					get_xml_prop_def(p_info->port_id,  transport_node, "id", 0);
					get_xml_prop_def(p_info->x,   transport_node, "x", 0);
					get_xml_prop_def(p_info->y,   transport_node, "y", 0);
					get_xml_prop_def(p_info->to_map_id,   transport_node, "to_map", 0);
					get_xml_prop_def(p_info->to_map_x,   transport_node, "to_map_x", 0);
					get_xml_prop_def(p_info->to_map_y,   transport_node, "to_map_y", 0);
					p_data->add_transport_info(p_info);
				}

				if( xmlStrcmp(transport_node->name, reinterpret_cast<const xmlChar*>("npc")) == 0 )
				{
					map_npc_info* p_info2 = new map_npc_info();
					get_xml_prop_def(p_info2->npc_id,  transport_node, "npc_id", 0);
					get_xml_prop_def(p_info2->x,  	  transport_node, "x", 0);
					get_xml_prop_def(p_info2->y,  	  transport_node, "y", 0);
					p_data->add_npc_info(p_info2);					
				}

				transport_node = transport_node->next;	
			}	



			add_map_data(p_data);
		}
		map_node = map_node->next;
	}


	xmlCleanupParser();
	xmlFreeDoc(doc);
	return true;
}
	    
bool   map_data_mgr::final()
{
	map<uint32_t, map_data*>::iterator pItr = datas.begin();
	for( ; pItr != datas.end();  ++pItr)
	{
		map_data* p_data = pItr->second;
		if(p_data == NULL)continue;
		p_data->final();
		delete p_data;
		p_data = NULL;
	}
	return true;
}


bool map_data_mgr::add_map_data(map_data* data)
{
	if( is_map_data_exist(data->map_id) )return false;
	datas[data->map_id] = data;
	return true;
}

bool map_data_mgr::is_map_data_exist(uint32_t map_id)
{
	map<uint32_t, map_data*>::iterator pItr = datas.find(map_id);
	return pItr != datas.end();
}

map_data* map_data_mgr::get_map_data_by_id(uint32_t map_id)
{
	map<uint32_t, map_data*>::iterator pItr = datas.find(map_id);
	if( pItr == datas.end())return NULL;
	return pItr->second;
}



