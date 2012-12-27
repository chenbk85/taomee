#include <cstdio>
#include <map>
#include <math.h>
#include <libtaomee++/random/random.hpp>
#include <libtaomee++/inet/pdumanip.hpp>

using namespace std;
using namespace taomee;
extern "C" {
#include <glib.h>

#include <libtaomee/log.h>
#include <libtaomee/timer.h>
}

#include "player.hpp"
#include "utils.hpp"
#include "home_attr.hpp"
#include "home.hpp"
#include "home_impl.hpp"

/*! hold all the players */
static PlayerMap all_players;

//static PlayerMap all_monsters;

/**
  * @brief allocate and add a player to this server
  * @return pointer to the newly added player
  */
Player* add_player(userid_t uid, fdsession_t* fdsess)
{

	Player* p = new Player(uid, fdsess);
	all_players.insert(PlayerMap::value_type(uid, p));
	return p;
}

void del_player(Player *p)
{
	delete p;
}

/**
  * @brief get player by user id
  * @param uid user id
  * @return pointer to the player if found, 0 otherwise
  */
Player* get_player(userid_t uid)
{
	PlayerMap::iterator it = all_players.find(uid);
	if (it != all_players.end()) {
		return it->second;
	}

	return 0;
}


Player * CreateMonster(summon_t & mon)
{
	Player *p = new Player();
	p->id = mon.mon_type;
	p->role_tm = mon.mon_tm; 
	p->lv = mon.lv;
	memcpy(p->nick, mon.nick, max_nick_size);
	return p;
}
/**
  * @brief clear players' info whose online server is down
  * @param fd online fd
  */
void clear_players(int fd)
{
	DEBUG_LOG("B4\t[player_num=%lld]", static_cast<long long>(all_players.size()));

	PlayerMap::iterator it = all_players.begin();
	while (it != all_players.end()) {
		PlayerMap::iterator t_it = it++;
		Player* p = t_it->second;
		if ((p->fd == fd) || (fd == -1)) {
			del_player(p);
		}
	}
	DEBUG_LOG("AF\t[player_num=%lld]", static_cast<long long>(all_players.size()));
}



//-----------------------------------------------------------
// Public Methods
Player::Player(userid_t uid, fdsession_t* fdsession)
{
	id = 0;
	if (fdsession) {
		fd		  = fdsession->fd;
		id		  = uid;
		fdsess = fdsession;
	} else {
		fdsession = 0;
	}

	role_tm = 0;
	role_type = 0;
    power_user = 0;
	player_show_state = 0;

	vip = 0;
	vip_level = 0;
    using_achieve_title = 0;
	app_mon = 0;
	honor = 0;

	coins = 0;
	lv = 0;

	xpos = 0;
	ypos = 0;
	clothes_cnt = 0;

	fight_summon = 0;
	waitcmd = 0;
	invisible = 0;
	summon_cnt = 0;
	pkg_queue  = g_queue_new();

	memset(allsummon, 0, max_summon_num * sizeof(summon_t)); 
	memset(nick, 0, max_nick_size);
	memset(clothes_arr, 0, sizeof(clothes_base_t) * max_clothes_num);

	CurHome = 0;
//	SelfHome = 0;
	
	TRACE_LOG("alloc player %u %p: fdsess=%p fd=%d", id, this, fdsess, fd);
}

Player::~Player()
{

	if (is_valid_uid(id)) {

   		cached_pkg_t* pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(pkg_queue));
		while (pkg) {
			g_slice_free1(pkg->len, pkg);
			pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(pkg_queue));
		}

		g_queue_free(pkg_queue);
		all_players.erase(id);
		if (CurHome) {
			this->CurHome->LeaveHome(this);
			if (this->CurHome->Empty()) {
				Home * home = this->CurHome;
				delete home;
			}
			CurHome = 0;
		}
		TRACE_LOG("dealloc player %u", id);

	} else {
		TRACE_LOG("dealloc monster %u", id);
	}
}

