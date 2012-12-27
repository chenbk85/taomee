/*
 * =====================================================================================
 *
 *       Filename:  advanced_class.c
 *
 *    Description:  先进班级
 *
 *        Version:  1.0
 *        Created:  02/13/2012 02:41:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee (), ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include <libtaomee/list.h>
#include "exclu_things.h"
#include "small_require.h"
#include "fire_cup.h"
#include "advanced_class.h"

/*
* 使用team_id变脸保存玩家的队伍，以及头顶信息
* 红队2100
* 蓝队2200
* 个位数表示当前头顶标记
*/
#define CHECK_VALID_TEAM(team_id)	(team_id/1000 == 2)
#define GET_TEAM_ID(team_id)		((team_id/100)%10)
#define GET_HEAD_FLAG(team_id)		(team_id%10)

#define HEAD_FLAG_MAX				3

#define AC_RED_TEAM_BEGIN			0
#define AC_RED_TEAM_END				3
#define AC_BLUE_TEAM_BEGIN			4
#define AC_BLUE_TEAM_END			7
#define AC_SHOW_STAGE_POS			8
#define AC_COMMENT_POS_BEGIN		9
#define AC_COMMENT_POS_END			11
#define AC_GAME_MAP_ID				38

#define AC_AWARD_GET_DAYTYPE		31356
#define AC_AWARD_DAY_LIMIT			150

static uint32_t sit_info[AC_COMMENT_POS_END + 1] = {0};
static uint32_t red_team_flag	= 0;
static uint32_t blue_team_flag	= 0;
static uint32_t red_team_cnt	= 0;
static uint32_t blue_team_cnt	= 0;
static uint8_t comment_info[3]	= {0};
static uint32_t comment_cnt		= 0;
static uint32_t ac_award_info[][2] = { {1351303,2} };

int ac_change_flag_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_VALID_ID( p->id );
	if( p->tiles->id != AC_GAME_MAP_ID )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_not_valid_map, 1 );
	}
	if( !CHECK_VALID_TEAM(p->team_id) )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_invalid_team, 1 );
	}
	uint32_t tar_id;
	CHECK_BODY_LEN( bodylen, sizeof(tar_id) );
	int j = 0;
	UNPKG_UINT32( buf, tar_id, j );
	CHECK_VALID_ID( tar_id );
	sprite_t* tar_p = get_sprite(tar_id);
	if( !tar_p )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_invalid_player, 1 );
	}

	if( !CHECK_VALID_TEAM(tar_p->team_id) )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_invalid_player, 1 );
	}

	if( GET_HEAD_FLAG(tar_p->team_id) != GET_HEAD_FLAG(p->team_id) )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_invalid_player, 1 );
	}

	//三次内不能和同一个人交互
	if( p->ac_interactive_user_id[0] == tar_p->id ||
		p->ac_interactive_user_id[1] == tar_p->id ||
		tar_p->ac_interactive_user_id[0] == p->id ||
		tar_p->ac_interactive_user_id[1] == p->id
		)
	{
		return send_to_self_error( p, p->waitcmd, -ERR_ac_invalid_interactive_tar, 1 );
	}
	else
	{
		p->ac_interactive_user_id[0]		= p->ac_interactive_user_id[1];
		p->ac_interactive_user_id[1]		= tar_p->id;
		tar_p->ac_interactive_user_id[0]	= tar_p->ac_interactive_user_id[1];
		tar_p->ac_interactive_user_id[1]	= p->id;
	}

	uint32_t cur_team_id = (uint32_t)(p->team_id / 100) * 100 + (GET_HEAD_FLAG(p->team_id) + 1) % HEAD_FLAG_MAX;
	send_request_to_db(SVR_PROTO_SPORT_SIGN, NULL, sizeof(cur_team_id), &cur_team_id, p->id);
	p->team_id = cur_team_id;

	cur_team_id = (uint32_t)(tar_p->team_id / 100) * 100 + (GET_HEAD_FLAG(tar_p->team_id) + 1) % HEAD_FLAG_MAX;
	send_request_to_db(SVR_PROTO_SPORT_SIGN, NULL, sizeof(cur_team_id), &cur_team_id, tar_p->id);
	tar_p->team_id = cur_team_id;
	//notice to map player changen flag
	int msg_len = sizeof( protocol_t );
	PKG_UINT32( msg, p->id, msg_len );
	PKG_UINT32( msg, p->team_id % AC_TEAM_ID_BASE, msg_len );
	PKG_UINT32( msg,tar_p->id, msg_len );
	PKG_UINT32( msg, tar_p->team_id % AC_TEAM_ID_BASE, msg_len );
	init_proto_head( msg, p->waitcmd, msg_len );
	send_to_map( p, msg, msg_len, 0 );

	//if player's flag change to 2,we need to give some award to them
	sprite_t* tmp_sprite[2]	= { p, tar_p };
	int i;
	uint32_t db_wait_cnt = 0;
	for( i = 0; i < 2; ++i )
	{
		if( GET_HEAD_FLAG(tmp_sprite[i]->team_id) == 0 )
		{
			//check award day limit
			db_set_sth_done( p, AC_AWARD_GET_DAYTYPE, AC_AWARD_DAY_LIMIT, tmp_sprite[i]->id );
			db_wait_cnt++;
		}
	}
	if( 0 == db_wait_cnt )
	{
		p->waitcmd = 0;
	}
	else
	{
		*(uint32_t*)p->session = db_wait_cnt;
	}
	return 0;
}

