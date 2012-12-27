/*
 * =========================================================================
 *
 *        Filename: timer_func.cpp
 *
 *        Version:  1.0
 *        Created:  2011-05-17 10:40:29
 *        Description:  
 *
 *        Author:  jim (jim@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#include "timer_func.h"
#include "cli_proto.h"
#include "Cmap_conf.h"
#include "Ctimer.h"
#include "global.h"
#include "Cgame.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>
#ifdef __cplusplus
}
#endif


void deal_cahce_cmd(uint32_t key, void*data,int data_len)
{
	DEBUG_LOG("deal_cahce_cmd");
	Csprite * p=g_sprite_map->get_sprite(key);	
	if (p){
		char * data=p->cache_buffer_lst.get_first_buffer();
		if ( data  ) {
			dispatch(data,p->fdsess,false );
			p->cache_buffer_lst.pop_front();
		}
	}else{
		DEBUG_LOG("ERROR no find cache buf userid=%u",key );
	}
}


void deal_all_off_line (uint32_t key, void*data,int data_len)
{
	//处理所有人都下线
	DEBUG_LOG("deal_all_off_line");
    time_t nowtime =time(NULL)+1800;
    struct tm tm_now;
    localtime_r(&nowtime, &tm_now);
	if (tm_now.tm_hour==0){//是凌晨
		DEBUG_LOG("set all off line");
		g_sprite_map->set_all_user_offline();
	}
	//加入下一个时间点	
	g_timer_map->add_timer(TimeVal(get_period_midnight()), n_deal_all_off_line,0);
			
}

// 通知客户端已经连续45分钟在线
void noti_45min_online (uint32_t key, void*data,int data_len)
{
	//通知客户端
	KDEBUG_LOG(key, "noti_45min_online");
	Csprite* p = g_sprite_map->get_sprite(key);
	if (p) {
		//加入下一个时间点	
		cli_noti_user_online_time_out cli_out;
		cli_out.type = 0;
		cli_out.time = GAME_REST_MIN_2;
		p->send_succ_server_noti(cli_noti_user_online_time_cmd, &cli_out);
		p->reset_online_timer();
	}
			
}

void reset_island_time(uint32_t key, void*data,int data_len)
{
	uint32_t islandid = *(uint32_t *)data;
	DEBUG_LOG("reset_island_time");
	g_map_conf_map.reset_island_timer(islandid);
}

void deal_game_timer(uint32_t key, void*data,int data_len)
{
	uint32_t game_map_id = *(uint32_t *)data;
	Csprite* p = g_sprite_map->get_sprite(key);
	KDEBUG_LOG(key, "deal_game_timer\t[%u]", game_map_id);
	if (p && p->game_map_id == game_map_id) {
		g_sprite_map->game_map->do_timer_func(game_map_id, (uint32_t *)((char *)data) + 4);	
	}
}

// 通知客户端累计时间
void noti_total_oltime (uint32_t key, void*data,int data_len)
{
	//通知客户端
	KDEBUG_LOG(key, "noti_total_oltime");
	Csprite* p = g_sprite_map->get_sprite(key);
	if (p) {
		//加入下一个时间点	
		cli_noti_user_online_time_out cli_out;
		cli_out.type = 1;
		uint32_t oltime = p->get_day_oltime();
		cli_out.time = oltime >= GAME_MAX_SEC ? GAME_MAX_MIN : GAME_NORMAL_MIN;
		p->send_succ_server_noti(cli_noti_user_online_time_cmd, &cli_out);
		p->reset_online_timer();
	}
		
}

void sw_report_online_info(uint32_t key, void*data,int data_len)
{
	DEBUG_LOG("sw_report_online_info" );
	//设置没有使用定时器了
	is_add_timer_to_connect_switch=false;
	connect_to_switch();
}

void init_timer_funcs()
{
#define ADD_TIMER_FUNC( func_name ) \
	g_timer_map->add_function(n_##func_name,func_name);
	ADD_TIMER_FUNC(sw_report_online_info);
	ADD_TIMER_FUNC(deal_cahce_cmd);
	ADD_TIMER_FUNC(deal_all_off_line);
	ADD_TIMER_FUNC(deal_game_timer);
	ADD_TIMER_FUNC(noti_45min_online);
	ADD_TIMER_FUNC(noti_total_oltime);
	ADD_TIMER_FUNC(reset_island_time);
}

