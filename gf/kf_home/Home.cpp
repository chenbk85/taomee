/**
* =====================================================================================
*       @file  Home.cpp
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

#include <libtaomee++/inet/pdumanip.hpp>

#include <stdio.h>

#include "cli_proto.hpp"
#include "Online.hpp"
#include "Home.hpp"
#include "Map.hpp"
#include "Player.hpp"

using namespace taomee;
/************************************************************************************
 ****** Home class
 ************************************************************************************/
Home::Home(home_id_t id, HomeManage* manage):mapManage_(this)
{
	DEBUG_LOG("HOME CREATE\t[id=%u]",id);
	memset(&base_, 0x00, sizeof(home_base_t));

	m_pManage_ = manage;
	base_.id = id;
	home_state_ = home_unload;
}

Home::~Home()
{
	DEBUG_LOG("HOME DELETE\t[id=%u]",base_.id);
}

int
Home::get_home_info(Player* p)
{
	if ( home_state_ == home_loading ) {
		playerWait.insert(PlayerMap::value_type(p->base_.id, p));
	} else if ( home_state_ == home_unload ) {
		home_state_ = home_loading;
		playerWait.insert(PlayerMap::value_type(p->base_.id, p));
		return db_get_home();
	} else if ( home_state_ == home_loaded ) {
		int idx = sizeof(cli_proto_t);
		idx += pack_home_info(pkgbuf + idx);
		init_cli_proto_head(pkgbuf, cli_proto_login_home, idx);
		p->send_to_player(pkgbuf, idx, 1);
		p->enter_map(1,0,0);
	}

	return 0;
}

int 
Home::db_get_home()
{
	return db_get_home_callback();
}

int
Home::db_get_home_callback()
{
	sprintf(base_.name, "home_%d", base_.id);
	home_state_ = home_loaded;

	int idx = sizeof(cli_proto_t);
	idx += pack_home_info(pkgbuf + idx);
	init_cli_proto_head(pkgbuf, cli_proto_login_home, idx);

	PlayerMap::iterator it;
	for ( it = playerWait.begin(); it != playerWait.end(); ++it) {
		it->second->send_to_player(pkgbuf, idx, 1);
		it->second->enter_map(1,0,0);
	}
	playerWait.clear();

	return 0;
}

int
Home::pack_home_info(void* buf)
{
	int idx = 0;
	pack(buf, base_.id, idx);
	pack(buf, base_.name, sizeof(base_.name), idx);
	TRACE_LOG("pack homeid=%u",base_.id);

	return idx;
}
/************************************************************************************
 ****** HomeManage class
 ************************************************************************************/
HomeManage::HomeManage()
{
	DEBUG_LOG("HOMEMANAGE CREATE");
}

HomeManage::~HomeManage()
{
	DEBUG_LOG("HOMEMANAGE DELETE");
}

Home*
HomeManage::allocHome(home_id_t id)
{
	/*HomeMap::iterator it = home_map_.find(id);
	if ( it != home_map_.end() ) {
		ERROR_LOG("");
		return 0;
	}*/

	Home* home = new Home(id, this);
	if (home) {
		std::pair<HomeMap::iterator, bool> ret;
		ret = home_map_.insert(HomeMap::value_type(id, home));
		if ( !(ret.second) ) {
			SAFE_DELETE(home);
			//ERROR_LOG("");
			return 0;
		}
	}

	return home;
}

void 
HomeManage::deallocHome(Home* home)
{
	if (!home) {
		return;
	}

	HomeMap::iterator it = home_map_.find(home->base_.id);
	if ( it != home_map_.end() ) {
		SAFE_DELETE(it->second);
		home_map_.erase(it);
	}
}

Home*
HomeManage::getHome(home_id_t id)
{
	HomeMap::iterator it = home_map_.find(id);
	if ( it != home_map_.end() ) {
		return it->second;
	} else {
		return 0;
	}
}








