#include <cstdio>

using namespace std;

#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include <kf/home.hpp>

using namespace taomee;

extern "C" {
#include <glib.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>

#include <async_serv/net_if.h>
}

#include "common_op.hpp"
#include "player.hpp"
#include "home_impl.hpp"
#include "home.hpp"
#include "global_data.hpp"
#include "summon_monster.hpp"
#include "stat_log.hpp"
//---------------------------------------------------------
struct leave_home_rsp_t
{
	uint32_t home_owner_id;
	uint32_t role_regtime;
};

struct enter_other_home_rsp_t
{
	uint32_t err_code;
	uint32_t home_owner_id;
	uint32_t role_regtime;
};

struct enter_home_rsp_t
{
	uint32_t home_owner_id;
	uint32_t role_regtime;
};

struct pet_call_rsp_t
{
	uint32_t pet_type;
	uint32_t pet_tm;
	uint32_t pet_state;
};

struct pick_fruit_rsp_t {
	uint32_t field_id;
	uint32_t fruit_id;
	uint32_t fruit_cnt;
}__attribute__((packed));

struct set_plant_status_rsp_t {
	uint32_t field_id;
	uint32_t plant_id;
	uint32_t plant_tm;
	uint32_t type;
	uint32_t item_id;
}__attribute__((packed));

struct pick_fruit_rsp_item_t {
	uint32_t type;
	uint32_t id;
	uint32_t count;
}__attribute__((packed));


struct pve_syn_home_info_rsp_t{
	uint32_t count;
	uint32_t pets[];
}__attribute__((packed));

/*
struct kick_off_guest_rsp_t{
	uint32_t user_id;
	uint32_t role_regtime;
}__attribute__((packed));
*/

struct sow_seed_rsp_t {
	uint32_t field_id;
	uint32_t plant_id;
	uint32_t plant_tm;
	uint32_t seed_id;
}__attribute__((packed));

struct level_up_rsp_t{
	uint32_t level;
	uint32_t exp;
	uint32_t fumo_point;
}__attribute__((packed));

struct add_decorate_lv_t {
    uint32_t    flag;
    uint32_t    decorate_id;
    uint32_t    decorate_tm;
    uint32_t    decorate_lv;
    uint32_t    status;
    uint32_t    itemcnt;
    uint32_t    itemarr[];
}__attribute__((packed));

struct get_decorate_reward_t {
    uint32_t    decorate_id;
    uint32_t    decorate_tm;
    uint32_t    decorate_lv;
    uint32_t    status;
}__attribute__((packed));

/**
  * @brief send a request to home svr
  * @param p the player who launches the request to home server
  * @param homepkgbuf body of the request
  * @param len length of homepkgbuf
  * @return 0 on success, -1 on error
  */
int send_to_home_svr(player_t* p, const void* homepkgbuf, uint32_t len, int index)
{
	if (home_fds[index] == -1) {
		home_fds[index] = connect_to_service(home_svrs[index].svr_name, home_svrs[index].id, 65535, 1); 
	}

	if ((home_fds[index] == -1) || (len > homeproto_max_len)) {
		ERROR_LOG("send to home server failed: fd=%d len=%d", home_fds[index], len);

		return send_header_to_player(p, cli_proto_leave_home, cli_err_home_system_err, 1);
	}

    return net_send(home_fds[index], homepkgbuf, len);
}

/**
  * @brief set home server protocol handle
  */
