/**
 * =====================================================================================
 *
 *	@file  dll.hpp
 *
 *	@brief  调取数据库动态链接库内容
 *
 *	@version:  1.0
 *	Created:  2009年12月24日 13时57分14秒
 *	Revision:  none
 *	Compiler:  gcc -Wall -Wextra dll.hpp
 *
 *	@author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *	Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef M_dbser_dll_hpp
#define M_dbser_dll_hpp

#include<string>
#include<cstdlib>
#include<dlfcn.h>

#include"benchapi.h"


namespace db_debuger
{
class error_dll
{
public:
	error_dll(const std::string& msg)
		:_msg(msg)
		{}

	error_dll(const char* msg)
		:_msg(msg)
		{}

	const std::string& what()
	{
		return _msg;
	}

private:
	std::string _msg;
};

class DllFun
{
public:
	void data_on(const char* d, int len)
	{
		_return=_data_on(const_cast<char*>(d), len, const_cast<char**>(&_receive_buf), (int*)&_receive_length, &_socket_info);
	}

	const char* receive_buf() const
	{
		return _receive_buf;
	}

	size_t receive_length() const
	{
		return _receive_length;
	}

	bool good()
	{
		return 0==_return;
	}

	operator bool()
	{
		return _dll_ok;
	}

	void clear()
	{
		_return=0;
		std::free((void*)_receive_buf);
		_receive_buf=0;
		_receive_length=0;
	}

	static void init(const char* dll_file, const char* conf_file);

private:
	int _return;
	const char *_receive_buf;
	size_t _receive_length;

private:
	typedef int (*_data_on_type)(char *send_buf, int sent_length, char **receive_buf, int *receive_length, const skinfo_t *);
	typedef int (*_module_init_type)(int , char**, int pid_type);

	static bool _dll_ok;
	static void* _dll;
	static skinfo_t _socket_info;
	static const char* _data_on_name;
	static const char* _module_init_name;
	static _data_on_type _data_on;
	static _module_init_type _module_init;
};
}

#endif //M_dbser_dll_hpp

