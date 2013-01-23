/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file c_newbench_io_event.h
 * @author tonyliu <tonyliu@taomee.com>
 */

#ifndef _C_NEWBENCH_IO_EVENT_H_
#define _C_NEWBENCH_IO_EVENT_H_

#include "connect.h"
#include "i_net_io_server.h"

/**
 * @brief  网络读写事件类
 */
class newbench_net_io_event: public i_net_io_event_handler
{
public:
    newbench_net_io_event();
    
    ~newbench_net_io_event();
    
    virtual int init();
    
    virtual int uninit();
    
    virtual int on_new_connection(i_net_io_server *p_net_io_server, const int connection_id, const int connection_fd,
                                  const char *peer_ip, const int peer_port, union net_io_storage *p_storage);

    virtual int on_recv_data(i_net_io_server *p_net_io_server, const int connection_id, const int connection_fd,
                             const char *ip, const int port,
                             char *p_data, int data_len, union net_io_storage *p_storage);

    virtual int on_connection_closed(i_net_io_server *p_net_io_server, const int connection_id, const int connection_fd,
                                     const char *ip, const int port, union net_io_storage *p_storage);

    virtual int on_wakeup(i_net_io_server *p_net_io_server);

private:
    u_int m_max_pkg_len;
    char *m_push_buf;
    int m_push_buf_len;
    shm_block_t* m_push_mb;
};

#endif
