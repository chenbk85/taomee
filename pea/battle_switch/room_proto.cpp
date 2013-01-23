#include "proto.hpp"
#include "dispatch.hpp"
#include "common.hpp"
#include "battle.hpp"



//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------
int pack_room_list_info(c_room *p_room, void *param)
{
    btlsw_pvp_room_list_out *p_out = (btlsw_pvp_room_list_out *)param;

    room_list_info_t room_info;

    room_info.map_id = p_room->get_map_id();
    room_info.max_count = p_room->get_seat_count();
    room_info.player_count = p_room->get_player_count();
    room_info.room_id = p_room->get_room_id();
    room_info.room_mode = p_room->get_mode();
    room_info.room_status = p_room->get_status();

    if (p_room->need_passwd()) {
        room_info.encrypted_flag = 1;
    } else {
        room_info.encrypted_flag = 0;
    }
    memcpy(room_info.room_name, p_room->get_name(), sizeof(room_info.room_name));
    p_out->room_list.push_back(room_info);

    return 0;
}








//------------------------------------------------------------------
// implement functions
//------------------------------------------------------------------









//------------------------------------------------------------------
// proto_XXX functions
// DEFAULT_ARG = player_t * p, Cmessage * c_in, Cmessage * c_out, void * param
//------------------------------------------------------------------
/* 进入大厅 */
int btlsw_player_enter_hall(DEFAULT_ARG)
{
	DEBUG_LOG("player enter_hall, u=%u, role_tm=%u, olid=%u",
			p->m_id, p->m_role_tm, p->m_server_id);
	return send_to_player(p);
}

/* 离开大厅 */
int btlsw_player_exit_hall(DEFAULT_ARG)
{
	DEBUG_TLOG("player exit_hall, u=%u, role_tm=%u, olid=%u",
			p->m_id, p->m_role_tm, p->m_server_id);
	send_to_player(p);

	/* TODO(zog): 通知大厅所有人 ??? */

	destroy_player(p);
	return 0;
}