#define SET_HOME_HANDLE(op_, len_, cmp_) \
		do { \
			if (home_handles[home_ ## op_ - home_cmd_start].hdlr != 0) { \
				ERROR_LOG("duplicate cmd=%u name=%s", home_ ## op_, #op_); \
				return false; \
			} \
			home_handles[home_ ## op_ - home_cmd_start] = homesvr_handle_t(homesvr_ ## op_ ## _callback, len_, cmp_); \
		} while (0)

/**
  * @brief init handles to handle protocol packages from home server
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_home_proto_handles()
{
	/* operation, bodylen, compare method */
	//SET_BTL_HANDLE(cancel_home, 0, cmp_must_eq);
	SET_HOME_HANDLE(enter_other_home, sizeof(enter_other_home_rsp_t), cmp_must_eq);
	SET_HOME_HANDLE(enter_home, sizeof(enter_home_rsp_t), cmp_must_eq);	
	SET_HOME_HANDLE(leave_home, sizeof(leave_home_rsp_t), cmp_must_eq);
	SET_HOME_HANDLE(pick_fruit, sizeof(pick_fruit_rsp_t), cmp_must_ge);
	SET_HOME_HANDLE(set_plant_status, sizeof(set_plant_status_rsp_t), cmp_must_eq);
	SET_HOME_HANDLE(pve_syn_home_info, sizeof(pve_syn_home_info_rsp_t), cmp_must_ge);
	//SET_HOME_HANDLE(kick_off_player, sizeof(kick_off_guest_rsp_t), cmp_must_eq);

	SET_HOME_HANDLE(sow_plant_seed, sizeof(sow_seed_rsp_t), cmp_must_eq);
	
	SET_HOME_HANDLE(level_up, sizeof(level_up_rsp_t), cmp_must_eq);	

	SET_HOME_HANDLE(add_decorate_lv, sizeof(add_decorate_lv_t), cmp_must_ge);	
	SET_HOME_HANDLE(get_decorate_reward, sizeof(get_decorate_reward_t), cmp_must_eq);	

	SET_HOME_HANDLE(transmit_only, sizeof(cli_proto_t), cmp_must_ge);
	return true;
}

/**
  * @brief uniform handler to process packages from home server
  * @param data package data
  * @param len package length
  * @param hs_fd home server fd
  */
void handle_home_svr_return(home_proto_t* data, uint32_t len, int hs_fd)
{
    if (data->cmd == home_query_player_count) {
        player_t* p = get_player(data->id);
        if( p == NULL){
            ERROR_LOG("home return error: uid=%u cmd=%u ",data->id, data->cmd);
            return ;
        }
        homesvr_query_home_player_count_callback(p, data);
        return ;
    }

	player_t* p = get_player(data->id);
	if( p == NULL || p->home_grp == NULL || home_fds[p->home_grp->fd_idx] != hs_fd){
		ERROR_LOG("home return error: uid=%u cmd=%u fd=%u",data->id, data->cmd, hs_fd);
		return ;
	}


	if (data->ret) {
		// some error code needs to be handled specially
		if (p->waitcmd) {
			send_header_to_player(p, p->waitcmd, data->ret, 1);
		}
		return;
	}

	int err = -1;
	int cmd = data->cmd - home_cmd_start;
	uint32_t bodylen = data->len - sizeof(home_proto_t);

	if( !(cmd >= 0 && cmd<(int)(sizeof(home_handles)/sizeof(home_handles[0]))))
	{
		err = 1;
		goto ret;
	}

	if(home_handles[cmd].hdlr == NULL)
	{
		err = 1;
		goto ret;
	}

	if ( ((home_handles[cmd].cmp_method == cmp_must_eq) && (bodylen != home_handles[cmd].len))
			|| ((home_handles[cmd].cmp_method == cmp_must_ge) && (bodylen < home_handles[cmd].len)) ) {
		if ((bodylen != 0) || (data->ret == 0)) {
			ERROR_LOG("invalid package len=%u %u cmpmethod=%d ret=%u uid=%u %u",
						bodylen, home_handles[cmd].len,
						home_handles[cmd].cmp_method, data->ret, data->id, cmd);
			goto ret;
		}
	}

	err = home_handles[cmd].hdlr(p, data);

ret:
	if (err) {
		close_client_conn(p->fd);
	}
}

//-------------------------------------------------------------------
/**
 * @brief load home servers' config from homesvr.xml file
 * @param cur the pointer to xml root node
 * @return 0 on success, -1 on error
 */
int load_home_servers(xmlNodePtr cur)
{
	home_svr_cnt = 0;
	memset(home_svrs, 0, sizeof(home_svrs));

	cur = cur->xmlChildrenNode; /* goto <Server> */
	while (cur) {
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("Server"))) {
			if (home_svr_cnt >= max_home_svr_num) {
				ERROR_LOG("Too many home servers: cnt=%d", home_svr_cnt);
				return -1;
			}

			string svr_name;
			get_xml_prop(svr_name, cur, "SvrName");
			if (svr_name.length() >= 16) {
				ERROR_LOG("Invalid service name: %s idx=%d",
							svr_name.c_str(), home_svr_cnt);
				return -1;
			}

			int16_t	svr_id;
			get_xml_prop(svr_id, cur, "SvrId");
			if (svr_id == 0) {
				ERROR_LOG("Invalid svr_id: %d idx=%d", svr_id, home_svr_cnt);
				return -1;
			}
			strncpy(home_svrs[home_svr_cnt].svr_name, svr_name.c_str(), sizeof(home_svrs[home_svr_cnt].svr_name));
			home_svrs[home_svr_cnt].id = svr_id;

			++home_svr_cnt;
		}
		cur = cur->next;
	}

	return 0;
}

/**
 * @brief reload home svr config 
 */
void reload_home_svr_config()
{
	home_server_t former_svrs[max_home_svr_num];
	memcpy(former_svrs, home_svrs, sizeof(home_svrs));

	try {
		load_xmlconf("./conf/homesvr.xml", load_home_servers);
	} catch (const exception& e) {
		INFO_LOG("Fail to Reload 'homesvr.xml'. Err: %s", e.what());
		return;
	}

	for (size_t i = 0; i < array_elem_num(home_fds); ++i) {
		if ( strcmp(former_svrs[i].svr_name, home_svrs[i].svr_name)
				|| (former_svrs[i].id != home_svrs[i].id) ) {
			KDEBUG_LOG(0, "RELOAD BTL SVR\t[idx=%llu name=%s %s id=%d %d]",
						static_cast<unsigned long long>(i),
						home_svrs[i].svr_name, former_svrs[i].svr_name,
						home_svrs[i].id, former_svrs[i].id);
			close(home_fds[i]);
			home_fds[i] = -1;
		}
	}
}

/**
  * @brief get index of a home server identified by the given 'fd'
  * @param fd fd of a home server
  * @return index of a home server, or -1 if home server identified by 'fd' is not found
  */
int get_homesvr_fd_idx(int fd)
{
	for (int i = 0; i < home_svr_cnt; ++i) {
		if (fd == home_fds[i]) {
			return i;
		}
	}
	return -1;
}


void enter_own_home(player_t* p)
{
	if( check_in_home(p))return;
	if( check_enter_home(p))return;
	if( check_in_trade(p))return ;
	create_home(p, p->id, p->role_tm);
}

void enter_other_home(player_t* p, uint32_t home_owner_id, uint32_t role_regtime)
{
	if( check_in_home(p))return;
	if( check_enter_home(p))return;
	if( check_in_trade(p))return ;
	create_home(p, home_owner_id, role_regtime);
}

void leave_home(player_t* p)
{
	KDEBUG_LOG(p->id, "L H\t[%u]", p->home_grp->home_owner_id);
	p->home_grp->home_owner_id = 0;
	p->home_grp->fd_idx = 0;
	free_home_grp(p->home_grp, p);
	p->home_grp = NULL;
}

bool check_in_home(player_t* p)
{
	return p->home_grp != NULL;	
}

bool check_in_trade(player_t* p)
{
	return p->trade_grp != NULL;
}

bool check_enter_home(player_t* p)
{
	return p->home_grp != NULL;
}

bool change_home(player_t* p, uint32_t home_owner_id, uint32_t role_regtime)
{
	return true;
}

