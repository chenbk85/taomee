#include <libtaomee++/inet/byteswap.hpp>

using namespace taomee;

extern "C" 
{
#include <assert.h>
#include <glib.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/utilities.h>
#include <async_serv/net_if.h>
}

#include "fwd_decl.hpp"
#include "cli_proto.hpp"
#include "battle_impl.hpp"
#include "battle_manager.hpp"
/*
typedef   int(*P_DEALFUN_T)( DEFAULT_ARG );
//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_cli_cmd_map;
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
		{cmdid, new (c_in), md5_tag, bind_bitmap, proto_name},

Ccmd< P_DEALFUN_T> g_cli_cmd_list[]={
	#include "pea_battle_bind_online_req.h"
};
*/




typedef boost::intrusive::member_hook<Player, ObjectHook, &Player::awaiting_hook> AwaitingOption;

typedef boost::intrusive::list<Player, AwaitingOption, boost::intrusive::constant_time_size<false> > AwaitingPlayerList;

/*
typedef int (*cli_proto_hdlr_t)(Player* p, uint8_t* body, uint32_t bodylen);

struct cli_proto_handle_t 
{
	cli_proto_handle_t(cli_proto_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
	{   
		hdlr = h;  
		len = l;  
		cmp_method = cmp; 
	}   
	cli_proto_hdlr_t    hdlr;
	uint32_t            len;
	uint8_t             cmp_method;
};


#define SET_CLI_HANDLE(op_, len_, cmp_) \
	do { \
		if (cli_handles[btl_ ## op_].hdlr != 0) { \
			ERROR_TLOG("duplicate cmd=%u name=%s", btl_ ## op_, #op_); \
			return false; \
		} \
		cli_handles[btl_ ## op_] = cli_proto_handle_t(op_ ## _cmd, len_, cmp_); \
	} while (0)
*/


uint8_t pkgbuf[1 << 21];


static AwaitingPlayerList awaiting_playerlist;

bool init_cli_proto_handles()
{
	return true;
}

bool final_cli_proto_handles()
{
	return true;	
}

int dispatch(void* data, fdsession_t* fdsess, bool cache_cmd_flag)
{
	btl_proto_t* pkg = reinterpret_cast<btl_proto_t*>(data);
	Player* p = get_player(pkg->id);
	if(p == NULL)
	{
		if(is_alloc_player_cmd(pkg->cmd))
		{
			p = add_player(pkg->id, fdsess);	
		}
		else
		{
			ERROR_TLOG("Player not found! BattleServer might have been restarted!");
			send_header_to_online(pkg->id, fdsess, pkg->cmd, pkg->seq, BTL_ERR_PLAYER_NOT_FOUND);
			return 0;
		}
	}
	else
	{
		if( pkg->cmd == online_proto_create_btl_cmd)
		{
			if(p->btl != NULL)
			{
				Battle* btl = p->btl;
				p->btl->del_player(p->id);
				if( btl->get_cur_players_count() == 0)
				{
					battle_mgr::get_instance()->del_battle(btl->get_battle_id());
					battle_factory::get_instance()->destroy_battle(btl);
				}
			}
			else
			{
				::del_player(p);
			}
			p = add_player(pkg->id, fdsess);
		}	
	}

	if( cache_cmd_flag && p->waitcmd != 0)
	{
		if (g_queue_get_length(p->pkg_queue) < 50)
		{
			cache_a_pkg(p, pkg, pkg->len);
			return 0;
		}
		else
		{
			WARN_TLOG("wait for cmd=%u id=%u newcmd=%u", p->waitcmd, p->id, pkg->cmd);
			return send_header_to_player(p, p->waitcmd, cli_err_system_busy, 0);
		}
	}

	uint32_t bodylen = pkg->len - sizeof(btl_proto_t);
	/*
	if(((cli_handles[pkg->cmd].cmp_method == cmp_must_eq) && (bodylen != cli_handles[pkg->cmd].len))
		||  ((cli_handles[pkg->cmd].cmp_method == cmp_must_ge) && (bodylen < cli_handles[pkg->cmd].len)))
	{
		ERROR_TLOG("invalid package cmd=%u len=%u %u cmpmethod=%d",
			pkg->cmd, bodylen, cli_handles[pkg->cmd].len, cli_handles[pkg->cmd].cmp_method);
		return -1;	
	}
	p->waitcmd = pkg->cmd;
	return cli_handles[pkg->cmd].hdlr(p, pkg->body, bodylen);*/
	//Ccmd< P_DEALFUN_T> * p_cmd_item =g_cli_cmd_map.getitem(pkg->cmd );
	const bind_proto_cmd_t * p_cmd_item = find_online_cmd_bind(pkg->cmd);
	
	if (p_cmd_item ) 
	{
		bool unpack_ret  = p_cmd_item->p_in->read_from_buf_ex((char*)pkg+sizeof(btl_proto_t), bodylen);
		if(!unpack_ret){
			ERROR_TLOG("pkglen error cmd_id = %u", pkg->cmd);	
			return -1;
		}
		p->waitcmd = pkg->cmd;
		return p_cmd_item->func(p, p_cmd_item->p_in,  NULL, NULL);
	}
	else
	{
		ERROR_TLOG("cmd = %u id not find", pkg->cmd);
		return -1;
	}
}

