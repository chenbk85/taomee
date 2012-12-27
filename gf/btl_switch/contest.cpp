/*
 * =====================================================================================
 *
 *       Filename:  contest.cpp
 *
 *     Description:  
 *
 *        Version:  1.0
 *        Created:  02/10/2012 02:26:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#include "contest.hpp"
#include <map>
extern "C" {
#include <glib.h>
}
#include <libtaomee++/inet/pdumanip.hpp>
using namespace taomee;

typedef std::list<ContestGroup*> GroupList;
typedef std::map<uint64_t, ContestGroup*> GroupMap;

static GroupList * waiting_groups = NULL;
static GroupMap  * going_groups = NULL;

static GroupList * ended_groups = NULL;
//static GroupList * ending_groups = NULL


struct session_t {
	session_t(uint32_t user, uint32_t tm, uint64_t sess) 
		: uid(user), role_tm(tm), session(sess) 
	{ }
	uint32_t uid;
	uint32_t role_tm;
	uint64_t session;
};

static std::map<uint32_t, session_t*>* all_old_session = NULL;



void player_try_contest_btl(contest_t * conest, user_contest_info_t * user);

int send_header_to_player(player_info_t * player,
	   					  uint32_t cmd, 
						  uint32_t errno,
						  uint32_t seqno);

int send_contest_room_info(player_info_t * p, 
						   int svr_id, 
						   int room_id, 
						   uint32_t pvp_lv);



void init_contest_groups()
{
	waiting_groups = new GroupList;
	going_groups = new GroupMap;
	ended_groups = new GroupList;
	all_old_session = new std::map<uint32_t, session_t*>;
}

void fini_contest_groups()
{

}
user_contest_info_t * alloc_user_contest_info(const user_contest_req_t * req)
{
	user_contest_info_t * info = new user_contest_info_t();
	info->user_id = req->user_id;
	info->role_tm = req->role_tm;
	info->role_type = req->role_type;
	memcpy(info->nick_name, req->nick_name,  MAX_NICK_SIZE);
	info->user_lv = req->user_lv;
	info->out_flag = 0;
	info->init_index = 0;
	info->cur_contest = NULL;
	info->group = NULL;
	info->guess_champion = NULL;
	info->player_val_ = req->expliot_val;
	return info;
}

void del_user_contest_info(user_contest_info_t * info) {
	delete info;
}

contest_t * alloc_new_contest()
{
	contest_t * contest = reinterpret_cast<contest_t*>(g_slice_alloc0(sizeof(contest_t)));

	INIT_LIST_HEAD(&(contest->timer_list));
	contest->next_contest = NULL;
	contest->pre_contest = NULL;
	contest->first_player = NULL;
	contest->second_player = NULL;
	contest->winner = 0;
	contest->battle_svr_id = -1;
	contest->room_index = -1;
  	contest-> win_val_ = 0;
  	contest->lose_val_ = 0;
    contest->monster_val_ = 0;
    contest->spurprise_val_ = 0;
    contest->monster_killer_ = 0;
	return contest;
}

void del_contest(contest_t * contest_info)
{
	REMOVE_TIMERS(contest_info);
	g_slice_free1(sizeof(contest_t), contest_info);
}


ContestGroup * CreateGroup(uint32_t uid, uint32_t role_tm)
{
	ContestGroup * group = new ContestGroup();
	group->InitsessionID(uid, role_tm);
	return group;
}

void ContestGroup::InitContests()
{
	assert(allUsers_.size() == USER_SIZE);
	for (uint32_t i = 0; i < USER_SIZE - 1; i++) {
		allContests_[i]->index = i;
	}

	//first cir contest
	std::list<user_contest_info_t*>::iterator it = allUsers_.begin();
	int contest_idx = USER_SIZE - 2;
	while (it != allUsers_.end()) {
		user_contest_info_t * first = *it;
		it++;
		if (it == allUsers_.end()) {
			return;
		}
		user_contest_info_t * second = *it;
		it++;
		contest_t * contest = allContests_[contest_idx];
		assert(contest);
		assert((contest_idx <= USER_SIZE - 2) && (contest_idx >= 0));
		contest->index = contest_idx;
		first->init_index = contest_idx;
		second->init_index = contest_idx;
		set_player_ready(contest, first);
		set_player_ready(contest, second);
		contest_idx--;
	}
}

user_contest_info_t * ContestGroup::get_user_contest_info_by_id(uint32_t uid)
{
	for (std::list<user_contest_info_t*>::iterator it = allUsers_.begin();
			it != allUsers_.end(); ++it) {
		if ((*it)->user_id == uid) {
			return *it;
		}
	}
	return NULL;
}

void ContestGroup::player_contest_btl(uint32_t uid)
{
	user_contest_info_t * user = get_user_contest_info_by_id(uid);
	assert(user != NULL && user->cur_contest);	

	DEBUG_LOG("USER %u TRY CONTEST %u  BTl", user->user_id, user->cur_contest->index);
	//	if (user) {
	if (user->cur_contest->status == STATUS_WAITING_CREATE ||
			user->cur_contest->status == STATUS_WAITING_ENTER) {
		player_try_contest_btl(user->cur_contest, user);
	}
	//	}
}

contest_t * get_player_cur_contest_by_id(uint32_t uid)
{
	return NULL;
}

ContestGroup * auto_join_group(user_contest_info_t * info)
{
//	uint64_t session = get_user_old_session(info->user_id, info->role_tm);
//	if (session) {
//		ContestGroup * group = find_contestgroup_by_session(session);
//		return group;
//	}

	for (GroupList::iterator it = waiting_groups->begin();
			it != waiting_groups->end(); ++it) {
		ContestGroup * group = *it;
		if (group->CanJoin(info)) {
			group->JoinIn(info);
			if (group->IsFull()) {
				waiting_groups->erase(it);
			}
			return group;
		}
	}

	ContestGroup * group = CreateGroup(info->user_id, info->role_tm);
	group->JoinIn(info);
	waiting_groups->push_back(group);
	return group;
}

ContestGroup * FindGroup(uint64_t session)
{
	GroupMap::iterator it = going_groups->find(session);
	if (it != going_groups->end()) {
		return (it->second);
	}
	return NULL;
}

void DelGroup(ContestGroup * group)
{
	for (GroupList::iterator it = waiting_groups->begin();
			it != waiting_groups->end(); ++it) {
		if ((*it)->sessionId == group->sessionId) {
			waiting_groups->erase(it);
			delete group;
			return;
		}
	}
	going_groups->erase(group->sessionId);
}

ContestGroup * find_contestgroup_by_session(uint64_t sessionId)
{
	for (GroupList::iterator it = waiting_groups->begin();
			it != waiting_groups->end(); ++it) {
		if ((*it)->sessionId == sessionId) {
			return (*it);
		}
	}

	GroupMap::iterator it = going_groups->find(sessionId);
	if ( it != going_groups->end()) {
		return it->second;
	}
	return NULL;
}

ContestGroup::~ContestGroup()
{
	for (std::vector<contest_t*>::iterator it = allContests_.begin();
			it != allContests_.end(); ++it) {
		contest_t * contest_ = *it;
		del_contest(contest_);
	}

	allContests_.clear();

	for (std::list<user_contest_info_t*>::iterator it = allUsers_.begin();
			it != allUsers_.end(); ++it) {
		user_contest_info_t * user = *it;
		player_info_t * info = Online::get_player_info(user->user_id);
		if (!info && user->out_flag) { //清理离线玩家数据
			clear_user_old_session(user->user_id);
		} else if (info && info->session_id == sessionId) {//通知在线玩家更改状态
			info->session_id = 0;
			send_header_to_player(info, proto_leave_contest_group, 0, 0);
		}
		delete user;
	}
	
	allUsers_.clear();

	if (status_ == STATUS_WAITING) {
		for (std::list<ContestGroup*>::iterator it = waiting_groups->begin();
				it != waiting_groups->end(); ++it) {
			if (this->sessionId == (*it)->sessionId) {
				waiting_groups->erase(it);
				break;
			}
		}
	}
    else {
		going_groups->erase(this->sessionId);
	}
}

void ContestGroup::pack_all_user_info(void * buf, int & idx)
{
	for (std::list<user_contest_info_t*>::reverse_iterator it = allUsers_.rbegin();
			it != allUsers_.rend(); ++it) {
		user_contest_info_t * user = *it;
		pack_h(buf, user->user_id, idx);
		pack_h(buf, user->role_tm, idx);
		pack_h(buf, user->role_type, idx);
		pack(buf, user->nick_name, MAX_NICK_SIZE, idx);
	}

}

void ContestGroup::pack_group_contest_info(void * buf, int & idx)
{
		int size = allContests_.size();
		pack_h(buf, size, idx);
		contest_t * last_contest = allContests_[0];
		pack_h(buf, last_contest->winner, idx);

		//userid, role_tm, role_type,  nick , all info of one player has a buf size = 28

		if (this->allUsers_.size() < USER_SIZE) {
			int empty_size = (USER_SIZE - 1) * 2 - allUsers_.size(); 
			memset(((char*)(buf) + idx), 0, empty_size * 28);
			idx += (empty_size * 28);
			pack_all_user_info(buf, idx);
		} else {
			for (std::vector<contest_t*>::iterator it = allContests_.begin();
					it != allContests_.end(); ++it) {
				contest_t * contest_info = *it;
				pack_contest_info(contest_info, buf, idx);
			}
		}
}

contest_t * ContestGroup::get_group_contest_by_index(uint32_t index)
{
	assert(index <= (USER_SIZE - 2) && index >= 0);
	return allContests_[index];
}

void ContestGroup::SetStart()
{

	InitContests();
	status_ = STATUS_PREAPREING;
	going_groups->insert(GroupMap::value_type(sessionId, this));
}

void ContestGroup::SetEnded(user_contest_info_t * player)
{
	if (status_ != STATUS_ENDING) {
		end_tm = get_now_tv()->tv_sec;
		status_ = STATUS_ENDING;
		assert(champion == NULL);
		champion = player;
		notify_all_player_over();
		ended_groups->push_back(this);
	}
//		
}

void ContestGroup::InitsessionID(uint32_t uid, uint32_t role_tm) 
{
	static int session = 0;
	int idx = 0;
	pack_h(&sessionId, uid, idx);
	pack_h(&sessionId, ++session, idx);
	DEBUG_LOG("GROUP SESSION %lu %u", sessionId, session);
}

void ContestGroup::JoinIn(user_contest_info_t * user)
{
	allUsers_.push_back(user);
	user->group = this;
	if (Lv_ == 0) {
		Lv_ = (user->user_lv - 1) / 10;
	}
}

void ContestGroup::Leave(uint32_t uid)
{
	//未开始的16强组,需要将玩家去掉
	if (status_ == STATUS_WAITING) {
		for (std::list<user_contest_info_t*>::iterator it = allUsers_.begin();
				it != allUsers_.end(); ++it) {
			if ((*it)->user_id == uid) {
				allUsers_.erase(it);
				return;
			}
		}
	} else  {
		//将玩家设置掉线状态
		for (std::list<user_contest_info_t*>::iterator it = allUsers_.begin();
				it != allUsers_.end(); ++it) {
			if ((*it)->user_id == uid) {
				(*it)->out_flag = 1;
				return;
			}
		}
		//out_num ++;
	}
}


bool ContestGroup::CanJoin(user_contest_info_t * user)
{
	if (allUsers_.size() < USER_SIZE) {
		if (Lv_) {
			if (Lv_ == ((user->user_lv - 1) / 10)) {
				return true;
			}
		} else {
			return true;
		}
	}
	return false;
//	return true;
}


void ContestGroup::notify_all_player_over()
{
	assert(allUsers_.size() != 0);
	assert(champion);
	for (std::list<user_contest_info_t*>::iterator it = allUsers_.begin();
			it != allUsers_.end(); ++it) {
		user_contest_info_t * user = *it;
		if (!user->out_flag) {
			int idx = sizeof(svr_proto_t);
			assert(champion);
			pack_h(s_pkg_, champion->user_id, idx);
			pack_h(s_pkg_, champion->role_tm, idx);
			pack(s_pkg_, champion->nick_name, MAX_NICK_SIZE, idx);

			player_info_t * player = Online::get_player_info(user->user_id);
			assert(player);
			uint32_t guess_flag = 0;
			if (user->guess_champion) {
				if (user->guess_champion->user_id == champion->user_id) {
					guess_flag = 1;
				} else {
					guess_flag = 2;
				}
			}
			pack_h(s_pkg_, guess_flag, idx);
			init_proto_head(s_pkg_, player->usrid, idx,  proto_contest_all_over, 0, 0);	
			send_pkg_to_client(player->fdsess, s_pkg_, idx);
		}
	}
}

void pack_contest_info(contest_t * contest, void * buf, int & idx)
{
//	pack_h(buf, contest->status, idx);
//	pack_h(buf, contest->winner, idx);
	if (contest->first_player) {
		user_contest_info_t * first = contest->first_player;
		pack_h(buf, first->user_id, idx);
		pack_h(buf, first->role_tm, idx);
		pack_h(buf, first->role_type, idx);
		pack(buf, first->nick_name, MAX_NICK_SIZE, idx);
		DEBUG_LOG("FIRST %u %u %s", first->user_id, first->role_tm, first->nick_name);
	} else {
		pack_h(buf, 0, idx);
		pack_h(buf, 0, idx);
		pack_h(buf, 0, idx);
		char name[16] = {0};
		pack(buf, name, MAX_NICK_SIZE, idx);
		DEBUG_LOG("FIRST %u %u %s", 0, 0, "0");
	}
	
	if (contest->second_player) {
		user_contest_info_t * second = contest->second_player;
		pack_h(buf, second->user_id, idx);
		pack_h(buf, second->role_tm, idx);
		pack_h(buf, second->role_type, idx);
		pack(buf, second->nick_name, MAX_NICK_SIZE, idx);

		DEBUG_LOG("SECOND %u %u %s", second->user_id, second->role_tm, second->nick_name);
	} else {
		pack_h(buf, 0, idx);
		pack_h(buf, 0, idx);
		pack_h(buf, 0, idx);
		char name[16] = {0};
		pack(buf, name, MAX_NICK_SIZE, idx);

		DEBUG_LOG("FIRST %u %u %s", 0, 0, "0");
	}

}

int constest_check_room_create(void * contest, void * data) 
{
	return 0;
}

contest_t * get_next_contest(contest_t * cur)
{
	ContestGroup * group = cur->first_player->group;
//	ERROR_LOG("GET NEXT CONTEST %u", cur->index);
	if (cur->index > 0) {
		//树形的Heap结构,顶点是决赛
		int index = (cur->index + 1) / 2 - 1;
//		ERROR_LOG("NEXT CONTEST IDX =%u", index);
		cur->next_contest = group->get_group_contest_by_index(index);
		assert(cur->next_contest != NULL);
		cur->next_contest->pre_contest = cur;
//		cur->next_contest->index = index;
		return cur->next_contest;
	}
	return NULL;
}

void set_contest_status(contest_t * contest, contest_status_t status)
{
	DEBUG_LOG("SET CONTEST %u  STATUS %u ", contest->index, status);
	contest->status = status;
	if (status == STATUS_ENDING) {
		REMOVE_TIMERS(contest);
	}
}

void pack_user_passed_contest(user_contest_info_t * user, void * buf, int & idx)
{

	int ifx = idx + 4;
	contest_t * passed_contest = user->group->get_group_contest_by_index(user->init_index);
	uint32_t passed_num = 0;
	while(passed_contest && passed_contest->status == STATUS_ENDING) {
		passed_num++;
		user_contest_info_t * enemy = passed_contest->first_player; 
		if (enemy->user_id == user->user_id) {
			enemy = passed_contest->second_player;
		}	
		pack_h(buf, enemy->user_id, ifx);
		pack_h(buf, enemy->role_tm, ifx);
		pack_h(buf, enemy->role_type, ifx);
		pack(buf, enemy->nick_name, MAX_NICK_SIZE, ifx);
		uint32_t win_flag = 0;
		if (passed_contest->winner == user->user_id) {
			win_flag = 1;
		}
		pack_h(buf, win_flag, ifx);
		pack_h(buf, passed_contest->win_val_, ifx);
		uint32_t monster_val = 0;
		if (passed_contest->monster_killer_ == user->user_id) {
			monster_val = passed_contest->monster_val_;
		}
		pack_h(buf, monster_val, ifx);
		pack_h(buf, passed_contest->spurprise_val_, ifx);

		if (passed_contest->winner == user->user_id) {
			passed_contest = passed_contest->next_contest;
		} else {
			break;
		}
	}

	pack_h(buf, passed_num, idx);
	idx = ifx;
}

void notify_contest_result_to_player(contest_t * contest)	
{
	
	user_contest_info_t * first_player = contest->first_player;
	user_contest_info_t * second_player = contest->second_player;

//	assert(first_player != NULL && second_player != NULL);

	if (first_player && second_player && !first_player->out_flag) {
		player_info_t * first = Online::get_player_info(first_player->user_id);
		if (first && first->session_id) {
			int idx = sizeof(svr_proto_t);
			pack_h(s_pkg_, first_player->win_times, idx);
			uint32_t win_flag = 0;
			if (first->usrid == contest->winner) {
				win_flag = 1;

			} 

			pack_h(s_pkg_, win_flag, idx);
			pack_h(s_pkg_, second_player->user_id, idx);
			pack_h(s_pkg_, second_player->role_tm, idx);
			pack_h(s_pkg_, second_player->role_type, idx);
			pack(s_pkg_, second_player->nick_name, MAX_NICK_SIZE, idx);
			pack_h(s_pkg_, contest->win_val_, idx);
			uint32_t monster_val = 0;
			if (contest->monster_killer_ == first->usrid) {
				monster_val = contest->monster_val_;
			}
			pack_h(s_pkg_, monster_val, idx);
			pack_h(s_pkg_, contest->spurprise_val_, idx);
			init_proto_head(s_pkg_, first->usrid, idx,  proto_player_win_contest, 0, 0);	
			send_pkg_to_client(first->fdsess, s_pkg_, idx);
		}
	}

	if (second_player && first_player && !second_player->out_flag) {
		player_info_t * second = Online::get_player_info(second_player->user_id);
		if (second && second->session_id) {
			int idx = sizeof(svr_proto_t);
			pack_h(s_pkg_, second_player->win_times, idx);
			uint32_t win_flag = 0;
			if (second->usrid == contest->winner) {
				win_flag = 1;
			}
			pack_h(s_pkg_, win_flag, idx);
			pack_h(s_pkg_, first_player->user_id, idx);
			pack_h(s_pkg_, first_player->role_tm, idx);
			pack_h(s_pkg_, first_player->role_type, idx);
			pack(s_pkg_, first_player->nick_name, MAX_NICK_SIZE, idx);
			pack_h(s_pkg_, contest->win_val_, idx);
			uint32_t monster_val = 0;
			if (contest->monster_killer_ == second->usrid) {
				monster_val = contest->monster_val_;
			}
			pack_h(s_pkg_, monster_val, idx);
			pack_h(s_pkg_, contest->spurprise_val_, idx);
			init_proto_head(s_pkg_, second->usrid, idx,  proto_player_win_contest, 0, 0);	
			send_pkg_to_client(second->fdsess, s_pkg_, idx);
		}
	}
}

//struct contest_result_info_t {
//	uint32_t win_flag;
//	uint32_t enmey_id;
//	uint32_t enemy_role_tm;
//	uint32_t enemy_role_type;
//	char     enemy_name[MAX_NICK_SIZE];
//	int      exploit_chg;
//};
//
//contest_result_info_t * get_user_contest_result(contest_t * contest_info, r

void player_win_contest(contest_t * contest, user_contest_info_t * player)
{

	set_contest_status(contest, STATUS_ENDING);
	contest->winner = player->user_id;
	DEBUG_LOG("Player %u Win Contest", player->user_id);
	notify_contest_result_to_player(contest);	
	player->win_times++;

	contest_t * next = get_next_contest(contest);
	if (next) {
		DEBUG_LOG("PLAYER %u NEXT CONTEST IS %u", player->user_id, next->index); 
		set_player_ready(next, player);
	} else {
		//notify contest group over
		ContestGroup * group = player->group;	
//		group->set_contest_champion(player);
		group->SetEnded(player);
	}
}

int check_contest_room_create(void * contest, void *data)
{
	contest_t * contest_info = reinterpret_cast<contest_t*>(contest);
	//检查房间创建，未创建则直接判第一玩家输
	if (contest_info->battle_svr_id == -1 
			&& contest_info->room_index == -1
			&& contest_info->status == STATUS_WAITING_CREATE) {
		DEBUG_LOG("FIRST Do not CRATE ROOM, SECOND WIN %u %u %u", contest_info->first_player->user_id,
			   contest_info->second_player->user_id, contest_info->index);	
		player_win_contest(contest_info, contest_info->second_player);
	} 
	return 0;
}

void contest_btl_over(contest_t * contest, uint32_t winner)
{
	if (contest->first_player->user_id == winner) {
		DEBUG_LOG("CONTEST %u OVER WINNER IS FIRST PLAYER %u", contest->index, winner);
		player_win_contest(contest, contest->first_player);
	} else {
		DEBUG_LOG("CONTEST %u OVER WINNER IS SECOND PLAYER %u", contest->index, winner);
		player_win_contest(contest, contest->second_player);
	}

}

int send_header_to_player(player_info_t * player,
	   					  uint32_t cmd, 
						  uint32_t errno,
						  uint32_t seqno)
{
	int idx = sizeof(svr_proto_t);
	init_proto_head(s_pkg_, player->usrid,  idx, cmd, errno, seqno);
	return send_pkg_to_client(player->fdsess, s_pkg_, idx);
}

int send_enter_contest_room_info(contest_t * contest, player_info_t * player)
{
	int idx = sizeof(svr_proto_t);
	assert(contest->first_player != NULL);
	pack_h(s_pkg_, contest->first_player->win_times, idx);
	pack_h(s_pkg_, contest->win_val_, idx);
	pack_h(s_pkg_, contest->lose_val_, idx);
	pack_h(s_pkg_, contest->spurprise_val_, idx);
	DEBUG_LOG("NOTI PLAYER %u OF CONTEST %u ENTER ROOM %u %u %u",
			player->usrid, contest->index, contest->win_val_, contest->lose_val_, contest->spurprise_val_); 
	init_proto_head(s_pkg_, player->usrid, idx, proto_contest_enter_room_msg, 0, 0); 
	return send_pkg_to_client(player->fdsess, s_pkg_, idx);
}

void notify_player_enter_room(user_contest_info_t * user)
{
	player_info_t * player = Online::get_player_info(user->user_id);
	assert(player->session_id != 0);
	assert(user->cur_contest);
	if (player) {
		send_enter_contest_room_info(user->cur_contest, player);
	}
}

void ran_contest_need_req(contest_t * contest_info)
{
	assert(contest_info->first_player != NULL && contest_info->second_player);

//	static int monster_id[5] = {11432, 11416, 11437, 11431, 11430};
	static int muti_num[5] = {2, 2, 3, 3, 4}; 
	static int monster_val[5] = {500, 400, 300, 200, 100};
	static int basic_win[5] = {300, 600, 900, 1200, 1500};
	assert(contest_info->first_player->win_times == contest_info->second_player->win_times);
	contest_info->win_val_ = basic_win[contest_info->first_player->win_times];
	uint32_t t_idx = (contest_info->first_player->role_tm + contest_info->second_player->role_tm) % 10; 
	if (t_idx < 5) {
		contest_info->win_val_ *= muti_num[t_idx]; 	
		contest_info->monster_val_ = monster_val[t_idx];
	} 

	uint32_t s_idx = rand() % 100;
	if (s_idx < 15) {
		contest_info->spurprise_val_ = 500;
	} else if (s_idx < 30) {
		contest_info->spurprise_val_ = 300;
	} else if (s_idx < 45) {
		contest_info->spurprise_val_ = 200;
	} else if (s_idx < 60) {
		contest_info->spurprise_val_ = 100;
	}

	DEBUG_LOG("CONTEST %u Exploit Info %u %u %u", contest_info->win_val_, contest_info->spurprise_val_,
			contest_info->monster_val_, contest_info->lose_val_);
}

int contest_ready(void * contest, void * data)
{
	contest_t * contest_info = reinterpret_cast<contest_t*>(contest);

	assert(contest_info->first_player && contest_info->second_player);
	assert(contest_info->status == STATUS_WAITING);	
	set_contest_status(contest_info, STATUS_WAITING_CREATE);
	DEBUG_LOG("SET CONTEST %u STATUS PREAPREING", contest_info->index);

    ran_contest_need_req(contest_info);

	//第一个玩家不在线第2个玩家直接获胜
	if (contest_info->first_player->out_flag) {
		DEBUG_LOG("FIRST %u NOT ON, WIN DIRECT!", contest_info->first_player->user_id);
		player_win_contest(contest_info, contest_info->second_player);
		//第2个玩家不在线，第一个玩家赢
	} else if (contest_info->second_player->out_flag) {
		DEBUG_LOG("SECOND %u NOT ON, FIRST WIN DIRCT!", contest_info->second_player->user_id);
		player_win_contest(contest_info, contest_info->first_player);
	} else {

		//通知第一个玩家创建
		DEBUG_LOG("CONTEST %u SEND ENTER ROOM TO FIRST PLAYER %u", contest_info->index, contest_info->first_player->user_id);
		notify_player_enter_room(contest_info->first_player);
		//5s 后检查房间创建
		ADD_TIMER_EVENT(contest_info, check_contest_room_create, 0, get_now_tv()->tv_sec + 5); 
	}	

	return 0;
}

void notify_contest_player_start_tm(contest_t * contest, uint32_t start_tm)
{
	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, contest->first_player->win_times, idx); //第几轮, 0开始
	uint32_t time_dec = start_tm - get_now_tv()->tv_sec;
	pack_h(s_pkg_, time_dec, idx);//开始时间
	pack_h(s_pkg_, contest->first_player->user_id, idx);
	pack_h(s_pkg_, contest->first_player->role_tm, idx);
	pack_h(s_pkg_, contest->first_player->role_type, idx);
	pack(s_pkg_, contest->first_player->nick_name, MAX_NICK_SIZE, idx);
	pack_h(s_pkg_, contest->second_player->user_id, idx);
	pack_h(s_pkg_, contest->second_player->role_tm, idx);
	pack_h(s_pkg_, contest->second_player->role_type, idx);
	pack(s_pkg_, contest->second_player->nick_name, MAX_NICK_SIZE, idx);

	player_info_t * first = Online::get_player_info(contest->first_player->user_id);
	if (first && !contest->first_player->out_flag) {
//		send_pkg_to_player(first, s_pkg_, proto_contest_auto_msg, 0, 0);
		init_proto_head(s_pkg_, first->usrid, idx, proto_contest_auto_msg, 0, 0); 
		send_pkg_to_client(first->fdsess, s_pkg_, idx);
		DEBUG_LOG("Notify Player START TM %u contest idx %u", first->usrid, contest->index);
	}
	player_info_t * second = Online::get_player_info(contest->second_player->user_id);
	if (second && !contest->second_player->out_flag) {
		init_proto_head(s_pkg_, second->usrid, idx, proto_contest_auto_msg, 0, 0); 
		send_pkg_to_client(second->fdsess, s_pkg_, idx);
		DEBUG_LOG("Notify Player START TM %u contest idx %u", second->usrid, contest->index);
	}
}

bool both_player_ready(contest_t * contest)
{
	return  (contest->first_player != NULL && contest->second_player != NULL);
}

void set_player_right_place(contest_t * contest, user_contest_info_t * player) 
{
	//第一轮初始比赛
	if (contest->index == player->init_index) {
		if (contest->first_player) {
			contest->second_player = player;
			DEBUG_LOG("CONTEST %u SECOND PLAYER %u READY", contest->index, player->user_id);
		} else {
			contest->first_player = player;
			DEBUG_LOG("CONTEST %u FIRST PLAYER %u READY", contest->index, player->user_id);
		}
	} else {
		assert(player->cur_contest);
		uint32_t pre_index = player->cur_contest->index;
		if (pre_index > (contest->index * 2 + 1)) {
			contest->second_player = player;
			DEBUG_LOG("CONTEST %u SECOND PLAYER %u READY", contest->index, player->user_id);
		} else if (pre_index == (contest->index * 2 + 1)) {
			contest->first_player = player;
			DEBUG_LOG("CONTEST %u FIRST PLAYER %u READY", contest->index, player->user_id);
		}
	}
}

void set_player_ready(contest_t * contest, user_contest_info_t * player)
{
	//保证树形的堆比赛结构
	set_player_right_place(contest, player);
	player->cur_contest = contest;
	player->player_status = STATUS_WAITING;

	if (player->group && !player->group->IsGoing()) {
		player->group->SetStatus(STATUS_GOING);
	}

	if (both_player_ready(contest)) {
		//第一轮比赛准备时间3分钟, 其他时间准备1分钟
		uint32_t start_tm = get_now_tv()->tv_sec + 60;
		if (contest->first_player->init_index 
				== contest->second_player->init_index) {
			start_tm = get_now_tv()->tv_sec + 120; 
		}
		ADD_TIMER_EVENT(contest, contest_ready, 0, start_tm);
		notify_contest_player_start_tm(contest, start_tm);
	}
}

int send_contest_room_info(player_info_t * p, 
						   int svr_id, 
						   int room_id, 
						   uint32_t pvp_lv)
{
	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, svr_id, idx);
	pack_h(s_pkg_, room_id, idx);
	pack_h(s_pkg_, pvp_lv, idx);
	init_proto_head(s_pkg_, p->usrid, idx, proto_auto_join_pvp_room, 0, 0);
	return send_pkg_to_client(p->fdsess, s_pkg_, idx);
}

void player_try_contest_btl(contest_t * contest, user_contest_info_t * user)
{
	assert(contest->first_player != NULL && contest->second_player != NULL);

	//第一个玩家返回要求创建房间
	if (contest->first_player->user_id == user->user_id
		&& contest->status == STATUS_WAITING_CREATE) { 
		contest->first_player->player_status = STATUS_PREAPREING;
//		set_contest_status(contest, STATUS_WAITING_ENTER);
		DEBUG_LOG("CONTEST %u First Player %u TRY BTL", contest->index, contest->first_player->user_id); 
		player_info_t * first = Online::get_player_info(user->user_id);
		assert(first != NULL);
		send_contest_room_info(first, -1, -1, pvp_16_contest);

	} else if (contest->second_player->user_id == user->user_id
			&& contest->status == STATUS_WAITING_ENTER) {	

		//第2个玩家直接返玩家1创建的房间号
		contest->second_player->player_status = STATUS_PREAPREING;
		assert(contest->battle_svr_id != -1 && contest->room_index != -1);
		player_info_t * second = Online::get_player_info(user->user_id);
		assert(second != NULL && second->session_id != 0);
		DEBUG_LOG("CONTEST %u SECOND PLAYER %u TRY BTL ROOM %u %u", contest->index,
				second->usrid, contest->battle_svr_id, contest->room_index);
		send_contest_room_info(second, contest->battle_svr_id, 
				contest->room_index, pvp_16_contest);
	}
}

int  check_second_player_in(void * contest, void * data)
{
	contest_t * contest_info = reinterpret_cast<contest_t*>(contest);
	assert(contest_info->first_player != NULL && contest_info->second_player != NULL);
	DEBUG_LOG("5s SECOND PLAYER %u  NOT IN CONTEST %u", contest_info->second_player->user_id, contest_info->index); 
	if (contest_info->status == STATUS_WAITING_ENTER) {
		DEBUG_LOG("FIRST PLAYER %u WIN CONTEST %u", contest_info->first_player->user_id, contest_info->index);
		player_win_contest(contest_info, contest_info->first_player);
	}
	return 0;
}

void player_create_contest_room(contest_t * contest, uint32_t btl_id, uint32_t room_index)
{
	contest->battle_svr_id = btl_id;
	contest->room_index = room_index;
	set_contest_status(contest, STATUS_WAITING_ENTER);

	DEBUG_LOG("CONTERST %u FIRST PLAYER %u CREATRE ROOM %u %u",
			contest->index, contest->first_player->user_id, btl_id, room_index);
	//移出检查房间创建的逻辑
	REMOVE_TIMERS(contest);
	//通知第2个玩家进入
	assert(contest->second_player != NULL);
	if (!contest->second_player->out_flag) {
		DEBUG_LOG("CONTEST %u  NOTIFY SECOND PLAYER %u ENTER", contest->index, contest->second_player->user_id);
		notify_player_enter_room(contest->second_player);
	}
	//5s后检查第2个玩家是否进入战斗房间的逻辑
	ADD_TIMER_EVENT(contest, check_second_player_in, 0, get_now_tv()->tv_sec + 5); 
}

int check_contest_ended(void * contest, void * data)
{
	contest_t * contest_info = reinterpret_cast<contest_t*>(contest);
	DEBUG_LOG("CONTEST NOT  ENDED IN 5 mins !");
	if (contest_info->status != STATUS_ENDING ) {
		if ((rand() % 2) == 1) {
			player_win_contest(contest_info, contest_info->first_player);
		} else {
			player_win_contest(contest_info, contest_info->second_player);
		}
	}
	return 0;
}

void player_enter_contest_room(contest_t * contest, uint32_t btl_id, uint32_t room_index)
{
	if (contest->status == STATUS_WAITING_ENTER) {
		assert(contest->second_player != NULL);
		set_contest_status(contest, STATUS_GOING);
		DEBUG_LOG("SECOND PLAYER %u ENTER CONTEST %u ROOM %u %u", contest->second_player->user_id,
				contest->index, btl_id, room_index);
		REMOVE_TIMERS(contest); //移出检查第2个玩家是否进入的逻辑
		//360s后检查战斗是否结束
		ADD_TIMER_EVENT(contest, check_contest_ended, 0, get_now_tv()->tv_sec + 500);
	}
}


int64_t get_user_old_session(uint32_t uid, uint32_t role_tm)
{
	std::map<uint32_t, session_t*>::iterator it = all_old_session->find(uid);
	if (it != all_old_session->end()) {
		if (it->second->role_tm == role_tm) {
			DEBUG_LOG("GET %u OLD SESSION %lu", uid, it->second->session);
			return (it->second)->session;
		} else {
			all_old_session->erase(it);
		}
	}
	return 0;
}

void save_player_session(uint32_t uid, uint32_t role_tm, uint64_t sess)
{
	session_t * session = new session_t(uid, role_tm, sess);
	all_old_session->insert(std::map<uint32_t, session_t*>::value_type(uid, session));
}

void clear_user_old_session(uint32_t uid)
{
	DEBUG_LOG("USER %u Clear Old Session", uid);
	std::map<uint32_t, session_t*>::iterator it = all_old_session->find(uid);
	if (it != all_old_session->end()) {
		session_t * old_session = it->second;
		DEBUG_LOG("USER %u Clear Old Session %lu Ok", uid,  old_session->session);
		all_old_session->erase(it);
		delete old_session;
	}
}

void update_all_end_contest()
{
	static uint32_t last_update_tm = 0;
	if (last_update_tm != (uint32_t)get_now_tv()->tv_sec) {
		for (GroupList::iterator it = ended_groups->begin(); it != ended_groups->end();
				++it) {
			if ((*it)->OutDate()) {
				ContestGroup * group = (*it);
				DEBUG_LOG("DEL OUT TIME GROUP %lu", group->sessionId);
				it = ended_groups->erase(it);
				delete group;
			}
		}
		last_update_tm = get_now_tv()->tv_sec;
	}
}



