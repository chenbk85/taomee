#include <libtaomee++/inet/byteswap.hpp>

using namespace taomee;

extern "C" {
#include <assert.h>
#include <glib.h>

#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
#include <libtaomee/utils.h>

#include <async_serv/net_if.h>
}

#include "fwd_decl.hpp"
#include "cli_proto.hpp"
#include "player.hpp"
#include "pea_common.hpp"
#include "battle_switch.hpp"
#include "proto.hpp"
#include "dbproxy.hpp"


uint8_t pkgbuf[1 << 21];

list_head_t awaiting_playerlist;


int dispatch(void* data, fdsession_t* fdsess, bool first_tm)
{	
	cli_proto_t* pkg = reinterpret_cast<cli_proto_t*>(data);

	
    int fd = fdsess->fd;
    uint32_t len = bswap(pkg->len);
    uint16_t cmd = bswap(pkg->cmd);
    uint32_t id = bswap(pkg->id);
    uint32_t seq = bswap(pkg->seqno);
    uint32_t ret = bswap(pkg->ret);
	// pkg_head.len   = bswap(pkg->len);
	// pkg_head.cmd   = bswap(pkg->cmd);
	// pkg_head.id    = bswap(pkg->id);
	// pkg_head.seqno = bswap(pkg->seqno);
	// pkg_head.ret   = bswap(pkg->ret);
	
	/*
	pkg_head.len   = (pkg->len);
	pkg_head.cmd   = (pkg->cmd);
	pkg_head.id    = (pkg->id);
	pkg_head.seqno = (pkg->seqno);
	pkg_head.ret   = (pkg->ret);
	*/
	/*
	// protocol for testing if this server is OK
	if (pkg->cmd == cli_proto_probe) {
		return send_pkg_to_client(fdsess, pkg, pkg->len);
	}
	*/

	player_t* p = get_player_by_fd(fd);
	
	if ( (cmd != cli_proto_login_cmd && !p) 
        || (cmd == cli_proto_login_cmd && p)
        || (p && p->id != id) ) 
	{
		ERROR_TLOG("pkg error: fd=%d uid=%u %u cmd=%u p=%p",
					fd, (p ? p->id : 0), id, cmd, p);
		if(p)
		{
        	send_header_to_player(p, cmd, ONLINE_ERR_SYSTEM_FAULT, 1);
		}
		return -1;
	}
	
	
	if (first_tm && p) 
	{
        if (cmd != cli_proto_login_cmd) 
		{
			/*
			if (is_invaild_seqno(p, pkg_head.len, pkg_head.seqno, pkg_head.cmd, pkg->body, pkg_head.len - sizeof(cli_proto_t))) 			{
				ERROR_TLOG("wrong seqno: uid=%u seq=%u %u ret=%u cmd=%u",
						p->id, p->real_seqno, pkg_head.seqno, pkg_head.ret, pkg_head.cmd);
				return -1;
			}
			*/
        }
		//p->seqno = pkg_head.seqno;

		if (p->waitcmd != 0) 
		{
			if (g_queue_get_length(p->pkg_queue) < 100) {
				ERROR_TLOG("p = %u cache pkg = %u waitcmd = %u", p->id, cmd, p->waitcmd);
				cache_a_pkg(p, (char*)pkg, len);
				return 0;
			} 
			else 
			{
				WARN_TLOG("wait for cmd=%u id=%u newcmd=%u", p->waitcmd, p->id, cmd);
				//return send_header_to_player(p, p->waitcmd, cli_err_system_busy, 0);
				return 0;
			}
		}
	}

	/*
	if ( p && (p->cur_map == 0)	&& (pkg_head.cmd != cli_proto_enter_map) ) {
		assert(!"impossible error! player not in map");
		return 0;
	}
	*/

	// login protocol
	player_t player;	
	if (cmd == cli_proto_login_cmd) 
	{
		player_t* old = get_player(id);
		if (old) 
		{
			TRACE_TLOG("MULTIPLE LOGIN TO SAME ONLINE, UID=%u", old->id);
            send_header_to_player(old, cli_proto_login_cmd, ONLINE_ERR_MULTI_LOGIN, 0);
            int old_fd = old->fd;
            del_player(old);
			close_client_conn(old_fd);
		}
		p          = &player;
		p->id      = id;
		p->fdsess  = fdsess;
        p->clear_waitcmd();
	}

    INFO_TLOG("[%u] sender: %u, fd: %u, seq: %u, len: %u", cmd, id, fd, seq, len);

    const bind_proto_cmd_t * p_cmd = find_cli_cmd_bind(cmd);
    if (NULL == p_cmd)
    {
        ERROR_TLOG("invalid cmd p = %u, cmd = %u", p->id, cmd);
		send_header_to_player(p, cmd, ONLINE_ERR_CMD_ID, 1);
        return -1;
    }


    bool unpack_ret = read_from_buf_n(
            p_cmd->p_in, ((char *)data) + sizeof(cli_proto_t), len - sizeof(cli_proto_t));
    if(!unpack_ret) {
        ERROR_TLOG("dispatch_pkglen_error(psys): cmd=%u, pkglen=%u", cmd, len);
        send_header_to_player(p, cmd, ONLINE_ERR_MSG_LEN, 1);
        return -1;
    }
    // 防止上一次协议的数据残留
    p_cmd->p_out->init();
    p->waitcmd = cmd;
    p->bind_cmd = p_cmd;
    p->seqno = seq;
    p->ret = ret;
	TRACE_LOG("%u<<<<<---------- RECV CLIENT PKG\t[cmd %u]", p->id, cmd);
    p_cmd->func(p, p_cmd->p_in, p_cmd->p_out, NULL);
    return 0;
	
	/*
	uint32_t bodylen = pkg_head.len - sizeof(cli_proto_t);
	if ( ((cli_handles[pkg_head.cmd].cmp_method == cmp_must_eq) && (bodylen != cli_handles[pkg_head.cmd].len))
			|| ((cli_handles[pkg_head.cmd].cmp_method == cmp_must_ge) && (bodylen < cli_handles[pkg_head.cmd].len)) ) {
		ERROR_TLOG("invalid package cmd=%u len=%u %u cmpmethod=%d",
					pkg_head.cmd, bodylen, cli_handles[pkg_head.cmd].len, cli_handles[pkg_head.cmd].cmp_method);
		return -1;
	}
	p->waitcmd = pkg_head.cmd;
	return cli_handles[pkg_head.cmd].hdlr(p, pkg->body, bodylen);
	*/
}


