#include <cerrno>
#include <cstdlib>
#include <string>
#include <vector>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/inet/byteswap.hpp>
#include <algorithm>
extern "C" 
{
#include <fcntl.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
#include  <libtaomee/project/stat_agent/msglog.h>
#include <libtaomee/project/utilities.h>
#include <assert.h>
}
#include "utils.hpp"
#include "online.hpp"
#include "battle_room.hpp"
#include "singleton.hpp"
#include "trade.hpp"

#include "ap_toplist.hpp"
#include "limit.hpp"
#include "contest.hpp"
#include "team_contest.hpp"
using namespace std;
using namespace taomee;


uint8_t       s_pkg_[pkg_size] = {0};

const uint32_t MAX_PLAYER_INFO_COUNT = (uint32_t)config_get_intval("max_uid", 200000);

online_info_t*      Online::p_online_infos = NULL;
std::map<int, int>  Online::online_fd_maps;
std::map<int, int>  Online::battle_fd_maps;
GHashTable * Online::p_player_infos = NULL;

player_info_t * alloc_new_player(uint32_t id, 
								 uint32_t tm, 
								 uint32_t type, 
								 uint32_t online)
{
	player_info_t * p_info = reinterpret_cast<player_info_t*>(g_slice_alloc0
			(sizeof(player_info_t)));

	p_info->usrid = id;
	p_info->role_tm = tm;
	p_info->online_id = online;
	p_info->room_index = -1;
	p_info->battle_svr_id = 0;
	p_info->trade_svr_id = 0;
	p_info->trade_room_index = -1;
	return p_info;
}

void free_player_info(void * p)
{
	g_slice_free1(sizeof(player_info_t), p);
}


void Online::init()
{
	Online::p_online_infos = new online_info_t[MAX_ONLINE_INFO_COUNT];
	//Online::p_player_infos = new player_info_t[MAX_PLAYER_INFO_COUNT];
	//Online::p_player_infos = new map<uint32_t,  player_info_t*>();
	Online::p_player_infos = g_hash_table_new_full(g_int_hash, g_int_equal, 
			0, free_player_info);
}

void Online::final()
{
	if(Online::p_online_infos != NULL){
		delete [] Online::p_online_infos;			    
		Online::p_online_infos = NULL;
	}
//	if(Online::p_player_infos != NULL){
//		Online::p_player_infos = NULL;
//	}
	g_hash_table_destroy(Online::p_player_infos);
	Online::p_player_infos = NULL;
}

bool Online::is_guest(uint32_t userid)
{
	return userid > 2000000000;
}

int Online::get_online_id_by_fd(int fd)
{
	if (online_fd_maps.find(fd) == online_fd_maps.end()){
		return -1;
	}
	return online_fd_maps[fd];
}

struct online_info_req_t
{
	uint16_t    domain_id;
	uint32_t    online_id;
	uint8_t     online_ip[16];
	uint16_t    online_port;
} __attribute__((packed));	


void set_online_info(online_info_t * info, online_info_req_t *req, fdsession_t * sess)
{
	info->online_id = req->online_id;
	info->domain = req->domain_id;
	memcpy(info->online_ip, req->online_ip, sizeof(info->online_ip));
	info->online_port = req->online_port;
	info->fdsess = sess;
}

int Online::report_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, sizeof(online_info_req_t));
	online_info_req_t* req = reinterpret_cast<online_info_req_t*>(pkg->body);
	if (!is_vaild_online_id(req->online_id)) {
		ERROR_LOG("invalid online id = %u from fd = %d", req->online_id, fdsess->fd);
		return -1;
	}	

	DEBUG_LOG("REPORT ONLINE INFO [ID = %u, IP=%s, PORT=%u]", 
			req->online_id, req->online_ip, req->online_port); 

	if (p_online_infos[req->online_id].online_id != 0) {
		DEBUG_LOG("duplicate online id \t [id = %u]", req->online_id);
		close_client_conn(p_online_infos[req->online_id].fdsess->fd);
	}

	set_online_info(&(p_online_infos[req->online_id]), req, fdsess);

	online_fd_maps[fdsess->fd] = req->online_id;

	g_limit_data_mrg.broad_limit_data(0);
	return 0;
}

