/*
 * =====================================================================================
 *
 *       Filename:  winter_vs_spring.c
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  01/09/2012 03:31:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee (), ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "exclu_things.h"
#include "small_require.h"
#include "fire_cup.h"
#include "winter_vs_spring.h"
#include "advanced_class.h"

#define WAS_SWITCH_TEAM_ATTACK_CNT	5
#define WAS_ATTACK_CD				1

int wvs_team_switch_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	typedef struct cmd_info_s
	{
		uint32_t tar_id;
	}cmd_info_t;

	CHECK_VALID_ID( p->id );

	//频率限制
	if( get_now_tv()->tv_sec - p->water_bomb_time > WAS_ATTACK_CD )
	{
		p->water_bomb_time = get_now_tv()->tv_sec;
	}
	else
	{
		response_proto_uint32_uint32( p, p->waitcmd, 0, 0, 0 );
		return 0;
	}

	CHECK_BODY_LEN( bodylen, sizeof(cmd_info_t) );
	cmd_info_t cmd;
	int j = 0;
	UNPKG_UINT32( buf, cmd.tar_id, j );
	CHECK_VALID_ID( cmd.tar_id );
	sprite_t* tar_sprite = get_sprite(cmd.tar_id);
	if( !tar_sprite )
	{
		ERROR_LOG( "wvs_team_switch_cmd Invalid Player ID:[%u]", cmd.tar_id );
		return send_to_self_error( p, p->waitcmd, ERR_wvs_invalid_player_id, 1 );
	}
	//自己没有加入队伍
	if( p->team_id / WVS_TEAM_ID_BASE != 1 )
	{
		ERROR_LOG( "wvs_team_switch_cmd Player hadn't join a team ID:[%u]", p->id );
		return send_to_self_error( p, p->waitcmd, ERR_wvs_hadnt_join_a_team, 1 );
	}

	uint32_t team_id;
	uint32_t isSwitch = 0;
	//对方没有加入队伍
	if( tar_sprite->team_id / WVS_TEAM_ID_BASE != 1 )
	{
		isSwitch = 1;
		team_id = p->team_id;
	}
	else
	{
		//和自己为同队
		if( tar_sprite->team_id == p->team_id )
		{
			return send_to_self_error( p, p->waitcmd, ERR_wvs_switch_same_team, 1 );
		}
		else
		{
			tar_sprite->attack_cnt++;
			if( tar_sprite->attack_cnt >= WAS_SWITCH_TEAM_ATTACK_CNT )
			{
				isSwitch = 1;
				team_id = p->team_id;
				tar_sprite->attack_cnt = 0;
			}
		}
	}
	if( 1 == isSwitch )
	{
		DEBUG_LOG( "Player [%u] Switch Team. Cur Team:[%u] Tar Team:[%u]", tar_sprite->id, p->team_id, team_id );
		pkg_host_uint32_uint32(p->session, team_id, cmd.tar_id);
		return send_request_to_db(SVR_PROTO_SPORT_SIGN, p, sizeof(team_id), &team_id, tar_sprite->id);
	}
	else
	{
		response_proto_uint32_uint32( p, p->waitcmd, 0, 0, 0 );
		return 0;
	}
}

int wvs_get_team_power_cmd(sprite_t *p, const uint8_t *buf, int bodylen)
{
	CHECK_VALID_ID( p->id );
	return send_request_to_db( SVR_PROTO_WVS_GET_TEAM_POWER, p, 0, NULL, p->id );
}

int wvs_get_team_power_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	uint32_t score_winter;
	uint32_t score_spring;
	CHECK_BODY_LEN( len, sizeof(uint32_t)*2 );
	int j = 0;
	UNPKG_H_UINT32( buf, score_winter, j );
	UNPKG_H_UINT32( buf, score_spring, j );
	switch(p->waitcmd)
	{
	case PROTO_AC_GET_GAME_INFO:
		return ac_get_game_info_callback( p, score_winter, score_spring );
		break;
	default:
		break;
	}
	response_proto_uint32_uint32( p, p->waitcmd, score_winter, score_spring, 0 );
	return 0;
}

