#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <kf/item_impl.hpp>
#include <kf/item_manager.hpp>
#include "global_data.hpp"
#include "check_shop.hpp"
using namespace taomee;





bool shop_mgr::init(const char* xml)
{
	if(xml == NULL)return false;
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr shop_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the shop xml file is not exist"));
		return false;
	}

	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the shop xml file content is empty"));
		return false;
	}

	shop_node = root->xmlChildrenNode;
	while(shop_node)
	{
		char items_str[4096] = {0};
		shop_data* p_data = new shop_data;	
		
		get_xml_prop_def( p_data->shop_id, shop_node,  "id", 0);
		get_xml_prop_raw_str_def(items_str, shop_node, "items", "");
		if(p_data->shop_id == 0 || strlen(items_str) == 0){
			xmlFreeDoc(doc);
			throw XmlParseError(std::string("invalid shop_id"));
			return false;
		}	
	
		char *token = NULL;
		token = strtok(items_str, ",");
		while(token != NULL)
		{
			uint32_t item_id = atoi(token);
			p_data->items.push_back(item_id);
			token = strtok( NULL, ",");
		}
		bool ret = get_shop_mgr()->add_shop_data(p_data);	
		if(!ret)
		{
			xmlFreeDoc(doc);
			throw XmlParseError(std::string("shop_id existed"));
			return false;
		}
		shop_node = shop_node->next;
	}
	return true;
}

bool shop_mgr::final()
{
	std::map<uint32_t,  shop_data*>::iterator pItr = m_datas.begin();
	for(; pItr != m_datas.end(); ++pItr)
	{
		shop_data* p_data = pItr->second;
		delete p_data;
	}	
	return true;
}

bool shop_mgr::add_shop_data(shop_data* data)
{
	if(  is_shop_data_exist(data->shop_id))return false;
	m_datas[data->shop_id] = data;
	return true;
}

shop_data* shop_mgr::get_shop_data_by_id(uint32_t shop_id)
{
	std::map<uint32_t,  shop_data*>::iterator pItr = m_datas.find(shop_id);
	if(pItr == m_datas.end())return NULL;
	return (pItr->second);
}

bool shop_mgr::is_shop_data_exist(uint32_t shop_id)
{
	return m_datas.find(shop_id) != m_datas.end();
}


bool check_items_price(char* items_str)
{
	if(items_str == NULL || strlen(items_str) == 0)return false;
	char *token = NULL;
	token = strtok(items_str, ",");
	while(token != NULL)
	{
		uint32_t item_id = atoi(token);
		const GfItem* itm = items->get_item(item_id);
		if(itm == NULL)
		{
			char err_str[1024] = {0};
			sprintf(err_str, "invalid item_id:%u in shop.xml",  item_id);
			throw XmlParseError(std::string(err_str));
			return false;
		}
		if(itm->price() == 0)
		{
			char err_str[1024] = {0};
			sprintf(err_str, "item:%u price is 0",  item_id);
			throw XmlParseError(std::string(err_str));
			return false;
		}
		token = strtok( NULL, ",");
	}
	return true;
}


bool check_npc_shop(const char* xml_name)
{
	if(xml_name == NULL)return false;
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr shop_node = NULL;

	xmlKeepBlanksDefault(0);
	doc = xmlParseFile(xml_name);
	if(doc == NULL){
		throw XmlParseError(std::string("the shop xml file is not exist"));
		return false;
	}

	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the shop xml file content is empty"));
		return false;
	}

	shop_node = root->xmlChildrenNode;
	while(shop_node)
	{
		char items_str[4096] = { 0 };
		uint32_t shop_id = 0;
		get_xml_prop_def( shop_id, shop_node,  "id", 0);
		get_xml_prop_raw_str_def(items_str, shop_node, "items", "");
		if(shop_id != 99999)
		{
			bool ret = check_items_price(items_str);
			if(ret == false)
			{
				xmlFreeDoc(doc);
				throw XmlParseError(std::string("the items_str error"));
				return false;
			}
		}
		shop_node = shop_node->next;
	}
	return true;
}


bool check_npc_item(uint32_t npc_id, uint32_t item_id)
{
	shop_data* p_data = get_shop_mgr()->get_shop_data_by_id(npc_id);
	if(p_data == NULL)return false;
	return p_data->is_item_exist(item_id);
	return true;
}



