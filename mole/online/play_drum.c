/*
 * =====================================================================================
 *
 *       Filename:  play_drum.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/09/2010 09:01:01 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tommychen
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "play_drum.h"
#include "exclu_things.h"

static play_drum_info_t play_drum_info = {0};

static play_drum_timer_t  play_drum_timer;
#define PLAY_DRUM_GAME_TIME  300
#define PLAY_DRUM_REST_TIME  900


int init_play_drum_timer()
{
    DEBUG_LOG("init_play_drum_timer init time %d ", (int)get_now_tv()->tv_sec);

    INIT_LIST_HEAD(&play_drum_timer.timer_list);
    ADD_TIMER_EVENT(&play_drum_timer, play_drum_end_timeout, NULL, get_now_tv()->tv_sec+PLAY_DRUM_GAME_TIME);
    play_drum_info.begin_time = get_now_tv()->tv_sec;
    notify_play_drum_begin_end(0, play_drum_info.win_team);
    return 0;
}

int user_play_drum_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	if (p->team_id < 1 || p->team_id > 4)
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if((get_now_tv()->tv_sec < (p->play_drum_time + 60)) || (play_drum_info.begin_time == 0))
	{
	    return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409B44E, get_now_tv()->tv_sec, msg_buff, sizeof(msg_buff));

	play_drum_info.team_cnt[p->team_id-1] += 1;
	p->play_drum_time = get_now_tv()->tv_sec;

	int i = sizeof(protocol_t);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}

int notify_play_drum_begin_end(uint32_t flag, uint32_t team_win )
{
    uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, flag, l);
	PKG_UINT32(n_msg, team_win, l);
    init_proto_head(n_msg, PROTO_NOTIFY_PLAY_DRUM_BEGIN_END, l);
    send_to_map3(221,n_msg, l);
    return 0;
}

int play_drum_end_game( )
{
    int i = 0;
    for(i = 0; i < 4; i++)
    {
        if (i == 0)
        {
            play_drum_info.win_team = 1;
        }
        else
        {
            if (play_drum_info.team_cnt[i] > play_drum_info.team_cnt[play_drum_info.win_team - 1])
            {
                play_drum_info.win_team = i + 1;
            }
        }
    }
    play_drum_info.end_time = get_now_tv()->tv_sec;
    play_drum_info.begin_time = 0;
	notify_play_drum_begin_end(1, play_drum_info.win_team);
	ADD_TIMER_EVENT(&play_drum_timer, play_drum_begin, NULL, get_now_tv()->tv_sec+PLAY_DRUM_REST_TIME);

    return 0;
}

int play_drum_end_timeout()
{
    DEBUG_LOG("play_drum_end_timeout end time %d ", (int)get_now_tv()->tv_sec);

    if ((get_now_tv()->tv_sec - play_drum_info.begin_time) >= PLAY_DRUM_GAME_TIME)
    {
        play_drum_end_game( );
    }

    return 0;
}

int play_drum_begin()
{
    DEBUG_LOG("play_drum_begin begin time %d ", (int)get_now_tv()->tv_sec);

    play_drum_info.begin_time = get_now_tv()->tv_sec;
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        play_drum_info.team_cnt[i] = 0;
    }

    ADD_TIMER_EVENT(&play_drum_timer, play_drum_end_timeout, NULL, get_now_tv()->tv_sec+PLAY_DRUM_GAME_TIME);
    notify_play_drum_begin_end(0, play_drum_info.win_team);
    return 0;
}

int get_play_drum_time_left()
{
    return  PLAY_DRUM_REST_TIME -(get_now_tv()->tv_sec - play_drum_info.end_time);
}

int play_drum_get_left_time_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    uint32_t time_val = 0;
    if (play_drum_info.begin_time > 0)
    {
        time_val = 0;
    }
    else
    {
        time_val = get_play_drum_time_left();
    }

    response_proto_uint32_uint32(p, p->waitcmd, time_val, play_drum_info.win_team, 0);
    return 0;
}

int get_play_drum_team_cnt_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);

    uint32_t time_left = 0;
    if (play_drum_info.begin_time > 0)
    {
	    time_left = PLAY_DRUM_GAME_TIME - (get_now_tv()->tv_sec - play_drum_info.begin_time);
	    DEBUG_LOG("time_left %d %d %d", time_left, (int)get_now_tv()->tv_sec, play_drum_info.begin_time);
	}
	else
	{
	    time_left = 0;
	}

	int i = sizeof(protocol_t);
	PKG_UINT32(msg, play_drum_info.team_cnt[0], i);
	PKG_UINT32(msg, play_drum_info.team_cnt[1], i);
	PKG_UINT32(msg, play_drum_info.team_cnt[2], i);
	PKG_UINT32(msg, play_drum_info.team_cnt[3], i);
	PKG_UINT32(msg, time_left, i);
	init_proto_head(msg, p->waitcmd, i);
	return  send_to_self(p, msg, i, 1);
}



