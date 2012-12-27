
#include "proto.h"
#include "dbproxy.h"
#include "login.h"
#include "exclu_things.h"
#include "mole_sports.h"
#include "message.h"
static int npc_path[] = {
    //0, none;1, fire; 2,chance; 3, luck; 4, color
    0,1,1,4,1,1,2,1,1,1,1,
    1,1,4,1,1,1,3,1,1,1,1,
    4,1,1,1,4,1,1,3,1,1,1,
    4,1,1,1,1,1,1,2,1,1,1,
    1,3,1,1,4,1,1,1,1,1,2,
    1,1,1,1,3,1,1,1,1,2,1,
    1,1,1,4,1,1,1,1,1,3,1,
    1,1,1,4,1,1,1,2,1,1,0,
};

static int team_fire[5];
int sports_sign_up_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
   CHECK_BODY_LEN(bodylen, 4); 
   if(p->tmpinfo.team) {
       return send_to_self_error(p, p->waitcmd, -ERR_already_sport_sign, 1);
   }
   int i = 0;
   uint32_t type;
   UNPKG_UINT32(body, type, i);
   if(type > 5 || type == 0) {
       ERROR_RETURN(("type error [%d %d]", p->id, type), -1);
   }
   *(uint32_t*)p->session = type;
   DEBUG_LOG("SPORTS SIGN UP [%d %d]", p->id, type);
   return send_request_to_db(SVR_PROTO_SPORT_SIGN, p, 4, &type, p->id);

}

int sports_sign_up_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    int type = *(uint32_t*)p->session;
    db_set_cnt(0, TEAM_MEMBER_NUM + type - 1, 1);
    static int flag[5] = {12755, 12756, 12757, 12758, 12759};
	db_single_item_op(0, p->id, flag[type -1], 1, 1);
    p->tmpinfo.team = type;
    switch(p->waitcmd) {
      //  case PROTO_SPORT_SIGN_UP:
      //      response_proto_head(p, p->waitcmd, 0);
      //      break;
        case PROTO_SPORT_RAND_SIGN:
            return sports_get_npc_info(p);
            break;
        default:
            break;
    }
    return 0;
}

int get_sports_team_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    typedef struct _sport_info {
        uint32_t team;
        uint32_t total_medal[3];
        uint32_t day_medal[3];
    }sport_info_t;
    CHECK_BODY_LEN(len, sizeof(sport_info_t));
    sport_info_t* my_sport_info = (sport_info_t*)buf; 
    DEBUG_LOG("SPORTS SELF INFO [%d %d %d %d %d %d %d %d]", p->id, p->waitcmd, my_sport_info->total_medal[0],my_sport_info->total_medal[1],my_sport_info->total_medal[2],my_sport_info->day_medal[0],my_sport_info->day_medal[1],my_sport_info->day_medal[2]);
    switch(p->waitcmd) {
        case PROTO_LOGIN:
            p->tmpinfo.team = my_sport_info->team;
            if(p->tmpinfo.team >5) {
                ERROR_LOG("error team [%d %d]", p->id, p->tmpinfo.team);
                p->tmpinfo.team = 5;
            }
        	// DEBUG_LOG("MY TEAM [%d %d]", p->id, p->tmpinfo.team);
            return sports_get_npc_info(p);
			//return proc_final_login_step(p);
        case PROTO_SPORT_BREAK:
            {
                uint32_t type = *(uint32_t*)p->session;
                if(my_sport_info->total_medal[type]) {
                    static int break_reward[3] = {12832, 12833, 12834};
                    *(uint32_t*)(p->session + 4) = break_reward[type];
	                db_single_item_op(p, p->id, break_reward[type], 1, 1);
                } else {
                    return send_to_self_error(p, p->waitcmd, -ERR_sports_no_medal, 1);
                }
            }
            break;
        case PROTO_SPORT_REACH_MAX:
            {
                uint32_t type = *(uint32_t*)p->session;
                static int max_medal[3] = {10, 15, 20};
                static int max_medal_type[3] = {133, 137, 138};
         //       DEBUG_LOG("REACH MAX INFO [%d %d]", p->id, my_sport_info->day_medal[type], max_medal[type] );
                if(my_sport_info->day_medal[type] >= max_medal[type]) {
                    return db_set_sth_done(p, max_medal_type[type], 1);
                } else {
                    return send_to_self_error(p, p->waitcmd, -ERR_sports_reach_max, 1);
                }
            }
            break;
        case PROTO_SPORT_SELF_INFO:
            {
                int l = sizeof(protocol_t);
                PKG_UINT32(msg, my_sport_info->total_medal[0], l);
                PKG_UINT32(msg, my_sport_info->total_medal[1], l);
                PKG_UINT32(msg, my_sport_info->total_medal[2], l);
                PKG_UINT32(msg, my_sport_info->day_medal[0], l);
                PKG_UINT32(msg, my_sport_info->day_medal[1], l);
                PKG_UINT32(msg, my_sport_info->day_medal[2], l);
                init_proto_head(msg, p->waitcmd, l);
                return send_to_self(p, msg, l, 1);
            }
            break;
        default:
            ERROR_RETURN(("err waitcmd [%d %d]", p->id, p->waitcmd), -1);
    }
    return 0;
}