int Online::clear_online_info(int fd)
{
	std::map<int, int>::iterator pItr = online_fd_maps.find(fd);
	if(pItr == online_fd_maps.end()){
		return -1;
	}

	int online_id = pItr->second;	
	online_fd_maps.erase(pItr);
	if (is_vaild_online_id(online_id)) {
		p_online_infos[online_id].domain = 0;
		p_online_infos[online_id].online_id = 0;
		memset( p_online_infos[online_id].online_ip, 0, sizeof(p_online_infos[online_id].online_ip));
		p_online_infos[online_id].online_port = 0;	
		p_online_infos[online_id].fdsess = NULL;
		for (std::map<uint32_t, uint32_t>::iterator it = p_online_infos[online_id].player_id_map->begin();
				it != p_online_infos[online_id].player_id_map->end(); ++it) {

			player_info_t * p = Online::get_player_info(it->second);
			if (p) {
				Online::del_player(p);
			}
		}
		p_online_infos[online_id].player_id_map->clear();
	}	
	return 0;
}

void Online::remove_player_from_online_map(uint32_t online_id, uint32_t uid)
{
	p_online_infos[online_id].player_id_map->erase(uid);
}

int Online::report_user_onoff(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct user_onoff_req_t
	{
		uint8_t login;
		uint32_t role_tm;
		uint32_t role_type;
		char     nick[16];
		uint32_t achieve_point;
		uint32_t last_up_tm;
	} __attribute__((packed));

	CHECK_VAL(bodylen,  sizeof(user_onoff_req_t));
	int online_id = get_online_id_by_fd(fdsess->fd);
	if (!is_vaild_online_id(online_id)) {
		ERROR_LOG("REPORT USER ON OFF WITH INVAILD ONLINE [ID %u]", online_id);
		return 0;
	}

	user_onoff_req_t *req = reinterpret_cast<user_onoff_req_t*>(pkg->body);
	if (is_guest(pkg->sender_id)){
		return 0;
	}

	player_info_t * p = Online::get_player_info(pkg->sender_id);
	if (p) {
		DEBUG_LOG("del player [%u]", pkg->sender_id);
		Online::del_player(p);
		Online::remove_player_from_online_map(p->online_id, p->usrid);
	}
		
	if (req->login)
	{
		DEBUG_LOG("player [%u] login", pkg->sender_id); 
		player_info_t * info = add_player(pkg->sender_id, req->role_tm, req->role_type, online_id);	
		info->fdsess = fdsess;
		info->seq = pkg->seq;

		//重新上线的比赛玩家
		uint64_t session = get_user_old_session(info->usrid, info->role_tm);
		if (session) {
			ContestGroup * group = find_contestgroup_by_session(session); 
			if (group) {
				user_contest_info_t * old_user = group->get_user_contest_info_by_id(info->usrid);
				old_user->out_flag = 0; //将玩家离线状态改为在线
				info->session_id = session;
				int idx = sizeof(svr_proto_t);//将session传回online
				pack_h(s_pkg_, group->sessionId, idx);
				init_proto_head(s_pkg_, pkg->sender_id, idx, proto_auto_join_contest, 0, 0);
				send_pkg_to_client(fdsess, s_pkg_, idx);
			} 
			clear_user_old_session(info->usrid);
		}
	}
	else
	{
		if (req->achieve_point > 0)
		{
			ap_data data;
			data.userid_ = pkg->sender_id;
			data.role_regtime_ = req->role_tm;
			data.ap_point_  = req->achieve_point;
			data.get_time_  = req->last_up_tm;
			memcpy( data.nick_, req->nick, 16);
			ap_list2.add_ap_data(&data);
		}
		DEBUG_LOG("player [%u] logout", pkg->sender_id);
	}
	return 0;
}