void Player::init_player(player_info_t * p_info)
{
	id = p_info->id;
	role_tm = p_info->role_tm;
	role_type = p_info->role_type;
    power_user = p_info->power_user;
	player_show_state = p_info->player_show_state;
	memcpy(nick, p_info->nick, max_nick_size);
	vip = p_info->vip;
	vip_level = p_info->vip_level;
	coins = p_info->coins;
	lv = p_info->lv;
    using_achieve_title = p_info->achieve_title;
	flag = p_info->flag;
	app_mon = p_info->app_mon;

	xpos = p_info->xpos;
	ypos = p_info->ypos;


	clothes_cnt = p_info->clothes_cnt;

	memcpy(clothes_arr, p_info->clothes_arr, clothes_cnt * sizeof(clothes_base_t));

	summon_cnt = p_info->summon_cnt;

	fight_summon = p_info->fight_mon_tm;
	summon_t * p_summon = reinterpret_cast<summon_t*>((uint8_t *)(p_info->clothes_arr) + clothes_cnt * sizeof(clothes_base_t));
	memcpy(allsummon, p_summon, sizeof(summon_t)*summon_cnt);
}

void Player::create_summon_monster(summon_t & mon)
{
}

void
Player::walk(uint32_t x, uint32_t y, uint32_t flag)
{
	
	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	pack(pkgbuf, role_type, idx);
	pack(pkgbuf, x, idx);
	pack(pkgbuf, y, idx);
	pack(pkgbuf, flag, idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf+sizeof(home_proto_t), id, cli_proto_walk, 0, idx-sizeof(home_proto_t));
	CurHome->send_to_home(pkgbuf, idx, this, 0);

}

void
Player::walk_keyboard(uint32_t x, uint32_t y, uint8_t dir, uint8_t state)
{
	
	xpos = x;
	ypos = y;
	direction = dir;
	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	pack(pkgbuf, role_type, idx);
	pack(pkgbuf, x, idx);
	pack(pkgbuf, y, idx);
	pack(pkgbuf, dir, idx);
	pack(pkgbuf, state, idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf+sizeof(home_proto_t), id, cli_proto_walk_keyboard, 0, idx-sizeof(home_proto_t));
	CurHome->send_to_home(pkgbuf, idx, this, 0);
	
}

void
Player::stand(uint32_t x, uint32_t y, uint8_t dir)
{
	xpos = x;
	ypos = y;
	direction = dir;
	TRACE_LOG("%u %u %u", id, x, y);
	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	pack(pkgbuf, x, idx);
	pack(pkgbuf, y, idx);
	pack(pkgbuf, dir, idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf+sizeof(home_proto_t), id, cli_proto_stand, 0, idx-sizeof(home_proto_t));
	CurHome->send_to_home(pkgbuf, idx, this, 0);
}

void
Player::jump(uint32_t x, uint32_t y)
{
	xpos = x;
	ypos = y;
	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	pack(pkgbuf, x, idx);
	pack(pkgbuf, y, idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf+sizeof(home_proto_t), id, cli_proto_jump, 0, idx-sizeof(home_proto_t));
	CurHome->send_to_home(pkgbuf, idx, this, 0);
}

void
Player::talk(uint8_t* msg, uint32_t msg_len, userid_t recvid)
{
	int idx = sizeof(home_proto_t);
//	idx += sizeof(cli_proto_t);
	pack(pkgbuf, msg, msg_len, idx);
	TRACE_LOG("%u %s %u %u %s", id, nick, recvid, msg_len, msg);

//	pack(pkgbuf, nick, sizeof(nick), idx);
//	pack(pkgbuf, recvid, idx);
//	pack(pkgbuf, msg_len, idx);
//	pack(pkgbuf, msg, msg_len, idx);

	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
//	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), id, cli_proto_talk, 0, idx - sizeof(home_proto_t));
	CurHome->send_to_home(pkgbuf, idx, 0, 0);
}

void
Player::stop()
{
	TRACE_LOG("uid=%u ", id);
}
/*
void
Player::enter_map(uint32_t mid)
{
	TRACE_LOG("uid=%u ", id);
}

void
Player::leave_map(bool leave_stage)
{
	TRACE_LOG("uid=%u ", id);
}
*/


void  Player::pack_home_attr_info(void* buf, int&idx)
{
	pack(buf, CurHome->OwnerUserId, idx);
	pack(buf, CurHome->OwnerRoleTime, idx);
	pack(buf, CurHome->GetHomeDetail()->get_home_level(), idx);
	pack(buf, CurHome->GetHomeDetail()->get_home_exp(), idx);
}

