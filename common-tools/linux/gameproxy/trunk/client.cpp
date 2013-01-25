#include "client.h"

Client::Client(void)
	:m_fdsess(0)
	,m_serverfd(-1)
	,m_clientfd(-1)
	,m_userid(0)
	,m_serialNum(0)
{
}

Client::~Client(void)
{
	m_serverfd = -1;
	for (list<BufPkg>::iterator itr = m_listBufPkgs.begin(); itr != m_listBufPkgs.end(); ++itr)
	{
		BufPkg& buf = (*itr);
		delete [] buf.pBuf;
	}
}


