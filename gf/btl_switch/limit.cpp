#include <sys/time.h>
extern "C" {
#include <stdint.h>
#include <async_serv/mcast.h>
}
#include <libtaomee++/inet/pdumanip.hpp>


#include "limit.hpp"
#include "mcast_proto.hpp"
#include "online.hpp"
#include "ap_toplist.hpp"

using namespace taomee;


limit_data_mrg g_limit_data_mrg;


bool limit_data_mrg::read_pre_init(const char* file)
{
	char text[1024] = { 0 };
	FILE* fp = fopen(file, "rb");
	if(fp == NULL)return false;

	while(!feof(fp))
	{
		limit_data_t data;
		fgets(text, sizeof(text), fp);
		//type 类型主要用于判别重置时间，活动类型等 特规定1000-2000之间为每天清空
		//item_id 类型, 有表示物品id 有表示兑换id
		//max_cnt 最大上限
		//left_cnt 剩余数量
		int ret = sscanf(text, "%u\t%u\t%u\t%u\t%u", 
							&data.type,
							&data.item_id,
							&data.max_cnt,
							&data.left_cnt,
							&data.reset_hour
			   			);
		
		if(ret == 5)
		{
			printf("init limit data: %u %u %u %u\n", data.type, data.item_id, data.left_cnt, 
				data.reset_hour);
			m_init_datas.insert(limit_data_map_t::value_type(data.item_id, data));
		}
	}
	fclose(fp);

	return true;
}

bool limit_data_mrg::init(const char* file)
{
	read_pre_init("init_limit_data.txt");

	char text[1024] = { 0 };
	FILE* fp = fopen(file, "rb");
	if(fp == NULL)return false;

	while(!feof(fp))
	{
		limit_data_t data;
		fgets(text, sizeof(text), fp);
		//米米号,角色号,玩家名字,成就点,最后更新时间，
		int ret = sscanf(text, "%u\t%u\t%u\t%u\t%u", 
							&data.type,
							&data.item_id,
							&data.max_cnt,
							&data.left_cnt,
							&data.reset_hour
			   			);
		printf("limit data: %u %u %u %u\n", data.type, data.item_id, data.left_cnt, 
			data.reset_hour);
		if(ret == 5)
		{
			add_limit_data(data);
		}
	}
	fclose(fp);

	//add..
	if (!get_limit_data(1136)) {
		limit_data_t data;
		data.item_id = 1136;
		data.max_cnt = 50000;
		data.left_cnt = 50000;
		data.type = 3;
		add_limit_data(data);
	}
	if (!get_limit_data(1163)) {
		limit_data_t data;
		data.item_id = 1163;
		data.max_cnt = 15;
		data.left_cnt = 15;
		data.type = 3;
		add_limit_data(data);
	}
	if (!get_limit_data(1165)) {
		limit_data_t data;
		data.item_id = 1165;
		data.max_cnt = 1000;
		data.left_cnt = 1000;
		data.type = 3;
		add_limit_data(data);
	}


	limit_data_map_t::iterator it = m_init_datas.begin();
	for ( ; it != m_init_datas.end(); ++it) {
		limit_data_t data = it->second;
		if (!get_limit_data(data.item_id)) {
			add_limit_data(data);
		}
	}
	return true;
}

bool limit_data_mrg::final(const char* file)
{
	FILE* fp = fopen(file, "wb");
	if(fp == NULL)return false;

	DEBUG_LOG("save limit data:%u", (uint32_t)m_datas.size());
	limit_data_map_t::iterator pItr = m_datas.begin();
	for( ; pItr != m_datas.end(); ++pItr)
	{
		limit_data_t* pdata= &(pItr->second);
		fprintf(fp, "%u\t%u\t%u\t%u\t%u\n",
				    pdata->type,
					pdata->item_id,
					pdata->max_cnt,
					pdata->left_cnt,
					pdata->reset_hour
				);
		DEBUG_LOG("%u %u %u %u %u", pdata->type,
					pdata->item_id,
					pdata->max_cnt,
					pdata->left_cnt,
					pdata->reset_hour);
	}
	fclose(fp);
	return true;
}

uint32_t limit_data_mrg::get_limit_cnt()
{
	uint32_t tmp = m_datas.size();
	return tmp;
}