bool create_home(player_t* p, uint32_t home_owner_id, uint32_t role_regtime)
{
	if(p->home_grp != NULL)return false;
	p->home_grp = alloc_home_grp();
	p->home_grp->home_owner_id = home_owner_id;
	p->home_grp->role_regtime = role_regtime;
	p->home_grp->fd_idx = home_owner_id % home_svr_cnt;
	p->home_grp->map_id = p->cur_map->id;
	KDEBUG_LOG(p->id, "E H\t[%u]", p->home_grp->home_owner_id);
	return true;
}

int homesvr_enter_own_home(player_t* p)
{
	int idx = sizeof(home_proto_t);
	uint32_t home_owner_id  = p->id;
	uint32_t role_regtime = p->role_tm;

	pack_home_player_info(homepkgbuf, idx,  p, home_owner_id ,role_regtime);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_enter_home);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_enter_other_home(player_t* p, uint32_t home_owner_id, uint32_t role_regtime)
{
	int idx = sizeof(home_proto_t);
	pack_home_player_info(homepkgbuf, idx,  p, home_owner_id, role_regtime);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_enter_other_home);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_leave_home(player_t* p, uint32_t home_owner_id, uint32_t role_regtime)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, home_owner_id, idx);
    pack_h(homepkgbuf, role_regtime, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_leave_home);
    return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);	
}

int  homesvr_pet_move(player_t* p, uint32_t pet_tm, uint32_t x, uint32_t y, uint32_t dir)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, pet_tm, idx);
	pack_h(homepkgbuf, x, idx);
	pack_h(homepkgbuf, y, idx);
	pack_h(homepkgbuf, dir, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_pet_move);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}


home_grp_t* get_home_grp(player_t* p)
{
	return p->home_grp;
}

void  pack_home_player_info(uint8_t* buf, int32_t& idx, player_t* p, uint32_t home_owner_id, uint32_t role_regtime)
{
	pack_h(buf, home_owner_id, idx);
	pack_h(buf, role_regtime, idx);
	pack_h(buf, p->id,idx);
	pack_h(buf, p->role_tm, idx);
	pack_h(buf, p->role_type, idx);
	pack_h(buf, p->power_user, idx);
	pack_h(buf, p->player_show_state, idx);
	pack_h(buf, p->vip, idx);
	pack_h(buf, p->vip_level, idx);
	pack_h(buf, p->using_achieve_title, idx);
	pack(buf, p->nick, max_nick_size, idx);
	pack_h(buf, p->lv, idx);
	pack_h(buf, p->flag, idx);
	pack_h(buf, p->app_mon, idx);
	pack_h(buf, p->honor, idx);
	pack_h(buf, p->coins, idx);
	pack_h(buf, p->xpos, idx);
	pack_h(buf, p->ypos, idx);
	pack_h(buf, p->clothes_num, idx);
	pack_h(buf, p->summon_mon_num, idx);
	pack_h(buf, p->fight_summon ? p->fight_summon->mon_tm : 0, idx);
	for(uint32_t i = 0; i < p->clothes_num; ++i)
	{
		pack_h(buf, p->clothes[i].clothes_id, idx);
		pack_h(buf, p->clothes[i].gettime, idx);
		pack_h(buf, p->clothes[i].timelag, idx);
		pack_h(buf, static_cast<uint32_t>(p->clothes[i].lv), idx);
	}
	for (uint32_t i = 0; i < p->summon_mon_num; ++i)
	{
		pack_h(buf, p->summons[i].mon_type, idx);
		pack_h(buf, p->summons[i].mon_tm, idx);
		pack_h(buf, p->summons[i].lv, idx);
		pack(buf, p->summons[i].nick, max_nick_size, idx);
		pack_h(buf, p->summons[i].call_flag, idx);
	}
}

int enter_own_home_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if(check_in_home(p)){
		return send_header_to_player(p, p->waitcmd, home_err_can_not_enter, 1);
	}	
	if(check_enter_home(p)){
		return send_header_to_player(p, p->waitcmd, home_err_can_not_enter, 1);
	}
	if(check_in_trade(p)){
		return send_header_to_player(p, p->waitcmd, home_err_can_not_enter, 1);
	}
	enter_own_home(p);
	do_stat_log_universal_interface_2(0x09819001, 0, p->id, 1);
	return homesvr_enter_own_home(p);
}

int enter_other_home_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t home_owner_id = 0;
	uint32_t role_regtime = 0;
	unpack(body, home_owner_id, idx);
	unpack(body, role_regtime, idx);

	if(p->id == home_owner_id){
		return send_header_to_player(p, p->waitcmd, home_err_can_not_enter, 1);
	}

	if(check_in_home(p)){
		return send_header_to_player(p, p->waitcmd, home_err_can_not_enter, 1);
	}
	if(check_enter_home(p)){
		return send_header_to_player(p, p->waitcmd, home_err_can_not_enter, 1);
	}
	if(check_in_trade(p)){
		return send_header_to_player(p, p->waitcmd, home_err_can_not_enter, 1);
	}

	enter_other_home(p, home_owner_id, role_regtime);
	return homesvr_enter_other_home(p, home_owner_id, role_regtime);
}


int enter_other_home_random_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	player_t* random_player[10];
	uint32_t player_cnt  = get_random_player_list_by_lv(p, random_player);

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, player_cnt, idx);
	for (uint32_t i = 0; i < player_cnt; i++) {
		pack(pkgbuf, random_player[i]->id, idx);
		pack(pkgbuf, random_player[i]->role_tm, idx);
		pack(pkgbuf, random_player[i]->role_type, idx);
		pack(pkgbuf, (uint32_t)(random_player[i]->lv), idx);
		pack(pkgbuf, random_player[i]->nick, sizeof(random_player[i]->nick),idx);

		TRACE_LOG("%u %u %u %u %s", random_player[i]->id, random_player[i]->role_tm, random_player[i]->role_type,
			random_player[i]->lv, random_player[i]->nick);
	}
	KDEBUG_LOG(p->id, "ENTER RANDOM HOME\t[%u]", player_cnt);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int leave_home_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{	
	if(!check_in_home(p)){
		return send_header_to_player(p, p->waitcmd,  home_err_can_not_leave, 1);
	}
	uint32_t home_owner_id  = 0;
	uint32_t role_regtime = 0;

	memcpy(p->tmp_session, body, bodylen);
	p->sesslen = bodylen;
	
	home_grp_t* p_grp = get_home_grp(p);
	if(p_grp)
	{
		home_owner_id = p_grp->home_owner_id;
		role_regtime  = p_grp->role_regtime;
	}
	return homesvr_leave_home(p, home_owner_id, role_regtime);	
}