player_info_t * Online::add_player( uint32_t user_id, uint32_t role_tm, uint32_t role_type, uint32_t online_id)
{
	player_info_t * p_info = alloc_new_player(user_id, role_tm, role_type, online_id);
	g_hash_table_insert(Online::p_player_infos, &(p_info->usrid), p_info);
	p_online_infos[p_info->online_id].player_id_map->insert(std::map<uint32_t, uint32_t>::value_type(p_info->usrid, p_info->usrid)); 
	return p_info;
}

int  Online::del_player(player_info_t * p_info)
{
//	p_online_infos[p_info->online_id].player_id_map->erase(p_info->usrid);	

	if (p_info->trade_svr_id != 0 && p_info->trade_room_index != -1) {
		player_leave_trade_room(p_info->usrid, p_info->trade_svr_id, p_info->trade_room_index);
	}

	if (p_info->session_id) {
		//处理那些还在16强比赛中的玩家状态
		ContestGroup * group = find_contestgroup_by_session(p_info->session_id);
		if (group) {
			group->Leave(p_info->usrid);
			if (!group->IsWaiting()) { //已经开始的玩家保存session
				save_player_session(p_info->usrid, p_info->role_tm, p_info->session_id);
			} 
			//Ended状态的比赛组由timer 删除
			if (group->need_del()) {
				delete group;
			}
		}
		p_info->session_id = 0;
	}

	g_hash_table_remove(Online::p_player_infos, &(p_info->usrid));
	return 0;
}

player_info_t* Online::get_player_info(uint32_t user_id)
{
	return reinterpret_cast<player_info_t*>(g_hash_table_lookup(p_player_infos, &user_id));	
}


bool Online::check_player_online(uint32_t user_id)
{
	return true;
}


int  Online::room_list(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	return 0;
}

int  Online::auto_join_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct auto_join_room_req_t
	{
		int32_t stage_id;
		int32_t diffcult;
	}__attribute__((packed));

	CHECK_VAL(bodylen,  sizeof(auto_join_room_req_t));

	auto_join_room_req_t * req = reinterpret_cast<auto_join_room_req_t*>(pkg->body);

	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 
	DEBUG_LOG("USER [%u] AUTO JOIN PVE ROOM [%u %u]", pkg->sender_id, req->stage_id, req->diffcult);
	
	BattleRoom * room = s_mgr.FindPVERoom(req->stage_id, req->diffcult); 

	int idx = sizeof(svr_proto_t);
	if (room) {
		pack_h(s_pkg_, room->BattleId, idx);
		pack_h(s_pkg_, room->RoomId, idx);
		pack_h(s_pkg_, room->StageId, idx);
		pack_h(s_pkg_, room->Diffcult, idx);
		DEBUG_LOG("USER [%u] FIND PVE ROOM [%u %u]", pkg->sender_id, room->BattleId, room->RoomId);
	} else {
		pack_h(s_pkg_, -1, idx);
		pack_h(s_pkg_, -1, idx);
		pack_h(s_pkg_, req->stage_id, idx);
		pack_h(s_pkg_, req->diffcult, idx);
		DEBUG_LOG("USER [%u] FIND PVE ROOM [%d %d]",pkg->sender_id,  -1, -1);
	}

	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);

}

//
//int send_pkg_to_player(player_info_t * player, void * buf, int length)
//{
//	return send_pkg_to_client(player->fdsess, buf, length);
//}


