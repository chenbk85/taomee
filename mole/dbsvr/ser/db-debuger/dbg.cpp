
/*
 * =====================================================================================
 *
 *       Filename:  dbg.cpp
 *
 *    Description:  这是一个用于dbser测试用的程序，它将成为一个测试框架
 *
 *        Version:  1.0
 *        Created:  2009年12月02日 20时39分34秒
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra dbg.cpp
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include<iostream>
#include<unistd.h>
#include<cstdlib>

#include<boost/lexical_cast.hpp>

#include"cmd.hpp"
#include"dll.hpp"
#include"cmd_line.hpp"

int main(int argc, char** argv)
{
	db_debuger::usage(argc, argv);

	try
	{
		db_debuger::DllFun::init("./libser.so", "./bench.conf");
	}
	catch(db_debuger::error_dll & e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}

	db_debuger::Cmd::DataType buf;
	if(db_debuger::global_cmd_line_map.count("cmd-value"))
	{
		std::string value_string=db_debuger::global_cmd_line_map["cmd-value"].as<std::string>();
		db_debuger::value_string_parse(buf, value_string);
	}
	uint32_t userid=501000;
	if(db_debuger::global_cmd_line_map.count("userid"))
	{
		userid=db_debuger::global_cmd_line_map["userid"].as<uint32_t>();
	}
	uint16_t cmdid=0;
	if(db_debuger::global_cmd_line_map.count("cmd"))
	{
		cmdid=db_debuger::global_cmd_line_map["cmd"].as<uint16_t>();
	}

	db_debuger::Cmd cmd(cmdid,userid);
	cmd.data(buf);
	cmd.send();

	return 0;
}

