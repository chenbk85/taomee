#include "i_net_io_server.h"

#include <stdio.h>
#include <unistd.h>

#include "c_net_io_notifier.h"

class c_net_io_event_handler: public i_net_io_event_handler
{
public:
    int on_new_connection(void * /* p_net_io_server */,
                          const int id,
                          const int fd,
                          const char *ip,
                          const int port,
                          union net_io_storage *p_storage)
    {
        printf("\n[new connection]id:%d,fd:%d\n,ip:%s,port:%d",
                id, fd, ip, port);
        fflush(stdout);
        p_storage->u64 = fd;
        return 0;
    }

    int on_recv_data(void *p_net_io_server,
                     const int id,
                     const int /* fd */,
                     const char * /* ip */,
                     const int /* port */,
                     char *p_data,
                     int data_len,
                     union net_io_storage *p_storage)
    {
        int id2 = 0, port2 = 0, fd2 = -1, time2 = -1;
        char ip2[16] = {'\0'};
        reinterpret_cast<i_net_io_server *>(
            p_net_io_server)->get_connection_info(
                id, ip2, 16, &port2, &time2, &port2, NULL);
        printf("\n[new connection]id:%d,fd:%d\n,ip:%s,port:%d",
            id2, fd2, ip2, port2);
        int ids[16];
        int len;
        reinterpret_cast<i_net_io_server *>(
            p_net_io_server)->enum_connections(ids, 16, &len);
        printf("\nids:");
        for (int i = 0; i < len; ++ i)
        {
            printf("%d,", ids[i]);
        }
        printf("\n");

        printf("\nrecv from id(%d):%s\n", id, p_data);
        fflush(stdout);
        reinterpret_cast<i_net_io_server *>(p_net_io_server)->send_data(id, p_data, data_len);
        return 0;
    }

    int on_connection_closed(void * /*p_net_io_server */,
                             const int id,
                             const int fd,
                             const char * /* ip */,
                             const int /* port */,
                             union net_io_storage *p_storage)
    {
        printf("\n[connection closed]id:%d,fd:%d,storage:%llu\n",
               id, fd, p_storage->u64);
        fflush(stdout);
        return 0;
    }

    int on_wakeup(void *p_net_io_server)
    {
        return 0;
    }
};

int main()
{
    i_net_io_server *p_net_io_server;
    int ret = net_io_server_create(&p_net_io_server);

    if (ret)
    {
        printf("\nfail to init net_io_server ret=%d\n", ret);
        return -1;
    }

    i_net_io_notifier *p_net_io_notifier;
    ret = net_io_notifier_create(&p_net_io_notifier);

    if (ret)
    {
        printf("\nfail to init net_io_notifier ret=%d\n", ret);
        return -1;
    }

    p_net_io_notifier->init();
    i_net_io_event_handler *p_net_io_event_handler = new c_net_io_event_handler();
    ret = p_net_io_server->init("10.1.14.41", 3333, p_net_io_event_handler, p_net_io_notifier, 1, 1);
    //ret = p_nh->init("10.1.14.41", 3333, [>p_neh<] NULL, p_nn, 1, 1);
    if (ret < 0)
    {
        printf("\n%s\n", p_net_io_server->get_last_errstr());
        fflush(stdout);
        return -1;
    }

    if (fork())
    {
        while (1)
        {
            p_net_io_server->do_io(-1,
                                   NET_IO_SERVER_CMD_ACCEPT
                                   | NET_IO_SERVER_CMD_READ
                                   | NET_IO_SERVER_CMD_WRITE);
            if (p_net_io_server->get_last_errno())
            {
                printf("\nlast error:%s\n", p_net_io_server->get_last_errstr());
                fflush(stdout);
            }
            printf("\nwake am i!\n");
            fflush(stdout);
        }
    }
    else
    {
        //for (int i = 0; i < 2; ++ i)
        //{
            //sleep(5);
            //printf("\ni do wake!\n");
            //fflush(stdout);
            ////p_net_io_notifier->popup();
        //}
    }

    return 0;
}
