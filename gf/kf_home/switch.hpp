// =====================================================================================
//
//       Filename:  switch.hpp
// 
//    	 Description: define the neccessary switch related vriables and dll interface
// 
//       Version:  1.0
//       Created:  03/11/2009 08:25:18 PM CST
//       Compiler:  g++
// 
//       Company:  TAOMEE
// 
// =====================================================================================

#ifndef  SWITCH_HPP_
#define  SWITCH_HPP_
extern "C"
{
#include <stdint.h>
#include <libtaomee/project/types.h>
}

/**
  * @brief Define the size of package
  */
enum
{
    pkg_size    = 8192
};

/**
  * @brief Define base home online id
  */
enum 
{
	home_online_base = 5001
};


/**
  * @brief Define the command for switch server 
  */
enum
{
    /*! Switch-Adminer protocol command */
	adm_proto_post_msg			= 60001,
	adm_proto_kick_user_offline	= 60002,
	adm_proto_syn_vip_flag      = 60003,
	adm_proto_syn_vip_value		= 60004,
	adm_proto_syn_user_gcoins	= 60005,
	adm_proto_sys_mail_noti		= 60006,

    /*! Switch-Online protocol command */
    proto_online_svrinfo        = 61001,
    proto_user_onoff            = 61002,
    proto_users_online_status   = 61003,
    proto_chat_across_svr       = 61004,
    proto_user_attr_upd_noti    = 61005,
    proto_chk_user_location     = 61006,
    proto_kick_user_offline     = 61007,
    proto_online_keepalive      = 61008,
    proto_user_map_upd_noti     = 61009,
    proto_chat_across_svr_2   	= 61010,
	proto_chat_across_svr_mentor_sys   = 61011,
	proto_upd_new_invitee_cnt	= 61012,
	proto_chat_across_svr_team_sys = 61013,
	proto_send_btl_inv			= 61051,
	proto_recv_btl_inv			= 61052,

    /*! Switch-HomeOnline protocol command */
    proto_home_online_info        = 62001,


    /*! Switch-Login protocol command */
    login_proto_get_recommended_svrlist = 63001,
    login_proto_get_ranged_svr_list     = 63002
};

/**
  * @brief Define the reason to kick user offline
  */
enum reason_kick_user_t
{
	reason_system_error	= 10002,
	reason_repeated_login	= 10004
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
    userid_t    sender_id; 
    //uint32_t    sender_role_tm; 
    uint8_t     body[];
};
/**
* @brief online-client protocol
*/
struct online_cli_proto_t {
	/*! package length */
	uint32_t    len;
	/*! protocol version */
	uint8_t     ver;
	/*! protocol command id */
	uint32_t    cmd;
	/*! user id */
	userid_t    id;
	/*! used as an auto-increment sequence number if from client */
	uint32_t    ret;
	/*! body of the package */
	uint8_t     body[];
};
#pragma pack()


// =====================================================================================
/**
  * @brief Set the package head
  * @param buf package buffer
  * @param uid user id of the sender
  * @param cmd command to be excuted
  * @param ret return of error number
  * @param seq sequence number
  */
inline void
init_proto_head(void* buf, userid_t uid, uint32_t len, uint16_t cmd, uint32_t ret, uint32_t seq)
{
    svr_proto_t* header = reinterpret_cast<svr_proto_t*>(buf);
    header->len = len;
    header->seq = seq;
    header->cmd = cmd;
    header->ret = ret;
    header->sender_id = uid;
    //header->sender_role_tm = role_tm;
}

#endif   // ----- #end SWITCH_HPP_  ----- 

