/*
 * =====================================================================================
 *
 *       Filename:  snowball_war.c
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
#include "snowball_war.h"
#include "exclu_things.h"


#define LIFE_VALUE_MAX  2000
#define GIFT_BOX_LIFE_VALUE 100
static snowball_team_t teams[2] = {};
static uint32_t game_max = 0;
static uint32_t game_begin = 0;
static uint32_t time_begin = 0;
static uint32_t timer_flag = 0;
static uint32_t special_gift = 0;
static uint32_t gift_box_life = 0;

static snowball_timer_t  war_timer;
#define SNOWBALL_GAME_TIME  300

int init_snowball_war_timer()
{
    INIT_LIST_HEAD(&war_timer.timer_list);
    return 0;
}

int snowball_enter_quit_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	uint32_t flag = 0;
	uint32_t teamid = 0;
	unpack(body, sizeof(flag), "L", &flag);
	p->snowball_flag = 0;
	//print_team_players_info();
	if (flag)
	{
	    if (check_player_in(0, p->id) || check_player_in(1, p->id))
	    {
            ERROR_LOG("user %d is not in game\t", p->id);
            return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	    }

	    if (game_max)
	    {
	        return send_to_self_error(p, p->waitcmd, -ERR_snowball_war_player_max, 1);
	    }

		if (p->followed == NULL)
		{
			return send_to_self_error(p, p->waitcmd, -ERR_can_not_follow_pet, 1);
		}

	    if (teams[0].count == teams[1].count)
	    {
	        int val = rand()%2;
	        set_player_enter(val, p->id);
	        teamid = val;
	    }
	    else if (teams[0].count < teams[1].count)
	    {
	        set_player_enter(0, p->id);
	        teamid = 0;
	    }
	    else
	    {
	        set_player_enter(1, p->id);
	        teamid = 1;
	    }

	    if (teams[0].count == MAX_TEAM_PLAYERS && teams[1].count == MAX_TEAM_PLAYERS)
	    {
	        game_max = 1;
	    }

	    if (teams[0].count > MAX_TEAM_PLAYERS || teams[1].count > MAX_TEAM_PLAYERS)
	    {
	        ERROR_LOG("teams[0].count %d teams[1].count %d", teams[0].count, teams[1].count);
	        snowball_end_game(0);
	        return send_to_self_error(p, p->waitcmd, -ERR_snowball_war_player_max, 1);
	    }

	    if (!game_begin)
	    {
	        game_begin = 1;
	        notify_snowball_begin_end(game_begin);
	        teams[0].life_value = LIFE_VALUE_MAX;
	        teams[1].life_value = LIFE_VALUE_MAX;
	        time_begin = get_now_tv()->tv_sec;
	        if (!timer_flag)
	        {
	            init_snowball_war_timer();
	            timer_flag = 1;
	        }

	        ADD_TIMER_EVENT(&war_timer, snowball_end_game_timeout, NULL, time_begin + SNOWBALL_GAME_TIME);
	    }

		p->snowball_flag |= 0x01;
		/*if (special_gift == 1) {
			p->snowball_flag |= 0x02;
		}*/
	}
	else
	{

	    uint32_t  teamid = 2;
	    teamid = get_teamid_player(p->id);
	    if (teamid == 0 || teamid == 1)
	    {
            set_player_quit(teamid, p->id);
			p->snowball_flag = 0;
	    }
	    else
	    {
	        ERROR_LOG("teamid %d userid %d flag %d", teamid, p->id, flag);
	        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	    }


	    if (teams[0].count < MAX_TEAM_PLAYERS || teams[1].count < MAX_TEAM_PLAYERS)
	    {
	        game_max = 0;
	    }

	    if (teams[0].count == 0 && teams[1].count == 0)
	    {
	        game_begin = 0;
	        notify_snowball_begin_end(game_begin);
	    }
	}

    uint32_t time_c = get_time_left();
	int j = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, j);
    PKG_UINT32(msg, flag, j);
    PKG_UINT32(msg, teamid, j);
    PKG_UINT32(msg, time_c, j);
    PKG_UINT32(msg, teams[0].life_value, j);
    PKG_UINT32(msg, teams[1].life_value, j);
	PKG_UINT32(msg, special_gift, j);
	PKG_UINT32(msg, gift_box_life, j);
    PKG_UINT32(msg, teams[0].pos[0], j);
    PKG_UINT32(msg, teams[0].pos[1], j);
    PKG_UINT32(msg, teams[0].pos[2], j);
    PKG_UINT32(msg, teams[1].pos[0], j);
    PKG_UINT32(msg, teams[1].pos[1], j);
    PKG_UINT32(msg, teams[1].pos[2], j);
	
	PKG_UINT32(msg, teams[0].count, j);
    int i = 0;
    for (i = 0; i < MAX_TEAM_PLAYERS; i++)
    {
        if (teams[0].players[i] != 0)
        {
            PKG_UINT32(msg, teams[0].players[i], j);
			PKG_UINT32(msg, i, j);
        }
    }

    PKG_UINT32(msg, teams[1].count, j);
    for (i = 0; i < MAX_TEAM_PLAYERS; i++)
    {
        if (teams[1].players[i] != 0)
        {
            PKG_UINT32(msg, teams[1].players[i], j);
			PKG_UINT32(msg, i, j);
        }
    }

	//print_team_players_info();

	init_proto_head(msg, p->waitcmd, j);
	send_to_map(p, msg, j, 1);

    return 0;

}

