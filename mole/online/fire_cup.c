/*
 * =====================================================================================
 *
 *       Filename:  fire_cup.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  07/26/2010 05:14:02 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <statistic_agent/msglog.h>
#include<libtaomee/project/utilities.h>
#include <statistic_agent/msglog.h>
#include "exclu_things.h"
#include "swap.h"
#include "game_bonus.h"
#include "fire_cup.h"
#include "small_require.h"

/* @brief 火神杯报名
 * 2012 春天vs冬天 阵营选择 ID分段从1000开始
 * 2012 先进班级队伍选择	ID分段从2000开始
 */

int sign_fire_cup_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	uint32_t team_id;
	CHECK_BODY_LEN(bodylen, sizeof(team_id));
	int j = 0;
	UNPKG_UINT32( buf, team_id, j );

	team_id = rand() % 2 + 1 ;
	pkg_host_uint32(p->session, team_id);
	return send_request_to_db(SVR_PROTO_SPORT_SIGN, p, sizeof(team_id), &team_id, p->id);
}

/* @brief 火神杯报名的回调函数
 */
int sign_fire_cup_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	int needAddChess = 0;
	sprite_t* pAddPlayer = NULL;
	switch( p->waitcmd )
	{
	case PROTO_WVS_SWITCH_TEAM:
		{
			uint32_t team_id;
			uint32_t tar_id;
			unpkg_host_uint32_uint32( p->session, &team_id, &tar_id );
			sprite_t* tar_sprite = get_sprite(tar_id);
			if( !tar_sprite )
			{
				ERROR_LOG( "sign_fire_cup_callback player not valid ID:[%u]", tar_id );
			}
			else
			{
				if( tar_sprite->team_id / WVS_TEAM_ID_BASE != 1 )
				{
					needAddChess = 1;
					pAddPlayer = tar_sprite;
				}
				tar_sprite->team_id = team_id;
			}

			if( team_id / WVS_TEAM_ID_BASE != 1 )
			{
				team_id = 0;
			}
			else
			{
				team_id -= WVS_TEAM_ID_BASE;
			}
			response_proto_uint32_uint32( p, p->waitcmd, tar_id, team_id, 1 );
			//通知全局表更新冬天春天双方阵营能量
			{
				int32_t alter[2] = {0,0};
				if( team_id == 1 )
				{
					alter[0] = 1;
					alter[1] = ( 1 == needAddChess ? 0 : -1);
				}
				else if( team_id == 2 )
				{
					alter[0] = ( 1 == needAddChess ? 0 : -1);
					alter[1] = 1;
				}
				else
				{
					ERROR_LOG( "sign_fire_cup_callback switch team Invalid TeamID:[%u], UserID:[%u]", team_id, tar_id );
				}
				send_request_to_db( SVR_PROTO_WVS_SET_TEAM_POWER, NULL, sizeof(alter), alter, p->id );
			}
		}
		break;
	case PROTO_SIGN_FIRE_CUP:
		{
			uint32_t team_id;
			unpkg_host_uint32(p->session, &team_id);
			p->team_id = team_id;

			uint32_t msg_buff[2] = {p->id, 1};
	        msglog(statistic_logfile, 0x0409C348+team_id-1, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));
	
			response_proto_uint32_uint32( p, p->waitcmd, p->id, p->team_id, 1 );
			return 0;
		}
		break;
	case  PROTO_USER_CHANGE_LAHM_SPORT_TEAM:
		{
			uint32_t team_id = 0;
			team_id = *(uint32_t*)p->session;
			p->team_id = team_id;
			response_proto_uint32_uint32( p, p->waitcmd, p->id, p->team_id, 0 );
			return 0;
		}
	default:
		{
			return 0;
		}
		break;
	}
	if( pAddPlayer && 1 == needAddChess )
	{
		uint32_t item_id = pAddPlayer->team_id % WVS_TEAM_ID_BASE == 1 ? 1351289 : 1351290;
		uint32_t item_cnt = 20;
		int sendlen = 0;
		uint8_t buff[128];
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		item_kind_t* ik = find_kind_of_item(item_id);
		item_t* it = get_item( ik, item_id );
		pkg_item_kind( p, buff, item_id, &sendlen );
		PKG_H_UINT32( buff, item_id, sendlen );
		PKG_H_UINT32( buff, item_cnt, sendlen );
		PKG_H_UINT32( buff, it->max, sendlen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );
	}
	return 0;
}

/* @brief 得到用户报名的队
 */
int get_team_id_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_GET_SPORT_TEAM, p, 0, NULL, p->id);
}

/* @brief 得到用户报名队的回调函数
 */
