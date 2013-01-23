/**
 *====================================================================
 *     @file	serverbench_interface.cpp
 *    @brief	衔接workbench接口
 * 
 *  @internal
 *   @created	13:3:2009   17:04:25
 *   @version   1.0.0.0
 *   compiler   gcc/g++
 *   platform 	linux
 *
 *    @author	aceway taomee  www.taomee.com
 *   @company   TaoMee,Inc. ShangHai CN (www.taomee.com)
 *
 * @copyright	2008 TaoMee, Inc. ShangHai CN. All right reserved.
 *====================================================================
 */


#include "serverbench_interface.h"
#include "collector_server.h"


/** 
 * @brief  初始化程序接口, 第一、二个参数是main()函数的对应参数。
 * @param  int argc, 同main()函数的 argc; 
 * @param  char **argv, 同main()函数的 argv; 
 * @param  int pid_type, 运行的进程类型： 0:PROC_MAIN, 1:PROC_CONN, 2:PROC_WORK;
 * @return 返回0－正常继续运行程序， 返回-1通知上层来结束程序。 
 */
extern "C" int handle_init (int argc, char **argv, int pid_type)
{
    switch (pid_type) 
    {
        case PROC_MAIN:
            DEBUG_LOG ("################case PROC_MAIN:");
            return 0;
        case PROC_WORK:
            DEBUG_LOG ("################case PROC_WORK:");
            return process_init();
        case PROC_CONN:
            DEBUG_LOG ("################case PROC_CONN:");
            return 0;
        default:
            DEBUG_LOG ("################case default:");
            DEBUG_LOG("##handle_init(arc=%d, argv=%p, pid_type=%d)\n\t\t [0:PROC_MAIN,1:PROC_CONN,2:PROC_WORK]", 
                    argc, argv, pid_type);
            return -1;
    }
    return -1;
}

/** 
 * @brief  按协议处理接收数据前，处理socket接收数据的钩子。
 * @param  const char* buffer,  已经接收保存了数据的socket缓冲区;
 * @param  int length, 缓冲区中已经收到的数据字节长度; 
 * @param  const skinfo_t *sk, 关于本次连接信息的结构体；
 * @return  返回－1表示数据异常，通知上层结束掉本次连接,      返回0， 当前不能确认协议包长度，需要继续接收数据，大于零表示本次通信的数据包长度(协议级的)。  
 */
extern "C" int handle_input (const char* buffer, int length, const skinfo_t *sk)
{
#ifdef  __DEBUG__
            printf("%sSerrver handle input !%s\n", GRN_TIP, END_CLR);
#endif
        return process_input(buffer, length, sk);
}

/** 
 * @brief  按协议处理接收缓冲区的数据, 并作出反映.
 * @param  char *recvbuf, 已经接收保存了数据的socket缓冲区; 
 * @param  int rcvlen,  缓冲区中已经收到的数据字节长度;
 * @param  char **sendbuf,  即将被发送的数据保存的内存的地址； 
 * @param  int *sndlen, 即将被发送的数据的长度; 
 * @param  const skinfo_t *sk,  关于本次连接信息的结构体；
 * @return  返回－1表示通知上层结束掉本次连接，0 表示保存本次连接继续通信。
 * @note  如果不对该次通信回应数据，必须将sendlen显式设置为0。
 */
extern "C" int handle_process (char *recvbuf, int rcvlen, char **sendbuf, int *sndlen, const skinfo_t *sk)
{
    char buffer[MAX_SNDBUF_LEN] = {'\0'};
    memcpy(buffer, recvbuf, rcvlen);
    *sndlen = 0;
    return process_info(buffer, rcvlen, sendbuf, sndlen, sk);
}


/** 
 * @brief  程序即将结束，清理现场。
 * @param   int a
 * @return no   
 */
extern "C" void handle_fini(int a)
{
#ifdef  __DEBUG__
            printf("%sServer handle finish !%s\n", GRN_TIP, END_CLR);
#endif
    process_finish(a);
}


/** 
 * @brief  本次连接断开前提供的钩子函数。
 * @param  const skinfo_t* sk, 本次连接的信息结构体; 
 * @return no  
 */
extern "C" void handle_close(const skinfo_t* sk)
{
#ifdef  __DEBUG__
            printf("%sServer handle close !%s\n", GRN_TIP, END_CLR);
#endif
}

