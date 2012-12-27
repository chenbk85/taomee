#include <cerrno>
#include <cstdlib>
#include <cstring>

#include <vector>

using namespace std;

#include <libtaomee++/inet/pdumanip.hpp>

using namespace taomee;

extern "C" {
#include <fcntl.h>
#include <sys/mman.h>

#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
#include "libtaomee/project/stat_agent/msglog.h"
}

#include "dbproxy.hpp"
#include "./proto/pop_switch.h"
#include "./proto/pop_switch_enum.h"
#include "./proto/pop_db_enum.h"
#include "utils.hpp"

#include "online.hpp"

int g_log_send_buf_hex_flag = 1;

struct friend_list_t {
	uint32_t cnt;
	userid_t uid[];
} __attribute__((packed));

uint32_t	Online::s_max_online_id_;
uint8_t		Online::s_pkg_[pkg_size];

char * 		Online::statistic_file;

online_info_t      Online::s_online_info_[online_num_max];
std::map<int, int>  Online::s_fd_map_;
Cuser_online_map  Online::user_online_map; 
Ctimer 	Online::timer; 
std::map< uint32_t, user_cmd_buff > user_cmd_list;

int Online::send_msg_to_online(int online_id, uint32_t userid,uint16_t cmdid,Cmessage *c_in )
{
	static char dbbuf[ sizeof( svr_proto_t) ];

	svr_proto_t* pkg = (svr_proto_t*)dbbuf;
	pkg->len = sizeof(svr_proto_t);
	pkg->seq = 0;
	pkg->cmd = cmdid;
	pkg->ret = 0;
	pkg->id  = userid;

	return send_msg_to_client(s_online_info_[online_id - 1].fdsess, (char*)dbbuf,c_in );
}

#define n_log_online_user_count 1

void Online::init()
{
	srand(time(0));
 	s_max_online_id_=0;
    timer.add_function(n_log_online_user_count ,Online::log_online_user_count  );
	timer.add_timer(time(NULL)+1,n_log_online_user_count,0  );
	statistic_file=config_get_strval("statistic_file");
}

void Online::fini()
{
}
void Online::clear_online_info(int fd)
{
	uint16_t online_id = s_fd_map_[fd];

	if (online_id) {
		int idx = online_id - 1;

		DEBUG_LOG("ONLINE CLOSE CONN\t[id=%u %u ip=%16.16s]",
					online_id, s_online_info_[idx].online_id, s_online_info_[idx].online_ip);
		//清空userid<-->onlineid
	    user_online_map.remove_by_onlineid(online_id);
		memset(&(s_online_info_[idx]), 0, sizeof(s_online_info_[0]));

		// reset max online id
		if (online_id == s_max_online_id_) {
			uint32_t i = s_max_online_id_ - 1;
			while (i && (s_online_info_[i - 1].online_id == 0)) {
				DEBUG_LOG("ONLINE CLOSE CONN:222: i=%u " ,i );
				--i;
			}
			s_max_online_id_ = i;
		}
		DEBUG_LOG("ONLINE CLOSE CONN:END");
	}
}