void proc_cached_pkgs()
{

    list_head_t* cur;
    list_head_t* next;
    list_for_each_safe (cur, next, &awaiting_playerlist) 
    {
        player_t* p = list_entry(cur, player_t, wait_cmd_player_hook);
        while (p->waitcmd == 0) 
        {
            cached_pkg_t* pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(p->pkg_queue));
            int err = dispatch(pkg->pkg, p->fdsess, false);
            g_slice_free1(pkg->len, pkg);
            if (!err) 
            {
                if (g_queue_is_empty(p->pkg_queue)) 
                {
                    list_del_init(&(p->wait_cmd_player_hook));
                    break;
                }
            } 
            else 
            {
                close_client_conn(p->fd);
                break;
            }
        }
    }	
}

void cache_a_pkg(player_t *p, char* buf, uint32_t buflen)
{
	uint32_t len = sizeof(cached_pkg_t) + buflen;
	cached_pkg_t* cachebuf = reinterpret_cast<cached_pkg_t*>(g_slice_alloc(len));
	cachebuf->len = len;
	memcpy(cachebuf->pkg, buf, buflen);
	g_queue_push_tail(p->pkg_queue, cachebuf);
	if (list_empty(&(p->wait_cmd_player_hook)))
    {
		list_add_tail(&(p->wait_cmd_player_hook), &awaiting_playerlist);
	}	
}

int send_to_player(player_t* p, char* pkg, uint32_t len, int completed)
{
	cli_proto_t* proto = reinterpret_cast<cli_proto_t*>(pkg);

	KDEBUG_LOG(p->id, "---------->>>>>SEND 2 CLIENT\t[cmd %u %u]", bswap(proto->cmd), bswap(proto->ret));
	if(send_pkg_to_client_ex(p->fdsess, pkg, len) == -1)
    {
		ERROR_TLOG("failed to send pkg to client: uid=%u cmd=%u", p->id, bswap(proto->cmd));
		return -1;
	}

	if(completed)
    {
        p->clear_waitcmd();
	}
	return 0;
}


int send_to_player(player_t * p, Cmessage * p_out, uint16_t cmd,  uint8_t completed)
{
    cli_proto_t pkg_head;
    init_cli_proto_head_full(&pkg_head, p->id, p->seqno, cmd, sizeof(cli_proto_t), p->ret);

	KDEBUG_LOG(p->id, "---------->>>>>SEND 2 CLIENT\t[cmd %u %u]", bswap(cmd), bswap(p->ret));
    if (-1 == send_msg_to_client_bigendian(p->fdsess, (char *)&pkg_head, p_out))
    {
        ERROR_TLOG("[%u] failed to send pkg to player: %u, fd: %d, seq: %u", p->waitcmd, p->id, p->fdsess->fd, p->seqno);
		return -1;
	}

    if (completed)
    {
        p->clear_waitcmd();
    }

	return 0;
}


int send_header_to_player(player_t* p, uint16_t cmd, uint32_t err, int completed)
{
	cli_proto_t pkg;

	init_cli_proto_head_full(&pkg, p->id, p->seqno, cmd, sizeof(pkg), err);
	return send_to_player(p, (char*)&pkg, sizeof(pkg), completed);
}

int send_error_to_player(player_t * p, uint32_t err)
{
    return send_header_to_player(p, p->waitcmd, err, 1);
}

void init_cli_proto_head_full(void* header, userid_t uid, uint32_t seqno, uint16_t cmd, uint32_t len, uint32_t ret)
{
	cli_proto_t* proto = reinterpret_cast<cli_proto_t*>(header);
	
	proto->id  = taomee::bswap(uid);
	proto->len = taomee::bswap(len);
	proto->cmd = taomee::bswap(cmd);
	proto->ret = taomee::bswap(ret);
	proto->seqno = taomee::bswap(seqno);
	// proto->id = uid;
	// proto->len = len;
	// proto->cmd = cmd;
	// proto->ret = ret;
	// proto->seqno = seqno;
}

void init_cli_proto_head(void* header, const player_t* p, uint16_t cmd, uint32_t len)
{
	if(p) 
	{
		init_cli_proto_head_full(header, p->id, p->seqno, cmd, len, 0);
	} 
	else 
	{
		init_cli_proto_head_full(header, 0, 0, cmd, len, 0);
	}	
}

