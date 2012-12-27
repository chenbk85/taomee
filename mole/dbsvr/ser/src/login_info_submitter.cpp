/**
 * =====================================================================================
 *       @file  login_info_submitter.cpp
 *      @brief  收集米米号登录信息，自动发送到收集信息的服务端,客户代码
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/10/2009 03:48:08 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include "login_info_submitter.h"
#include <algorithm>

using namespace std;

inline bool udpclientcls::init()
{
    memset(&m_svraddr, 0, sizeof(m_svraddr)); 

    m_svraddr.sin_family = AF_INET; /* host byte order */
    m_svraddr.sin_port = htons(m_server_port); /* short, network byte order */
    char ip[256] = {'\0'};

    if(m_server_ip.length() > 16)
    {
        perror("ip format too long.");
        m_sockfd = -1;
        return false;
    }
    uint8_t len = m_server_ip.length();
    memcpy(ip, m_server_ip.c_str(), len);
    inet_pton(AF_INET, ip, &m_svraddr.sin_addr);
    if((m_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket(AF_INET, SOCK_DGRAM, 0) failed");
        m_sockfd = -1;
        return false;
    }
    return true;
}

inline bool udpclientcls::send(const string &msg)
{
    if(m_sockfd == -1)
    {
        init();
    }
    int32_t numbytes = sendto(m_sockfd, msg.c_str(), msg.length(), MSG_DONTWAIT, NULL, 0);
    if(numbytes == -1)
    {
	//	close(m_sockfd);
    //    m_sockfd = -1;
        perror("send string message failed");
        return false;
    }
    return true;
}

inline bool udpclientcls::send(const char msg_buffer[], const uint32_t len)
{
    if(m_sockfd == -1)
    {
        init();
    }
    socklen_t to_len = sizeof(m_svraddr);
    int32_t numbytes = sendto(m_sockfd, msg_buffer, len, MSG_DONTWAIT, (const sockaddr*)&m_svraddr, to_len);
    if(numbytes == -1)
    {
//		close(m_sockfd);
      //  m_sockfd = -1;
        perror("send buffer message failed");
        return false;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
inline void login_info_submitter::submit_login_info(const s_login_info &login_info)
{
    m_vectorinfo.push_back(login_info);

    if(m_vectorinfo.size() == 1)
    {
       m_first_sub_time = time(NULL);
    }

    time_t now = time(NULL);
    if(m_vectorinfo.size() >= m_max_cache_count || (now - m_first_sub_time) >= m_max_cache_time)
    {
        m_first_sub_time = time(NULL);
        send_data();
    }
}

inline void login_info_submitter::send_data()
{
    if(m_vectorinfo.size() < 1) return;
//////////////////////////////////////////////////
    char buffer[sizeof(s_pkg_header) + sizeof(s_login_info) * g_max_cache_count] = {'\0'};
    ((s_pkg_header*)buffer)->pkg_len = sizeof(s_pkg_header) + sizeof(s_login_info) * m_vectorinfo.size();
    ((s_pkg_header*)buffer)->cmd_id = (uint8_t)e_login_ip;

    copy(m_vectorinfo.begin(), m_vectorinfo.end(), (s_login_info*)(buffer+sizeof(s_pkg_header)));
    m_statsvr.send(buffer, ((s_pkg_header*)buffer)->pkg_len);
//////////////////////////////////////////////////
    m_vectorinfo.clear();
}

void send_login_data(const uint32_t &mimi_number, const uint8_t &login_item, const uint32_t &login_ip, const time_t &login_time)
{
    s_login_info tmp_info;
    tmp_info.mimi_number = mimi_number;
    tmp_info.login_item = login_item;
    tmp_info.login_ip = login_ip;
    tmp_info.login_time = (uint32_t)login_time;

    static login_info_submitter submitter;
    submitter.submit_login_info(tmp_info);
}

