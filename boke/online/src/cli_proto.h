
#ifndef MOLE2_CLI_PROTO_H
#define MOLE2_CLI_PROTO_H

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <async_serv/async_serv.h>
#ifdef __cplusplus
}
#endif
//新的命令分发
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>
#include "./proto/pop_online.h" 
#include "./proto/pop_online_enum.h" 

#include "benchapi.h"
#include "dbproxy.h"



typedef struct cached_pkg{
	uint16_t	len;
	uint8_t		pkg[];
}__attribute__((packed))cached_pkg_t;


enum {
	// we return errno from dbproxy by plusing 100000 to dberr (100000 + dberr)
	cli_err_base_dberr		= 100000,
	cli_err_base_codeerr	= 300000,
};

void init_cli_handle_funs();
int dispatch(void* data, fdsession_t* fdsess,  bool first_tm);

#endif
