#ifndef SWITCH_ONLINE_HPP_
#define SWITCH_ONLINE_HPP_

#include <map>
#include <set>

extern "C" {
#include <arpa/inet.h>

#include <libtaomee/log.h>

#include <async_serv/async_serv.h>
}
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include "Ctimer.h"



#include "proto.hpp"

struct online_info_t {
	uint16_t	domain_id;
	uint32_t	online_id;
	char		online_ip[16];
	in_port_t	online_port;
	uint32_t	user_num;
	uint32_t	seqno; // depreciated

	fdsession_t*	fdsess;
};

struct usr_info_t {
    uint16_t online_id;
    list_head_t online_node; //user's list node
} __attribute__((packed));

struct cmd_info_t {
	uint32_t	cmd;
	uint32_t	seq;
	cmd_info_t(uint32_t cmdid, uint32_t seqid = 0) : cmd(cmdid), seq(seqid) {}
	bool operator < (const cmd_info_t& rhs) const
	{
		return cmd < rhs.cmd;
	}
};

typedef std::map< uint32_t, std::set<cmd_info_t> > map_uint_cmdset_t;
class user_cmd_buff {
	map_uint_cmdset_t	cmd_map;
public:
	fdsession_t* fdsess;
	bool is_waiting(uint32_t userid, uint32_t cmd)
	{
		map_uint_cmdset_t::iterator it = cmd_map.find(userid);
		if (it == cmd_map.end())
			return false;		

		return it->second.find(cmd) != it->second.end();
	}

	void add_waiting(uint32_t userid, uint32_t cmd, uint32_t seq = 0)
	{
		cmd_map[userid].insert(cmd_info_t(cmd, seq));
		DEBUG_LOG("CS\t[%u %u]", cmd, seq);
	}

	uint32_t remove_waiting(uint32_t userid, uint32_t cmd)
	{
		map_uint_cmdset_t::iterator it = cmd_map.find(userid);
		if (it == cmd_map.end())
			return 0;		

		uint32_t seq = it->second.find(cmd)->seq;
		
		it->second.erase(cmd);
		if (it->second.size() == 0) {
			cmd_map.erase(userid);
		}

		return seq;
	}

	void remove_all(uint32_t userid)
	{
		cmd_map.erase(userid);
	}
};

extern std::map< uint32_t, user_cmd_buff > user_cmd_list;

#include <map>
#include <set>
class Cuser_online_map {
	private:
		std::map<uint32_t, std::set<uint32_t> > online_map;
		std::map<uint32_t, uint32_t > user_map;
	public:
		void add( uint32_t userid,uint32_t onlineid,uint32_t old_onlineid ){
			std::map<uint32_t, std::set<uint32_t> >::iterator it;
			it=online_map.find(onlineid);
			if (it==online_map.end() ){//没有找到
				std::set<uint32_t > tmp_set;
				tmp_set.insert(userid);
				online_map[onlineid]=tmp_set;
			}else{//找到了
				it->second.insert(userid  );

			}
			if (old_onlineid!=0){
				it=online_map.find(old_onlineid );
				if (it!=online_map.end() ){ //找到了
					it->second.erase(userid);
				}
			}
			this->user_map[userid]=onlineid;
		}

		void remove_by_userid(uint32_t userid){
			
			std::map<uint32_t,uint32_t>::iterator it;
			it=this->user_map.find(userid);
			uint32_t onlineid=0;
			if ( it!=this->user_map.end()){//找到了
				onlineid=it->second;
				this->user_map.erase(it);
			}
		
			std::map<uint32_t, std::set<uint32_t> >::iterator it_set;
			it_set=online_map.find(onlineid);
			if (it_set!=online_map.end() ){ //找到了
				it_set->second.erase(userid  );
			}
			
		}

		void remove_by_onlineid(uint32_t onlineid){
			
			std::map<uint32_t, std::set<uint32_t> >::iterator it;
			it=online_map.find(onlineid);
			if (it!=online_map.end() ){ //找到了
				std::set<uint32_t>::iterator  set_it;
				for ( set_it=it->second.begin();set_it!= it->second.end();++set_it ){
					this->user_map.erase(*set_it);
				}
			}
		}

		uint32_t get_onlineid_by_userid( uint32_t userid){
			return this->user_map[userid];
		}

		std::set<uint32_t> & get_userid_set_by_onlineid( uint32_t onlineid){
			return this->online_map[onlineid];
		}
};




class Online {
public:
	static void init();
	static void fini();
//-------------------------------------------------------
//函数定义

#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    static int proto_name( svr_proto_t* pkg, Cmessage* c_in,  fdsession_t* fdsess ) ;
#include "./proto/pop_switch_bind_for_cli.h"


	// clear online info on connection closed
	static void clear_online_info(int fd);
	static void log_online_user_count(uint32_t key, void*data,int data_len);
	static int send_recommend_svr(fdsession_t* fdsess, uint32_t uid, uint32_t seq, uint32_t last_online_id);
	static  Ctimer  		timer; 
private:
	// types
	enum {
		/*! maximum online id */
		online_num_max	= 2048
	};

	static int send_msg_to_online(int online_id, uint32_t userid,uint16_t cmdid,Cmessage *c_in=NULL );
	//
	//

	static online_info_t s_online_info_[online_num_max];
	static uint32_t      s_max_online_id_; // max online id
	static char * statistic_file;

	static   std::map<int, int>  s_fd_map_;
	static  Cuser_online_map  user_online_map; 

	static uint8_t		s_pkg_[pkg_size];
};




#endif // SWITCH_ONLINE_HPP_

