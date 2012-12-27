#include "pet_attr.hpp"
#include "singleton.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
using namespace taomee;

/*
pet_attr_data_mgr* pet_attr_data_mgr::getInstance()
{
	static  pet_attr_data_mgr obj;
	return &obj;
}
*/
bool pet_attr_data_mgr::is_attr_type_exist(uint32_t attr_type)
{
	std::map<uint32_t, pet_attr_data*>::iterator pItr = datas.find(attr_type);
	if(pItr != datas.end())return true;
	return false;
}

bool pet_attr_data_mgr::add_pet_attr_data(pet_attr_data* p_data)
{
	if( is_attr_type_exist(p_data->attr_type))return false;
	datas[ p_data->attr_type ] = p_data;
	return true;
}

bool pet_attr_data_mgr::add_item_attr_add_data(item_attr_t * p_data) 
{
	std::map<uint32_t, item_attr_t*>::iterator pItr = item_datas.find(p_data->item_id);
	if (pItr == item_datas.end()) {
		item_datas[ p_data->item_id ] = p_data;
		return true;
	}
	return false;
}

pet_attr_data* pet_attr_data_mgr::get_pet_attr_data_by_type(uint32_t attr_type)
{
	std::map<uint32_t, pet_attr_data*>::iterator pItr = datas.find(attr_type);
	if(pItr == datas.end())return NULL;
	return pItr->second;
}

int pet_attr_data_mgr::get_attr_per_by_item_id(uint32_t item_id)
{
	std::map<uint32_t, item_attr_t*>::iterator pItr = item_datas.find(item_id);

	if(pItr == item_datas.end())return 0;
	item_attr_t * attr = pItr->second;
	uint32_t odds = rand() % 100;
	for (uint32_t i = 0; i < attr->per_num; i++) {
		if (odds >= attr->attrs[i].odds_min 
				&& odds < attr->attrs[i].odds_max) {
			return  attr->attrs[i].add_per;
		}
		continue;
	}
	return 0;
}


bool pet_attr_data_mgr::init(const char* xml)
{
	if(xml == NULL){
		return false;
	}

	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;
	xmlNodePtr type_node = NULL;
	xmlNodePtr attr_node = NULL;

	xmlKeepBlanksDefault(0);

	doc = xmlParseFile(xml);
	if(doc == NULL){
		throw XmlParseError(std::string("the pet attribute xml file is not exist"));
		return false;
	}

	root = xmlDocGetRootElement(doc);
	if(root == NULL){
		xmlFreeDoc(doc);
		throw XmlParseError(std::string("the pet attribute xml file content is empty"));
		return false;
	}

	type_node = root->xmlChildrenNode;
	while(type_node)
	{
		if( xmlStrcmp( type_node->name, BAD_CAST"Attr") != 0)
		{

			if (xmlStrcmp(type_node->name, BAD_CAST"Item_Attr") == 0) {
				item_attr_t * data = new item_attr_t;
				get_xml_prop(data->item_id, type_node, "ID");
				xmlNodePtr odds_node = type_node->xmlChildrenNode;
				data->per_num = 0;
				while (odds_node) {
					if (xmlStrcmp(odds_node->name, BAD_CAST"Add_Attr") == 0) {
						get_xml_prop(data->attrs[data->per_num].odds, odds_node, "Odds");
						char tmp[10] = {0};
						get_xml_prop_raw_str(tmp, odds_node, "Add_per");
						data->attrs[data->per_num].add_per = atoi(tmp); 
						data->per_num++;
					}
					odds_node = odds_node->next;
				}

				//check vaild odds
				uint32_t min_odds = 0;
				for (uint32_t i = 0; i < data->per_num; i++) {
					data->attrs[i].odds_min = min_odds;
				    data->attrs[i].odds_max = data->attrs[i].odds + min_odds;
					min_odds = data->attrs[i].odds_max;	
				}	
				bool ret = add_item_attr_add_data(data);
				if(!ret)
				{
					throw XmlParseError(std::string("item_type has been existed"));
					return false;
				}

			} 

			type_node = type_node->next;
			continue;
		}
	
		pet_attr_data* p_data = new pet_attr_data;
		get_xml_prop_def(p_data->attr_type, type_node, "Type", 0);
		attr_node = type_node->xmlChildrenNode;

		while(attr_node)
		{
			if( xmlStrcmp( attr_node->name, BAD_CAST"base_attr") == 0 )
			{
				get_xml_prop_def( p_data->base_data.agility,  attr_node, "agility", 0);
				get_xml_prop_def( p_data->base_data.strength, attr_node, "strength", 0);
				get_xml_prop_def( p_data->base_data.stamina,  attr_node, "stamina", 0);
				get_xml_prop_def( p_data->base_data.body_quality,  attr_node, "body_quality", 0);
				get_xml_prop_def( p_data->base_data.hp,  attr_node, "hp", 0);
				get_xml_prop_def( p_data->base_data.attack,  attr_node, "attack", 0);
				get_xml_prop_def( p_data->base_data.crit_value,  attr_node, "crit_value", 0);
				get_xml_prop_def( p_data->base_data.hit_value,  attr_node, "hit_value", 0);
				get_xml_prop_def( p_data->base_data.def_value,  attr_node, "def_value", 0);
				get_xml_prop_def( p_data->base_data.dodge_value,  attr_node, "dodge_value", 0);
				get_xml_prop_def( p_data->base_data.accurate_value,  attr_node, "accurate", 0);
			}
			if( xmlStrcmp( attr_node->name, BAD_CAST"step_attr") == 0 )
			{
				get_xml_prop_def( p_data->step_data.agility,  attr_node, "agility", 0);
				get_xml_prop_def( p_data->step_data.strength, attr_node, "strength", 0);
				get_xml_prop_def( p_data->step_data.stamina,  attr_node, "stamina", 0);
				get_xml_prop_def( p_data->step_data.body_quality,  attr_node, "body_quality", 0);
				get_xml_prop_def( p_data->step_data.hp,  attr_node, "hp", 0);
				get_xml_prop_def( p_data->step_data.attack,  attr_node, "attack", 0);
				get_xml_prop_def( p_data->step_data.crit_value,  attr_node, "crit_value", 0);
				get_xml_prop_def( p_data->step_data.hit_value,  attr_node, "hit_value", 0);
				get_xml_prop_def( p_data->step_data.def_value,  attr_node, "def_value", 0);
				get_xml_prop_def( p_data->step_data.dodge_value,  attr_node, "dodge_value", 0);
				get_xml_prop_def( p_data->step_data.accurate_value,  attr_node, "accurate", 0);
			}
			attr_node = attr_node->next;
		}
		bool ret = add_pet_attr_data(p_data);
		if(!ret)
		{
			throw XmlParseError(std::string("pet_type has been existed"));
			return false;
		}
		type_node = type_node->next;
	}
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return true;
}

