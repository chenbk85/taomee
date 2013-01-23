#include "player.hpp"
#include "proto.hpp"
#include "cli_proto.hpp"
#include "battle.hpp"


/* ---------- CODE FOR cli_proto_player_move ---------*/
//

/* 玩家移动命令 */
int cli_proto_btl_player_move(DEFAULT_ARG)
{
	cli_proto_btl_player_move_in * p_in = P_IN;
	cli_proto_btl_player_move_out * p_out = P_OUT;


	p->clear_waitcmd();
	if(p->check_in_battle())
	{
		return btlsvr_player_move(p, p_in->x, p_in->y, p_in->dir, p_in->angle, p_in->gun_angle);
	}
	return 0;
}


/* ---------- CODE FOR cli_proto_player_attack ---------*/
//

/* 玩家攻击命令 */
int cli_proto_player_attack(DEFAULT_ARG)
{
	cli_proto_player_attack_in * p_in = P_IN;
	cli_proto_player_attack_out * p_out = P_OUT;

	p->clear_waitcmd();
	if(p->check_in_battle())
	{
		btlsvr_player_attack(p, p_in->angle, p_in->muzzle_velocity);	
	}
	return 0;
}


int cli_proto_player_ready_attack(DEFAULT_ARG)
{
	p->clear_waitcmd();
	if(p->check_in_battle())
	{
		btlsvr_player_read_attack(p);
	}
	return 0;
}

/* ---------- CODE FOR cli_proto_get_btl_user_list ---------*/
//

/* 拉取战斗中玩家信息 */
int cli_proto_get_btl_user_list(DEFAULT_ARG)
{
	p->clear_waitcmd();
	if(!p->check_in_battle())
	{
		return 0;
	}
	return btlsvr_get_btl_user_list(p);
}

/* ---------- CODE FOR cli_proto_load_resource ---------*/
//

/* 通知客户端开始加载资源 */
int cli_proto_load_resource(DEFAULT_ARG)
{
	p->clear_waitcmd();
	if (!p->check_in_battle())
	{
		return 0;	
	}
	return btlsvr_load_resource(p);
}

int cli_proto_progress_bar(DEFAULT_ARG)
{
	/* TODO(zog): 遇到 100% 直接转发, <100%的, 看跟上次时间间隔, 小于 1 秒的, 就直接丢掉 */
	cli_proto_progress_bar_in* p_in = P_IN;

	p->clear_waitcmd();
	if(!p->check_in_battle())return 0;
	
	return btlsvr_player_progress_bar(p, p_in->progress); 
}

int cli_proto_player_cancel_turn(DEFAULT_ARG)
{
	p->clear_waitcmd();
	if(!p->check_in_battle())return 0;
	return btlsvr_player_cancel_turn(p);	
}

int cli_proto_select_skill(DEFAULT_ARG)
{
	cli_proto_select_skill_in * p_in = P_IN;
	p->clear_waitcmd();
	if(!p->check_in_battle())return 0;
	return btlsvr_select_skill(p, p_in->skill_id, p_in->skill_lv);
}

int cli_proto_get_btl_player_skill_list(DEFAULT_ARG)
{
	p->clear_waitcmd();
	if(!p->check_in_battle())return 0;
	return btlsvr_get_player_skill_list(p);
}

int cli_proto_player_finish_turn(DEFAULT_ARG)
{
	p->clear_waitcmd();
	if(!p->check_in_battle())return 0;
	return btlsvr_player_finish_turn(p);
}