int ac_change_flag_award_day_limit_callback( sprite_t* p, uint32_t user_id )
{
	CHECK_VALID_ID( user_id );
	sprite_t* tar_p = get_sprite(user_id);
	if( !tar_p )
	{
		ERROR_LOG( "ac_change_flag_award_day_limit_callback Invalid ID:[%u]", user_id );
		return 0;
	}
	uint32_t db_wait_cnt = *(uint32_t*)p->session;
	db_wait_cnt--;
	if( 0 == db_wait_cnt )
	{
		p->waitcmd = 0;
	}
	else
	{
		*(uint32_t*)p->session = db_wait_cnt;
	}
	uint32_t award_item_id = 1351303;
	uint32_t award_cnt = 2;
	int sendlen = 0;
	uint8_t buff[128];
	PKG_H_UINT32( buff, 0, sendlen );
	PKG_H_UINT32( buff, 1, sendlen );
	PKG_H_UINT32( buff, 203, sendlen );
	PKG_H_UINT32( buff, 0, sendlen );
	pkg_item_kind( tar_p, buff, award_item_id, &sendlen );
	PKG_H_UINT32( buff, award_item_id, sendlen );
	PKG_H_UINT32( buff, award_cnt, sendlen );
	item_kind_t* ik = find_kind_of_item(award_item_id);
	item_t* it = get_item( ik, award_item_id );
	PKG_H_UINT32( buff, it->max, sendlen );
	send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, tar_p->id );
	int l = sizeof( protocol_t );
	PKG_UINT32( msg, award_item_id, l );
	PKG_UINT32( msg, award_cnt, l );
	init_proto_head( msg, PROTO_AC_AWARD, l );
	return send_to_self( tar_p, msg, l, 0 );
}

#define OP_GET_OFF			3			//从位置上下来
#define OP_COMMENT_YOU		2			//优
#define OP_COMMENT_LIANG	1			//良

