#include <stdint.h>
#include "buff.hpp"
#include "player.hpp"
#include <libtaomee/log.h>
#include <libtaomee++/conf_parser/xmlparser.hpp>

using namespace taomee;


/**
 *   @brief init from xml data for mgr
 *   @param const char* 
 *   @return true sucess, false otherwise fail
**/
bool buff_data_mgr::init(const char* xml_name)
{
	if(xml_name == NULL){
		return false;
	}
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL;
	xmlNodePtr buff_node = NULL;
	xmlNodePtr effect_node = NULL;

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
	
	buff_node = root->xmlChildrenNode;
	while(buff_node)	
	{
		if( xmlStrcmp( buff_node->name, BAD_CAST"buff") != 0 ){
			buff_node = buff_node->next;
			continue;
		}
		uint32_t effect_id = 0;
		uint32_t effect_type = 0;
		buff_data *pData = new buff_data;
		effect_node = buff_node->xmlChildrenNode;
		
		get_xml_prop(pData->buff_id, buff_node, "buff_id");
		get_xml_prop_def(pData->duration_time, buff_node, "duration_time", 0);
		get_xml_prop_def(pData->icon,  buff_node, "icon", 0);
		get_xml_prop_def(pData->dead_delete, buff_node, "dead_delete", 0);
		get_xml_prop_def(pData->buff_type, buff_node, "buff_type", 1);
		get_xml_prop_def(pData->category, buff_node, "buff_category", 0);
		get_xml_prop_def(pData->level, buff_node, "buff_priority", 0);
		get_xml_prop_def(pData->buff_trigger_times, buff_node, "buff_trigger_times", 0);	
		get_xml_prop_def(pData->buff_finish_type, buff_node, "buff_finish_type", 1);

		while(effect_node)
		{	
			get_xml_prop(effect_id, effect_node, "effect_id");
			get_xml_prop(effect_type, effect_node, "effect_type");
			if(effect_id == 0 || effect_type == 0){
					ret = false;
					delete pData;
					pData = NULL;
					throw XmlParseError(std::string("invalid effect_id or effect_type"));
					goto ERR_LOGIC;
			}
			effect_id = effect_type * EFFECT_ID_FACTOR + effect_id;
			pData->effect_ids.push_back(effect_id);
			effect_node = effect_node->next;
		}

		if(is_buff_data_exist(pData->buff_id)){
			//ERROR_LOG("BUFFF ID IS EXIST %u", pData->buff_id);
			ret = false;
			delete pData;
			pData = NULL;
			throw XmlParseError(std::string("buff_id is exist"));
			goto ERR_LOGIC;
		}
		add_buff_data(pData);
		buff_node = buff_node->next;
	}
	
ERR_LOGIC:
	
	/*std::map<uint32_t, buff_data*>::iterator pItr = m_buff_data_maps.begin();
	for(; pItr != m_buff_data_maps.end(); ++pItr) {
		buff_data* pData2 = pItr->second;
		TRACE_LOG("buff id[%u]", pData2->buff_id);
	}*/

	xmlFreeDoc(doc);
    xmlCleanupParser();
	return ret;
}



/**
 *   @brief final data for mgr
 *   @param void
 *   @return true sucess, false otherwise fail
**/
bool buff_data_mgr::final()
{
	std::map<uint32_t, buff_data*>::iterator pItr = m_buff_data_maps.begin();
	for(; pItr != m_buff_data_maps.end(); ++pItr)
	{
		buff_data* pData = pItr->second;
		if(pData != NULL)
		{
			delete pData;
			pData = NULL;
		}
	}
	m_buff_data_maps.clear();
	return true;
}

/**
 *   @brief create static obj for mgr
 *   @param void
 *   @return  buff_data_mgr*
**/
buff_data_mgr* buff_data_mgr::getInstance()
{
	static buff_data_mgr obj;
	return &obj;
}

/**
 *   @brief get buff data count in mgr
 *   @param void
 *   @return uint32_t
**/
uint32_t buff_data_mgr::get_buff_data_count()
{
	return m_buff_data_maps.size();
}

/**
 *   @brief check the buff data is exist
 *   @param uint32_t 
 *   @return true sucess, false otherwise fail
**/
bool buff_data_mgr::is_buff_data_exist(uint32_t buff_id)
{
	return m_buff_data_maps.find(buff_id) != m_buff_data_maps.end();
}
       
