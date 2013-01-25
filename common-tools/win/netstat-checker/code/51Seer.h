// 51Seer.h: interface for the C51Seer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_51SEER_H__D1E152DE_DB16_499B_B444_D2EEC593242A__INCLUDED_)
#define AFX_51SEER_H__D1E152DE_DB16_499B_B444_D2EEC593242A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TaoMee.h"
class C51Seer : public CTaoMee
{
public:
	C51Seer();
	virtual ~C51Seer();
public:
	//获取WEB_FILE文件
	virtual int GetWebFile();
	virtual int TryOnline(unsigned char* session,int len);
};

#endif // !defined(AFX_51SEER_H__D1E152DE_DB16_499B_B444_D2EEC593242A__INCLUDED_)
