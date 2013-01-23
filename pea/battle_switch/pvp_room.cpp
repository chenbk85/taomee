/** 
 * ========================================================================
 * @file pvp_room.cpp
 * @brief 
 * @version 1.0
 * @date 2012-03-31
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "pvp_room.hpp"


int c_pvp_room::calc_match_flag(uint8_t *p_flag)
{
    if (ROOM_MODE_PVP_COMPETE == get_mode()) {
        // 竞技模式一定要匹配
        m_match_flag = 1;
        if (p_flag) {
            *p_flag = m_match_flag;
        }
        return 0;
    }

	/* 到此, 一定是自由模式 (ROOM_MODE_PVP_FREE) */
    if (m_player_count * 2 > m_max_seat_count) {
        // 玩家人数过半的不能匹配
        m_match_flag = 0;
        if (p_flag) {
            *p_flag = m_match_flag;
        }
        return 0;
    }

    uint8_t seat = 0;
    if (0 != find_first_player(&seat)) {
        return -1;
    }

    room_seat_t * p_seat = get_seat(seat);
    uint8_t team = p_seat->player->get_team();
    seat++;

    while (seat < m_max_seat_count) {
        p_seat = get_seat(seat);

        if (p_seat->is_taken()) {
            if (team != p_seat->player->get_team()) {
                // 房间中有不同的颜色的不能匹配
                m_match_flag = 0;
                if (p_flag) {
                    *p_flag = m_match_flag;
                }

                return 0;
            }
        }
        seat++;
    }

    // 其他情况均需要匹配
    m_match_flag = 1;
    if (p_flag) {
        *p_flag = m_match_flag;
    }

    return 0;
}

int c_pvp_room::calc_start_flag(uint8_t * p_flag)
{
    for (uint8_t i = 0; i < m_max_seat_count; i++) {
        room_seat_t * p_seat = get_seat(i);
        if (p_seat->is_taken()) {
            if (PLAYER_STATUS_READY != p_seat->player->get_status()) {
                m_start_flag = 0;
                if (p_flag) {
                    *p_flag = m_start_flag;
                }

                return 0;
            }
        }
    }

    m_start_flag = 1;
    if (p_flag) {
        *p_flag = m_start_flag;
    }

    return 0;
}

int c_pvp_room::set_mode(uint8_t mode)
{
    c_room::set_mode(mode);

    if (ROOM_MODE_PVP_FREE == mode) {
        // 将关闭的座位全部打开，已经打开的或者有人的座位不动
        m_max_seat_count = MAX_PVP_ROOM_SEAT_NUM;
        for (uint8_t i = 0; i < MAX_PVP_ROOM_SEAT_NUM; i++) {
            room_seat_t * p_seat = get_seat(i);
            if (p_seat->is_close()) {
                open_seat(i);
            }
        }
    } else if (ROOM_MODE_PVP_COMPETE == mode) {
        // 关闭4个对手位置
        for (uint8_t i = MAX_PVP_ROOM_SEAT_NUM / 2; i < MAX_PVP_ROOM_SEAT_NUM; i++) {
            close_seat(i);
        }

        // 修改max_seat_count一定在close_seat后面
        // 否则get_seat会得到错误的seat
        m_max_seat_count = MAX_PVP_ROOM_SEAT_NUM / 2;
    }

    return 0;
}

int c_pvp_room::add_player(uint8_t seat, c_player * p_player, uint32_t team)
{
    return c_room::add_player(seat, p_player, team);
}


// 不指定位置，自动安排
int c_pvp_room::add_player(c_player * p_player, uint8_t * p_seat)
{
    if (is_full()) {
		ERROR_TLOG("c_pvp_room::add_player: full, u=%u", p_player->m_id);
        return -1;
    }

    if (ROOM_MODE_PVP_COMPETE == get_mode()) {
        return c_room::add_player(p_player, p_seat);
    }

    // 自由模式要平衡两边队伍人数

    uint8_t seat = 0;
    uint8_t seat_1 = -1;
    uint8_t seat_2 = -1;
    uint32_t team = 0;

    // 在队伍1的位置中找一个空位
    find_first_blank(&seat_1, team_start_seat(TEAM_ONE), team_start_seat(TEAM_TWO));

    // 在队伍2的位置中找一个空位
    find_first_blank(&seat_2, team_start_seat(TEAM_TWO));

    if (-1 == seat_1 && -1 != seat_2) {
        // 队伍1满了，只能安排到队伍2
        team = TEAM_TWO;
        seat = seat_2;
    } else if (-1 != seat_1 && -1 == seat_2) {
        // 队伍2满了，只能安排到队伍1
        team = TEAM_TWO;
        seat = seat_1;
    } else if (-1 != seat_1 && -1 != seat_2) {
        // 队伍1和2都有空位
        uint8_t count_one = get_team_player_count(TEAM_ONE);
        uint8_t count_two = get_team_player_count(TEAM_TWO);
        if (count_one > count_two) {
            team = TEAM_TWO;
            seat = seat_2;
        } else {
            team = TEAM_ONE;
            seat = seat_1;
        }
    } else {
        // 不可能
        return -1;
    }

    *p_seat = seat;
    return add_player(seat, p_player, team);
}

int c_pvp_room::change_team(uint8_t old_seat, uint32_t new_team)
{
    if (ROOM_MODE_PVP_COMPETE == get_mode()) {
        // 竞技pvp不允许调整队伍
        return -1;
    }
    
    room_seat_t * p_old_seat = get_seat(old_seat);
    if (!p_old_seat->is_taken()) {
        return -1;
    }

    c_player * p = p_old_seat->player;
    if (new_team == p->get_team()) {
        return 0;
    }

    uint8_t new_seat = 0;
    if (TEAM_ONE == new_team) {
        // 2换到1
        if (0 != find_first_blank(&new_seat)) {
            // 满了
            return -1;
        }

        if (TEAM_TWO == calc_team(new_seat)) {
            // 队伍1满了
            return -1;
        }

        change_seat(old_seat, new_seat);
        p->set_team(new_team);
    } else {
        // 1换到2
        if (0 != find_first_blank(&new_seat, team_start_seat(TEAM_TWO))) {
            // 满了
            return -1;
        }

        change_seat(old_seat, new_seat);
        p->set_team(new_team);
    }

    return 0;
}


// 位置和队伍的对应
uint32_t c_pvp_room::calc_team(uint8_t seat)
{
    seat %= MAX_PVP_ROOM_SEAT_NUM;
    if (seat >= team_start_seat(TEAM_TWO)) {
        return TEAM_TWO;
    } else {
        return TEAM_ONE;
    }
}

uint8_t c_pvp_room::team_start_seat(uint32_t team)
{
    switch (team) {
	case TEAM_ONE:
		return 0;
	case TEAM_TWO:
		return MAX_PVP_ROOM_SEAT_NUM / 2;
	default:
		return -1;
    }
}

bool c_pvp_room::check_validity(uint32_t cur_time)
{
    return c_room::check_validity(cur_time);
}