int Online::sw_report_online_info ( svr_proto_t* pkg, Cmessage* c_in,  fdsession_t* fdsess ) 
{
	sw_report_online_info_in * p_in = P_IN; 

	if ((p_in->online_id > online_num_max) || (p_in->online_id == 0)) {
		DEBUG_LOG("invalid onlineid=%u from fd=%d", p_in->online_id, fdsess->fd);
		return -1;
	}

	int idx = p_in->online_id - 1;
	online_info_t   *p_cur_online_info=&(s_online_info_[idx]);

	// this can happen on a very special case
	if (p_cur_online_info->online_id == p_in->online_id) {
		DEBUG_LOG("DUPLICATE ONLINE ID\t[id=%u]", p_in->online_id);
		return 0;
		// close previous fd
		close_client_conn(p_cur_online_info->fdsess->fd);
	}
	
	p_cur_online_info->domain_id   = p_in->domain_id;
	p_cur_online_info->online_id   = p_in->online_id;
	memcpy(p_cur_online_info->online_ip, p_in->online_ip, 
			sizeof(p_cur_online_info->online_ip));

	p_cur_online_info->online_port = p_in->online_port;
	p_cur_online_info->user_num    = p_in->user_num;
	p_cur_online_info->seqno       = p_in->seqno; // depreciated
	p_cur_online_info->fdsess      = fdsess;


	s_fd_map_[fdsess->fd] = p_in->online_id;

	//update s_max_online_id_
	if (s_max_online_id_ < p_in->online_id) {
		s_max_online_id_ = p_in->online_id;
	}

	for (uint32_t i=0;i<p_in->userid_list.size();i++ ) {
		user_online_map.remove_by_userid(p_in->userid_list[i] );
		//设置用户在线情况
		user_online_map.add(p_in->userid_list[i] ,p_in->online_id , 0);
	}

	return 0;
}
int Online::send_recommend_svr(fdsession_t* fdsess, uint32_t uid, uint32_t seq, uint32_t last_online_id)
{
	sw_get_recommend_svr_list_out out;
	out.max_online_id=s_max_online_id_ ;

	if (last_online_id) {
		online_info_t* ponline = &s_online_info_[last_online_id - 1];
		if (ponline->online_id == last_online_id) {
			online_item_t item;
			item.online_id=ponline->online_id;
			item.user_num=ponline->user_num;
			item.online_port=ponline->online_port;
			memcpy(item.online_ip,ponline->online_ip,sizeof(item.online_ip ) );
			item.friend_count=0;
			out.online_list.push_back(item);	
		}
	}
		
	for (uint32_t i = 0; i <= s_max_online_id_ ; ++i) {
		if (s_online_info_[i].online_id >0 && s_online_info_[i].online_id != last_online_id) {//存在
			if (s_online_info_[i].user_num > 45)
				continue;
			online_item_t item;
			item.online_id=s_online_info_[i].online_id;
			item.user_num=s_online_info_[i].user_num;
			item.online_port=s_online_info_[i].online_port;
			memcpy(item.online_ip,s_online_info_[i].online_ip,sizeof(item.online_ip ) );
			item.friend_count=0;
			out.online_list.push_back(item);	
			if (out.online_list.size()>=10){
				break;
			}
		}
	}

	init_proto_head(s_pkg_, uid, sizeof(svr_proto_t) , seq, sw_get_recommend_svr_list_cmd, 0);
	return send_msg_to_client(fdsess, (char*)s_pkg_, &out );

}

int Online::sw_get_recommend_svr_list(svr_proto_t* pkg, Cmessage *c_in,fdsession_t* fdsess)
{
	user_cmd_list[fdsess->fd].fdsess = fdsess;
	if (user_cmd_list[fdsess->fd].is_waiting(pkg->id, pkg->cmd)) {
		KDEBUG_LOG(pkg->id, "handle\t[%u]", pkg->cmd);
		return 0;
	}	

	//HACK: 设置原有的用户离线
	uint16_t online_id = user_online_map.get_onlineid_by_userid(pkg->id);
	if (online_id != 0) {
		// 让上线用户下线
		send_msg_to_online(online_id,pkg->id,sw_set_user_offline_cmd );
		DEBUG_LOG("KICK PREVIOUS LOGIN OFFLINE\t[uid=%u olid=%d]", pkg->id, online_id);
		send_recommend_svr(fdsess, pkg->id, pkg->seq, online_id);
	} else {
		if (send_request_to_db(pkg->id, ((fdsess->fd << 16) | pkg->cmd), pop_get_base_info_cmd, NULL, 0) == 0)
			user_cmd_list[fdsess->fd].add_waiting(pkg->id, pkg->cmd, pkg->seq);
	}

	return 0;
}

int Online::sw_get_ranged_svrlist(svr_proto_t* pkg, Cmessage *c_in,fdsession_t* fdsess)
{
	sw_get_ranged_svrlist_in * p_in = P_IN; 
	sw_get_ranged_svrlist_out out;
	DEBUG_LOG( "sw_get_ranged_svrlist_out[%u %u]", p_in->start_id, p_in->end_id);


	if ((p_in->start_id == 0) || (p_in->end_id > online_num_max)
			|| (p_in->start_id > p_in->end_id) || ((p_in->end_id - p_in->start_id) > 200)) {
		DEBUG_LOG("invalid range: start=%u end=%u", p_in->start_id, p_in->end_id);
		return -1;
	}

	out.max_online_id=s_max_online_id_ ;
	for (uint32_t i = p_in->start_id - 1; i != p_in->end_id; ++i) {
		if (s_online_info_[i].online_id) {
			online_item_t item;
			item.online_id=s_online_info_[i].online_id;
			item.user_num=s_online_info_[i].user_num;
			item.online_port=s_online_info_[i].online_port;
			memcpy(item.online_ip,s_online_info_[i].online_ip,sizeof(item.online_ip ) );
			item.friend_count=0;
			out.online_list.push_back(item);	
		}
	}

	init_proto_head(s_pkg_, pkg->id, sizeof(*pkg ) , pkg->seq, pkg->cmd, 0);

	return send_msg_to_client(fdsess, (char*)s_pkg_, &out );
}

