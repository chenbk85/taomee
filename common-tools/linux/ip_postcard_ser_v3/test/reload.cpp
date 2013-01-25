#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>

struct send_buf_t
{
    uint32_t    len;
    uint32_t    seq;
    uint16_t    cmd;
    uint32_t    ret;
    uint32_t    id;
}__attribute__((packed));

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: send to proxy xxx.xxx.xxx.xxx prot \n");
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_aton(argv[1], &(server_addr.sin_addr));
    server_addr.sin_port = htons(atoi(argv[2]));

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd)
    {
        printf("error:socket\n");
        return -1;
    }

    if (-1 == connect(fd, (sockaddr *)&server_addr, sizeof(sockaddr_in)))
    {
        printf("error:connect\n");
        return -1;
    }

    send_buf_t sb;
    sb.len = sizeof(sb);
    sb.seq = 0;
    sb.cmd = 0xC081;
    sb.ret = 0;

    int send_len = send(fd, &sb, sb.len, 0);
    if (send_len != sb.len)
    {
        printf("error:net send\n");
        return -1;
    }

    return 0;
}