int snowball_end_game_timeout()
{
    DEBUG_LOG("timeout end game !");

    if ((get_now_tv()->tv_sec - time_begin + 3) >= SNOWBALL_GAME_TIME)
    {
        snowball_end_game(1);
    }

    return 0;
}

int snowball_set_team_win(uint32_t teamid)
{
	if (teamid >= 2) {
		return 0;
	}

	int i = 0;
	for (i = 0; i < MAX_TEAM_PLAYERS; i++) {
		if(teams[teamid].players[i] != 0) {
			sprite_t* sp = get_sprite(teams[teamid].players[i]);
			if (sp != NULL) {
				sp->snowball_flag |= 0x04;
			}
		}
	}
	return 0;
}

int snowball_set_all_gift_flag()
{
	int i = 0;
	for (i = 0; i < MAX_TEAM_PLAYERS; i++) {
		if(teams[0].players[i] != 0) {
			sprite_t* sp = get_sprite(teams[0].players[i]);
			if (sp != NULL) {
				sp->snowball_flag |= 0x02;
			}
		}
		if(teams[1].players[i] != 0) {
			sprite_t* sp = get_sprite(teams[1].players[i]);
			if (sp != NULL) {
				sp->snowball_flag |= 0x02;
			}
		}
	}
	return 0;
}

int snowball_end_game(uint32_t flag)
{
    if (game_begin)
    {
        DEBUG_LOG("snowball end game !");		
		game_begin = 0;
	    notify_snowball_begin_end(game_begin);
	    memset(teams, 0, sizeof (teams));
	    game_max = 0;
        time_begin = 0;
    }

    return 0;

}

int set_player_enter(int teamid, uint32_t userid)
{
    uint32_t i = 0;
    for (i = 0; i < MAX_TEAM_PLAYERS; i++)
    {
        if(teams[teamid].players[i] == 0)
        {
            teams[teamid].players[i] = userid;
            teams[teamid].count = teams[teamid].count + 1;
            DEBUG_LOG("set_player_enter teamid %d userid %d count %d", teamid, userid, teams[teamid].count);
            break ;
        }
    }

    //print_team_players_info();

    return 0;
}

int set_player_quit(int teamid, uint32_t userid)
{
    uint32_t i = 0;
    for (i = 0; i < MAX_TEAM_PLAYERS; i++)
    {
        if(teams[teamid].players[i] == userid)
        {
            teams[teamid].players[i] = 0;
            teams[teamid].count = teams[teamid].count - 1;
            DEBUG_LOG("set_player_quit teamid %d userid %d, count %d", teamid, userid, teams[teamid].count);
            break;
        }
    }

    //print_team_players_info();

    return 0;
}

int set_player_leave_pos(int teamid, uint32_t userid)
{
    uint32_t i = 0;
    for (i = 0; i < 3; i++)
    {
        if(teams[teamid].pos[i] == userid)
        {
            teams[teamid].pos[i] = 0;
            DEBUG_LOG("set_player_leave_pos teamid %d userid %d", teamid, userid);
            break;
        }
    }

    return 0;
}


