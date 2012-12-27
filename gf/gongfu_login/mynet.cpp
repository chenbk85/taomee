extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <libtaomee/timer.h>
}
#include <errno.h>
#include "mynet.h"
#include "myinit.h"
#include "mysvrlist.h"
#include "myproto.h"
#include "libtype.h"

int gf_proxysvr_fd = -1;
int gf_deluser_fd = -1;
int timestamp_fd = -1;
int switch_fd = -1;
static int multicast_fd = -1;
static struct sockaddr_in multicast_addr;

int send_request_to_deluser(usr_info_t* p, uint16_t cmd, const void* pkgbuf, uint32_t body_len)
{
    static uint8_t buf[1024];

    if (gf_deluser_fd == -1) {
        gf_deluser_fd = connect_to_svr(my_opt.gf_deluser_ip,
                                        my_opt.gf_deluser_port, 65535, 1);
    }

    if ((gf_deluser_fd == -1) ||  (body_len > (sizeof(buf) - sizeof(svr_proto_t)))) {
        ERROR_LOG("send to deluser failed: fd=%d len=%d", gf_deluser_fd, body_len);
        send_warning(p, 5, inet_addr(my_opt.gf_deluser_ip));
        return -1;
    }

    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(buf);
    pkg->len = sizeof(svr_proto_t) + body_len;
    p->counter = (p->counter + 1)%SVR_USR_COUNTER;
    pkg->seq = (p->session->fd<< 16) | p->counter;
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = p->uid;
    //*(reinterpret_cast<uint32_t*>(pkg->body)) = 0;//role_tm
    memcpy((pkg->body), pkgbuf, body_len);

    DEBUG_LOG("DELUSER SEND\t[uid=%u fd=%d cmd=0x%X]",p->uid,p->session->fd,cmd);
    if (p) {
        if(p->tmr.timer_list.next && p->tmr.timer_list.prev) {
            REMOVE_TIMERS(&p->tmr);
        }
        INIT_LIST_HEAD(&p->tmr.timer_list);
        ADD_TIMER_EVENT(&p->tmr, mydeluser_timeout, p, get_now_tv()->tv_sec + 10); 
    }
    return net_send(gf_deluser_fd, buf, pkg->len);
}

