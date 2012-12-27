/*
 * =====================================================================================
 *
 *       Filename:  lanterns_day.c
 *
 *    Description:  元宵节投掷汤圆
 *
 *        Version:  1.0
 *        Created:  01/31/2012 05:13:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee (), ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <math.h>
#include "item.h"
#include "exclu_things.h"
#include "lanterns_day.h"
#include "small_require.h"

//特殊汤圆刷新间隔
#define LD_SP_LANTERN_UPDATE_TIMER				60
#define LD_LANTERN_STATE_ITEM_ID				150017
#define LD_ATTACK_AWARD_ITEM_ID					1351300


//能刷新出特殊汤圆的场景的ID
static uint32_t sp_lantern_map_id[] = { 4, 7, 10, 28, 83 };
static uint32_t sp_map_cnt = sizeof(sp_lantern_map_id) / sizeof(uint32_t);

//场景中特殊汤圆的状态：1表示存在，0表示击破
static uint8_t sp_lantern_info[5];
//当前刷新的场景序号
static uint32_t cur_update_map;

//特殊汤圆被击破后所掉落的奖励信息
static uint32_t sp_lantern_award_info[][3] = { 
	//天使
	{1353277, 1, 1353277},
#ifndef TW_VER
	{1353284, 1, 1353284},
#else
#endif
	{1353283, 1, 1353283},
	{1353281, 1, 1353281},
	{1353249, 1, 1353249},
	{1353206, 1, 1353206},
	//猪			/猪
	{1593000, 1, 1593000},
	{1593001, 1, 1593001},
	{1593006, 1, 1593006},
	{1593007, 1, 1593007},
	{1593026, 1, 1593026},
	{1593024, 1, 1593024},
	{1593063, 1, 1593063},
	{1593064, 1, 1593064},
	{1593061, 1, 1593061},
	//动物	   /动物
	{1270006, 1, 1270006},
	{1270007, 1, 1270007},
	{1270012, 1, 1270012},
	{1270015, 1, 1270015},
	{1270016, 1, 1270016},
	{1270017, 1, 1270017},
	//布多多， 布少少
	{0, 100, 1193},
	{0, 1000, 1194},
	{0, 100, 1193},
};
static uint32_t sp_award_cnt = sizeof(sp_lantern_award_info) / ( sizeof(uint32_t) * 3 );

typedef struct ld_timer {
	list_head_t timer_list;
}__attribute__(( packed )) ld_timer_t;

static ld_timer_t my_timer;

int ld_get_sp_lanterns_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID( p->id );
	int i;
	int j = sizeof(protocol_t);
	int has_sp_lantern = 0;
	DEBUG_LOG( "ld_get_sp_lanterns_info_cmd Player Map ID:[%lu]", p->tiles->id );
	for( i = 0; i < sp_map_cnt; ++i )
	{
		if( p->tiles->id == sp_lantern_map_id[i] )
		{
			if( sp_lantern_info[i] == 1 )
			{
				has_sp_lantern = 1;
			}
			break;
		}
	}
	DEBUG_LOG( "ld_get_sp_lanterns_info_cmd UserID:[%u] Map[%lu] Is Has SPL:[%d]", p->id, p->tiles->id, has_sp_lantern );
	PKG_UINT32( msg, p->tiles->id, j );
	PKG_UINT32( msg, has_sp_lantern, j );
	init_proto_head( msg, p->waitcmd, j );
	return send_to_self( p, msg, j, 1 );
}

int ld_timer_initialize()
{
	DEBUG_LOG( "Lantern's Day SP Lantern Update Timer Begin" );
	memset( sp_lantern_info, 0, sizeof(sp_lantern_info) );
	cur_update_map = rand() % sp_map_cnt;
	INIT_LIST_HEAD(&my_timer.timer_list);
	ADD_TIMER_EVENT( &my_timer, ld_update_sp_lanterns, NULL, get_now_tv()->tv_sec + LD_SP_LANTERN_UPDATE_TIMER );
	return 0;
}

int ld_update_sp_lanterns( void* onwer, void* data )
{
	uint32_t cur_hour = get_now_tm()->tm_hour;
	uint32_t cur_min = get_now_tm()->tm_min;
	int today = get_today();
	int day_begin;
#ifndef TW_VER
	day_begin = 20120203;
#else
	day_begin = 20120210;
#endif
	uint32_t day_pass = today - day_begin;
	DEBUG_LOG( "Lantern's Day SP Lantern Update ... MapID[%u] Hour[%u] Min[%u] DayPass[%u]", cur_update_map, cur_hour, cur_min, day_pass );
	if( ( day_pass < 3 && cur_hour == 14 && cur_min < 30 ) ||
		( day_pass >= 3 && day_pass < 7 && cur_hour == 19 && cur_min >= 30 && cur_min <= 59 )
		)
	{
		sp_lantern_info[cur_update_map] = 1;
		
		int j = sizeof(protocol_t);
		PKG_UINT32( msg, sp_lantern_map_id[cur_update_map], j );
		PKG_UINT32( msg, 1, j );
		init_proto_head( msg, PROTO_LD_GET_SP_LANTERN_INFO, j );
		send_to_all_players( NULL, msg, j, 0 );

		cur_update_map++;
		cur_update_map %= sp_map_cnt;
	}
	ADD_TIMER_EVENT( &my_timer, ld_update_sp_lanterns, NULL, get_now_tv()->tv_sec + LD_SP_LANTERN_UPDATE_TIMER );
	return 0;
}

int ld_attack_player(sprite_t* p_attacker, sprite_t* p_tar)
{
	if( !p_attacker || !p_tar )
	{
		ERROR_LOG( "ld_attack_player Invalid sprite" );
		return 0;
	}

	if (!(p_attacker->fdsess) || !(p_tar->fdsess)){
		ERROR_LOG( "ld_attack_player Invalid sprite" );
		return 0;
	}
	
	DEBUG_LOG( "Attack  Player P1:[%u]  P2[%u]", p_attacker->id, p_tar->id );
	//notice db
	{
		int sendlen = 0;
		uint8_t buff[128];
		item_kind_t* ik = find_kind_of_item( LD_ATTACK_AWARD_ITEM_ID );
		item_t* it = get_item( ik, LD_ATTACK_AWARD_ITEM_ID );
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		pkg_item_kind( p_attacker, buff, LD_ATTACK_AWARD_ITEM_ID, &sendlen );
		PKG_H_UINT32( buff, LD_ATTACK_AWARD_ITEM_ID, sendlen );
		PKG_H_UINT32( buff, 2, sendlen );
		PKG_H_UINT32( buff, it->max, sendlen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p_attacker->id );

		sendlen = 0;
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		pkg_item_kind( p_tar, buff, LD_ATTACK_AWARD_ITEM_ID, &sendlen );
		PKG_H_UINT32( buff, LD_ATTACK_AWARD_ITEM_ID, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, it->max, sendlen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p_tar->id );
	}

	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, p_tar->id, msg_len );
	init_proto_head( msg, PROTO_LD_PLAYER_BE_ATTACKED, msg_len );
	send_to_map( p_tar, msg, msg_len, 0 );

	//for attacker
	msg_len = sizeof(protocol_t);
	//reason 1--attack 2--be attack
	PKG_UINT32( msg, 1, msg_len );
	PKG_UINT32( msg, LD_ATTACK_AWARD_ITEM_ID, msg_len );
	PKG_UINT32( msg, 2, msg_len );
	init_proto_head( msg, PROTO_LD_GET_AWARD, msg_len );
	send_to_self( p_attacker, msg, msg_len, 0 );

	//for the other
	msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, 2, msg_len );
	PKG_UINT32( msg, LD_ATTACK_AWARD_ITEM_ID, msg_len );
	PKG_UINT32( msg, 1, msg_len );
	init_proto_head( msg, PROTO_LD_GET_AWARD, msg_len );
	send_to_self( p_tar, msg, msg_len, 0 );
	return 0;
}

int ld_attack_sp_lanterns_cmd(sprite_t* p, const uint8_t* body, int len)
{
	DEBUG_LOG( "Attack SP Lantern Map:[%lu] UserID:[%u]", p->tiles->id, p->id );
	CHECK_VALID_ID( p->id );
	CHECK_BODY_LEN( len, 0 );
	int mapIdx;
	for( mapIdx = 0; mapIdx < sp_map_cnt; ++mapIdx )
	{
		if( p->tiles->id == sp_lantern_map_id[mapIdx] )
		{
			if( sp_lantern_info[mapIdx] == 1 )
			{
				//扣除一个汤圆
				*(int*)p->session = mapIdx;
				return db_single_item_op( p, p->id, LD_LANTERN_STATE_ITEM_ID, 1, 0 );
			}
			else
			{
				break;
			}
		}
	}
	return send_to_self_error( p, p->waitcmd, -ERR_ld_tar_invalid_attack, 1 );
}

int ld_attack_sp_lanterns_callback(sprite_t* p)
{
	if( !p )
	{
		ERROR_LOG( "ld_attack_sp_lanterns_callback Invalid Sprite" );
		return 0;
	}

	//统计 击破特殊汤圆
	{
		uint32_t msgbuff[2]= {p->id, 1};
		msglog(statistic_logfile, 0x0409C31A, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	//remove sp lantern
	int mapIdx = *(int*)p->session;
	sp_lantern_info[mapIdx] = 0;
	uint32_t award_idx = rand() % sp_award_cnt;
	DEBUG_LOG( "Attack SP Lantern OK Map:[%lu] UserID:[%u] ItemIdx[%u] ItemID[%u] ItemCnt[%u] ItemOutID[%u]", 
		p->tiles->id, p->id, award_idx, sp_lantern_award_info[award_idx][0], sp_lantern_award_info[award_idx][1],
		sp_lantern_award_info[award_idx][2] );
	//notice db
	{
		int sendlen = 0;
		uint8_t buff[128];
		item_kind_t* ik = find_kind_of_item( sp_lantern_award_info[award_idx][0] );
		item_t* it = get_item( ik, sp_lantern_award_info[award_idx][0] );
		PKG_H_UINT32( buff, 0, sendlen );
		PKG_H_UINT32( buff, 1, sendlen );
		PKG_H_UINT32( buff, 203, sendlen );
		PKG_H_UINT32( buff, 0, sendlen );
		pkg_item_kind( p, buff, sp_lantern_award_info[award_idx][0], &sendlen );
		PKG_H_UINT32( buff, sp_lantern_award_info[award_idx][0], sendlen );
		PKG_H_UINT32( buff, sp_lantern_award_info[award_idx][1], sendlen );
		PKG_H_UINT32( buff, it->max, sendlen );
		send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );
	}

	//notice map
	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, p->id, msg_len );
	PKG_UINT32( msg, sp_lantern_award_info[award_idx][2], msg_len );
	init_proto_head( msg, p->waitcmd, msg_len );
	send_to_map( p, msg, msg_len, 1 );
	return 0;
}

