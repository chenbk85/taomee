#ifndef _AP_IOPLIST_H_
#define _AP_IOPLIST_H_

#include"fwd_decl.hpp"
#include "vector"
#include "battle_switch.hpp"
using namespace std;

#define MAX_TOPLIST_SIZE 1000

struct player_t;

typedef struct ap_data
{
	ap_data()
	{
		userid = 0;
		role_regtime = 0;
		ap_point = 0;
		last_tm = 0;
		memset(nick, sizeof(nick), 0);
	}
	uint32_t userid;
	uint32_t role_regtime;
	char     nick[  max_nick_size ];
	uint32_t ap_point;
	uint32_t last_tm;
}ap_data;


typedef struct get_ap_toplist_rsp_t
{
	uint32_t begin_index;
	uint32_t end_index;
	uint32_t count;
	ap_data  datas[];
}get_ap_toplist_rsp_t;

class ap_toplist
{
public:
	ap_toplist()
	{
		m_last_update_tm = 0;
		m_cur_update_count = 0;
		memset(m_datas, sizeof(ap_data)* MAX_TOPLIST_SIZE, 0);
	}
public:
	void clear();
	bool check_update( uint32_t begin_index, uint32_t end_index);
	int load_ap_toplist(get_ap_toplist_rsp_t* rsp);	
public:
	ap_data				 m_datas[MAX_TOPLIST_SIZE];
	uint32_t           	 m_last_update_tm;
	uint32_t             m_cur_update_count;
};



int db_get_ap_toplist(player_t* p, uint32_t begin_index, uint32_t end_index);
int db_get_ap_toplist_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

int get_ap_toplist_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
int send_ap_toplist(player_t* p, uint32_t begin_index, uint32_t end_index);

int btlsw_get_ap_toplist_callback(player_t* p, battle_switch_proto_t* bpkg, uint32_t len);

#endif