/**
 *   @brief add buff data to mgr
 *   @param buff_data*
 *   @return true sucess, false otherwise fail
**/
bool buff_data_mgr::add_buff_data(buff_data* pData)
{
	if(pData == NULL) return false;
	if( is_buff_data_exist(pData->buff_id))return false;
	m_buff_data_maps[ pData->buff_id ] = pData;
	return true;
}
        
/**
 *   @brief del buff data from mgr
 *   @param uint32_t
 *   @return true sucess, false otherwise fail
**/
bool buff_data_mgr::del_buff_data(uint32_t buff_id)
{
	std::map<uint32_t, buff_data*>::iterator pItr = m_buff_data_maps.find(buff_id);
	if( pItr == m_buff_data_maps.end())return false;
	buff_data* pData = pItr->second;
	if(pData != NULL)
	{
		delete pData;
		pData = NULL;
	}
	m_buff_data_maps.erase(pItr);
	return true;
}

/**
 *   @brief final data for mgr
 *   @param void
 *   @return true sucess, false otherwise fail
**/
buff_data*  buff_data_mgr::get_buff_data_by_id(uint32_t buff_id)
{
	if(is_buff_data_exist(buff_id))
	{
		return m_buff_data_maps[buff_id];
	}
	return NULL;
}

/**
 *   @brief create buff by buff id
 *   @param uint32_t
 *   @return buff*
**/
////////////////////////////////////////////////////////////////////////
buff* create_buff(uint32_t buff_id)
{
	buff_data* pBuffdata = buff_data_mgr::getInstance()->get_buff_data_by_id(buff_id);
	if( pBuffdata == NULL )
	{
		return NULL;
	}
	if(pBuffdata->effect_ids.size() == 0)
	{
		return NULL;
	}


	std::vector<effect_data*>  data_points;	
	for(uint32_t i =0; i< pBuffdata->effect_ids.size(); i++)
	{
		effect_data* pEffectdata = effect_data_mgr::getInstance()->get_effect_data_by_id( pBuffdata->effect_ids[i] );
		if( ! pEffectdata )	
		{
			return NULL;
		}
		data_points.push_back(pEffectdata);	
	}
	
	buff *pBuff = new buff( pBuffdata->buff_id, 
							pBuffdata->category,  
							pBuffdata->level, 
							pBuffdata->icon, 
							pBuffdata->dead_delete, 
							pBuffdata->show_client);
	pBuff->set_buff_type(pBuffdata->buff_type);
	pBuff->set_buff_times(pBuffdata->buff_trigger_times);
	pBuff->set_buff_finish_type(pBuffdata->buff_finish_type);

	struct timeval begin_time = *get_now_tv();

	for(uint32_t i = 0; i< data_points.size(); i++ )
	{
		effect_data* pEffectdata = data_points[i];
		base_effect* pEffect = new base_effect();
		pEffect->init_base_effect(pEffectdata, begin_time);
		
		pBuff->add_base_effect(pEffect);
	}
	pBuff->init_base_duration_timer(begin_time ,pBuffdata->duration_time);		
	return pBuff;
}

bool buff_has_stuck_effect(uint32_t buff_id)
{
	buff_data* pBuffdata = buff_data_mgr::getInstance()->get_buff_data_by_id(buff_id);
	if( pBuffdata == NULL )
	{
		return false;
	}

	std::vector<effect_data*>  data_points;	
	for(uint32_t i =0; i< pBuffdata->effect_ids.size(); i++) {
		if (pBuffdata->effect_ids[i] == 104) {
			return true;
		}
	}
	return false;
}

/**
 *   @brief destroy buff
 *   @param buff*
 *   @return void
**/
void destroy_buff(buff* pBuff)
{
	TRACE_LOG("%u", pBuff->get_buff_id());
	if(pBuff != NULL)
	{
		delete pBuff;
	}
}

/**
 *   @brief reset buff timer
 *   @param buff*
 *   @return true sucess, false otherwise fail
**/
bool  reset_buff(buff* pBuff)
{
	if(pBuff == NULL)return false;
	struct timeval begin_time = *get_now_tv();
	pBuff->reset_timer(begin_time);
	return true;
}
