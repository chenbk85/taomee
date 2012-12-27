/**
* =====================================================================================
*       @file  Player.cpp
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

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>

#include <async_serv/net_if.h>
}

#include "cli_proto.hpp"
#include "utils.hpp"
#include "Player.hpp"
#include "Home.hpp"
#include "Map.hpp"
#include "Online.hpp"

using namespace taomee;

/************************************************************************************
 **** Player class
 ************************************************************************************/
Player::Player(userid_t id) 
{
	DEBUG_LOG("PLAYER CREATE\t[id=%u]",id);
	memset(&base_, 0x00, sizeof(player_base_t));

	base_.id = id;

	m_pManage_ = 0;
	m_pOnline_ = 0;
}

Player::~Player()
{
	DEBUG_LOG("PLAYER DELETE\t[id=%u]",base_.id);
	
	m_pManage_ = 0;
	m_pOnline_ = 0;
}

int
Player::setManage(PlayerManage* manage) 
{
	if ( manage && m_pManage_ ) {
		ERROR_LOG("setManage error![uid=%u]", base_.id);
		return 0;
	}
	m_pManage_ = manage;
	
	return 0;
}

int
Player::setOnline(Online* online)
{
	if ( online && m_pOnline_ ) {
		ERROR_LOG("player[%u] already in online",base_.id);
		return 0;
	} 
	m_pOnline_ = online;
	return 0;
}

int
Player::enter_map(map_id_t mapid, uint32_t x, uint32_t y)
{
	if (m_pManage_) {
		ERROR_LOG("invalid operation: enter map uid=%u", base_.id);
		return cli_err_still_in_map;
	}

	Home* home = g_home->getHome(homeid);
	if (!home) {
		ERROR_LOG("player not in home: uid=%u home_id=%u", base_.id, homeid);
		return cli_err_not_in_home;
	}

	Map* map = home->mapManage_.getMap(mapid);
	if (!map) {
		ERROR_LOG("enter_map:no mapid[%u]",mapid);
		return cli_err_no_mapid;
	}

	map->playerManage_.addPlayer(this);
	
	int idx = sizeof(cli_proto_t);
	idx += pack_entermap_rsp(pkgbuf + idx);
	init_cli_proto_head(pkgbuf, cli_proto_enter_map, idx);
	map->playerManage_.send_to_all_players(this, pkgbuf, idx, 1);
	TRACE_LOG("enter map uid=%u, mapid=%u", base_.id, mapid);

	return 0;
}

int
Player::leave_map(int self_flag)
{
	if (!m_pManage_) {
		/*ERROR_LOG("invalid operation: leave map uid=%u", base_.id);
		return cli_err_not_in_map;*/
		return 0;
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, base_.id, idx);
	init_cli_proto_head(pkgbuf, cli_proto_leave_map, idx);
	if (!self_flag) {
		m_pManage_->send_to_all_players(this, pkgbuf, idx, 1);
	} else  {
		m_pManage_->send_to_all_players_except_self(this, pkgbuf, idx);
	}

	m_pManage_->delPlayer(this);
	TRACE_LOG("leave map uid=%u",base_.id);
	return 0;
}

int
Player::list_players_in_map()
{
	if (!m_pManage_) {
		ERROR_LOG("invalid operation: leave map uid=%u", base_.id);
		return cli_err_not_in_map;
	}

	int idx_tmp = sizeof(cli_proto_t);
	int idx = idx_tmp + 4;
	uint32_t cnt = 0;
	PlayerMap::iterator it;
	for ( it = m_pManage_->player_map_.begin(); it != m_pManage_->player_map_.end(); ++it ) {
		idx += pack_entermap_rsp(pkgbuf + idx);
		cnt++;
	}
	pack(pkgbuf, cnt, idx_tmp);
	init_cli_proto_head(pkgbuf, cli_proto_list_players, idx);
	send_to_player(pkgbuf, idx, 1);
	return 0;
}

int
Player::logout(uint32_t reason)
{
	if ( reason == normal_logout ) {//request to logout
		if (m_pManage_) {
			leave_map(1);
		}
		send_header_to_player(cli_proto_logout_home, 0, 1);
	} else {//unexpected,such as disconnect
		if (m_pManage_) {
			leave_map(0);
		}
	}

	if (m_pOnline_){
		m_pOnline_->del_player(this);
	}

	return 0;
}

int
Player::pack_entermap_rsp(uint8_t* buf)
{
	int idx = 0;
	pack(buf, base_.id, idx);
	pack(buf, base_.nick, sizeof(base_.nick), idx);
	TRACE_LOG("pack enter map uid=%u nick=[%s]",base_.id, base_.nick);

	return idx;
}

void
Player::send_to_client(void* buf, int len, int complete)
{
	cli_proto_t* proto = reinterpret_cast<cli_proto_t*>(buf);

	proto->id = taomee::bswap(base_.id);
	proto->seq = taomee::bswap(0);
	if (send_pkg_to_client(base_.fdsess, buf, len) == -1) {
		ERROR_LOG("failed to send pkg to client: uid=%u cmd=%u", base_.id, taomee::bswap(proto->cmd));
		//TODO:delete player???
		return;
	}
	TRACE_LOG("send to client uid=%u cmd=%u fd=%u",base_.id, taomee::bswap(proto->cmd), base_.fdsess->fd);

	if (complete) {
		base_.waitcmd = 0;
	}

	return;
}

