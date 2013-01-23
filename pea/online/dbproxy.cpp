extern "C" 
{
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
}

#include "dbproxy.hpp"
#include "player.hpp"
#include "db_player.hpp"
#include "cli_proto.hpp"
#include "session.hpp"

using namespace taomee;

// typedef int (*dbproto_hdlr_t)(player_t* p, uint32_t id, void* body, uint32_t bodylen, uint32_t ret);

// struct dbproto_handle_t 
// {
    // dbproto_handle_t(dbproto_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
    // { hdlr = h; len = l; cmp_method = cmp; }
    // dbproto_hdlr_t      hdlr;
    // uint32_t            len;
    // uint8_t             cmp_method;
// };

// static dbproto_handle_t db_handles[65536];

int proxysvr_fd = -1;
char * proxy_name = config_get_strval("dbproxy_name");

// uint8_t dbpkgbuf[dbproto_max_len] = {0};

/*
#define SET_DB_HANDLE(op_, len_, cmp_) \
    do { \
        if (db_handles[db_proto_ ## op_ ## _cmd].hdlr != 0) { \
            ERROR_TLOG("duplicate cmd=%u name=%s", db_proto_ ## op_ ## _cmd, #op_); \
            return false; \
        } \
        db_handles[db_proto_ ## op_ ## _cmd] = dbproto_handle_t(db_ ## op_ ## _callback, len_, cmp_); \
    } while (0)
*/

// ///////////////////////////////////////////////////////////////////////////////////////////////////////

// bool init_db_proto_handles()
// {
    // memset(db_handles, 0, sizeof(db_handles));
    // SET_DB_HANDLE(get_player,  0,  cmp_must_ge);
    // return true;	
// }

// bool final_db_proto_handles()
// {
    // memset(db_handles, 0, sizeof(db_handles));
    // return true;
// }

void connect_to_dbproxy()
{
    if (NULL != resolve_service_name(proxy_name, 0))
    {
        proxysvr_fd = connect_to_service(proxy_name, 0, 65535, 1);
        if (-1 != proxysvr_fd)
        {
            DEBUG_TLOG("DBPROXY CONNECTED");
        }
    }
}


int connect_to_dbproxy_timely(void* owner, void* data)
{
    if (!data)
    {
        ADD_TIMER_EVENT(&g_events, connect_to_dbproxy_timely, reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);	
    }
    else if (proxysvr_fd == -1)
    {
        connect_to_dbproxy();
        if (proxysvr_fd == -1)
        {
            ADD_TIMER_EVENT(&g_events, connect_to_dbproxy_timely, reinterpret_cast<void*>(1), get_now_tv()->tv_sec + 10);	
        }
    }
    return 0;
}



void init_connect_to_dbproxy()
{
    connect_to_dbproxy();
    if(proxysvr_fd == -1) 
    {
        connect_to_dbproxy_timely(0, 0);
    }
}

bool is_dbproxy_fd(int fd)
{
    return (proxysvr_fd == fd);
}

void close_dbproxy_fd()
{
    proxysvr_fd = -1;
}



int  send_to_db(player_t* p, uint32_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, uint32_t body_len)
{
    static uint8_t dbbuf[dbproto_max_len];

    if (proxysvr_fd == -1) 
    {
        proxysvr_fd = connect_to_service(config_get_strval("dbproxy_name"), 0, 65535, 1);
    }

    if ((proxysvr_fd == -1) )
    {
        ERROR_TLOG("connect to db proxy fail fd = %d", proxysvr_fd);
        return -1;
    }

    if( body_len > (sizeof(dbbuf) - sizeof(db_proto_t))) 
    {
        ERROR_TLOG("send pkglen is too long: fd=%d len=%d", proxysvr_fd, body_len);
        //if(p) 
        {
            //if (p->waitcmd == cli_proto_login){
            //return -1;
            //}
            //return send_error_to_player(p, cli_err_system_error);
        }
        return -1;
    }

    db_proto_t* pkg = reinterpret_cast<db_proto_t*>(dbbuf);
    pkg->len = sizeof(db_proto_t) + body_len;
    pkg->seq = (p ? ((p->fd << 16) | p->waitcmd) : 0);
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = id;
    pkg->role_tm  = role_tm;
    memcpy(pkg->body, body_buf, body_len);
    return net_send_ex(proxysvr_fd, dbbuf, pkg->len);
}

