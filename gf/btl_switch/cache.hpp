/*
 * =====================================================================================
 *
 *       Filename:  cache.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/10/2012 05:44:48 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#ifndef CACHE_HPP
#define CACHE_HPP

extern "C" {
#include <stdint.h>
}

enum cache_cmd_t {
  cache_save_team_server_info =  11002,
  cache_get_team_server_info  =  11003,
};

struct cachesvr_proto_t {
	uint32_t len;
	uint32_t seq;
	uint32_t cmd;
	uint32_t ret;
	uint32_t uid;
	uint32_t role_tm;
	uint8_t  body[];
}__attribute__((packed));	

extern int cachesvr_fd;

int send_request_to_cache_svr(uint32_t uid, uint32_t role_tm, uint32_t cmd, void * buf, int len);


void handle_cachesvr_callback(void * data, int len);

#endif


