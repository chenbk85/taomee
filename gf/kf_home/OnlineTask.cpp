/**
* =====================================================================================
*       @file  OnlineTask.cpp
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
#include <assert.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>

#include <async_serv/net_if.h>
}

#include "fwd_decl.hpp"
#include "cli_proto.hpp"
#include "Player.hpp"
#include "OnlineTask.hpp"
#include "Home.hpp"
#include "utils.hpp"

using namespace taomee;


const int func_max = 100;
typedef int (*func_t)(Player* p, uint8_t* body, uint32_t bodylen);
func_t funcs[func_max] = {0};

/*! for packing protocol data and send to client. 2M */  
uint8_t pkgbuf[1 << 21];


/************************************************************************************
 **** OnlineTask class
 ************************************************************************************/
OnlineTask::OnlineTask(fdsession_t* fdsession)
{
	fdsession_ = fdsession;
	DEBUG_LOG("OnlineTask create!");
}

OnlineTask::~OnlineTask()
{
	PlayerMap::iterator it;
	for ( it = player_map_.begin(); it != player_map_.end(); ++it ) {
		if ( it->second ) {
			it->second->logout(disconnect);
			g_player->deallocPlayer(it->second);
		}
	}
	player_map_.clear();
	fdsession_ = 0;
	DEBUG_LOG("OnlineTask delete!");
}

int
OnlineTask::dispatch_msg(void* data)
{
	return 0;
}

void
OnlineTask::add_player(Player* p)
{
	if (!p) {
		ERROR_LOG("online add player error!");
		return;
	}

	std::pair<PlayerMap::iterator, bool> ret;
	ret = player_map_.insert(PlayerMap::value_type(p->base_.id, p));
	if ( !(ret.second) ) {
		WARN_LOG("player insert into online[uid=%u fd=%u]",p->base_.id, fdsession_->fd);
		ret.first->second = p;
	}
	TRACE_LOG("player[%u] insert into online[%u]",p->base_.id, fdsession_->fd);

	p->setOnline(this);
}

void
OnlineTask::del_player(Player* p)
{
	if (!p) {
		ERROR_LOG("online add player error!");
		return;
	}

	PlayerMap::iterator it = player_map_.find(p->base_.id);
	if ( it != player_map_.end() ) {
		it->second->setOnline(0);
		player_map_.erase(it);
	}

	TRACE_LOG("player [%u]delete from online uid[%u]",p->base_.id, fdsession_->fd);
}

void
OnlineTask::init_handles()
{
	funcs[cli_proto_login_home - cli_proto_cmd_start] = OnlineTask::login_home;
	funcs[cli_proto_logout_home - cli_proto_cmd_start] = OnlineTask::logout_home;
}

int
OnlineTask::login_home(Player* p, uint8_t* body, uint32_t bodylen)
{
	DEBUG_LOG("login_home!");

	CHECK_VAL_EQ(p, bodylen, 4);

	int idx = 0;
	home_id_t homeid;
	unpack(body, homeid, idx);

	Home* home = g_home->getHome(homeid);
	if (!home) {
		home = g_home->allocHome(homeid);
		if (!home) {
			ERROR_LOG("home alloc error\t[homeid=%u]",homeid);
			p->send_header_to_player(cli_proto_login_home, cli_err_system_error, 1);
			return 0;
		}
	}
	
	p->homeid = homeid;
	return home->get_home_info(p);
}

int
OnlineTask::logout_home(Player* p, uint8_t* body, uint32_t bodylen)
{
	DEBUG_LOG("logout_home!");
	
	CHECK_VAL_EQ(p, bodylen, 4);

	int idx = 0;
	uint32_t leave_reason = 0;
	unpack(body, leave_reason, idx);

	p->logout(leave_reason);
	g_player->deallocPlayer(p);

	return 0;
}


/************************************************************************************
 **** OnlineTaskManage class
 ************************************************************************************/
OnlineTaskManage::OnlineTaskManage()
{
	DEBUG_LOG("ONLINETASKMANAGE CREATE!");
}

OnlineTaskManage::~OnlineTaskManage()
{
	OnlineMap::iterator it;
	for ( it = online_map_.begin(); it != online_map_.end(); ++it ) {
		SAFE_DELETE(it->second);
	}
	online_map_.clear();
	DEBUG_LOG("ONLINETASKMANAGE DELETE!");
}

