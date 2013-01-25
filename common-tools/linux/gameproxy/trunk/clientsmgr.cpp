#include "client.h"
#include "clientsmgr.h"
using namespace std;
extern "C" {
#include <async_serv/net_if.h>
#include <libtaomee/log.h>
}
ClientsMgr::ClientsMgr(void)
{
	clientsPool.InitMemoryPool(100000, sizeof(Client));
}

ClientsMgr::~ClientsMgr(void)
{
}

Client* ClientsMgr::CreateClient()
{
	return new(clientsPool.GetMemory())Client();
}

void ClientsMgr::ReleaseClient(Client* pClient)
{
	pClient->~Client();
	clientsPool.FreeMemory(pClient);
}

Client* ClientsMgr::FindClient(userid_t userid)
{
	std::map<userid_t, Client*>::iterator itr = m_mapClients.find(userid);
	if (itr != m_mapClients.end())
	{
		return itr->second;
	}
	return 0;
}

Client* ClientsMgr::FindClientByFd(int fd)
{
	std::map<int, Client*>::iterator itr = m_mapFdClients.find(fd);
	if (itr != m_mapFdClients.end())
	{
		return itr->second;
	}
	return 0;
}

Client* ClientsMgr::FindClientByServerFd(int fd)
{
	std::map<int, Client*>::iterator itr = m_mapServerFdClients.find(fd);
	if (itr != m_mapServerFdClients.end())
	{
		return itr->second;
	}
	return 0;
}

bool ClientsMgr::AddClient(Client* pClient)
{
	if (pClient==0 || pClient->m_fdsess==0)
	{
		return false;
	}
	if (FindClientByFd(pClient->m_clientfd))
	{
		return false;
	}
	m_mapClients.insert(make_pair(pClient->m_userid, pClient));
	m_mapFdClients.insert(make_pair(pClient->m_clientfd, pClient));
	//m_mapServerFdClients.insert(make_pair(pClient->m_serverfd, pClient));
	return true;
}

bool ClientsMgr::DeleteClient(Client* pClient)
{
	if (pClient)
	{
		DEBUG_LOG("DeleteClient serverfd=%d clientfd= %d",pClient->m_serverfd,pClient->m_clientfd);

		m_mapClients.erase(pClient->m_userid);
		int num = m_mapFdClients.erase(pClient->m_clientfd);
		if (pClient->m_serverfd > 0)
		{
			m_mapServerFdClients.erase(pClient->m_serverfd);
		}
		
		if (num > 0)
		{
			ReleaseClient(pClient);
		}

		return true;
	}
	return false;
}
