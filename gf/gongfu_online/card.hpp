/*
 * =====================================================================================
 *
 *       Filename:  card.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/22/2011 02:58:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus@taomee.com
 *        Company:  Taomee.sh
 *
 * =====================================================================================
 */
#ifndef CARD_HPP__
#define CARD_HPP__
extern "C" {
#include <stdint.h>
}


struct player_t;

enum {
	max_set_cnt = 7, 
	max_card_set_id = 1000,
	max_set_for_card = 20,
};

struct card_t
{
	uint32_t card_id;
	uint32_t item_id;
	uint32_t star_cnt;
	uint32_t set_cnt;
	uint32_t sets[max_set_for_card];
};

card_t * get_card_by_id(uint32_t id);

struct card_set_t 
{
	uint32_t set_id;
	uint32_t card_cnt;
	std::list<uint32_t> * card_list;
	card_set_t(uint32_t id):set_id(id)
	{
		card_cnt = 0;
		card_list = new std::list<uint32_t>;
	}
	card_set_t()
	{
		card_cnt = 0;
		card_list = new std::list<uint32_t>;
	}
	~card_set_t()
	{
		if (!card_list->empty())
		{
			card_list->clear();
		}
		delete card_list;
	}
};

card_set_t * get_card_set(uint32_t set_id);

struct set_reward_t 
{
	int32_t set_id;
	int32_t swap_id;
};


struct player_card_t {
	uint32_t card_id;
	uint32_t card_type;
	uint32_t card_set;
	player_card_t(uint32_t id, uint32_t type, uint32_t set) :
		card_id(id), card_type(type), card_set(set)
	{

	}
}__attribute__((packed));

typedef std::map<uint32_t, player_card_t> PCardMap;
typedef std::map<uint32_t, player_card_t>::iterator PCardMapIterator;

struct player_card_set_t 
{
	uint32_t set_id;

	std::map<uint32_t, player_card_t> * set_card_info;

	player_card_set_t()
	{
		set_id = 0;
		set_card_info = new std::map<uint32_t, player_card_t>;
	}

	~player_card_set_t()
	{
		delete set_card_info;
	}
};

player_card_set_t * get_player_card_set(uint32_t set);

typedef std::map<uint32_t, player_card_set_t*> PCardSetMap;
typedef std::map<uint32_t, player_card_set_t*>::iterator PCardSetMapIterator;




void insert_card_to_set(player_t *p, player_card_t * card); 

void earse_card_drom_set(player_t *p,
		                 uint32_t card_id,
						 uint32_t card_type,
						 uint32_t set_id);

struct db_insert_card_rsp_t 
{
	uint32_t item_id;
	uint32_t card_id;
	uint32_t card_type;
	uint32_t card_set;
}__attribute__((packed));

struct db_get_card_list_rsp_t
{
	uint32_t card_cnt;
	player_card_t cards[];
}__attribute__((packed));

typedef db_insert_card_rsp_t db_erase_card_rsp_t;


int load_cards(xmlNodePtr xml);

/*

card_set_t * get_card_set(uint32_t set_id);
*/

int db_get_card_list(player_t * p);

int db_get_card_list_callback(player_t* p,
	   			       		  userid_t id,
							  void* body,
							  uint32_t bodylen,
							  uint32_t ret);

int db_insert_card(player_t *p,
				   uint32_t item_id,
		           uint32_t card_type,
				   uint32_t card_set);

int db_insert_card_callback(player_t * p,
		                    userid_t id,
							void * body,
							uint32_t bodylen,
							uint32_t ret);


int db_erase_card(player_t * p,
				  uint32_t item_id,
		          uint32_t card_id,
				  uint32_t card_type,
				  uint32_t card_set);

int db_erase_card_callback(player_t *p,
		                   userid_t id,
						   void * body,
						   uint32_t bodylen,
						   uint32_t ret);

int player_get_card_list_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int player_insert_card_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

int player_erase_card_cmd(player_t * player, uint8_t * body, uint32_t bodylen);

bool can_reward_card_set(player_t * p, uint32_t card_set);

int card_set_reward_cmd(player_t * p, uint8_t * body, uint32_t bodylen);        

uint32_t get_set_id_by_swap_id(uint32_t swap_id);
#endif