int send_request_to_db(usr_info_t* p, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
    static uint8_t dbbuf[1024];

    if (gf_proxysvr_fd == -1) {
        gf_proxysvr_fd = connect_to_svr(my_opt.gf_dbproxy_ip,
                                        my_opt.gf_dbproxy_port, 65535, 1);
    }

    if ((gf_proxysvr_fd == -1) ||  (body_len > (sizeof(dbbuf) - sizeof(svr_proto_t)))) {
        ERROR_LOG("send to dbproxy failed: fd=%d len=%d", gf_proxysvr_fd, body_len);
        send_warning(p, 5, inet_addr(my_opt.gf_dbproxy_ip));
        return -1;
    }

    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(dbbuf);
    pkg->len = sizeof(svr_proto_t) + body_len;
    p->counter = (p->counter + 1)%SVR_USR_COUNTER;
    pkg->seq = (p->session->fd<< 16) | p->counter;
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = p->uid;
    //*(reinterpret_cast<uint32_t*>(pkg->body)) = 0;//role_tm
    memcpy((pkg->body), dbpkgbuf, body_len);

    if (p) {
        if(p->tmr.timer_list.next && p->tmr.timer_list.prev) {
            REMOVE_TIMERS(&p->tmr);
        }
        INIT_LIST_HEAD(&p->tmr.timer_list);
        ADD_TIMER_EVENT(&p->tmr, mydbproxy_timeout, p, get_now_tv()->tv_sec + 10); 
    }
    return net_send(gf_proxysvr_fd, dbbuf, pkg->len);
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
    if (inet_pton(AF_INET, my_opt.multicast_interface, &ipaddr)!=1)
       ERROR_LOG("inet_pton<=0 [%s]",my_opt.multicast_interface );

    if (setsockopt(multicast_fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
        ERROR_LOG("MCAST SET OPTION ERROR:%d,%m",errno );
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

void m_process_db_return(svr_proto_t* dbpkg, uint32_t pkglen)
{
    if (!dbpkg->seq) {
        return;
    }
    uint32_t counter = dbpkg->seq & 0xFFFF;
    uint32_t connfd  = dbpkg->seq >> 16;
    usr_info_t* p = get_usr_by_fd(connfd);
    if(!p->waitcmd || (p->uid && p->uid != dbpkg->id)) {
        ERROR_LOG("connection closed: fd=%u",connfd);
        return;
    }
    if(counter != p->counter) {
        ERROR_LOG("counter error [%d %d %d %d]", p->uid, p->waitcmd, counter, p->counter);
        return;
    }
    if(dbpkg->ret == 1001 || dbpkg->ret == 1002 || dbpkg->ret == 1003) {
        send_warning(p, 1, dbpkg->cmd);
    }

    DEBUG_LOG("DB RETURN\t[uid=%u %u fd=%d cmd=0x%X ret=%u]",
                p->uid, dbpkg->id, connfd, dbpkg->cmd, dbpkg->ret);
    REMOVE_TIMERS(&p->tmr);
    int err = -1;
    uint32_t bodylen = dbpkg->len - sizeof(svr_proto_t);
    switch(dbpkg->cmd) {
        case DB_LOGIN_GF:
            err = login_gf_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case DB_GET_ROLE_LIST:
            err = get_role_list_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case DB_GF_REGISTER:
            err = gf_register_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case DB_ADD_GAME_FLAG:
            err = add_game_flag_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        default:
            ERROR_LOG("err db cmd [%d %d]", dbpkg->cmd, dbpkg->id);
    }
    if(err) {
        clear_usr_info(p);
        close_client_conn(connfd);
    }
}

void handle_deluser_return(svr_proto_t* pkg, uint32_t pkglen)
{
    if (!pkg->seq) {
        ERROR_LOG("deluser return error");
        return;
    }
    uint32_t counter = pkg->seq & 0xFFFF;
    uint32_t connfd  = pkg->seq >> 16;
    usr_info_t* p = get_usr_by_fd(connfd);
    if(!p->waitcmd || (p->uid && p->uid != pkg->id)) {
        ERROR_LOG("connection closed: waitcmd=%u fd=%u uid=%u pkg->id=%u",
				p->waitcmd, connfd, p->uid, pkg->id);
        return;
    }
    if(counter != p->counter) {
        ERROR_LOG("counter error [%d %d %d %d]", p->uid, p->waitcmd, counter, p->counter);
        return;
    }

    DEBUG_LOG("DELUSER RETURN\t[uid=%u %u fd=%d cmd=0x%X ret=%u]",
                p->uid, pkg->id, connfd, pkg->cmd, pkg->ret);
    REMOVE_TIMERS(&p->tmr);
    int err = -1;
    uint32_t bodylen = pkg->len - sizeof(svr_proto_t);
    switch(pkg->cmd) {
        case DELUSER_DELETE_ROLE:
            err = delete_role_callback(p, pkg->body, bodylen, pkg->ret);
            break;
        default:
            ERROR_LOG("err deluser cmd [%d %d]", pkg->cmd, pkg->id);
    }
    if(err) {
        clear_usr_info(p);
        close_client_conn(connfd);
    }
}

void handle_timestamp_return(login_timestamp_pk_header_t* dbpkg, uint32_t pkglen)
{
    if (!dbpkg->seq) {
        ERROR_LOG("timestamp return error");
        return;
    }

    uint32_t counter = dbpkg->seq & 0xFFFF;
    uint32_t connfd  = dbpkg->seq >> 16;
    usr_info_t* p = get_usr_by_fd(connfd);
    if(!p->waitcmd) {
        ERROR_LOG("connection closed: fd=%u",connfd);
        return;
    }
    if(counter != p->counter) {
        ERROR_LOG("counter error [%d %d %d %d]", p->uid, p->waitcmd, counter, p->counter);
        return;
    }

    uint32_t cmd = ntohl(dbpkg->commandid); 
    //DEBUG_LOG("TMSTM RETURN\t[uid=%u cmd=%u fd=%u ret=%u]",
                //p->uid, cmd, connfd, dbpkg->result);

    REMOVE_TIMERS(&p->tmr);
    int err = -1;
    uint32_t bodylen = dbpkg->length - sizeof(login_timestamp_pk_header_t); 
    switch(cmd) {
        case TIMESTAMP_GET_FRIENDTIME:
            err = get_friend_time_callback(p, dbpkg->body, bodylen, dbpkg->result);
            break;
        default:
            ERROR_LOG("err db cmd [%d %d]", cmd, connfd);
    }
    if(err) {
        clear_usr_info(p);
        close_client_conn(connfd);
    }
}

void handle_switch_return(svr_proto_t* dbpkg, uint32_t pkglen)
{
    if (!dbpkg->seq) {
        ERROR_LOG("Switch return error");
        return;
    }

    uint32_t counter = dbpkg->seq & 0xFFFF;
    int connfd  = dbpkg->seq >> 16;
    uint32_t bodylen = dbpkg->len - sizeof(svr_proto_t); 

    if(connfd == 0xFFFF) {
        update_backup_svrlist_callback(dbpkg->body, bodylen, dbpkg->ret);
        return;
    }

    usr_info_t* p = get_usr_by_fd(connfd);
    if(!p->waitcmd || p->uid != dbpkg->id) {
        ERROR_LOG("connection closed: fd=%u uid=%u %u",connfd, p->uid, dbpkg->id);
        return;
    }
    if(counter != p->counter) {
        ERROR_LOG("counter error [%d %d %d %d]", p->uid, p->waitcmd, counter, p->counter);
        return;
    }

    DEBUG_LOG("SWITCH RETURN\t[uid=%u %u fd=%d cmd=%u ret=%u len=%u]",
                p->uid, dbpkg->id, connfd, dbpkg->cmd, dbpkg->ret, dbpkg->len);
    int err = -1;
    REMOVE_TIMERS(&p->tmr);
    switch(dbpkg->cmd) {
        case SWITCH_GET_RECOMMEND_SVR_LIST:
            err = get_recommeded_svr_list_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case SWITCH_GET_RANGED_SVR_LIST:
            err = get_ranged_svr_list_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        default:
            ERROR_LOG("err db cmd [%d %d]", dbpkg->cmd, dbpkg->id);
    }
    if(err) {
        clear_usr_info(p);
        close_client_conn(connfd);
    }
}
