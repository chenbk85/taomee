/**
 *============================================================
 *  @file      NormalAI.cpp
 *  @brief    all states of a player are defined here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */
 
extern "C" {
#include <libtaomee/log.h>
}

#include <libtaomee++/random/random.hpp>
#include <libtaomee++/inet/byteswap.hpp>

#include "MonsterAI.hpp"
#include "NormalAI.hpp"
#include "../player.hpp"
#include "../stage.hpp"

using namespace taomee;

void
NormalAI::wait(Player* player, int millisec)
{
	//TRACE_LOG("wait state: uid=%u", player->id);
	if ( !(player->i_ai->target_pos.is_zero()) ) {
		player->i_ai->set_target_pos(player->i_ai->target_pos);
	}
	
	map_t*  m = player->cur_map;
	Player* target = 0;
	int dist = 2000000000;

	for (PlayerSet::iterator it = m->players.begin(); it != m->players.end(); ++it) {
		Player* p = *it;
		if (p->in_battle()) {
			int tmp = player->pos().squared_distance(p->pos());
			if (tmp < dist) {
				target = p;
				dist   = tmp;
			}
		}
	}

	// target found and we change the state of the monster to 'MoveState'
	if (dist < player->mon_info->visual_field) {
		player->i_ai->target = target;
		// attack if attackable
		if (player->select_skill(target)) {
			//player->state_machine->change_state(AttackState::instance());
			player->i_ai->change_state(ATTACK_STATE);
			return;
		}

		// move to an attackable position
		if (player->mon_info->spd) {
			attack_region_t rgn = player->get_attack_region();
			if ((rgn.x != 0) && (rgn.y != 0)) {
				//player->state_machine->change_state(MoveState::instance());
				player->i_ai->change_state(MOVE_STATE);
				return;
			}
		}
	}
}

void 
NormalAI::linger(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
NormalAI::move(Player* player, int millisec)
{
	//calc target pos
	if (player->i_ai->target_pos.is_zero()) {
		calc_target_pos(player);
	}
	
	const Vector3D& curpos = player->pos();
	Vector3D& topos  = player->i_ai->target_pos;

	const Player* target = player->i_ai->target;

	Vector3D pos;
	Vector3D s = player->velocity() * millisec / 1000;
	if (curpos.squared_distance(topos) > s.squared_length()) {
		pos = curpos + s;
	} else {
		pos = topos;
	}

	TRACE_LOG("monster move: uid=%u tm=%d v=%s pos=%s %s", player->id, millisec,
				player->velocity().to_string().c_str(),	player->pos().to_string().c_str(), pos.to_string().c_str());

	player->adjust_position_ex(pos);

	uint16_t* plag;
	int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
	pack(pkgbuf, pos.x(), idx);
	pack(pkgbuf, pos.y(), idx);
	pack(pkgbuf, pos.z(), idx);
	pack(pkgbuf, player->dir, idx);
	pack(pkgbuf, static_cast<uint8_t>(2), idx);

	plag = reinterpret_cast<uint16_t*>(pkgbuf + idx);
	idx += sizeof(*plag);

	init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), player->id, cli_proto_npc_move, 0, idx - sizeof(btl_proto_t));
	init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

	for (PlayerSet::const_iterator it = player->cur_map->players.begin(); it != player->cur_map->players.end(); ++it) {
		Player* p = *it;
		*plag = bswap(static_cast<uint16_t>(player->lag + p->lag));

		send_to_player(p, pkgbuf, idx, 0);
	}

	Vector3D target_pos;
	if (player->i_ai->target_path.size()) {
		target_pos = Vector3D(player->i_ai->target_path[0].x, player->i_ai->target_path[0].y);
	} else {
		target_pos = topos;
	}
	if (target_pos.squared_distance(target->pos()) > (250 * 250)) {
		calc_target_pos(player);
		return;
	}

	if (pos == topos && player->i_ai->target_path.size()) {
		KfAstar::Point& pt = player->i_ai->target_path.back();
		player->i_ai->set_target_pos(Vector3D(pt.x, pt.y));
		player->i_ai->target_path.pop_back();
		return;
	}

	if (pos == topos) {
		if ((player->dir == dir_left) && (target->pos().x() > (pos.x() + player->mon_info->len / 4))) {
			player->i_ai->set_target_pos(Vector3D(topos.x() + player->mon_info->len / 4, topos.y()));
		} else if ((player->dir == dir_right) && ((target->pos().x() + player->mon_info->len / 4) < pos.x())) {
			player->i_ai->set_target_pos(Vector3D(topos.x() - player->mon_info->len / 4, topos.y()));
		} else if (player->select_skill(target)) {
			//player->state_machine->change_state(AttackState::instance());
			//stand(player);
			monster_stand(player);
			player->i_ai->target_pos.init();
			player->i_ai->change_state(ATTACK_STATE);
		} else {
			monster_stand(player);
			calc_target_pos(player);
		}
	}
}

void
NormalAI::evade(Player* player, int millisec)
{
	player->i_ai->change_state(WAIT_STATE);
}

void
NormalAI::attack(Player* player, int millisec)
{
	player_skill_t* skill = player->select_skill(player->i_ai->target);
	if (skill) {
		int idx = sizeof(btl_proto_t) + sizeof(cli_proto_t);
		pack(pkgbuf, static_cast<uint32_t>(player->team), idx);
		pack(pkgbuf, skill->skill_id, idx);
		pack(pkgbuf, skill->lv, idx);
		pack(pkgbuf, player->pos().x(), idx);
		pack(pkgbuf, player->pos().y(), idx);
		pack(pkgbuf, player->pos().z(), idx);
		init_cli_proto_head_full(pkgbuf + sizeof(btl_proto_t), player->id, cli_proto_player_attack, 0, idx - sizeof(btl_proto_t));
		init_btl_proto_head(pkgbuf, btl_transmit_only, idx);

		player->cur_map->send_to_map(pkgbuf, idx);

		player->launch_skill(skill);
	} else {
		//player->state_machine->change_state(WaitState::instance());
		player->i_ai->change_state(WAIT_STATE);
	}
}

void
NormalAI::stuck(Player* player, int millisec)
{
	if (timecmp(*get_now_tv(), player->stuck_end_tm) > 0) {
		//player->state_machine->change_state(WaitState::instance());
		player->i_ai->change_state(WAIT_STATE);
	}
}

