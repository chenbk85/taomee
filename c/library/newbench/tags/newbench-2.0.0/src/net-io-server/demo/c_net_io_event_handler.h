#ifndef __C_NET_IO_EVENT_HANDLER_H__
#define __C_NET_IO_EVENT_HANDLER_H__

#include "i_net_io_server.h"

class c_net_io_event_handler: public i_net_io_event_handler
{
public:
    int on_new_connection(void *p_net_io_server,
                          int id,
                          int fd,
                          const char *ip,
                          int port,
                          union net_io_storage *p_storage);

    int on_recv_data(void *p_net_io_server,
                     int id,
                     int fd,
                     const char *ip,
                     int port,
                     char *p_data,
                     int data_len,
                     union net_io_storage *p_storage);

    int on_connection_closed(void *p_net_io_server,
                             int id,
                             int fd,
                             const char *ip,
                             int port,
                             union net_io_storage *p_storage);

    int on_wakeup(void *p_net_io_server);
};

#endif //!__C_NET_IO_EVENT_HANDLER_H__
