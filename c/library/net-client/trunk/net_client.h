/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file net_client.h
 * @author richard <richard@taomee.com>
 * @date 2011-07-14
 */

#ifndef NET_CLIENT_H_2011_07_14
#define NET_CLIENT_H_2011_07_14

#include <stdint.h>

#include "i_net_client.h"

class net_client : public i_net_client
{
public:
    net_client();
    virtual ~net_client();

    virtual int init(const char *server_ip, int server_port);
    
    virtual int open_conn(struct timeval *timeout);
    
    virtual int send_rqst(const char *rqst_msg, char *resp_buf, int buf_len, 
                          struct timeval *timeout,
                          byte_count_t byte_count = FOUR_BYTE, 
                          byte_order_t byte_order = HOST_BYTE_ORDER);

    virtual int close_conn();

    virtual const char * get_last_error();

    virtual int uninit();

private:
    bool m_inited;
    char m_server_ip[16];
    uint16_t m_server_port;
    char m_error_str[1024];
    int m_sockfd;
};

#endif /* NET_CLIENT_H_2011_07_14 */
