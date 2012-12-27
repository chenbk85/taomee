/**
 * ====================================================================
 *  @file       cmdprocess.h
 *
 *  @brief      定义函数指针和申明命令处理函数
 *
 *  platform   Debian 4.1.1-12 
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 * ====================================================================
 */  



#ifndef  CMPROCESS_INC
#define  CMPROCESS_INC


/*解密session的key*/
#ifdef TW_VERSION
	#define GS_KEY  ",.ta0me>" 
#else
	#define GS_KEY ">ta:mee<"
#endif

/*最大数目的命令处理函数*/
#define GM_FUNC_COUNT 100
/**/
#define CMD_ERR -100

/*定义函数指针*/
typedef int (*fun_pointer_t)(char *recvbuf, int rcvlen, char *sendbuf, int *sndlen);

int process_command(char *recvbuf, int rcvlen, char *sendbuf, int *sndlen);

#endif   /* ----- #ifndef CMPROCESS_INC  ----- */

