#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "i_net_io_server.h"
#include "c_net_io_notifier.h"
#include "c_net_io_event_handler.h"

int main(int argc, char **argv)
{
    i_net_io_server *p_server;
    int ret = net_io_server_create(&p_server);

    if (ret < 0)
    {
        printf("\nfail to create instance of net_io_server!\n");
        return -1;
    }

    if (argc < 3)
    {
        printf("\nplease implement it with parameters ip, port\nfor example: ./simple_file_server 127.0.0.1 3333\n");
        return -1;
    }

    i_net_io_notifier *p_notifier;
    ret = net_io_notifier_create(&p_notifier);
    p_notifier->init();

    c_net_io_event_handler event_handler;

    ret = p_server->init(argv[1],
                         atoi(argv[2]),
                         &event_handler,
                         p_notifier,
                         1,
                         1);

    if (ret < 0)
    {
        printf("\nfail to init server\n");
        return -1;
    }

    while (1)
    {
        p_server->do_io(-1,
                        NET_IO_SERVER_CMD_ACCEPT
                        | NET_IO_SERVER_CMD_READ
                        | NET_IO_SERVER_CMD_WRITE);
    }

    printf("\nthe end\n");
    return 0;
}
