// Dustman.h: interface for the CDustman class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUSTMAN_H__550C410E_51FC_4335_973E_6319C92DC295__INCLUDED_)
#define AFX_DUSTMAN_H__550C410E_51FC_4335_973E_6319C92DC295__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <wininet.h>
#include <UrlHist.h>
#define SWEEP_BUFFER_SIZE			10000
#pragma comment (lib,"Wininet.lib")
#pragma comment (lib,"shlwapi.lib")

class CDustman  
{
public:
	CDustman();
	virtual ~CDustman();
public:
	BOOL ClearInternetTempFile();
private:
	enum DEL_CACHE_TYPE //要删除的类型。
	{
		File,//表示internet临时文件
		Cookie //表示Cookie
	};
	BOOL DeleteUrlCache(DEL_CACHE_TYPE eType);
	BOOL EmptyDirectory(LPCTSTR szPath, BOOL bDeleteDesktopIni = FALSE,   BOOL bWipeIndexDat = FALSE);
	BOOL WipeFile(LPCTSTR szDir, LPCTSTR szFile);

};

#endif // !defined(AFX_DUSTMAN_H__550C410E_51FC_4335_973E_6319C92DC295__INCLUDED_)
