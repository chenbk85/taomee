/**
* =====================================================================================
*       @file  Map.hpp
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
#ifndef KF_MAP_HPP_
#define KF_MAP_HPP_

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/timer.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>
}
#include "fwd_decl.hpp"
#include "Player.hpp"
#include "utils.hpp"

class MapManage;
class Home;

struct map_base_t {
	/*! map id */
	map_id_t id;
	/*! map name */
	char	name[MAX_NAME_LEN];
	/*! map x nad y */
	uint32_t	init_x;
	uint32_t	init_y;
};


class Map {
public:
	~Map();
	MapManage* getMapManage() const { return m_pManage_; }

public:
	map_base_t base_;
	PlayerManage playerManage_;
private:
	MapManage* m_pManage_;
private:
	friend class MapManage;
	Map(map_id_t id, MapManage* manage);
};

class MapManage {
public:
	MapManage(Home* home);
	~MapManage();

	Map* allocMap(map_id_t id);
	void deallocMap(Map* map);

	Map* getMap(map_id_t id);
private:
	typedef std::map<map_id_t, Map*> MapMap;
	Home* m_pHome_;
	MapMap map_map_;
};

#endif //KF_MAP_HPP_
