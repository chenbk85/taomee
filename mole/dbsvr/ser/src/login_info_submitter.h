/**
 * =====================================================================================
 *       @file  login_info_submitter.h
 *      @brief  收集米米号登录信息客户代码
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/10/2009 03:47:36 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <time.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <assert.h>
extern "C"
{
#include <string.h>
}
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/types.h>

#ifndef _LOGIN_INFO_COLLECTOR_H_
#define _LOGIN_INFO_COLLECTOR_H_

 /**< @brief 收集器的服务端信息 */
const char  g_statsvr_ip[16] = "192.168.0.148";
//const char  g_statsvr_ip[16] = "10.1.1.5";
const unsigned short g_statsvr_port = 52222;


/**< @brief 收集器的本地缓冲性能*/
const uint8_t  g_max_cache_count = 50;
const uint8_t  g_max_cache_time = 60;

#pragma pack(push)
#pragma pack(1)
struct s_pkg_header
{
    uint16_t    pkg_len;
    uint8_t     cmd_id;
};

enum e_protocol_cmd
{
    e_login_ip  = 0x10//
};

struct s_login_info
{
    uint32_t     mimi_number;
    uint8_t      login_item;
    uint32_t     login_ip;
    uint32_t     login_time;
};
#pragma pack(pop)


/**
 * @class udpclientcls
 * @brief 本类只给收集信息的服务端发送数据，不接收数据
 */
class udpclientcls{
    public:
        udpclientcls()
            :m_server_ip(g_statsvr_ip),m_server_port(g_statsvr_port), m_sockfd(-1)
        {
            init();
        };
        udpclientcls(const std::string &server_ip, const unsigned short &server_port)
            :m_server_ip(server_ip),m_server_port(server_port), m_sockfd(-1)
        {
            init();
        };
        ~udpclientcls()
        {
            close(m_sockfd);
        };
        bool send(const std::string &msg);
        bool send(const char msg_buffer[], const uint32_t len);
    private:
        bool init();
    private:
        std::string m_server_ip;
        unsigned short m_server_port;

        int32_t m_sockfd;
        struct sockaddr_in m_svraddr;
};


/**
 * @class login_info_submitter
 * @brief 本类负责收集数据，定时/定量提交给收集的服务端
 */
class login_info_submitter
{
    public:
        login_info_submitter()
            : m_statsvr(g_statsvr_ip, g_statsvr_port), m_max_cache_time(g_max_cache_time), m_max_cache_count(g_max_cache_count)
        {
            assert(m_max_cache_time <= g_max_cache_time);
            assert(m_max_cache_count <= g_max_cache_count);
        };

        login_info_submitter(const time_t max_cache_time, const uint8_t max_cache_count, const std::string svr_ip, const unsigned short svr_port)
            :m_statsvr(svr_ip, svr_port), m_max_cache_time(max_cache_time),m_max_cache_count(max_cache_count) 
        {
            assert(m_max_cache_time <= g_max_cache_time);
            assert(m_max_cache_count <= g_max_cache_count);
        };
        ~login_info_submitter()
        {
            send_data();
        };
        void submit_login_info(const s_login_info &login_info);
    private:
        void send_data();
    private:
        udpclientcls    m_statsvr;
        std::vector<s_login_info> m_vectorinfo;
        time_t  m_first_sub_time;
        time_t  m_max_cache_time;
        uint8_t m_max_cache_count;
};


/** 
 * @brief  提交米米号登录信息，对客户端仅仅需要调用改接口
 * @param  const uint32_t &mimi_number, 米米号
 * @param  const uint8_t &login_item,  米米号登录的项目ID，目前1－摩尔庄园， 2－机器派派，3－大玩国 
 * @param  const uint32_t &login_ip, 米米号登录时用户的IP地址，32位整数形式 
 * @param  const time_t &login_time), 用户登录的时间 
 * @return  
 */
void send_login_data(const uint32_t &mimi_number, const uint8_t &login_item, 
                     const uint32_t &login_ip, const time_t &login_time);
#endif
