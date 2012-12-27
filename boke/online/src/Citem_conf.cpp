/*
 * =========================================================================
 *
 *        Filename: Citem_conf.cpp
 *
 *        Version:  1.0
 *        Created:  2011-07-22 14:36:46
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */
#include    "Citem_conf.h"
#include    "util.h"

void Citem_conf_map::add(uint32_t kindid , item_conf_t & item_conf )
{
	uint32_t itemid=item_conf.itemid;

	if (item_conf.comprise_itemid_list.size()>0) {
		this->cardid_comprise_map[itemid]=item_conf.comprise_itemid_list;
	}

	if (item_conf.source>0){
		source_conf_t source_conf;
 		source_conf.source=item_conf.source;
 		source_conf.pay=item_conf.pay;
		source_conf.gamept = -1u;

		this->source_map[itemid]= source_conf;  
	}
	
	this->conf_map[kindid][itemid ]=item_conf;
}

bool Citem_conf_map::check_can_change(uint32_t itemid )   
{
	std::map<uint32_t,  item_conf_t> ::iterator it;
	//1:装扮
	std::map<uint32_t,  item_conf_t> & attire_map=this->conf_map[1];
	it=attire_map.find(itemid );
	if (it!=attire_map.end()){
		return  bool(it->second.change);
	}else{
		return false;
	}
}

void Citem_conf_map::add_medal(uint32_t medalid)
{
	this->medal_set.insert(medalid);
}

uint32_t Citem_conf_map::get_clothtype(uint32_t itemid )
{
	std::map<uint32_t,  item_conf_t> ::iterator it;

	std::map<uint32_t,  item_conf_t> & attire_map=this->conf_map[1];
	it=attire_map.find(itemid );
	if (it!=attire_map.end()){
		return  it->second.clothtype;
	}else{
		return 0;
	}

}

uint32_t Citem_conf_map::get_type(uint32_t itemid )
{
	std::map<uint32_t,  item_conf_t> ::iterator it;
	uint32_t kid = itemid / 100000;

	std::map<uint32_t,  item_conf_t> & attire_map=this->conf_map[kid];
	it=attire_map.find(itemid );
	if (it!=attire_map.end()){
		return  it->second.type;
	}else{
		return 0;
	}
}

bool Citem_conf_map::is_on_island(uint32_t itemid, uint32_t island)   
{
	std::map<uint32_t, item_conf_t>::iterator it;
	//2:卡片
	uint32_t kid = itemid / 100000;
	std::map<uint32_t,  item_conf_t> & attire_map=this->conf_map[kid];
	it=attire_map.find(itemid);
	if (it!=attire_map.end()){
		return  it->second.islandid_list.find(island) != it->second.islandid_list.end();
	}else{
		return false;
	}
}

source_conf_t * Citem_conf_map::get_buy_info( uint32_t itemid )
{
	source_conf_t* pitem = get_item_info(itemid);
	return pitem && (pitem->source == 1 || pitem->source == 4) ? pitem : NULL;
}

source_conf_t * Citem_conf_map::get_item_info( uint32_t itemid )
{
	std::map<uint32_t, source_conf_t>::iterator it = this->source_map.find(itemid);
	return it != source_map.end() ? &it->second : NULL;
}

bool Citem_conf_map::is_medal(uint32_t itemid)
{
	return this->medal_set.find(itemid) != this->medal_set.end();
}

bool Citem_conf_map::is_cloth(uint32_t itemid)
{
	return itemid / 100000 == 1;
}

item_conf_t * Citem_conf_map::get_item_conf(uint32_t kindid, uint32_t itemid)
{
	std::map<uint32_t, std::map<uint32_t, item_conf_t> >::iterator it;
	it = this->conf_map.find(kindid);
	if (it == this->conf_map.end())
		return NULL;

	std::map<uint32_t, item_conf_t>::iterator itconf;
	itconf = it->second.find(itemid);
	return itconf != it->second.end() ? &itconf->second : NULL;
}

uint32_t Citem_movement_conf::get_movement_item(uint32_t exprid)
{
	std::map<uint32_t, uint32_t>::iterator it;
	it = this->item_movement_map.find(exprid);
	if (it != this->item_movement_map.end()) {
		return it->second;
	}
	return 0;
}

void Citem_movement_conf::init()
{
	this->item_movement_map.clear();
}

void Citem_movement_conf::add_movement(uint32_t exprid, uint32_t itemid)
{
	this->item_movement_map[exprid] = itemid;
}
