#ifndef NET_CMD_DEF_HPP
#define NET_CMD_DEF_HPP

#include <stdint.h>

enum cli_cmd_t
{
	cli_cmd_begin = 0,
	
	cli_cmd_max
};

enum cli_err_t
{
	cli_err_system_error    = 10002,
    cli_err_system_busy     = 10003
};

enum btl_cmd_t
{
	btl_cmd_start 			= 10000,



	btl_cmd_max             = 13999,
};

enum btl_err_t 
{
	btl_err_begin = 0,
	btl_err_max
};

enum 
{
	btl_proto_max_len    = 8 * 1024
};

enum 
{
	cli_proto_max_len   = 8 * 1024
};

enum 
{
	btlsw_proto_max_len   = 8 * 1024
};

enum 
{
	dbproto_max_len = 16 * 1024	
};



#pragma pack(1)

struct db_proto_t {
	/*! package length */
	uint32_t    len;
	/*! sequence number ((p->fd << 16) | p->waitcmd) */
	uint32_t    seq;
	/*! command id */
	uint16_t    cmd;
	/*! errno */
	uint32_t    ret;
	/*! user id */
	uint32_t   id;
	/*! role create time */
	uint32_t    role_tm;
	/*! package body */
	uint8_t     body[];
};


struct btl_proto_t 
{
	/*! package length */
	uint32_t    len;
	/*! battle group id */
	uint32_t    seq;
	/*! command id */
	uint16_t    cmd;
	/*! errno */
	uint32_t    ret;
	/*! user id */
	uint32_t    id; 
	/*! package body */
	uint8_t     body[];
};

struct cli_proto_t 
{
	/*! package length */
	uint32_t    len;
	/*! protocol command id */
	uint16_t    cmd;
	/*! user id */
	uint32_t    id;
	/*! used as an auto-increment sequence number for checking GameCheater */
	uint32_t    seqno;
	/*! error number: 0 for OK */
	uint32_t    ret;
	/*! body of the package */
	uint8_t     body[];
};


struct btlsw_proto_t 
{
	/*! package length */
	uint32_t    len;
	/*! battle group id */
	uint32_t    seq;
	/*! command id */
	uint16_t    cmd;
	/*! errno */
	uint32_t    ret;
	/*! user id */
	uint32_t    id; 
	/*! char role-reg-time */
	int32_t		role_tm; 
	/*! package body */
	uint8_t     body[];
};


struct cached_pkg_t 
{
	uint32_t    len;
	uint8_t     pkg[];
};



enum cmp_t
{
	cmp_must_eq     = 1,
	cmp_must_ge     = 2, 
};

#pragma pack()























#endif
