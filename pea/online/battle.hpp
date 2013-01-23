#ifndef BATTLE_H_
#define BATTLE_H_

#include <libtaomee++/bitmanip/bitmanip.hpp>

extern "C" 
{
#include <libtaomee/list.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/project/types.h>
#include <async_serv/dll.h>
}

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "cli_proto.hpp"

class player_t;

extern  int battle_fds[];

typedef struct battle_grp_t
{
	uint32_t id;
	uint32_t fd_idx;
}battle_grp_t;

bool is_alloc_player_cmd(uint32_t cmd);//该函数战斗服务器也有一个

void init_btl_proto_head(const player_t* p, userid_t id, void* header, int len, uint32_t cmd);

int send_to_battle_svr(player_t* p, const void* btlpkgbuf, uint32_t len, int index);
int send_to_battle_svr(player_t* p, Cmessage *p_out, uint16_t cmd, int index);

battle_grp_t* alloc_battle_grp();

void free_battle_grp(battle_grp_t* grp);

bool init_battle_server_config(const char* xml);

bool final_battle_server_config();

int  get_btlsvr_fd_idx(int fd);

void do_while_battle_svr_crashed(int btl_fd);

bool init_btl_proto_handles();

bool final_btl_proto_handles();

void handle_battle_svr_return(btl_proto_t* data, uint32_t len, int bs_fd);

bool init_player_btl_grp(player_t* p);

bool final_player_btl_grp(player_t* p);

int btlsvr_init_battle(player_t* p, uint32_t stage, uint32_t player_count, uint32_t cmd);

int btlsvr_create_btl_callback(player_t* p, btl_proto_t* bpkg);

int btlsvr_transmit_only_callback(player_t* p, btl_proto_t* pkg);

int btlsvr_battle_end_callback(player_t* p, btl_proto_t* pkg);

int btlsvr_syn_player_info(player_t* p, uint32_t battle_id, uint32_t fd_idx);

int btlsvr_load_resource(player_t* p);

int btlsvr_get_btl_user_list(player_t* p);

int btlsvr_player_move(player_t* p, uint32_t x, uint32_t y, uint32_t dir, uint32_t angle, uint32_t gun_angle);

int btlsvr_player_attack(player_t* p, uint32_t gun_angle, uint32_t muzzle_velocity);

int btlsvr_player_read_attack(player_t* p);

int btlsvr_player_cancel_battle(player_t* p);

int btlsvr_player_progress_bar(player_t* p,  uint32_t progress);

int btlsvr_player_cancel_turn(player_t* p);

int btlsvr_player_finish_turn(player_t* p);

int btlsvr_select_skill(player_t* p, uint32_t skill_id, uint32_t skill_lv);

int btlsvr_get_player_skill_list(player_t* p);
#endif