int send_to_db(const player_t * p, uint32_t id, uint32_t role_tm, uint16_t cmd, Cmessage * p_out)
{
    if (proxysvr_fd == -1)
    {
        connect_to_dbproxy();
        
    }

    if (proxysvr_fd == -1)
    {
        ERROR_TLOG("connect to db proxy fail fd = %d", proxysvr_fd);
        return -1;

    }

    db_proto_t head;
    head.len = sizeof(db_proto_t);
    head.seq = (p ? ((p->fd << 16) | p->waitcmd) : 0);
    head.cmd = cmd;
    head.ret = 0;
    head.id  = id;
    head.role_tm  = role_tm;
    return net_send_msg(proxysvr_fd, (char *)&head, p_out);
}


int send_to_db(const player_t * p, uint16_t cmd, Cmessage * p_out)
{
    return send_to_db(p, p->id, p->role_tm, cmd, p_out);
}

void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen)
{
	if (!dbpkg->seq) {
		TRACE_TLOG("no_wait_db_return, cmd=%u, u=%u", dbpkg->cmd, dbpkg->id);
		return ;
	}

    int connfd  = dbpkg->seq >> 16;
    uint32_t waitcmd = dbpkg->seq & 0xFFFF;

    player_t *p = get_player_by_fd(connfd);
    if(!p || p->id != dbpkg->id) {
        ERROR_TLOG("nofound player, fd=%d, dbpkg->id=%u, p->id=%u",
				connfd, dbpkg->id, p ? p->id : 0);
        return;
    }

	/*QA(singku) 这个地方可能导致BUG 先注释掉 
    DB 不会自动发包要求广播, 因此可以检查 waitcmd */
    if(p->waitcmd != waitcmd) {
        ERROR_TLOG("db_wcmd=(%u) != p->waitcmd(%u), dbcmd=0x%X", 
                waitcmd, p->waitcmd, dbpkg->cmd);	
        //return ;
    }

    uint16_t cmd = dbpkg->cmd;
    uint32_t ret = dbpkg->ret;
    uint32_t body_len = dbpkg->len - sizeof(db_proto_t);

    if (ret) {
        ERROR_TLOG("db return error: ret = %u cmd = 0x%X", ret, cmd);
        send_error_to_player(p, ret);
        return;
    }

    if (db_check_session_cmd == dbpkg->cmd) {
        check_session_callback(p, dbpkg->body, body_len, ret);
        return;
    }

    const bind_proto_cmd_t * p_cmd = find_db_cmd_bind(cmd);
    if (!p_cmd) {
        WARN_TLOG("db cmdid not existed: %u", cmd);
        return;
    }

    bool unpack_ret = p_cmd->p_out->read_from_buf_ex((char *)dbpkg + sizeof(db_proto_t), body_len);
    if (!unpack_ret)
    {
        ERROR_TLOG("pkglen error cmd_id = 0x%X read_len = %u", cmd, dbpkg->len);
        return;
    }

    int err_code = p_cmd->func(p, p_cmd->p_out, NULL, NULL);

    // if(
            // ((db_handles[dbpkg->cmd].cmp_method == cmp_must_eq) && (bodylen != db_handles[dbpkg->cmd].len)) ||
            // ((db_handles[dbpkg->cmd].cmp_method == cmp_must_ge) && (bodylen < db_handles[dbpkg->cmd].len)) 
      // )
    // {
        // ERROR_TLOG("invalid package len=%u needlen=%u cmd=0x%X cmpmethod=%d ret=%u",
                // bodylen, db_handles[dbpkg->cmd].len, dbpkg->cmd,
                // db_handles[dbpkg->cmd].cmp_method, dbpkg->ret);	
        // goto ERROR_LOGIC;
    // }
    // err_code = db_handles[dbpkg->cmd].hdlr(p, dbpkg->id, dbpkg->body, bodylen, dbpkg->ret);


// ERROR_LOGIC:
    if(err_code) 
    {
        close_client_conn(p->fd);
    }
}