OnlineTask*
OnlineTaskManage::allocOnline(fdsession_t* fdsession)
{
	OnlineTask* online = new OnlineTask(fdsession);
	if (online) {
		std::pair<OnlineMap::iterator, bool> ret;
		ret = online_map_.insert(OnlineMap::value_type(fdsession->fd, online));
		if ( !(ret.second) ) {
			SAFE_DELETE(online);
			ERROR_LOG("insert online error![fd=%u]",fdsession->fd);
			return 0;
		}
	}
	return online;
}

void
OnlineTaskManage::deallocOnline(int fd)
{
	OnlineMap::iterator it = online_map_.find(fd);
	if ( it != online_map_.end() ) {
		SAFE_DELETE(it->second);
		online_map_.erase(it);
	}
}

OnlineTask*
OnlineTaskManage::getOnline(int fd)
{
	OnlineMap::iterator it = online_map_.find(fd);
	if ( it != online_map_.end() ) {
		return it->second;
	}
	return 0;
}

int 
OnlineTaskManage::dispatch(void* data, fdsession_t* fdsess)
{
	DEBUG_LOG("OnlineTask dispatch!");
	cli_proto_t* pkg = reinterpret_cast<cli_proto_t*>(data);

	// protocol for testing if this server is OK
	if (pkg->cmd == cli_proto_probe) {
		DEBUG_LOG("test ok");
		return send_pkg_to_client(fdsess, pkg, pkg->len);
	}

	pkg->cmd = taomee::bswap(pkg->cmd);
	pkg->id = taomee::bswap(pkg->id);
	pkg->len = taomee::bswap(pkg->len);

	int idx = pkg->cmd - cli_proto_cmd_start;
	if ( (pkg->cmd < cli_proto_cmd_start) || (pkg->cmd >= cli_proto_cmd_end) ) {
		//ERROR_LOG("invalid cmd=%u from fd=%d hdl=%p", pkg->cmd, fdsess->fd, funcs[idx]);
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fdsess->fd);
		return 0;
	}
	
	//get the online
	OnlineTask* online = g_online->getOnline(fdsess->fd);
	if (!online) {
		online = g_online->allocOnline(fdsess);
		if (!online) {
			return 0;
		}
	}

	//get the player
	Player* p = g_player->getPlayer(pkg->id);
	if ( (!p && pkg->cmd != cli_proto_login_home) 
			|| (p && pkg->cmd == cli_proto_login_home)
			|| (p && pkg->id != p->base_.id)) {
		ERROR_LOG("pkg error: fd=%u uid=%u %u cmd=%u p=%p", 
				fdsess->fd, (p ? p->base_.id : 0), pkg->id, pkg->cmd, p);
		return 0;
	}

	if ( pkg->cmd == cli_proto_login_home ) {
		p = g_player->allocPlayer(pkg->id);
		if (!p) {
			ERROR_LOG("alloc player error[uid=%u]",pkg->id);
			return 0;
		}
		//add the player to online
		online->add_player(p);
		p->base_.fdsess = fdsess;
	}

	//wait cmd
	if ( p->base_.waitcmd != 0 ) {
		if ( p->base_.waitcmd_queue.size() < 50 ) {
			uint32_t len = sizeof(cached_pkg_t) + pkg->len;
			cached_pkg_t* buf = reinterpret_cast<cached_pkg_t*>(malloc(len));
			buf->len = len;
			memcpy(buf->pkg, pkg, pkg->len);
			p->base_.waitcmd_queue.push(buf);
			DEBUG_LOG("CACHED A PKG\t[%u %u %u]",p->base_.id, p->base_.waitcmd, pkg->cmd);
			return 0;
		} else {
			WARN_LOG("wait for cmd=%u id=%u newcmd=%u",p->base_.waitcmd, p->base_.id, pkg->cmd);
			p->send_header_to_client(p->base_.waitcmd, cli_err_system_busy, 0);
			return 0;
		}
	}

	p->base_.waitcmd = pkg->cmd;
	uint32_t bodylen = pkg->len - sizeof(cli_proto_t);

	return funcs[idx](p, pkg->body, bodylen);
}






