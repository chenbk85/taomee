extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <libtaomee/timer.h>
}
#include "net.h"
#include "dbproxy.h"
#include "asyc_main_login_type.h"
#include "init.h"
uint8_t cli_buf[BUFFER_SIZE];
struct sockaddr_in m_sockaddr;

int mainlogin_dbproxysvr_fd = -1;
int warning_fd = -1;

int dbproxy_timeout(void* owner, void* data)
{
    usr_info_t* p = reinterpret_cast<usr_info_t*>(data);
    ERROR_LOG("DB TIMEOUT [%d %d]", p->uid, p->waitcmd);
    if(p->waitcmd) {
#ifndef TW_VER
        send_warning(p, 1, p->waitcmd);
#endif
        send_to_self_error(p, SYSTEM_ERR);
    }
    return 0;
}

int send_request_to_mainlogin_db(usr_info_t* p, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
    static uint8_t dbbuf[1024];

    if (mainlogin_dbproxysvr_fd == -1) {
        mainlogin_dbproxysvr_fd = connect_to_svr(g_ds_ini.mainlogin_dbproxy_ip,
                                        g_ds_ini.mainlogin_dbproxy_port, 65535, 1);
    }

    if ((mainlogin_dbproxysvr_fd == -1) ||  (body_len > (sizeof(dbbuf) - sizeof(svr_proto_t)))) {
        ERROR_LOG("send to dbproxy failed: fd=%d len=%d", mainlogin_dbproxysvr_fd, body_len);
        send_to_self_error(p, SYSTEM_ERR);
#ifndef TW_VER
        send_warning(p, 5, inet_addr(g_ds_ini.mainlogin_dbproxy_ip));
#endif
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
        ADD_TIMER_EVENT(&p->tmr, dbproxy_timeout, p, get_now_tv()->tv_sec + g_ds_ini.svr_timeout); 
    //    DEBUG_LOG("SEND REQ TO DB\t[uid=%u %u cmd=%u 0x%X]", p->uid, p->uid, p->waitcmd, cmd);
    }
    return net_send(mainlogin_dbproxysvr_fd, dbbuf, pkg->len);
}

void init_proto_head(void* buf, uint32_t len, uint32_t cmd, userid_t uid, uint32_t result)
{
    cli_login_pk_header_t* p = reinterpret_cast<cli_login_pk_header_t*>(buf);

    p->length     = htonl(len);
    //p->version    = 0x01;
    p->commandid  = htons(cmd);
    //keep client fd
    p->userid     = htonl(uid);
    p->result     = htonl(result);
    p->seqnum     = htonl(0);
}

void init_warning_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd)
{
    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(buf);

    pkg->len = len;
    pkg->seq = 0;
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = p?p->uid:0;
}

int send_warning(usr_info_t* p, int type, int cmdid)
{
    static uint8_t warning_buff[BUFFER_SIZE];
    int len = sizeof(svr_proto_t);
    warning_pkg_t* warning = reinterpret_cast<warning_pkg_t*>(warning_buff + len);
    warning->notify_category = g_ds_ini.business_type;
    warning->notify_id = type;
    warning->mimi_account = p?p->uid:0;
    warning->common_id = cmdid;
    len += sizeof(warning_pkg_t);
    init_warning_head(p, warning_buff, len, 0XF000);
    if (warning_fd == -1) {
        warning_fd = create_udp_socket(&m_sockaddr, g_ds_ini.warning_ip,
                                        g_ds_ini.warning_port);

    }
    sendto(warning_fd , warning_buff, len, 0, (struct  sockaddr*)&m_sockaddr, sizeof(m_sockaddr));
    ERROR_LOG("WARNING [%d %d %d %d]", p?p->uid:0, warning_fd, cmdid, type);
    return 0;
}

int send_to_self_error(usr_info_t* p, uint32_t error)
{
    static uint8_t err_buf[BUFFER_SIZE];
    init_proto_head(err_buf, sizeof(cli_login_pk_header_t), p->waitcmd, p->uid, error);
    DEBUG_LOG("PROC RESULT\t[uid=%d %d %d]", p->uid, p->waitcmd, error);
    clear_usr_info(p);
    return send_pkg_to_client(p->session, err_buf, sizeof(cli_login_pk_header_t));
}


int send_to_self(usr_info_t* p, const uint8_t *buf, uint32_t length)
{
    DEBUG_LOG("SEND TO SELF\t[%d %d %d]", p->uid, p->waitcmd, length);
    clear_usr_info(p);
    return send_pkg_to_client(p->session, buf, length);
}