int snowball_game_player_quit(sprite_t* p)
{
    uint32_t teamid = 2;
    if (check_player_in(0, p->id))
    {
        teamid = 0;
    }
    else if (check_player_in(1, p->id))
    {
        teamid = 1;
    }
    if (teamid != 2)
    {
        set_player_quit(teamid, p->id);
        set_player_leave_pos(teamid, p->id);
    }
	p->snowball_flag = 0;

    if (teamid != 2)
    {
        DEBUG_LOG("snowball_game_player_quit teamid %d userid %d", teamid, p->id);

        uint32_t flag = 0;
        uint32_t time_c = get_time_left();
	    int j = sizeof(protocol_t);
	    uint8_t n_msg[1024] = {0};
	    PKG_UINT32(n_msg, p->id, j);
        PKG_UINT32(n_msg, flag, j);
        PKG_UINT32(n_msg, teamid, j);
        PKG_UINT32(n_msg, time_c, j);
        PKG_UINT32(n_msg, teams[0].life_value, j);
        PKG_UINT32(n_msg, teams[1].life_value, j);
        PKG_UINT32(n_msg, special_gift, j);
		PKG_UINT32(n_msg, gift_box_life, j);
		PKG_UINT32(n_msg, teams[0].pos[0], j);
        PKG_UINT32(n_msg, teams[0].pos[1], j);
        PKG_UINT32(n_msg, teams[0].pos[2], j);
        PKG_UINT32(n_msg, teams[1].pos[0], j);
        PKG_UINT32(n_msg, teams[1].pos[1], j);
        PKG_UINT32(n_msg, teams[1].pos[2], j);
        PKG_UINT32(n_msg, teams[0].count, j);
        int i = 0;
        for (i = 0; i < MAX_TEAM_PLAYERS; i++)
        {
            if (teams[0].players[i] != 0)
            {
                PKG_UINT32(n_msg, teams[0].players[i], j);
                PKG_UINT32(n_msg, i, j);
            }
        }
        PKG_UINT32(n_msg, teams[1].count, j);
        for (i = 0; i < MAX_TEAM_PLAYERS; i++)
        {
            if (teams[1].players[i] != 0)
            {
                PKG_UINT32(n_msg, teams[1].players[i], j);
                PKG_UINT32(n_msg, i, j);
            }
        }

    	init_proto_head(n_msg, PROTO_SNOWBALL_ENTER_QUIT, j);
    	send_to_map3(47, n_msg,  j);

    }

    if (teams[0].count == 0 && teams[1].count == 0)
	{
        snowball_end_game(0);
	}

    return 0;
}


int get_teamid_player(uint32_t userid)
{
    uint32_t i = 0;
    for (i = 0; i < 2; i++)
    {
        uint32_t j = 0;
        for (j = 0; j < MAX_TEAM_PLAYERS; j++)
        {
            if (teams[i].players[j] == userid)
            {
                return i;
            }
        }
    }

    return 2;
}

int print_team_players_info()
{
    int i = 0;
    for (i = 0; i < 2; i++)
    {
        int j =0;
		DEBUG_LOG("team info teamid[%u]", i);
        for (j = 0; j < MAX_TEAM_PLAYERS; j++)
        {
            DEBUG_LOG("teamid %d userid %d teamcount %d", i, teams[i].players[j], teams[i].count);
        }
    }

    return 0;
}

int check_player_in(int teamid, uint32_t userid)
{
    uint32_t i = 0;
    for (i = 0; i < MAX_TEAM_PLAYERS; i++)
    {
        if(teams[teamid].players[i] == userid)
        {
            return 1;
        }
    }
    return 0;
}

int check_player_pos_in(int teamid, uint32_t userid)
{
    uint32_t i = 0;
    for (i = 0; i < 3; i++)
    {
        if(teams[teamid].pos[i] == userid)
        {
            return 1;
        }
    }
    return 0;
}


