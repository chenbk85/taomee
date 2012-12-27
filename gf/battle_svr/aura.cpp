#include <stdio.h>
#include <stdint.h>
#include "aura.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>

extern "C" {
#include <libtaomee/log.h>
}


using namespace taomee;


/**
 *   @brief create aura by aura id
 *   @param uint32_t
 *   @return aura*
**/
aura* create_aura(uint32_t aura_id, uint32_t create_id)
{
	aura_data* pAuradata = aura_data_mgr::getInstance()->get_aura_data_by_id(aura_id);
    if( pAuradata == NULL ){
        return NULL;
    }
    if(pAuradata->effect_ids.size() == 0){
        return NULL;
	}

    std::vector<effect_data*>  data_points;
    for(uint32_t i =0; i< pAuradata->effect_ids.size(); i++)
    {
        effect_data* pEffectdata = effect_data_mgr::getInstance()->get_effect_data_by_id( pAuradata->effect_ids[i] );
        if( ! pEffectdata ){
            return NULL;
        }
        data_points.push_back(pEffectdata);
    }

    aura *pAura = new aura(pAuradata->aura_id, pAuradata->aura_type, pAuradata->icon, pAuradata->aura_radius, create_id, pAuradata->show_client);
    struct timeval begin_time = *get_now_tv();

    for(uint32_t i = 0; i< data_points.size(); i++ )
    {
        effect_data* pEffectdata = data_points[i];
        base_effect* pEffect = new base_effect();
        pEffect->init_base_effect(pEffectdata, begin_time);
        pAura->add_base_effect(pEffect);
    }
	//光环的感染数据
	pAura->partnet_infect_datas = pAuradata->partnet_infect_datas;
	pAura->oppose_infect_datas = pAuradata->oppose_infect_datas;
	pAura->neutral_infect_datas = pAuradata->neutral_infect_datas;
    return pAura;
}

/**
 *   @brief destroy aura 
 *   @param aura*
 *   @return void
**/
void destroy_aura(aura* pAura)
{
	if(pAura != NULL){
		delete pAura;
	}	
}


/**
 *   @brief create static class obj
 *   @param void
 *   @return aura_data_mgr*
**/
aura_data_mgr* aura_data_mgr::getInstance()
{
	static aura_data_mgr obj;
	return &obj;
}

/**
 *   @brief init from xml file for mgr
 *   @param const char*
 *   @return true sucess, false otherwise fail
**/
bool  aura_data_mgr::init(const char* xml_name)
{
	if(xml_name == NULL){
        return false;
    }
    xmlDocPtr doc = NULL;
    xmlNodePtr root = NULL;
    xmlNodePtr aura_node = NULL;
    xmlNodePtr effect_node = NULL;

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

	aura_node = root->xmlChildrenNode;
    while(aura_node)
    {
        uint32_t effect_id = 0;
		uint32_t effect_type = 0;
		uint32_t buff_id = 0;
		uint32_t target = 0;
		aura_data *pData = new aura_data;
		get_xml_prop_def(pData->aura_id, aura_node, "aura_id", 0);
		get_xml_prop_def(pData->aura_type, aura_node, "aura_type", 1);
		get_xml_prop_def(pData->icon, aura_node, "aura_icon", 0);
		get_xml_prop_def(pData->aura_radius,  aura_node, "aura_radius", 0);

        effect_node = aura_node->xmlChildrenNode;
        while(effect_node)
        {
        	if( xmlStrcmp(effect_node->name, (const xmlChar*)"effect") == 0)
			{
				get_xml_prop_def(effect_id, effect_node, "effect_id", 0);
				get_xml_prop_def(effect_type, effect_node, "effect_type", 0);
				if(effect_id == 0 || effect_type == 0){
					throw XmlParseError(std::string("the aura effect_id == 0 || effect_type == 0"));
				}
				effect_id = effect_type * EFFECT_ID_FACTOR + effect_id;
				pData->effect_ids.push_back(effect_id);
			}
			else if( xmlStrcmp(effect_node->name, (const xmlChar*)"buff") == 0)
			{
				get_xml_prop_def(buff_id, effect_node, "buff_id", 0);
				get_xml_prop_def(target, effect_node,  "target", 0);
				if(buff_id < 20000 || target == 0){
					throw XmlParseError(std::string("the aura buff_id < 20000 || target == 0"));	
				}
				
				if(target == 1){
					pData->partnet_infect_datas.push_back(buff_id);			
				}else if(target == 2){
					pData->oppose_infect_datas.push_back(buff_id);
				}else {
					pData->neutral_infect_datas.push_back(buff_id);
				}
			}
			effect_node = effect_node->next;
        }//while
		
		if( is_aura_data_exist(pData->aura_id))
        {
        	printf("aura data exist %u", pData->aura_id);
            ret = false;
            delete pData;
            pData = NULL;
			throw XmlParseError(std::string("aura_id is exist"));
            goto ERR_LOGIC;
        }
        add_aura_data(pData);
        aura_node = aura_node->next;
    }//while

ERR_LOGIC:
    xmlFreeDoc(doc);
    xmlCleanupParser();
	return ret;
}


/**
 *    @brief final mgr obj
 *    @param void
 *    @return true sucess, false otherwise fail
**/
bool  aura_data_mgr::final()
{
	std::map<uint32_t, aura_data*>::iterator pItr = m_aura_data_maps.begin();
    for(; pItr != m_aura_data_maps.end(); ++pItr)
    {
    	aura_data* pData = pItr->second;
        if(pData != NULL)
        {
            delete pData;
        	pData = NULL;
    	}
    }
    m_aura_data_maps.clear();
    return true;
}

/**
 *    @brief check the aura id is exist in mgr
 *    @param uint32_t
 *    @return true sucess, false otherwise fail
**/
bool  aura_data_mgr::is_aura_data_exist(uint32_t aura_id)
{
	return m_aura_data_maps.find(aura_id) != m_aura_data_maps.end();
}


/**
 *    @brief add aura data to mgr
 *    @param aura_data*
 *    @return true sucess, false otherwise fail
**/
bool  aura_data_mgr::add_aura_data(aura_data* pData)
{
	if(pData == NULL){
		return false;
	}
    if( is_aura_data_exist(pData->aura_id)){
		return false;
	}
	TRACE_LOG("%u", pData->aura_id);
    m_aura_data_maps[ pData->aura_id ] = pData;
    return true;
}
/**
 *   @brief del aura data from mgr
 *   @param uint32_t
 *   @return true sucess, false otherwise fail
**/
bool  aura_data_mgr::del_aura_data(uint32_t aura_id)
{
	std::map<uint32_t, aura_data*>::iterator pItr = m_aura_data_maps.find(aura_id);
    if( pItr == m_aura_data_maps.end()){
		return false;
	}
        
	aura_data* pData = pItr->second;
    if(pData != NULL){
        delete pData;
        pData = NULL;
    }
    m_aura_data_maps.erase(pItr);
    return true;
}

/**
 *   @brief get aura count of mgr
 *   @param void
 *   @return uint32_t
**/
uint32_t aura_data_mgr::get_aura_data_count()
{
	return m_aura_data_maps.size();
}

/**
 *    @brief get aura data in mgr
 *    @param uint32_t
 *    @return aura_data*
**/
aura_data* aura_data_mgr::get_aura_data_by_id(uint32_t aura_id)
{
	if(is_aura_data_exist(aura_id)){
		return m_aura_data_maps[aura_id];
    }
	return NULL;
}