int Online::auto_join_pvp_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct auto_join_pvp_room_req_t
	{
		uint32_t userid;
		uint32_t role_tm;
		uint32_t role_type;
		char     nick[16];
		int32_t join_level;
		int32_t pvp_mode;
	}__attribute__((packed));

	CHECK_VAL(bodylen, sizeof(auto_join_pvp_room_req_t));

	auto_join_pvp_room_req_t * req = reinterpret_cast<auto_join_pvp_room_req_t*>(pkg->body);

	if (req->pvp_mode == pvp_16_contest) {
		player_info_t * player_info = Online::get_player_info(pkg->sender_id);
		if (player_info && player_info->session_id) {
			ContestGroup * group = find_contestgroup_by_session(player_info->session_id);
			if (group->IsGoing()) {
				group->player_contest_btl(player_info->usrid);
			}
		}
		return 0;
	}

	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 

	DEBUG_LOG("AUTO [%u] JOIN PVP ROOM [%u %u]", pkg->sender_id, req->join_level, req->pvp_mode);

	BattleRoom * room = NULL;

	if (req->pvp_mode >= pvp_red_blue_1 && req->pvp_mode <= pvp_red_blue_3) {
		room = s_mgr.FindRedBlueRoom(req->join_level, req->pvp_mode);
	}else {
		room = s_mgr.FindPVPRoom(req->join_level, req->pvp_mode); 
	}

	int idx = sizeof(svr_proto_t);
	if (room) {
		room->SetSecondUser(req->userid, req->role_tm, req->role_type, req->nick);
		pack_h(s_pkg_, room->BattleId, idx);
		pack_h(s_pkg_, room->RoomId, idx);
		pack_h(s_pkg_, room->PVPMode, idx);
		DEBUG_LOG("USER [%u] FIND PVP ROOM [%u %u]", pkg->sender_id, room->BattleId, room->RoomId);
	} else {
		pack_h(s_pkg_, -1, idx);
		pack_h(s_pkg_, -1, idx);
		pack_h(s_pkg_, req->pvp_mode, idx);
		DEBUG_LOG("USER [%u] FIND PVP ROOM [%d %d]", pkg->sender_id, -1, -1);
	}

	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}


/////////////////////////////////for battle server////////////////////////////////
int Online::report_battle_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct battle_info_req_t
	{
		uint32_t    battle_svr_id;
	} __attribute__((packed));
	CHECK_VAL(bodylen, sizeof(battle_info_req_t));
	battle_info_req_t* req = reinterpret_cast<battle_info_req_t*>(pkg->body);	
	//add_room_list_to_group(req->battle_svr_id);
	battle_fd_maps[fdsess->fd] = req->battle_svr_id;
	return 0;
}


int Online::clear_battle_info(int fd)
{
	std::map<int, int>::iterator pItr = battle_fd_maps.find(fd);
	if(pItr == battle_fd_maps.end()){
		return -1;
	}
	
	int battle_svr_id = pItr->second;
	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 
	DEBUG_LOG("BTL SVR %u CRASH CLEAR ROOM INFO", battle_svr_id);
	s_mgr.ClearBattleRoom(battle_svr_id);

	battle_fd_maps.erase(pItr);
	return 0;
}



int Online::report_create_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct create_room_req_t
	{
		uint32_t    battle_svr_id;
		uint32_t    room_index;
		int32_t     stage_id;
		int32_t     diffcult;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(create_room_req_t));
	create_room_req_t* req = reinterpret_cast<create_room_req_t*>(pkg->body);

	DEBUG_LOG("CREATE PVE ROOM [%u %u %u %u]", req->battle_svr_id, req->room_index, req->stage_id, req->diffcult);

	BattleRoom * room = CreatePVERoom(req->battle_svr_id, req->room_index, 
		  							req->stage_id, req->diffcult);

	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 

	s_mgr.AddPVERoom(room);

	return 0;
}

int Online::report_destroy_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct destroy_room_req_t
	{
		uint32_t    battle_svr_id;
		uint32_t    room_index;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(destroy_room_req_t));

	destroy_room_req_t * req = reinterpret_cast<destroy_room_req_t*>(pkg->body);

	DEBUG_LOG("DESTORY PVE ROOM [%u %u]", req->battle_svr_id, req->room_index);

	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 

	s_mgr.DestroyPVERoom(req->battle_svr_id, req->room_index);

	return 0;
}

