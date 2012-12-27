// =====================================================================================
// 
//       Filename:  process.hpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  07/07/2010 04:05:59 PM CST
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  xcwen (xcwen), jim@taomee.com
//        Company:  TAOMEE
// 
// =====================================================================================

#ifndef  PROCESS_INC
#define  PROCESS_INC

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>
#include <async_serv/net_if.h>
}

typedef struct proto_header{
	/*包的全部长度*/
	uint32_t proto_len;
	/*协议版本号*/
	uint32_t proto_id; 
	/*命令编号*/
	uint16_t cmd_id; 
	/*错误码*/
	int32_t result;
	/*用户米米号*/
	uint32_t uid;  
}__attribute__((packed)) proto_header_t;

int process(void *data, fdsession_t* fdsess);

int process_init();

int process_fini();
#endif   // ----- #ifndef PROCESS_INC  ----- 

