#include "home_attr.hpp"
#include "player.hpp"
#include "home.hpp"
#include "home_impl.hpp"
#include "dbproxy.hpp"
#include "utils.hpp"

using namespace taomee;


int db_get_home_data_callback( Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret )
{
	CHECK_DBERR(p, ret);
	home_attr_data_rsp_t* rsp = reinterpret_cast<home_attr_data_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(home_attr_data_rsp_t));	
	p->CurHome->init_home_attr_data(rsp);
	if (!p->CurHome->IsOwner(p)) {
		p->CurHome->send_home_attr_info(p);
	}
	return 0;
}

int db_get_home_data(Player* p)
{
	return  send_request_to_db(p, p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, dbproto_get_home_data, 0, 0);
}


int db_set_home_update_tm(uint32_t owner_id, uint32_t owner_tm, uint32_t last_update_tm)
{
	int idx = 0;
	pack_h(dbpkgbuf, last_update_tm, idx);
	return send_request_to_db(NULL, owner_id, owner_tm, dbproto_set_home_update_tm, dbpkgbuf, idx);
}

int db_replace_home_exp_level(Player* p, uint32_t exp, uint32_t level, uint32_t fumo_point, bool callback)
{
//	int idx = 0;
//	pack_h(dbpkgbuf, exp, idx);
//	pack_h(dbpkgbuf, level, idx);
//	pack_h(dbpkgbuf, fumo_point, idx);
//	return send_request_to_db( (callback ? p : NULL), p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, dbproto_replace_home_exp_level, dbpkgbuf, idx);
	return 0;
}

int db_replace_home_exp_level_callback( Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	home_replace_exp_level_rsp_t* rsp = reinterpret_cast<home_replace_exp_level_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(home_replace_exp_level_rsp_t));
	p->CurHome->GetHomeDetail()->set_home_level(rsp->home_level);
	p->CurHome->GetHomeDetail()->set_home_exp(rsp->home_exp);
	int idx = sizeof(home_proto_t);
	pack_h(pkgbuf, rsp->home_level, idx);
	pack_h(pkgbuf, rsp->home_exp, idx);
	pack_h(pkgbuf, rsp->fumo_point, idx);
	init_home_proto_head(pkgbuf,  p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

int db_update_active_point(Player* p, uint32_t active_point, uint32_t last_tm)
{
//	int idx = 0;
//	pack_h(dbpkgbuf, active_point, idx);
//	pack_h(dbpkgbuf, last_tm, idx);
//	return send_request_to_db(p, p->id, p->role_tm, dbproto_update_home_active_point, dbpkgbuf, idx);
	return 0;
}

