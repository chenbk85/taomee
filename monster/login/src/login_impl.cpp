#include "login_impl.hpp"
#include "message.h"
#include "data_structure.h"
#include "util.h"
#include "libtype.h"

using std::set;
using std::vector;

c_monster_login login;

bool c_monster_login::init_service()
{
    const char *switch_name = config_get_strval("switch_name");
    if (NULL == switch_name || strlen(switch_name) > 31)
    {
        ERROR_LOG("switch name get from config failed.");
        return false;
    }
    strcpy(m_switch_svr_name, switch_name);

    const char *tel_or_net = config_get_strval("tel_or_net");
    if(tel_or_net == NULL)
    {
        ERROR_LOG("tel_or_net get from config failed");
        return false;
    }

    m_tel_or_net = atoi(tel_or_net);
    if(m_tel_or_net  != 0 && m_tel_or_net != 1)
    {
        ERROR_LOG("tel_or_net must be 0 or 1");
        return false;
    }

    return true;
}

void c_monster_login::fini_service()
{
    return;
}

int c_monster_login::get_other_svrs_pkg_len(int fd, const void* avail_data, int avail_len)
{
    return get_dbproxy_pkg_len(avail_data);
}

int c_monster_login::proc_other_cmds(usr_info_t* usr, uint8_t* body, uint32_t bodylen)
{
    KDEBUG_LOG(usr->uid,"CLIENT CMD=%u",usr->waitcmd);
    switch (usr->waitcmd)
    {
        case as_msg_acquire_online:
            return get_svr_list_cmd(usr, body, bodylen);
        default:
            KERROR_LOG(usr->uid, "INVALID CLIENT CMD=%u",usr->waitcmd);
            break;
    }

    return -1;
}

int c_monster_login::proc_other_dbproxy_return(usr_info_t* p, uint16_t cmd, const uint8_t* body, uint32_t bodylen, uint32_t ret)
{
    return 0;
}

void c_monster_login::proc_other_svrs_return(int fd, void* dbpkg, uint32_t pkglen)
{
    if (fd == m_switch_fd)
    {
        DEBUG_LOG("m_switch_fd svrs return %u %u", fd, pkglen);
        handle_switch_return(reinterpret_cast<svr_msg_header_t*>(dbpkg), pkglen);
    }
}

void c_monster_login::on_other_svrs_fd_closed(int fd)
{
    if (fd == m_switch_fd)
    {
        DEBUG_LOG("SWITCH CONNECTION CLOSED\t[fd=%d]", m_switch_fd);
        m_switch_fd = -1;
    }
}

void c_monster_login::handle_switch_return(svr_msg_header_t* dbpkg, uint32_t pkglen)
{
    if (!dbpkg->seq_id)
    {
        ERROR_LOG("switch return seq err");
        return;
    }

    uint32_t counter = dbpkg->seq_id & 0xFFFF;
    int connfd  = dbpkg->seq_id >> 16;
    uint32_t bodylen = dbpkg->len - sizeof(svr_msg_header_t);

    usr_info_t* p = user_mng.get_user(connfd);

    if(!p || (counter != p->counter))
    {
        if (p)
        {
            KERROR_LOG(p->uid, "switch return pkg err fd=%u %u",connfd, dbpkg->user_id);
        }
        else
        {
            ERROR_LOG("user pointer is null");
        }
        return;
    }

    DEBUG_LOG("SWITCH RETURN\t[uid=%u %u fd=%d cmd=%u ret=%u len=%u]",
            p->uid, dbpkg->user_id, connfd, dbpkg->msg_type, dbpkg->result, dbpkg->len);
    int err = -1;
    REMOVE_TIMERS(p);
    switch(dbpkg->msg_type)
    {
        case svr_msg_acquire_online:
            err = get_online_callback(p, dbpkg->body, bodylen, dbpkg->result);
            break;
        default:
            ERROR_LOG("err cmd [%d %d]", dbpkg->msg_type, dbpkg->user_id);
            break;
    }
    if(err) {
        close_client_conn(connfd);
    }
}

void c_monster_login::init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd)
{
    svr_msg_header_t* pkg = reinterpret_cast<svr_msg_header_t*>(buf);

    pkg->len = len;
    pkg->seq_id = (p->session->fd << 16) | p->counter;
    pkg->msg_type = cmd;
    pkg->result = 0;
    pkg->user_id  = p->uid;
}

static int switch_timeout(void* owner, void* data)
{
    usr_info_t* p = reinterpret_cast<usr_info_t*>(owner);
    //c_monster_login* login = reinterpret_cast<c_monstser_login*>(data);
    KDEBUG_LOG(p->uid, "SWITCH TIMEOUT CMD=%d", p->waitcmd);
    close_client_conn(p->session->fd);
    return 0;
}

int c_monster_login::send_to_switch(usr_info_t* p, const uint8_t *buf, uint32_t length)
{
    if (m_switch_fd == -1) {
        m_switch_fd = connect_to_service(m_switch_svr_name, 0, 65535, 1);
    }
    if (m_switch_fd == -1) {
        send_warning(p, 0, p->waitcmd);
        return -1;
    }
    if(p) {
        ADD_TIMER_EVENT(p, switch_timeout, this, get_now_tv()->tv_sec + REQUEST_TIMEOUT);
    }
    return net_send(m_switch_fd , buf, length);
}

int c_monster_login::get_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
    CHECK_VAL(bodylen, 0);
    init_switch_head(p, m_sendbuf, sizeof(svr_msg_header_t) + sizeof(uint8_t), svr_msg_acquire_online);
    *(uint8_t*)(m_sendbuf + sizeof(svr_msg_header_t)) = m_tel_or_net;
    int ret = send_to_switch(p, m_sendbuf, sizeof(svr_msg_header_t) + sizeof(uint8_t));
    if (0 == ret)
    {
        return 0;
    }

    //连接switch失败，使用备用的服务器地址
    if (m_online_addr_vec.size() == 0)
    {
        return -1;
    }

    ++m_index;
    if (m_index == (int)m_online_addr_vec.size())
    {
        m_index = 0;
    }
    KINFO_LOG(p->uid, "get backup addr:[%s:%u]", m_online_addr_vec[m_index].ip, m_online_addr_vec[m_index].port);

    return send_online_addr(p, m_online_addr_vec[m_index].ip, m_online_addr_vec[m_index].port);
}

int c_monster_login::get_online_callback(usr_info_t* p, char* body, uint32_t bodylen, int ret)
{
    CHECK_VAL(bodylen, sizeof(online_addr_t));
    online_addr_t* online_addr = reinterpret_cast<online_addr_t*>(body);

    set<online_addr_t, c_addr_cmp>::iterator iter = m_online_addr_set.find(*online_addr);
    if (iter == m_online_addr_set.end())    //新的地址，加入备用地址里面
    {
        m_online_addr_set.insert(*online_addr);
        m_online_addr_vec.push_back(*online_addr);
    }

    return send_online_addr(p, online_addr->ip, online_addr->port);
}

int c_monster_login::send_online_addr(usr_info_t* p, char *ip, uint16_t port)
{
    m_clipkg->init(as_msg_acquire_online, p->uid, ERR_NO_ERR);
    m_clipkg->pack(ip, 16);
    m_clipkg->pack(port);
    KDEBUG_LOG(p->uid, "get [%s:%u]", ip, port);
    return send_to_user(p);
}
