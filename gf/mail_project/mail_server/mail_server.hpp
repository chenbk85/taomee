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


enum
{
	///////////////////for online server///////////////////////
	proto_online_svrinfo = 60001,
	proto_online_mail_head_list = 60002,
	proto_online_mail_body      = 60003,
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
}

#endif   // ----- #end SWITCH_HPP_  ----- 

