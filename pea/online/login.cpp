#include <libtaomee++/inet/pdumanip.hpp>

extern "C" 
{
#include <arpa/inet.h>
#include <libtaomee/timer.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/project/utilities.h>
}

#include "cli_proto.hpp"
#include "db_player.hpp"
#include "utils.hpp"
#include "login.hpp"
#include "item_bag.hpp"
#include "session.hpp"
#include "db_item.hpp"
#include "db_mail.hpp"
#include "db_pet.hpp"
#include "db_player.hpp"
#include "db_friends.hpp"
#include "task.hpp"
#include "db_task.hpp"
#include "prize.hpp"

using namespace taomee;
using namespace std;


/* ---------- CODE FOR cli_proto_login ---------*/
//

/* 登录命令 */
int cli_proto_login(DEFAULT_ARG)
{
	cli_proto_login_in * p_in = P_IN;
	cli_proto_login_out * p_out = P_OUT;

	player_t* new_p = add_player(p);

	new_p->role_tm = p_in->role_tm;
	new_p->server_id = p_in->server_id;
    memcpy(new_p->session, p_in->session, SESSION_LEN);


	uint32_t cliip = p->fdsess ? p->fdsess->remote_ip : 0;
	uint16_t cliport = p->fdsess ? p->fdsess->remote_port : 0;
	DEBUG_TLOG("LOGIN\t[u=%u, tm=%u, svrid=%u, cliip=0x%x:%hu]",
			p->id, p->role_tm, p->server_id, cliip, cliport);

    // 一般release版本加上session检查
#ifdef CHECK_SESSION
    return check_session(p);
#else
	return db_get_player(new_p);
#endif

}

int process_login(player_t * p)
{
    if (!p->check_module(MODULE_USER))
    {
        return db_get_player(p);
    }

    DEBUG_LOG("user: %u, user inited", p->id);

    if (!p->check_module(MODULE_BAG))
    {
        return db_get_player_items(p);
    }

    DEBUG_LOG("user: %u, bag inited", p->id);

    if (!p->check_module(MODULE_EQUIP))
    {
        return db_get_player_equips(p);
    }

    DEBUG_LOG("user: %u, equip inited", p->id);

    if (!p->check_module(MODULE_MAIL))
    {
        return db_mail_head_list(p);
    }

    DEBUG_LOG("user: %u, mail inited", p->id);

    if (!p->check_module(MODULE_PET))
    {
        return db_get_pet(p);
    }

    DEBUG_LOG("user: %u, pet inited", p->id);

    if (!p->check_module(MODULE_FRIEND))
    {
        return db_get_friends_list(p);
    }

    DEBUG_LOG("user: %u, friend inited", p->id);

    if (!p->check_module(MODULE_TASK))
    {
        return db_get_task_full_list(p, task_id_min, task_id_max);
    }

    DEBUG_LOG("user: %u, task inited", p->id);
  
    if (!p->check_module(MODULE_PRIZE))
    {
        return db_get_prize_list(p);
    }

    DEBUG_LOG("user: %u, prize inited", p->id);

    /*MYTEST(singku) 登录增加一个奖励*/
    //add_prize(p, 2001);

    return send_login_rsp(p);
}




int send_login_rsp(player_t* p)
{
	cli_proto_login_out out;

	out.uid                = p->id;
	out.exp                = p->get_player_attr_value(OBJ_ATTR_EXP);
	out.level              = p->get_player_attr_value(OBJ_ATTR_LEVEL);
	out.hp                 = p->get_player_attr_value(OBJ_ATTR_HP);
	out.magic              = p->get_player_attr_value(OBJ_ATTR_MAGIC);
	out.agility            = p->get_player_attr_value(OBJ_ATTR_AGILITY);
	out.physique           = p->get_player_attr_value(OBJ_ATTR_PHYSIQUE);
	out.luck               = p->get_player_attr_value(OBJ_ATTR_LUCK);
	out.atk                = p->get_player_attr_value(OBJ_ATTR_ATK);
	out.defence            = p->get_player_attr_value(OBJ_ATTR_DEF);
	out.double_hit         = p->get_player_attr_value(OBJ_ATTR_DOUBLE);
	out.max_bag_grid_count = p->bag->max_grid_count;
	out.gold               = p->get_player_attr_value(OBJ_ATTR_GOLD);
	out.map_id             = p->last_map_id;
	out.map_x              = p->last_map_x;
	out.map_y              = p->last_map_y;
    out.model.eye_model    = p->eye_model;
	out.model.resource_id  = p->resource_id;
    out.free_rand_gift     = p->extra_info->id2value(EXTRA_INFO_FREE_RAND_GIFT);
	strcpy(out.nick, p->nick);

	uint32_t cliip = p->fdsess ? p->fdsess->remote_ip : 0;
	uint16_t cliport = p->fdsess ? p->fdsess->remote_port : 0;
	DEBUG_TLOG("LOGIN SUCC\t[u=%u, tm=%u, svrid=%u, cliip=0x%x:%hu]",
			p->id, p->role_tm, p->server_id, cliip, cliport);
	return send_to_player(p, &out, p->waitcmd, 1);
}


