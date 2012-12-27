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



typedef struct limit_data
{
	limit_data()
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
}limit_data_t;



class limit_data_mrg
{
public:
	typedef std::map <uint32_t, limit_data_t>		limit_data_map_t;
	
	limit_data_mrg()
    {
        daily_reset_flag = 0;
    }
	~limit_data_mrg(){}
public:
	bool read_pre_init(const char* file);
	bool init(const char* file);
	bool final(const char* file);
	limit_data_t* get_limit_data(uint32_t item_id);
	uint32_t reduce_item_cnt(uint32_t item_id, uint32_t cnt);	
	void del_item(uint32_t item_id);
	uint32_t add_item_cnt(uint32_t item_id,uint32_t cnt, uint32_t reset_type = 2);
	void broad_limit_data(limit_data_t* p_data);
	void reset_limit_data(void* data);
	void reset_one_limit_data(limit_data_t* p_data);
	uint32_t get_reset_hour();
	uint32_t get_limit_cnt();
private:
	bool add_limit_data(limit_data_t& data);
private:
	limit_data_map_t m_datas;	
	limit_data_map_t m_init_datas;	
    uint8_t daily_reset_flag;
};



extern limit_data_mrg g_limit_data_mrg;



int add_reset_limit_data_timer( void* owner, void* data);
int save_limit_data( void* owner, void* data);






#endif