/* 拉取pvp房间列表 */
int btlsw_pvp_room_list(DEFAULT_ARG)
{
	btlsw_pvp_room_list_in *p_in = P_IN;
    btlsw_pvp_room_list_out *p_out = P_OUT;
    p_out->init();

    uint32_t online_id = p_in->online_id;

    c_online *p_online = get_online(online_id);;
    if (!p_online) {
        ERROR_TLOG("nofound online, u=%u, olid=%u", p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
        send_to_player(p);

        return 0;
    }

    p_out->online_id = online_id;


	/* TODO(zog): 拉取太多了 (和策划商量一个拉取方式, 注意: 大多数房间是看不到, 也不会去看的) */
    c_room_map *p_map = p_online->get_wait_map();
    p_map->traverse_room(pack_room_list_info, p_out);

    p_map = p_online->get_match_map();
    p_map->traverse_room(pack_room_list_info, p_out);

    p_map = p_online->get_go_map();
    p_map->traverse_room(pack_room_list_info, p_out);

	return send_to_player(p, p_out);
}

/* 建立pvp房间 */
int btlsw_pvp_create_room(DEFAULT_ARG)
{
    if (p->is_has_room()) {
        ERROR_LOG("failed to pvp_create_room, already in room, u=%u, roomid=%u",
				p->m_id, p->get_room()->get_room_id());
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
        send_to_player(p);
        return 0;
    }

	btlsw_pvp_create_room_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    uint8_t room_mode = p_in->room_mode;
    const char * room_name = p_in->room_name;
    const char *passwd = p_in->password;
    uint32_t map_id = p_in->map_id;
    uint32_t creator_role_tm = p_in->creator_role_tm;

	/* TODO(zog): 密码结尾加 '\0' */

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("user: %u, online %u not found", p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
        send_to_player(p);

        return 0;
    }

    c_pvp_room_pool * p_room_pool = p_online->get_room_pool();
    c_pvp_room *p_room = NULL;
    if (0 != p_room_pool->create_obj(&p_room)) {
        ERROR_TLOG("create_pvp_room obj failed, u=%u, olid=%u, err(%s)", 
                p->m_id, online_id, p_room_pool->get_last_errstr());
        p->m_ret = BTLSW_ERR_SYSTEM_FAULT;
        send_to_player(p);

        return 0;
    }
    p_room->create(p->m_id);
 
    uint32_t room_id = p_room->get_room_id();

    p_room->set_mode(room_mode);
    p_room->set_name(room_name);
    p_room->set_passwd(passwd);
    p_room->set_map(map_id);

    // 放入等待列表
    c_room_map * p_wait = p_online->get_wait_map();
    p_wait->add_room(p_room);

    uint8_t seat = 0;
    p_room->add_player(p, &seat);

    uint32_t right = p->get_right();
    uint32_t team = p->get_team();
    uint8_t status = p->get_status();

	btlsw_pvp_create_room_out * p_out = P_OUT;

    p_out->online_id = online_id;
    p_out->room_info.room_id = room_id;
    memcpy(p_out->room_info.room_name, room_name, MAX_ROOM_NAME_LEN);
    memcpy(p_out->room_info.password, passwd, MAX_ROOM_PWD_LEN);
    p_out->room_info.room_mode = room_mode;
    p_out->room_info.map_id = map_id;
    p_out->host_info.seat_index = seat;
    p_out->host_info.room_right = right;
    p_out->host_info.team = team;
    p_out->host_info.player_status = status;
    p_out->host_info.user_id = p->m_id;
    p_out->host_info.role_tm = creator_role_tm;
    p_out->host_info.server_id = online_id;
    p_out->host_info.seat_status = p_room->get_seat(seat)->get_status();

    DEBUG_TLOG("create_room pvp, u=%u, olid=%u, roomid=%u, mode=%u, pass=[%s]",
			p->m_id, online_id, room_id, room_mode, passwd);

    return send_to_player(p, p_out);
}


/* ---------- CODE FOR btlsw_pvp_destroy_room ---------*/
//

/* 销毁pvp房间 */
int btlsw_pvp_destroy_room(DEFAULT_ARG)
{
	// btlsw_pvp_destroy_room_in * p_in = P_IN;
    // btlsw_pvp_destroy_room_out * p_out = P_OUT;


    // uint32_t online_id = p_in->online_id;
    // uint32_t room_id = p_in->room_id;

    // c_online * p_online = NULL;
    // if (0 != get_online(online_id, &p_online))
    // {
        // return -1;
    // }

    // c_room * p_room = NULL;

    // c_room_map * p_map = p_online->get_wait_map();
    // if (0 == p_map->get_room(room_id, &p_room))
    // {
        // p_map->del_room(p_room);
    // }

    // p_map = p_online->get_match_map();
    // if (0 == p_map->get_room(room_id, &p_room))
    // {
        // p_map->del_room(p_room);
    // }


    // p_map = p_online->get_go_map();
    // if (0 == p_map->get_room(room_id, &p_room))
    // {
        // p_map->del_room(p_room);
    // }

    // c_room_pool * p_room_pool = p_online->get_room_pool();
    // p_room_pool->destroy_room(p_room);

    // p_out->online_id = online_id;
    // p_out->room_id = room_id;


    // send_to_player(p, p_out);

	return 0;
}

/* ---------- CODE FOR btlsw_pvp_set_room_attr ---------*/
//

/* 修改pvp房间属性 */
int btlsw_pvp_set_room_attr(DEFAULT_ARG)
{
    btlsw_pvp_set_room_attr_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_info.room_id;
    // uint8_t room_mode = p_in->room_info.room_mode;
    const char * room_name = p_in->room_info.room_name;
    const char * password = p_in->room_info.password;
    uint32_t map_id = p_in->room_info.map_id;


    if (!p->check_right(ROOM_RIGHT_SET_ATTR)) {
        ERROR_TLOG("no_right set_room_attr, u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_BEYOND_RIGHT;
        send_to_player(p);

        return 0;
    }

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(set_room_attr), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
        send_to_player(p);
        return 0;
    }


    c_room_map * p_map = p_online->get_wait_map();

    c_room * p_room = NULL;
    if (0 != p_map->get_room(room_id, &p_room)) {
        ERROR_TLOG("nofound room(set_room_attr), u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
        send_to_player(p);
        return 0;
    }

    p_room->set_name(room_name);
    // 禁止修改房间模式
    // p_room->set_mode(room_mode);
    p_room->set_passwd(password);
    p_room->set_map(map_id);

    // 应答
    btlsw_pvp_set_room_attr_out * p_out = P_OUT;

    p_out->online_id = online_id;

	p_out->room_info= p_in->room_info;

    // 强调房间模式不修改
    p_out->room_info.room_mode = p_room->get_mode();

    send_to_room(p_room, p, p_out);

	p->clear_waitcmd();
	return 0;
}

/* ---------- CODE FOR btlsw_pvp_set_room_seat ---------*/
//

/* 开关房间座位 */
int btlsw_pvp_set_room_seat(DEFAULT_ARG)
{
    btlsw_pvp_set_room_seat_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_id;
    uint8_t seat = p_in->seat;
    uint8_t status = p_in->seat_status;

    if (!p->check_right(ROOM_RIGHT_SET_ATTR)) {
        ERROR_TLOG("no_right set_room_seat, u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_BEYOND_RIGHT;
        send_to_player(p);

        return 0;

    }

    c_online * p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(pvp_set_room_seat), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);

        return 0;
    }

    c_room_map * p_map = p_online->get_wait_map();
    c_room * p_room = NULL;
    if (0 != p_map->get_room(room_id, &p_room)) {
        ERROR_TLOG("nofound room(pvp_set_room_seat), u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);
        return 0;
    }


    switch (status) {
	case SEAT_STATUS_CLOSE:
		p_room->close_seat(seat);
		break;
	case SEAT_STATUS_BLANK:
		p_room->open_seat(seat);
		break;
	default:
		break;
	}


    // 应答
    btlsw_pvp_set_room_seat_out * p_out = P_OUT;

    p_out->online_id = online_id;
    p_out->room_id = room_id;
    p_out->seat = seat;
    p_out->seat_status = status;

    send_to_room(p_room, p, p_out);

	p->clear_waitcmd();
	return 0;
}

void pack_room_info(c_room * p_room, c_player * p, Cmessage * c_out)
{
    btlsw_pvp_get_room_info_out * p_out = P_OUT;

    p_out->online_id = p_room->get_online_id();

    p_out->room_info.room_id = p_room->get_room_id();
    memcpy(p_out->room_info.room_name, p_room->get_name(), MAX_ROOM_NAME_LEN);
    memcpy(p_out->room_info.password, p_room->get_password(), MAX_ROOM_PWD_LEN);
    p_out->room_info.room_mode = p_room->get_mode();
    p_out->room_info.map_id = p_room->get_map_id();

    p_out->host_seat = p_room->get_host_seat();
    p_out->host_id = p_room->get_host_id();
    p_out->user_seat = p->get_seat();
    p_out->user_id = p->m_id;

    for (uint8_t i = 0; i < p_room->get_max_seat_count(); i++) {

        // 一定要init，否则会有上次残留的数据
        p_out->room_player[i].init();

        room_seat_t * p_seat = p_room->get_seat(i);
        p_out->room_player[i].seat_index = i;
        p_out->room_player[i].seat_status = p_seat->get_status();

        
        if (p_seat->is_taken()) {
            c_player * p_player = p_seat->player;
            p_out->room_player[i].player_status = p_player->get_status();
            p_out->room_player[i].room_right = p_player->get_right();
            p_out->room_player[i].team = p_player->get_team();
            p_out->room_player[i].user_id = p_player->m_id;
            p_out->room_player[i].role_tm = p_player->m_role_tm;
            p_out->room_player[i].server_id = p_player->m_server_id;
        }
    }
}

/* ---------- CODE FOR btlsw_pvp_join_room ---------*/
//

/* 玩家加入pvp房间 */
int btlsw_pvp_join_room(DEFAULT_ARG)
{
	btlsw_pvp_join_room_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_id;
    const char *passwd = p_in->password;

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(join_room), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		return send_to_player(p);
    }

    c_room_map *p_map = p_online->get_wait_map();
    c_room * p_room = NULL;
    if (0 != p_map->get_room(room_id, &p_room)) {
        ERROR_TLOG("nofound room, u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_NO_ROOM_FOUND;
		return send_to_player(p);
    }

    if (p_room->is_full()) {
        WARN_TLOG("full room, u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_ROOM_FULL;
        return send_to_player(p);
    }

	/* TODO(zog): 增加对 password 的 '\0' 的填充
	 * 注意: 所有设置房间密码的交互部分都需要做此检查和保护 */
    if (!p_room->verify_passwd(passwd)) {
        WARN_TLOG("pass-auth failed, u=%u, olid=%u, roomid=%u, pass=[%s]",
				p->m_id, online_id, room_id, passwd);
        p->m_ret = BTLSW_ERR_PASSWORD;
        return send_to_player(p);
    }

	/* TODO(zog): 匹配值到处都是, 但策划说不存在匹配值 ??? */
    //p->set_match_value(match_value);

    uint8_t seat = 0;
    if (0 != p_room->add_player(p, &seat)) {
        ERROR_TLOG("failed c_room::add_player, u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_SYSTEM_FAULT;
		return send_to_player(p);
    }

    uint32_t right = p->get_right();
    uint32_t team = p->get_team();

    // 应答
    btlsw_pvp_get_room_info_out out;
    pack_room_info(p_room, p, &out);
    p->m_waitcmd = btlsw_pvp_get_room_info_cmd;
    send_to_room(p_room, p, &out);

    DEBUG_TLOG("join room, u=%u, olid=%u, roomid=%u, seat=%u, right=0x%x, team=%u",
			p->m_id, online_id, room_id, seat, right, team);

	return send_to_player(p);
}


/* 玩家离开房间 (pvp/pve共用) */
int btlsw_leave_room(DEFAULT_ARG)
{
	btlsw_leave_room_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_id;
    uint8_t seat = p_in->seat;
    uint32_t user_id = p_in->user_id;

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(leave_room), u=%u, olid=%u, roomid=%u, seat=%hhu",
				p->m_id, online_id, room_id, seat);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);

        return 0;
    }

    c_room_map * p_map = NULL;
    c_room * p_room = NULL;
    if (0 != p_online->get_room(room_id, &p_room, &p_map)) {
        ERROR_TLOG("nofound room(pvp_leave_room), u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);
        return 0;
    }

    c_player * p_player = p;
    room_seat_t * p_seat = p_room->get_seat(seat);
    if (p_seat->is_taken()) {
        if (0 != p_room->del_player(seat, &p_player)) {
            ERROR_TLOG("user: %u, online: %u, room: %u, del player %u failed",
					p->m_id, online_id, room_id, user_id);
            p->m_ret = BTLSW_ERR_SYSTEM_FAULT;
            send_to_player(p);
            return 0;
        }
    }

    uint32_t host_id = p_room->get_host_id();
    uint8_t host_seat = p_room->get_host_seat();

    // 应答
    btlsw_leave_room_out * p_out = P_OUT;

    p_out->online_id = online_id;
    p_out->host_seat = host_seat;
    p_out->room_id = room_id;
    p_out->host_id = host_id;
    p_out->seat = seat;
    p_out->user_id = user_id;

    // 已经从房间中删除了该玩家
    // 所以要单独给他发
	// 要用false保持waitcmd，因为send_to_room要waitcmd
    if (p != p_player) {
        btlsw_proto_t header;
        init_proto_head(&header, p);
        send_to_player(p_player, &header, p_out, false);
    } else {
        send_to_player(p, p_out, false);
    }

    send_to_room(p_room, p, p_out);

    p_player->get_online()->del_player(p_player);

    if (p_room->is_empty()) {
        p_online->destroy_room(p_room);
        DEBUG_TLOG("detory_room, u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
    }

    DEBUG_TLOG("leave_room, u=%u, olid=%u, roomid=%u, mode=%u, host_u=%u",
			p->m_id, online_id, room_id, user_id, p_room->get_mode(), host_id);

	p->clear_waitcmd();
	return 0;
}

typedef struct {
    c_room_map * p_map;
    c_room_map * p_peer_map;
    c_room * p_room;
    c_room * p_peer_room;
} match_info_t;

int compute_room_match(c_room * p_room_y, void * param)
{
    c_pvp_room * p_room_x = (c_pvp_room *)param;
    if (p_room_y == p_room_x) {
        return -1;
    }

    int value = 0;

    // 模式和人数必须一致
    if (p_room_x->get_mode() == p_room_y->get_mode() 
     && p_room_x->get_player_count() == p_room_y->get_player_count()) {
        value = p_room_x->diff_match_value(p_room_y->get_match_value());
        return value;
    } else {
        return -1;
    }
}

int match_room(c_online * p_peer_online, void * param)
{
    match_info_t * p_info = (match_info_t *)param;

    if (p_info->p_room->get_online() == p_peer_online) {
        return 0;
    }

    c_room * p_peer_room = NULL;
    c_room_map * p_peer_map = p_peer_online->get_match_map();
    if (0 != p_peer_map->find_room(compute_room_match, p_info->p_room, &p_peer_room)) {
        // 没找到
        // 返回0，表示继续找
        return 0;
    } else {
        // 找到了
        p_info->p_peer_map = p_peer_map;
        p_info->p_peer_room = p_peer_room;

        // 返回-1，表示中断遍历
        return -1;
    }
}

int auto_join_match(c_room * p_room, void * param)
{
    uint32_t match_value = *(uint32_t *)param;
    int value = 0;

    // 有空余位置且没有加密
    if (!p_room->is_full() && !p_room->need_passwd()) {
        value = p_room->diff_match_value(match_value);
        return value;
    } else {
        return -1;
    }
}

/* ---------- CODE FOR btlsw_pvp_auto_join_room ---------*/
//

/* 自动加入pvp房间 */
int btlsw_pvp_auto_join_room(DEFAULT_ARG)
{
    btlsw_pvp_auto_join_room_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    //uint32_t role_tm = p_in->role_tm;

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(auto_join_room), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		return send_to_player(p);
    }

    c_room_map * p_map = p_online->get_wait_map();
    c_room * p_room = NULL;

	/* TODO(zog): 匹配值 !!! */
	uint32_t match_value = 0;
    if (0 != p_map->find_room(auto_join_match, &match_value, &p_room)) {
        // 没找到
        p->m_ret = BTLSW_ERR_NO_FIT_ROOM_FOUND;
		return send_to_player(p);
    }

    uint32_t room_id = p_room->get_room_id();

    p->set_match_value(match_value);

    uint8_t seat = 0;
    if (0 != p_room->add_player(p, &seat)) {
        ERROR_TLOG("failed auto_join_room, u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_SYSTEM_FAULT;
		return send_to_player(p);
    }

    uint32_t right = p->get_right();
    uint32_t team = p->get_team();

    btlsw_pvp_get_room_info_out out;

    pack_room_info(p_room, p, &out);
    p->m_waitcmd = btlsw_pvp_get_room_info_cmd;

    send_to_room(p_room, p, &out);

    DEBUG_TLOG("auto_join_room, u=%u, olid=%u, roomid=%u, "
			"seat=%u, right=0x%x, team=%u",
			p->m_id, online_id, room_id, seat, right, team);

	return 0;
}

/* ---------- CODE FOR btlsw_pvp_set_player_attr ---------*/
//

/* 设置pvp玩家属性 */
int btlsw_pvp_set_player_attr(DEFAULT_ARG)
{
    btlsw_pvp_set_player_attr_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_id;
    uint32_t user_id = p_in->user_id;
    uint8_t seat = p_in->seat;
    uint8_t status = p_in->player_status;
    uint8_t team = p_in->team;

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(pvp_set_player_attr), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);
        return 0;
    }

    c_room_map * p_map = p_online->get_wait_map();
    c_room * p_room = NULL;
    if (0 != p_map->get_room(room_id, &p_room)) {
        ERROR_TLOG("nofound room(set_player_attr), u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);
        return 0;
    }

    if (p->m_id != user_id) {
        ERROR_TLOG("u=%u, set_others_attr, other_u=%u", p->m_id, user_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
        send_to_player(p);
        return 0;
    }

    uint8_t old_seat = p->get_seat();

    if (seat != old_seat) {
        // 调整座位
        p_room->change_seat(old_seat, seat);
    }

    if (0 == p->check_status(status)) {
        // 调整状态
        p->set_status(status);
    }

    if (team != p->get_team()) {
        // 调整队伍
        p_room->change_team(p->get_seat(), team);
    }

    // 应答
    btlsw_pvp_set_player_attr_out * p_out = P_OUT;

    p_out->online_id = online_id;
    p_out->room_id = room_id;
    p_out->user_id = user_id;
    p_out->seat = p->get_seat();
    p_out->player_status = p->get_status();
    p_out->team = p->get_team();

    send_to_room(p_room, p, p_out);

    DEBUG_TLOG("set_player_attr, olid=%u, roomid=%u, set_u=%u, u=%u, seat=%u, status=%u, team=%u",
			online_id, room_id, user_id, p->m_id, p->get_seat(), p->get_status(), p->get_team());

	return 0;
}


/* ---------- CODE FOR btlsw_pvp_grant_player_right ---------*/
//

/* 变更pvp玩家权力 */
int btlsw_pvp_grant_player_right(DEFAULT_ARG)
{
    btlsw_pvp_grant_player_right_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_id;
    uint32_t granter_id = p_in->granter_id;
    uint32_t granted_id = p_in->granted_id;
    uint32_t right = p_in->right;


    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(grant_player_right), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);

        return 0;
    }

    c_room_map * p_map = p_online->get_wait_map();
    c_room * p_room = NULL;
    if (0 != p_map->get_room(room_id, &p_room)) {
        ERROR_TLOG("user: %u, online: %u, room %u not found", p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);

        return 0;
    }

    c_player * p_granter = NULL;
    c_player * p_granted = NULL;
    if (0 != p_room->query_player(granter_id, NULL, &p_granter)
     || 0 != p_room->query_player(granted_id, NULL, &p_granted)) {
        ERROR_TLOG("user: %u, online: %u, room: %u, no player found", p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);

        return 0;
    }

    if (!p_granter->check_right(ROOM_RIGHT_GRANT)) {
        ERROR_TLOG("user: %u, online: %u, room: %u user %u has no right to grant user %u",
				p->m_id, online_id, room_id, granter_id, granted_id);
        p->m_ret = BTLSW_ERR_BEYOND_RIGHT;
        send_to_player(p);

        return 0;
    }

    p_granted->set_right(right);

    btlsw_pvp_grant_player_right_out * p_out = P_OUT;

    p_out->online_id = online_id;
    p_out->room_id = room_id;
    p_out->granter_id = granter_id;
    p_out->granted_id = granted_id;
    p_out->right = p_granted->get_right();

    send_to_room(p_room, p, p_out);

    DEBUG_TLOG("u=%u, olid=%u, roomid=%u, granter=%u, grantee=%u right=0x%x",
			p->m_id, online_id, room_id, granter_id, granted_id, right);

	return 0;
}

/* ---------- CODE FOR btlsw_pvp_kick_player ---------*/
//

/* 踢出pvp玩家 */
int btlsw_pvp_kick_player(DEFAULT_ARG)
{
    btlsw_pvp_kick_player_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_id;
    uint32_t kicker_id = p_in->kicker_id;
    uint32_t kicked_id = p_in->kicked_id;

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(pvp_kick_player), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);
        return 0;
    }

    c_room_map * p_map = p_online->get_wait_map();
    c_room * p_room = NULL;
    if (0 != p_map->get_room(room_id, &p_room)) {
        ERROR_TLOG("user: %u, online: %u, room %u not found", p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);

        return 0;
    }

    c_player * p_kicker = NULL;
    c_player * p_kicked = NULL;
    uint8_t kicked_seat = 0;
    if (0 != p_room->query_player(kicker_id, NULL, &p_kicker)
     || 0 != p_room->query_player(kicked_id, &kicked_seat, &p_kicked)) {
        ERROR_TLOG("user: %u, online: %u, room: %u, no player found", p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);

        return 0;
    }

    if (!p_kicker->check_right(ROOM_RIGHT_KICK)) {
        ERROR_TLOG("user: %u, online: %u, room: %u user %u has no right to kick user %u",
				p->m_id, online_id, room_id, kicker_id, kicked_id);
        p->m_ret = BTLSW_ERR_BEYOND_RIGHT;
        send_to_player(p);

        return 0;
    }

    if (0 != p_room->del_player(kicked_seat)) {
        ERROR_TLOG("user: %u, online: %u, room: %u, del player %u failed",
				p->m_id, online_id, room_id, kicked_id);
        p->m_ret = BTLSW_ERR_SYSTEM_FAULT;
		send_to_player(p);

        return 0;
    }

    uint32_t host_id = p_room->get_host_id();
    uint8_t host_seat = p_room->get_host_seat();

    btlsw_pvp_kick_player_out * p_out = P_OUT;
    p_out->online_id = online_id;
    p_out->room_id = room_id;
    p_out->kicker_id = kicker_id;
    p_out->kicked_id = kicked_id;
    p_out->kicked_seat = kicked_seat;
    p_out->host_seat = host_seat;
    p_out->host_id = host_id;

    // 发给被踢出的玩家
    btlsw_proto_t header;
    init_proto_head(&header, p_kicker);
    send_to_player(p_kicked, &header, p_out, false);
    // 发给房间里剩余的玩家
    send_to_room(p_room, p, p_out);

    p_kicked->get_online()->del_player(p_kicked);

    DEBUG_TLOG("u=%u, olid=%u, roomid=%u, kicker=%u, kickee=%u, host_u: %u",
			p->m_id, online_id, room_id, kicker_id, kicked_id, host_id);
	return 0;
}



