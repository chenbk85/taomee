#include "dll_interface.h"
extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/conf_parser/config.h>
#include <async_serv/net_if.h>
#include <arpa/inet.h>
}
#include "memory.h"
#include <libtaomee++/inet/byteswap.hpp>
#include "extend_proto.h"
#include "global_data.h"
#include "clientsmgr.h"
#include "client.h"

bool global_data_Initialize()
{
	if (0 == g_pClientsMgr )
	{
		g_pClientsMgr = new ClientsMgr;
	}
	return true;
};
int cliprotoheadsize = 0;
int netbytesorder = 1;
int init_service(int isparent)
{
	DEBUG_LOG("INIT...");
	if (!isparent) {
		global_data_Initialize();
	}
		config_init("./cliproto.conf");
		cliprotoheadsize = config_get_intval("cliprotoheadsize", 0);
		netbytesorder = config_get_intval("netbytesorder", 1);
		if (cliprotoheadsize == 0)
		{
			ERROR_LOG("cliproto.conf err");
			return -1;
		}
		DEBUG_LOG("cliprotoheadsize=%d",cliprotoheadsize);
		DEBUG_LOG("netbytesorder=%d",netbytesorder);
	//}

	return 0;
}

int fini_service(int isparent)
{
	DEBUG_LOG("FINI...");
	if (!isparent) {
		//fini_players();
		if (g_pClientsMgr)
		{
			delete ((ClientsMgr*)g_pClientsMgr);
		}
	}

	return 0;
}

void proc_events()
{
}

enum {
	max_incoming_pkg_len	= 64 * 1024,
	cli_proto_max_len = 32 * 1024
};

int get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
	const char request[]  = "<policy-file-request/>";
	const char response[] = "<?xml version=\"1.0\"?>"
								"<!DOCTYPE cross-domain-policy>"
								"<cross-domain-policy>"
								"<allow-access-from domain=\"*\" to-ports=\"*\" />"
								"</cross-domain-policy>";

	if (avail_len < 4) {
		return 0;
	}

	int len = -1;
	if (isparent) {
		// the client requests for a socket policy file
		if ((avail_len == sizeof(request)) && !memcmp(avail_data, request, sizeof(request))) {
			net_send(fd, response, sizeof(response));

			TRACE_LOG("Policy Req [%s] Received, Rsp [%s] Sent", request, response);
			return 0;
		}

		const cli_proto_t* pkg = reinterpret_cast<const cli_proto_t*>(avail_data);

		if (netbytesorder)
		{
			len = taomee::bswap(pkg->len);
		}
		else
		{
			len = pkg->len;
		}
		
		if ((len > cli_proto_max_len) || (len < cliprotoheadsize)) {
			KERROR_LOG(0, "[p] invalid len=%d from fd=%d", len, fd);
			return -1;
		}
	} else {
		const cli_proto_t* pkg = reinterpret_cast<const cli_proto_t*>(avail_data);

		if (netbytesorder)
		{
			len = taomee::bswap(pkg->len);
		}
		else
		{
			len = pkg->len;
		}
		if ((len > max_incoming_pkg_len) || (len < cliprotoheadsize)) {
					KERROR_LOG(0, "[c] invalid len=%d from fd=%d", len, fd);
					return -1;
		}
	}

	return len;
}

static void SendAllBufPkg(Client* pClient)
{
	list<BufPkg>::iterator itr = pClient->m_listBufPkgs.begin();
	while ( itr != pClient->m_listBufPkgs.end())
	{
		BufPkg buf = (*itr);
		if (net_send(pClient->m_serverfd, buf.pBuf, buf.len) < 0)
		{
			ERROR_LOG("net_send err fd=%d", pClient->m_serverfd);
			break;
		}
		//cli_proto_t* pkg = reinterpret_cast<cli_proto_t*>(buf.pBuf);
		//cli_proto_t  pkg_head;
		//pkg_head.len = taomee::bswap(pkg->len);
		//pkg_head.ver = taomee::bswap(pkg->ver);
		//pkg_head.cmd = taomee::bswap(pkg->cmd);
		//pkg_head.id  = taomee::bswap(pkg->id);
		//pkg_head.ret = taomee::bswap(pkg->ret);
		//KERROR_LOG(pkg_head.id, "SendAllBufPkg fd=%d cmd=%u,pkg.len=%u, len=%d",pClient->m_serverfd, pkg_head.cmd,pkg_head.len,buf.len);
		delete [] buf.pBuf;
		pClient->m_listBufPkgs.erase(itr++);
	}
}
struct ExtArg
{
	uint32_t	serialNum;
	int			clientFd;
	char sip[INET_ADDRSTRLEN];
	uint32_t	ip;
	uint16_t port;
};

