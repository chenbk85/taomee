#include <cerrno>
#include <cstdlib>
#include <string>
#include <vector>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/inet/byteswap.hpp>
#include <algorithm>
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
#include "utils.hpp"
#include "online.hpp"
#include "dbproxy.hpp"

using namespace std;
using namespace taomee;


uint8_t       s_pkg_[pkg_size] = {0};


online_info_t*      Online::p_online_infos = NULL;
std::map<int, int>  Online::online_fd_maps;



void Online::init()
{
	final();
	Online::p_online_infos = new online_info_t[MAX_ONLINE_INFO_COUNT];
}

void Online::final()
{
	if(Online::p_online_infos != NULL){
		delete [] Online::p_online_infos;			    
		Online::p_online_infos = NULL;
	}
}


int Online::get_online_id_by_fd(int fd)
{
	if (online_fd_maps.find(fd) == online_fd_maps.end()){
		return -1;
	}
	return online_fd_maps[fd];
}

int Online::get_mail_head_list(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct get_mail_head_list_req_t
	{
		uint32_t userid;
		uint32_t role_regtime;
	}__attribute__((packed));

	CHECK_VAL_EQ(bodylen, sizeof(get_mail_head_list_req_t));
	get_mail_head_list_req_t* req = reinterpret_cast<get_mail_head_list_req_t*>(pkg->body);

	online_info_t* info = get_online_info_by_fd(fdsess->fd);
	return db_mail_head_list(info, req->userid, req->role_regtime);
}

int Online::get_mail_body(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct get_mail_body_req_t	
	{
		uint32_t userid;
		uint32_t role_regtime;
		uint32_t mail_id;
	}__attribute__((packed));
	
	CHECK_VAL_EQ(bodylen, sizeof(get_mail_body_req_t));
	get_mail_body_req_t* req = reinterpret_cast<get_mail_body_req_t*>(pkg->body);
	
	online_info_t* info = get_online_info_by_fd(fdsess->fd);
	return db_mail_body(info, req->userid, req->role_regtime, req->mail_id);
}

int Online::report_online_info(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct online_info_req_t
	{
		uint16_t    domain_id;
		uint32_t    online_id;
		uint32_t    game_type;
		uint8_t     online_ip[16];
		uint16_t    online_port;
	} __attribute__((packed));	


	CHECK_VAL_EQ(bodylen, sizeof(online_info_req_t));
	online_info_req_t* req = reinterpret_cast<online_info_req_t*>(pkg->body);
	
	if(  !(req->online_id > 0 && req->online_id < MAX_ONLINE_INFO_COUNT) ){
		ERROR_LOG("invalid online id = %u from fd = %d", req->online_id, fdsess->fd);
		return -1;
	}	
	
	int idx = req->online_id;
	if( p_online_infos[idx].online_id == req->online_id)
	{
		DEBUG_LOG("dduplicate online id\t[id = %u]", req->online_id);
		close_client_conn( p_online_infos[idx].fdsess->fd );
	}	
	p_online_infos[idx].domain    = req->domain_id;
 	p_online_infos[idx].online_id = req->online_id;
	p_online_infos[idx].game_type = req->game_type;
	memcpy(p_online_infos[idx].online_ip, req->online_ip, sizeof( p_online_infos[idx].online_ip ));
	p_online_infos[idx].online_port = req->online_port;
	p_online_infos[idx].fdsess = fdsess;
	p_online_infos[idx].fd = fdsess->fd;
	online_fd_maps[fdsess->fd] = req->online_id;
	return 0;
}


online_info_t*  Online::get_online_info_by_fd(int fd)
{
	std::map<int, int>::iterator pItr = online_fd_maps.find(fd);
	if(pItr == online_fd_maps.end())return NULL;
	int online_id = pItr->second;
	if( !(online_id >=0 && online_id <  MAX_ONLINE_INFO_COUNT ))return NULL;
	return &p_online_infos[online_id];
}

int Online::clear_online_info(int fd)
{
	std::map<int, int>::iterator pItr = online_fd_maps.find(fd);
	if(pItr == online_fd_maps.end()){
		return -1;
	}

	int online_id = pItr->second;	
	online_fd_maps.erase(pItr);
	if( online_id >=0 && online_id <  MAX_ONLINE_INFO_COUNT )
	{
		p_online_infos[online_id].domain = 0;
		p_online_infos[online_id].online_id = 0;
		memset( p_online_infos[online_id].online_ip, 0, sizeof(p_online_infos[online_id].online_ip));
		p_online_infos[online_id].online_port = 0;
		p_online_infos[online_id].fd = 0;
		p_online_infos[online_id].game_type = 0;
		p_online_infos[online_id].fdsess = NULL;
	}	
	return 0;
}

int db_mail_head_list(online_info_t* info, uint32_t userid, uint32_t role_regtime)
{
	return send_request_to_db(info->fd, userid, role_regtime, dbproto_mail_head_list, 0, 0);
}


int db_mail_head_list_callback(online_info_t* info, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	if(ret != 0)return ret;	

	mail_head_list_rsp_t *rsp = reinterpret_cast<mail_head_list_rsp_t*>(body);
	CHECK_VAL_EQ(bodylen, sizeof(mail_head_list_rsp_t) + rsp->count*sizeof(mail_header));

	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, rsp->count, idx);
	for(uint32_t i =0; i < rsp->count; i++)
	{
		mail_header* header = &rsp->headers[i];
		pack_h(s_pkg_,  header->mail_id, idx);
		pack_h(s_pkg_, 	header->mail_time, idx);
		pack_h(s_pkg_,  header->mail_state, idx);
		pack_h(s_pkg_,  header->mail_templet, idx);
		pack_h(s_pkg_,  header->mail_type, idx);
		pack_h(s_pkg_,  header->sender_id, idx);
		pack_h(s_pkg_,  header->sender_role_tm, idx);
		pack(s_pkg_,  	header->mail_title,  sizeof(header->mail_title), idx);
	}
	init_proto_head(s_pkg_, id, idx, proto_online_mail_head_list, 0, 0);
	return send_pkg_to_client(info->fdsess, s_pkg_, idx);
}


int db_mail_body(online_info_t* info, uint32_t userid, uint32_t role_regtime, uint32_t mail_id)
{
	char buf[1024] = {0};
	int idx = 0;
	pack_h(buf, mail_id, idx);
	return send_request_to_db(info->fd, userid, role_regtime,  dbproto_mail_body, buf, idx);
}


int db_mail_body_callback(online_info_t* info, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	if(ret != 0)return ret;
	mail_body_rsp_t *rsp = reinterpret_cast<mail_body_rsp_t*>(body);
	int idx = sizeof(svr_proto_t);
	pack_h(s_pkg_, rsp->mail_id, idx);
	pack(s_pkg_, rsp->mail_body, sizeof(rsp->mail_body), idx);
	pack(s_pkg_, rsp->mail_numerical_enclosure, 1024, idx);
	pack(s_pkg_, rsp->mail_item_enclosure,  1024, idx);
	pack(s_pkg_, rsp->mail_equip_enclosure, 1024, idx);
	init_proto_head(s_pkg_, id, idx, proto_online_mail_body, 0, 0);
	return send_pkg_to_client(info->fdsess, s_pkg_, idx);
}
