/**
* =====================================================================================
*       @file  Map.cpp
*      @brief
*
*  Detailed description starts here.
*
*   @internal
*     Created  3/17/2010 11:13:56 AM
*    Revision  1.0.0.0
*    Compiler  gcc/g++
*     Company  TaoMee.Inc, ShangHai.
*   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
*
*     @author  mark (), mark@taomee.com
* This source code is wrote for TaoMee,Inc. ShangHai CN.
* =====================================================================================
*/

#include "Map.hpp"
#include "Player.hpp"

/************************************************************************************
 ***** Map class
 ************************************************************************************/
Map::Map(map_id_t id, MapManage* manage):playerManage_(this),m_pManage_(manage)
{
	DEBUG_LOG("MAP CREATE\t[id=%u]",id);
	memset(&base_, 0x00, sizeof(map_base_t));

	base_.id = id;
}

Map::~Map()
{
	DEBUG_LOG("MAP DELETE\t[id=%u]",base_.id);
}

/************************************************************************************
 ***** MapManage class
 ************************************************************************************/
MapManage::MapManage(Home* home):m_pHome_(home)
{
	//this->m_pHome_ = home;
	//TODO:alloc map
	this->allocMap(1);
	this->allocMap(2);
}

MapManage::~MapManage()
{
	MapMap::iterator it;
	for ( it = map_map_.begin(); it != map_map_.end(); ++it ) {
		SAFE_DELETE(it->second);
	}
	map_map_.clear();
}

Map*
MapManage::allocMap(map_id_t id)
{
	/*MapMap::iterator it = map_map_.find(id);
	if ( it != map_map_.end() ) {
		ERROR_LOG("");
		return;
	}*/

	Map* map = new Map(id, this);
	if (map) {
		std::pair<MapMap::iterator, bool> ret;
		ret = map_map_.insert(MapMap::value_type(id, map));
		if ( !(ret.second) ) {
			SAFE_DELETE(map);
			ERROR_LOG("");
			return 0;
		}
	}

	return map;
}

void
MapManage::deallocMap(Map* map)
{
	if (!map) {
		return;
	}

	MapMap::iterator it = map_map_.find(map->base_.id);
	if ( it != map_map_.end() ) {
		SAFE_DELETE(it->second);
		map_map_.erase(it);
	}
}

Map*
MapManage::getMap(map_id_t id)
{
	MapMap::iterator it = map_map_.find(id);
	if ( it != map_map_.end() ) {
		return it->second;
	} else {
		return 0;
	}

}