int ac_set_sit_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_VALID_ID( p->id );
	if( p->tiles->id != AC_GAME_MAP_ID )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_not_valid_map, 1 );
	}
	uint32_t sit_id;
	uint32_t op;
	CHECK_BODY_LEN( bodylen, sizeof(sit_id) + sizeof(op) );
	int j = 0;
	UNPKG_UINT32( buf, sit_id, j );
	UNPKG_UINT32( buf, op, j );
	if( sit_id < AC_RED_TEAM_BEGIN || sit_id > AC_COMMENT_POS_END ||
		(op != OP_GET_OFF && sit_info[sit_id] != 0 ) || (op == OP_GET_OFF && sit_info[sit_id] != p->id ) )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_ac_invalid_sit, 1 );
	}
	//避免一个人占多个位置 给人评分的不算
	if( op != OP_GET_OFF )
	{
		int i;
		for( i = 0; i < AC_COMMENT_POS_BEGIN; ++i )
		{
			if( sit_info[i] == p->id )
			{
				return send_to_self_error( p, p->waitcmd, -ERR_ac_invalid_op, 1 );
			}
		}
	}

	uint32_t refresh_area_id = 0;
	uint32_t refresh_date = 0;
	uint32_t is_error = 0;
	switch( sit_id )
	{
	case AC_RED_TEAM_BEGIN ... AC_RED_TEAM_END:
		//统计 优秀班级体大比拼参与
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409C31E, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
		if( GET_HEAD_FLAG(p->team_id) == red_team_flag )
		{
			if( op == OP_GET_OFF )
			{
				sit_info[sit_id] = 0;
				red_team_cnt--;
			}
			else
			{
				sit_info[sit_id] = p->id;
				red_team_cnt++;
			}
			if( red_team_cnt == AC_RED_TEAM_END - AC_RED_TEAM_BEGIN + 1 )
			{
				memset( sit_info + AC_RED_TEAM_BEGIN, 0, red_team_cnt*sizeof(uint32_t) );
				red_team_cnt = 0;
				refresh_area_id = 1;
				red_team_flag = rand() % HEAD_FLAG_MAX;
				refresh_date = red_team_flag;
			}
		}
		else
		{
			is_error = 1;
		}
		break;
	case AC_BLUE_TEAM_BEGIN ... AC_BLUE_TEAM_END:
		//统计 优秀班级体大比拼参与
		{
			uint32_t msgbuff[2]= {p->id, 1};
			msglog(statistic_logfile, 0x0409C31E, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
		if( GET_HEAD_FLAG(p->team_id) == blue_team_flag )
		{
			if( op == OP_GET_OFF )
			{
				sit_info[sit_id] = 0;
				blue_team_cnt--;
			}
			else
			{
				sit_info[sit_id] = p->id;
				blue_team_cnt++;
			}
			
			if( blue_team_cnt == AC_BLUE_TEAM_END - AC_BLUE_TEAM_BEGIN + 1 )
			{
				memset( sit_info + AC_BLUE_TEAM_BEGIN, 0, blue_team_cnt*sizeof(uint32_t) );
				blue_team_cnt = 0;
				refresh_area_id = 2;
				blue_team_flag = rand() % HEAD_FLAG_MAX;
				refresh_date = blue_team_flag;
			}
		}
		else
		{
			is_error = 1;
		}
		break;
	case AC_SHOW_STAGE_POS:
		if( op == OP_GET_OFF )
		{
			sit_info[sit_id] = 0;
			//评选者下台时要清空评定值
			memset( comment_info, 0, sizeof(comment_info) );
			comment_cnt = 0;
		}
		else
		{
			sit_info[sit_id] = p->id;
		}
		break;
	case AC_COMMENT_POS_BEGIN ... AC_COMMENT_POS_END:
		{
#ifndef TW_VER
#define DAY1		20120302
#define DAY2		20120303
#define DAY3		20120304
#else
#define DAY1		20120309
#define DAY2		20120310
#define DAY3		20120311
#endif
			uint32_t cur_hour = get_now_tm()->tm_hour;
			uint32_t cur_min = get_now_tm()->tm_min;
			int today = get_today();
			switch(today)
			{
			case DAY1:
				if( !( cur_hour == 19 || ( cur_hour == 20 && cur_min <= 30 ) ) )
				{
					return send_to_self_error( p, p->waitcmd, -ERR_ac_invalid_op, 1 );
				}
				break;
			case DAY2:
			case DAY3:
				if( !( cur_hour == 13 || ( cur_hour == 14 && cur_min <= 30 ) ) )
				{
					return send_to_self_error( p, p->waitcmd, -ERR_ac_invalid_op, 1 );
				}
				break;
			default:
				return send_to_self_error( p, p->waitcmd, -ERR_ac_invalid_op, 1 );
			}

			switch(op)
			{
			case OP_GET_OFF:
				sit_info[sit_id] = 0;
				break;
			case OP_COMMENT_YOU:
			case OP_COMMENT_LIANG:
				
				if( sit_info[AC_SHOW_STAGE_POS] == 0 ||					//没有人在评选台上时不能投票
					comment_info[sit_id - AC_COMMENT_POS_BEGIN] != 0	//已经评选过了
					)
				{
					return send_to_self_error( p, p->waitcmd, -ERR_ac_invalid_op, 1 );
				}
				comment_info[sit_id - AC_COMMENT_POS_BEGIN] = op;
				comment_cnt++;
				if( comment_cnt == AC_COMMENT_POS_END - AC_COMMENT_POS_BEGIN + 1 )
				{
					refresh_area_id = 3;
					comment_cnt = 0;
					sit_info[AC_SHOW_STAGE_POS] = 0;
					memset( comment_info, 0, sizeof(comment_info) );
					//player on show stage can get award by himself by swap cmd
				}
				//送一“优”字
				{
					int sendlen = 0;
					uint8_t buff[128];
					PKG_H_UINT32( buff, 0, sendlen );
					PKG_H_UINT32( buff, 1, sendlen );
					PKG_H_UINT32( buff, 203, sendlen );
					PKG_H_UINT32( buff, 0, sendlen );
					pkg_item_kind( p, buff, 1351303, &sendlen );
					PKG_H_UINT32( buff, 1351303, sendlen );
					PKG_H_UINT32( buff, 1, sendlen );
					item_kind_t* ik = find_kind_of_item(1351303);
					item_t* it = get_item( ik, 1351303 );
					PKG_H_UINT32( buff, it->max, sendlen );
					send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );
				}
				//统计 优秀摩尔评定参与
				{
					uint32_t msgbuff[2]= {p->id, 1};
					msglog(statistic_logfile, 0x0409C320, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
				}
				break;
			default:
				return send_to_self_error( p, p->waitcmd, -ERR_ac_invalid_op, 1 );
				break;
			}
		}
		break;
	default:
		ERROR_LOG( "ac_set_sit_cmd Invalid Sit ID:[%u]", sit_id );
		return send_to_self_error( p, p->waitcmd, -ERR_ac_invalid_sit, 1 );
		break;
	}
	if( 1 == is_error )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_ac_invalid_sit, 1 );
	}
	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, p->id, msg_len );
	PKG_UINT32( msg, sit_id, msg_len );
	PKG_UINT32( msg, op, msg_len );
	init_proto_head( msg, p->waitcmd, msg_len );
	send_to_map( p, msg, msg_len, 1 );

	if( refresh_area_id != 0 )
	{
		if( refresh_area_id != 3 )
		{
			//评选完之后给本队加5分
			{
				int32_t alter[2] = {0,0};
				alter[refresh_area_id - 1] = 5;
				send_request_to_db( SVR_PROTO_WVS_SET_TEAM_POWER, NULL, sizeof(alter), alter, p->id );
			}
			list_head_t* player;
			map_t* p_map = get_map( AC_GAME_MAP_ID );
			if( !p_map )
			{
				ERROR_RETURN( ("ac_set_sit_cmd Invalid Map ID:[%u]", AC_GAME_MAP_ID), -1 );
			}
			list_for_each( player, &p_map->sprite_list_head )
			{
				sprite_t* l = list_entry( player, sprite_t, map_list );
				if( !IS_NPC_ID(l->id) && GET_TEAM_ID(l->team_id) == refresh_area_id )
				{
					l->fp_can_get_award = 1;
				}
			}
		}
		int j = sizeof(protocol_t);
		PKG_UINT32( msg, refresh_area_id, j );
		PKG_UINT32( msg, refresh_date, j );
		init_proto_head( msg, PROTO_AC_AREA_REFRESH, j );
		send_to_map( p, msg, j, 1 );
	}
	return 0;
}

