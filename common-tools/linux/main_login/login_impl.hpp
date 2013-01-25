/**
 *============================================================
 *  @file      login_impl.hpp
 *  @brief     在这里编写登录服务器的具体实现。现在你看到的只是主登录的一个简单实现例子，
 *             如果你需要实现拉服务器列表、创建角色等功能，请继承Login类，然后实现具体细节。
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef LIBTAOMEEPP_LOGIN_IMPL_HPP_
#define LIBTAOMEEPP_LOGIN_IMPL_HPP_

//#include <libtaomee++/project/main_login/cli_proto.hpp>
//#include <libtaomee++/project/main_login/main_login.hpp>
#include <main_login/cli_proto.hpp>
#include <main_login/main_login.hpp>

/**
 * @brief 主登录类型
 */
typedef Login<CliProto2> MainLogin;

/**
 * @brief 主登录实例
 */
extern MainLogin login;

#endif // LIBTAOMEEPP_LOGIN_IMPL_HPP_
