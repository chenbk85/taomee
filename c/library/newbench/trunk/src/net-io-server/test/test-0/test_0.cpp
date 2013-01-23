/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file test_0.cpp
 * @author richard <richard@taomee.com>
 * @date 2011-07-20
 */

#include <stdio.h>

#include "i_net_io_server.h"

class net_io_notifier : public i_net_io_notifier
{
public:
    virtual int init()
    {
        fprintf(stdout, "DEBUG: net_io_notifier: init");
        return 0;
    }

    virtual int uninit()
    {
        fprintf(stdout, "DEBUG: net_io_notifier: uninit");
        return 0;
    }

    virtual int popup()
    {
        fprintf(stdout, "DEBUG: net_io_notifier: popup");
        return 0;
    }

    virtual int got()
    {
        fprintf(stdout, "DEBUG: net_io_notifier: got");
        return 0;
    }

    virtual int release()
    {
        fprintf(stdout, "DEBUG: net_io_notifier: release");
        return 0;
    }

    virtual int get_eventfd() const 
    {
        fprintf(stdout, "DEBUG: net_io_notifier: get_eventfd");
        return 0;
    }
};

class net_io_event_handler : public i_net_io_event_handler
{
public:
    virtual int on_new_connection(void *p_net_io_server, int id, int fd,
                                  const char *ip, int port, union net_io_storage *p_storage)
    {
        fprintf(stdout, "DEBUG: net_io_event_handler: on_new_connection: %s:%d\n", ip, port);
        return 0;
    }

    virtual int on_recv_data(void *p_net_io_server, int id, int fd,
                             const char *ip, int port, char *p_data, int data_len,
                             union net_io_storage *p_storage)
    {
        fprintf(stdout, "DEBUG: net_io_event_handler: on_recv_data: %s:%d data_len: %d\n", ip, port, data_len);
        int i = 0;
        for (i = 0; i != data_len; ++i) {
            fprintf(stdout, "0x%02X ", (uint8_t)p_data[i]);
            if (((i + 1) % 8) == 0) {
                fprintf(stdout, "\n");
            }
            fflush(stdout);
        }
        if (((i + 1) % 8) != 0) {
            fprintf(stdout, "\n");
        }
        
        if (((i_net_io_server *)p_net_io_server)->send_data(id, p_data, data_len) == -1) {
            ((i_net_io_server *)p_net_io_server)->close_connection(id, true);
        }

        return 0;
    }

    virtual int on_connection_closed(void *p_net_io_server,
                                     int id, int fd,
                                     const char *ip, int port, union net_io_storage *p_storage)
    {
        fprintf(stdout, "DEBUG: net_io_event_handler: on_connection_closed: %s:%d\n", ip, port);
        return 0;
    }

    virtual int on_wakeup(void *p_net_io_server)
    {
//        fprintf(stdout, "DEBUG: net_io_event_handler: on_wakeup\n");
        return 0;
    }
};

int main()
{
    i_net_io_event_handler *p_net_io_event_handler = new net_io_event_handler();

    i_net_io_server *p_net_io_server = NULL;
    if (net_io_server_create(&p_net_io_server) != 0) {
        fprintf(stderr, "ERROR: net_io_server_create\n");
        return -1;
    }

    if (p_net_io_server->init("127.0.0.1", 4444, p_net_io_event_handler, NULL, 1, 1) != 0) {
        fprintf(stderr, "ERROR: p_net_io_server->init\n");
        return -1;
    }

    for (;;) {
        if (p_net_io_server->do_io(1, NET_IO_SERVER_CMD_ACCEPT | 
                                      NET_IO_SERVER_CMD_READ   | 
                                      NET_IO_SERVER_CMD_WRITE) != 0) {
            fprintf(stderr, "ERROR: p_net_io_server->do_io\n");
            break;
        }
    }

    if (p_net_io_server->uninit() != 0) {
        fprintf(stderr, "ERROR: p_net_io_server->uninit\n");
        return -1;
    }

    if (p_net_io_server->release() != 0) {
        fprintf(stderr, "ERROR: p_net_io_server->release\n");
        return -1;
    }

    return 0;
}