bool is_alloc_player_cmd(uint32_t cmd)
{
	return ( (cmd >= online_proto_create_btl_cmd) && (cmd <= online_proto_syn_player_info_cmd));
}


int send_header_to_online(userid_t uid, fdsession_t* fdsess, uint32_t cmd, uint32_t seqno, uint32_t err)
{
	btl_proto_t pkg;
	
	init_btl_proto_head_full(&pkg, cmd, sizeof(pkg), err);
	
	pkg.id  = uid;
	pkg.seq = seqno;
	if (send_pkg_to_client(fdsess, &pkg, sizeof(pkg)) == -1) 
	{
		ERROR_TLOG("failed to send pkg to client: uid=%u cmd=%u", uid, cmd);
		return -1;
	}
	return 0;
}

int send_to_player(Player* p, void* pkgbuf, uint32_t len, int completed)
{
	assert(len <= btl_proto_max_len);

	btl_proto_t* proto = reinterpret_cast<btl_proto_t*>(pkgbuf);
	proto->id = p->id;
	proto->seq = p->btl ? p->btl->get_battle_id() : 0;
	if (send_pkg_to_client(p->fdsess, pkgbuf, len) == -1) 
	{
		ERROR_TLOG("failed to send pkg to client: uid=%u cmd=%u", p->id, proto->cmd);	
		return -1;
	}
	if(completed)
	{
		p->waitcmd = 0;	
	}
	return 0;
}

int send_header_to_player(Player* p, uint32_t cmd, uint32_t err, int completed)
{
	btl_proto_t pkg;

	init_btl_proto_head_full(&pkg, cmd, sizeof(pkg), err);
	return send_to_player(p, &pkg, sizeof(pkg), completed);
}

void init_btl_proto_head_full(void* header, uint32_t cmd, uint32_t len, uint32_t ret)
{
	btl_proto_t* p = reinterpret_cast<btl_proto_t*>(header);
	
	p->len = len;
	p->cmd = cmd;
	p->ret = ret;
}

void init_btl_proto_head(void* header, uint32_t cmd, uint32_t len)
{
	init_btl_proto_head_full(header, cmd, len, 0);	
}

void init_cli_proto_head_full(void* header, userid_t id, uint16_t cmd, uint32_t ret, uint32_t len)
{
	cli_proto_t* p = reinterpret_cast<cli_proto_t*>(header);

	p->id  = taomee::bswap(id);
	p->len = taomee::bswap(len);
	p->cmd = taomee::bswap(cmd);
	p->ret = taomee::bswap(ret);
}

void init_cli_proto_head(void* header, uint16_t cmd, uint32_t len)
{
	init_cli_proto_head_full(header, 0, cmd, 0, len);
}


void cache_a_pkg(Player* p, const void* buf, uint32_t buflen)
{
	uint16_t len = sizeof(cached_pkg_t) + buflen;
	cached_pkg_t* cachebuf = reinterpret_cast<cached_pkg_t*>(g_slice_alloc(len));
	cachebuf->len = len;
	memcpy(cachebuf->pkg, buf, buflen);
	if(!p->awaiting_hook.is_linked())
	{
		awaiting_playerlist.push_back(*p);	
	}

	g_queue_push_tail(p->pkg_queue, cachebuf);
}

void proc_cached_pkgs()
{
	AwaitingPlayerList::iterator nx = awaiting_playerlist.begin();	
	for (AwaitingPlayerList::iterator it = nx; it != awaiting_playerlist.end(); it = nx)
	{
		++nx;
		while (it->waitcmd == 0)
		{
			cached_pkg_t* pkg = reinterpret_cast<cached_pkg_t*>(g_queue_pop_head(it->pkg_queue));		
			
			uint32_t uid = it->id;
			int err = dispatch(pkg->pkg, it->fdsess, false);
			g_slice_free1(pkg->len, pkg);
			
			Player* p = get_player(uid);
			if(!p)
			{
				WARN_TLOG("player delete already!\t[uid=%u]",uid);
				break;
			}
			if(!err)
			{
				if(g_queue_is_empty(it->pkg_queue))
				{
					awaiting_playerlist.erase(it);
					break;
				}
			}
			else
			{
				close_client_conn(it->fd);
				break;
			}
		}
		
		
	}
}