/* ---------- CODE FOR btlsw_pvp_start_battle ---------*/
//

/* 由房主发出，通知房间中的玩家，加入相应的战斗服务器，正式开始战斗 */
int btlsw_pvp_start_battle(DEFAULT_ARG)
{
    btlsw_pvp_start_battle_in * p_in = P_IN;
    btlsw_pvp_start_battle_out * p_out = P_OUT;


    uint32_t btl_id = p_in->btl_id;
    uint32_t fd_idx = p_in->fd_idx;
    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_id;

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(pvp_start_battle), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);
        return 0;
    }

    c_room_map * p_map = p_online->get_go_map();
    c_room * p_room = NULL;
    if (0 != p_map->get_room(room_id, &p_room)) {
        ERROR_TLOG("nofound room(pvp_start_battle), u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);

        return 0;
    }

    c_battle * p_battle = p_room->get_battle();
    if (!p_battle) {
		ERROR_TLOG("nofound battle(pvp_start_battle), u=%u", p->m_id);
		p->m_ret = BTLSW_ERR_SYSTEM_FAULT;
		send_to_player(p);
        return 0;
    }

    p_battle->set_battle(btl_id, fd_idx);

    p_out->btl_id = btl_id;
    p_out->fd_idx = fd_idx;

    // 先制造消息头
    btlsw_proto_t header;
    init_proto_head(&header, p);

    // 给房间发
    for (uint8_t i = 0; i < p_battle->m_room_count; i++) {
        c_room * p_guest = p_battle->m_room[i];

        p_out->online_id = p_guest->get_online_id();
        p_out->room_id = p_guest->get_room_id();

        send_to_room(p_guest, &header, p_out);
    }

	p->clear_waitcmd();

    return 0;
}


