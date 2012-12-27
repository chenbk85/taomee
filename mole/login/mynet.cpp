extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <libtaomee/timer.h>
}
#include "mynet.h"
#include "mydbproxy.h"
#include "myinit.h"
#include "mysvrlist.h"
#include "libtype.h"
int mole_proxysvr_fd = -1;
int timestamp_fd = -1;
int switch_fd = -1;
static int multicast_fd = -1;
static struct sockaddr_in multicast_addr;

int send_request_to_db(usr_info_t* p, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
    static uint8_t dbbuf[1024];

    if (mole_proxysvr_fd == -1) {
        //mole_proxysvr_fd = connect_to_svr(my_opt.mole_dbproxy_ip, my_opt.mole_dbproxy_port, 65535, 1);
		 mole_proxysvr_fd = connect_to_service(my_opt.mole_dbproxy_name, 0, 65535, 1);

    }

    if ((mole_proxysvr_fd == -1) ||  (body_len > (sizeof(dbbuf) - sizeof(svr_proto_t)))) {
        ERROR_LOG("send to dbproxy failed: fd=%d len=%d", mole_proxysvr_fd, body_len);
		const ip_port_t *ptr_ip_port = get_last_connecting_service();
        //send_warning(p, 5, inet_addr(my_opt.mole_dbproxy_ip));
        send_warning(p, 5, inet_addr(ptr_ip_port->ip));
        return -1;
    }

    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(dbbuf);
    pkg->len = sizeof(svr_proto_t) + body_len;
    p->counter = (p->counter + 1)%SVR_USR_COUNTER;
    pkg->seq = (p->session->fd<< 16) | p->counter;
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = p->uid;
    memcpy(pkg->body, dbpkgbuf, body_len);

    if (p) {
        if(p->tmr.timer_list.next && p->tmr.timer_list.prev) {
            REMOVE_TIMERS(&p->tmr);
        }
        INIT_LIST_HEAD(&p->tmr.timer_list);
        ADD_TIMER_EVENT(&p->tmr, mydbproxy_timeout, p, get_now_tv()->tv_sec + 10); 
    }
    return net_send(mole_proxysvr_fd, dbbuf, pkg->len);
}

void init_timestamp_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd)
{
    login_timestamp_pk_header_t* pkg = reinterpret_cast<login_timestamp_pk_header_t*>(buf);

    pkg->length     = htonl(len);
    pkg->version    = 0x01;
    pkg->commandid  = htonl(cmd);
    //keep client fd
    p->counter = (p->counter + 1)%SVR_USR_COUNTER;
    pkg->seq        = (p->session->fd<< 16) | p->counter;
    pkg->result     = 0;
}

void init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd)
{
    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(buf);

    pkg->len = len;
    p->counter = (p->counter + 1)%SVR_USR_COUNTER;
    pkg->seq = (p->session->fd<< 16) | p->counter;
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = p->uid;
}

int send_to_timestamp(usr_info_t* p, const uint8_t *buf, uint32_t length)
{
    if (timestamp_fd== -1) {
        timestamp_fd= connect_to_svr(my_opt.time_ip,
                my_opt.time_port, 65535, 1);
    }
    if(p) {
        if(p->tmr.timer_list.next && p->tmr.timer_list.prev) {
            REMOVE_TIMERS(&p->tmr);
        }
        INIT_LIST_HEAD(&p->tmr.timer_list);
        ADD_TIMER_EVENT(&p->tmr, timestamp_timeout, p, get_now_tv()->tv_sec + 10); 
    }
    return net_send(timestamp_fd, buf, length);
}

int send_to_switch(usr_info_t* p, const uint8_t *buf, uint32_t length)
{
    if (switch_fd == -1) {
        //switch_fd = connect_to_svr(my_opt.master_ip, my_opt.master_port, 65535, 1);
        switch_fd = connect_to_service(my_opt.master_ser, 0, 65535, 1);
    }
    if (switch_fd == -1 && p) {
        return get_backup_svr_list(p);
    }
    if(p) {
        if(p->tmr.timer_list.next && p->tmr.timer_list.prev) {
            REMOVE_TIMERS(&p->tmr);
        }
        INIT_LIST_HEAD(&p->tmr.timer_list);
        ADD_TIMER_EVENT(&p->tmr, switch_timeout, p, get_now_tv()->tv_sec + 10); 
    }
    return net_send(switch_fd , buf, length);
}


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
    inet_pton(AF_INET, my_opt.multicast_ip, &(multicast_addr.sin_addr));
    multicast_addr.sin_port = htons(my_opt.multicast_port);

    // Set Default Interface For Outgoing Multicasts
    in_addr_t ipaddr;
    inet_pton(AF_INET, my_opt.multicast_interface, &ipaddr);
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
