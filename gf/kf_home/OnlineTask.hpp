/**
* =====================================================================================
*       @file  OnlineTask.hpp
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
#ifndef KF_ONLINETASK_HPP_
#define KF_ONLINETASK_HPP_

extern "C" {
#include <libtaomee/project/types.h>
}

//#include "fwd_decl.hpp"
#include "Player.hpp"


class OnlineTaskManage;
class OnlineTask;
typedef std::map<int, OnlineTask*> OnlineMap;

class OnlineTask {
public:
	~OnlineTask();

	void add_player(Player* p);
	void del_player(Player* p);
	int dispatch_msg(void* data);

	static void init_handles();
	static int login_home(Player* p, uint8_t* body, uint32_t bodylen);
	static int logout_home(Player* p, uint8_t* body, uint32_t bodylen);
private:
	friend class OnlineTaskManage;
	OnlineTask(fdsession_t* fd);
private:
	PlayerMap player_map_;
	fdsession_t* fdsession_;
};

class OnlineTaskManage {
public:
	OnlineTaskManage();
	~OnlineTaskManage();

	void deallocOnline(int fd);
	OnlineTask* getOnline(int fd);

	static int dispatch(void* data, fdsession_t* fdsess);
private:
	OnlineMap online_map_;
	OnlineTask* allocOnline(fdsession_t* fdsession);
};

/*! for packing protocol data and send to client. 2M */
extern uint8_t pkgbuf[1 << 21];


#endif //KF_ONLINETASK_HPP_