int sports_fin_pre_task_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    static int bg[5] = {12803, 12804, 12805, 12806, 12807};
    if(!p->tmpinfo.team){
        return send_to_self_error(p, p->waitcmd, -ERR_sports_not_sign, 1);
    }
    DEBUG_LOG("SPRORT FIN PRE TASK [%d %d]", p->id, bg[p->tmpinfo.team - 1]);
	return db_single_item_op(p, p->id, bg[p->tmpinfo.team - 1], 1, 1);
}

int sports_encourage_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    if(!p->tmpinfo.team) {
        return send_to_self_error(p, p->waitcmd, -ERR_sports_not_sign, 1);
    }
    int i = 0;
    uint32_t team;
    UNPKG_UINT32(body, team, i);
    if(team > 5 || team == 0) {
        ERROR_RETURN(("encourage error [%d %d]", p->id, team), -1);
    }
    *(uint32_t*)p->session = team;
    DEBUG_LOG("ENCOURAGE [%d %d]", p->id, team);
    return db_set_sth_done(p, 132, 1);
}

int do_sports_encourage(sprite_t *p)
{
    return add_medal(p, 1, 1, 0);
}

int sports_rand_sign_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
   CHECK_BODY_LEN(bodylen, 0); 
   if(p->tmpinfo.team) {
       return send_to_self_error(p, p->waitcmd, -ERR_already_sport_sign, 1);
   }
   return send_request_to_db(SVR_PROTO_SPORT_RAND_SIGN, p, 0, NULL, p->id);

}

int sports_rand_sign_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 4);
    uint32_t team = *(uint32_t*)buf;
    if(!team || team > 5) {
        ERROR_RETURN(("db team error [%d %d]", p->id, team), -1);
    }
   *(uint32_t*)p->session = team;
   DEBUG_LOG("SPORTS RAND SIGN UP [%d %d]", p->id, team);
   return send_request_to_db(SVR_PROTO_SPORT_SIGN, p, 4, &team, p->id);
}

int add_medal(sprite_t *p, int count, int type, int limited)
{
    if(p->waitcmd != PROTO_GAME_SCORE) {
        *(uint32_t*)(p->session + 4) = type;
        *(uint32_t*)(p->session + 8) = count;
    }

    uint32_t dbmsg[3];
    dbmsg[0] = type;
    dbmsg[1] = limited;
    dbmsg[2] = count;
    return send_request_to_db(SVR_PROTO_SPORT_ADD_MEDAL, p, 12, dbmsg, p->id);
}

