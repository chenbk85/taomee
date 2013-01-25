// 51Mole.h: interface for the C51Mole class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_51MOLE_H__56030BD9_D12F_4BED_B7F9_9889EA065F2C__INCLUDED_)
#define AFX_51MOLE_H__56030BD9_D12F_4BED_B7F9_9889EA065F2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TaoMee.h"
class C51Mole : public CTaoMee
{
public:
	C51Mole();
	virtual ~C51Mole();
public:
	//获取WEB_FILE文件
	virtual int GetWebFile();
	virtual int TryOnline(unsigned char* session,int len);
};

#endif // !defined(AFX_51MOLE_H__56030BD9_D12F_4BED_B7F9_9889EA065F2C__INCLUDED_)