int Online::report_join_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct join_room_t {
		uint32_t battle_id;
		uint32_t room_id;
	}__attribute__((packed));

	CHECK_VAL(bodylen, sizeof(join_room_t));

	join_room_t * req = reinterpret_cast<join_room_t*>(pkg->body);

	DEBUG_LOG("SECOND USER JOIN PVE ROOM [%u %u]", req->battle_id, req->room_id);

	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 
	s_mgr.JoinPVERoom(req->battle_id, req->room_id);
	return 0;
}

int Online::report_leave_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	return 0;
}

int Online::report_room_hot_join(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	return 0;
}

int Online::clear_all_rooms(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct clear_all_rooms_req_t
	{
		int32_t battle_svr_id;
	}__attribute__((packed));
	CHECK_VAL(bodylen, sizeof(clear_all_rooms_req_t));
	clear_all_rooms_req_t * req = reinterpret_cast<clear_all_rooms_req_t*>(pkg->body);

	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 
	DEBUG_LOG("CLEAR ALL SVR %u ROOM", req->battle_svr_id);
	s_mgr.ClearBattleRoom(req->battle_svr_id);

	return 0;	 
}

int Online::report_team_room_start(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	return 0;
	
}

int Online::report_create_pvp_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct create_pvp_room_req_t
	{
		uint32_t    userid;
		uint32_t    user_tm;
		uint32_t    role_type;
		char        nick[16];
		uint32_t    battle_svr_id;
		uint32_t    room_index;
		int32_t     pvp_mode;
		uint32_t    creator_lv;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(create_pvp_room_req_t));

	
	create_pvp_room_req_t* req = reinterpret_cast<create_pvp_room_req_t*>(pkg->body);

	DEBUG_LOG("CREATE PVP ROOM [%u %u %u %u]", req->battle_svr_id, req->room_index, req->pvp_mode, req->creator_lv);

	if (req->pvp_mode == pvp_16_contest) {
		player_info_t * player_info = Online::get_player_info(pkg->sender_id);
		if (player_info && player_info->session_id) {
			ContestGroup * group = find_contestgroup_by_session(player_info->session_id);
			user_contest_info_t * user =  group->get_user_contest_info_by_id(pkg->sender_id);

			assert(user->cur_contest != NULL && user->cur_contest->first_player != NULL);

			if (user->cur_contest->first_player->user_id == pkg->sender_id 
					&& user->cur_contest->status == STATUS_WAITING_CREATE) {
				player_create_contest_room(user->cur_contest, req->battle_svr_id, req->room_index);
			}
		}
		return 0;
	}

	BattleRoom * room = CreatePVPRoom(req->battle_svr_id, req->room_index, 
			req->creator_lv, req->pvp_mode);

	room->SetFirstUser(req->userid, req->user_tm, req->role_type, req->nick);

	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 

	s_mgr.AddPVPRoom(room);

	return 0;
}


int Online::report_destroy_pvp_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{

	struct destroy_pvp_room_req_t
	{
		uint32_t    battle_svr_id;
		uint32_t    room_index;					    
//		uint32_t    winner;
	} __attribute__((packed));	
	CHECK_VAL(bodylen, sizeof(destroy_pvp_room_req_t));
	destroy_pvp_room_req_t* req = reinterpret_cast<destroy_pvp_room_req_t*>(pkg->body);

	DEBUG_LOG("DESTORY PVP ROOM [%u %u]", req->battle_svr_id, req->room_index);

	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 

	s_mgr.DestroyPVPRoom(req->battle_svr_id, req->room_index);
	
	return 0;
}