int sports_add_medal_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    uint32_t type, count;
    switch(p->waitcmd) {
        case PROTO_GAME_SCORE:
            {
                int i = sizeof(protocol_t);
                uint32_t* pInt = (void*)(p->session + i + 32);
                if(!pInt[5]) {
	                return item_trading_callback(p, p->id, 0, 0);
                }
                i = i + 32 + 5 * 4;
                UNPKG_UINT32(p->session, type, i);
                type = type - 100;
                count = 1;
	            item_trading_callback(p, p->id, 0, 12);
            }
            break;
        case PROTO_SPORT_ENCOURAGE:
            type = 1;
            count = 1;
            response_proto_uint32(p, p->waitcmd, 101, 0);
            break;
        case PROTO_SPORT_CHANCE_CYCLE:
            {
                int itmid = *(uint32_t*)p->session;
                type = *(uint32_t*)(p->session + 4);
                count = *(uint32_t*)(p->session + 8);
                response_proto_uint32(p, p->waitcmd, itmid, 0);
            }
            break;
        case PROTO_SPORT_SET_TASK:
            {
                type = *(uint32_t*)(p->session + 4);
                count = *(uint32_t*)(p->session + 8);
                response_proto_head(p, p->waitcmd, 0);
            }
            break;
        default:
            ERROR_RETURN(("err add medal [%d %d]", p->id, p->waitcmd), -1);
    }

    DEBUG_LOG("ADD MEDAL [%d %d %d %d]", p->id, p->waitcmd, type, count);
    uint32_t dbmsg[3];
    dbmsg[0] = p->tmpinfo.team;
    dbmsg[1] = type;
    dbmsg[2] = count;
    return send_request_to_db(SVR_PROTO_SPORT_ADD_TEAM_MEDAL, 0, 12, dbmsg, p->id);
}

int sports_get_task_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    if(!p->tmpinfo.task_id) {
        int l = sizeof(protocol_t);
        PKG_UINT32(msg, 0, l);
        PKG_UINT32(msg, 0, l);
        init_proto_head(msg, p->waitcmd, l);
        return send_to_self(p, msg, l, 1);

    }
    return send_request_to_db(SVR_PROTO_SPORT_GET_TASK, p, 0, NULL, p->id);
}

int check_task_can_get(uint32_t task_info, uint32_t task_id)
{
    if(task_id > 14) {
        ERROR_LOG("task id error [%d]", task_id);
        return 0;
    }
    return task_info >> ((task_id - 1)* 2) & 0x3;
}

int sports_get_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len ,4);
    uint32_t task_info = *(uint32_t*)buf;
    uint32_t result = check_task_can_get(task_info, p->tmpinfo.task_id);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, p->tmpinfo.task_id, l);
    PKG_UINT32(msg, result, l);
    init_proto_head(msg, p->waitcmd, l);
    return send_to_self(p, msg, l, 1);
}

int sports_start_task_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    if(p->tmpinfo.task_id) {
        uint32_t dbmsg[2];
        dbmsg[0] = p->tmpinfo.task_id;
        dbmsg[1] = 1;
        DEBUG_LOG("START TASK [%d %d %d]", p->id, p->tmpinfo.team, p->tmpinfo.task_id);
        return send_request_to_db(SVR_PROTO_SPORT_SET_TASK, p, 8, dbmsg, p->id);
    } else {
        return send_to_self_error(p, p->waitcmd, -ERR_sports_no_task, 1);
    }
}

int sports_fin_task_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    if(p->tmpinfo.task_id && p->tmpinfo.team) {
        uint32_t dbmsg[2];
        dbmsg[0] = p->tmpinfo.task_id;
        dbmsg[1] = 2;
        DEBUG_LOG("FIN TASK [%d %d %d]", p->id, p->tmpinfo.team, p->tmpinfo.task_id);
        return send_request_to_db(SVR_PROTO_SPORT_SET_TASK, p, 8, dbmsg, p->id);
    } else {
        return send_to_self_error(p, p->waitcmd, -ERR_sports_no_task, 1);
    }
}

int sports_set_task_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 0);
    switch(p->waitcmd) {
        case PROTO_SPORT_SET_TASK:
            return add_medal(p, 5, 1, 0);
        case PROTO_SPORT_START_TASK:
            response_proto_head(p, p->waitcmd, 0);
            break;
        default:
            break;
    }
    return 0;
}

int sports_get_npc_info(sprite_t *p)
{
    return send_request_to_db(SVP_PROTO_GET_NPC_INFO, p, 4, &p->tmpinfo.team, p->id);
}

