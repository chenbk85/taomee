/**
* =====================================================================================
*       @file  Home.hpp
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
#ifndef KF_HOME_HPP_
#define KF_HOME_HPP_

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/timer.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>
}

#include "fwd_decl.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "utils.hpp"

class Home;
class HomeManage;

typedef std::map<home_id_t, Home*> HomeMap;

struct home_base_t {
	/*! home id */
	home_id_t id;
	 /*! home name */
	char	name[MAX_NAME_LEN];
};

class Home {
public:
	~Home();
	int get_home_info(Player* p);
public:
	home_base_t base_;
	MapManage mapManage_;
	HomeManage* getHomeManage() const { return m_pManage_; }
private:
	enum {
		home_unload = 1,
		home_loading,
		home_loaded
	};
	int home_state_;
	HomeManage* m_pManage_;
	PlayerMap playerWait;
private:
	friend class HomeManage;
	Home(home_id_t id, HomeManage* manage);
	int db_get_home();
	int db_get_home_callback();
	int pack_home_info(void* buf);
};

class HomeManage {
public:
	HomeManage();
	~HomeManage();

	Home* allocHome(home_id_t id);
	void deallocHome(Home* home);

	Home* getHome(home_id_t id);
private:
	HomeMap home_map_;
};

#endif //KF_HOME_HPP_