void 
Player::pack_player_map_info(void * buf, int & idx)
{
	//	TRACE_LOG("%u", id);
	pack(buf, id, idx);
	pack(buf, role_tm, idx);
	pack(buf, role_type, idx);
	pack(buf, power_user, idx);
	pack(buf, player_show_state, idx);
	pack(buf, vip, idx);
	pack(buf, vip_level, idx);
	pack(buf, using_achieve_title, idx);
	pack(buf, nick, max_nick_size, idx);
	pack(buf, (uint32_t)lv, idx);
	pack(buf, flag, idx);
	pack(buf, honor, idx);
	pack(buf, xpos, idx);
	pack(buf, ypos, idx);
	pack(buf, app_mon, idx);
	switch(role_type)
	{
		case 1:
			pack(buf, 145, idx);
     		break;
		
		case 2:
			pack(buf, 150, idx);
			break;

		case 3:
			pack(buf, 140, idx);
			break;

		case 4:
 	 		pack(buf, 140, idx);
			break;

		default:
				pack(buf, 140, idx);
			break;
	}
	pack(buf, clothes_cnt, idx);
	for(uint32_t i = 0; i < clothes_cnt; i++)  {
		pack(buf, clothes_arr[i].id, idx);
		pack(buf, clothes_arr[i].gettime, idx);
		pack(buf, clothes_arr[i].timelag, idx);
		pack(buf, clothes_arr[i].lv, idx);
	}

	pack(buf, summon_cnt, idx);

	for (uint32_t i = 0; i < summon_cnt; i++) {
	    pack(buf, allsummon[i].mon_type, idx); 
	 	pack(buf, allsummon[i].lv, idx);
	    pack(buf, allsummon[i].nick, max_nick_size, idx);
	    pack(buf, allsummon[i].call_flag, idx);
		pack(buf, 0, idx); //attr_per;
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
			
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);
		pack(buf, 0, idx);

		pack(buf, 0, idx); // summon skill count

	}
}

void Player::set_summon_flag(uint32_t pet_tm, uint32_t flag)
{
	for (uint32_t i = 0; i < summon_cnt; i++) {
		if (allsummon[i].mon_tm == pet_tm) {
			allsummon[i].call_flag = flag;
			break;
		}
	}

	if (this->CurHome->OwnerUserId == id && this->CurHome->OwnerRoleTime == role_tm)
	{
		CurHome->change_home_pet_flag(pet_tm, flag);
	}

	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	pack(pkgbuf, this->id, idx);
	pack(pkgbuf, this->role_tm, idx);
	pack(pkgbuf, pet_tm, idx);
	pack(pkgbuf, flag, idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), this->id, cli_proto_pet_home_call_mcast, 0, idx-sizeof(home_proto_t));
	CurHome->send_to_home(pkgbuf, idx, 0, 0);

}

void Player::summon_move(pet_move_t * rsp)
{

	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	pack(pkgbuf, this->id, idx);
	pack(pkgbuf, this->role_tm, idx);
	pack(pkgbuf, rsp->pet_tm, idx);
	pack(pkgbuf, rsp->x_pos, idx);
	pack(pkgbuf, rsp->y_pos, idx);
	pack(pkgbuf, rsp->dir, idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), this->id, cli_proto_pet_move, 0, idx-sizeof(home_proto_t));
	CurHome->send_to_home(pkgbuf, idx, 0, 0);

}

void Player::fight_summon_callback(uint8_t flag)
{
	for (uint32_t i = 0; i < summon_cnt; i++) {
		if (allsummon[i].mon_tm == this->fight_summon) {
			allsummon[i].call_flag = flag;
			break;
		}
	}

	if (this->CurHome->OwnerUserId == id && this->CurHome->OwnerRoleTime == role_tm)
	{
		CurHome->change_home_pet_flag(this->fight_summon, flag);
	}


	int idx = sizeof(home_proto_t);
	idx += sizeof(cli_proto_t);
	pack(pkgbuf, (uint8_t)(flag), idx);
	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), this->id, cli_proto_call_summon, 0, idx-sizeof(home_proto_t));


//	CurHome->send_to_home(pkgbuf, idx, 0, 0);
	CurHome->send_to_home(pkgbuf, idx, 0, 0);
}

void Player::add_curhome_exp(uint32_t addexp)
{
	CurHome->GetHomeDetail()->add_home_exp(3); 
	db_replace_home_exp_level(this, 
			                  CurHome->GetHomeDetail()->get_home_exp(),  
							  CurHome->GetHomeDetail()->get_home_level(),
							  0,
							  false
							  );
	if (CurHome->Owner) {
		Player * pl = CurHome->Owner;
		int idx = sizeof(home_proto_t);
		idx += sizeof(cli_proto_t);
		pack(pkgbuf, addexp, idx);
		init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
		init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), pl->id, cli_proto_add_home_exp, 0, idx-sizeof(home_proto_t));
		send_to_player(pl, pkgbuf, idx, 0);
	}
}