int ac_get_game_info_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_VALID_ID( p->id );
	return send_request_to_db( SVR_PROTO_WVS_GET_TEAM_POWER, p, 0, NULL, p->id );
}

int ac_get_game_info_callback(sprite_t* p, uint32_t red_score, uint32_t blue_score)
{
	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, red_score, msg_len );
	PKG_UINT32( msg, blue_score, msg_len );
	PKG_UINT32( msg, red_team_flag, msg_len );
	PKG_UINT32( msg, blue_team_flag, msg_len );
	int i;
	for( i = 0; i <= AC_COMMENT_POS_END; ++i )
	{
		PKG_UINT32( msg, sit_info[i], msg_len );
	}
	PKG_UINT8( msg, comment_info[0], msg_len );
	PKG_UINT8( msg, comment_info[1], msg_len );
	PKG_UINT8( msg, comment_info[2], msg_len );
	init_proto_head( msg, p->waitcmd, msg_len );			
	return send_to_self( p, msg, msg_len, 1 );
}

int ac_get_award_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_VALID_ID( p->id );
	if( 1 != p->fp_can_get_award )
	{
		return send_to_self_error( p, p->waitcmd, -ERR_fp_can_not_get_award, 1 );
	}
	uint32_t award_id = rand() % ( sizeof(ac_award_info) / (2 * sizeof(uint32_t) ) );

	int sendlen = 0;
	uint8_t buff[128];
	PKG_H_UINT32( buff, 0, sendlen );
	PKG_H_UINT32( buff, 1, sendlen );
	PKG_H_UINT32( buff, 203, sendlen );
	PKG_H_UINT32( buff, 0, sendlen );
	pkg_item_kind( p, buff, ac_award_info[award_id][0], &sendlen );
	PKG_H_UINT32( buff, ac_award_info[award_id][0], sendlen );
	PKG_H_UINT32( buff, ac_award_info[award_id][1], sendlen );
	item_kind_t* ik = find_kind_of_item(ac_award_info[award_id][0]);
	item_t* it = get_item( ik, ac_award_info[award_id][0] );
	PKG_H_UINT32( buff, it->max, sendlen );
	send_request_to_db( SVR_PROTO_EXCHG_ITEM, NULL, sendlen, buff, p->id );

	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, ac_award_info[award_id][0], msg_len );
	PKG_UINT32( msg, ac_award_info[award_id][1], msg_len );
	init_proto_head( msg, p->waitcmd, msg_len );
	return send_to_self( p, msg, msg_len, 1 );
}

int ac_player_leave( uint32_t id )
{
	int i;
	for( i = 0; i <= AC_COMMENT_POS_END; ++i )
	{
		if( sit_info[i] == id )
		{
			sit_info[i] = 0;
			break;
		}
	}
	switch(i)
	{
	case AC_RED_TEAM_BEGIN ... AC_RED_TEAM_END:
		red_team_cnt--;
		break;
	case AC_BLUE_TEAM_BEGIN ... AC_BLUE_TEAM_END:
		blue_team_cnt--;
		break;
	case AC_SHOW_STAGE_POS:
		//评选者下台时要清空评定值
		memset( comment_info, 0, sizeof(comment_info) );
		comment_cnt = 0;
		break;
	case AC_COMMENT_POS_BEGIN ... AC_COMMENT_POS_END:
		return 0;
		break;
	default:
		return 0;
	}
	int msg_len = sizeof(protocol_t);
	PKG_UINT32( msg, id, msg_len );
	PKG_UINT32( msg, i, msg_len );
	PKG_UINT32( msg, OP_GET_OFF, msg_len );
	init_proto_head( msg, PROTO_AC_SET_SIT, msg_len );
	send_to_map3( AC_GAME_MAP_ID, msg, msg_len );
	return 0;
}
