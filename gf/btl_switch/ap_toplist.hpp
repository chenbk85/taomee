#ifndef _AP_TOPLIST_H_
#define _AP_TOPLIST_H_
#include <list>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern "C"
{
#include <libtaomee/list.h>
#include <libtaomee/timer.h>
}

using namespace std;

#define MAX_NICK_LEN 16
#define AP_TOPLIST_COUNT 1000
#define AP_SAVE_TIME    3600

typedef struct ap_data
{
	ap_data()
	{
		userid_ = 0;
		role_regtime_ = 0;
		get_time_ = 0;
		ap_point_ = 0;
		memset(nick_, 0, sizeof(nick_));
	}

	int compare(ap_data* pdata)
	{
		if(ap_point_ > pdata->ap_point_)return 1;
		if(ap_point_ < pdata->ap_point_)return -1;
		if(ap_point_ == pdata->ap_point_)
		{
			if( get_time_ < pdata->get_time_)return 1;
			if( get_time_ > pdata->get_time_)return -1;
			if( get_time_ == pdata->get_time_)return 0;
		}
		return 0;
	}
	uint32_t userid_;
	uint32_t role_regtime_;
	uint32_t ap_point_;
	uint32_t get_time_;
	char 	 nick_[MAX_NICK_LEN];
}ap_data;




class ap_toplist
{
public:
	ap_toplist(){}
	~ap_toplist(){}
public:
	bool init(const char* file);
	bool final(const char* file);
	bool add_ap_data(ap_data* pdata);
	void get_ap_data(uint32_t begin_index, uint32_t end_index,  std::vector<ap_data>& datas);	
private:
	std::list<ap_data> m_datas;	
};


struct timer_head_t {
	    list_head_t timer_list;
};

extern ap_toplist ap_list1;
extern ap_toplist ap_list2;
extern timer_head_t g_events;

int save_ap_toplist_data( void* owner, void* data );












#endif