int homesvr_enter_other_home_callback(player_t* p, home_proto_t* hpkg)
{
	enter_other_home_rsp_t* rsp = reinterpret_cast<enter_other_home_rsp_t*>(hpkg->body);	
	if(rsp->home_owner_id == 0 || rsp->role_regtime == 0 || rsp->err_code){
		leave_home(p);	
		return send_header_to_player(p, p->waitcmd, rsp->err_code, 1);
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->home_owner_id, idx);
	pack(pkgbuf, rsp->role_regtime, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int homesvr_enter_home_callback(player_t* p, home_proto_t* hpkg)
{
	enter_home_rsp_t* rsp = reinterpret_cast<enter_home_rsp_t*>(hpkg->body);
	if(rsp->home_owner_id == 0 || rsp->role_regtime == 0){
		leave_home(p);
		return send_header_to_player(p, p->waitcmd, home_err_can_not_enter, 1);
	}
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

int homesvr_leave_home_callback(player_t* p, home_proto_t* hpkg)
{
	leave_home_rsp_t* rsp = reinterpret_cast<leave_home_rsp_t*>(hpkg->body);
	if(rsp->home_owner_id == 0 || rsp->role_regtime == 0){
		return send_header_to_player(p, p->waitcmd, home_err_can_not_leave, 1);
	}

	uint32_t pre_map_id = p->home_grp->map_id;
	leave_home(p);
	p->invisible = 0;

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, pre_map_id, idx);
	
	memcpy(pkgbuf + idx, p->tmp_session, p->sesslen);
	idx += p->sesslen;
	p->sesslen = 0;
	

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int homesvr_transmit_only_callback(player_t* p, home_proto_t* pkg)
{
	return send_to_player(p, pkg->body, pkg->len - sizeof(home_proto_t), 0);
}


int pet_move_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t pet_tm = 0;
	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t dir = 0;

	unpack(body, pet_tm, idx);
	unpack(body, x, idx);
	unpack(body, y, idx);
	unpack(body, dir, idx);

	if(!check_in_home(p)){
		return send_header_to_player(p, p->waitcmd, home_err_not_in_home, 1);
	}


	summon_mon_t* p_pet = get_player_summon(p, pet_tm);
	if(p_pet == NULL){
		return send_header_to_player(p, p->waitcmd, cli_err_no_summon_mon, 1);
	}
	
	p->waitcmd = 0;
	return homesvr_pet_move(p, pet_tm, x, y, dir);
}


int pet_home_call_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t pet_tm =0;
	uint32_t state = 0;
	unpack(body, pet_tm, idx);
	unpack(body, state, idx);

	if(!check_in_home(p)){
		return send_header_to_player(p, p->waitcmd, home_err_not_in_home, 1);
	}


	if( !(state == 0 || state == 3)){
		return send_header_to_player(p, p->waitcmd, cli_err_no_summon_mon, 1);
	}

	if(p->fight_summon && p->fight_summon->mon_tm == pet_tm){
		return send_header_to_player(p, p->waitcmd, cli_err_no_summon_mon, 1);
	}

	summon_mon_t* p_pet = get_player_summon(p, pet_tm);
	if(p_pet == NULL){
		return send_header_to_player(p, p->waitcmd, cli_err_no_summon_mon, 1);
	}
	p_pet->call_flag = state;
    db_set_fight_summon(p, p_pet->mon_tm, state);	 
	
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, pet_tm, idx);
	pack(pkgbuf, state, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	send_to_player(p, pkgbuf, idx, 1);

	return homesvr_pet_home_call(p, pet_tm, state);
}

int homesvr_pet_home_call(player_t*p , uint32_t pet_tm, uint32_t pet_state)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, pet_tm, idx);
	pack_h(homepkgbuf, pet_state, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx,  home_pet_call);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}
/*
int homesvr_pet_call_callback(player_t* p, home_proto_t* hpkg)
{
	pet_call_rsp_t* rsp = reinterpret_cast<pet_call_rsp_t*>(hpkg->body);
	
	return 0;
}
*/

int get_plants_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->home_grp)) {
		p->waitcmd = 0;
		return 0;
	}

	p->waitcmd = 0;
	int idx = sizeof(home_proto_t);
	init_home_proto_head(p, p->id, homepkgbuf, idx,  home_get_plants_list);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int set_plant_status_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->home_grp)) {
		p->waitcmd = 0;
		return 0;
	}
	uint32_t plant_id;
	uint32_t plant_tm;
	uint32_t type;
	uint32_t field_id;
	uint32_t item_id;
	int idx = 0;

	unpack(body, field_id, idx);
	unpack(body, plant_id, idx);
	unpack(body, plant_tm, idx);
	unpack(body, type, idx);
	unpack(body, item_id, idx);

//	if (type == 2) {
//	//类型是施肥
//		if(!(p->my_packs->is_item_exist(item_id))) {
//		//并且不存在肥料
//			return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1); 
//		}
//	} else if (type == 1) {
//	//类型是浇水
//		if (item_id) {
//		//浇水是不需要物品的
//			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);  
//		}
//	} else {
//	//系统错误
//		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
//	}
	
	idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, field_id, idx);
	pack_h(homepkgbuf, plant_id, idx);
	pack_h(homepkgbuf, plant_tm, idx);
	pack_h(homepkgbuf, type, idx);
	pack_h(homepkgbuf, item_id, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx,  home_set_plant_status);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}


