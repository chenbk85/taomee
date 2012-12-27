/**
 * =====================================================================================
 *       @file  flight.cpp
 *      @brief  飞行棋源文件
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  03/19/2009 05:32:07 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  sylar (), sylar@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <libtaomee++/random/random.hpp>
#include <ant/inet/pdumanip.hpp>
#include "flight.hpp"
extern "C" {
#include <gameserv/proto.h>
#include <gameserv/timer.h>
#include <gameserv/dbproxy.h>

}

const char *event_type_txt[13] = {
    "event_none",
    "event_wait",
    "event_start",
    "event_finished",
    "event_kill",
    "event_normal_fly_start",
    "event_normal_fly_end",
    "event_big_fly_start",
    "event_big_fly_end",
    "event_rain",
    "event_storm",
    "event_add_item",
    "event_overlap",
};


Flight::Flight(game_group_t *grp):owner(0), players_cnt(0), bout_cnt(0),winner_cnt(0)
{
    m_grp = grp;
    players_cnt = grp->count;
    int pos_order[4] = {0, 2, 1, 3};
    for(int i = 0; i < players_cnt; i++) {
        m_player[i].setup(m_grp->players[i], pos_order[i]);
    }
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < players_cnt; j++) {
            if(m_player[j].get_pos() == i) {
                player.push_back(m_player[j]);
            }
        }
    }

    for(int i = 0; i < players_cnt; i++) {
        //DEBUG_LOG("%lu PLAYER[%d] %d pos %d", m_grp->id, i, player[i].id(), player[i].get_pos() );
        int pos = player[i].get_pos();
        for(int j = 0; j < 4; j++) {
            //place planes at airport.
            map[GRID_AIRSHED + OFFSET_AIRPORT(pos) + j].plane_sn[0] = j;
            map[GRID_AIRSHED + OFFSET_AIRPORT(pos) + j].cnt = 1;
            map[GRID_AIRSHED + OFFSET_AIRPORT(pos) + j].owner = player[i].id();
            map[GRID_AIRSHED + OFFSET_AIRPORT(pos) + j].status = grid_unready;
        }
    }
    notify_game_start();
}

int
Flight::bonus_point[AIRLINE_LOOP_GRIDS + 1] = {
    -1,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,3,
    0,1,2,
};


bool
Flight::is_my_turn(sprite_t *p)
{
    if(player[owner].is_block()) {
        ERROR_LOG("%lu FLIGHT:you has been block\tuid=%d",m_grp->id, p->id);
        return false;
    }
    return (p->id == player[owner].id())? true:false;
}

void
Flight::delete_player(sprite_t *p)
{
    int pos = -1;
    int j = 0;
    std::vector<Player>::iterator it;
    for (it = player.begin(); it != player.end(); it++) {
        if((*it).id() == p->id) {
            pos = j;
            break;
        }
        j++;
    }
    if(pos != -1) {
        if(!(*it).is_block()) {
            for(int i = 0; i < players_cnt; i++) {
                if(i != pos && player[i].is_block())
                    player[i].set_status(status_normal_roll);
            }
        }
    }
    if(it != player.end()) {
        int pos[4];
        for(int i = 0; i < 4; i++) {
            pos[i] = ((*it).get_plane_pos(i));
            DEBUG_LOG("%lu CLEAR [%d %d]", m_grp->id, (*it).id(), ((*it).get_plane_pos(i)));
            grid_reset(((*it).get_plane_pos(i)));
        }
        player.erase(it);
        players_cnt--;
    }
    if(pos != -1) {
        if(pos < owner) {
            owner = owner - 1;
        } else if (pos == owner && pos == players_cnt) {
            owner = 0;
        }
    }
    //DEBUG_LOG("%lu DELETE PLAYER %d POS %d OWNER %d", m_grp->id, p->id, pos, owner);
}

int
Flight::handle_data(sprite_t* p, int cmd, const uint8_t body[], int len)
{
    switch (cmd){
        case proto_player_leave:
            {
                if(player[owner].id() == p->id) {
                    notify_player_leave(p->id, player[(owner +1)%players_cnt].id());
                }
                delete_player(p);
                DEBUG_LOG("%lu LEAVE GAME [%d %d %d]", m_grp->id, p->id, player[owner].id(), player[(owner +1)%players_cnt].id());
                //for (int i = 0; i < players_cnt; i++) {
                //    DEBUG_LOG("%lu player[%d] id %d", m_grp->id, i, player[i].id());
                //}
                if(players_cnt == 1) {
                    notify_player_end(player[0].get_sprite());
                    return GER_end_of_game;
                }
            }
            return 0;
        case GAME_SYN:
            notify_game_syn(p->id);
            return 0;
    }

    if(!is_my_turn(p)) {
        ERROR_LOG("%lu FLITHG:it is not your turn\tuid=%d %d %d",m_grp->id, p->id, p->waitcmd, player[owner].id());
        return GER_end_of_game;
    }
    memset(path, 0, sizeof(path));

    //DEBUG_LOG("%lu MY TURN %d", m_grp->id, player[owner].id());

    switch (cmd){
        case ROLL:
            {
                int roll_point = player[owner].roll();
                if(!roll_point) {
                    ERROR_LOG("%lu FLIGHT:illegal roll\t uid=%d",m_grp->id, p->id);
                    return GER_end_of_game;
                }
                if(roll_point != 6 && player[owner].no_plane_can_move()) {

                    player[owner].use_power();
                    if(player[owner].can_continue()) {
                        player[owner].set_discontinue();
                        notify_roll_point(roll_point, player[owner].id());
                        player[owner].reset();
                    } else if (player[owner].can_roll_again()){
                        //DEBUG_LOG("%lu CAN ROLL AGAIN %d", m_grp->id, player[owner].id());
                        notify_roll_point(roll_point, player[owner].id());
                        player[owner].set_can_use_item();
                    } else {
                        notify_roll_point(roll_point, player[(owner + 1)%players_cnt].id());
                        player[owner].reset();
                        owner = (owner + 1)%players_cnt;
                        player[owner].reset();
                    }
                    map_update();
                    break;
                }
                notify_roll_point(roll_point, 0);
            }
            break;
        case ITEM:
            {
                CHECK_BODY_LEN(len, 8);
                uint32_t item_id = 0;
                uint32_t grid_id = 0;
                int i = 0;
                ant::unpack(body, item_id, i);
                ant::unpack(body, grid_id, i);
                if(player[owner].use_item(item_id)) {
                    ERROR_LOG("%lu FLIGHT:use item error [%d %d]", m_grp->id, player[owner].id(), item_id);
                    notify_use_item(p->id, item_id, 0, grid_id);
                    return 0;
                }
                return handle_item(item_id, grid_id);
            }
        case MOVE:
            //player's action grid_finished
            {
                CHECK_BODY_LEN(len, 4);
                uint32_t grid_id = 0;
                int i = 0;
                ant::unpack(body, grid_id, i);
                //DEBUG_LOG("%lu PLAYER WANT TO MOVE  %d %d", m_grp->id, p->id, grid_id);

                if(grid_id > GRIDS_TOTAL || grid_id ==  0) {
                    ERROR_LOG("%lu Flight: %d error grid id %d", m_grp->id, p->id, grid_id);
                    return GER_end_of_game;
                }

                if(map[grid_id].owner!= player[owner].id() || map[grid_id].cnt < 1 || map[grid_id].cnt > 4) {
                    ERROR_LOG("%lu FLIGHT:%d can not move %d, owner %d, plane cnt %d", m_grp->id, p->id, grid_id, map[grid_id].owner, map[grid_id].cnt);
                    return GER_end_of_game;
                }

                int step = player[owner].use_power();
                if(step == 0){
                    ERROR_LOG("%lu FLIGHT:STEP error %d %d", m_grp->id, step, p->id);
                    return GER_end_of_game;
                }

                if(handle_action(grid_id, step)) {
                    return GER_end_of_game;
                }
                //DEBUG_LOG("%lu finished num [%d %d %d]", m_grp->id, p->id, player[owner].id(), player[owner].plane_finished_num());
                if(player[owner].plane_finished_num() == PLANE_NUM_CAN_WIN) {
                    notify_player_action(player[(owner + 1)%players_cnt].id());
                    delete_player(p);
                    notify_player_win(p);
                    if(players_cnt == 1) {
                        notify_player_end(player[0].get_sprite());
                        return GER_end_of_game;
                    }
                    map_update();
                    return 0;
                }
                if(player[owner].can_roll_again() || player[owner].can_continue()) {
                    notify_player_action(player[owner].id());
                    player[owner].set_can_use_item();
                } else {
                    notify_player_action(player[(owner + 1)%players_cnt].id());
                }
                game_update();

            }
            break;
        default:
            //undef cmd;
            ERROR_LOG("%lu FLIGHT:undef cmd %d, uid=%d", m_grp->id, cmd, m_grp->players[owner]->id);
            return GER_end_of_game;
    }
    return 0;
}

int
Flight::handle_item(int item_id, int grid_id)
{
    uint32_t target = map[grid_id].owner;
    msg_len = ITEM_ATK_MSG_HEAD + sizeof(protocol_t);
    DEBUG_LOG("%lu USE ITEM [%d %d %d]", m_grp->id, player[owner].id(), item_id, grid_id);
    switch (item_id) {
        case item_shield:
            if(grid_id == 0  || grid_id >= GRID_AIRSHED || target != player[owner].id()) {
                ERROR_LOG("%lu FLIGHT:shield target error %d %d", m_grp->id, player[owner].id(), grid_id);
                return GER_end_of_game;
            }
            //DEBUG_LOG("%lu SHIELD %d grid[%d] %d planes", m_grp->id, player[owner].id(), grid_id, map[grid_id].cnt);
            map[grid_id].status = grid_shielded;
            break;
        case item_twice_roll:
            player[owner].set_status(status_twice_roll);
            player[owner].set_can_use_item();
            break;
        case item_get_six:
            player[owner].set_status(status_get_six);
            break;
        case item_block:
            for(int i = 0; i < players_cnt; i++){
                if(i != owner) {
                    player[i].set_status(status_blocked);
                }
            }
            player[owner].set_can_continue();
            player[owner].set_can_use_item();
            break;
        case item_attack:
            {
                if(grid_id >= GRID_AIRSHED || grid_id == 0) {
                    ERROR_LOG("%lu FLIGHT:attack target out of range %d %d", m_grp->id, player[owner].id(), grid_id);
                    return GER_end_of_game;
                }

                if(target == (uint32_t)EMPTY || target == player[owner].id()) {
                    ERROR_LOG("%lu FLIGHT:attack target error %d %d", m_grp->id, player[owner].id(), target);
                    return GER_end_of_game;
                }
                uint8_t attack_suc = NO_DATA;
                if(map[grid_id].status != grid_shielded) {
                    DEBUG_LOG("%lu ITEM ATTACK [%d %d %d]", m_grp->id, player[owner].id(), grid_id, target);
                    attack_suc = 1;
                    ant::pack(pkg, attack_suc, msg_len);
                    grid_attack(grid_id);
                } else {
                    ant::pack(pkg, attack_suc, msg_len);
                    map[grid_id].status = grid_free;
                }
                break;
            }
        default:
            ERROR_LOG("%lu FLIGHT: ITEM type error %d ", m_grp->id, item_id);
            return GER_end_of_game;
    }
    notify_use_item(player[owner].id(), item_id, target, grid_id);
    return 0;
}

int
Flight::handle_action(int grid_id, int steps)
{
    DEBUG_LOG("%lu MAP INFO %d [%d %d %d %d]", m_grp->id, player[owner].id(), player[owner].get_plane_pos(0), player[owner].get_plane_pos(1), player[owner].get_plane_pos(2), player[owner].get_plane_pos(3));

    path_cnt = 0;
    is_all_move = all_move;
    int power = steps + 1;
    int now;
    int next = grid_id;
    int direction = 1;
    bool has_big_fly = false;
    bool has_normal_fly = false;
    bool has_encounter_weather = false;

    is_end = false;
    msg_len = PATH_MSG_HEAD_LEN + sizeof(protocol_t);
    while(power) {
        now = next;
        ant::pack(pkg, (uint8_t)now, msg_len);
        //take off
        if(now >= (GRID_AIRSHED + OFFSET_AIRPORT(player[owner].get_pos())) && now < (GRID_AIRPORT_START + OFFSET_AIRPORT(player[owner].get_pos()))) {
            if(steps == 6 && map[now].status == grid_unready) {
                next = GRID_AIRPORT_START + OFFSET_AIRPORT(player[owner].get_pos());
                //DEBUG_LOG("%lu PLANE TAKE OFF %d => %d", m_grp->id, now, next);
                ant::pack(pkg, (uint8_t)event_wait, msg_len);
                path[path_cnt].event_id[path[path_cnt].event_cnt] = event_wait;
                path[path_cnt].event_cnt++;
                power = 2;
            } else {
                //error
                ERROR_LOG("%lu FLIGHT:error take off, roll point %d %d", m_grp->id, player[owner].id(), steps);
                return GER_end_of_game;
            }
        //start
        } else if(now == (GRID_AIRPORT_START + OFFSET_AIRPORT(player[owner].get_pos()))) {
            if(power - 1) {
                next = GRID_START + OFFSET_NORMAL(player[owner].get_pos());
                //DEBUG_LOG("%lu START %d = > %d", m_grp->id, now, next);
                is_all_move = single_move;
                ant::pack(pkg, (uint8_t)event_start, msg_len);
                path[path_cnt].event_id[path[path_cnt].event_cnt] = event_start;
                path[path_cnt].event_cnt++;
            }
        //landing
        } else if(now == (GRID_LANDING + OFFSET_NORMAL(player[owner].get_pos()))%AIRLINE_LOOP_GRIDS) {
            next = (GRID_RUNWAY_START + OFFSET_RUNWAY(player[owner].get_pos()));
            //DEBUG_LOG("%lu LANDING %d = > %d", m_grp->id, now, next);
        //at the finish point
        } else if(now == (GRID_RUNWAY_FIN + OFFSET_RUNWAY(player[owner].get_pos()))) {
            if(!(power - 1)) {
                //end;
                ant::pack(pkg, (uint8_t)event_finished, msg_len);
                is_end = true;
                path[path_cnt].event_id[path[path_cnt].event_cnt] = event_finished;
                path[path_cnt].event_cnt++;
            } else {
                direction = -1;
            }
            //DEBUG_LOG("%lu RUNWAY FIN %d, direction %d", m_grp->id, now, direction);
        //come across head and tail
        } else if(now == AIRLINE_LOOP_GRIDS) {
            next = GRID_START;
            //DEBUG_LOG("%lu HEAD TAIL %d = > %d", m_grp->id, now, next);
        //big fly finished
        } else if (now == ((GRID_BIG_FLY_FIN + OFFSET_NORMAL(player[owner].get_pos()))%AIRLINE_LOOP_GRIDS) && has_big_fly) {
            ant::pack(pkg, (uint8_t)event_big_fly_end, msg_len);
            path[path_cnt].event_id[path[path_cnt].event_cnt] = event_big_fly_end;
            path[path_cnt].event_cnt++;
            //DEBUG_LOG("%lu BIG FLY FIN %d", m_grp->id, now);
        //normal fly end
        } else if (player[owner].get_pos() == bonus_point[now] && has_normal_fly) {
            ant::pack(pkg, (uint8_t)event_normal_fly_end, msg_len);
            path[path_cnt].event_id[path[path_cnt].event_cnt] = event_normal_fly_end;
            path[path_cnt].event_cnt++;
            //DEBUG_LOG("%lu NORMAL FLY FIN %d", m_grp->id, now);
        }
        //normal point, increase step
        if(next == now) {
            next = now + direction;
            if(power -1) {
                //DEBUG_LOG("%lu NORMAL MOVE %d = > %d", m_grp->id, now, next);
            }
        }
        //encounter rain
        if(map[now].weather == rain) {
            ant::pack(pkg, (uint8_t)event_rain, msg_len);
            path[path_cnt].event_id[path[path_cnt].event_cnt] = event_rain;
            path[path_cnt].event_cnt++;
            has_encounter_weather = true;
            if((power - 1)) {
                power = 2;
            }
        }

        //encounter storm
        if(map[now].weather == storm) {
            ant::pack(pkg, (uint8_t)event_storm, msg_len);
            path[path_cnt].event_id[path[path_cnt].event_cnt] = event_storm;
            path[path_cnt].event_cnt++;
            do {
                next = taomee::ranged_random(GRID_START, AIRLINE_LOOP_GRIDS);
            }while( map[next].weather != weather_none);
            has_encounter_weather = true;
            power = 2;
        }

        if(!(power - 1)) {
            //get item
            if(map[now].item != item_none) {
                DEBUG_LOG("%lu GET ITEM %d %d %d ", m_grp->id, player[owner].id(), now, map[now].item);
                player[owner].add_item(map[now].item);
                ant::pack(pkg, (uint8_t)event_add_item, msg_len);
                ant::pack(pkg, (uint8_t)map[now].item, msg_len);
                map[now].item = item_none;
                path[path_cnt].event_id[path[path_cnt].event_cnt] = event_add_item;
                path[path_cnt].event_cnt++;
            }
            //attack
            if(grid_attack(now)){
                path[path_cnt].event_id[path[path_cnt].event_cnt] = event_kill;
                path[path_cnt].event_cnt++;
            }
            //dangerous runway point
            if((now > (GRID_RUNWAY_FIN + OFFSET_RUNWAY(player[owner].get_pos()))
                    || now < (GRID_RUNWAY_START + OFFSET_RUNWAY(player[owner].get_pos())))
                        && ((now == 67 || now == 73 || now == 55 || now ==61))) {
                next = (GRID_BIG_FLY_FIN + OFFSET_NORMAL(player[owner].get_pos()))%AIRLINE_LOOP_GRIDS;
                //DEBUG_LOG("%lu BIG FLY ACROSS RUNWAY %d %d = > %d", m_grp->id, player[owner].id(), now, next);
                power = 2;
            }
            //big fly start
            if((now == (GRID_BIG_FLY_START + OFFSET_NORMAL(player[owner].get_pos()))%AIRLINE_LOOP_GRIDS)
                    && !has_big_fly
                    && !has_encounter_weather) {
                ant::pack(pkg, (uint8_t)event_big_fly_start, msg_len);
                path[path_cnt].event_id[path[path_cnt].event_cnt] = event_big_fly_start;
                path[path_cnt].event_cnt++;
                int runway_risk[4] = {67, 73, 55, 61};
                next = runway_risk[player[owner].get_pos()];
                //DEBUG_LOG("%lu BIG FLY START %d = > %d", m_grp->id, now, next);
                power = 2;
                has_big_fly = true;
            }
            //normal fly
            int pos = player[owner].get_pos();
            if(now >= GRID_START && now <= AIRLINE_LOOP_GRIDS) {
                if(pos == bonus_point[now]
                        && !has_normal_fly
                        && !has_encounter_weather
                        && now != ((GRID_LANDING + OFFSET_NORMAL(player[owner].get_pos()))%AIRLINE_LOOP_GRIDS)
                        && now != (((GRID_BIG_FLY_START + OFFSET_NORMAL(player[owner].get_pos())))%AIRLINE_LOOP_GRIDS))  {
                    power = 5;
                    //DEBUG_LOG("%lu NORMAL FLY START %d", m_grp->id, now);
                    has_normal_fly = true;
                    ant::pack(pkg, (uint8_t)event_normal_fly_start, msg_len);
                    path[path_cnt].event_id[path[path_cnt].event_cnt] = event_normal_fly_start;
                    path[path_cnt].event_cnt++;
                }  else {
                    //DEBUG_LOG("%lu DO NOT NORMAL FLY %d, %d, %d, %d, %d, %d, %d", pos, now, bonus_point[now], has_normal_fly, has_encounter_weather,  ((GRID_LANDING + OFFSET_NORMAL(player[owner].get_pos()))%AIRLINE_LOOP_GRIDS), (((GRID_BIG_FLY_START + OFFSET_NORMAL(player[owner].get_pos())))%AIRLINE_LOOP_GRIDS));
                }

            }
        }

        if((!(power - 1)) && map[now].owner == player[owner].id()
                && (now != (GRID_AIRPORT_START + OFFSET_AIRPORT(player[owner].get_pos())))) {
            ant::pack(pkg, (uint8_t)event_overlap, msg_len);
            path[path_cnt].event_id[path[path_cnt].event_cnt] = event_overlap;
            path[path_cnt].event_cnt++;
        }

        if(!path[path_cnt].event_cnt) {
            ant::pack(pkg, (uint8_t)event_none, msg_len);
        }
        ant::pack(pkg, (uint8_t)PATH_POINT_END, msg_len);
        path[path_cnt].grid_id = now;
        path_cnt++;
        power--;
    }

    grid_move(path[path_cnt - 1].grid_id, path[0].grid_id);

    for(int i = 0; i < path_cnt; i++) {
        for(int j = 0; j < path[i].event_cnt; j++) {
            DEBUG_LOG("%lu %d %d GRIDS, PATH[%d] = %d, EVENT[%d] = %s", m_grp->id, player[owner].id(),path_cnt, i, path[i].grid_id, j, event_type_txt[path[i].event_id[j]]);
        }
    }
    return 0;
}

int
Flight::grid_attack(int target)
{
    uint32_t enemy = map[target].owner;
    if(enemy != (uint32_t)EMPTY && enemy != player[owner].id()) {
        DEBUG_LOG("%lu %d ATTACK GRID ENEMY [%d %d %d]", m_grp->id, player[owner].id(), target, enemy, map[target].cnt);
        ant::pack(pkg, (uint8_t)event_kill, msg_len);
        ant::pack(pkg, enemy, msg_len);
        ant::pack(pkg, (uint8_t)map[target].cnt, msg_len);
        int pos;
        int idx;
        for(int i = 0; i < players_cnt; i++) {
            if(enemy == player[i].id()) {
                pos = player[i].get_pos();
                idx = i;
            }
        }
        if(map[target].status == grid_shielded) {
            map[target].status = grid_free;
        }
        //enemy back to airport
        for(int i = 0; i < map[target].cnt; i++) {
            int airport_id= GRID_AIRSHED + OFFSET_AIRPORT(pos) + map[target].plane_sn[i];
            map[airport_id].owner = enemy;
            map[airport_id].cnt = 1;
            map[airport_id].plane_sn[0] = map[target].plane_sn[i];
            map[airport_id].status = grid_unready;
            player[idx].plane_move(map[target].plane_sn[i], airport_id);
            DEBUG_LOG("%lu UNDER ATTACK [%d %d %d]", m_grp->id, enemy, map[target].plane_sn[i], airport_id);
            ant::pack(pkg, (uint8_t)map[target].plane_sn[i], msg_len);
        }
        for(int i = 0; i < (4 - map[target].cnt); i++) {
            ant::pack(pkg, (uint8_t)NO_DATA, msg_len);
        }
        map[target].cnt = 0;
        map[target].owner = EMPTY;
        for(int i = 0; i < 4; i++) {
            map[target].plane_sn[i] = EMPTY;
        }
        return -1;
    }
    return 0;
}

void
Flight::grid_reset(int grid_id)
{
    map[grid_id].owner = EMPTY;
    map[grid_id].cnt = 0;
    map[grid_id].status = grid_free;
    for(int i = 0; i < 4; i++) {
        map[grid_id].plane_sn[i] = EMPTY;
    }
}



void
Flight::grid_clear(int grid_id)
{
    if(path[0].grid_id == (GRID_AIRPORT_START + OFFSET_AIRPORT(player[owner].get_pos()))) {
        int cnt = map[path[0].grid_id].cnt;
        map[path[0].grid_id].plane_sn[cnt] = EMPTY;
        //DEBUG_LOG("%lu CLEAR START POINT %d cnt %d", m_grp->id, path[0].grid_id, cnt);
        return;

    }
    //DEBUG_LOG("%lu CLEAR [%d %d]", m_grp->id, map[grid_id].owner, grid_id);
    map[grid_id].owner = EMPTY;
    map[grid_id].cnt = 0;
    map[grid_id].status = grid_free;
    for(int i = 0; i < 4; i++) {
        map[grid_id].plane_sn[i] = EMPTY;
    }
}

void
Flight::grid_move(int target, int start)
{
    int len = sizeof(protocol_t) + 8;
    int plane_move_num;
    if(is_all_move) {
        plane_move_num = map[start].cnt;
    } else{
        plane_move_num = 1;
    }
    ant::pack(pkg, (uint8_t)plane_move_num, len);
    uint8_t is_shielded = NO_DATA;
    if(map[path[0].grid_id].status == grid_shielded) {
        is_shielded = 1;
    }
    ant::pack(pkg, is_shielded, len);
    DEBUG_LOG("%lu %d PLANE MOVE %d PLANES %d => %d %d", m_grp->id, player[owner].id(), plane_move_num, start, target, map[start].cnt);
    if(start == target) {
        for(int i = 0; i < plane_move_num; i++) {
            ant::pack(pkg, (uint8_t)map[start].plane_sn[i], len);
        }
        for(int i = 0; i < (4 - plane_move_num); i++) {
            ant::pack(pkg, (uint8_t)NO_DATA, len);
        }
        ant::pack(pkg, (uint8_t)path_cnt, len);
        return;
    }
    int s_cnt = map[start].cnt;
    int t_cnt = map[target].cnt;
    for(int i = 0; i < plane_move_num; i++) {
        if(is_end) {
            target = GRID_AIRSHED + OFFSET_AIRPORT(player[owner].get_pos()) + map[start].plane_sn[s_cnt - i - 1];
            map[target].status = grid_finished;
            player[owner].plane_finished();
            DEBUG_LOG("%lu REACH END [%d %d %d]", m_grp->id, player[owner].id(), target, player[owner].plane_finished_num());
        }
        map[target].plane_sn[t_cnt + i] = map[start].plane_sn[s_cnt - i - 1];
        map[target].owner = player[owner].id();
        ant::pack(pkg, (uint8_t)map[start].plane_sn[s_cnt - i - 1], len);
        map[start].cnt--;
        map[target].cnt++;
        player[owner].plane_move(map[start].plane_sn[s_cnt - i - 1], target);
    }
    for(int i = 0; i < (4 - plane_move_num); i++) {
        ant::pack(pkg, (uint8_t)NO_DATA, len);
    }
    ant::pack(pkg, (uint8_t)path_cnt, len);
    if(map[start].status == grid_shielded) {
        map[target].status = grid_shielded;
    }
    grid_clear(start);
}



void
Flight::notify_roll_point(int point, uint32_t uid)
{
	int len = sizeof (protocol_t);
    ant::pack(pkg, player[owner].id(), len);
	ant::pack(pkg, point, len);
	ant::pack(pkg, uid, len);
	init_proto_head(pkg, ROLL, len);
    DEBUG_LOG("%lu ROLL [%d %d %d]", m_grp->id, player[owner].id(), point, uid);
    send_to_players(m_grp, pkg, len);
}

void
Flight::notify_use_item(uint32_t owner, int item_id, uint32_t target, int grid_id)
{
	int len = sizeof (protocol_t);
	ant::pack(pkg, owner, len);
	ant::pack(pkg, item_id, len);
	ant::pack(pkg, target, len);
	ant::pack(pkg, grid_id, len);
	init_proto_head(pkg, ITEM, len);
    send_to_players(m_grp, pkg, len);
}

void
Flight::notify_player_win(sprite_t *p)
{
    static int coin[3][4] = {
        {300, 0, 0, 0},
        {400, 200, 0, 0},
        {500, 300, 200, 0},
    };
    winner_cnt++;

	if (winner_cnt == 1){
		uint32_t db_buf[] = {0, 1, 0, 0, 99, 1351374, 1, 1};
		send_request_to_db(db_proto_modify_items, NULL, sizeof(db_buf), db_buf, p->id);
	}
    int len = sizeof (protocol_t);
    ant::pack(pkg, p->id, len);
	ant::pack(pkg, winner_cnt, len);
    DEBUG_LOG("%lu WINNER [%d %d]", m_grp->id, p->id, winner_cnt);
    game_score_t score;
    memset(&score, 0, sizeof(score));
    score.coins= coin[m_grp->game->players - 2][winner_cnt - 1];
    score.exp = score.coins/10;
    score.strong = score.coins/20;
    //score.itmid = get_fire_medal(p, winner_cnt - 1);
	ant::pack(pkg, score.coins, len);
	ant::pack(pkg, score.exp, len);
	ant::pack(pkg, score.strong, len);
	init_proto_head(pkg, GAME_WIN, len);
    send_to_players(m_grp, pkg, len);
    submit_game_score(p, &score);
}

void
Flight::notify_player_end(sprite_t *p)
{
    int coin = 50;
    if(get_now_tv()->tv_sec - m_grp->start <= 60){
        coin = 0;
        return;
    }
    winner_cnt++;
    int len = sizeof (protocol_t);
    ant::pack(pkg, p->id, len);
	ant::pack(pkg, winner_cnt, len);
    DEBUG_LOG("%lu LAST PLAYER[%d %d]", m_grp->id, p->id, winner_cnt);
    game_score_t score;
    memset(&score, 0, sizeof(score));
    score.coins= coin;
    score.exp = score.coins/10;
    score.strong = score.coins/20;
	ant::pack(pkg, score.coins, len);
	ant::pack(pkg, score.exp, len);
	ant::pack(pkg, score.strong, len);
	init_proto_head(pkg, GAME_WIN, len);
    send_to_players(m_grp, pkg, len);
    submit_game_score(p, &score);
}


void
Flight::notify_player_action(uint32_t uid)
{
    int len = sizeof(protocol_t);
    ant::pack(pkg, player[owner].id(), len);
    ant::pack(pkg, uid, len);
	init_proto_head(pkg, MOVE, msg_len);
    DEBUG_LOG("%lu ACTION [%d %d]", m_grp->id, player[owner].id(), uid);
//     for(int i = sizeof(protocol_t); i < msg_len; i++) {
//        // DEBUG_LOG("pkg[%d] %d", (i - sizeof(protocol_t)), pkg[i]);
//     }
    send_to_players(m_grp, pkg, msg_len);
}

void
Flight::notify_game_start()
{
    int len = sizeof(protocol_t);
    for(int i = 0; i < 4; i++) {
        uint32_t uid = 0;
        for(int j = 0; j < players_cnt; j++) {
            if(player[j].get_pos() == i) {
                uid = player[j].id();
            }
        }
        ant::pack(pkg, uid, len);
    }
	init_proto_head(pkg, GAME_READY, len);
    send_to_players(m_grp, pkg, len);
}

void
Flight::notify_game_syn(uint32_t uid)
{
    int len = sizeof(protocol_t);
    ant::pack(pkg, uid, len);
	init_proto_head(pkg, GAME_SYN, len);
    //DEBUG_LOG("%lu READY %d", m_grp->id, uid);
    send_to_players(m_grp, pkg, len);
}

void
Flight::notify_player_leave(uint32_t uid, uint32_t next_uid)
{
    int len= sizeof(protocol_t);
    ant::pack(pkg, uid, len);
    ant::pack(pkg, next_uid, len);
    DEBUG_LOG("%lu PLAYER LEAVE [%d %d]", m_grp->id, uid, next_uid);
    ant::pack(pkg, (uint8_t)0, len);
    ant::pack(pkg, (uint8_t)NO_DATA, len);
    ant::pack(pkg, (uint8_t)NO_DATA, len);
    ant::pack(pkg, (uint8_t)NO_DATA, len);
    ant::pack(pkg, (uint8_t)NO_DATA, len);
    ant::pack(pkg, (uint8_t)NO_DATA, len);
    ant::pack(pkg, (uint8_t)0, len);

	init_proto_head(pkg, MOVE, len);
    send_to_players(m_grp, pkg, len);
}

void
Flight::map_update()
{
    int len= sizeof(protocol_t);
    uint8_t map_pkg[1024];
    //weather update
    if((bout_cnt%3) == 1 && player[owner].id() == player[0].id()) {
        //clear old weather
        if(m_weather.grid_id != EMPTY) {
            map[m_weather.grid_id].weather = weather_none;
        }
        //create new
        m_weather.type = taomee::ranged_random(1,2);
        do {
            m_weather.grid_id = taomee::ranged_random(1,52);
        }while(map[m_weather.grid_id].owner != EMPTY
                    || map[m_weather.grid_id].item != item_none
                    || map[m_weather.grid_id].weather != weather_none);
        map[m_weather.grid_id].weather = m_weather.type;

        DEBUG_LOG("%lu WEATHER\t[%d %d]", m_grp->id, m_weather.type, m_weather.grid_id);

        for(int i = 0; i < 3; i++) {
            //clear old item
            if(m_item[i].grid_id != EMPTY) {
                    map[m_item[i].grid_id].item = item_none;
            }
            //create new
            bool is_exist;
            do {
                is_exist = false;
                m_item[i].type = taomee::ranged_random(1, 5);
                for(int j = 0; j < i; j++) {
                    if(m_item[j].type == m_item[i].type)
                        is_exist = true;
                }
            } while (is_exist);


            do {
                m_item[i].grid_id = taomee::ranged_random(GRID_START, AIRLINE_LOOP_GRIDS);
            }while(map[m_item[i].grid_id].owner != EMPTY
                    || map[m_item[i].grid_id].item != item_none
                    || map[m_item[i].grid_id].weather != weather_none);
            map[m_item[i].grid_id].item = m_item[i].type;
        }
        DEBUG_LOG("%lu ITEM\t[%d %d] [%d %d] [%d %d]", m_grp->id, m_item[0].type, m_item[0].grid_id, m_item[1].type, m_item[1].grid_id, m_item[2].type, m_item[2].grid_id);

        if(m_weather.type == rain) {
            ant::pack(map_pkg, (uint8_t)m_weather.grid_id, len);
            ant::pack(map_pkg, (uint8_t)NO_DATA, len);
        } else {
            ant::pack(map_pkg, (uint8_t)NO_DATA, len);
            ant::pack(map_pkg, (uint8_t)m_weather.grid_id, len);
        }

        for(int i = 0; i < 5; i++) {
            uint8_t item_grid_id = NO_DATA;
            for(int j = 0; j < 3; j++) {
                if((i+1) == m_item[j].type) {
                    item_grid_id = m_item[j].grid_id;
                }
            }
            ant::pack(map_pkg, item_grid_id, len);
        }
        init_proto_head(map_pkg, MAP_UPDATE, len);
        send_to_players(m_grp, map_pkg, len);
    }

    if(player[owner].id() == player[0].id()) {
        bout_cnt++;
    }
}

void
Flight::game_update()
{
    if(player[owner].bout_finished()) {
        player[owner].reset();
        //DEBUG_LOG("%lu %d, FINISHED THIS BOUT", m_grp->id, player[owner].id());
        map_update();
        if(player[owner].can_continue()) {
            player[owner].set_discontinue();
        } else {
            owner = (owner + 1)%players_cnt;
            player[owner].reset();
            for(int i = 0; i < players_cnt; i++) {
                if(player[i].is_block())
                    player[i].set_status(status_normal_roll);
            }
        }
        return;
    }
    //DEBUG_LOG("%lu %d, did not finish this bout", m_grp->id, player[owner].id());
    return;
}