int notify_snowball_begin_end(uint32_t flag)
{
    uint32_t score_red = 0;
    uint32_t score_blue = 0;
    uint32_t team_win = 0;
	special_gift = 0;
    if (!flag)	//结束游戏
    {
        if (teams[0].life_value == 0)
        {
            team_win = 1;
            score_blue  = get_time_left()*5 + teams[1].life_value + (2000 - teams[0].life_value)*2;
            score_red  = (2000 - teams[1].life_value)*2;
            if (score_red >= get_time_left()*5)
            {
                score_red = score_red - get_time_left()*5;
            }
            else
            {
                score_red = 0;
            }
        }
        else if (teams[1].life_value == 0)
        {
            team_win = 0;
            score_red  = get_time_left()*5 + teams[0].life_value + (2000 - teams[1].life_value)*2;
            score_blue  = (2000 - teams[0].life_value)*2;
            if (score_blue >= get_time_left()*5)
            {
                score_blue = score_blue - get_time_left()*5;
            }
            else
            {
                score_blue = 0;
            }
        }
        else
        {
            team_win = 2;
            score_red  = (2000 - teams[1].life_value)*2;
            score_blue  = (2000 - teams[0].life_value)*2;
        }
    } else if (flag == 1) {		//开始游戏
		if ((get_now_tm()->tm_wday == 5 && get_now_tm()->tm_hour == 19)
			|| ((get_now_tm()->tm_wday == 6 || get_now_tm()->tm_wday == 0) && get_now_tm()->tm_hour == 14)) { //周五7:00—8:00,周六、周日14:00-15:00
			special_gift = 1;
			gift_box_life = GIFT_BOX_LIFE_VALUE;
		}
		//special_gift = 1;
		//gift_box_life = GIFT_BOX_LIFE_VALUE;
	}

    uint8_t n_msg[128] = {0};
	int l = sizeof(protocol_t);
	PKG_UINT32(n_msg, flag, l);
	PKG_UINT32(n_msg, team_win, l);
	PKG_UINT32(n_msg, score_red, l);
	PKG_UINT32(n_msg, score_blue, l);
    init_proto_head(n_msg, PROTO_NOTIFY_SNOWBALL_BEGIN_END, l);
    send_to_map3(47, n_msg, l);

    return 0;
}

int snowball_game_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
	uint32_t score = 0;
	unpack(body, sizeof(score), "L", &score);
	if (p->snowball_flag == 0) {
		ERROR_LOG("user can not get bonus, donot paly game: uid[%u]", p->id);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	if (score > 5500)
	{
	    score = 5500;
	}

    *(uint32_t*)p->session  = score;
	if (score >= 2000)
	{
	    db_set_sth_done(p, 40001, 10, p->id);
	}
	else
	{
	    send_snowball_game_bonus_to_db(p, score);
	}

	return 0;
}

int snowball_game_bonus_day_limit_callback(sprite_t *p)
{
	uint32_t score = *(uint32_t *)p->session;
	return send_snowball_game_bonus_to_db(p, score);
}


int send_snowball_game_bonus_to_db(sprite_t* p, uint32_t score)
{
    uint32_t itemid = 190809;
	float rate = (float)(score)/5500;
	uint32_t xiaomee = rate * 1000;
	uint32_t strong = rate * 80;
	uint32_t mole_exp = rate * 150;
	uint32_t count = 0;
	if (score >= 2000 && score <= 3000)
	{
	    count = 1;
	}
	else if (score >= 3001 && score <= 5000)
	{
	    count = 2;
	}
	else if (score >= 5000)
	{
	    count = 3;
	}
	*(uint32_t*)(p->session + 4)  = xiaomee;
	*(uint32_t*)(p->session + 8)  = mole_exp;
	*(uint32_t*)(p->session + 12)  = strong;
	*(uint32_t*)(p->session + 16)  = count;

	uint8_t buff[1024] = {};
	int j = 0;
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 4, j);
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 0, j);
	item_t* itm = get_item_prop(0);
    if (pkg_item_kind(p, buff, itm->id, &j) == -1)
	{
	    return -1;
	}
	PKG_H_UINT32(buff, itm->id, j);
	PKG_H_UINT32(buff, xiaomee, j);
	PKG_H_UINT32(buff, itm->max, j);

    itm = get_item_prop(1);
    if (pkg_item_kind(p, buff, itm->id, &j) == -1)
	{
	    return -1;
	}
	PKG_H_UINT32(buff, itm->id, j);
	PKG_H_UINT32(buff, mole_exp, j);
	PKG_H_UINT32(buff, itm->max, j);

	itm = get_item_prop(2);
    if (pkg_item_kind(p, buff, itm->id, &j) == -1)
	{
	    return -1;
	}
	PKG_H_UINT32(buff, itm->id, j);
	PKG_H_UINT32(buff, strong, j);
	PKG_H_UINT32(buff, itm->max, j);

	itm = get_item_prop(itemid);
    if (pkg_item_kind(p, buff, itm->id, &j) == -1)
	{
	    return -1;
	}
	PKG_H_UINT32(buff, itm->id, j);
	PKG_H_UINT32(buff, count, j);
	PKG_H_UINT32(buff, itm->max, j);

	send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, j, buff, p->id);
	return 0;

}

