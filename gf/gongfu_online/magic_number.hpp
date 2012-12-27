/**
 *============================================================
 *  @file      magic_number.hpp
 *  @brief     for consuming and inquiring magic_number
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef GF_MAGIC_NUMBER_HPP__
#define GF_MAGIC_NUMBER_HPP__

extern "C" {
#include <libtaomee/project/types.h>
}

#include "player.hpp"

enum ts_proto_t {
	tsproto_view_magic_gift    = 2701, 
	tsproto_consume_magic_gift = 2702, 
};

/* magic_number hash length */
const uint32_t c_ts_hash_len = 32;

/* magic_number hash length */
const uint32_t c_ts_min_len = 12;

/* magic_number code length */
const uint32_t c_ts_max_len = 32;

#pragma pack(1)
/**
  * @brief magic_number server protocol
  */
struct magic_number_proto_t {
	/*! package length */
	uint32_t	pkglen;
	/*! sequence number ((fd << 16) | waitcmd) */
	uint32_t	seq;
	/*! command id */
	uint16_t	cmd;
	/*! errno */
	uint32_t	ret;
	/*! user id */
	userid_t	id;
	/*! package body */
	uint8_t		body[];
};
/**
 * @brief consum magic gift body header
 */
struct magic_in_header{
    uint8_t     magic_num[c_ts_hash_len];
    //uint32_t    userid;
    //uint32_t    roletm;
    //uint32_t    userip;
    uint32_t    chosecount;
};
/**
 * @brief consum magic gift body header
 */
struct magic_out_header{
    //uint32_t    flag;
    uint32_t    item_cnt;
    //uint32_t    attire_cnt;
};
/**
 * @brief consum magic gift body item
 */
struct magic_in_item {
    uint32_t    itemid;
    uint16_t    item_cnt;
};

struct magic_out_item{
    uint32_t    itemid;
    uint32_t    uniqueid;//nonsense
    uint32_t    item_cnt;
};

#pragma pack()


/*! file descriptor for connection with magic_number server */
extern int magic_number_svr_fd;

//---------------package sending and receiving interface with magic_number server-------------------------
/**
  * @brief uniform interface to send request to tiemspace server
  * @param p sender 
  * @param cmd command id with mc server 
  * @param body package body
  * @param bodylen package body length
  * @return 0 on success, -1 on error
  */
int send_request_to_magic_number_svr(const player_t* p, uint16_t cmd, const uint8_t* body, uint32_t bodylen);

/**
  * @brief uniform interface to process on return from magic_number server
  * @param pkg returned package
  * @param pkglen package length
  */
void handle_magic_number_svr_return(magic_number_proto_t* pkg, uint32_t pkglen);


//-----------------------------------------------------------------------------------------------------
// Cmds from client processings
//-----------------------------------------------------------------------------------------------------
/**
  * @brief player view magic_number for gifts
  * @param p the player who view the code
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int ts_view_magic_gift_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief player consumes magic_number for gifts
  * @param p the player who consumes the code
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int ts_consume_magic_gift_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
//-----------------------------------------------------------------------------------------------------


//------- Functions for processing  packages received from magic_number server and responsing to client -------------
/**
  * @brief process on view magic_number
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic_number server
  * @return 0 on success, -1 on error
  */
int ts_view_magic_gift_callback(player_t* p, const void* body, uint32_t bodylen, uint32_t ret);
/**
  * @brief process on consuming magic_number
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from magic_number server
  * @return 0 on success, -1 on error
  */
int ts_consume_magic_gift_callback(player_t* p, const void* body, uint32_t bodylen, uint32_t ret);


#endif // GF_MAGIC_NUMBER_HPP_