int homesvr_set_plant_status_callback(player_t* p, home_proto_t* hpkg)
{
	set_plant_status_rsp_t* rsp = reinterpret_cast<set_plant_status_rsp_t*>(hpkg->body);

	if (rsp->item_id) {
		p->my_packs->del_item(p, rsp->item_id, 1, channel_string_home);

		const GfItem* itm = items->get_item(rsp->item_id);
		db_use_item(p, rsp->item_id, 1, itm->is_item_shop_sale(), false);
	}


//	if( p->id != p->home_grp->home_owner_id 
//		 || p->role_tm != p->home_grp->role_regtime){
//
//		if (rsp->type == 1) {
//			p->m_home_data->add_action_point(1);
//		} else if (rsp->type == 2) {
//			p->m_home_data->add_action_point(2);
//		}
//
//		db_update_home_active_point(p, p->m_home_data->home_action_point, p->m_home_data->last_tm);
//	}


	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->field_id, idx);
	pack(pkgbuf, rsp->plant_id, idx);
	pack(pkgbuf, rsp->plant_tm, idx);
	pack(pkgbuf, rsp->type, idx);
	pack(pkgbuf, rsp->item_id, idx);

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int pick_fruit_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->home_grp)) {
		p->waitcmd = 0;
		return 0;
	}

//	if( p->id != p->home_grp->home_owner_id 
//		 || p->role_tm != p->home_grp->role_regtime){
//		if (p->m_home_data->home_action_point < 3) {
//			return send_header_to_player(p, p->waitcmd, cli_err_cannot_pick, 1);
//		}
//	}


	uint32_t field = 0;
	uint32_t fruit = 0;
	int idx = 0;
	unpack(body, field, idx);
	unpack(body, fruit, idx);

	if (p->my_packs->is_item_full(p, fruit, 1)) {
		return send_header_to_player(p, p->waitcmd, cli_err_bag_no_space, 1);
	}

	const GfItem * item = items->get_item(fruit);
	if (!item) {
		return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
	}
	
	idx = sizeof(home_proto_t);

	pack_h(homepkgbuf, field, idx);
	pack_h(homepkgbuf, fruit, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx,  home_pick_fruit);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_pick_fruit_callback(player_t* p, home_proto_t* hpkg)
{
	pick_fruit_rsp_t* rsp = reinterpret_cast<pick_fruit_rsp_t*>(hpkg->body);

	p->my_packs->add_item(p, rsp->fruit_id, rsp->fruit_cnt, channel_string_home);

	uint32_t max_bag_grid_count = get_player_total_item_bag_grid_count(p);
	if (!p->my_packs->check_enough_bag_item_grid(p, rsp->fruit_id, 1)){
		//物品包裹满 通过邮件插送
		char title[MAX_MAIL_TITLE_LEN + 1] = {0};
		char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
		std::vector<mail_item_enclosure> item_vec;
		strncpy(title, char_content[0].content, sizeof(title) - 1);
		strncpy(content, char_content[1].content, sizeof(content) - 1);
		//ERROR_LOG("mail: %s ===== %s", title, content);
		item_vec.push_back(mail_item_enclosure(rsp->fruit_id, rsp->fruit_cnt));
		db_send_system_mail(p, title, sizeof(title), content, sizeof(content), mail_templet_normal, 0, &item_vec, 0);
		item_vec.clear();

	} else {
		int idx = 0;
		int type = 2;
		pack_h(dbpkgbuf, type, idx);
		pack_h(dbpkgbuf, rsp->fruit_id, idx);
		pack_h(dbpkgbuf, rsp->fruit_cnt, idx);
		pack_h(dbpkgbuf, max_bag_grid_count, idx);
		send_request_to_db(0, p->id, p->role_tm, dbproto_add_item, dbpkgbuf, idx);
	}

//	if( p->id != p->home_grp->home_owner_id 
//		 || p->role_tm != p->home_grp->role_regtime){
//		p->m_home_data->dec_action_point(3);
//		db_update_home_active_point(p, p->m_home_data->home_action_point, p->m_home_data->last_tm);
//	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->field_id, idx);
	pack(pkgbuf, rsp->fruit_id, idx);
	pack(pkgbuf, rsp->fruit_cnt, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int homesvr_player_walk(player_t* p, uint32_t x, uint32_t y, uint32_t flag)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, x, idx);
	pack_h(homepkgbuf, y, idx);
	pack_h(homepkgbuf, flag, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_player_walk);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_walk_keyboard(player_t* p, uint32_t x, uint32_t y, uint8_t dir, uint8_t state)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, x, idx);
	pack_h(homepkgbuf, y, idx);
	pack_h(homepkgbuf, dir, idx);
	pack_h(homepkgbuf, state, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_walk_keyboard);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_player_stop(player_t* p, uint32_t x, uint32_t y, uint8_t dir)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, x, idx);
	pack_h(homepkgbuf, y, idx);
	pack_h(homepkgbuf, dir, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_player_stop);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_player_jump(player_t* p, uint32_t x, uint32_t y)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, x, idx);
	pack_h(homepkgbuf, y, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_player_jump);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_player_talk(player_t* p, uint8_t* msg, uint32_t msg_len, userid_t recvid)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, recvid, idx);
	pack_h(homepkgbuf, msg_len, idx);
	pack(homepkgbuf, msg, msg_len, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_player_talk);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}


int homesvr_player_enter_battle(player_t* p)
{
	int idx = sizeof(home_proto_t);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_player_enter_battle);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}
int homesvr_player_leave_battle(player_t* p)
{
	int idx = sizeof(home_proto_t);
	pack_home_player_info(homepkgbuf, idx,  p, p->home_grp->home_owner_id ,p->home_grp->role_regtime);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_player_leave_battle);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}


int homesvr_player_home_list_user(player_t* p)
{
	int idx = sizeof(home_proto_t);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_list_user);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_player_home_list_pet(player_t* p)
{
	int idx = sizeof(home_proto_t);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_list_pet);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}