int Online::report_join_pvp_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct join_room_t {
		int32_t battle_id;
		int32_t room_id;
		uint32_t pvp_lv;
	}__attribute__((packed));

	CHECK_VAL(bodylen, sizeof(join_room_t));

	join_room_t * req = reinterpret_cast<join_room_t*>(pkg->body);
	if (req->pvp_lv == pvp_16_contest) {
		ERROR_LOG("REPROT PLAYRE ENTER ROOM CONTEST %u", pkg->sender_id);
		player_info_t * player_info = Online::get_player_info(pkg->sender_id);
		if (player_info && player_info->session_id) {
			ContestGroup * group = find_contestgroup_by_session(player_info->session_id);
			user_contest_info_t * user =  group->get_user_contest_info_by_id(pkg->sender_id);
			assert(group != NULL && user != NULL);
			if (user->cur_contest->status == STATUS_WAITING_ENTER 
					&& user->cur_contest->battle_svr_id == req->battle_id
					&& user->cur_contest->room_index == req->room_id) {
				player_enter_contest_room(user->cur_contest, req->battle_id, req->room_id);
			}
		}
		return 0;
	}

	DEBUG_LOG("SECOND USER JOIN PVP ROOM [%u %u]", req->battle_id, req->room_id);

	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 
	s_mgr.JoinPVPRoom(req->battle_id, req->room_id);
	return 0;

}

int Online::report_leave_pvp_room(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	return 0;
	
}


int Online::report_pvp_room_start(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct pvp_room_start_t {
		uint32_t battle_svr;
		uint32_t room_id;
	}__attribute__((packed));

	CHECK_VAL(bodylen, sizeof(pvp_room_start_t));

	pvp_room_start_t * req = reinterpret_cast<pvp_room_start_t*>(pkg->body);
	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 
	s_mgr.StartPVPRoom(req->battle_svr, req->room_id);

	return 0;	
}


int Online::get_ap_toplist(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	return 0;
}