bool limit_data_mrg::add_limit_data(limit_data_t& data)
{
	DEBUG_LOG("add limit data: %u %u %u", data.type, data.item_id, data.left_cnt);
	m_datas.insert(limit_data_map_t::value_type(data.item_id, data));
	return true;
}

limit_data_t* limit_data_mrg::get_limit_data(uint32_t item_id)
{
	limit_data_map_t::iterator it = m_datas.find(item_id);
	if (it == m_datas.end()) {
		return 0;
	}
	return &(it->second);
}

uint32_t limit_data_mrg::reduce_item_cnt(uint32_t item_id,uint32_t cnt)
{
	limit_data_map_t::iterator it = m_datas.find(item_id);
	if (it == m_datas.end()) {
		return 0;
	}
	DEBUG_LOG("reduce limit data: %u %u", item_id, cnt);
	limit_data_t* p_data = &(it->second);
	p_data->left_cnt = p_data->left_cnt > cnt ? p_data->left_cnt - cnt : 0;

	broad_limit_data(p_data);
	return p_data->left_cnt;
}

void limit_data_mrg::del_item(uint32_t item_id)
{
	m_datas.erase(item_id);
}


uint32_t limit_data_mrg::add_item_cnt(uint32_t item_id,uint32_t cnt, uint32_t reset_type)
{
	limit_data_map_t::iterator it = m_datas.find(item_id);
	if (it == m_datas.end()) {
		limit_data_t tmp_data;
		tmp_data.type = reset_type;
		tmp_data.item_id = item_id;
		tmp_data.left_cnt = cnt;
		add_limit_data(tmp_data);
		broad_limit_data(&tmp_data);
		return cnt;
	}
	DEBUG_LOG("update data: %u %u", item_id, cnt);
	
	limit_data_t* p_data = &(it->second);
	p_data->left_cnt += cnt;

	broad_limit_data(p_data);
	return p_data->left_cnt;
}

uint32_t limit_data_mrg::get_reset_hour()
{
	limit_data_map_t::iterator it = m_datas.begin();
	if (it == m_datas.end()) {
		return 0;
	}
	return it->second.reset_hour;
}

void limit_data_mrg::reset_one_limit_data(limit_data_t* p_data)
{
	DEBUG_LOG("reset_limit_data %u: %u -> %u", p_data->item_id, p_data->left_cnt, p_data->max_cnt);
	p_data->left_cnt = p_data->max_cnt;
}

void limit_data_mrg::reset_limit_data(void* data)
{
	if (data == (void*)1) {
	//hour reset
		limit_data_map_t::iterator it = m_datas.begin();
		for (; it != m_datas.end(); ++it) {
			if (it->second.type == 3) {
//				limit_data_t* p_data = &(it->second);
				//reset_one_limit_data(p_data);
			}
		}
	}

	if (data == (void*)2) {
	//daily reset -- 14 hour
		limit_data_map_t::iterator it = m_datas.begin();
		for (; it != m_datas.end(); ++it) {
			if (it->second.type == 1 || it->second.type == 3 ||
				(it->second.type >= 1000 && it->second.type < 2000)) {
				limit_data_t* p_data = &(it->second);
				reset_one_limit_data(p_data);
			}
		}
	}
	if (data == (void*)3) {
	//week reset
		struct tm tm_tmp = *get_now_tm();
		if (tm_tmp.tm_wday == 4 && tm_tmp.tm_hour > 12) {
			del_item(100000010);
			del_item(100000011);
		}
		if (tm_tmp.tm_wday == 5 && tm_tmp.tm_hour < 12) {
			del_item(100000010);
			del_item(100000011);
		}
	}
	if (data == (void*)4) {
	//daily reset -- 0 hour
        daily_reset_flag = 0;
		limit_data_map_t::iterator it = m_datas.begin();
		for (; it != m_datas.end(); ++it) {
			if (it->second.type == 4) {
				limit_data_t* p_data = &(it->second);
				reset_one_limit_data(p_data);
			}
		}
	}

    if (data == (void*)7 && !daily_reset_flag) {
	//daily real time
		limit_data_map_t::iterator it = m_datas.begin();
		for (; it != m_datas.end(); ++it) {
			if (it->second.type == 7) {
				limit_data_t* p_data = &(it->second);
				reset_one_limit_data(p_data);
			}
		}
        daily_reset_flag = 1;
	}

	broad_limit_data(0);
	return;
}
void limit_data_mrg::broad_limit_data(limit_data_t* p_data)
{
struct limit_data_mcast_header {
	uint32_t type;//0 reset; 1 update
	uint32_t cnt;
};

struct limit_data_mcast_item {
	uint32_t item_id;
	uint32_t item_cnt;
};
	if (m_datas.size() == 0) {
		return ;
	}
	int idx = sizeof(mcast_pkg_t);
	
	if (p_data) {
		pack_h(s_pkg_, 1, idx);
		pack_h(s_pkg_, 1, idx);
		pack_h(s_pkg_, p_data->type, idx);
		pack_h(s_pkg_, p_data->item_id, idx);
		pack_h(s_pkg_, p_data->left_cnt, idx);
	} else {
		uint32_t cnt = m_datas.size();
		pack_h(s_pkg_, 0, idx);
		pack_h(s_pkg_, cnt, idx);

		limit_data_map_t::iterator it = m_datas.begin();
		for (; it != m_datas.end(); ++it) {
			pack_h(s_pkg_, it->second.type, idx);
			pack_h(s_pkg_, it->second.item_id, idx);
			pack_h(s_pkg_, it->second.left_cnt, idx);
		}
	}
    init_mcast_pkg_head(s_pkg_, 60006, 0);
    send_mcast_pkg(s_pkg_, idx);
}

