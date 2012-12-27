/**
 * =====================================================================================
 *
 *	@file  cmd_line.hpp
 *
 *	@brief  DB debuger 命令行解析
 *
 *	@version:  1.0
 *	Created:  2010年01月07日 10时45分08秒
 *	Revision:  none
 *	Compiler:  gcc -Wall -Wextra cmd_line.hpp
 *
 *	@author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *	Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#ifndef M_db_debuger_cmd_line_hpp
#define M_db_debuger_cmd_line_hpp

#include<boost/program_options.hpp>
#include<iostream>
#include<cstdlib>

namespace db_debuger
{
namespace po = boost::program_options;
extern po::variables_map global_cmd_line_map;
extern po::options_description global_cmd_line_description;

void cmd_line(int& argc, char**& argv);

void usage(int& argc, char**& argv);

}

#endif //M_db_debuger_cmd_line_hpp