bool pet_attr_data_mgr::final()
{
	std::map<uint32_t, pet_attr_data*>::iterator pItr = datas.begin();
	for(; pItr != datas.end(); ++pItr)
	{
		pet_attr_data* p_data = pItr->second;
		delete p_data;
		p_data = NULL;
	}
	return true;
}

void load_pet_attribute(const char * xml)
{
	pet_attr_data_mgr & s_mgr =	singleton_default<pet_attr_data_mgr>::instance(); 
	s_mgr.init(xml);
}

void  calc_summon_attr(attr_data * attr, uint32_t attr_type, uint32_t attr_per, uint32_t lv)
{
	attr->init();
	pet_attr_data_mgr & s_mgr =	singleton_default<pet_attr_data_mgr>::instance(); 
	pet_attr_data * pet_data = s_mgr.get_pet_attr_data_by_type(attr_type);
	
	if (pet_data) {
		attr_data * step = &(pet_data->step_data);
		attr_data * base = &(pet_data->base_data);

		attr->accurate_value = base->accurate_value;
	    attr->agility = base->agility;
		attr->attack = base->attack;	
		attr->body_quality = base->body_quality;
		attr->crit_value = base->crit_value;
		attr->def_value = base->def_value;
		attr->dodge_value = base->dodge_value;
		attr->hit_value = base->hit_value;
		attr->hp = base->hp;
		attr->stamina = base->stamina;
		attr->strength = base->strength;



		//base info
		attr->hp += (step->hp * (lv - 1) * attr_per) / 100;    
		attr->stamina += (step->stamina * (lv - 1) * attr_per) / 100;
		attr->strength += (step->strength * (lv - 1) * attr_per) / 100;
		attr->hit_value += (step->hit_value * (lv - 1) * attr_per) / 100;
		attr->dodge_value += (step->dodge_value * (lv - 1) * attr_per) / 100;
		attr->def_value += (step->def_value * (lv - 1) * attr_per) / 100;
		attr->crit_value += (step->crit_value * (lv - 1) * attr_per) / 100;
		attr->body_quality += (step->body_quality * (lv - 1) * attr_per) / 100;
		attr->agility += (step->agility * (lv - 1) * attr_per) / 100;
		attr->accurate_value += (step->accurate_value * (lv - 1) * attr_per) / 100;
		attr->attack += (step->attack * (lv - 1) * attr_per) / 100;

		//attr 2 attr
		attr->hp += attr->body_quality * 10;
		attr->attack += attr->strength * 10;
		attr->crit_value += attr->agility * 5;
		attr->dodge_value += attr->agility * 5;
		attr->hit_value += attr->accurate_value * 10;
		attr->attack = (attr->attack * 2000) / 21413;
	}
}


int get_attr_per(uint32_t item_id)
{
	pet_attr_data_mgr & s_mgr =	singleton_default<pet_attr_data_mgr>::instance(); 
	return s_mgr.get_attr_per_by_item_id(item_id);
}
