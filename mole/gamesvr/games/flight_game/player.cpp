/**
 * =====================================================================================
 *       @file  player.cpp
 *      @brief  玩家信息源文件
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  03/23/2009 11:22:53 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  sylar (), sylar@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include "player.hpp"
#include <libtaomee++/random/random.hpp>
Player::Player():plane_finished_cnt(0),roll_point(0),status(status_normal_roll),is_continue(0),can_use_item(true),can_roll_more(false)
{
    for(int i = 0; i < ITEM_KIND_NUM; i++) {
        item_cnt[i] = 0;
    }
}

int
Player::roll()
{
    if(roll_point != 0) {
        return 0;
    }
    switch(status) {
        case status_twice_roll:
            //DEBUG_LOG("ROLL status status_twice_roll %d", p->id);
            status = status_last_roll;
            can_roll_more = true;
            roll_point = taomee::ranged_random(1,7);
            if(roll_point == 6 || roll_point == 7) {
                roll_point = 6;
            }
            break;
        case status_last_roll:
            //DEBUG_LOG("ROLL status status_last_roll %d", p->id);
            roll_point = taomee::ranged_random(1,7);
            if(roll_point == 6 || roll_point == 7) {
                roll_point = 6;
            }
            status = status_none;
            can_roll_more = false;
            break;
        case status_get_six:
            //DEBUG_LOG("ROLL status status_get_six %d", p->id);
            roll_point = 6;
            status = status_none;
            can_roll_more = false;
            break;
        case status_normal_roll:
            //DEBUG_LOG("ROLL status status_normal_roll, %d", p->id);
            roll_point = taomee::ranged_random(1,7);
            if(roll_point == 6 || roll_point == 7) {
                roll_point = 6;
            }
            can_roll_more = false;
            if(roll_point != 6) {
                status = status_none;
            } else {
                can_roll_more = true;
                status = status_normal_roll;
            }
            break;
        default:
            //illegal situation
            roll_point = 0;
            ERROR_LOG("FLIGHT:illegal roll\tuid=%d,status=%d",p->id, status);
    }
    return roll_point;
}

int
Player::use_item(int item_id)
{
    if(item_id > ITEM_KIND_NUM || item_id < 1 || item_cnt[item_id - 1] == 0) {
        ERROR_LOG("FLIGHT:USE ITEM error %d %d", item_id, p->id);
        return -1;
    }
    //DEBUG_LOG("USE ITEM\tuid=%d, item_id=%d, cnt %d",p->id, item_id, item_cnt[item_id -1]);
    if(can_use_item){
        item_cnt[item_id - 1]--;
        can_use_item = false;
    } else {
        return -1;
    }
    return 0;
}

void
Player::setup(sprite_t *player, int pos)
{
    p = player;
    m_pos = pos;
    for(int j = 0; j < 4; j++) {
        plane_pos[j] = PLANE_START + 5 * m_pos + j;
    }
}

int
Player::use_power()
{
    int point = roll_point;
    roll_point = 0;
    return point;
}

bool
Player::bout_finished()
{
    if(status == status_none &&roll_point == 0) {
        return true;
    }
    //DEBUG_LOG("%d bout not finished status %d roll point %d", p->id, status, roll_point);
    return false;
}

bool
Player::can_continue()
{
    return is_continue;
}

void
Player::reset()
{
    set_status(status_normal_roll);
    can_use_item = true;
}

bool
Player::no_plane_can_move()
{
    bool res = true;
    for(int i = 0; i < 4; i++) {
        if(plane_pos[i] != (PLANE_START + 5 * m_pos + i))
            res = false;
        //DEBUG_LOG("%d pos %d plane_pos[%d] %d", p->id, m_pos, i, plane_pos[i]);
    }
    return res;
}

