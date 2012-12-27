/**
 *============================================================
 *  @file      common_op.hpp
 *  @brief    common operations such as walk, talk...
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_TEMPORARY_TEAM_HPP_
#define KF_TEMPORARY_TEAM_HPP_

#include <libtaomee++/inet/pdumanip.hpp>

#include "cli_proto.hpp"
#include "fwd_decl.hpp"
extern "C" {
#include <libtaomee/dataformatter/bin_str.h>
}

enum {
	max_temporary_team_member_number = 2,
};

enum {
	temporary_team_member_type_team_normal = 0,
	temporary_team_member_type_team_leader = 1,
};

enum {
	tmp_team_reply_accept = 0,
	tmp_team_reply_refuse = 1,	
	tmp_team_reply_btl = 2,	
	tmp_team_reply_trd = 3,	
	tmp_team_reply_home = 4,	
	tmp_team_reply_repeat = 5,	
	tmp_team_reply_client_config_refuse = 6,	
};
#pragma pack(1)

struct invite_player_2_tmp_team_in_t {
	uint32_t invitee_id;
	uint32_t invitee_tm;
};

struct reply_tmp_team_invite_in_t {
	uint32_t teamid;
	uint32_t inviter_id;
	uint32_t inviter_tm;
	uint32_t flag;// 0 accept  1 refuse
};
struct tmp_team_invite_notification_t {
	uint32_t inviter_id;
	uint32_t inviter_tm;
	char inviter_nick[max_nick_size];
	uint32_t temporary_team_id;
};

struct tmp_team_player_recv_self_leave_msg_t {
	uint32_t team_id;
	uint32_t leave_id;
	uint32_t leave_tm;
	uint32_t leave_type;
};

struct tmp_team_invite_recv_reply_t {
	uint32_t teamid;
	uint32_t invitee_id;
	uint32_t invitee_tm;
	uint32_t level;
	char inviter_nick[max_nick_size];
	uint32_t flag;
};

struct team_team_member_changed_t {
	uint32_t teamid;
	uint32_t cnt;
};

struct tmp_team_member_recv_pvp_invite_t {
	uint32_t teamid;
};

struct mp_team_member_recv_reply_pvp_invite_t {
	uint32_t teamid;
	uint32_t memberid;
	uint32_t membertm;
	uint32_t flag;
};

struct team_leader_recv_member_leave_msg_t {
	uint32_t teamid;
	uint32_t member_id;
	uint32_t member_tm;
	char nick[max_nick_size];
};

#pragma pack()

int invite_player_2_tmp_team_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int reply_tmp_team_invite_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int tmp_team_member_leave_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

int tmp_team_invite_pvp_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int tmp_team_reply_pvp_invite_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

uint32_t get_tmp_team_pvp_match_value(const player_t* p);

int on_invitee_recv_invite_msg(void* buf, uint32_t len);

int on_tmp_team_member_changed(void* buf, uint32_t len);

int on_tmp_team_member_recv_pvp_invite(void* buf, uint32_t len);

int on_tmp_team_member_recv_reply_pvp_invite(void* buf, uint32_t len);

int on_tmp_team_inviter_recv_reply_msg(void* buf, uint32_t len);

int on_tmp_team_recv_team_status_rsp(void* buf, uint32_t len);

int on_team_leader_recv_member_leave_msg(void* buf, uint32_t len);

int on_tmp_team_player_recv_self_leave_msg(void* buf, uint32_t len);

int on_tmp_team_recv_pvp_rsp(void* buf, uint32_t len);

void mcast_tmp_team_destroy_msg(player_t* from);

void mcast_tmp_team_pvp_rsp(player_t* p);

void destroy_temporary_team(player_t* p, bool report_flg = true);


uint32_t alloc_new_temporay_team_id();

bool is_player_have_temporary_team(player_t* p);

bool is_player_temporary_team_leader(player_t* p);

uint32_t get_temporary_team_leader(player_t* p);

bool is_temporary_team_have_member(player_t* p, uint32_t member_id, uint32_t member_tm);

temporary_team_member_t* get_tmp_team_leader(const player_t* p);

void mcast_tmp_team_member_changed_msg(const player_t* from, uint32_t teamid, bool destroy_flag = false);

void send_tmp_team_member_leave_msg_2_leader(userid_t to, const player_t* from, 
	uint32_t teamid, uint32_t memberid, uint32_t membertm, char* nick);

void player_leave_temporary_team(player_t* p);

void send_player_leave_tmp_team_rsp(player_t* from, uint32_t leave_id, uint32_t leave_tm, uint32_t leave_type);

bool judge_player_catch_2v2_rewards(player_t* p, uint32_t swap_id);
bool do_2v2_title(player_t* p, uint32_t swap_id);
#endif
