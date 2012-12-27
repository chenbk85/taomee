/**
 * =====================================================================================
 *
 *	@file  dll.cpp
 *
 *	@brief  调取数据库动态链接库内容
 *
 *	@version:  1.0
 *	Created:  2009年12月24日 15时09分10秒
 *	Revision:  none
 *	Compiler:  gcc -Wall -Wextra dll.cpp
 *
 *	@author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *	Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include<iostream>
#include"dll.hpp"

namespace db_debuger
{
	bool DllFun::_dll_ok=false;
	void* DllFun::_dll=0;
	skinfo_t DllFun::_socket_info={0, 0, 0, 0, 0, 0, 0, 0};
	const char* DllFun::_data_on_name="handle_process";
	const char* DllFun::_module_init_name="handle_init";
	DllFun::_data_on_type DllFun::_data_on=0;
	DllFun::_module_init_type DllFun::_module_init=0;

	void DllFun::init(const char* dll_file, const char* conf_file)
	{
		//已经初始化了
		if(_dll_ok) return;

		_dll=::dlopen(dll_file, RTLD_NOW);
		_dll_ok=(_dll!=NULL);
		if(!_dll_ok)
		{
			throw error_dll(dlerror());
		}

		_data_on=(_data_on_type)::dlsym(_dll, _data_on_name);
		_dll_ok=(_data_on!=NULL);
		if(!_dll_ok)
		{
			throw error_dll(dlerror());
		}

		_module_init=(_module_init_type)::dlsym(_dll, _module_init_name);
		_dll_ok=(_module_init!=NULL);
		if(!_dll_ok)
		{
			throw error_dll(dlerror());
		}

		typedef int (*config_init_type)(const char*);
		config_init_type cinit=(config_init_type)::dlsym(_dll, "config_init");
		_dll_ok=(cinit!=NULL);
		if(!_dll_ok)
		{
			throw error_dll(dlerror());
		}
		cinit(conf_file);

		_dll_ok=(0==_module_init(0, 0, PROC_WORK));
		if(!_dll_ok)
		{
			throw error_dll("module init error");
		}
	}

}