void
Player::send_to_player(void* buf, int len, int complete)
{
	cli_proto_t* proto = reinterpret_cast<cli_proto_t*>(buf);

	//proto->id = base_.id;
	proto->id = taomee::bswap(base_.id);
	//proto->seq = ( (base_.fdsess->fd << 16) | proto->cmd );
	proto->seq = taomee::bswap( (base_.fdsess->fd << 16) | proto->cmd );
	if (send_pkg_to_client(base_.fdsess, buf, len) == -1) {
		ERROR_LOG("failed to send pkg to client: uid=%u cmd=%u", base_.id, taomee::bswap(proto->cmd));
		//TODO:delete player???
		return;
	}
	TRACE_LOG("send to player uid=%u cmd=%u fd=%u",base_.id, taomee::bswap(proto->cmd), base_.fdsess->fd);
	//TRACE_LOG("send to player uid=%u cmd=%u fd=%u",proto->id, proto->cmd, base_.fdsess->fd);

	if (complete) {
		base_.waitcmd = 0;
	}
	return;
}

void
Player::send_header_to_client(uint16_t cmd, uint32_t err, int complete)
{
	cli_proto_t pkg;
	init_cli_proto_head(&pkg, cmd, sizeof(pkg));
	pkg.ret = err;

	send_to_client(&pkg, sizeof(pkg), complete);
}

void
Player::send_header_to_player(uint16_t cmd, uint32_t err, int complete)
{
	cli_proto_t pkg;
	init_cli_proto_head(&pkg, cmd, sizeof(pkg));
	pkg.ret = err;

	send_to_player(&pkg, sizeof(pkg), complete);
}



/************************************************************************************
 **** PlayerManage class
 ************************************************************************************/
PlayerManage::PlayerManage(Map* map):map_(map)
{
	DEBUG_LOG("PLAYERMANAGE CREATE\t[mapid=%u]", map->base_.id);
}

PlayerManage::~PlayerManage()
{
	DEBUG_LOG("PLAYERMANAGE DELETE\t[mapid=%u]", map_->base_.id);
	map_ = 0;
	//TODO:delete player_map_???
}

void
PlayerManage::addPlayer(Player* p)
{
	if (!p) {
		ERROR_LOG("playerManage add player error!");
		return;
	}

	if ( p->setManage(this) < 0 ) {
		return;
	}

	std::pair<PlayerMap::iterator, bool> ret;
	ret = player_map_.insert(PlayerMap::value_type(p->base_.id, p));
	if ( !(ret.second) ) {
		WARN_LOG("player insert into playerManage[uid=%u]",p->base_.id);
		ret.first->second = p;
	}
	TRACE_LOG("player[%u] insert into playerManage",p->base_.id);
}

void
PlayerManage::delPlayer(Player* p)
{
	if (!p) {
		return;
	}

	PlayerMap::iterator it = player_map_.find(p->base_.id);
	if ( it != player_map_.end() ) {
		it->second->setManage(0);
		player_map_.erase(it);
	}
	TRACE_LOG("delete from playerMange uid=%u",p->base_.id);
}

Player*
PlayerManage::getPlayer(userid_t id)
{
	PlayerMap::iterator it = player_map_.find(id);
	if ( it != player_map_.end() ) {
		return it->second;
	} else {
		return 0;
	}
}

void
PlayerManage::send_to_all_players(Player* p, void* buf, int len, int complete)
{
	PlayerMap::iterator it;
	for ( it = player_map_.begin(); it != player_map_.end(); ++it) {
		if ( !p && it->second == p ) {
			p->send_to_player(buf, len ,complete);
		} else {
			it->second->send_to_player(buf, len, 0);
		}
	}
}

void
PlayerManage::send_to_all_players_except_self(Player* p, void* buf, int len)
{
	PlayerMap::iterator it;
	for ( it = player_map_.begin(); it != player_map_.end(); ++it) {
		if ( it->second != p ) {
			it->second->send_to_player(buf, len, 0);
		}
	}
}

/************************************************************************************
 **** GlobalPlayer class
 ************************************************************************************/
GlobalPlayer::GlobalPlayer()
{
	DEBUG_LOG("GLOBALPLAYER CREAT!");
}

GlobalPlayer::~GlobalPlayer()
{
	PlayerMap::iterator it;
	for ( it = player_map_.begin(); it != player_map_.end(); ++it ) {
		SAFE_DELETE(it->second);
	}
	player_map_.clear();
	DEBUG_LOG("GOLBALPLAYER DELETE!");
}

Player*
GlobalPlayer::allocPlayer(userid_t id)
{
	/*PlayerMap::iterator it = player_map_.find(id);
	if ( it != player_map_.end() ) {
		ERROR_LOG("");
		return;
	}*/

	Player* p = new Player(id);
	if ( p ) {
		std::pair<PlayerMap::iterator, bool> ret;
		ret = player_map_.insert(PlayerMap::value_type(id, p));
		if ( !(ret.second) ) {
			SAFE_DELETE(p);
			ERROR_LOG("player[%u] insert into g_player error!",id);
			return 0;
		}
	}

	return p;
}

void
GlobalPlayer::deallocPlayer(Player* p)
{
	if (!p) {
		return;
	}

	PlayerMap::iterator it = player_map_.find(p->base_.id);
	if ( it != player_map_.end() ) {
		SAFE_DELETE(it->second);
		player_map_.erase(it);
	}
}

Player*
GlobalPlayer::getPlayer(userid_t id)
{
	PlayerMap::iterator it = player_map_.find(id);
	if ( it != player_map_.end() ) {
		return it->second;
	} else {
		return 0;
	}
}