int send_snowball_game_bonus_to_client(sprite_t* p)
{
    uint32_t itemid = 190809;
    uint32_t score = *(uint32_t *)p->session;
	uint32_t xiaomee = *(uint32_t*)(p->session + 4);
	uint32_t mole_exp = *(uint32_t*)(p->session + 8);
	uint32_t strong = *(uint32_t*)(p->session + 12);
	uint32_t count = *(uint32_t*)(p->session + 16);

	uint32_t info_m[7] = {xiaomee,mole_exp,strong,0,0,1,p->id};
	msglog(statistic_logfile, 0x02040057, now.tv_sec, info_m, sizeof(info_m));

	msglog(statistic_logfile, 0x02042200, now.tv_sec, &count, 4);

	DEBUG_LOG("bonus score %d xiaomee %d mole_exp %d strong %d count %d",
	    score, xiaomee, mole_exp, strong, count);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, score, j);
	PKG_UINT32(msg, 4, j);
    PKG_UINT32(msg, 0, j);
    PKG_UINT32(msg, xiaomee, j);
    PKG_UINT32(msg, 1, j);
    PKG_UINT32(msg, mole_exp, j);
    PKG_UINT32(msg, 2, j);
    PKG_UINT32(msg, strong, j);
    PKG_UINT32(msg, itemid, j);
    PKG_UINT32(msg, count, j);
	if (p->snowball_flag & 0x02) {
		const uint32_t itemid = 19202;
		PKG_UINT32(msg, itemid, j);
		PKG_UINT32(msg, 1, j);
		uint32_t db_buf[] = {0, 1, 0, 0, 1, itemid, 1, 99999};
		send_request_to_db(SVR_PROTO_EXCHG_ITEM, p, sizeof(db_buf), db_buf, p->id);
	} else {
		PKG_UINT32(msg, 0, j);
		PKG_UINT32(msg, 0, j);
	}
	init_proto_head(msg, p->waitcmd, j);
	send_to_self(p, msg, j, 1);
	return 0;

}


int snowball_attack_enemy_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 12);
	uint32_t object = 0;
	uint32_t userid = 0;
	uint32_t attack_value = 0;
	unpack(body, sizeof(object)+sizeof(userid)+sizeof(attack_value),"LLL",
	    &object,&userid,&attack_value);

	DEBUG_LOG("snowball_attack_enemy_cmd enter p->id %u object %d userid %u attcak_value %d",
	    p->id, object, userid, attack_value);

    uint32_t attack_flag = 0;

	if ( (object == 0 || object > 3) || (attack_value != 5 && attack_value != 100) )
	{
	    ERROR_LOG("%d error value object %d, attack_value %d\t", p->id, object, attack_value);
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	int teamid = 2;
	if (check_player_in(0, p->id))
	{
	    teamid = 0;
	}

	if (check_player_in(1, p->id))
	{
	    teamid = 1;
	}

    if (teamid != 0 && teamid != 1)
	{
        ERROR_LOG("user %d is not in game\t", p->id);
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if (object == 1) {	//打人
	    if (!check_player_in(!teamid, userid))
	    {
	        ERROR_LOG("user %d is not in enemy\t", userid);
            return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	    }
		int val = rand()%10;
	    if (attack_value == 50)
	    {
	        if (val < 8)
	        {
	            attack_flag = 1;
	        }
	    }
	    else
	    {
	        if (val < 5)
	        {
	            attack_flag = 1;
	        }
	    }
	} else if (object == 2) {	//打城堡
	    if (teams[!teamid].life_value >= attack_value)
	    {
	        teams[!teamid].life_value = teams[!teamid].life_value - attack_value;
	    }
	    else
	    {
	        teams[!teamid].life_value = 0;
	    }

	    if (teams[!teamid].life_value == 0)
	    {
			snowball_set_team_win(teamid);
			snowball_end_game(0);
	    }
	} else {	//打礼物盒
		if (attack_value == 100) {
			attack_value = 10;
		}
		if (gift_box_life > attack_value) {
			gift_box_life -= attack_value;
		} else if (gift_box_life <= attack_value) {
			attack_value = gift_box_life;
			notify_attack_gift_box_over(p);
		}
	}

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, j);
    PKG_UINT32(msg, object, j);
    PKG_UINT32(msg, userid, j);
    PKG_UINT32(msg, attack_flag, j);
	PKG_UINT32(msg, special_gift, j);
	PKG_UINT32(msg, gift_box_life, j);
    PKG_UINT32(msg, teams[0].life_value, j);
    PKG_UINT32(msg, teams[1].life_value, j);
	init_proto_head(msg, p->waitcmd, j);
	send_to_map(p, msg, j, 1);
	DEBUG_LOG("snowball_attack_enemy_cmd leave: uid[%u] life1[%u] life2[%u] len[%u]", p->id, teams[0].life_value, teams[1].life_value, j);
	return 0;
}

