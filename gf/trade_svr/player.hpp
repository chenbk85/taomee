/**
 *============================================================
 *  @file      player.hpp
 *  @brief    player related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KFBTL_PLAYER_HPP_
#define KFBTL_PLAYER_HPP_

#include <list>
#include <map>
#include <vector>

#include <boost/intrusive/list.hpp>

#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee++/event/eventable_obj.hpp>
#include <libtaomee++/event/event_mgr.hpp>

#include <kf/player_attr.hpp>

extern "C" {
#include <libtaomee/list.h>
#include <libtaomee/timer.h>

#include <libtaomee/project/constants.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/project/types.h>

#include <async_serv/dll.h>
}
#include <kf/item_impl.hpp>
//#include "mempool/mempool.hpp"
//#include "state_machine/state_machine.hpp"

#include "fwd_decl.hpp"

#include "cli_proto.hpp"
#include "dbproxy.hpp"
#include "object.hpp"
#include "trade.hpp"


//pre declare----
class Player;
class Market;
class Shop;
struct player_attr_t;


/**
  * @brief type of map that holds all the players
  */
typedef std::map<userid_t, Player*> PlayerMap;
/**
  * @brief Player
  */
//class Player : public MovingObject, public MemPool<Player> {
class Player : public taomee::MemPool{
public:
	/**
	  * @brief constructor
	  * @param uid user id
	  * @param fdsession fd session
	  */
	Player(userid_t uid = 0, fdsession_t* fdsession = 0);
	/**
	  * @brief destructor
	  */
	~Player();

	//----------------------------------
	// player's normal actions
	//----------------------------------
	/**
	  * @brief player move
	  */
	void walk(uint32_t x, uint32_t y, uint32_t flag);
	/**
	  * @brief player move
	  */
	void walk_keyboard(uint32_t x, uint32_t y, uint8_t dir, uint8_t state);

	/**
	* @brief player stand
	*/
	void stand(uint32_t x, uint32_t y, uint8_t dir);
	/**
	  * @brief player jump
	  */
	void jump(uint32_t x, uint32_t y);
	/**
	  * @brief player stop moving
	  */
	void stop();

	void talk(uint8_t* msg, uint32_t msg_len, userid_t recvid);

	void pack_player_map_info(uint8_t *buf, int &idx, bool bHost);

	void player_setup_shop( )
	{
		uint32_t x =  40000;
		uint32_t y =  40000; 
		stand(x, y, direction);
	}

	void player_drop_shop(uint32_t shopid)
	{
		uint32_t x = 50 + shopid * 250;
		uint32_t y = 400; 
		if (shopid >= 4) {
			stand(x, y, direction);
		} else {
			stand(x, y, direction);
		}
	}

public:
	void init_player_info(player_info_t * p_info);

	void set_fight_summon(uint32_t mon_tm);

public:
	/*! player's id */
	userid_t	id;
	/*! the create time of the player's role */
	uint32_t	role_tm;
	/*! type of a player's role */
	uint32_t	role_type;

    uint32_t    power_user;

	uint32_t	player_show_state;
	/*! player's nick name */
	char		nick[max_nick_size];
	/*! vip flag */
	uint32_t	vip;

	uint32_t    vip_level;

	uint32_t    using_achieve_title;

	uint32_t    flag;

	uint32_t    honor;


	uint32_t    app_mon;

	uint32_t	coins;

	uint16_t	lv;

	uint32_t    xpos;
	uint32_t    ypos;
	uint8_t 	direction;

	uint32_t   invisible;

	uint16_t    summon_cnt;
	summon_t    summons[max_summon_num];
	summon_t*   fight_mon;

	uint32_t    clothes_cnt;
	
	//uint32_t    clothes_id[max_clothes_num];
    clothes_base_t  clothes_arr[max_clothes_num];

	Market * p_market;

	Shop * p_shop;

	uint32_t sell_cnt;

	/*! save cmd id that is currently under processing */
	uint16_t	waitcmd;


	GQueue*		pkg_queue;

	/*! hook that is used to link all players whose waitcmd is not 0 together */
	ObjectHook	awaiting_hook;

	/*! save socket fd from parent process */
	int				fd;
	/*! fd session */
	fdsession_t*	fdsess;
	
	/*! true if we need to save a player's attr */
	bool	need_sav_;

	uint32_t retry_times;

};


/**
  * @brief clear players' info whose online server is down
  * @param fd online fd
  */
void clear_players(int fd = -1);

/**
  * @brief get player by user id
  * @param uid user id
  * @return pointer to the player if found, 0 otherwise
  */
Player* get_player(userid_t uid);

/**
  * @brief allocate and add a player to this server
  * @return pointer to the newly added player
  */
Player* add_player(userid_t uid, fdsession_t* fdsess);

void del_player(Player *p);



#endif // KFBTL_PLAYER_HPP_

