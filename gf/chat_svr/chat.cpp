#include <cerrno>
#include <cstdlib>
#include <string>
#include <vector>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/inet/byteswap.hpp>
#include <algorithm>
#include <assert.h>
extern "C" 
{
#include <fcntl.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
#include  <libtaomee/project/stat_agent/msglog.h>
#include <libtaomee/project/utilities.h>
}
#include "chat.hpp"
#include "player.hpp"
#include "group.hpp"
#include "utils.hpp"
using namespace std;
using namespace taomee;
uint8_t pkgbuf[1<<21];

ChatMrg* p_chat_mrg;


/* class ChatMrg function define */

ChatMrg::ChatMrg()
{
	init();
}

ChatMrg::~ChatMrg()
{
	final();
}

void ChatMrg::init()
{
	p_player_mrg = new PlayerMrg();
	p_talk_group_mrg = new TalkGroupMrg();
}

void ChatMrg::final()
{
	delete p_player_mrg;
	delete p_talk_group_mrg;
}

int ChatMrg::msg_dispatch(void* data, int len, fdsession_t* fdsess)
{
	svr_proto_t* head = reinterpret_cast<svr_proto_t*>(data);
	TRACE_LOG("%u %p", head->cmd, fdsess);
	switch (head->cmd) {
		case 40001: //report user on/off
			report_user_onoff(head->sender_id, fdsess, head->body, head->len - sizeof(svr_proto_t));
			break;
		case 40002: //chat cmd
			process_chat_msg(head->sender_id, head->body, head->len - sizeof(svr_proto_t));
			break;
		case 40003: //transmit only cmd
			transmit_msg(head->sender_id, head->body, head->len - sizeof(svr_proto_t));
			break;
		case 40004: //report user base info cmd
			report_user_base_info(head->sender_id, head->body, head->len - sizeof(svr_proto_t));
			break;
		default:
			break;
	}
	return 0;
}

int ChatMrg::report_user_base_info(uint32_t uid, void* body, int len)
{
	report_user_base_info_in_t*p_in = (report_user_base_info_in_t*)body;
	CHECK_VAL(len, sizeof(report_user_base_info_in_t));	
	Player* p = p_player_mrg->get_player(uid);
	if (!p) {
		return 0;
	}
	DEBUG_LOG("REPORT USER BASE INFO %u %u", uid, p_in->fight_team_id);
	if (p->get_fight_team_id()) {
		p_talk_group_mrg->del_talk_obj(talk_type_fight_team, p->get_fight_team_id(), uid);
	}

	p->set_fight_team_id(p_in->fight_team_id);	
	if (p->get_fight_team_id()) {
		add_player_to_fight_team_group(p->get_fight_team_id(), p);
	}

	return 0;

}
int ChatMrg::transmit_msg(uint32_t uid, void* body, int len) 
{
	transmit_str_in_t *p_in = (transmit_str_in_t*)body;
	CHECK_VAL(p_in->len, len - sizeof(transmit_str_in_t));	
	int idx = sizeof(svr_proto_t);
	pack(pkgbuf, (uint8_t*)body + sizeof(transmit_str_in_t), len - sizeof(transmit_str_in_t), idx);
	init_chat_proto_head_full(pkgbuf, 40003, idx, 0);
	p_talk_group_mrg->send_to_group(p_in->group_type, p_in->group_id, pkgbuf, idx);	
	return 0;
}

int ChatMrg::process_chat_msg(uint32_t uid, void* body, int len)
{
	//talk_msg_in_head_t* p_msg = reinterpret_cast<talk_msg_in_head_t*>(body);
	return 0;
}

int ChatMrg::report_user_onoff(uint32_t uid, fdsession_t* fdsess, void* body, int len)
{
	user_onoff_info_t *p_onoff_info = reinterpret_cast<user_onoff_info_t*>(body);
	DEBUG_LOG("report onoff %u %u %u %s %u", uid, p_onoff_info->onoff, p_onoff_info->user_tm, p_onoff_info->nick, p_onoff_info->fight_team_id);
	if (p_onoff_info->onoff) {
	//player on line
		Player* p = p_player_mrg->alloc_player(uid, this, fdsess);
		p->set_player_base_attr(p_onoff_info);

		if (p->get_fight_team_id()) {
			add_player_to_fight_team_group(p->get_fight_team_id(), p);
		}
	} else {
	//player off line
		p_player_mrg->del_player(uid);
	}
	return 0;
}

int ChatMrg::add_player_to_fight_team_group(uint32_t fight_team_id, Player* p)
{
	p_talk_group_mrg->add_talk_obj((uint32_t)talk_type_fight_team, fight_team_id, p->uid(), p);
	return 0;	
}



/**
  * @brief send a package to player p
  * @param p player to send a package to
  * @param pkgbuf package to send
  * @param len length of the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_to_player(Player* p, void* pkgbuf, uint32_t len, int completed)
{
	assert(len <= pkg_size);

	svr_proto_t* proto = reinterpret_cast<svr_proto_t*>(pkgbuf);

	proto->sender_id	= p->uid();
	proto->seq	= p ? p->uid() : 0;
	if (send_pkg_to_client(p->fdsess, pkgbuf, len) == -1) {
		ERROR_LOG("failed to send pkg to client: uid=%u cmd=%u", p->uid(), proto->cmd);
		return -1;
	}

	if (completed) {
		p->waitcmd = 0;
	}
	DEBUG_LOG("send to player %u cmd: %u len:%u err:%u", p->uid(), proto->cmd, len, proto->ret);
	return 0;
}


