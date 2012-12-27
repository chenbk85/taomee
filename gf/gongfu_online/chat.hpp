/**
 *============================================================
 *  @file      message.hpp
 *  @brief    message related functions are declared here. (talk_cmd, ...)
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_CHAT_HPP_
#define KF_CHAT_HPP_ 

#include "fwd_decl.hpp"

enum talk_type_enum {
	talk_type_cur_map = 0,
	talk_type_tmp_team = 1,
	talk_type_fight_team = 2,
	talk_type_trade = 3,
	talk_type_system = 4,
	talk_type_home = 5,
};

#pragma pack(1)
/**
  * @brief switch protocol definition
  */
struct chat_proto_t {
	uint32_t	len;
	uint32_t	seq;
	uint16_t	cmd;
	uint32_t	ret;
	uint32_t	sender_id;
	uint8_t		body[];
};

struct transmit_pkg_rsp_t {
	uint32_t talk_type;
	uint32_t group_id;
	uint32_t len;
	uint8_t  body[];
};
#pragma pack()


enum chat_svr_cmd {
	chat_svr_cmd_start = 40000,
	chat_svr_cmd_report_user_onoff = 40001,
	chat_svr_cmd_chat_cross_chat_svr = 40002,
	chat_svr_cmd_transmit_pkg_cross_chat_svr = 40003,
	chat_svr_cmd_report_user_base_info = 40004,
};



enum {
	chat_proto_max_len = 1024 * 20,
};


/* *
 * * @brief Define the talk pkg recv protocol 
 * */
struct talk_msg_in_head_t
{
   uint32_t    type;
   uint32_t    to_id;
   uint32_t    msg_len;
   uint8_t     msg[]; //max len is 512
};


/* *
 *   * @brief Define the talk pkg send protocol 
 *     */
struct talk_msg_out_head_t
{
   uint32_t    from_id;
   uint32_t    from_tm;
   char		   from_nick[16];

   uint32_t    to_id;
   uint32_t    type;
   uint32_t    msg_len;
   uint8_t     msg[]; //max len is 512
};

/**
  * @brief data type for handlers that handle protocol packages from home server
  */
typedef int (*chatsvr_hdlr_t)(player_t* p, chat_proto_t* pkg);

/**
  * @brief data type for handles that handle protocol packages from home server
  */
struct chatsvr_handle_t {
	chatsvr_handle_t(chatsvr_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
		{ hdlr = h; len = l; cmp_method = cmp; }

	chatsvr_hdlr_t		hdlr;
	uint32_t			len;
	uint8_t				cmp_method;
};


/**
  * @brief player talks
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int chat_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief chat cmd callback  from chat svr
 */
int chatsvr_chat_cross_chat_svr_callback(player_t* p, chat_proto_t* hpkg);

//added by cws for chat to knowall
int online_chat_knowall(player_t* p, uint32_t type, uint32_t to_uid, uint32_t msg_len, uint8_t* msg_body);
int find_answer(uint8_t* msg_body, char * answer_msg_body);
int load_quesAns_config(xmlNodePtr cur);
int load_one_answer(xmlNodePtr cur, char* ans);

int online_chat(player_t* p, uint32_t type, uint32_t to_uid, uint32_t msg_len, uint8_t* msg_body);

int report_user_onoff_to_chat_svr(player_t* p, uint32_t onoff);

int report_user_base_info_to_chat_svr(player_t* p, uint32_t talk_group, uint32_t talk_id);

int transmit_pkg_cross_chat_svr(player_t* p, talk_type_enum talk_type, uint32_t group_id, void* pkg, uint32_t len);

/**
 * @brief transmit pkg cmd callback  from chat svr
 */
int chatsvr_transmit_pkg_cross_chat_svr_callback(player_t* p, chat_proto_t* hpkg);

int fight_team_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len);

int temp_team_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len);

int trade_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len);


int tmp_team_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len);

int pack_chat_rsp_pkg(uint8_t* pkg, player_t* from_p, uint32_t to_uid, uint32_t type, uint32_t msg_len, uint8_t* msg_body, bool local_flg = false);

int trade_cur_map_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len);
int home_cur_map_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len);

// inline function

void handle_chat_svr_return(chat_proto_t* data, uint32_t len, int chat_fd);


/**
  * @brief init header of home server protocol
  * @param p the player who is going to send a pkg to home server
  * @param id
  * @param header header to be initialized
  * @param len length of the whole protocol package
  * @param cmd client command id  
  */
inline void init_chat_proto_head(const player_t* p, userid_t id, void* header, int len, uint32_t cmd)
{
	chat_proto_t* pkg = reinterpret_cast<chat_proto_t*>(header);

	pkg->len = len;
	pkg->seq = ((p) ? ( (p->fd) << 16 ) | (p->waitcmd) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->sender_id  = id;
}
bool init_chat_proto_handles();

extern int chat_svr_fd;
#endif // KF_MESSAGE_HPP_
