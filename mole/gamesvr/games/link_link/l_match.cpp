/*
 * =====================================================================================
 *
 *       Filename:  cmatch.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/10/2012 01:42:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <cstdlib>
#include <cstring>

extern "C" {
#include <time.h>
#include <libtaomee/log.h>
#include <libxml/tree.h>
#include <gameserv/dll.h>
#include <gameserv/timer.h>
#include <gameserv/proto.h>
#include <gameserv/config.h>
#include <gameserv/dbproxy.h>
#ifndef TW_VER
#include  <statistic_agent/msglog.h>
#endif
}

#include "l_match.h"
#include "c_link.h"


/* 
 * @brief 每次玩家进入
 * @param sprite_t* p 玩家指针 
 * @return 无 
 */
void Cmatch::init(sprite_t *p)
{
	DEBUG_LOG("p->id: %u", p->id);
	if(m_wait_player == 0){
		m_wait_player = p;
	}
	else{
		if(p == m_wait_player){
			return;
		}
		else{

			DEBUG_LOG("wait_id: %u", m_wait_player->id);
			free_game_group( p->group );
			p->group = m_wait_player->group;
			m_wait_player->group->players[1] = p;
			m_wait_player->group->count = 2;
				
#ifndef TW_VER
			{
				//part in
				uint32_t msgbuff[2]= { p->id, 1 };
				msglog( statistic_file, 0x0409C3C0, get_now_tv()->tv_sec, &msgbuff, sizeof(msgbuff) );
			}
#endif
			c_link* game = new c_link(m_wait_player->group, p, m_wait_player);
			m_wait_player->group->game_handler = game;
			//DEBUG_LOG("game address: %x",game);
			m_wait_player = 0;
		}
	}
}
/*
 * @brief notify client which server ready now 
 * @param sprite_t* p 玩家指针 
 */
/*
int Cmatch::notify_server_ready(sprite_t *p)
{
	int l = sizeof (protocol_t);
	init_proto_head(pkg, server_ready, l);
	if (send_to_self (p, pkg, l, 1 ) == 0) {
	} else {
		ERROR_LOG("notify server ready error");
		return -1;
	}
	return 0; 
}
*/
/*
 * @brief 玩家中途退出，游戏组指针空置 
 * @param sprite_t* p 玩家指针 
 * @param uint8_t body[] 数据包
 * @param int len  数据包长度
 */
int  Cmatch::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{

	// player leave game
   if(cmd == proto_player_leave){
	   if(m_wait_player == p){
		   m_wait_player = 0;
	   }
	  
	   p->group->game_handler = NULL;
	   for (int i = 0; i < p->group->count; i++) {
		   p->group->players[i]->waitcmd = 0;
	   }
	
	}
   	return  GER_end_of_game;
}


/*
 * @brief 玩家中途退出，游戏组指针空置 
 * @param sprite_t* p 玩家指针 
 * @param uint32_t id 玩家ID
 * @param const void* buf 数据包
 * @param int len  数据包长度
 * @param uint32_t ret_code 返回值
 */

int  Cmatch::handle_db_return(sprite_t* p, uint32_t id, const void* buf, int len, uint32_t ret_code)
{	
	return 0;
}

/*  
 * @brief 玩家中途退出，游戏组指针空置 
 * @param void* data 
 */
int Cmatch::handle_timeout(void* data)
{

	//if (m_wait_player != 0) {
		//m_wait_player = 0;

	//} else {
		//ADD_TIMER_EVENT(p->group, on_game_timer_expire, u_info, now.tv_sec + 10);
	//}
	return 0;
}

