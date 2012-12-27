/**
 * ============================================================================
 * @file    cmdprocess.cpp
 *
 * @brief   为benchapi.cpp中的函数提供子函数，包括插入分数，提取分数等。
 *
 * compiler gcc version 4.1.2
 *
 * platfomr Debian 4.1.1-12
 *
 * copyright TaoMee, Inc. ShangHai CN. All rights reserved.
 * copyright TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ============================================================================
 */
 


#include <cstring>
#include <cstdlib>

extern "C"{

#include <arpa/inet.h>
#include <stdint.h>
#include <time.h>

#include <libtaomee/interprocess/filelock.h>
#include <libtaomee/crypt/qdes.h>
#include <serverbench/benchapi.h>
}

#include "./pkg/Cclientproto.h"

#include "packet.h"
#include "globalvar.h"
#include "base.h"
#include "cmdprocess.h"
#include "user_time.h"



/**
 * define function array, if want to add new function,
 * you can add function here
 */
fun_pointer_t cmdmap[GM_FUNC_COUNT + 1] = { NULL, 
											NULL, 
											NULL, 
											NULL,
											NULL,
											NULL,
											NULL,
											time_stamp_update,
											time_stamp_obtain,
								       	  };
/*get the total numbers of funcitons*/
#define DEALFUN_COUNT (sizeof(cmdmap) / sizeof(cmdmap[0]))


/**
 * @brief   根据不同的命令号调用相应的函数
 * @param   recvbuf, 接收到的字符串指针
 * @param   rcvlen, 接收到的字符串长度
 * @param   sendbuf, 发送的字符串的指针
 * @param   sndlen, 保存发送字符串长度的指针
 * @return  0, 执行成功;-1,执行失败
 */
int process_command(char *recvbuf, int rcvlen, char *sendbuf, int *sndlen)
{
	/*get command id*/
	//uint32_t req_cmd = ntohl((((proto_head_ptr_t)recvbuf))->cmd_id);
	uint32_t req_cmd = (((proto_head_ptr_t)recvbuf))->cmd_id;
	DEBUG_LOG("===commandid %u", req_cmd);
	if (req_cmd > DEALFUN_COUNT || cmdmap[req_cmd] == NULL) {
		ERROR_LOG("command id is invalid, %u", req_cmd);
		return CMD_ERR;
	}

    return (cmdmap[req_cmd](recvbuf, rcvlen, sendbuf, sndlen));
}



