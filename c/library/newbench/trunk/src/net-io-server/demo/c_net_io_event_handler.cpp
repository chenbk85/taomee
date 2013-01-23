#include "c_net_io_event_handler.h"

#include <stdio.h>
#include <string.h>
#include <string>
#include <map>

using namespace std;

static map<int, string> msg_mgr_;

int c_net_io_event_handler::on_new_connection(void *p_net_io_server,
                                              int id,
                                              int fd,
                                              const char *ip,
                                              int port,
                                              union net_io_storage *p_storage)
{
    printf("\n[connection build]id: %d, fd: %d, ip: %s, port: %d\n",
           id, fd, ip, port);
    fflush(stdout);
    return 0;
}

int c_net_io_event_handler::on_recv_data(void *p_net_io_server,
                                         int id,
                                         int fd,
                                         const char *ip,
                                         int port,
                                         char *p_data,
                                         int data_len,
                                         union net_io_storage *p_storage)
{
    string buffer = msg_mgr_[id];
    buffer.append(p_data, data_len);
    size_t end = buffer.find("\r\n");
    int status = 0;

    if (end != string::npos)
    {
        if (end <= 3)
        {
            printf("%s", p_data);
            status = -1;
            goto ret;
        }

        // complete command
        string tmp = buffer.substr(0, end - 1);
        size_t start = tmp.find_first_not_of("\t ");


        if ((start == string::npos)
            || ("get " != buffer.substr(0, 4)))
        {
            status = -1;
            goto ret;
        }

        string command = buffer.substr(start, end - start);
        string parameter = command.substr(4);
        parameter.insert(0, "./res/");

        FILE *p_file = fopen(parameter.c_str(), "r");

        printf("\nrequest file: %s\n", parameter.c_str());
        fflush(stdout);

        if (NULL == p_file)
        {
            printf("\nno such file: %s\n", parameter.c_str());
            fflush(stdout);
            status = -2;
            goto ret;
        }

        static char buffer_send[8 * 1024];

        while (fgets(buffer_send, sizeof(buffer_send), p_file))
        {
            int ret = reinterpret_cast<i_net_io_server *>(
                        p_net_io_server)->send_data_atomic(
                            id,
                            buffer_send,
                            strlen(buffer_send));
        }

        int ret = reinterpret_cast<i_net_io_server *>(
                    p_net_io_server)->send_data_atomic(
                        id, "0*********", 10);

        buffer.clear();
        fclose(p_file);
    }

ret:
    if (-1 == status)
    {
        // invalid command
        buffer.clear();
        reinterpret_cast<i_net_io_server *>(
            p_net_io_server)->send_data_atomic(
                id, "1*********", 10);
    }
    else if (-2 == status)
    {
        buffer.clear();
        reinterpret_cast<i_net_io_server *>(
            p_net_io_server)->send_data_atomic(
                id, "2*********", 10);
    }

    return 0;
}

int c_net_io_event_handler::on_connection_closed(
        void *p_net_io_server,
        int id,
        int fd,
        const char *ip,
        int port,
        union net_io_storage *p_storage)
{
    printf("\n[connection closed]id: %d, fd: %d, ip: %s, port: %d\n",
           id, fd, ip, port);
    fflush(stdout);
    return 0;
}

int c_net_io_event_handler::on_wakeup(void *p_net_io_server)
{
    return 0;
}