/* ---------- CODE FOR btlsw_pvp_end_battle ---------*/
//

/* 战斗结束 */
int btlsw_pvp_end_battle(DEFAULT_ARG)
{
    btlsw_pvp_end_battle_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_id;

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(pvp_end_battle), u=%u, roomid=%u, olid=%u",
				p->m_id, room_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);

        return 0;
    }

    c_room_map * p_map = p_online->get_go_map();
    c_room * p_room = NULL;
    if (0 != p_map->get_room(room_id, &p_room)) {
        p->clear_waitcmd();
		// 房间已经不在go_map里面了
        // 属于正常情况
        // 因为room已经被置为wait了
        return 0;
    }

    c_battle * p_battle = p_room->get_battle();
    if (!p_battle) {
		p->clear_waitcmd();
        // 房间已经不在某个battle里面了
        // 属于正常情况
        // 因为battle已经被清理掉了
        return 0;
    }

    for (uint8_t i = 0; i < p_battle->m_room_count; i++) {
        p_room = p_battle->m_room[i];
        if (!p_room) {
            continue;
        }

        p_online = p_room->get_online();
        if (!p_online) {
            continue;
        }

        p_online->set_room_status(p_room, ROOM_STATUS_WAIT);
    }

    del_battle(p_battle);

    p->clear_waitcmd();

	return 0;
}



