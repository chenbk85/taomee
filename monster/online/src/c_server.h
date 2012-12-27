/*
 * =====================================================================================
 *
 *       Filename:  account.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月18日 16时53分12秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_ACCOUNT_H_20110718
#define H_ACCOUNT_H_20110718

#include <string.h>

extern "C" 
{
#include <async_serv/service.h>
#include <libtaomee/log.h>
}

#include "user_manager.h"

typedef int (*TIMEOUT_CALLBACK)(void *p_owner, void *p_data);

class c_server
{
public:
    c_server();
    ~c_server();
    int init(int bufsz, int timeout, const char *server_name, const char *ip = NULL, int port = 0);
    int uninit();
    int release();

    int send_data(usr_info_t *p_user, const char *p_data, int data_len, TIMEOUT_CALLBACK timeout_cb);
    int get_server_fd();
    void set_server_fd(); 

private:
    int connect_to_server();

private:
    uint8_t m_inited;

    char m_ip[16];
    int m_port;
    int m_fd;
    int m_bufsz;
    int m_timeout;
    char m_server_name[100];
};

int create_server_instance(c_server **pp_instance);

inline int c_server::get_server_fd()
{
    return m_fd;
}

inline void c_server::set_server_fd()
{
    m_fd = -1;
}

inline int c_server::connect_to_server()
{
    if (m_port != 0)  //通过ip连接
    {
        m_fd = connect_to_svr(m_ip, m_port, m_bufsz, m_timeout);
    }
    else    //通过服务名称连接
    {

        m_fd = connect_to_service(m_server_name, 0, m_bufsz, m_timeout);  
    }
    return m_fd;
}

#endif //H_ACCOUNT_H_20110718
