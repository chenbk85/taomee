#ifndef _BUY_ITEM_LIMIT_H_
#define _BUY_ITEM_LIMIT_H_


#include"fwd_decl.hpp"
#include <vector>
#include <map>

struct player_t;

enum limit_type 
{
	begin_limit_type = 0,
	day_limit_type,
	week_limit_type,
	month_limit_type,
	end_limit_type 
};


typedef struct buy_item_limit_data
{
	buy_item_limit_data()
	{
		item_id = 0;
		item_limit_type = 0;
		item_limit_count = 0;
	}
	uint32_t item_id;
	uint32_t item_limit_type;
    uint32_t item_limit_count;	
}buy_item_limit_data;




typedef struct player_item_limit_data
{	
	player_item_limit_data()
	{
		item_id = 0;
		last_time = 0;
		item_count = 0;
		item_limit_type = 0;
	}
	bool check_data_expiry()
	{
		time_t cur_time = time(NULL);
		time_t old_time = last_time;
	    struct tm s_new_time = { 0 };
		struct tm s_old_time = { 0 };

		localtime_r(&cur_time, &s_new_time);	 
		localtime_r(&old_time, &s_old_time);
		switch( item_limit_type )
		{
			case 1://check in same day 
			{
				return s_old_time.tm_mday != s_new_time.tm_mday || 
					   s_old_time.tm_mon  != s_new_time.tm_mon || 
					   s_old_time.tm_year != s_new_time.tm_year;
			}
			break;

			case 2://check in same week
			{
				return s_old_time.tm_wday != s_new_time.tm_wday || 
					   s_old_time.tm_mon  != s_new_time.tm_mon || 
					   s_old_time.tm_year != s_new_time.tm_year;
			}
			break;

			case 3://check in same month
			{
				return s_old_time.tm_mon  != s_new_time.tm_mon ||
						s_old_time.tm_year != s_new_time.tm_year;
			}
			break;

			default:
				return false;
		}	
	}
	uint32_t item_id;
	uint32_t item_limit_type;
	uint32_t last_time;
	uint32_t item_count;
}player_item_limit_data;


typedef struct get_buy_item_limit_list_rsp_t
{
	uint32_t count;
	player_item_limit_data data[];
}get_buy_item_limit_list_rsp_t;


class buy_item_limit_mgr
{
public:
	buy_item_limit_mgr(){}
	~buy_item_limit_mgr(){}
public:
	//static buy_item_limit_mgr* getInstance();	
public:
	bool init(const char* xml);
	bool final();
	bool add_buy_item_limit_data( buy_item_limit_data* pdata );
	bool is_buy_item_limit_exist(uint32_t item_id);
	buy_item_limit_data* get_limit_data_by_item_id(uint32_t item_id);
private:
	std::map<uint32_t, buy_item_limit_data*> m_data_map;
};


int get_buy_item_limit_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int send_buy_item_limit_list(player_t* p);

bool init_player_buy_item_limit_data(player_t* p);
bool final_player_buy_item_limit_data(player_t* p);

player_item_limit_data* get_player_item_limit_data_by_item_id(player_t* p, uint32_t item_id);
bool check_player_buy_item_limit_data(buy_item_limit_mgr* mgr, player_t* p, uint32_t item_id, uint32_t item_count = 1);
bool add_player_buy_item_limit_data( buy_item_limit_mgr* mgr,  player_t* p,  uint32_t item_id, uint32_t item_count = 1);
bool init_player_buy_item_limit_list(player_t* p, get_buy_item_limit_list_rsp_t* rsp);





//////////////////////db interface///////////////////////////////
int	db_get_buy_item_limit_list(player_t* p);
int db_get_buy_item_limit_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int db_set_buy_item_limit_data(player_t* p,  player_item_limit_data* pdata);
#endif




