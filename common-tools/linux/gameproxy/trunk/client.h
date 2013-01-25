
#ifndef _CLIENT_H_
#define _CLIENT_H_
extern "C" {
#include <async_serv/service.h>
#include <libtaomee/project/types.h>
}
#include <list>
using namespace std;
struct BufPkg
{
	int len;
	char* pBuf;
};

class Client
{
public:
	Client(void);
	~Client(void);
public:
	fdsession_t*	m_fdsess;
	int				m_serverfd;	
	int				m_clientfd;
	userid_t		m_userid;
	uint32_t		m_serialNum;
	list<BufPkg>		m_listBufPkgs;
};


#endif
