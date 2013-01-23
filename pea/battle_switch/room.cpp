#include "room.hpp"
#include "online.hpp"


c_room::c_room(c_online *p_online, uint32_t index)
{
    // 计算room_id
    // 高16位是online_id，低16位是下标
    uint32_t online_id = p_online->m_id;
    m_room_id = (online_id<<16) + index;

    m_online = p_online;
    m_battle = NULL;

    // 具体到pvp_room和pve_room时再确定座位数
    m_max_seat_count = 0;
    m_seat_count = m_max_seat_count;

}

int c_room::init()
{
	/* TODO(zog): 把这些放到构造函数里去 */
    set_name(NULL);
    set_passwd(NULL);
    set_status(ROOM_STATUS_NO);
    set_map(0);
    set_mode(0);
    set_match_value(0);

    // 默认第一个是房主
    m_host_seat = 0;

    // 默认不可以开始
    m_start_flag = 0;
    // 默认需要匹配
    m_match_flag = 1;

    // 玩家数为0
    m_player_count = 0;

    for (uint8_t i = 0; i < m_max_seat_count; i++) {
        (m_seat + i)->set_blank();
    }

    return 0;
}

uint32_t c_room::get_online_id()
{
    return m_online->m_id;
}

int c_room::query_player(uint32_t user_id, uint8_t *p_seat, c_player **pp_player)
{
    for (uint8_t i = 0; i < m_max_seat_count; i++) {
        room_seat_t *p_room_seat = get_seat(i);
        if (p_room_seat->is_taken()) {
            if (user_id == p_room_seat->player->m_id) {
                if (p_seat) {
                    *p_seat = i;
                }

                if (pp_player) {
                    *pp_player = p_room_seat->player;
                }
                return 0;
            }
        }
    }

    return -1;
}


void c_room::calc_match_value()
{
    uint32_t value = 0;
    room_seat_t * p_seat = NULL;
    for (uint8_t i = 0; i < m_max_seat_count; i++) {
        p_seat = get_seat(i);
        if (p_seat->is_taken()) {
            if (value < p_seat->player->get_match_value()) {
                value = p_seat->player->get_match_value();
            }
        }
    }

    set_match_value(value);
}

int c_room::find_first_blank(uint8_t * p_seat, uint8_t start_seat, uint8_t end_seat)
{
    room_seat_t * p_room_seat = NULL;
    for (uint8_t i = start_seat; i < m_max_seat_count && i < end_seat; i++) {
        p_room_seat = get_seat(i);
        if (p_room_seat->is_blank()) {
            *p_seat = i;
            return 0;
        }
    }
    return -1;
}


// 玩家变更座位
int c_room::change_seat(uint8_t old_seat, uint8_t new_seat)
{
    if (old_seat == new_seat) {
        return 0;
    }

    room_seat_t * p_old_seat = get_seat(old_seat);
    room_seat_t * p_new_seat = get_seat(new_seat);
    if (p_old_seat->is_taken() && p_new_seat->is_blank()) {
        p_new_seat->status = p_old_seat->status;
        p_new_seat->player = p_old_seat->player;
        p_new_seat->player->set_seat(new_seat);
        p_old_seat->set_blank();
        return 0;
    } else {
        return -1;
    }
}


int c_room::change_team(uint8_t old_seat, uint32_t team)
{
    if (ROOM_MODE_PVP_COMPETE == get_mode()) {
        // 竞技模式不许变更队伍
        return 0;
    }
    room_seat_t * p_seat = get_seat(old_seat);
    if (p_seat->is_taken()) {
        p_seat->player->set_team(team);
        return 0;
    } else {
        return -1;
    }
}

int c_room::add_player(uint8_t seat, c_player *p_player, uint32_t team)
{
	if (0 == query_player(p_player->m_id, NULL, NULL)) {
		ERROR_TLOG("%u is already in room", p_player->m_id);
		return -1;
	}

    room_seat_t * p_seat = get_seat(seat);

    if (!p_seat->is_blank()) {
		ERROR_TLOG("seat is taken, u=%u", p_player->m_id);
        return -1;
    }

    p_seat->set_player(p_player);
    p_player->enter_room(this, seat, team);

    if (0 == m_player_count) {
        // 第一个玩家自动成为房主
        set_host(seat);
    }
    m_player_count++;

    // 调整房间的匹配值
    if (p_player->get_match_value() > get_match_value()) {
        set_match_value(p_player->get_match_value());
    }

    return 0;
}


// 不指定位置，自动安排
int c_room::add_player(c_player *p_player, uint8_t *p_seat)
{
    uint8_t seat = 0;
    if (0 != find_first_blank(&seat)) {
		ERROR_TLOG("c_room::add_player, u=%u", p_player->m_id);
        return -1;
    }

    *p_seat = seat;

    return add_player(seat, p_player, TEAM_ONE);
}

