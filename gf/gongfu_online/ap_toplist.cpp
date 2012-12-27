#include "ap_toplist.hpp"
#include "utils.hpp"
#include "global_data.hpp"
#include "player.hpp"
#include "cli_proto.hpp"
#include "dbproxy.hpp"

using namespace taomee;



int db_get_ap_toplist(player_t* p, uint32_t begin_index, uint32_t end_index)
{
	int idx = 0;
	pack_h(dbpkgbuf, begin_index, idx);
    pack_h(dbpkgbuf, end_index, idx);
	return send_request_to_db(p, p->id, p->role_tm,  dbproto_get_ap_toplist, dbpkgbuf, idx);
}

int db_get_ap_toplist_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	get_ap_toplist_rsp_t *rsp = reinterpret_cast<get_ap_toplist_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(get_ap_toplist_rsp_t) + rsp->count*sizeof(ap_data));
	get_ap_toplist()->load_ap_toplist(rsp);
	return send_ap_toplist(p, rsp->begin_index, rsp->end_index);
}


int ap_toplist::load_ap_toplist(get_ap_toplist_rsp_t* rsp)
{
	for( uint32_t i = rsp->begin_index - 1; i< rsp->begin_index -1 + rsp->count; i++)
	{
		m_datas[i] = rsp->datas[i] ;
	}
	if( m_last_update_tm == 0 ) {
		m_last_update_tm = time(NULL);
	}
	m_cur_update_count = rsp->end_index;
	return 0;
}


void ap_toplist::clear()
{	
	m_last_update_tm = 0;
	m_cur_update_count = 0;
	memset(m_datas, sizeof(ap_data)* MAX_TOPLIST_SIZE, 0);
}

bool ap_toplist::check_update( uint32_t begin_index, uint32_t end_index)
{
	uint32_t now = time(NULL);
	if(  ( now /(60*60*24) ) != ( m_last_update_tm/(60*60*24) ) )
	{
		clear();	
		return true;
	}

	if(  end_index >  m_cur_update_count )return true;
	return false;
}

int get_ap_toplist_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t begin_index = 0;
	uint32_t end_index = 0;
	unpack(body, begin_index, idx);
	unpack(body, end_index, idx);

	return get_ap_toplist(p, begin_index, end_index);

	/*
	if( !(begin_index >= 1 && begin_index < MAX_TOPLIST_SIZE )){
		return send_header_to_player(p, p->waitcmd, cli_err_ap_toplist_para, 1);	
	}
	if( !(end_index > 1 && end_index <= MAX_TOPLIST_SIZE)){
		return send_header_to_player(p, p->waitcmd, cli_err_ap_toplist_para, 1);
	}
	if( end_index <= begin_index){
		return send_header_to_player(p, p->waitcmd, cli_err_ap_toplist_para, 1);
	}
	
	bool ret = get_ap_toplist()->check_update( begin_index, end_index);
	if(ret)return db_get_ap_toplist(p, begin_index, end_index);
	return send_ap_toplist(p,  begin_index, end_index);
	*/
}


int btlsw_get_ap_toplist_callback(player_t* p, battle_switch_proto_t* bpkg, uint32_t len)
{
	uint32_t begin_index = 0;
	uint32_t end_index = 0;
	uint32_t count = 0;
	uint32_t userid = 0;
	uint32_t role_regtime = 0;
	uint32_t ap_point = 0;
	uint32_t last_update_tm = 0;
	char     nick[ max_nick_size ] = { 0 };


	int idx = 0;
	int idx2 = sizeof(cli_proto_t);

	unpack_h(bpkg->body, begin_index, idx);
	unpack_h(bpkg->body, end_index, idx);
	unpack_h(bpkg->body, count, idx);

	pack(pkgbuf, begin_index, idx2);
	pack(pkgbuf, end_index, idx2);
	pack(pkgbuf, count, idx2);

	for(uint32_t i =0; i< count; i++)
	{
		unpack_h(bpkg->body, userid, idx);
		unpack_h(bpkg->body, role_regtime, idx);
		unpack(bpkg->body, nick, max_nick_size, idx);
		unpack_h(bpkg->body, ap_point, idx);
		unpack_h(bpkg->body, last_update_tm, idx);
	
		pack(pkgbuf, userid, idx2);
		pack(pkgbuf, role_regtime, idx2);
		pack(pkgbuf, nick, max_nick_size, idx2);
		pack(pkgbuf, ap_point, idx2);
		pack(pkgbuf, last_update_tm, idx2);
	}
	init_cli_proto_head(pkgbuf, p,  cli_proto_get_ap_toplist, idx2);
	return send_to_player(p, pkgbuf, idx2, 1);
}

int send_ap_toplist(player_t* p, uint32_t begin_index, uint32_t end_index)
{
	int idx = sizeof(cli_proto_t);
	uint32_t count = 0;
	pack(pkgbuf, begin_index, idx);
	pack(pkgbuf, end_index, idx);
	int old_idx = idx;
	pack(pkgbuf, count, idx);

	for(uint32_t i = begin_index - 1; i< end_index; i++)
	{
		ap_data* pdata = &(get_ap_toplist()->m_datas[i]);
		if(pdata->userid == 0)continue;

		pack(pkgbuf, pdata->userid, idx);
		pack(pkgbuf, pdata->role_regtime, idx);
		pack(pkgbuf, pdata->nick,  sizeof(pdata->nick), idx);
		pack(pkgbuf, pdata->ap_point, idx);
		pack(pkgbuf, pdata->last_tm, idx);
		count ++;
	}
	pack(pkgbuf, count ,old_idx);
	init_cli_proto_head(pkgbuf, p,  cli_proto_get_ap_toplist, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}



