/*
 * =====================================================================================
 *
 *       Filename:  muclast.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/12/2010 04:50:11 PM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), jim@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include <sys/types.h>
#include <sys/socket.h>
#include "mulcast.hpp"

int multicast_fd = -1;
static struct sockaddr_in multicast_addr;


/**
  * @brierf initialize the multicast fd.
  * @param  void.
  * @return     int. 0 on success. -1 on database error.
  */
int multicast_init()
{
    //DEBUG_LOG("enter multicast_init");

    multicast_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (multicast_fd == -1) {
        ERROR_LOG("MCAST FD INIT ERROR");
        return -1;
    }

    multicast_addr.sin_family = AF_INET;
    //inet_pton(AF_INET, my_opt.multicast_ip, &(multicast_addr.sin_addr));
    //multicast_addr.sin_port = htons(my_opt.multicast_port);

    // Set Default Interface For Outgoing Multicasts
    in_addr_t ipaddr;
    //inet_pton(AF_INET, my_opt.multicast_interface, &ipaddr);
    if (setsockopt(multicast_fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
        ERROR_LOG("MCAST SET OPTION ERROR");
        return -1;
    }
    return 0;
}
/**
  * @brierf handle the multicast message to multicast group members.
  * @param  body_buf. Package body buffer.
  * @param    body_len. Body buffer's len.
  * @return     int. 0 on success. -1 on database error.
  */
int  multicast_player_login(uint8_t* body_buf, int body_len)
{
    TRACE_LOG("enter multicast_player_login");

    static char multicast_buf[4096];

    int len = sizeof(login_online_pk_header_t) + body_len;

    if ( len > 4096 ) {
        ERROR_LOG("multicast send len is too big");
        return -1;
    }

    // make the package header
    login_online_pk_header_t* pHeader =(login_online_pk_header_t*)multicast_buf;
    pHeader->len        = len;
    pHeader->online_id  = 0;
    pHeader->cmd        = MULTICAST_CMD;
    pHeader->ret        = 0;
    pHeader->id         = 0;
    pHeader->mapid      = 0;
    pHeader->opid       = 1100; //online cmd, notify friend login
    memcpy(multicast_buf + sizeof(login_online_pk_header_t), body_buf, body_len);

    sendto(multicast_fd, multicast_buf, len, 0,
            reinterpret_cast<sockaddr*>(&multicast_addr), sizeof(multicast_addr));

    return 0;
}