int Online::sw_report_user_onoff(svr_proto_t* pkg,Cmessage *c_in, fdsession_t* fdsess)
{
	sw_report_user_onoff_in * p_in = P_IN; 

	uint16_t cur_online_id = s_fd_map_[fdsess->fd];
	int idx = cur_online_id - 1;
	CHECK_VAL_GE(idx, 0);

	if (p_in->is_on_online ) { //user online
		DEBUG_LOG("USER ON \t[uid=%u olineid=%d]", pkg->id, cur_online_id);
		uint16_t online_id = user_online_map.get_onlineid_by_userid(pkg->id);
		// this would be a bug of online server
		if (online_id == cur_online_id) {
			DEBUG_LOG("impossible error! online id match=%d", cur_online_id);
			return -1;
		} else if (online_id) {
			send_msg_to_online(online_id,pkg->id,sw_set_user_offline_cmd );
			DEBUG_LOG("KICK PREVIOUS LOGIN OFFLINE\t[uid=%u olid=%d]", pkg->id, online_id);
		}
		++(s_online_info_[idx].user_num);

		user_online_map.add(pkg->id, cur_online_id,online_id );
	} else { //user offline
		--(s_online_info_[idx].user_num);
		if (user_online_map.get_onlineid_by_userid( pkg->id) == cur_online_id) {
			user_online_map.remove_by_userid(pkg->id );
		}
		DEBUG_LOG("USER OFF\t[uid=%u olineid=%d]", pkg->id, cur_online_id);
	}

	// TODO - to be commented out
	if (((int)(s_online_info_[idx].user_num)) < 0) {
		DEBUG_LOG("impossible error! user num=%d", s_online_info_[idx].user_num);
		s_online_info_[idx].user_num=0;
		return -1;
	}

	return 0;
}


void Online::log_online_user_count(uint32_t key, void*data,int data_len)
{
	uint32_t count=0;

	for (uint32_t i = 0; i <= s_max_online_id_ ; ++i) {
		count+=s_online_info_[i].user_num;
	}

	msglog(statistic_file ,0x11000004,  time(NULL), &count, sizeof(count) );
	timer.add_timer(time(NULL)+60,n_log_online_user_count,0  );

}

int Online::sw_get_user_count(svr_proto_t* pkg,Cmessage *c_in, fdsession_t* fdsess)
{

	sw_get_user_count_out out;
	out.online_count=0;
	for (uint32_t i = 0; i <= s_max_online_id_ ; ++i) {
		out.online_count+=s_online_info_[i].user_num;
	}

	init_proto_head(s_pkg_, pkg->id, sizeof(*pkg ) , pkg->seq, pkg->cmd, 0);
	return send_msg_to_client(fdsess, (char*)s_pkg_, &out );
}

int Online::sw_noti_across_svr(svr_proto_t* pkg,Cmessage *c_in, fdsession_t* fdsess)
{
	sw_noti_across_svr_in *p_in = (sw_noti_across_svr_in *)c_in;
	sw_noti_across_svr_out cli_out;
	cli_out.cmdid = p_in->cmdid;
	cli_out._buflist_len = p_in->_buflist_len;
	memcpy(cli_out.buflist, p_in->buflist, cli_out._buflist_len);
	for (uint32_t i = 0; i <= s_max_online_id_ ; ++i) {
		online_info_t& olinfo = s_online_info_[i];
		if (olinfo.online_id) {
			send_msg_to_online(olinfo.online_id, 0, sw_noti_across_svr_cmd, &cli_out);
		}
	}

	return 0;
}