int get_team_id_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t team_id = 0;
	CHECK_BODY_LEN(len, sizeof(team_id));
	unpkg_host_uint32((uint8_t *)buf, &team_id);
	if (p->waitcmd == PROTO_LOGIN) {
		p->team_id = team_id;
		DEBUG_LOG("team_id: %u ====!!!!!!", p->team_id);
		uint32_t taskid = 300;
		DEBUG_LOG("!1!!team: %u", p->team_id);
		//give two team awards
		if((p->team_id/100)%10 == 1 || (p->team_id/100)%10 == 2){
			uint32_t b_flag = set_only_one_bit(p, 212);
			if(b_flag){
				uint32_t team_id = (p->team_id/100)%10;
				DEBUG_LOG("===team: %u", p->team_id);
				if(team_id == 1){
					db_exchange_single_item_op(p, 202, 161045, 1, 0);
#ifdef TW_VER
					send_postcard("公民管理处", 0, p->id, 1000313, "", 0);
#else
					send_postcard("公民管理處", 0, p->id, 1000313, "", 0);
#endif
				}
				else if(team_id == 2){//champion team
					db_exchange_single_item_op(p, 202,  161044, 1, 0);
#ifdef TW_VER
					send_postcard("公民管理处", 0, p->id, 1000312, "", 0);
#else
					send_postcard("公民管理處", 0, p->id, 1000312, "", 0);
#endif
				}
			}
		}

	 	return send_request_to_db(SVR_PROTO_GET_TASK_INFO, p, 4, &taskid, p->id);
	}
	if( team_id / AC_TEAM_ID_BASE != 1 )
	{
		team_id = 0;
	}
	else
	{
		team_id %= AC_TEAM_ID_BASE;
	}
	response_proto_uint32(p, p->waitcmd, team_id, 0);
	return 0;
}

/* @brief 得到各个队的奖牌数
 */
int get_team_medal_num_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_GET_FIRE_CUP_TEAM_MEDAL, p, 0, NULL, p->id);
}

/* @brief 得到各个队的奖牌数的回调函数
 */
int get_team_medal_num_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	typedef struct {
		uint32_t id;
		uint32_t num;
	}__attribute__((packed)) team_id_medal_num_t;

	uint32_t count = 0;
	unpkg_host_uint32((uint8_t *)buf, &count);
	team_id_medal_num_t *info = (team_id_medal_num_t *)(buf + sizeof(count));

	int i = 0;
	int bytes = sizeof(protocol_t);
	for (i = 0; i < count; i++) {
		uint32_t team_count = (info + i)->num;
		PKG_UINT32(msg, (info + i)->id, bytes);
		PKG_UINT32(msg, (info + i)->num, bytes);
		msglog(statistic_logfile, 0x020D0236 + ((info + i)->id - 1), get_now_tv()->tv_sec, &team_count, sizeof(uint32_t));
	}
	init_proto_head(msg, p->waitcmd, bytes);
	return send_to_self(p, msg, bytes, 1);
}

/* @brief 得到个人的奖牌数
 */
int get_sprite_medal_num_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	return send_request_to_db(SVR_PROTO_GET_FIRE_CUP_NUM, p, 0, NULL, p->id);
}

/* @brief 得到个人奖牌数的回调函数
 */
int get_sprite_medal_num_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	uint32_t num = 0;
	uint32_t team_id = 0;
	unpkg_host_uint32_uint32((uint8_t *)buf, &team_id, &num);
	if (p->waitcmd == PROTO_EXCHANGE_STH) {
		return itemin_cmd_get_callback(p, id, buf, len);
	}
	response_proto_uint32_uint32(p, p->waitcmd, team_id, num, 0);
	return 0;
}

/* @brief 增加奖牌数
 */
int add_medal_num_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	int num = 0;
	CHECK_BODY_LEN(bodylen, sizeof(num));
	return send_request_to_db(SVR_PROTO_SPORT_ADD_MEDAL, p, sizeof(num), &num, p->id);
}

/* @brief 增加奖牌数的回调函数
 */
int add_medal_num_callback(sprite_t *p, uint32_t id, const char *buf, int len)
{
	/*用纹章兑换物品*/
	if (p->waitcmd == PROTO_EXCHANGE_STH) {
		uint32_t team_id = 0;
		uint32_t num = 0;
		unpkg_host_uint32_uint32((uint8_t *)buf, &team_id, &num);
		response_proto_uint32(p, p->waitcmd, num, 0);
		return 0;
	}

	CHECK_BODY_LEN(len, 16);
	uint32_t teamid = 0;
	teamid = *(uint32_t*)buf;
	uint32_t medal = 0;

	uint32_t gameid = *(uint32_t*)p->session;
	uint32_t i = *(uint32_t*)(p->session + 4);
	medal = *(uint32_t*)(p->session + 8);

	uint32_t buff[2] = { };
	buff[0] = teamid;
	buff[1] = medal;

//	send_request_to_db(SVR_PROTO_SYS_ADD_TEAM_MEDAL, NULL, 8, buff, p->id);

	send_game_bonus_to_db(p, gameid, i);

	return 0;
}

int add_fire_cup_medal_count(sprite_t* p, uint32_t item_id, int32_t item_cnt)
{
	if (p->team_id != 0 && item_id == 190872 && item_cnt != 0) {
		DEBUG_LOG("add fire cup medal: uid[%u] item[%u] count[%d]", p->id, item_id, (int32_t)item_cnt);
		uint32_t buff[2] = {p->team_id, item_cnt};
		send_request_to_db(SVR_PROTO_SYS_ADD_TEAM_MEDAL, NULL, 8, buff, p->id);
	}
	return 0;
}

int add_lahm_sport_medal_count(sprite_t* p, uint32_t item_id, int32_t item_cnt)
{	
	return 0;
}