int sports_get_npc_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 24);
    int i = 0;
    int taskid, npc_pos, npc_event, daily_mail, weekly_mail, hot_top;
    UNPKG_H_UINT32(buf, daily_mail, i);
    UNPKG_H_UINT32(buf, weekly_mail, i);
    UNPKG_H_UINT32(buf, hot_top, i);
    UNPKG_H_UINT32(buf, taskid, i);
    UNPKG_H_UINT32(buf, npc_pos, i);
    UNPKG_H_UINT32(buf, npc_event, i);
    DEBUG_LOG("NPC INFO [%d %d %d %d ]", p->id, taskid, npc_pos, npc_event);
    if(npc_pos > 86) {
        ERROR_LOG("NPC POS ERROR! [%d %d]", p->id, npc_pos);
        npc_pos = 86;
    }
    if(npc_path[npc_pos] == 1) {
        p->tmpinfo.task_id = taskid;
    } else {
        p->tmpinfo.task_id = 0;
    }
    p->tmpinfo.npc_pos = npc_pos;
    if(p->waitcmd == PROTO_SPORT_RAND_SIGN) {
        int type = *(uint32_t*)p->session;
        response_proto_uint32(p, p->waitcmd, type, 0);
        return 0;
    }
    return proc_final_login_step(p);
}

int sports_team_history_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    int type;
    UNPKG_UINT32(body, type, i);
    if(type != 0 && type != 1) {
        ERROR_RETURN(("type error [%d %d]", p->id, type), -1);
    }
    return send_request_to_db(SVR_PROTO_SPORT_TEAM_HISTORY, p, 4, &type, p->id);
}

int sports_team_history_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN_GE(len, 4);
    uint32_t count = *(uint32_t*)buf;
    typedef struct _t_info {
        uint32_t data;
        uint32_t team;
        uint32_t gold;
        uint32_t silver;
        uint32_t copper;
        uint32_t member;
    }team_info_t;
    CHECK_BODY_LEN(len, 4 + sizeof(team_info_t) * count);
    team_info_t *team_info = (team_info_t*)(buf +4);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, count, l);
    int i;
    for(i = 0; i < count; i++) {
        PKG_UINT32(msg, team_info[i].team, l);
        PKG_UINT32(msg, team_info[i].gold, l);
        PKG_UINT32(msg, team_info[i].silver, l);
        PKG_UINT32(msg, team_info[i].copper, l);
        PKG_UINT32(msg, team_info[i].member, l); 
    }
    init_proto_head(msg, p->waitcmd, l);
    return send_to_self(p, msg, l, 1);
}

int sports_water_maomao_tree(sprite_t *p, uint32_t uid, int ret)
{
    return send_request_to_db(SVR_PROTO_SPORT_WATER_TREE, ret?p:0, 4, &uid, p->id);
}

int sports_watering_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    if(p->tmpinfo.task_id != 10) {
        ERROR_RETURN(("no task [%d %d]", p->id, p->tmpinfo.task_id), -1);
    }
    return send_request_to_db(SVR_PROTO_SPORT_WATER_CNT, p, 0, NULL, p->id);

}

int sports_get_watering_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
    CHECK_BODY_LEN(len, 4);
    uint32_t count = *(uint32_t*)buf;
    //DEBUG_LOG("WATERING [%d %d]", p->id, count);
    response_proto_uint32(p, p->waitcmd, count, 0);
    return 0;
}

int sports_get_team_cloth_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0; 
    int type;
    UNPKG_UINT32(body, type, i);
    if(!p->tmpinfo.team) {
        ERROR_RETURN(("no team [%d]", p->id), -1);
    }
    if(type != 0 && type != 1) {
        ERROR_RETURN(("type error [%d %d]", p->id, type), -1);
    }
    int j = 0;
	uint8_t buff[13];
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 12811, j);
	PKG_H_UINT32(buff, 12831, j);
	PKG_UINT8(buff, 2, j);
    *(uint32_t*)p->session = type;
	return send_request_to_db(SVR_PROTO_GET_ITEM, p, j, buff, p->id);

}