int snowball_query_teams_info_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    uint32_t time_c = 0;
    if (game_begin)
    {
        time_c = get_time_left();
    }

    int j = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, j);
	PKG_UINT32(msg, game_begin, j);
    PKG_UINT32(msg, time_c, j);
	PKG_UINT32(msg, special_gift, j);
	PKG_UINT32(msg, gift_box_life, j);
    PKG_UINT32(msg, teams[0].life_value, j);
    PKG_UINT32(msg, teams[1].life_value, j);
    PKG_UINT32(msg, teams[0].pos[0], j);
    PKG_UINT32(msg, teams[0].pos[1], j);
    PKG_UINT32(msg, teams[0].pos[2], j);
    PKG_UINT32(msg, teams[1].pos[0], j);
    PKG_UINT32(msg, teams[1].pos[1], j);
    PKG_UINT32(msg, teams[1].pos[2], j);
    PKG_UINT32(msg, teams[0].count, j);

    DEBUG_LOG("red team count %d", teams[0].count);

    int i = 0;
    for (i = 0; i < MAX_TEAM_PLAYERS; i++)
    {
        if (teams[0].players[i] != 0)
        {
            PKG_UINT32(msg, teams[0].players[i], j);
			PKG_UINT32(msg, i, j);
            DEBUG_LOG("red team user %d", teams[0].players[i]);
        }
    }

    PKG_UINT32(msg, teams[1].count, j);
    DEBUG_LOG("blue team count %d", teams[1].count);
    for (i = 0; i < MAX_TEAM_PLAYERS; i++)
    {
        if (teams[1].players[i] != 0)
        {
            PKG_UINT32(msg, teams[1].players[i], j);
			PKG_UINT32(msg, i, j);
            DEBUG_LOG("blue team user %d", teams[1].players[i]);
        }
    }

	init_proto_head(msg, p->waitcmd, j);
	send_to_self(p, msg, j, 1);
	return 0;

}

int get_time_left()
{
    return  SNOWBALL_GAME_TIME -(get_now_tv()->tv_sec - time_begin);
}

int snowball_get_left_time_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    uint32_t time_val = get_time_left();
    response_proto_uint32(p, p->waitcmd, time_val, 0);
    return 0;
}


