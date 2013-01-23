/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file i_net_client.h
 * @author richard <richard@taomee.com>
 * @date 2011-07-14
 */

#ifndef I_NET_CLIENT_2011_07_14
#define I_NET_CLIENT_2011_07_14

#include <memory>                                          // for std::shared_ptr
#include <sys/time.h>                                      // for struct timeval

class i_net_client
{
public:
    // The byte count occupied by the message length in the header.
    typedef enum byte_count { 
        TWO_BYTE,
        FOUR_BYTE
    } byte_count_t;

    // The message length's byte order in the header.
    typedef enum byte_order {     // the msg's length
        HOST_BYTE_ORDER,
        NET_BYTE_ORDER
    } byte_order_t;

    virtual ~i_net_client() = 0;

    virtual int init(const char *server_ip, int server_port) = 0;
    
    virtual int open_conn(struct timeval *timeout) = 0;

    virtual int send_rqst(const char *rqst_msg, char *resp_buf, int buf_len, 
                          struct timeval *timeout,
                          byte_count_t byte_count = FOUR_BYTE, 
                          byte_order_t byte_order = HOST_BYTE_ORDER) = 0;

    virtual int close_conn() = 0;

    virtual const char * get_last_error() = 0;

    virtual int uninit() = 0;
};

std::shared_ptr<i_net_client> create_net_client_instance();

#endif /* I_NET_CLIENT_2011_07_14 */
