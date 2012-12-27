/**
 *============================================================
 *  @file      gf_cryptogram.hpp
 *  @brief     for consuming and inquiring gf_cryptogram
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef GF_CRYPTO_NUMBER_HPP__
#define GF_CRYPTO_NUMBER_HPP__

extern "C" {
#include <libtaomee/project/types.h>
}

#include "player.hpp"

enum ts_gf_crypto_proto_t {
	tsproto_view_cryptogram_gift    = 2801, 
	tsproto_consume_cryptogram_gift = 2802, 
};

/**/
#define MAX_GIFT_LIMIT 30

/* gf_cryptogram hash length */
const uint32_t max_hash_len = 32;

#pragma pack(1)
/**
  * @brief gf_cryptogram server protocol
  */
struct gf_cryptogram_proto_t {
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
 * @brief consum cryptogram gift body header
 */
struct cryptogram_in_header{
    uint8_t     hash_num[max_hash_len];
    uint8_t     cryptogram_num[max_hash_len];
    uint32_t    userip;
    uint32_t    max_bag;
    uint32_t    role_tm;
};
struct view_gift_t {
    uint32_t    itemid;
    uint32_t    item_cnt;
    uint32_t    itemtype;
};

struct consume_gift_t {
    uint32_t    itemid;
    uint32_t    item_cnt;
    uint32_t    itemtype;
    uint32_t    uniqueid;
};
#pragma pack()


/*! file descriptor for connection with gf_cryptogram server */
//extern int gf_cryptogram_svr_fd;

//---------------package sending and receiving interface with gf_cryptogram server-------------------------
/**
  * @brief uniform interface to send request to tiemspace server
  * @param p sender 
  * @param cmd command id with mc server 
  * @param body package body
  * @param bodylen package body length
  * @return 0 on success, -1 on error
  */
int send_request_to_gf_cryptogram_svr(const player_t* p, uint16_t cmd, const uint8_t* body, uint32_t bodylen);

/**
  * @brief uniform interface to process on return from gf_cryptogram server
  * @param pkg returned package
  * @param pkglen package length
  */
void handle_gf_cryptogram_svr_return(gf_cryptogram_proto_t* pkg, uint32_t pkglen);


//-----------------------------------------------------------------------------------------------------
// Cmds from client processings
//-----------------------------------------------------------------------------------------------------
/**
  * @brief player view gf_cryptogram for gifts
  * @param p the player who view the code
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int ts_view_cryptogram_gift_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
/**
  * @brief player consumes gf_cryptogram for gifts
  * @param p the player who consumes the code
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int ts_consume_cryptogram_gift_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
//-----------------------------------------------------------------------------------------------------


//------- Functions for processing  packages received from gf_cryptogram server and responsing to client -------------
/**
  * @brief process on view gf_cryptogram
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from gf_cryptogram server
  * @return 0 on success, -1 on error
  */
int ts_view_cryptogram_gift_callback(player_t* p, const void* body, uint32_t bodylen, uint32_t ret);
/**
  * @brief process on consuming gf_cryptogram
  * @param  p the player
  * @param body package body
  * @param bodylen package body length
  * @param ret return value from gf_cryptogram server
  * @return 0 on success, -1 on error
  */
int ts_consume_cryptogram_gift_callback(player_t* p, const void* body, uint32_t bodylen, uint32_t ret);


#endif // GF_CRYPTO_NUMBER_HPP_