int c_room::kick_player(uint8_t kicker_seat, uint8_t kicked_seat)
{
    room_seat_t *p_seat = get_seat(kicker_seat);
    if (!p_seat->player->check_right(ROOM_RIGHT_KICK)) {
        // 无权
        return -1;
    }

    return del_player(kicked_seat);
}

int c_room::del_player(uint8_t seat, c_player ** pp_player)
{
    room_seat_t * p_seat = get_seat(seat);

    INFO_TLOG("c_room del_player, seat: %u", seat);

    if (!p_seat->is_taken()) {
        return 0;
    }

    p_seat->player->leave_room();

    if (pp_player) {
        *pp_player = p_seat->player;
    }

    TRACE_TLOG("player count: %u", m_player_count);
    m_player_count--;

    p_seat->set_blank();

    if (m_host_seat == seat) {
        // 离开的是房主
        uint8_t host_seat = 0;
        // 按座位排序第一的成为新房主
        if (0 == find_first_player(&host_seat)) {
            set_host(host_seat);
        } else {
            // 房间没人了
            // destroy();
            // INFO_TLOG("destroyed");
            // return 0;
        }
    }

    // 重新计算匹配值
    calc_match_value();

    return 0;
}

int c_room::del_player(uint32_t user_id, c_player ** pp_player)
{
    uint8_t seat = 0;
    if (0 != query_player(user_id, &seat, NULL)) {
        return 0;
    }

    return del_player(seat, pp_player);
}

int c_room::find_first_player(uint8_t * p_seat)
{
    room_seat_t * p_room_seat = NULL;
    for (uint8_t i = 0; i < m_max_seat_count; i++) {
        p_room_seat = get_seat(i);
        if (p_room_seat->is_taken()) {
            *p_seat = i;
            return 0;
        }
    }

    return -1;
}

int c_room::close_seat(uint8_t seat)
{
    room_seat_t * p_seat = get_seat(seat);

    if (p_seat->is_close()) {
        return 0;
    }

    if (p_seat->is_taken()) {
        del_player(seat);
    }

    m_seat_count--;
    p_seat->set_close();

    return 0;
}

int c_room::open_seat(uint8_t seat)
{
    room_seat_t * p_seat = get_seat(seat);

    if (p_seat->is_blank()) {
        return 0;
    }

    if (p_seat->is_taken()) {
        return 0;
    }

    m_seat_count++;
    p_seat->set_blank();

    return 0;
}

bool c_room::check_validity(uint32_t cur_time)
{
    // CHECK_TRUE(0 != m_max_seat_count)
    // CHECK_TRUE(0 != m_seat_count)
    // CHECK_TRUE(NULL != m_seat)
    // CHECK_TRUE(0 != m_room_id)
    // CHECK_TRUE(NULL != m_online)
    // CHECK_TRUE(m_seat_count <= m_max_seat_count)

    return true;
}

int c_room::traverse_player(player_func_t cb, void * param, uint8_t ignore_flag)
{
    room_seat_t * p_seat = NULL;
    for (uint8_t i = 0; i < m_max_seat_count; i++) {
        p_seat = get_seat(i);
        if (p_seat->is_taken()) {
            int ret = cb(p_seat->player, param);
            if (!ignore_flag && ret) {
				return ret;
			}
        }
    }

    return 0;
}

uint8_t c_room::get_team_player_count(uint32_t team)
{
    uint8_t count = 0;
    room_seat_t * p_seat = NULL;
    for (uint8_t i = 0; i < m_max_seat_count; i++) {
        p_seat = get_seat(i);
        if (p_seat->is_taken()) {
            if (p_seat->player->get_team() == team) {
                count++;
            }
        }
    }

    return count;
}

int wrap_send_to_player(c_player *p_player, void *data)
{
	btlsw_proto_out_t *p_data = (btlsw_proto_out_t *)data;
	btlsw_proto_t *p_header = p_data->p_header;
	Cmessage *p_out = p_data->p_out;

	return send_to_player(p_player, p_header, p_out, false);
}

int send_to_room(c_room *p_room, btlsw_proto_t *p_header, Cmessage *p_out)
{
    btlsw_proto_out_t out;
    out.p_header = p_header;
    out.p_out = p_out;

    p_room->traverse_player(wrap_send_to_player, &out);
    return 0;
}

int send_to_room(c_room *p_room, c_player *p, Cmessage *p_out, bool complete)
{
    btlsw_proto_t header;
	init_proto_head(&header, p);
    send_to_room(p_room, &header, p_out);

	if (complete) {
		p->clear_waitcmd();
	}

	return 0;
}