static void conn_callback(int fd, void *arg)
{
	ExtArg* pArg = (ExtArg*)arg;
	int clientfd = pArg->clientFd;
	uint32_t serial = pArg->serialNum;
	
	
	Client* pClient = pClientsMgr->FindClientByFd(clientfd);
	if (pClient)
	{
		if (fd < 0)
		{	
			KERROR_LOG(pClient->m_userid, "conn_callback err ip[%s][0x%x] port[%d] fd = [%d] clientfd[%d] serverfd[%d]",pArg->sip,pArg->ip,pArg->port,fd, pClient->m_clientfd, pClient->m_serverfd);
			close_client_conn(pClient->m_clientfd);
			delete pArg;
			return;
		}
		if(pClient->m_serialNum == serial)
		{
			pClient->m_serverfd = fd;
			pClientsMgr->m_mapServerFdClients.insert(make_pair(fd, pClient));
			SendAllBufPkg(pClient);
		}
		else
		{
			DEBUG_LOG("last client closed,clientfd[%d] reused, so close serverfd[%d] new uid=[%u]", pClient->m_clientfd, fd,pClient->m_userid);
			close_svr(fd);
		}
	}
	else
	{
		DEBUG_LOG("client leaved, close_svr fd=%d", fd);
		if (fd > 0)
		{
			close_svr(fd);
		}
	}
	delete pArg;
}

int proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	if (!pClientsMgr)
	{
		ERROR_LOG("pClientsMgr = NULL");
		return -1;
	}
	uint8_t* pMsgBody = NULL;
	cli_proto_t  pkg_head;
	if (cliprotoheadsize == 18)
	{
		new_cli_proto_t* pkg = reinterpret_cast<new_cli_proto_t*>(data);
		if (netbytesorder)
		{
			pkg_head.len = taomee::bswap(pkg->len);
			pkg_head.cmd = taomee::bswap(pkg->cmd);
			pkg_head.id  = taomee::bswap(pkg->id);
		}
		else
		{
			pkg_head.len = pkg->len;
			pkg_head.cmd = pkg->cmd;
			pkg_head.id  = pkg->id;
		}
		
		pMsgBody = pkg->body;
	}
	else
	{
		cli_proto_t* pkg = reinterpret_cast<cli_proto_t*>(data);
		if (netbytesorder)
		{
			pkg_head.len = taomee::bswap(pkg->len);
			pkg_head.cmd = taomee::bswap(pkg->cmd);
			pkg_head.id  = taomee::bswap(pkg->id);
		}
		else
		{
			pkg_head.len = pkg->len;
			pkg_head.cmd = pkg->cmd;
			pkg_head.id  = pkg->id;
		}
		pMsgBody = pkg->body;
	}
	
	
	TRACE_LOG("+++dispatch pkg len=%d from cmd=%d, uid=%u", pkg_head.len, pkg_head.cmd, pkg_head.id);
	if( pkg_head.id==0)
	{
		KERROR_LOG(pkg_head.id, "pkg_head.id[%u]", pkg_head.id);
		return -1;
	}
	
	Client* pClient = pClientsMgr->FindClient(pkg_head.id);
	if (pkg_head.cmd == cli_proto_extend_port)
	{
		const int mustLen = sizeof(online_addr_t) + cliprotoheadsize;
		if (pkg_head.len!=(uint32_t)len||len != mustLen)
		{
			KERROR_LOG(pkg_head.id, "cmd 999 but len err,pkg_head.len=%u, len=%d mustLen=%d", pkg_head.len, len,mustLen);
			return -1;
		}
		if (pClient)
		{
			DEBUG_LOG( "Reconnected!uid=%u oldfd=%d newfd=%d",pkg_head.id, pClient->m_clientfd, fdsess->fd);
			if (pClient->m_clientfd != fdsess->fd)
			{
				close_client_conn(pClient->m_clientfd);
			}
			else
			{
				return 0;
			}
		}
	
		const online_addr_t* pOnline_Addr = (const online_addr_t*)pMsgBody;

		uint16_t port = ntohs(pOnline_Addr->port);
		char sip[INET_ADDRSTRLEN] = {0};
		inet_ntop(AF_INET, (const void *)&(pOnline_Addr->ip), (char *)sip, INET_ADDRSTRLEN );
		if (0 == strcmp( sip, "192.168"))
		{
			KERROR_LOG(pkg_head.id, "connect to Lan Ip!");
			return -1;
		}
		pClient = pClientsMgr->CreateClient();
		if (pClient == 0)
		{
			KERROR_LOG(pkg_head.id, "CreateClient error, mempool full!");
			return -1;
		}
		ExtArg * parg = new ExtArg;
		parg->serialNum = ++g_serialNum;
		parg->clientFd = fdsess->fd;
		parg->port = port;
		strncpy(parg->sip,sip,sizeof(parg->sip)-1);
		parg->sip[sizeof(parg->sip)-1]=0;
		parg->ip = pOnline_Addr->ip.s_addr;
		int ret = asyn_connect_to_svr (sip, port, 200*1024, conn_callback, (void *)parg);

		//int client_online_fd = connect_to_svr(sip, port, 200*1024, 1);
		//if (client_online_fd > 0)
		if (ret==0)
		{
			pClient->m_serverfd = 0;//等待回来
			pClient->m_fdsess = fdsess;
			pClient->m_clientfd = fdsess->fd;
			pClient->m_userid = pkg_head.id;
			pClient->m_serialNum = parg->serialNum;
			if (!pClientsMgr->AddClient(pClient))
			{
				pClientsMgr->ReleaseClient(pClient);
				KERROR_LOG(pkg_head.id, "AddClient error clientfd=%d", pClient->m_clientfd);
				return -1;
			}
		}
		else
		{
			delete parg;
			pClientsMgr->ReleaseClient(pClient);
			KERROR_LOG(pkg_head.id, "asyconnect_to_online_svr error sip=%s port=%d", sip,port );
			return -1;
		}
	}
	else 
	{
		if(pClient)
		{
			if (pClient->m_serverfd > 0)
			{
				//KERROR_LOG(pkg_head.id, "fd=[%d]>0 cmd=%u,pkg.len=%u, len=%d", pClient->m_serverfd,pkg_head.cmd,pkg_head.len,len);
				net_send(pClient->m_serverfd, data, len);
			}
			else
			{
				if (pClient->m_listBufPkgs.size() > 1000)
				{
					KERROR_LOG(pkg_head.id, "m_listBufPkgs full !");
					return -1;
				}
				//缓存客户端包
				BufPkg buf;
				buf.len = len;
				buf.pBuf = new char[len];
				memcpy(buf.pBuf, data, len);
				pClient->m_listBufPkgs.push_back(buf);
				
				//KERROR_LOG(pkg_head.id, "push_back cmd=%u,pkg.len=%u, len=%d", pkg_head.cmd,pkg_head.len,len);
			}
		}
		else
		{
			KERROR_LOG(pkg_head.id, "must first cli_proto_extend_port !");
			return -1;
		}
	}
	return 0;
}