int get_home_list_pet_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	p->waitcmd = 0;
	if (p->home_grp) {
		return homesvr_player_home_list_pet(p);	
	}
	return 0;
}

int pve_syn_home_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if(!check_in_home(p))return -1;
	return homesvr_pve_syn_home_info(p);
}

int homesvr_pve_syn_home_info(player_t* p)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, p->home_grp->home_owner_id, idx);
	pack_h(homepkgbuf, p->home_grp->role_regtime, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_pve_syn_home_info);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_home_owner_off_line(player_t* p)
{
	int idx = sizeof(home_proto_t);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_player_off_line);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}


int homesvr_pve_syn_home_info_callback(player_t* p, home_proto_t* hpkg)
{
	pve_syn_home_info_rsp_t* rsp = reinterpret_cast<pve_syn_home_info_rsp_t*>(hpkg->body);
	p->clear_home_btl_pet();
	for(uint32_t i =0; i< rsp->count; i++)
	{
		uint32_t pet_id = rsp->pets[i];
		switch(pet_id)
		{
			case 1002:
			case 1003:
			case 1004:
			case 1005: 
				p->add_home_btl_pet(13106);
			break;


			case 1012:
			case 1013:
			case 1014:
			case 1015:
				p->add_home_btl_pet(13107);
			break;

			case 1022:
			case 1023:
			case 1024:
			case 1025:
				p->add_home_btl_pet(13108);
			break;

			case 1032:
			case 1033:
			case 1034:
			case 1035:
			case 1036:
				p->add_home_btl_pet(13109);
			break;

			case 1042:
			case 1043:
			case 1044:
			case 1045:
			case 1046:
				p->add_home_btl_pet(13110);
			break;

			case 1052:
			case 1053:
			case 1054:
			case 1055:
			case 1056:
				p->add_home_btl_pet(13111);
			break;

			case 1062:
			case 1063:
			case 1064:
			case 1065:
			case 1066:
				p->add_home_btl_pet(13112);
			break;

			case 1072:
			case 1073:
			case 1074:
			case 1075:
			case 1076:
				p->add_home_btl_pet(13113);
			break;

			case 1082:
			case 1083:
			case 1084:
			case 1085:
			case 1086:
				p->add_home_btl_pet(13114);
			break;

			case 1092:
			case 1093:
			case 1094:
			case 1095:
			case 1096:
				p->add_home_btl_pet(13115);
			break;

			case 1102:
			case 1103:
			case 1104:
			case 1105:
			case 1106:
				p->add_home_btl_pet(13116);
			break;

			case 1112:
			case 1113:
			case 1114:
			case 1115:
			case 1116:
				p->add_home_btl_pet(13117);
			break;

			case 1122:
			case 1123:
			case 1124:
			case 1125:
			case 1126:
				p->add_home_btl_pet(13118);
			break;

			case 1132:
			case 1133:
			case 1134:
			case 1135:
			case 1136:
				p->add_home_btl_pet(13119);
			break;
		}
	}
	return send_header_to_player(p, p->waitcmd, 0, 1);	
}

int sow_plant_seed_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	if( !check_in_home(p)){
		return send_header_to_player(p, p->waitcmd, home_err_not_in_home, 1);
	}

	if( p->id != p->home_grp->home_owner_id 
		 || p->role_tm != p->home_grp->role_regtime){
		return send_header_to_player(p, p->waitcmd, home_err_not_kick_off, 1);
	}

	//static uint32_t home_field[10] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

	uint32_t field_id = 0;
	uint32_t plant_id = 0;
	uint32_t seed_id = 0;
	int idx = 0;
	unpack(body, field_id, idx);
	unpack(body, plant_id, idx);
	unpack(body, seed_id, idx);

	/*
	if (field_id > home_field[p->m_home_data->home_level - 1]) {
		return send_header_to_player(p, p->waitcmd, home_err_not_home_leveless, 1);
	} */
		
 	uint32_t have_cnt = p->my_packs->get_item_cnt(seed_id);
    if (have_cnt <= 0) {
        TRACE_LOG(" player %d don't have seed %u!", p->id, seed_id);
        return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
    }

	do_stat_log_universal_interface_1(0x09819002, 0, 1);
	return homesvr_sow_plant_seed(p, field_id, plant_id, seed_id);
}

int homesvr_sow_plant_seed(player_t *p, uint32_t field, uint32_t plant, uint32_t seed)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, field, idx);
	pack_h(homepkgbuf, plant, idx);
	pack_h(homepkgbuf, seed, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_sow_plant_seed);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_sow_plant_seed_callback(player_t * p, home_proto_t * pkg)
{
	sow_seed_rsp_t * rsp = reinterpret_cast<sow_seed_rsp_t*>(pkg->body);
	p->my_packs->del_item(p, rsp->seed_id, 1, channel_string_home);

	const GfItem* itm = items->get_item(rsp->seed_id);
	if (itm) {
	//GfItem * item = get_item(rsp->seed_id);
		db_use_item(p, rsp->seed_id, 1, itm->is_item_shop_sale(), 0);
	}

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, rsp->field_id, idx);
	pack(pkgbuf, rsp->plant_id, idx);
	pack(pkgbuf, rsp->plant_tm, idx);
	pack(pkgbuf, rsp->seed_id, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);

}	



int home_kick_off_guest_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t guest_id = 0;
	uint32_t guest_regtime = 0;
	unpack(body, guest_id, idx);
	unpack(body, guest_regtime, idx);

	
	if( !check_in_home(p)){
		return send_header_to_player(p, p->waitcmd, home_err_not_in_home, 1);
	}
	if( p->id == guest_id){
		return send_header_to_player(p, p->waitcmd, home_err_not_kick_off, 1);
	}
	p->waitcmd = 0;
	return homesvr_kick_off_guest(p, guest_id, guest_regtime);
}

int homesvr_kick_off_guest(player_t* p, uint32_t user_id, uint32_t role_regtime)
{
	int idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, user_id, idx);
	pack_h(homepkgbuf, role_regtime, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_kick_off_player);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

