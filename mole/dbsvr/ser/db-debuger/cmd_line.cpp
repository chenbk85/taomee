/**
 * =====================================================================================
 *
 *	@file  cmd_line.cpp
 *
 *	@brief  DB debuger 命令行解析
 *
 *	@version:  1.0
 *	Created:  2010年01月07日 10时49分49秒
 *	Revision:  none
 *	Compiler:  gcc -Wall -Wextra cmd_line.cpp
 *
 *	@author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *	Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include<boost/program_options.hpp>
#include"cmd_line.hpp"

namespace db_debuger
{

po::variables_map global_cmd_line_map;
po::options_description global_cmd_line_description("允许的命令行：");

void cmd_line(int& argc, char**& argv)
{
	po::options_description& desc=global_cmd_line_description;
	desc.add_options()
	    ("help", "产生一个简单的帮助信息")
		("cmd", po::value<uint16_t>(), "指定要执行的数据库命令号")
		("userid", po::value<uint32_t>(), "指定这个命令所针对的用户米米号")
		("cmd-value", po::value<std::string>(), "依字符串格式指定的命令传入的值")
		;

	po::variables_map& vm=global_cmd_line_map;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);  
}

void usage(int& argc, char**& argv)
{
	try {
		cmd_line(argc, argv);
	} catch(...) {
		
		std::cout << global_cmd_line_description << std::endl;
		std::exit(0);
	}
	if(1==argc || global_cmd_line_map.count("help"))
	{
		std::cout << global_cmd_line_description << std::endl;
		std::exit(0);
	}
}

}

