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

#include "item.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "trade.hpp"
#include "market.hpp"


/*! hold all the players */
static PlayerMap all_players;

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
	summon_cnt = 0;
	fight_mon = 0;
	waitcmd = 0;
	invisible = 0;
//	p_market = 0;
	p_shop = 0;
	sell_cnt = 0;
	retry_times = 0;

	pkg_queue  = g_queue_new();

	memset(nick, 0, max_nick_size);
	memset(clothes_arr, 0, sizeof(clothes_base_t) * max_clothes_num);
	memset(summons, 0, sizeof(summons));
	fdsess = fdsession;
	
	TRACE_LOG("alloc player %u %p: fdsess=%p fd=%d", id, this, fdsess, fd);
}

Player::~Player()
{
	//save player info

	//leave_map(true);

    cached_pkg_t* pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(pkg_queue));
	while (pkg) {
		g_slice_free1(pkg->len, pkg);
		pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(pkg_queue));
	}

	if (p_market)
	{
		p_market->Player_Leave(this);
		p_market = 0;
	}

	g_queue_free(pkg_queue);
	all_players.erase(id);

	TRACE_LOG("dealloc player %u", id);
}

void
Player::walk(uint32_t x, uint32_t y, uint32_t flag)
{
	xpos = x;
	ypos = y;
	direction = 0;

	int idx = sizeof(tr_proto_t);
	idx += sizeof(cli_proto_t);
	pack(trpkgbuf, role_type, idx);
	pack(trpkgbuf, x, idx);
	pack(trpkgbuf, y, idx);
	pack(trpkgbuf, flag, idx);
	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, idx, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), id, cli_proto_walk, 0, idx-sizeof(tr_proto_t));
	p_market->send_to_market(0, trpkgbuf, idx, 0);

}

void
Player::walk_keyboard(uint32_t x, uint32_t y, uint8_t dir, uint8_t state)
{
	xpos = x;
	ypos = y;
	direction = dir;
	int idx = sizeof(tr_proto_t);
	idx += sizeof(cli_proto_t);
	pack(trpkgbuf, role_type, idx);
	pack(trpkgbuf, x, idx);
	pack(trpkgbuf, y, idx);
	pack(trpkgbuf, dir, idx);
	pack(trpkgbuf, state, idx);
	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, idx, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), id, cli_proto_walk_keyboard, 0, idx-sizeof(tr_proto_t));
	p_market->send_to_market(0, trpkgbuf, idx, 0);
}

void
Player::stand(uint32_t x, uint32_t y, uint8_t dir)
{
	xpos = x;
	ypos = y;
	direction = dir;
	TRACE_LOG("%u %u %u", id, x, y);
	int idx = sizeof(tr_proto_t);
	idx += sizeof(cli_proto_t);
	pack(trpkgbuf, x, idx);
	pack(trpkgbuf, y, idx);
	pack(trpkgbuf, dir, idx);
	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, idx, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), id, cli_proto_stand, 0, idx-sizeof(tr_proto_t));
	p_market->send_to_market(0, trpkgbuf, idx, 0);
}

void
Player::jump(uint32_t x, uint32_t y)
{
	xpos = x;
	ypos = y;
	int idx = sizeof(tr_proto_t);
	idx += sizeof(cli_proto_t);
	pack(trpkgbuf, x, idx);
	pack(trpkgbuf, y, idx);
	init_tr_proto_head_full(trpkgbuf, trd_transmit_only, idx, 0);
	init_cli_proto_head_full(trpkgbuf+sizeof(tr_proto_t), id, cli_proto_jump, 0, idx-sizeof(tr_proto_t));
	p_market->send_to_market(0, trpkgbuf, idx, 0);

}

void
Player::talk(uint8_t* msg, uint32_t msg_len, userid_t recvid)
{
	int idx = sizeof(tr_proto_t);

	pack(pkgbuf, msg, msg_len, idx);
	TRACE_LOG("%u %s %u %u %s", id, nick, recvid, msg_len, msg);

//	pack(pkgbuf, nick, sizeof(nick), idx);
//	pack(pkgbuf, recvid, idx);
//	pack(pkgbuf, msg_len, idx);
//	pack(pkgbuf, msg, msg_len, idx);

	init_tr_proto_head_full(pkgbuf, trd_transmit_only, idx, 0);
//	init_cli_proto_head_full(pkgbuf + sizeof(tr_proto_t), id, cli_proto_talk, 0, idx - sizeof(tr_proto_t));
	p_market->send_to_market(0, pkgbuf, idx, 0);
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

void
Player::init_player_info(player_info_t * p_info)
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
	summon_cnt = p_info->summon_cnt;
	memcpy(clothes_arr, p_info->clothes_arr, clothes_cnt * sizeof(clothes_base_t));

	summon_t* p_summon = reinterpret_cast<summon_t *>((uint8_t*)(p_info->clothes_arr) + clothes_cnt * sizeof(clothes_base_t));
	memcpy(summons, p_summon, sizeof(summon_t) * summon_cnt);
	set_fight_summon(p_info->fight_mon_tm);
	TRACE_LOG("init player info [%u ]", id);
}

void
Player::set_fight_summon(uint32_t mon_tm)
{
	for (uint32_t i = 0; i < summon_cnt; i++) {
		if (summons[i].mon_tm == mon_tm) {
			fight_mon = &(summons[i]);
			break;
		}
	}
}

void 
Player::pack_player_map_info(uint8_t * buf, int & idx, bool bHost)
{
	if (bHost) {
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
		for(uint32_t i = 0; i < clothes_cnt; ++i)  {
			pack(buf, clothes_arr[i].id, idx);
			pack(buf, clothes_arr[i].gettime, idx);
			pack(buf, clothes_arr[i].timelag, idx);
			pack(buf, clothes_arr[i].lv, idx);
		}
		uint16_t tmp_cnt = 0;
		if (!fight_mon) {
			tmp_cnt = 0;
			pack(buf, tmp_cnt, idx);
		} else {
			tmp_cnt = 1;
			pack(buf, tmp_cnt, idx);
			pack(buf, fight_mon->mon_type, idx); 
			pack(buf, fight_mon->lv, idx);
			pack(buf, fight_mon->nick, max_nick_size, idx);
			pack(buf, fight_mon->call_flag, idx);

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

			pack(buf, 0, idx); //summon skill count
		}
	}else {
		return; 
	}
}



