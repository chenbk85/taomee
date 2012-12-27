/**
 * ============================================================================
 * @file   benchapi.cpp
 *
 * @brief  实现serverbench提供的hadnle_init, handle_process, handle_input等接口函数
 *
 * compiler : gcc version 4.1.2
 *
 * platfomr : Debian 4.1.1-12
 *
 * copyright : TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ============================================================================
 */



#include <cstdio>
#include <cstring>
#include <cstdlib>

extern "C"{
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>

#include <serverbench/benchapi.h>

#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>

}
#include "./pkg/Cclientproto.h"

#include "base.h"
#include "cmdprocess.h"
#include "packet.h"
#include "globalvar.h"
#include "user_time.h"


/*存储返回给客户端的信息*/
char g_buffer[1024 * 1024];


void sigusr1(int signo)
{
	config_init("/home/miller/cache_svr/conf/bench.conf");
	log_init("/home/miller/cache_svr/log", static_cast<log_lvl_t>(config_get_intval("log_level", log_lvl_trace)),
							config_get_intval("log_size", (1 << 30)), (log_lvl_t)config_get_intval("log_maxfiles", 100),
											config_get_strval("log_prefix"));
}

/**
 * @brief   初始化配置文件，游戏flag、game_id,游戏分数和用户时间等相关文件
 * @param   argc, 参数的数目
 * @param   argc, 指向参数的指针
 * @param   type, 命令参数类型
 * @return  0, 成功;-1, 失败
 */