/*
int homesvr_kick_off_player_callback(player_t* p, home_proto_t* pkg)
{
	kick_off_guest_rsp_t* rsp = reinterpret_cast<kick_off_guest_rsp_t*>(pkg->body);
	leave_home(p);
    p->invisible = 0;	
	return 0;
}
*/

int get_home_attr_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    if (p->home_grp) {
		if (p->home_grp->home_owner_id == p->id) {
			int idx = sizeof(cli_proto_t);
			pack(pkgbuf, p->id, idx);
			pack(pkgbuf, p->role_tm, idx);
			pack(pkgbuf, p->m_home_data->home_level, idx);
			pack(pkgbuf, p->m_home_data->home_exp, idx);
			init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
			return send_to_player(p, pkgbuf, idx, 1);
		}
		p->waitcmd = 0;
		return homesvr_home_attr(p);
	}
	ERROR_LOG("NOT IN HOME %u ", p->id);
    return 0;	
}


int homesvr_home_attr(player_t* p)
{
	int idx = sizeof(home_proto_t);
    init_home_proto_head(p, p->id, homepkgbuf, idx, home_get_home_attr);
    return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);	
}

int home_level_up_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (p->home_grp   
			&& p->home_grp->home_owner_id == p->id ){
		home_exp2level* p_level = get_home_exp2level_data(p->m_home_data->home_level + 1);
		home_exp2level* cur_lv_data =  get_home_exp2level_data(p->m_home_data->home_level);
		if (p_level && cur_lv_data && p->m_home_data->home_exp >= p_level->exp) {
			p->m_home_data->home_level++;
			uint32_t dec_fumo = p_level->fumo_point - cur_lv_data->fumo_point;
			db_set_home_data(p, p->m_home_data->home_level, p->m_home_data->home_exp, dec_fumo); 
			p->fumo_points_total -= dec_fumo;
			homesvr_level_up(p);
		} else {
			return -1;
		}

		int idx = sizeof(cli_proto_t);
		pack(pkgbuf, p->m_home_data->home_level, idx);
		pack(pkgbuf, p->m_home_data->home_exp, idx);
		pack(pkgbuf, p->fumo_points_total, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}
	return -1;
}

int homesvr_level_up(player_t* p)
{
	int idx = sizeof(home_proto_t);
	init_home_proto_head(p, p->id, homepkgbuf, idx, home_level_up);
    return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);	
}

int homesvr_level_up_callback(player_t* p, home_proto_t* pkg)
{
//	level_up_rsp_t* rsp = reinterpret_cast<level_up_rsp_t*>(pkg->body);
//	p->fumo_points_total -= rsp->fumo_point;
//
//	do_stat_log_fumo_add_reduce((uint32_t)0, rsp->fumo_point);
//	p->m_home_data->home_level = rsp->level;
//	home_exp2level* p_level = get_home_exp2level_data(rsp->level);
//	if(p_level)
//	{
//		p->m_home_data->home_action_point = p_level->action_point;
//	}
//
//	int idx = sizeof(cli_proto_t);
//	pack(pkgbuf, rsp->level, idx);
//	pack(pkgbuf, rsp->exp, idx);
//	pack(pkgbuf, rsp->fumo_point, idx);
//	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
//	return send_to_player(p, pkgbuf, idx, 1);
	return 0;
}

int get_home_log_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t home_uid = 0;
	uint32_t home_role_tm = 0;
	unpack(body, home_uid, idx);
	unpack(body, home_role_tm, idx);

	idx = 0;
	return send_request_to_db(p, home_uid, home_role_tm, dbproto_get_home_log, dbpkgbuf, idx);
}

int db_get_home_log_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	
	get_home_log_rsp_t* p_rsp = (get_home_log_rsp_t*)body;

	CHECK_VAL_EQ(bodylen, sizeof(get_home_log_rsp_t) + 
						p_rsp->count * sizeof(get_home_log_item_rsp_t));

	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p_rsp->count, idx);
	get_home_log_item_rsp_t* p_item = (get_home_log_item_rsp_t*)((get_home_log_rsp_t*)body + 1);
	for (uint32_t i = 0; i < p_rsp->count; i++) {
		
		pack(pkgbuf, p_item->op_uid, idx);
		pack(pkgbuf, p_item->op_utm, idx);
		pack(pkgbuf, p_item->role_type, idx);
		pack(pkgbuf, p_item->nick, sizeof(p_item->nick), idx);
		pack(pkgbuf, p_item->type, idx);
		pack(pkgbuf, p_item->access_type, idx);
		pack(pkgbuf, p_item->tm, idx);
		p_item++;
	}
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int insert_vitality_2_home_cmd(player_t * p, uint8_t * body, uint32_t bodylen)
{
	uint32_t vitality_value = 0;
	int idx = 0;
	unpack(body, vitality_value, idx);
	if (vitality_value
			&& vitality_value <= p->vitality_point 
			&& p->m_home_data->home_level < MAX_HOME_LEVEL) {
		p->vitality_point -= vitality_value;
		p->m_home_data->home_exp += vitality_value;
		db_set_home_data(p, p->m_home_data->home_level,  p->m_home_data->home_exp);
		db_set_player_vitality_point(p, p->vitality_point);
	} else {
	   return -1;
	}

	idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->m_home_data->home_exp, idx);
	pack(pkgbuf, p->vitality_point, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

//----------------------------- for decorate ------------------------------
int get_decorate_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->home_grp)) {
		p->waitcmd = 0;
		return 0;
	}

	p->waitcmd = 0;
	int idx = sizeof(home_proto_t);
	init_home_proto_head(p, p->id, homepkgbuf, idx,  home_get_decorate_list);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int set_decorate_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->home_grp)) {
		p->waitcmd = 0;
		return 0;
	}
	uint32_t decorate_id;
	int idx = 0;
	unpack(body, decorate_id, idx);

    p->waitcmd = 0;
	idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, decorate_id, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx,  home_set_decorate);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

