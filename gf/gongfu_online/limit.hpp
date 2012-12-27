#ifndef _LIMIT_H_
#define _LIMIT_H_
#include <list>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>

extern "C"
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
}

enum {
	dragon_globle_limit_id_1 = 100000001,
	dragon_globle_limit_id_2 = 100000002,
	dragon_globle_limit_id_3 = 100000003,
	dragon_globle_limit_id_4 = 100000004,
	dragon_globle_limit_id_5 = 100000005,

	summon_dragon_userid = 100000010,
	summon_dragon_nimbus = 100000011,

	red_banner_id	= 100000020,
	blue_banner_id	= 100000021,
	
};

struct player_t;

typedef struct global_limit_data
{
	global_limit_data()
	{
		type = 0;
		item_id = 0;
		max_cnt = 0;
		left_cnt = 0;
		reset_hour = 0;
	}

	uint32_t type;
	uint32_t item_id;
	uint32_t max_cnt;
	uint32_t left_cnt;
	uint32_t reset_hour;
	uint32_t order;
}global_limit_data_t;



class limit_data_mrg
{
public:
	typedef std::map <uint32_t, global_limit_data_t>		limit_data_map_t;
	
	limit_data_mrg(){}
	~limit_data_mrg(){}
public:
	bool init();
	bool final();
	global_limit_data_t* get_limit_data(uint32_t item_id);
	uint32_t reduce_item_cnt(uint32_t item_id, uint32_t cnt, bool broad_cast = false);
	uint32_t reduce_item_cnt_with_order(uint32_t item_id,uint32_t cnt, uint32_t order, bool broad_cast);
	uint32_t add_item_cnt(uint32_t item_id,uint32_t cnt, bool broad_cast, uint32_t reset_type = 2);
	uint32_t update_item_cnt(uint32_t item_id, uint32_t cnt);	
	uint32_t get_item_cnt(uint32_t item_id);
	int pack_limit_data(uint8_t* buf, uint32_t type);
	void broad_limit_data(global_limit_data_t* p_data);
	bool add_limit_data(global_limit_data_t& data);
	void order_datas();

    int reset_global_limit_data(uint32_t type);
private:
	
	limit_data_map_t m_datas;	
};



void add_red_banner(player_t *p, uint32_t cnt);

void add_blue_banner(player_t *p, uint32_t cnt);

uint32_t get_red_banner_cnt();

uint32_t get_blue_banner_cnt();



/**
  * @brief  get global limit info
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_global_limit_data_cmd(player_t* p, uint8_t* body, uint32_t bodylen);





#endif