extern "C"
int handle_init(int argc, char **argv, int type)
{
	uint32_t file_size = sizeof(gs_mem_t) * max_game;
	switch (type) {
	case PROC_MAIN:
		/*initialize configuration file*/
		config_init(argv[1]);
		max_user = config_get_intval("max_user_id", 0);
		max_game = config_get_intval("max_game_id", 0);
		/*get timer interval time*/
		config_time_step = config_get_intval("timer_step_time", 0);
		
	    file_size = sizeof(gs_mem_t) * max_game;

		/*open game score file*/
		gm_score_fd = open(config_get_strval("game_score_file"), O_CREAT | O_RDWR,
				           S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); 

		if (gm_score_fd == -1) {
			BOOT_LOG(-1, "handle_init : fail to open %s", config_get_strval("game_score_file"));
		}

		/*make sure this file can contain score*/
		if (lseek(gm_score_fd, file_size, SEEK_SET) == -1){
			BOOT_LOG(-1, "handle_init : fail to enlarge the score file");
		}
	
		/*write data to the end*/
		if (write(gm_score_fd, "", 1) == -1) {
			BOOT_LOG(-1, "handle_init : fail to write score file");
		}

		if (lseek(gm_score_fd, 0, SEEK_SET)) {
			BOOT_LOG(-1, "handle_init : fail to move file pointer");
		}

		/*map game score file*/
		gm_score_map = mmap(0, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, gm_score_fd, 0);
		if (gm_score_map == MAP_FAILED) {
			BOOT_LOG(-1, "handle_init : fail to map %s file", config_get_strval("game_score_file"));
		}
		gmsc_adr = (gs_mem_ptr_t)gm_score_map;
	
		cpo = new Cclientproto(config_get_strval("dbproxy_ip"), config_get_intval("dbproxy_port", 0));

		if ((gmid_array = (gmid_ptr_t)malloc(sizeof(gmid_list) * max_game)) == NULL) {
			BOOT_LOG(-1, "fail to malloc for game id");
		}
		
		/*get game flag and id from file*/
		if (get_game_id(gmid_array, config_get_strval("game_id_file")) < 0) {
			BOOT_LOG(-1, "handle_init : fail to initilaze the game id and flag");
		}
		
		if (init_game_score(cpo, gmid_array) < 0) {
			BOOT_LOG(-1, "handle_init : fail to init the game score");
		}
		struct sigaction sa;
		sa.sa_handler = sigusr1;
    	sigemptyset(&sa.sa_mask);
    	sigaddset(&sa.sa_mask, SIGHUP);
    	sa.sa_flags = 0;
    	if (sigaction(SIGHUP, &sa, NULL) < 0)
   	 	{

        	exit(1);
		}

		break;

	case PROC_WORK:
		/*open time file and map time file map*/
		time_file_fd = open(config_get_strval("timestamp_file"), O_CREAT | O_RDWR,
				            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (time_file_fd == -1) {
			BOOT_LOG(-1, "handle_init : fail to open %s", config_get_strval("timestamp_file"));
		}

		/*make sure this file can contain score*/
		if (lseek(time_file_fd, sizeof(nick_jy_lamu) * max_user, SEEK_SET) == -1) {
			BOOT_LOG(-1, "handle_init : fail to enlarge the score file");
		}
	
		/*write data to the end*/
		if (write(time_file_fd, "", 1) == -1) {
			BOOT_LOG(-1, "handle_init : fail to write score file");
		}

		if (lseek(time_file_fd, 0, SEEK_SET)) {
			BOOT_LOG(-1, "handle_init : fail to move file pointer");
		}
		time_map_file = mmap(0, sizeof(nick_jy_lamu) * max_user, PROT_READ | PROT_WRITE, MAP_SHARED, time_file_fd, 0);
		if (time_map_file == MAP_FAILED) {
			BOOT_LOG(-1, "handle_init : fail to map %s", config_get_strval("timestamp_file"));
		}
		time_mem = (nick_jy_lamu *)time_map_file;
		break;

	case PROC_CONN:
	 	break;

	default:
		break;
    }
    
    return 0;
}



/**
 * @brief   检查输入的字符串是否符合长度要求及是否是FLASH请求
 * @param   buffer, 输入的字符串指针
 * @param   length, 字符串的长度
 * @param   sk, 保存socket的相关信息
 * @return  0, 数据长度达不到要求或者是FLASH请求
 * @return  -1, 发生错误; reallen, 字符串的实际长度
 */
extern "C"
int handle_input(const char *buffer, int length, const skinfo_t *sk)
{
    int reallen;
    const char *policy =   "<policy-file-request/>";
    const char *response = "<?xml version=\"1.0\"?>"
                           "<!DOCTYPE cross-domain-policy SYSTEM\"/xml/dtds/cross-domain-policy.dtd\">"
			   			   "<cross-domain-policy>"
			   			   "<site-control permitted-cross-domain-policies=\"all\"/>"
			   			   "<allow-access-from domain=\"*\"to-ports=\"*\"/>"
			   			   "</cross-domain-policy>";


    if (length < MIN_HEAD_LENGTH) {
        return 0;
    }
	
    /*send response to flash*/
    if (length == FLASH_REQ && !memcmp(buffer, policy, FLASH_REQ)) {
        send(sk->sockfd, response, strlen(response) + 1, 0);
		return 0;
    }

	/*get real length*/
    reallen = ntohl(*((uint32_t*)buffer)); 
    if (reallen < MIN_LENGTH || reallen > MAX_LENGTH) {
		ERROR_LOG("the length is %d, the ip is %x", reallen, sk->remote_ip);
		proto_head_t *temp;
		temp = (proto_head_ptr_t)buffer;
		ERROR_LOG("command id %u, user id %u, parameter length %d", 
				ntohl(temp->cmd_id), ntohl(temp->user_id), length);
        return -1;
    }

    return reallen;
}
  


/**
 * @brief  根据命令类型调用相应的处理函数
 * @param  recvbuf 接收字符串指针
 * @param  rcvlen 接收的字符串长度
 * @param  sendbuf 发送的字符串的指针
 * @param  sndlen  存储发送的字符串长度
 * @param  sk 保存socket的信息
 * @return -1 执行失败;0 执行成功
 */
extern "C"
int handle_process(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen, const skinfo_t *sk)
{
    int ret;
    proto_head_t *head;

	/*get client ip address*/
    client_ip = sk->remote_ip;
	*sendbuf = g_buffer;
	/*deal with the command*/
	uint32_t req_cmd = ntohl((((proto_head_ptr_t)recvbuf))->cmd_id);
	if (req_cmd == 30000) {
		*sndlen = PROTO_HEAD_SIZE;
		 /*copy head to sendbuf*/
		 memcpy(*sendbuf, recvbuf, *sndlen);
		 head = (proto_head_ptr_t)(*sendbuf);
		 head->len = htonl(*sndlen);
		 head->result = 0;
		 return 0;
	}
    ret = process_command(recvbuf, rcvlen, *sendbuf, sndlen);
	/*if fail, send a packet to the client*/
    if (ret != SUCCESS) {
        *sndlen = PROTO_HEAD_SIZE;
		/*copy head to sendbuf*/
		memcpy(*sendbuf, recvbuf, *sndlen); 
		head = (proto_head_ptr_t)(*sendbuf);
		head->len = htonl(*sndlen);
		head->result = htonl(ret);
		return 0;
    }
    return 0;
}



/**
 *  @brief  定时器调用此函数
 *  @param  sec, 定时器的时间间隔
 *  @return 0
 */
extern "C"
int handle_timer(int *sec)
{
	static int time_step;
	*sec = 60;
	if (time_step + *sec > config_time_step) {
		if (init_game_score(cpo, gmid_array) < 0) {
			ERROR_LOG("fail to refresh the game score");
		}
		time_step = 0;
	} else {
		time_step += *sec;
	}

	return 0;
}