int snowball_ocupy_pos_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	uint32_t pos = 0;
	uint32_t teamid = 0;
	uint32_t flag = 0;
	unpack(body, sizeof(pos) + sizeof(flag), "LL", &pos, &flag);
	if(pos > 2)
	{
	    ERROR_LOG("error pos %d \t", pos);
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

    if (check_player_in(0, p->id))
    {
        teamid = 0;
    }
    else if (check_player_in(1, p->id))
    {
        teamid = 1;
    }
    else
    {
        ERROR_LOG("user %d is not in game\t", p->id);
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
    }

    if (flag)
    {
        if (check_player_pos_in(teamid, p->id))
        {
            ERROR_LOG("user %d is aready in pos\t", p->id);
            return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
        }
        if (teams[teamid].pos[pos] != 0)
        {
            return send_to_self_error(p, p->waitcmd, -ERR_snowball_war_pos_aready_ocupy, 1);
        }
        else
        {
            teams[teamid].pos[pos] = p->id;
        }
    }
    else
    {
        if (teams[teamid].pos[pos] != p->id)
        {
            return send_to_self_error(p, p->waitcmd, -ERR_snowball_war_pos_aready_ocupy, 1);
        }
        else
        {
            teams[teamid].pos[pos] = 0;
        }
    }

    int j = sizeof(protocol_t);
    PKG_UINT32(msg, p->id, j);
    PKG_UINT32(msg, flag, j);
    PKG_UINT32(msg, teamid, j);
    PKG_UINT32(msg, pos, j);
    PKG_UINT32(msg, teams[0].pos[0], j);
    PKG_UINT32(msg, teams[0].pos[1], j);
    PKG_UINT32(msg, teams[0].pos[2], j);
    PKG_UINT32(msg, teams[1].pos[0], j);
    PKG_UINT32(msg, teams[1].pos[1], j);
    PKG_UINT32(msg, teams[1].pos[2], j);
	init_proto_head(msg, p->waitcmd, j);
	send_to_map(p, msg, j, 1);
	return 0;

}

int notify_attack_gift_box_over(sprite_t* p)
{
	gift_box_life = 0;
	
	int l = sizeof(protocol_t);
	init_proto_head(msg, PROTO_SNOWBALL_CHECK_CHRISTMAS_GIFT, l);
	send_to_map(p, msg, l, 0);
	DEBUG_LOG("GIFT BOX LIFE OVER: uid[%u]", p->id);
	snowball_set_all_gift_flag();
	return 0;
}

int snowball_lahm_attack_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	//DEBUG_LOG("snowball_lahm_attack_cmd start");
	int teamid = 2;
	if (check_player_in(0, p->id)) {
	    teamid = 0;
	}
	if (check_player_in(1, p->id)) {
	    teamid = 1;
	}
    if (teamid != 0 && teamid != 1) {
        ERROR_LOG("user %d is not in game\t", p->id);
        return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	uint32_t rand_val = rand() % 100;
	uint32_t attack_flag = ATTACK_ONE;
	uint32_t attack_opt = 0;
	uint32_t attack_value = 0;
	if (ISVIP(p->flag)) {
		if (rand_val < 50) {
			attack_flag = ATTACK_ONE;
			attack_value = 1;
		} else if (rand_val < 80) {
			attack_flag = ATTACK_USER;
		} else if (rand_val < 90) {
			attack_flag = ATTACK_FIVE;
			attack_value = 5;
		} else {
			attack_flag = ATTACK_BIG_BALL;
			attack_value = 10;
		}
	} else {
		if (rand_val < 60) {
			attack_flag = ATTACK_ONE;
			attack_value = 1;
		} else if (rand_val < 90) {
			attack_flag = ATTACK_USER;
		} else {
			attack_flag = ATTACK_THREE;
			attack_value = 3;
		}
	}

	if (special_gift == 1 && gift_box_life > 0) {	//有礼物盒
		attack_flag = ATTACK_GIFT_BOX;
		attack_opt = 1;
		if (gift_box_life > attack_value) {
			gift_box_life -= attack_value;
		} else if (gift_box_life <= attack_value) {
			attack_value = gift_box_life;
			notify_attack_gift_box_over(p);
		}
	}

	if (attack_flag == ATTACK_USER) {	//如果是攻击对方
		if (teams[!teamid].count == 0) {	//对方如果没人
			attack_flag = ATTACK_ONE;
			attack_value = 1;
			attack_opt = 0;
		} else {
			int rand_idx = rand() % teams[!teamid].count;
			int i = 0;
			int idx = 0;
			for (i = 0; i < MAX_TEAM_PLAYERS; i++) {
				if (teams[!teamid].players[i] != 0) {
					if (idx == rand_idx) {
						attack_opt = teams[!teamid].players[i];
						break;
					}
					idx++;
				}
			}

			if (attack_opt == 0) {
				attack_flag = ATTACK_ONE;
				attack_value = 1;
				attack_opt = 0;
			} else {
				if (rand() % 100 <= 20) {
					attack_value = 1;
				}
			}
		}
	}
	
	if (attack_flag == ATTACK_ONE || attack_flag == ATTACK_THREE || attack_flag == ATTACK_FIVE || attack_flag == ATTACK_BIG_BALL) {//攻击城堡
		if (teams[!teamid].life_value >= attack_value) {
	        teams[!teamid].life_value = teams[!teamid].life_value - attack_value;
	    } else {
	        teams[!teamid].life_value = 0;
	    }
		if (teams[!teamid].life_value == 0) {
			snowball_set_team_win(teamid);
			snowball_end_game(0);
	    }
	}

	DEBUG_LOG("snowball lahm attack: uid[%u] attack_flag[%u] attack_opt[%u] attack_value[%u]",  p->id, attack_flag, attack_opt, attack_value);
	DEBUG_LOG("snowball giftbox&team info: uid[%u] gift[%u] gift_life[%u] team1[%u] team2[%u]",  p->id, special_gift, gift_box_life, teams[0].life_value, teams[1].life_value);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, p->id, j);
    PKG_UINT32(msg, attack_flag, j);
    PKG_UINT32(msg, attack_opt, j);
    PKG_UINT32(msg, attack_value, j);
	PKG_UINT32(msg, special_gift, j);
	PKG_UINT32(msg, gift_box_life, j);
    PKG_UINT32(msg, teams[0].life_value, j);
    PKG_UINT32(msg, teams[1].life_value, j);
	init_proto_head(msg, p->waitcmd, j);
	send_to_map(p, msg, j, 1);
