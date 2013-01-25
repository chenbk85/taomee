// Base64.h: interface for the CBase64 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASE64_H__862D8A17_F846_4E4F_95C6_3431A64516DA__INCLUDED_)
#define AFX_BASE64_H__862D8A17_F846_4E4F_95C6_3431A64516DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class CBase64  
{
public:
	CBase64();
	virtual ~CBase64();
	std::string base64_encode(unsigned char const* , unsigned int len);
	std::string base64_decode(std::string const& s);
	static inline bool is_base64(unsigned char c)
	{return (isalnum(c) || (c == '+') || (c == '/'));};
};

#endif // !defined(AFX_BASE64_H__862D8A17_F846_4E4F_95C6_3431A64516DA__INCLUDED_)