uint32_t select_revive_grass(player_t* p)
{
    if (p->my_packs->get_item_cnt(1302001) > 0) {
        return 1302001;
    }

    if (p->my_packs->get_item_cnt(1302000) > 0) {
        return 1302000;
    }
    return 0;
}

int add_decorate_lv_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->home_grp)) {
		p->waitcmd = 0;
		return 0;
	}
	uint32_t decorate_id = 0, flag = 0;
	int idx = 0;
	unpack(body, decorate_id, idx);
	unpack(body, flag, idx);

    TRACE_LOG("decorate action[%u] id[%u]", flag, decorate_id);

	idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, decorate_id, idx);
	pack_h(homepkgbuf, flag, idx);
    uint32_t cnt = 1;
    uint32_t item_arr[] = {1500575, 1300002, 1300005, 1302001};
    if (flag == 1) {
        cnt = 1;
        if (p->my_packs->get_item_cnt(item_arr[0]) <= 0) {
            //err
            return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
        }
        pack_h(homepkgbuf, cnt, idx);
        pack_h(homepkgbuf, item_arr[0], idx);
    } else {
        cnt = 3;
        for (uint32_t i = 1; i < 3; i++) {
            if (p->my_packs->get_item_cnt(item_arr[i]) <= 0) {
                //err
                return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
            }
        }
        item_arr[3] = select_revive_grass(p);
        if (item_arr[3] == 0) {
            return send_header_to_player(p, p->waitcmd, cli_err_no_item, 1);
        }
        pack_h(homepkgbuf, cnt, idx);
        for (uint32_t i = 1; i < 4; i++) {
            pack_h(homepkgbuf, item_arr[i], idx);
        }
    }

	init_home_proto_head(p, p->id, homepkgbuf, idx, home_add_decorate_lv);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_add_decorate_lv_callback(player_t* p, home_proto_t* hpkg)
{
    if (hpkg->ret != 0) {
        return send_header_to_player(p, p->waitcmd, cli_err_no_undertake_task, 1);
    }

    add_decorate_lv_t* rsp = reinterpret_cast<add_decorate_lv_t *>(hpkg->body);

	int idx = sizeof(cli_proto_t);
    pack(homepkgbuf, rsp->flag, idx);
    pack(homepkgbuf, rsp->decorate_id, idx);
    pack(homepkgbuf, rsp->decorate_tm, idx);
    pack(homepkgbuf, rsp->decorate_lv, idx);
    pack(homepkgbuf, rsp->status, idx);
    pack(homepkgbuf, rsp->itemcnt, idx);

    TRACE_LOG("decorate [%u %u %u] del item cnt [%u]", rsp->flag, rsp->decorate_id, rsp->decorate_lv, rsp->itemcnt);
    uint32_t *item_obj = rsp->itemarr;
    db_del_item_elem_t itemarr[4] = {{0},};
    for (uint32_t i = 0; i < rsp->itemcnt; i++) {
        //p->my_packs->del_item(p, item_obj[i], 1);
        pack(homepkgbuf, item_obj[i], idx);
        
        itemarr[i].item_id = item_obj[i];
        itemarr[i].count   = 1;
    }
    
    player_del_items(p, itemarr, rsp->itemcnt, false);

	init_cli_proto_head(homepkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, homepkgbuf, idx, 1);
}

int get_decorate_reward_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	if (!(p->home_grp)) {
		p->waitcmd = 0;
		return 0;
	}
	uint32_t decorate_id = 0;
	int idx = 0;
	unpack(body, decorate_id, idx);

	idx = sizeof(home_proto_t);
	pack_h(homepkgbuf, decorate_id, idx);

	init_home_proto_head(p, p->id, homepkgbuf, idx, home_get_decorate_reward);
	return send_to_home_svr(p, homepkgbuf, idx, p->home_grp->fd_idx);
}

int homesvr_get_decorate_reward_callback(player_t* p, home_proto_t* hpkg)
{
    if (hpkg->ret != 0) {
        return send_header_to_player(p, p->waitcmd, cli_err_no_undertake_task, 1);
    }

    get_decorate_reward_t* rsp = reinterpret_cast<get_decorate_reward_t*>(hpkg->body);

    uint32_t itemid = 1410002;
    p->my_packs->add_item(p, itemid, 1, channel_string_other);

    int idx = 0;
    int type = 2;
    pack_h(dbpkgbuf, type, idx);
    pack_h(dbpkgbuf, itemid, idx);
    pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);
    pack_h(dbpkgbuf, 999, idx);
    send_request_to_db(0, p->id, p->role_tm, dbproto_add_item, dbpkgbuf, idx);

	idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->decorate_id, idx);
    pack(pkgbuf, rsp->decorate_tm, idx);
    pack(pkgbuf, rsp->decorate_lv, idx);
    pack(pkgbuf, rsp->status, idx);
    pack(pkgbuf, itemid, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int query_home_player_count_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int fd_idx = p->id % home_svr_cnt;
    TRACE_LOG("query [%u] home player count ... [%u]", p->id, fd_idx);
	p->waitcmd = 0;
	int idx = sizeof(home_proto_t);
    pack_h(homepkgbuf, p->id, idx);
    pack_h(homepkgbuf, p->role_tm, idx);
	init_home_proto_head(p, p->id, homepkgbuf, idx,  home_query_player_count);
	return send_to_home_svr(p, homepkgbuf, idx, fd_idx);
}


int homesvr_query_home_player_count_callback(player_t* p, home_proto_t* hpkg)
{
    int idx = 0;
    uint32_t player_cnt = 0;
    unpack(hpkg->body, player_cnt, idx);

    TRACE_LOG("query [%u] home player count ... [%u]", p->id, player_cnt);
    idx = sizeof(cli_proto_t);
    pack(pkgbuf, player_cnt, idx);
    init_cli_proto_head(pkgbuf, p, cli_proto_query_home_player_count, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

