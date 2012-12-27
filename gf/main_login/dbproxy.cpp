#include "dbproxy.h"
#include "proto.h"
#include "init.h"
#include "net.h"
#include  <libtaomee/project/stat_agent/msglog.h>
extern "C"{
//#include  <statistic_agent/msglog.h>
#include  <libtaomee/timer.h>
}

void handle_db_return(svr_proto_t* dbpkg, uint32_t pkglen)
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
#ifndef TW_VER
        send_warning(p, 1, dbpkg->cmd);
#endif
    }

    DEBUG_LOG("DB RETURN\t[uid=%u %u fd=%d cmd=0x%X ret=%u]",
                p->uid, dbpkg->id, connfd, dbpkg->cmd, dbpkg->ret);
    REMOVE_TIMERS(&p->tmr);
    int err = -1;
    uint32_t bodylen = dbpkg->len - sizeof(svr_proto_t); 
    switch(dbpkg->cmd) {
        case DB_GET_UID_BY_EMAIL:
            err = get_uid_by_mail_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case DB_CHECK_UID_PWD:
        case DB_CHECK_UID_PWD_EX:
            err = check_pwd_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        default:
            ERROR_LOG("err db cmd [%d %d]", dbpkg->cmd, dbpkg->id);
    }
    if(err) {
        clear_usr_info(p);
        close_client_conn(connfd);
    }
}

void statistic_msglog(uint32_t offset, void *data, size_t len)
{
    time_t now;
    time(&now);
    char* file = g_ds_ini.statistic_file;
    int msgret = msglog(file, offset, now, data, len);
    if (msgret != 0) {
        ERROR_LOG( "statistic log error\t[message type=%x return=%d]", offset, msgret);
    }
    return;
}

void clear_usr_info(usr_info_t* p)
{
    p->uid = 0;
    p->waitcmd = 0;
    if(p->tmr.timer_list.next && p->tmr.timer_list.prev) {
        REMOVE_TIMERS(&p->tmr);
    }
}
