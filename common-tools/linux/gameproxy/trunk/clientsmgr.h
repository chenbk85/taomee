
#ifndef _CLIENTS_MGR_H_
#define _CLIENTS_MGR_H_
#include "memorypool.h"
#include <map>
#include "extend_proto.h"


class Client;
class ClientsMgr
{
public:
	ClientsMgr(void);
	~ClientsMgr(void);
public:
		
	Client* CreateClient();
	void ReleaseClient(Client* pClient);

	

	Client* FindClient(userid_t userid);
	Client* FindClientByFd(int fd);
	Client* FindClientByServerFd(int fd);

	bool AddClient(Client* pClient);
	bool DeleteClient(Client* pClient);
	void DeleteClientsByflag();

	MemoryPool clientsPool;
	std::map<userid_t, Client*> m_mapClients;
	std::map<int, Client*> m_mapFdClients;
	std::map<int, Client*> m_mapServerFdClients;
};


#endif