int room_start_match(c_player * p, uint32_t online_id, uint32_t room_id)
{
    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(start_match), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);
        return 0;
    }

    c_room_map * p_map = p_online->get_wait_map();
    c_room * p_room = NULL;
    if (0 != p_map->get_room(room_id, &p_room)) {
        ERROR_TLOG("nofound room(start_match), u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);
        return 0;
    }


    // 先考察能不能开始
    uint8_t start_flag = 0;
    if (0 != p_room->calc_start_flag(&start_flag)) {
        ERROR_TLOG("user: %u, room: %u, calc start flag", online_id, room_id);
        p->m_ret = BTLSW_ERR_SYSTEM_FAULT;
        send_to_player(p);

        return 0;
    }
    
    if (!start_flag) {
        p->m_ret = BTLSW_ERR_CANT_START_ROOM;
        send_to_player(p);
        return 0;
    }

    // 再考察要不要匹配
    uint8_t match_flag = 0;
    if (0 != p_room->calc_match_flag(&match_flag)) {
        ERROR_TLOG("user: %u, room: %u, calc match flag", online_id, room_id);
        p->m_ret = BTLSW_ERR_SYSTEM_FAULT;
        send_to_player(p);

        return 0;
    }

    DEBUG_TLOG("u=%u, roomid=%u, match_flag=%u", p->m_id, room_id, match_flag);

    if (match_flag) {
        // 需要匹配

        btlsw_pvp_set_room_status_out out;
        out.online_id = online_id;
        out.room_id = room_id;
        out.room_status = ROOM_STATUS_MATCH;

        // 先发房间状态包
		// waitcmd不能清掉，后面要用
        send_to_room(p_room, p, &out, false);


        match_info_t info;
        info.p_map = p_map;
        info.p_peer_map = p_online->get_match_map();
        info.p_room = p_room;
        info.p_peer_room = NULL;

        if (0 != info.p_peer_map->find_room(compute_room_match, p_room, &(info.p_peer_room))) {
            // 在本服没找到匹配的房间

            // 跨服查找
            traverse_online(match_room, &info);

            if (!info.p_peer_room) {
                // 没找到
                // 放入匹配列表
                p_online->set_room_status(p_room, ROOM_STATUS_MATCH);
				p->clear_waitcmd();
                return 0;
            }
        }

        // 以下为找到了
        // 决定谁作为房主，以当前房间作为房主
        c_room *p_host = p_room;

        p_online->set_room_status(p_room, ROOM_STATUS_GO);
        info.p_peer_room->get_online()->set_room_status(info.p_peer_room, ROOM_STATUS_GO);

        c_battle * p_battle = NULL;
        if (0 != new_battle(p_host, &p_battle)) {
			ERROR_TLOG("user: %u, get battle failed", p->m_id);
			p->m_ret = BTLSW_ERR_SYSTEM_FAULT;
			send_to_player(p);
            return 0;
        }
        p_battle->add_room(info.p_peer_room);

        // 再发房间状态报文
        // 先给p_room发
        out.room_status = ROOM_STATUS_GO;

        send_to_room(p_room, p, &out, false);

        // 然后给p_peer_room发
        out.online_id = info.p_peer_room->get_online_id();
        out.room_id = info.p_peer_room->get_room_id();
        out.room_status = ROOM_STATUS_GO;
        send_to_room(info.p_peer_room, p, &out, false);

        uint32_t host_id = p_host->get_host_id();

        // 最后给online发开始战斗的报文
        btlsw_pvp_start_room_out new_out;
        new_out.online_id = online_id;
        new_out.room_id = room_id;
        new_out.host_id = host_id;
        new_out.map_id = p_host->get_map_id();
        new_out.player_count = p_host->get_player_count() + info.p_peer_room->get_player_count();

        send_to_player(p, btlsw_pvp_start_room_cmd, &new_out);
		p->clear_waitcmd();
    } else {
        // 不需要匹配，直接开始
        uint32_t host_id = p_room->get_host_id();
        p_online->set_room_status(p_room, ROOM_STATUS_GO);
        c_battle * p_battle = NULL;
        if (0 != new_battle(p_room, &p_battle)) {
			ERROR_TLOG("user: %u, get battle failed", p->m_id);
			p->m_ret = BTLSW_ERR_SYSTEM_FAULT;
			send_to_player(p);
            return 0;
        }

        // 先发房间状态报文
        btlsw_pvp_set_room_status_out out;
        out.online_id = p_room->get_online_id();
        out.room_id = p_room->get_room_id();
        out.room_status = ROOM_STATUS_GO;
        send_to_room(p_room, p, &out);

        // 再给online发开始战斗的报文
        btlsw_pvp_start_room_out new_out;
        new_out.online_id = online_id;
        new_out.room_id = room_id;
        new_out.host_id = host_id;
        new_out.map_id = p_room->get_map_id();
        new_out.player_count = p_room->get_player_count();

        send_to_player(p, btlsw_pvp_start_room_cmd, &new_out);
    }

    return 0;
}