int add_team_cloth(sprite_t* p, uint8_t* buf, int count)
{
    if(count == 2) {
        return send_to_self_error(p, p->waitcmd, -ERR_already_get_item, 1);
    }
    static int boy_hat[5] = {12815, 12811, 12823, 12827, 12819};
    static int boy_cloth[5] = {12816, 12812, 12824, 12828, 12820};
    static int girl_hat[5] = {12817, 12813, 12825, 12829,12821};
    static int girl_cloth[5] = {12818, 12814, 12826, 12830,12822};
    uint32_t type = *(uint32_t*)p->session;
    if(!type) {
        db_single_item_op(0, p->id, boy_hat[p->tmpinfo.team -1], 1, 1);
        db_single_item_op(0, p->id, boy_cloth[p->tmpinfo.team -1], 1, 1);
    } else {
        db_single_item_op(0, p->id, girl_hat[p->tmpinfo.team -1], 1, 1);
        db_single_item_op(0, p->id, girl_cloth[p->tmpinfo.team -1], 1, 1);
    }
    response_proto_head(p, p->waitcmd, 0); 
    return 0;
}

int sports_reward_break_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t type;
    UNPKG_UINT32(body, type, i);
    if(type > 2) {
        ERROR_RETURN(("type err [%d %d %d]", p->id, type, p->tmpinfo.team), -1);
    }
    if(!p->tmpinfo.team) {
        return send_to_self_error(p, p->waitcmd, -ERR_sports_not_sign, 1);
    }
    *(uint32_t*)p->session = type;
    return db_get_sports_team(p);
}

int sports_reward_reach_max_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t type;
    UNPKG_UINT32(body, type, i);
    if(type > 2) {
        ERROR_RETURN(("type err [%d %d %d]", p->id, type, p->tmpinfo.team), -1);
    }
    if(!p->tmpinfo.team) {
        return send_to_self_error(p, p->waitcmd, -ERR_sports_not_sign, 1);
    }
    *(uint32_t*)p->session = type;
    return db_get_sports_team(p);
}

int sports_chance_cylce_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    //check user whether can do this
    if(!p->tmpinfo.team) {
        return send_to_self_error(p, p->waitcmd, -ERR_sports_not_sign, 1);
    }
    if(npc_path[p->tmpinfo.npc_pos] != 2) {
        return send_to_self_error(p, p->waitcmd, -ERR_sports_chance_cycle, 1);
    }
    return db_set_sth_done(p, 134, 1); 
}

int do_sports_chance_cycle(sprite_t *p)
{
    int j = 0;
	uint8_t buff[13];
	PKG_H_UINT32(buff, 0, j);
	PKG_H_UINT32(buff, 12835, j);
	PKG_H_UINT32(buff, 12841, j);
	PKG_UINT8(buff, 2, j);
	return send_request_to_db(SVR_PROTO_GET_ITEM, p, j, buff, p->id);

}

int get_chance_cycle_reward(sprite_t* p, uint8_t* buf, int count)
{
    int cloth_info[5] = {0};
    static int cloth_id[5] = {12835, 12836, 12837, 12838, 12840};
    int i, k;

    int pos = 0;
    for(i = 0; i < 5; i++) {
        int exist = 0;
        for(k = 0; k < count; k++) {
            int tmp_id = *(uint32_t*)(buf + 8 * k);
            int tmp_cnt = *(uint32_t*)(buf + 8 * k + 4);
            if(cloth_id[i] == tmp_id && tmp_cnt) {
                exist = tmp_id;
                break;
            }
        }
        if(!exist) {
            cloth_info[pos] = cloth_id[i];
            pos++;
        }
    }
    DEBUG_LOG("CLOTH INFO [%d %d %d %d %d %d]", p->id, cloth_info[0], cloth_info[1], cloth_info[2], cloth_info[3], cloth_info[4]);
    int itmid = 0;
    if(pos) {
        itmid = cloth_info[rand()%pos];
	    db_single_item_op(0, p->id, itmid, 1, 1);
    } else {
        itmid = rand()%3 + 3;
        *(uint32_t*)p->session = itmid;
        DEBUG_LOG("CHANCE CYCLE[%d %d]", p->id, itmid);
        return add_medal(p, itmid, 1, 0);
    }
    DEBUG_LOG("CHANCE CYCLE[%d %d]", p->id, itmid);
    response_proto_uint32(p, p->waitcmd, itmid, 0);
    return 0;
}

