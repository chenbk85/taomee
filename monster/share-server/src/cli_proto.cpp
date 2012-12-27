/**
 * =====================================================================================
 *       @file  cli_proto.cpp
 *      @brief  as协议相关的处理函数
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/30/2011 10:04:54 AM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <glib.h>
#include <assert.h>
#include <math.h>
#include <map>
#include <vector>
#include <list>
#include <queue>
#include <libtaomee++/inet/pdumanip.hpp>
extern "C"
{
#include <async_serv/net_if.h>
}

#include "message.h"
#include "data_structure.h"
#include "constant.h"
#include "stat.h"

#include "cli_proto.h"
#include "data.h"
#include "function.h"
#include "utility.h"

using namespace taomee;
using std::map;
using std::list;
using std::queue;
using std::vector;

void init_cli_handle_funs()
{
    g_cli_cmd_map.clear();

    g_cli_cmd_map[as_msg_room_info] = process_get_room_info;                        //40104
    g_cli_cmd_map[as_msg_get_user_info] = process_get_user_info;                    //40112
    g_cli_cmd_map[online_msg_login_user] = process_new_user_login;                  //40001
    g_cli_cmd_map[as_msg_login_user] = process_login_user;                          //40002

    return;
}

int dispatch(void *p_data, int len, fdsession_t *fdsess, bool first_tm)
{
    as_msg_header_t *p_pkg = (as_msg_header_t *)p_data;
    as_msg_header_t pkg_header = {0};
    pkg_header.len = taomee::bswap(p_pkg->len);
    pkg_header.msg_type = taomee::bswap(p_pkg->msg_type);
    pkg_header.user_id = taomee::bswap(p_pkg->user_id);

    map<uint16_t, PROTO_PROCESS_FUNC>::iterator cmd_iter;
    int ret = 0;
    usr_info_t *p_user = NULL;
    g_errno = ERR_NO_ERR;
    DEBUG_LOG("AS\t[msg:%u, len:%u]", pkg_header.msg_type, pkg_header.len);

    p_user = g_user_mng.get_user(fdsess->fd);
    if (NULL == p_user) //新连接，分配用户缓存
    {
        p_user = g_user_mng.alloc_user(fdsess->fd, pkg_header.user_id, fdsess);
        if (NULL == p_user)
        {
            ERROR_LOG("alloc user failed");
            return -1;
        }
	p_user->uid = pkg_header.user_id;
    }
    p_user->uid = pkg_header.user_id;

    if (first_tm && p_user->waitcmd != 0) //有包还没处理完，需要缓存在命令链表里
    {
        if (p_user->cached_pkg_queue.size() >= MAX_REQ_PER_USER)
        {
            ERROR_LOG("too many cmd in process");
            g_errno = ERR_TOO_MANY_PKG;
            goto EXIT;
        }

        if (cache_a_pkg(p_user, (char *)p_pkg, pkg_header.len) != 0)
        {
            ERROR_LOG("cache pkg failed");
            return -1;
        }
        return 0;
    }

    assert(p_user != NULL);
    //当前请求可以马上处理,设置正在处理的协议号
    cmd_iter = g_cli_cmd_map.find(pkg_header.msg_type);
    //未知的协议
    if (cmd_iter == g_cli_cmd_map.end())
    {
        ERROR_LOG("invalid msg:%u", pkg_header.msg_type);
        return -1;
    }

    p_user->waitcmd = pkg_header.msg_type;
    ret = cmd_iter->second(p_user, p_pkg->body, pkg_header.len - sizeof(as_msg_header_t));
    if (ret != 0)
    {
        return -1;
    }

EXIT:
    if (g_errno != 0) //协议处理失败，给客户端回失败消息
    {
        send_header_to_user(p_user, SYSTEM_ERR_NUM, g_errno);
    }

    return 0;
}

int cache_a_pkg(usr_info_t *p_user, const char *buf, uint32_t buf_len)
{
    uint16_t len = sizeof(cached_pkg_t) + buf_len;
    cached_pkg_t *cache_buf = reinterpret_cast<cached_pkg_t *>(g_slice_alloc(len));
    if (NULL == cache_buf)
    {
        return -1;
    }
    cache_buf->len = len;
    memcpy(cache_buf->pkg, buf, buf_len);
    p_user->cached_pkg_queue.push(cache_buf);
    if (p_user->waiting_flag == 0)   //用户不在等待链表里面，需要加入
    {
        g_waiting_user_list.insert(g_waiting_user_list.end(), p_user->session->fd);
        p_user->waiting_flag = 1;
    }
    return 0;
}

void proc_cached_pkgs()
{
    for (list<int>::iterator iter = g_waiting_user_list.begin(); iter != g_waiting_user_list.end();)    //遍历所有待处理的用户
    {
        usr_info_t *p_user = g_user_mng.get_user(*iter);
        if (NULL == p_user) //用户已经断开连接,将用户从链表里面删除
        {
            list<int>::iterator cur_iter = iter;
            ++iter;
            g_waiting_user_list.erase(cur_iter);
            continue;
        }

        queue<cached_pkg_t *> &cmd_queue = p_user->cached_pkg_queue;
        while (0 == p_user->waitcmd)   //当前用户没有命令再处理，则从命令队列里去命令处理
        {
            cached_pkg_t *p_cached_cmd = cmd_queue.front();
            if(p_cached_cmd == NULL)
            {
                CRIT_LOG("user %u cmd_queue is empty.", p_user->session->fd);
                break;
            }

            int err = dispatch(p_cached_cmd->pkg, p_cached_cmd->len - sizeof(cached_pkg_t), p_user->session, false);
            usr_info_t *p_tmp_usr = g_user_mng.get_user(*iter);
            if(p_tmp_usr == NULL)
            {
                CRIT_LOG("user %u deleted already.", p_user->session->fd);
                break;
            }

            g_slice_free1(p_cached_cmd->len, p_cached_cmd);
            cmd_queue.pop();
            if (0 == err)
            {
                if (cmd_queue.empty())
                {
                    list<int>::iterator cur_iter = iter;
                    ++iter;
                    g_waiting_user_list.erase(cur_iter);
                    p_user->waiting_flag = 0;
                    break;
                }
            }
            else
            {
                close_client_conn(p_user->session->fd);
                break;
            }
        }

        ++iter;
    }
}

int process_get_room_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, (int)msg_body_len, (int)sizeof(as_msg_get_room_req_t), 0);

    //获得包体
    as_msg_get_room_req_t *p_req = &p_user->buffer;
    int idx = 0;
    taomee::unpack(p_msg_body, p_req->user_id, idx);
    taomee::unpack(p_msg_body, p_req->room_id, idx);

    //pack请求包
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    db_msg_get_room_req_t *p_body = (db_msg_get_room_req_t *)(g_send_buffer + sizeof(svr_msg_header_t));

    pack_svr_pkg_head(p_header, svr_msg_db_room_info, p_req->user_id, GET_SVR_SN(p_user), sizeof(db_msg_get_room_req_t));

    p_body->room_id = p_req->room_id;
    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_DB_PROXY;
        CRIT_LOG("send data to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_get_user_info(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, (int)sizeof(as_msg_get_user_req_t), 0);

    int idx = 0;
    as_msg_get_user_req_t req = {0};
    taomee::unpack(p_msg_body, req.user_id, idx);

    INFO_LOG("get user:%u info from db", req.user_id);
    svr_msg_header_t *p_header = (svr_msg_header_t *)g_send_buffer;
    pack_svr_pkg_head(p_header, svr_msg_db_other_role_info, req.user_id, GET_SVR_SN(p_user), 0);

    if (g_p_db_proxy_svr->send_data(p_user, g_send_buffer, p_header->len, timeout_cb) != 0)
    {
        g_errno = ERR_MSG_ACCOUNT;
        CRIT_LOG("send to db proxy failed.");
        return 0;
    }

    return 0;
}

int process_new_user_login(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, (int)sizeof(as_user_login_req_t), 0);

    int idx = 0;
    user_login_info_t req = {0};
    req.user_id = p_user->uid;
    taomee::unpack(p_msg_body, req.gender, idx);
    taomee::unpack(p_msg_body, req.name, sizeof(req.name), idx);

    p_user->waitcmd = 0;

    //遍历当前用户，看用户在不在链表里面
    for (list<user_login_info_t>::iterator iter = g_login_user_list.begin(); iter != g_login_user_list.end(); ++iter) 
    {
        if (iter->user_id == req.user_id)  //用户在链表里面
        {
            KINFO_LOG(p_user->uid, "already in login user");
            g_login_user_list.erase(iter);
            g_login_user_list.insert(g_login_user_list.end(), req); 

            return 0; 
        }
    }

    //用户不再链表里面
    if ((int)g_login_user_list.size() == g_max_login_user)   //链表已满，删除最前登录的用户，加入当前用户
    {
        g_login_user_list.erase(g_login_user_list.begin());
    	g_login_user_list.insert(g_login_user_list.end(), req); 
    	KINFO_LOG(p_user->uid, "full, del first user");

	return 0;
    }

    //直接插入当前用户
    g_login_user_list.insert(g_login_user_list.end(), req); 
    KINFO_LOG(p_user->uid, "add to login user");

    return 0;
}

int process_login_user(usr_info_t *p_user, char *p_msg_body, uint16_t msg_body_len)
{
    assert(p_user != NULL && p_msg_body != NULL);
    CHECK_VAL(0, msg_body_len, 0, 0);

    pack_as_pkg_header(0, p_user->waitcmd, 0, ERR_NO_ERR);
    g_send_msg.pack((uint16_t)g_login_user_list.size());
    for (list<user_login_info_t>::reverse_iterator iter = g_login_user_list.rbegin(); iter != g_login_user_list.rend(); ++iter) 
    {
        g_send_msg.pack(iter->user_id);
        g_send_msg.pack(iter->name, sizeof(iter->name));
        g_send_msg.pack(iter->gender);
    }
    
    g_send_msg.end();
    send_data_to_user(p_user, g_send_msg.get_msg(), g_send_msg.get_msg_len());

    return 0;
}
