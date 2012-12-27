/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年05月18日 16时13分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#include "../../common/constant.h"
#include "../../common/data_structure.h"
#include "../../common/message.h"

int main(int argc, char **argv)
{

    if(argc != 4)
    {
        printf("usage:./send_reload_conf online_ip online_port conf_type(0, 1, 2, ...., 16)\n");
        return -1;
    }

    char online_ip[16] = {0};
    strncpy(online_ip, argv[1], 15);
    uint16_t online_port = atoi(argv[2]);
    uint32_t conf_type = atoi(argv[3]);
    if(!(conf_type >= 0 && conf_type < MAX_CONF_FILE_NUM))
    {
        printf("conf_type must between 0 and %d\n", MAX_CONF_FILE_NUM);
        return -1;
    }

    int net_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(net_fd < 0)
    {
        printf("socket failed.\n");
        return -1;
    }

    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(online_ip);
    serv_addr.sin_port = htons(online_port);

    int result =  connect(net_fd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    if(result < 0)
    {
        printf("connect to server %s:%u failed(%s).\n", online_ip, online_port, strerror(errno));
        close(net_fd);
        return -1;
    }


    char send_buf[1024] = {0};
    as_msg_header_t *p_send_msg = (as_msg_header_t*)send_buf;
    p_send_msg->len = htonl(sizeof(as_msg_header_t) + sizeof(uint32_t));
    p_send_msg->msg_type = htons((uint16_t)msg_reload_conf_file);
    p_send_msg->user_id = htonl((uint32_t)47159775);
    p_send_msg->result = htonl(0);
    p_send_msg->err_no = htonl(0);
    *(uint32_t*)(send_buf + sizeof(as_msg_header_t)) = htonl(conf_type);
    int bytes_sent = send(net_fd, p_send_msg, sizeof(as_msg_header_t) + sizeof(uint32_t), 0);
    if(bytes_sent > 0)
    {
        printf("send %d bytes to server\n", bytes_sent);
    }
    else
    {
        printf("send to server failed(bytes_sent:%d errno:%s).\n", bytes_sent, strerror(errno));
    }

     sleep(1);
     close(net_fd);

    return 0;

}
