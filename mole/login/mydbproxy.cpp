#include "mydbproxy.h"
#include "mysvrlist.h"
#include "myproto.h"
#include "myinit.h"
#include "mynet.h"
extern "C"{
//#include  <statistic_agent/msglog.h>
#include  <libtaomee/timer.h>
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
        case DB_LOGIN_MOLE:
            err = login_mole_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case DB_MOLE_REGISTER:
            err = mole_register_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case DB_ADD_GAME_FLAG:
            err = add_mole_flag_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        default:
            ERROR_LOG("err db cmd [%d %d]", dbpkg->cmd, dbpkg->id);
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

    DEBUG_LOG("SWITCH RETURN\t[uid=%u %u fd=%d cmd=%u ret=%u]",
                p->uid, dbpkg->id, connfd, dbpkg->cmd, dbpkg->ret);
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
