/** 
 * ========================================================================
 * @file db.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

extern "C"
{
#include <libtaomee/inet/inet_utils.h>
}


#include "db.h"
#include "timer.h"
#include "proto.h"
#include "collect.h"
#include "metric_group.h"


using namespace taomee;

int g_db_fd = -1;
const char * g_db_ip = config_get_strval("db_ip", NULL);
const int g_db_port = config_get_intval("db_port", 0);

uint32_t g_switch_conf_len = 0; //switch配置数据包体长度
uint32_t g_switch_count = 0;    //switch个数



int connect_to_db()
{
    if (!is_legal_ip(g_db_ip) || !is_legal_port(g_db_port))
    {
        return -1;
    }

    g_db_fd = net_connect_ser(g_db_ip, g_db_port, 0);
    if (is_connected_to_db())
    {
        DEBUG_LOG("connected to db, %s:%u", g_db_ip, g_db_port);

        return 0;
    }
    else
    {
        return -1;
    }
}


int connect_to_db_timely(void * owner, void * data)
{
    if (data)
    {
        if (!is_connected_to_db())
        {
            if (0 != connect_to_db())
            {
                ADD_TIMER_EVENT(&g_event, connect_to_db_timely, reinterpret_cast<void *>(1), get_now_tv()->tv_sec + CONNECT_TO_SERVER_INTERVAL);
            }
        }
    }
    else
    {
        ADD_TIMER_EVENT(&g_event, connect_to_db_timely, reinterpret_cast<void *>(1), get_now_tv()->tv_sec + CONNECT_TO_SERVER_INTERVAL);
    }
    return 0;
}

void init_connect_to_db()
{
    if (0 != connect_to_db())
    {
        connect_to_db_timely(0, 0);
    }
}

bool is_db_fd(int fd)
{
    return ((fd >= 0) && (g_db_fd == fd));
}

bool is_connected_to_db()
{
    return (-1 != g_db_fd);
}


void close_db_fd()
{
    g_db_fd = -1;
}


int send_to_db(const void * buf, uint32_t len)
{
    if (!is_connected_to_db())
    {
        if (0 != connect_to_db())
        {
            ERROR_LOG("connect to db failed");
            return -1;
        }
    }

    const db_proto_t * pkg = reinterpret_cast<const db_proto_t *>(buf);

    DEBUG_LOG("send to db, cmd: %u, fd: %d", pkg->cmd, g_db_fd);

    return net_send_ser_ex(g_db_fd, buf, len);
}


int send_to_db(uint16_t cmd, Cmessage * p_out)
{
    if (!is_connected_to_db())
    {
        if (0 != connect_to_db())
        {
            ERROR_LOG("connect to db failed");
            return -1;
        }
    }

    db_proto_t head;

    uint32_t id = 0;
    uint32_t seq = 0;

    init_db_proto_header(&head, sizeof(db_proto_t), seq, cmd, id);
    
    DEBUG_LOG("send cli to db, cmd = %u, fd = %d", cmd, g_db_fd);

    return net_send_ser_msg(g_db_fd, &head, p_out);
}





int dispatch_db(int fd, const char * buf, uint32_t len)
{
    const db_proto_t * pkg = reinterpret_cast<const db_proto_t *>(buf);

    uint16_t cmd = pkg->cmd;
    uint32_t uid = pkg->id;
    uint32_t seq = pkg->seq;
    // uint32_t node_ip = seq;


    TRACE_LOG("dispatch db[%u] sender=%u, fd=%u, seq=%u, len=%u",
            cmd, uid, fd, seq, len);

    // c_node * p_node = find_node_by_ip(node_ip);
    // if (NULL == p_node || p_node->m_node_id != uid)
    // {
        // ERROR_LOG("miss node, ip: %s, id: %u, pkg->id: %u",
                // long2ip(node_ip), 
                // p_node ? p_node->m_node_id : 0,
                // uid);
        // return -1;
    // }
    const cmd_proto_t * p_cmd = find_db_cmd(cmd);
    if (NULL == p_cmd)
    {
        ERROR_LOG("cmdid not existed: %u", cmd);
        return 0;
    }


    uint32_t body_len = len - sizeof(db_proto_t);

    p_cmd->p_out->init();
    bool read_ret = p_cmd->p_out->read_from_buf_ex(pkg->body, body_len);
    if (!read_ret) 
    {
        ERROR_LOG("read_from_buf_ex error cmd=%u, u=%u", cmd, uid);
        return -1;
    }

    int cmd_ret = p_cmd->func(fd, p_cmd->p_in, p_cmd->p_out, body_len);
    if (DB_SUCC == cmd_ret)
    {

    }

    return 0;
}


int db_get_work_switch_config()
{
    uint32_t work_id = get_work_id();
    uint32_t work_num = get_work_num();
    db_p_get_node_switch_config_in in;
    in.work_id = work_id;
    //in.work_num = work_num;
    in.work_num = 0;

    return send_to_db(db_p_get_node_switch_config_cmd, &in);
}


int db_get_node_switch_config_timely(void * owner, void * param)
{
    db_get_work_switch_config();

    ADD_TIMER_EVENT(&g_event, db_get_node_switch_config_timely, NULL,
            get_now_tv()->tv_sec + GET_NODE_SERVER_CONFIG_INTERVAL);

    return 0;
}

void init_time_event()
{
    db_get_node_switch_config_timely(NULL, NULL);
}


int db_p_get_node_switch_config_cb(DEFAULT_ARG)
{
    db_p_get_node_switch_config_out * p_out = P_OUT;
    // group info为空则表示没有采集配置
    if (p_out->switch_confs.empty())
    {
        // 没有配置的要去定时拉取配置
        //ADD_TIMER_EVENT(&g_event, db_get_node_switch_config_timely, NULL,
        //        get_now_tv()->tv_sec + GET_NODE_SERVER_CONFIG_INTERVAL);
        return 0;
    }

    //简单判断: 交换机个数且包体长度相同则认为配置相同(大部分情况OK)
    uint32_t switch_count = p_out->switch_confs.size();
    if (switch_count == g_switch_count && out_len == g_switch_conf_len )
    {
        DEBUG_LOG("no change of switch configuration");
        return 0;
    }
    g_switch_conf_len = out_len;
    g_switch_count = switch_count;

    fini_collect();
    fini_metric_group();
    fini_metric_timer();

    do
    {
        if (0 != init_switch_metric_group(c_out))
        {
            ERROR_LOG("init_switch_metric_group() failed.");
            break;
        }
        if (0 != init_switch_collect_so(c_out))
        {
            ERROR_LOG("init_switch_collect_so() failed.");
            break;
        }
        if (0 != init_metric_group_collect_timer())
        {
            ERROR_LOG("init_metric_group_collect_timer() failed.");
            break;
        }

        return 0;

    } while (false);

    fini_collect();
    fini_metric_group();
    fini_metric_timer();

    return -1;
}

//======================================多余部分======================================
int switch_p_get_metric_data(DEFAULT_ARG)
{
    return 0;
}

int switch_p_get_version(DEFAULT_ARG)
{
    return 0;
}

int head_p_metric_data_cb(DEFAULT_ARG)
{
    return 0;
}

