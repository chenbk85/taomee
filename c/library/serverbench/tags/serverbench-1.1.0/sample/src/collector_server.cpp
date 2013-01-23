/**
 * =====================================================================================
 *       @file  collector_server.cpp
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/10/2009 02:52:18 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <errno.h>
#include "collector_server.h"
#include "db_operator.h"
#include "collector_config.h"
extern s_config_variable        g_config_variable;

int32_t process_init()
{
    int32_t ret_code = -1; 
    //Init programm config data
    ret_code = config_variable_init();
    if(0 != ret_code) 
    {   
        return ret_code;
    }   

    //Init mysql
    ret_code = mysql_db_init(); 
    if(0 != ret_code) 
    {   
        return ret_code;
    }   
    return 0;
}

/** 
 * @brief 对应server bench api的 handle_input();
 * @param  const char *buffer,  同server bench api的 handle_input();
 * @param  const int32_t length,  同server bench api的 handle_input();
 * @param  const skinfo_t *sk, 同server bench api的 handle_input(); 
 * @return  返回－1表示数据异常，通知上层结束掉本次连接,      返回0，                                                               
当前不能确认协议包长度，需要继续接收数据，大于零表示本次通信的数据包长度(协议级的)。  
 */
int32_t process_input(const char *buffer, const int32_t length, const skinfo_t *sk)
{
#ifdef  __DEBUG__
    printf("%s process_input()!%s\n", GRN_TIP, END_CLR);
#endif
    if((uint32_t)length < sizeof(s_pkg_header))
    {   
        DEBUG_LOG("process_input() length < %s (pakage header len),  return 0, continue to recieve.", sizeof(s_pkg_header));
        return 0;
    }   
    uint16_t package_len = *(uint16_t*)buffer;
    if(length < package_len)
    {   
        DEBUG_LOG("process_input() length < %s (pakage header len),  return 0, continue to recieve.", sizeof(s_pkg_header));
        return 0;
    }   
    if((uint32_t)package_len < sizeof(s_pkg_header))
    {
        ERROR_LOG("process_input(), package len in header len=%d is too small, header len=%u, return -1.", package_len, sizeof(s_pkg_header));
        return -1;
    }
    if((uint32_t)package_len > MAX_SNDBUF_LEN)
    {
        ERROR_LOG("process_input(), package len in header len=%d > max=%u,  return -1.", package_len,
MAX_SNDBUF_LEN);
        return -1;
    }
    if((uint32_t)length > MAX_SNDBUF_LEN) 
    {   
        ERROR_LOG("process_input(), length is longer than max , len=%d > max=%u,  return -1.",
                  length, MAX_SNDBUF_LEN);
        return -1; 
    }
    return package_len;
}

/** 
 * @brief 对应server bench api的 handle_process(), 按本模块协议处理客户端来的请求;
 * @param  const char *buffer,  同server bench api的 handle_process();
 * @param  const int32_t length,  同server bench api的 handle_process();
 * @param  const skinfo_t *sk, 同server bench api的 handle_process(); 
 * @return  返回－1表示通知上层结束掉本次连接，0 表示保存本次连接继续通信。
 * @note  如果不对该次通信回应数据，必须将sendlen显式设置为0。
 */ 
int32_t process_info(char *recvbuf, const int32_t rcvlen, char **sendbuf, int32_t *sndlen, const skinfo_t *sk)
{
    *sndlen = 0;
    s_pkg_header *pheader = (s_pkg_header*)recvbuf;
    int32_t ret_code = -1;
    
    switch(pheader->cmd_id)
    {
        case e_login_ip:
            ret_code = process_login_info(recvbuf + sizeof(s_pkg_header), pheader->pkg_len - sizeof(s_pkg_header));
            break;
        default:
            ERROR_LOG("process_info() command id error: [%x].", pheader->cmd_id);
            ret_code = -1;
    }
    return ret_code;
}

int32_t process_login_info(const char *p_info_body, const uint16_t info_len)
{
    bool len_state = (info_len % sizeof(s_login_info)) == 0;
    uint16_t count = info_len / sizeof(s_login_info);
    if(len_state)
    {
        uint8_t index = 0;
        //printf("%sReceive login info, data len %u, count: %u!%s\n", GRN_TIP, info_len, count, END_CLR);
        for(index = 0; index < count; ++index)
        {
            s_login_info *p_info_array = (s_login_info*)p_info_body;
            db_process_login_info(p_info_array[index]);
#ifdef  __DEBUG__
            printf("(mimi, item, ip, time)---(%u, %u, %u, %u)\n", 
                    p_info_array[index].mimi_number,  p_info_array[index].login_item,
                    p_info_array[index].login_ip,  p_info_array[index].login_time);
            DEBUG_LOG("(mimi, item, ip, time)---(%u, %u, %u, %u)\n", 
                    p_info_array[index].mimi_number,  p_info_array[index].login_item,
                    p_info_array[index].login_ip,  p_info_array[index].login_time);
#endif
        }
    }
    else
    {
        //printf("%sReceive login info data len ERROR!%s\n", GRN_TIP, END_CLR);
        ERROR_LOG("process_login_info() data len and info count error, total len = %u, len per info = %u.", 
                    info_len, sizeof(s_login_info));
    }
    return 0;
}

int32_t process_finish(int param)
{
    mysql_db_finish();
    memset(&g_config_variable, 0, sizeof(s_config_variable));
    return 0;
}

