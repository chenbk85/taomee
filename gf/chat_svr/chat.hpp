// =====================================================================================
//
//       Filename:  online.hpp
// 
//    	 Description: specify the communication between online and switch 
// 
//       Version:  1.0
//       Created:  12/26/2011 08:35:48 PM CST
//       Compiler:  g++
// 
//       Company:  TAOMEE
// 
// =====================================================================================


#ifndef CHAT_HPP_
#define CHAT_HPP_

#include <map>
#include <set>
#include <fstream>
#include <list>
#include <libtaomee++/memory/mempool.hpp>

using namespace std;

extern "C" 
{
#include <arpa/inet.h>
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <string.h>
}

/**
  * @brief Define the size of package
  */
enum
{
    pkg_size    = 8192
};

enum talk_type_enum {
	talk_type_cur_map = 0,
	talk_type_tmp_team = 1,
	talk_type_fight_team = 2,
	talk_type_trade = 3,
	talk_type_system = 4,
};

#pragma pack(1)
/**
  * @brief Define the communication protocol between the switch
  *        and online server(or adminer server, login server) 
  */
struct svr_proto_t
{
	uint32_t    len; 
    uint32_t    seq; 
    uint16_t    cmd; 
    uint32_t    ret; 
    uint32_t	sender_id; 
    uint8_t     body[];
};

/**
  * @brief Define the talk pkg recv protocol 
  */
struct talk_msg_in_head_t
{
	uint32_t    type; 
    uint32_t    to_id; 
    uint32_t    msg_len; 
    uint8_t     msg[]; //max len is 512
};

/**
  * @brief Define the talk pkg send protocol 
  */
struct talk_msg_out_head_t
{
	uint32_t    from_id;
   	uint32_t 	from_tm;
	uint32_t	from_nick[16];
	
    uint32_t    to_id; 
	uint32_t	type;
    uint32_t    msg_len; 
    uint8_t     msg[]; //max len is 512
};

/**
  * @brief report user on off 
  */
struct user_onoff_info_t
{
	uint32_t    onoff;//0:off, 1:on
   	uint32_t 	user_tm;
	char		nick[16];
    uint32_t    fight_team_id; 
};

struct  transmit_str_in_t {
		uint32_t group_type;
		uint32_t group_id;
		uint32_t len;
		uint8_t body[];
};

struct report_user_base_info_in_t {
	uint32_t user_tm;
	char 	 nick[16];
	uint32_t fight_team_id;
};

#pragma pack()




// =====================================================================================
class PlayerMrg;
class GroupMrg;
class TalkGroupMrg;
class Player;
class ChatMrg 
{
public:
	ChatMrg();
	~ChatMrg();
public:
	int msg_dispatch(void* data, int len, fdsession_t* fdsess);
	int add_player_to_fight_team_group(uint32_t fight_team_id, Player* p);
	int report_user_onoff(uint32_t uid, fdsession_t* fdsess, void* body, int len);
	int report_user_base_info(uint32_t uid, void* body, int len);
	
	int process_chat_msg(uint32_t uid, void* body, int len);
	int transmit_msg(uint32_t uid, void* body, int len);

public:
	void init();
	void final();

public:
	PlayerMrg * p_player_mrg;
	TalkGroupMrg * p_talk_group_mrg;
};

/**
  * @brief send a package to player p
  * @param p player to send a package to
  * @param pkgbuf package to send
  * @param len length of the package
  * @param completed 1 and p->waitcmd will be set 0, 0 and p->waitcmd will remain unchanged
  * @return 0 on success, -1 on error
  */
int send_to_player(Player* p, void* pkgbuf, uint32_t len, int completed);

/**
  * @brief init header of client protocol
  * @param header header to be initialized
  * @param cmd client command id
  * @param len length of the whole protocol package
  * @param ret return code
  */
inline void init_chat_proto_head_full(void* header, uint32_t cmd, uint32_t len, uint32_t ret)
{
	svr_proto_t* p = reinterpret_cast<svr_proto_t*>(header);

	p->len = len;
	p->cmd = cmd;
	p->ret = ret;
}


extern ChatMrg* p_chat_mrg;
#endif // CHAT_HPP

