#ifndef _STAGE_CONFIG_H_
#define _STAGE_CONFIG_H_

#include"fwd_decl.hpp"
#include <vector>
#include <map>
#include "player.hpp"

using namespace std;


typedef struct stage_time_data
{
	stage_time_data()
	{
		year_ = -1;	
		month_ = -1;
		week_ = -1;
		day_ = -1;
		begin_hour_ = -1;
		begin_minute_ = -1;
		end_hour_ = -1;
		end_minute_ = -1;
		enter_flag_ = -1;
	}
	int32_t  year_;
	int32_t  month_;
	int32_t  week_;
	int32_t  day_;
	int32_t  begin_hour_;
	int32_t  begin_minute_;
	int32_t  end_hour_;
	int32_t  end_minute_;
	int32_t  enter_flag_;
}stage_time_data;

struct limit_data_t {
	uint32_t count;
	struct {
		uint32_t itemid;
		uint32_t count;
	} itemarr[10];
};



typedef struct stage_data
{
	stage_data()
	{
		stage_id_ = 0;
		min_lv_ = 0;
		max_lv_ = 0;
		pet_flag_ = 0;
		unlimited_item.count = 0;
		condition_item.count = 0;
	}
	void push_stage_time_data(stage_time_data& data)
	{
		time_datas_.push_back(data);
	}
	bool check_level(player_t* p)
	{
		if (min_lv_ && p->lv < min_lv_) {
			return false;
		}
		if (max_lv_ && p->lv > max_lv_) {
			return false;
		}
		return  true;
	}
	bool allow_pet()
	{
		return pet_flag_;
	}
	uint32_t stage_id_;
	uint32_t min_lv_;
	uint32_t max_lv_;
	uint32_t pet_flag_;
	uint32_t free_daily_id;
	limit_data_t unlimited_item;
	limit_data_t condition_item;
	uint32_t coins;
	uint32_t stage_diff;
	uint32_t from;
	uint32_t to;
	uint32_t time_limit_id;
	std::vector<stage_time_data>  time_datas_;
}stage_data;

struct stage_data_arr {
	uint32_t type;
	stage_data stage_diff_data[12];
};


typedef stage_data*  stage_data_ptr;

class stage_data_mgr
{
public:
	stage_data_mgr()
	{ 
		max_stage_id_ = 0;
	}
public:
	//static stage_data_mgr* getInstance();
public:
	bool init(const char* xmlfile);
	bool final();
	bool add_data( stage_data* pData);
	stage_data* get_data_by_stage_id(uint32_t stage_id, uint32_t stage_diff = 0);
	uint32_t get_max_stage_id();
public:
	std::map<uint32_t,  stage_data_arr> stage_datas_;
	uint32_t max_stage_id_;
};

uint32_t check_player_enter_stage(stage_data_mgr* mgr, player_t* p,  uint32_t stage_id, uint32_t stage_diff, bool is_free = true);

int check_player_enter_stage_time(stage_data_mgr * mgr, uint32_t stage_id);



bool string2time(const char* str,  int32_t& hour, int32_t& minute);
uint32_t check_and_reduce_coins(player_t *p, uint32_t coin);
int  get_stage_errcode(uint32_t stage_id);

uint32_t get_stage_begin_time(uint32_t stage_id, uint32_t stage_diff = 0);
uint32_t get_stage_end_time(uint32_t stage_id, uint32_t stage_diff = 0);

#endif