int Online::get_contest_pvp_room_info(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	struct get_pvp_room_t {
		uint32_t pvp_mode;
	}__attribute__((packed));

	CHECK_VAL(bodylen, sizeof(get_pvp_room_t));

	get_pvp_room_t * req = reinterpret_cast<get_pvp_room_t*>(pkg->body);

	int idx = sizeof(svr_proto_t);
	RoomManager & s_mgr = singleton_default<RoomManager>::instance(); 
	s_mgr.PackGoingPVPRoomInfo(req->pvp_mode, s_pkg_, idx);

	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

int Online::consume_item(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	struct consume_item_t {
		uint32_t add_del_type;//0: reduce,  1:add
		uint32_t reset_type;
		uint32_t item_id;
		uint32_t item_cnt;
	}__attribute__((packed));
	CHECK_VAL(bodylen, sizeof(consume_item_t));

	consume_item_t * req = reinterpret_cast<consume_item_t*>(pkg->body);

	if (req->add_del_type) {
		g_limit_data_mrg.add_item_cnt(req->item_id, req->item_cnt, req->reset_type);
	} else {
		g_limit_data_mrg.reduce_item_cnt(req->item_id, req->item_cnt);
	}
	return 0;
}

int Online::reset_global_data(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	struct reset_global_data_t {
		uint32_t add_del_type;
		uint32_t reset_type;
	}__attribute__((packed));
	CHECK_VAL(bodylen, sizeof(reset_global_data_t));

	reset_global_data_t * req = reinterpret_cast<reset_global_data_t*>(pkg->body);
    DEBUG_LOG("realtime reset global data [%u %u]", req->add_del_type, req->reset_type);
    g_limit_data_mrg.reset_limit_data(reinterpret_cast<void*>(req->reset_type));
	return 0;
}

int Online::get_get_version(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	int idx = sizeof(svr_proto_t);
	char tmpbuf[250] = "20110810";
	pack(s_pkg_, tmpbuf, sizeof(tmpbuf), idx);

	TRACE_LOG("%s", tmpbuf);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

int Online::auto_join_16_hero_contest(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	CHECK_VAL(bodylen, sizeof(user_contest_req_t));
	user_contest_req_t * req = reinterpret_cast<user_contest_req_t*>(pkg->body);

	player_info_t * player_info = Online::get_player_info(pkg->sender_id);
	if (!player_info) {
		//可能btlsw挂掉重启后，无user的online, 与fd信息
		player_info = Online::add_player(req->user_id,
				req->role_tm,
				req->role_type,
				get_online_id_by_fd(fdsess->fd));
		player_info->fdsess = fdsess;
	}	

	user_contest_info_t * user_info = alloc_user_contest_info(req);
	ContestGroup * group = auto_join_group(user_info);
	assert(group != NULL);

	player_info->session_id = group->sessionId;

	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, group->sessionId, idx);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	send_pkg_to_client(fdsess, s_pkg_, idx);

	if (group->IsFull()) {
		group->SetStart();
	}

	return 0;
}

int Online::leave_16_hero_contest(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	player_info_t * player_info = Online::get_player_info(pkg->sender_id);
	if (player_info && player_info->session_id) {
		ContestGroup * group = find_contestgroup_by_session(player_info->session_id);
		assert(group != NULL);
		group->Leave(player_info->usrid); 
		//主动退出的玩家清理session;
		//clear_user_old_session(player_info->usrid);
		//已经结束的战斗组只能由timer中删除
		if (group->need_del( )) {
			delete group;
		}
		player_info->session_id = 0;
	}
	int idx = sizeof(svr_proto_t);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0,  pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

int Online::list_16_hero_contest_info(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	player_info_t * player_info = Online::get_player_info(pkg->sender_id);

	if (player_info && player_info->session_id) {
		ContestGroup * group = find_contestgroup_by_session(player_info->session_id);
//		user_contest_info_t * user = group->get_user_contest_info_by_id(player_info->usrid);
		assert(group != NULL);
		//		if (group) {
		int idx = sizeof(svr_proto_t);
		group->pack_group_contest_info(s_pkg_, idx);
		init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0,  pkg->seq);
		return send_pkg_to_client(fdsess, s_pkg_, idx);
		//		}
	}
	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, 0, idx);	
	pack_h(s_pkg_, 0, idx);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0,  pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

int Online::pvp_btl_over(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	struct pvp_over_t {
		int32_t battle_svr;
		int32_t room_id;
		uint32_t pvp_lv;
		uint32_t winner_id;
		uint32_t extra_info;
	}__attribute__((packed));
	CHECK_VAL(bodylen, sizeof(pvp_over_t));
	pvp_over_t * req = reinterpret_cast<pvp_over_t*>(pkg->body);
	if (req->pvp_lv == pvp_16_contest) {
//		ERROR_LOG("REPROT PLAYRE ENTER ROOM CONTEST %u", pkg->sender_id);
		player_info_t * player_info = Online::get_player_info(pkg->sender_id);
		if (player_info && player_info->session_id) {
			ContestGroup * group = find_contestgroup_by_session(player_info->session_id);
			user_contest_info_t * user =  group->get_user_contest_info_by_id(pkg->sender_id);
			if (group && user && user->cur_contest) {
				DEBUG_LOG("CONTEST %u  OVER WInner %u MONSTER KILLER %u", 
						user->cur_contest->index, req->winner_id, req->extra_info); 
				if (user->cur_contest->status == STATUS_GOING  
						&& user->cur_contest->battle_svr_id == req->battle_svr
						&& user->cur_contest->room_index == req->room_id) {
					user->cur_contest->monster_killer_ = req->extra_info;
					player_win_contest(user->cur_contest, user);
				}
			} else {
				ERROR_LOG("CONTEST ERROR USER %u %u", pkg->sender_id);
			}
		}
	}
	return 0;
}

int Online::list_16_hero_passed_contest(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	player_info_t * player_info = Online::get_player_info(pkg->sender_id);
	if (player_info && player_info->session_id) {
		ContestGroup * group = find_contestgroup_by_session(player_info->session_id);
		assert(group != NULL);
		user_contest_info_t * user =  group->get_user_contest_info_by_id(pkg->sender_id);
		assert(user != NULL);
		int idx = sizeof(svr_proto_t);
		pack_user_passed_contest(user, s_pkg_, idx);
		init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0,  pkg->seq);
		return send_pkg_to_client(fdsess, s_pkg_, idx);
	} else { 
		int idx = sizeof(svr_proto_t);
		pack(s_pkg_, 0, idx);
		init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
		return send_pkg_to_client(fdsess, s_pkg_, idx);
	}
}

