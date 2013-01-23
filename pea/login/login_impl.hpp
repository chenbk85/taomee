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

#ifndef LIBTAOMEEPP_TLOGIN_IMPL_HPP_
#define LIBTAOMEEPP_TLOGIN_IMPL_HPP_

//#include <libtaomee++/project/main_login/cli_proto.hpp>
//#include <libtaomee++/project/main_login/main_login.hpp>
#include <main_login/cli_proto.hpp>
#include <main_login/main_login.hpp>

#include "proto.hpp"

class c_login : public Login<CliProto2>
{

    public:

        inline CliProto2 * get_clipkg()
        {
            return m_clipkg;

        }


        inline int send_to_user(usr_info_t * p)
        {
            return Login<CliProto2>::send_to_user(p);
        }

        inline int send_to_dbproxy(usr_info_t * p, uint16_t cmd, const void * dbpkgbuf, uint32_t body_len)
        {
            return Login<CliProto2>::send_to_dbproxy(p, p->uid, cmd, dbpkgbuf, body_len);
        }

    private:


        // 副登录如果需要初始化额外的内容，比如说读取配置文件等等，可以通过实现这个接口来完成。
        // 返回：正常的话，返回true，如果出现任何初始化失败的状况，请返回false。
        bool init_service ();
        // 副登录如果需要清理额外的内容，比如说释放内存等等，可以通过实现这个接口来完成
        void fini_service ();
        // 副登录如果需要解析其它服务器来包（比如说Switch）的长度，可以通过实现这个接口来完成。 一般来说，因为服务器来包包头都一样，可以直接在这里调用get_dbproxy_pkg_len。
        // 返回: 解析出来的数据包长度，或者如果失败的话，返回-1
        int get_other_svrs_pkg_len (int fd, const void *avail_data);
        // 副登录如果需要处理其它的客户端协议命令，比如说拉去服务器列表/创建角色，可以通过实现这个接口来完成。
        // 返回: 一切正常的话，返回0，如果客户端发包非法，或者处理过程中出现系统级的错误，请返回-1
        int proc_other_cmds (usr_info_t *usr, uint8_t *body, uint32_t bodylen);
        // 副登录如果需要处理其它的dbproxy回报，比如说创建角色的返回包，可以通过实现这个接口来完成
        // 返回: 一切正常的话，返回0，如果DBproxy返回包非法，或者处理过程中出现系统级的错误，请返回-1
        int proc_other_dbproxy_return (usr_info_t *p, uint16_t cmdid, const uint8_t *body, uint32_t bodylen, uint32_t ret);
        // 副登录如果需要处理其它服务器的返回，比如说Switch的返回，可以通过实现这个接口来完成
        void proc_other_svrs_return (int fd, void *dbpkg, uint32_t pkglen);
        // 副登录如果需要处理其它服务器断开连接的情况，比如说Switch断开连接，可以通过实现这个接口来完成
        void on_other_svrs_fd_closed (int fd);




};


int send_to_user(usr_info_t * p, uint16_t cmd, Cmessage * p_out);
int send_to_dbproxy(usr_info_t * p, uint16_t cmd, const void * buf, uint32_t body_len);


int do_proto_get_server_list(DEFAULT_ARG);




extern c_login login;
extern CliProto2 * g_pkg;

#endif // LIBTAOMEEPP_TLOGIN_IMPL_HPP_