void init_reset_timer(void* data)
{
	struct tm tm_tmp = *get_now_tm();
	DEBUG_LOG("in reset_limit_data_timer: %u %u %u", tm_tmp.tm_mday, tm_tmp.tm_hour, tm_tmp.tm_min);

	uint32_t reset_hour = tm_tmp.tm_hour;
	uint32_t reset_min = 0;
	uint32_t reset_sec = 0;
	uint32_t run_sec = 0;
	int32_t int_tm = 0;

	if (data == (void*)1) {
		reset_min = 0;
		reset_sec = 0;
		run_sec = 60 * 60;
	} else if (data == (void*)2) {
		reset_hour = 14;
		reset_min = 0;
		reset_sec = 0;

		run_sec = 60 * 60 * 24;
	} else if (data == (void*)3) {
		reset_hour = 23;
		reset_min = 59;
		reset_sec = 59;

		run_sec = 60 * 60 * 24;
	} if (data == (void*)4) {
		reset_hour = 23;
		reset_min = 59;
		reset_sec = 59;

		run_sec = 60 * 60 * 24;
	} 

	
	tm_tmp.tm_hour = reset_hour;
	tm_tmp.tm_min  = reset_min;
	tm_tmp.tm_sec  = reset_sec;
	int_tm	 = mktime(&tm_tmp);
	
	if (get_now_tv()->tv_sec >= int_tm) {
		int_tm += run_sec;
	} 


	char now[64];
	time_t tt = (time_t)int_tm;
	struct tm *ttime; 
	ttime = localtime(&tt);
	strftime(now, 64, "%Y-%m-%d %H:%M:%S", ttime); 
	DEBUG_LOG("add timer type:%p  at %s", data, now);
	
	ADD_TIMER_EVENT(&g_events, add_reset_limit_data_timer,  data, int_tm);

}

int add_reset_limit_data_timer( void* owner, void* data)
{
	if (data == (void*)0) {

		init_reset_timer(reinterpret_cast<void*>(1));
		init_reset_timer(reinterpret_cast<void*>(2));
		init_reset_timer(reinterpret_cast<void*>(3));
		init_reset_timer(reinterpret_cast<void*>(4));
		//ADD_TIMER_EVENT(&g_events, add_reset_limit_data_timer,  reinterpret_cast<void*>(1), int_tm);
		//ADD_TIMER_EVENT(&g_events, add_reset_limit_data_timer,  reinterpret_cast<void*>(2), int_tm);
		//ADD_TIMER_EVENT(&g_events, add_reset_limit_data_timer,  reinterpret_cast<void*>(3), int_tm);
	} else {
		g_limit_data_mrg.reset_limit_data(data);
		init_reset_timer(data);
	}
	return 0;
}

int save_limit_data( void* owner, void* data)
{
	g_limit_data_mrg.final("limit_data.txt");
	ADD_TIMER_EVENT(&g_events, save_limit_data,  reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 60);
	return 0;
}

