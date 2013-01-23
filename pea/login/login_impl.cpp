#include "login_impl.hpp"
#include "server.hpp"
#include "session.hpp"
#include "player.hpp"



#ifdef USE_HEX_LOG_PROTO

int g_log_send_buf_hex_flag = 1;

#else

int g_log_send_buf_hex_flag = 0;

#endif



c_login login;
CliProto2 * g_pkg = login.get_clipkg();

using namespace taomee;



int send_to_user(usr_info_t * p, uint16_t cmd, Cmessage * p_out);



bool c_login::init_service()
{
    load_xmlconf("./conf/server.xml", init_server_data);
    return true;
}


void c_login::fini_service()
{

}


int c_login::get_other_svrs_pkg_len (int fd, const void *avail_data)
{
    return get_dbproxy_pkg_len(avail_data);
}




int c_login::proc_other_cmds (usr_info_t * p, uint8_t * body, uint32_t bodylen)
{

    uint32_t cmd = p->waitcmd;
    uint32_t user_id = p->uid;
    INFO_TLOG("user: %u, proc other cmd: %u, len: %u", user_id, cmd, bodylen);

    const bind_proto_cmd_t * p_cmd = find_login_cmd_bind(cmd);
    if (NULL == p_cmd)
    {
        ERROR_TLOG("invalid cmd: %u", cmd);
        return -1;
    }

    bool unpack_ret = read_from_buf_n(
            p_cmd->p_in, 
            (char *)body,
            bodylen);
    if(!unpack_ret)
    {
        ERROR_TLOG("pkglen error cmd_id = %u len = %u", cmd, bodylen);
        return -1;
    }

    c_player * p_player = (c_player *)(p->tmpinfo);
    p_player->ret = 0;


    return p_cmd->func(p, p_cmd->p_in, p_cmd->p_out, this);
}


int c_login::proc_other_dbproxy_return (usr_info_t *p, uint16_t cmdid, const uint8_t *body, uint32_t bodylen, uint32_t ret)
{
    TRACE_TLOG("user: %u, proc other dbproxy return, cmd: %u(%x), len: %u, ret: %u", p->uid, cmdid, cmdid, bodylen, ret);


    if (db_check_session_cmd == cmdid)
    {
        return check_session_callback(p, body, bodylen, ret);
    }

    // 跳过role_tm
    if (bodylen < 4)
    {
        return -1;
    }
    body += 4;
    bodylen -= 4;


    if (0 != ret)
    {
        ERROR_TLOG("ret: %u", ret);
        g_pkg->init(p->waitcmd, p->uid, ret);
        send_to_user(p);
        return -1;
    }

    const bind_proto_cmd_t * p_cmd = find_db_cmd_bind(cmdid);
    if (NULL == p_cmd)
    {

        ERROR_TLOG("invalid db cmd: %u", cmdid);
        g_pkg->init(p->waitcmd, p->uid, LOGIN_ERR_SYSTEM_FAULT);
        send_to_user(p);
        return -1;
    }

    bool unpack_ret = p_cmd->p_out->read_from_buf_ex((char *)body, bodylen);

    if(!unpack_ret)
    {
        ERROR_TLOG("pkglen error cmd_id = %u len = %u", cmdid, bodylen);
        g_pkg->init(p->waitcmd, p->uid, LOGIN_ERR_MSG_LEN);
        send_to_user(p);
        return -1;
    }

    int err = p_cmd->func(p, p_cmd->p_in, p_cmd->p_out, NULL);

    if (err)
    {
        return -1;
    }
    return 0;
}


void c_login::proc_other_svrs_return (int fd, void *dbpkg, uint32_t pkglen)
{

}





void c_login::on_other_svrs_fd_closed (int fd)
{

}


