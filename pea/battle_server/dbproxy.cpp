extern "C" 
{
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
}

#include "dbproxy.hpp"
#include "player.hpp"
#include "cli_proto.hpp"

using namespace taomee;


int proxysvr_fd = -1;
char * proxy_name = config_get_strval("dbproxy_name");

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

/*
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
*/


void init_connect_to_dbproxy()
{
    connect_to_dbproxy();
    /*
	if(proxysvr_fd == -1) 
    {
        connect_to_dbproxy_timely(0, 0);
    }
	*/
}

bool is_dbproxy_fd(int fd)
{
    return (proxysvr_fd == fd);
}

void close_dbproxy_fd()
{
    proxysvr_fd = -1;
}



int  send_request_to_db(player_t* p, uint32_t id, uint32_t role_tm, uint16_t cmd, const void* body_buf, uint32_t body_len)
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
            //return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
        }
        return -1;
    }

    db_proto_t* pkg = reinterpret_cast<db_proto_t*>(dbbuf);
    pkg->len = sizeof(db_proto_t) + body_len;
    pkg->seq = (p ? p->btl->get_battle_id() : 0);
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = id;
    pkg->role_tm  = role_tm;
    memcpy(pkg->body, body_buf, body_len);
    return net_send_ex(proxysvr_fd, dbbuf, pkg->len);
}

int send_request_to_db(const Player * p, uint32_t id, uint32_t role_tm, uint16_t cmd, Cmessage * p_out)
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
    head.seq = (p ? p->btl->get_battle_id() : 0);
    head.cmd = cmd;
    head.ret = 0;
    head.id  = id;
    head.role_tm  = role_tm;
    return net_send_msg(proxysvr_fd, (char *)&head, p_out);
}

void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen)
{
	if(!dbpkg->seq){
		return;	
	}
	

    Player* p = get_player(dbpkg->id);

    if( p == NULL)
    {
        ERROR_TLOG("can not find player id = %u", dbpkg->id);
        return;
    }
	else if( p->btl == NULL)
	{
		ERROR_TLOG("player btl is null  uid = %u, dbcmd = %u", p->id, dbpkg->cmd);
		return;
	}
	else if( p->btl->get_battle_id() != dbpkg->seq)
	{
		ERROR_TLOG("player btl id = %u, pkg seq =%u, dbcmd = %u", p->btl->get_battle_id(), dbpkg->seq, dbpkg->cmd);
		return;
	}


    uint16_t cmd = dbpkg->cmd;
    uint32_t ret = dbpkg->ret;
    uint32_t body_len = dbpkg->len - sizeof(db_proto_t);

	if(ret)
	{
		ERROR_TLOG("db return error: ret = %u cmd = %u", ret, cmd);
		send_header_to_player(p, p->waitcmd, ret, 1);
		return;
	}

    const bind_proto_cmd_t * p_cmd = find_db_cmd_bind(cmd);
    if (NULL == p_cmd)
    {
        WARN_TLOG("db cmdid not existed: %u", cmd);
        return;
    }

    bool unpack_ret = p_cmd->p_out->read_from_buf_ex((char *)dbpkg + sizeof(db_proto_t), body_len);
    if (!unpack_ret)
    {
        ERROR_TLOG("pkglen error cmd_id = %u read_len = %u", cmd, dbpkg->len);
        return;
    }

    int err_code = p_cmd->func(p, p_cmd->p_out, NULL, NULL);

    if(err_code) 
    {
        close_client_conn(p->fd);
    }
}

