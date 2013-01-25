#ifndef MOLE2_CODE_H
#define MOLE2_CODE_H

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "svr_proto.h"

#define MAGIC_CODE_LEN		32
/**
  * @brief command id for dbproxy
  */
enum code_cmd_t {
	proto_code_check_code 	= 3001,
	proto_code_use_code		= 3002,
};

/**
  * @brief errno returned from dbproxy
  */
enum code_err_t {
	code_err_parameter    	= 1,
	code_err_not_exsited  	= 2,
	code_err_unactive     	= 3,
	code_err_out_of_date	= 4,
	code_err_frozen			= 5,
	code_err_used_up		= 6,
	code_err_server_busy	= 7,
	code_err_not_authorized	= 8,
	code_err_cnt_max		= 9,
};

/**
  * @brief send a TCP request to code server
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int  send_request_to_code(sprite_t* p, uint32_t cmd, const void* codepkgbuf, uint32_t body_len);
int  send_request_to_spacetime(sprite_t* p, uint32_t cmd, const void* codepkgbuf, uint32_t body_len);

/**
  * @brief send a UDP talk report to db
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
//int send_udp_talk_report_to_db(const player_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

/**
  * @brief handle package return from dbproxy
  * @param dbpkg package from dbproxy
  * @param pkglen length of dbpkg
  */
void handle_magic_return(code_proto_t* cpkg, uint32_t pkglen);
void handle_spacetime_return(code_proto_t* cpkg, uint32_t pkglen);

/**
  * @brief init handles to handle protocol packages from codesvr
  * @return true if all handles are inited successfully, false otherwise
  */
int init_magic_code_proto_handles(int rstart);
int init_spacetime_code_proto_handles(int rstart);

int check_magic_code_cmd(sprite_t* p, uint8_t* body, uint32_t bodylen);
int use_magic_code_cmd(sprite_t* p, uint8_t* body, uint32_t bodylen);

#endif // MOLE2_CODE_H