int room_stop_match(c_player * p, uint32_t online_id, uint32_t room_id)
{

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(stop_match), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);

        return 0;
    }

    c_room_map * p_map = p_online->get_match_map();
    c_room * p_room = NULL;
    if (0 != p_map->get_room(room_id, &p_room)) {
        ERROR_TLOG("nofound room(stop_match), u=%u, olid=%u, roomid=%u",
				p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);
        return 0;
    }

    p_online->set_room_status(p_room, ROOM_STATUS_WAIT);

    btlsw_pvp_set_room_status_out out;
    out.online_id = online_id;
    out.room_id = room_id;
    out.room_status = ROOM_STATUS_WAIT;

    send_to_room(p_room, p, &out);

    return 0;
}


/* ---------- CODE FOR btlsw_pvp_set_room_status ---------*/
//

/* 修改房间状态 */
int btlsw_pvp_set_room_status(DEFAULT_ARG)
{
	btlsw_pvp_set_room_status_in * p_in = P_IN;

    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_id;
    uint8_t room_action = p_in->room_action;


    if (!p->check_right(ROOM_RIGHT_HOST))
    {

        ERROR_TLOG("user: %u, online: %u, room: %u user %u has no right to set room status", p->m_id, online_id, room_id, p->m_id);
        p->m_ret = BTLSW_ERR_BEYOND_RIGHT;
        send_to_player(p);

        return 0;

    }

    if (ROOM_ACTION_START_MATCH == room_action)
    {
        return room_start_match(p, online_id, room_id);
    }
    else if (ROOM_ACTION_STOP_MATCH == room_action)
    {
        return room_stop_match(p, online_id, room_id);
    }


	return 0;
}



/* ---------- CODE FOR btlsw_pvp_get_room_info ---------*/
//

/* 拉取房间内信息 */
int btlsw_pvp_get_room_info(DEFAULT_ARG)
{
	btlsw_pvp_get_room_info_in * p_in = P_IN;
	btlsw_pvp_get_room_info_out * p_out = P_OUT;

    uint32_t online_id = p_in->online_id;
    uint32_t room_id = p_in->room_id;

    c_online *p_online = get_online(online_id);
    if (!p_online) {
        ERROR_TLOG("nofound online(pvp_get_room_info), u=%u, olid=%u",
				p->m_id, online_id);
        p->m_ret = BTLSW_ERR_ONLINE_ID;
		send_to_player(p);

        return 0;
    }

    c_room_map * p_map = NULL;
    c_room * p_room = NULL;
    if (0 != p_online->get_room(room_id, &p_room, &p_map)) {
        ERROR_TLOG("user: %u, online: %u, room %u not found", p->m_id, online_id, room_id);
        p->m_ret = BTLSW_ERR_REQUEST_DATA;
		send_to_player(p);
        return 0;
    }

    pack_room_info(p_room, p, p_out);

    send_to_player(p, p_out);

    return 0;
}
