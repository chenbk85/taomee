/**
  *============================================================
  *  @file      map.hpp
  *  @brief    map related functions are declared here
  * 
  *  compiler   gcc4.1.2
  *  platform   Linux
  *
  *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
  *
  *============================================================
  */

#ifndef KF_MAP_HPP_
#define KF_MAP_HPP_

#include <libtaomee++/conf_parser/xmlparser.hpp>

extern "C" {
#include <stdint.h>

#include <libtaomee/list.h>
}

#include "fwd_decl.hpp"
#include "player.hpp"

/*! length of a map name */
const int	map_name_len	= 64;


struct map_t {
	/*! map id */
	map_id_t	id;
	/*! map name */
	char		name[map_name_len];

	uint32_t	init_x;
	uint32_t	init_y;

	/*! number of players within a map */
	uint32_t	player_num;

	/*! head node to link all users within a map together */
	list_head_t	playerlist;

	/*! hook to be linked to all_maps[c_bucket_size] */
	list_head_t	hash_hook;

};

//---------------------------------------------------------------------

/**
  * @brief player enters map
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int enter_map_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief player leaves map
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int leave_map_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief get info of all the players in this map
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int list_user_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

//---------------------------------------------------------------------

/**
  * @brief return a pointer to a map which id equals to 'mid'
  * @param mid map id
  * @see pointer to the map if map with id equals to 'mid' is found, 0 otherwise
  */
map_t* get_map(map_id_t mid);
/**
  * @brief add player p to map newmap
  * @param p
  * @param newmap
  * @param oldmap
  * @return 0 on success, -1 on error
  */
int enter_map(player_t* p, map_id_t mapid, uint32_t x, uint32_t y);
/**
  * @brief remove player p from map m
  * @param p
  * @return 0 on success, -1 on error
  */
int leave_map(player_t* p);

/**
  * @brief pack player p's info into buf and send it to other players
  * @param p p's info will be packed into buf
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  */
void send_entermap_rsp(player_t* p, int complete);

/**
  * @brief pack player p's info into buf and send it to other players
  * @param p p's info will be packed into buf
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  */
void send_leave_map_rsp(player_t* p, int complete);
//---------------------------------------------------------------------

/**
  * @brief load map configs from the xml file
  * @return 0 on success, -1 on error
  * @see unload_maps
  */
int load_maps(xmlNodePtr cur);
/**
  * @brief unload all the maps and free memory
  * @see load_maps
  */
void unload_maps();

//------------------------------------------------------------------
// inline functions
//------------------------------------------------------------------

/**
 *   * @brief get map_id_t high-32-bit
 *     */
inline uint32_t get_mapid_high32(map_id_t mapid)
{

	return (mapid >> 32);

}

/**
 *   * @brief get map_id_t low-32-bit
 *     */
inline uint32_t get_mapid_low32(map_id_t mapid)
{

	return (mapid & 0x00000000FFFFFFFFULL);

}

/**
 *   * @brief make map_id_t
 *     */
inline map_id_t make_mapid(uint32_t high, uint32_t low)
{

	map_id_t mapid = 0;
	mapid |= high;
	return ((mapid << 32) | low);

}

/*

class PuzzlePlayer {
 public:
  
 private:
   uint32_t book_flag_;
   player_t * player;
};

typedef boost::shared_ptr<PuzzlePlayer> PlayerPtr;

class PuzzleRoom {
 public:

 private:
  uint32_t total_cir;
  uint32_t cur_cir;
  uint32_t prepare_tm;
  uint32_t answer_tm;
  std::vector<uint32_t> rand_puzzles;
  std::List<PlayerPtr> room_players_;
};

class PuzzleRoomManager {
 public:
 private:
};
*/


//for Intelligence test
//
struct intelligence_test_t {
	uint32_t test_id;
	uint32_t right_answer;
	uint32_t reward_exp;
	uint32_t reward_coins;
};


class intelligence_test_room
{
 public:
	 enum {
		 in_total_cir_count = 25,
		 in_prepare_time = 900,
		 in_cir_per_time_part1 = 20,
		 in_cir_per_time_part2 = 10,
	 };
	 intelligence_test_room()
	 {
		 room_id = 0;
		 test_start_tm = 0;
		 cur_question = 0;
		 cur_answer = 0;
		 cur_map = NULL;
	     test_start_prepare_tm = 0;
		 time_mcast_time =0;

	 }

	 ~intelligence_test_room()
	 {
		 intelligence_tests.clear();
	 }

	 void update(uint32_t now_time);
	 //随机题目	
	 void rand_test_2_going();

	 void do_prepare_logic(uint32_t now_time);

	 void do_going_logic(uint32_t now_time);

	 uint32_t room_id;

	 uint32_t cur_question;
	 uint32_t cur_answer;

	 uint32_t test_start_tm;
	 uint32_t test_start_prepare_tm;
	 uint32_t time_mcast_time;

	 map_t * cur_map;

	 std::vector<intelligence_test_t*> intelligence_tests;
};

int load_all_intelligence_test(xmlNodePtr xml);

intelligence_test_t *  get_intelligence_test_info_by_id(uint32_t test_id);

void init_intelligence_test_room();

void proc_intelligence_test_event();

int list_test_room_info_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int set_book_for_fault_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

int try_enter_test_room_cmd(player_t * p, uint8_t * body, uint32_t bodylen);

#endif // KF_MAP_HPP_

