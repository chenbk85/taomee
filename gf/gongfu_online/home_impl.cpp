#include "home_impl.hpp"
#include "player.hpp"
#include "utils.hpp"
#include "item.hpp"
#include "login.hpp"
#include "global_data.hpp"
#include <libtaomee/project/utilities.h>
#include <kf/home.hpp>
using namespace std;
using namespace taomee;


bool init_player_home_data(player_t *p )
{
	p->m_home_data = new player_home_data();
	return true;
}
bool final_player_home_data(player_t *p )
{
	delete p->m_home_data;
	p->m_home_data = NULL;
	return true;
}

bool load_player_home_data(player_t *p, get_home_data_rsp_t* rsp)
{	
	if( rsp->home_level == 0)
	{
		p->m_home_data->home_level = 1;
	}
	else
	{
		p->m_home_data->home_level = rsp->home_level;
	}
	p->m_home_data->home_exp = rsp->home_exp;
	//p->m_home_data->reset_action_point();
	return true;
}

int  db_get_player_home_data(player_t *p)
{
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_home_data, 0, 0);
}

int  db_get_home_data_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_home_data_rsp_t *rsp = reinterpret_cast<get_home_data_rsp_t*>(body);
	load_player_home_data(p, rsp);
	if ( p->waitcmd == cli_proto_login ){
		return  db_kill_boss_list(p);
	}
	return 0;
}

int  db_set_home_data(player_t * p, uint32_t home_lv, uint32_t home_exp, uint32_t del_fumo) 
{
	int idx = 0;
//    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(2), idx);
//    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(1302001), idx);
//    taomee::pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx);
	pack_h(dbpkgbuf, home_lv, idx);
	pack_h(dbpkgbuf, home_exp, idx);
	pack_h(dbpkgbuf, del_fumo, idx);
    return send_request_to_db(0, p->id, p->role_tm, dbproto_set_home_data, dbpkgbuf, idx);
}

int db_set_home_data_callback(player_t *p, userid_t id, void * body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	set_home_back_rsp_t * pkg = reinterpret_cast<set_home_back_rsp_t*>(body);
	p->m_home_data->home_level = pkg->home_level;
	p->m_home_data->home_exp = pkg->home_exp;
	if (pkg->dec_fumo) {
		p->fumo_points_total -= pkg->dec_fumo;
	}
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, p->m_home_data->home_level, idx);
	pack(pkgbuf, p->m_home_data->home_exp, idx);
	pack(pkgbuf, p->fumo_points_total, idx);
//	pack(pkgbuf, p->vitality_point, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


//void player_home_data::reset_action_point()
//{
//	home_exp2level* p_level = get_home_exp2level_data(home_level);
//	if(!is_in_same_day() && p_level != NULL){
//		home_action_point = p_level->action_point;
//		last_tm = time(NULL);
//	}
//}
//
//void player_home_data::add_action_point(uint32_t point)
//{
//	home_exp2level* p_level = get_home_exp2level_data(home_level);
//
//	if(!is_in_same_day() && p_level != NULL){
//		home_action_point = p_level->action_point;
//	}
//	home_action_point += point;
//	last_tm = time(NULL);
//}
//
//void player_home_data::dec_action_point(uint32_t point)
//{
//	home_exp2level* p_level = get_home_exp2level_data(home_level);
//
//	if(!is_in_same_day() && p_level != NULL){
//		home_action_point = p_level->action_point;
//	}
//	if(home_action_point > point)
//	{
//		home_action_point -= point;
//	}
//	else
//	{
//		home_action_point = 0;
//	}
//	last_tm = time(NULL);
//}
//
//void player_home_data::set_action_point(uint32_t point, uint32_t tm)
//{
//	home_action_point = point;
//	last_tm = tm;
//}
//
//int db_update_home_active_point(player_t* p, uint32_t point, uint32_t last_tm)
//{
//	int idx = 0;
//	pack_h(dbpkgbuf,  point, idx);
//	pack_h(dbpkgbuf,  last_tm, idx);
//	return send_request_to_db(0, p->id, p->role_tm, dbproto_update_home_active, dbpkgbuf, idx);
//}