int Online::list_all_contest_player(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)  
{
	player_info_t * player_info = Online::get_player_info(pkg->sender_id);
	if (player_info && player_info->session_id) {
		if (player_info && player_info->session_id) {
			ContestGroup * group = find_contestgroup_by_session(player_info->session_id);
			assert(group != NULL);
			int idx = sizeof(svr_proto_t);
			uint32_t size = group->get_user_size();
			pack_h(s_pkg_, size, idx);
			user_contest_info_t * user = group->get_user_contest_info_by_id(pkg->sender_id);
			if (user && user->guess_champion) {
				pack_h(s_pkg_, user->guess_champion->user_id, idx);
			} else {
				pack_h(s_pkg_, 0, idx);
			}
			group->pack_all_user_info(s_pkg_, idx);
			init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0,  pkg->seq);
			return send_pkg_to_client(fdsess, s_pkg_, idx);
		}
	}
	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, 0, idx);
	pack_h(s_pkg_, 0, idx);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

int Online::guess_contest_champion(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	struct guess_champion_t {
		uint32_t guess_id;
		uint32_t guess_tm;
	}__attribute__((packed));
	CHECK_VAL(bodylen, sizeof(guess_champion_t));
	guess_champion_t * req = reinterpret_cast<guess_champion_t*>(pkg->body);
	player_info_t * player_info = Online::get_player_info(pkg->sender_id);
	uint32_t err = 20013;
	if (player_info && player_info->session_id) {
		if (player_info && player_info->session_id) {
			ContestGroup * group = find_contestgroup_by_session(player_info->session_id);
			assert(group != NULL);
			user_contest_info_t * user =  group->get_user_contest_info_by_id(pkg->sender_id);
			user_contest_info_t * guess_champion = group->get_user_contest_info_by_id(req->guess_id);
//			uint32_t reduce_val = 0;
			if (group->CanGuessChampion()) {
				if (guess_champion && user->guess_champion == NULL) {
//					reduce_val = 200;
					err = 0;
					user->guess_champion = guess_champion;
//					user->player_val_ -= reduce_val;
				} 
			} else {
				err = 20015;
			}

			assert(user != NULL);
			int idx = sizeof(svr_proto_t);
			pack_h(s_pkg_, err, idx);
			pack_h(s_pkg_, req->guess_id, idx);
			pack_h(s_pkg_, req->guess_tm, idx);
//			pack_h(s_pkg_, reduce_val, idx);
			init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
			return send_pkg_to_client(fdsess, s_pkg_, idx);
		}
	}
	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, err, idx);
	pack_h(s_pkg_, req->guess_id, idx);
	pack_h(s_pkg_, req->guess_tm, idx);
//	pack_h(s_pkg_, 0, idx);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}

int Online::team_contest_auto_join(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{
	CHECK_VAL(bodylen, 4);
	int idx = 0;
	uint32_t team_id = 0;
	unpack_h(pkg->body, team_id, idx);
	uint32_t server_id =  auto_find_team_server_id(team_id);
	DEBUG_LOG("Team %u AUTO TEAM CONTEST SERVRE %u", team_id, server_id);
	idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, server_id, idx);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);

}

int Online::team_contest_get_server_id(svr_proto_t * pkg, uint32_t bodylen, fdsession_t * fdsess)
{

	CHECK_VAL(bodylen, 4);
	int idx = 0;
	uint32_t team_id = 0;
	unpack_h(pkg->body, team_id, idx);
	uint32_t server_id = get_team_server_id(team_id);
	DEBUG_LOG("Team %u  TEAM CONTEST SERVRE IS >>> %u", team_id, server_id);
	idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, server_id, idx);
	init_proto_head(s_pkg_, pkg->sender_id, idx, pkg->cmd, 0, pkg->seq);
	return send_pkg_to_client(fdsess, s_pkg_, idx);
}