int sports_self_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    if(!p->tmpinfo.team) {
        return send_to_self_error(p, p->waitcmd, -ERR_sports_not_sign, 1);
    }
    return db_get_sports_team(p);
}

int sports_get_fire_stat_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    if(!p->tiles) {
        ERROR_RETURN(("map error %d", p->id), -1);
    }
    int idx = 0;
    switch(p->tiles->id) {
        case 8:
            break;
        case 3:
            idx = 1;
            break;
        case 10:
            idx = 2;
            break;
        case 77:
            idx = 3;
            break;
        case 37:
            idx = 4;
            break;
        default:
            ERROR_RETURN(("map error %d %d", p->id, p->tiles->id), -1);
    }
    response_proto_uint32(p, p->waitcmd, team_fire[idx], 0);
    return 0;
}

int fire_stop(void* owner, void* data)
{
    int idx = 0;
	map_t* mp = (map_t*)owner;
    switch(mp->id) {
        case 8:
            break;
        case 3:
            idx = 1;
            break;
        case 10:
            idx = 2;
            break;
        case 77:
            idx = 3;
            break;
        case 37:
            idx = 4;
            break;
        default:
            ERROR_RETURN(("map error "), -1);
    }
    team_fire[idx] = 0;
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, team_fire[idx], l);
    init_proto_head(msg, PROTO_SPORT_FIRE_STAT, l);
    send_to_map2(mp, msg, l);
    DEBUG_LOG("FIRE STOP [%d]", mp->id);
    return 0;
}

int sports_set_fire_start_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    if(!p->tiles) {
        ERROR_RETURN(("map error %d", p->id), -1);
    }
    int idx = 0;
    switch(p->tiles->id) {
        case 8:
            break;
        case 3:
            idx = 1;
            break;
        case 10:
            idx = 2;
            break;
        case 77:
            idx = 3;
            break;
        case 37:
            idx = 4;
            break;
        default:
            ERROR_RETURN(("map error %d", p->id), -1);
    }
    if(!team_fire[idx]) {
        team_fire[idx] = 1;
		ADD_TIMER_EVENT(p->tiles, fire_stop, 0, now.tv_sec + 1800);
        int l = sizeof(protocol_t);
        PKG_UINT32(msg, team_fire[idx], l);
        init_proto_head(msg, PROTO_SPORT_FIRE_STAT, l);
        send_to_map2(p->tiles, msg, l);
        DEBUG_LOG("FIRE START [%d]", p->id);
    }
    response_proto_uint32(p, p->waitcmd, team_fire[idx], 0);
    return 0;
}

int do_sports_max_reward(sprite_t *p)
{
    uint32_t type = *(uint32_t*)p->session;
    static int max_reward[3] = {12839, 160374,160375};
    *(uint32_t*)(p->session + 4) = max_reward[type];
    char buf[20];
    int i = 0;
    PKG_H_UINT32(buf, 1, i);
    PKG_H_UINT32(buf, max_reward[type], i);
    PKG_H_UINT32(buf, 1, i);
    PKG_H_UINT32(buf, 99, i);
    PKG_H_UINT32(buf, 0, i);
    return send_request_to_db(SVR_PROTO_BUY_ITEM, p, 20, buf, p->id);
}

int get_reward_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
	uint32_t team_items[5] = {160383, 160384, 160385, 160385, 160385};
    if(!p->tmpinfo.team || p->tmpinfo.team > 5) {
        return send_to_self_error(p, p->waitcmd, -ERR_sports_not_sign, 1);
    }
	
	*(uint32_t*)p->session = team_items[p->tmpinfo.team - 1];
    char buf[20];
    int i = 0;
    PKG_H_UINT32(buf, 1, i);
    PKG_H_UINT32(buf, team_items[p->tmpinfo.team - 1], i);
    PKG_H_UINT32(buf, 1, i);
    PKG_H_UINT32(buf, 1, i);
    PKG_H_UINT32(buf, 0, i);
    return send_request_to_db(SVR_PROTO_BUY_ITEM, p, 20, buf, p->id);
}