void proc_pkg_from_serv(int fd, void* data, int len)
{
	if (pClientsMgr)
	{
		Client* pClient = pClientsMgr->FindClientByServerFd(fd);
		if (pClient)
		{
			send_pkg_to_client(pClient->m_fdsess, data, len);
		}
	}
}
//static void CloseServerOnlineFd(int serverFd)
//{
	//struct cli_proto_t tempProtoHead = {0};
	//uint32_t tempcmd = cli_proto_cmd_max+1;
	//tempProtoHead.cmd = taomee::bswap(tempcmd);
	//net_send(serverFd, &tempProtoHead, sizeof(tempProtoHead));
	
//}
void on_client_conn_closed(int fd)
{
	DEBUG_LOG( "on_client_conn_closed fd=%d", fd);
	if (pClientsMgr)
	{
		Client* pClient = pClientsMgr->FindClientByFd(fd);
		if (pClient)
		{
			DEBUG_LOG( "on_client_conn_closed uid=%u", pClient->m_userid);
			int serverfd = pClient->m_serverfd;
			pClientsMgr->DeleteClient(pClient);
			//CloseServerOnlineFd(serverfd);
			close_svr(serverfd);
		}
	}
}

void on_fd_closed(int fd)
{
	DEBUG_LOG( "on_fd_closed fd=%d", fd);
	if (pClientsMgr)
	{
		Client* pClient = pClientsMgr->FindClientByServerFd(fd);
		if (pClient)
		{
			int clientfd = pClient->m_clientfd;
			pClientsMgr->DeleteClient(pClient);
			close_client_conn(clientfd);
		}
	}
}


/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
void proc_mcast_pkg(const void* data, int len)
{
	/*const mcast_pkg_t* pkg = reinterpret_cast<const mcast_pkg_t*>(data);
	if (pkg->server_id != get_server_id()) {
		switch (pkg->main_cmd) {
		case mcast_reload_conf:
			reload_conf_op(pkg->minor_cmd, pkg->body, len - sizeof(mcast_pkg_t));
			break;
		case mcast_team_info:
			mcast_team_info_op(pkg->minor_cmd, pkg->body, len - sizeof(mcast_pkg_t));
			break;
		default:
			KERROR_LOG(0, "unsurported cmd %u", pkg->main_cmd);
			break;
		}
	}*/
}

/**
  * @brief Called to reload global data(mainly global function address.etc)
  */
int	reload_global_data()
{
	KDEBUG_LOG(0, "BEGIN RELOADING GLOBAL DATA");
	///*unregister all timer callbacks*/
	//unregister_timers_callback();

	///* reset function address */
	//if (!init_cli_proto_handles() 
	//		|| !init_db_proto_handles()
	//		|| !init_btl_proto_handles()
	//		|| init_all_timer_type()
	//		|| load_xmlconf("./conf/games.xml", load_games)) {
	//	return -1;
	//}
	//		
	//refresh_timers_callback();
	KDEBUG_LOG(0, "RELOAD GLOBAL DATA SUCCESS");

	return 0;
}