int send_to_user(usr_info_t * p, uint16_t cmd, Cmessage * p_out)
{
    c_player * p_player = (c_player *)(p->tmpinfo);
    g_pkg->init(cmd, p->uid, p_player->ret);

    if (NULL != p_out)
    {
        byte_array_t ba;
        ba.set_is_bigendian(true);
        p_out->write_to_buf_ex(ba);

        g_pkg->pack(ba.get_buf(), ba.get_postion());
    }


    p->waitcmd = 0;
    p->inc_counter();
    g_pkg->send_to(p->session);


#ifdef USE_HEX_LOG_PROTO
    if (g_log_send_buf_hex_flag)
    {
        static char outbuf[13000];
        uint32_t len = taomee::bswap(g_pkg->m_len);
        bin2hex(outbuf, (char *)g_pkg, len, 2000);
        TRACE_TLOG("[CO] %s", outbuf);
    }
#endif

    return 0;
}

int send_to_dbproxy(usr_info_t * p, uint16_t cmd, const void * buf, uint32_t body_len)
{
    return login.send_to_dbproxy(p, cmd, buf, body_len);

}

int proto_get_server_list(DEFAULT_ARG) 
{

    proto_get_server_list_in * p_in = P_IN;

    c_player * p_player = (c_player *)(p->tmpinfo);
    memcpy(p_player->session, p_in->session, SESSION_LEN);

    // 一般release版本才要检查session
#ifdef CHECK_SESSION
    return check_session(p);
#else

    // 跳过session验证
    return do_proto_get_server_list(p, NULL, NULL, NULL);

#endif
}


int proto_get_role_list(DEFAULT_ARG)
{
    proto_get_role_list_in * p_in = P_IN;

    uint32_t server_id = p_in->server_id;

    int idx = 0;

    uint32_t role_tm = 0;

    pack_h(g_pkg, role_tm, idx);
    pack_h(g_pkg, p->uid, idx);
    pack_h(g_pkg, server_id, idx);


    return send_to_dbproxy(p, db_proto_get_role_list_cmd, g_pkg, idx);

}


int proto_create_role(DEFAULT_ARG)
{
    proto_create_role_in * p_in = P_IN;

    uint32_t server_id = p_in->server_id;
    uint32_t eye_model = p_in->model.eye_model;
    uint32_t resource_id = p_in->model.resource_id;
    char * nick = p_in->nick;


    int idx = 0;

    uint32_t role_tm = 0;
    pack_h(g_pkg, role_tm, idx);
    pack_h(g_pkg, p->uid, idx);
    pack_h(g_pkg, server_id, idx);
    pack(g_pkg, nick, MAX_NICK_SIZE, idx);
    pack_h(g_pkg, eye_model, idx);
    pack_h(g_pkg, resource_id, idx);

    return send_to_dbproxy(p, db_proto_create_role_cmd, g_pkg, idx);
}


int do_proto_get_server_list(DEFAULT_ARG)
{
    proto_get_server_list_out out;
    proto_get_server_list_out * p_out = &out;

    container_for_each(server_data_mgr::instance(), it)
    {

        server_data_t * p_data = it->second;
        server_info_t info;
        info.server_id = p_data->server_id;
        memcpy(info.server_ip, p_data->server_ip, sizeof(info.server_ip));
        info.server_port = p_data->server_port;


        p_out->server_info.push_back(info);

    }

    p_out->server_total = p_out->server_info.size();


    return send_to_user(p, p->waitcmd, p_out);
}


int db_proto_get_role_list_callback(DEFAULT_ARG)
{

    db_proto_get_role_list_out * p_out = P_OUT;
    proto_get_role_list_out out;

    byte_array_t ba;

    p_out->write_to_buf_ex(ba);
    out.read_from_buf_ex(ba);


    return send_to_user(p, p->waitcmd, &out);
}


int db_proto_create_role_callback(DEFAULT_ARG)
{
    db_proto_create_role_out * p_out = P_OUT;
    proto_create_role_out out;
    out.role_tm = p_out->db_user_id.role_tm;
    return send_to_user(p, p->waitcmd, &out);
}