//	DEBUG_LOG("snowball_lahm_attack_cmd leave");
	return 0;
}

int snowball_game_new_bonus_cmd(sprite_t* p, uint8_t* body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 0);
	if (p->snowball_flag == 0) {
		ERROR_LOG("user can not get bonus, donot paly game: uid[%u]", p->id);
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if (p->snowball_flag & 0x02) {	//如果是特殊时间，没有限制
		return snowball_game_new_bonus_day_limit_callback(p);
	}
	return db_set_sth_done(p, 40001, 24, p->id);
}

int snowball_game_new_bonus_day_limit_callback(sprite_t *p)
{
	uint32_t db_buf[256] = {0};
	db_buf[1] = 1;
	uint32_t buf_len = 4;
	int l = sizeof(protocol_t);
	uint32_t is_win = (uint32_t)(p->snowball_flag & 0x04);

	const uint32_t snowball_id = 1351181;
	uint32_t snowball_cnt = (is_win == 0) ? 5 : 10;
	PKG_UINT32(msg, snowball_id, l);
	PKG_UINT32(msg, snowball_cnt, l);
	db_buf[buf_len++] = 99;
	db_buf[buf_len++] = snowball_id;
	db_buf[buf_len++] = snowball_cnt;
	db_buf[buf_len++] = 99999;	
	
	uint32_t gift_id = 0;//{180047, 180049, 180057, 180059, 1353306};
	if (p->snowball_flag & 0x02) {
		int rand_val = rand() % 100;
		if (rand_val <= 17) {
			gift_id = 180047;
		} else if (rand_val <= 35) {
			gift_id = 180049;
		} else if (rand_val <= 52) {
			gift_id = 180057;
		} else if (rand_val <= 70) {
			gift_id = 180059;
		} else {
			gift_id = 1353306;
		}
		
		const item_t* itm = get_item_prop(gift_id);
		uint32_t flag;
        int temp_len = 0;
        pkg_item_kind(p, (uint8_t*)(&flag), itm->id, &temp_len);
		db_buf[1] = 2;
		db_buf[buf_len++] = flag;
		db_buf[buf_len++] = gift_id;
		db_buf[buf_len++] = 1;
		db_buf[buf_len++] = itm->max;

		PKG_UINT32(msg, gift_id, l);
		PKG_UINT32(msg, 1, l);
	} else {
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 0, l);
	}

	send_request_to_db(SVR_PROTO_EXCHG_ITEM, NULL, buf_len * sizeof(uint32_t), db_buf, p->id);
	
	uint32_t msg_buff[2] = {p->id, 1};
	msglog(statistic_logfile, 0x0409BF41, now.tv_sec, msg_buff, sizeof(msg_buff));
	if (p->snowball_flag & 0x04) {
		msglog(statistic_logfile, 0x0409C2CD, now.tv_sec, msg_buff, sizeof(msg_buff));
	}
	
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}
