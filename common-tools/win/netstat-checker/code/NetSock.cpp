// NetSock.cpp: implementation of the CNetSock class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "netTest.h"
#include "NetSock.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetSock::CNetSock()
{
	InitWSA();
	Init();
}

CNetSock::~CNetSock()
{
	CloseSocket();
	CleanWSA();
}

int CNetSock::CreateSock( int nPORT,char* pszIP,int nType )
{
	m_sa.sin_family = AF_INET;       
	m_sa.sin_port = htons(nPORT);
	m_sa.sin_addr.s_addr = inet_addr(pszIP);
	
	if (INVALID_SOCKET == (m_s = socket(AF_INET, nType,0)))
	{
		return -1;
	}
	return 0;
}

int CNetSock::Connect()
{
	int nRes = 0;
	if (SOCKET_ERROR == connect(m_s, (sockaddr*)&m_sa, sizeof(m_sa)))
	{
		CloseSocket();
		nRes = -1;
	}
	return nRes;
}

int CNetSock::InitWSA()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD( 2, 2 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return -1;
	}
	
	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */
	
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup( );
		return -1; 
	}
	
	/* The WinSock DLL is acceptable. Proceed. */
	return 0;
}

void CNetSock::CleanWSA()
{
	WSACleanup();
}

void CNetSock::InvalidSocket()
{
	m_s = INVALID_SOCKET;
}

void CNetSock::CloseSocket()
{
	if (INVALID_SOCKET != m_s)
	{
		closesocket(m_s);
		InvalidSocket();
	}
}

int CNetSock::Send( char* pszData,int nLen )
{
	int nSend = 0;
	int nTemp = 0;
	while (1)
	{
		nTemp = send(m_s, pszData+nSend,nLen-nSend, 0);
		if (SOCKET_ERROR == nTemp)
		{
			return -1;
		}
		nSend += nTemp;
		if (nSend == nLen)
		{
			break;
		}
	}
	
	return nSend;
}

int CNetSock::Recv( char* pszData,int nLen )
{
	int nRecv = 0;
	int nRst = 0;
 	nRecv = recv(m_s,pszData, nLen,0);
	switch (nRecv)
	{
	case 0:
	case SOCKET_ERROR:
		nRst = -1;
		break;
	default:
		nRst = nRecv;
		break;
	}
	return nRst;
}

void CNetSock::Init()
{
	InvalidSocket();
	memset(&m_sa,0,sizeof(m_sa));
	CloseSocket();
}