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

#include <set>
#include <vector>
//#include <libtaomee++/project/main_login/cli_proto.hpp>
//#include <libtaomee++/project/main_login/main_login.hpp>
#include <main_login/cli_proto.hpp>
#include <main_login/main_login.hpp>
#include "constant.h"
#include "data_structure.h"
#include "libtype.h"

class c_addr_cmp
{
public:
    bool operator()(const online_addr_t &key_a, const online_addr_t &key_b) const
    {
        if(strcmp(key_a.ip, key_b.ip) < 0)
        {
            return true;
        }

        if(0 == strcmp(key_a.ip, key_b.ip))
        {
            if(key_a.port < key_b.port)
            {
                return true;
            }
        }

        return false;
    }
};

/**
 * @brief 主登录类型
 */
typedef Login<CliProto2> MainLogin;

class c_monster_login : public MainLogin
{
public:
    c_monster_login() : m_switch_fd(-1), m_index(-1)
    {

    }

    ~c_monster_login()
    {
        if (m_switch_fd >= 0)
        {
            close(m_switch_fd);
            m_switch_fd = -1;
            m_index = -1;
        }
    }

private:
    /**
     * @brief 副登录如果需要初始化额外的内容，比如说读取配置文件等等，可以通过实现这个接口来完成
     * @return 正常的话，返回true，如果出现任何初始化失败的状况，请返回false。
     */
    bool init_service();

    /**
     * @brief 副登录如果需要清理额外的内容，比如说释放内存等等，可以通过实现这个接口来完成
     */
    void fini_service();

    /**
     * @brief 副登录如果需要解析其它服务器来包（比如说Switch）的长度，可以通过实现这个接口来完成。
     *        一般来说，因为服务器来包包头都一样，可以直接在这里调用get_dbproxy_pkg_len。
     * @return 解析出来的数据包长度，或者如果失败的话，返回-1
     */
    int get_other_svrs_pkg_len(int fd, const void* avail_data, int avail_len);


    /**
     * @brief 副登录如果需要处理其它的客户端协议命令，比如说拉去服务器列表/创建角色，可以通过实现这个接口来完成
     * @return 一切正常的话，返回0，如果客户端发包非法，或者处理过程中出现系统级的错误，请返回-1
     */
    int  proc_other_cmds(usr_info_t* usr, uint8_t* body, uint32_t bodylen);

    /**
     * @brief 副登录如果需要处理其它的dbproxy回报，比如说创建角色的返回包，可以通过实现这个接口来完成
     * @return 一切正常的话，返回0，如果DBproxy返回包非法，或者处理过程中出现系统级的错误，请返回-1
     */
    int  proc_other_dbproxy_return(usr_info_t* p, uint16_t cmd, const uint8_t* body, uint32_t bodylen, uint32_t ret);

    /**
     * @brief 副登录如果需要处理其它服务器的返回，比如说Switch的返回，可以通过实现这个接口来完成
     */
    void proc_other_svrs_return(int fd, void* dbpkg, uint32_t pkglen);

    /**
     * @brief 副登录如果需要处理其它服务器断开连接的情况，比如说Switch断开连接，可以通过实现这个接口来完成
     */
    void on_other_svrs_fd_closed(int fd);

private:
    void handle_switch_return(svr_msg_header_t* dbpkg, uint32_t pkglen);
    void init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd);
    int send_to_switch(usr_info_t* p, const uint8_t *buf, uint32_t length);

    /**
     * @brief 通过switch获得online的ip和端口
     */
    int get_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);

    /**
     * @brief 处理获得online的ip和端口的返回包
     */
    int get_online_callback(usr_info_t* p, char* body, uint32_t bodylen, int ret);

    /**
     * @brief 返回as获得的online地址
     */
    int send_online_addr(usr_info_t* p, char *ip, uint16_t port);

private:
    uint8_t m_sendbuf[MAX_SEND_PKG_SIZE];
    int m_switch_fd;
    char m_switch_svr_name[32];

    std::vector<online_addr_t> m_online_addr_vec;
    std::set<online_addr_t, c_addr_cmp> m_online_addr_set;
    int m_index;

    uint8_t m_tel_or_net;//电信(0)or网通(1)
};



/**
 * @brief 主登录实例
 */
extern c_monster_login login;

#endif // LIBTAOMEEPP_LOGIN_IMPL_HPP_
