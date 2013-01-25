/** 
 * ========================================================================
 * @file alarm.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-04
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

extern "C"
{
#include <libtaomee/timer.h>
}
#include "alarm.h"
#include "db.h"
#include "proto.h"
#include "alarm_interface.h"
#include "itl_common.h"


#define INSTANCE_METRIC_ID  59

char g_msg_buf[POST_DATA_LEN] = {0};

int send_host_alarm_timely(void * owner, void * param);
int handle_mysql_collect_fail(c_metric_alarm * p_alarm);


int alarm_p_get_version(DEFAULT_ARG)
{
    p->clear_waitcmd();
    return 0;
}

int alarm_p_change_node_alarm_config(DEFAULT_ARG)
{
    
    c_node * p_node = p;
    DEBUG_LOG("change alarm config node[%u: %s]", p_node->m_node_id, long2ip(p_node->m_node_ip));
    dealloc_node(p);
    // return db_get_node_alarm_config(p);
    return 0;
}

int alarm_p_metric_data(DEFAULT_ARG)
{
    alarm_p_metric_data_in * p_in = P_IN;

    c_node * p_node = p;
    p->m_node_ip = p_in->node_ip;
    // uint32_t node_id = p_in->node_id;
    uint32_t metric_id = p_in->metric_id;
    // const char * metric_name = p_in->metric_name;
    // uint32_t collect_time = p_in->collect_time;
    // uint32_t value_type = p_in->value_type;


    c_metric * p_metric = p->m_metric->get_object(metric_id);
    if (NULL == p_metric)
    {
        WARN_LOG("no alarm config for node[%u: %s] metric[%u: %s]", 
                p->m_node_id, long2ip(p_node->m_node_ip),
                metric_id, p_in->metric_name);
        p_metric = new c_metric(p);
        p_metric->m_metric_id = metric_id;
        p->m_metric->insert_object(p_metric->m_metric_id, p_metric);
    }

    STRNCPY(p_metric->m_metric_name, p_in->metric_name, MAX_METRIC_NAME_LEN);
    p_metric->m_metric_class = p_in->metric_class;
    STRNCPY(p_metric->m_metric_unit, p_in->metric_unit, MAX_METRIC_UNIT_LEN);
    STRNCPY(p_metric->m_metric_fmt, p_in->metric_fmt, MAX_METRIC_FMT_LEN);
    p_metric->m_collect_time = p_in->collect_time;
    p_metric->set_value_type(p_in->value_type);

    char * p_arg = p_in->metric_arg;


    c_metric_alarm * p_alarm = find_metric_alarm(p_metric->m_alarm, p_arg);
    if (NULL == p_alarm)
    {
        WARN_LOG("no alarm config for metric[%u: %s](%s)", 
                p_metric->m_metric_id, 
                p_metric->m_metric_name, 
                p_arg);
        p_alarm = alloc_metric_alarm(p_metric, p_arg);
        if (NULL == p_alarm)
        {
            p->clear_waitcmd();
            return 0;
        }
    }

    int index = 0;
    p_alarm->m_value.unpack(&p_in->_value_len, index);
    p_node->m_node_running = true;
    DEBUG_LOG("node[%u: %s], metric[%u: %s](%s), collect: %s, class: %u, value type: %u, value: %s", 
            p_node->m_node_id,
            long2ip(p_node->m_node_ip),
            p_metric->m_metric_id,
            p_metric->m_metric_name,
            p_alarm->m_arg,
            timestamp2str(p_metric->m_collect_time),
            p_metric->m_metric_class,
            p_metric->m_value_type,
            p_alarm->m_value.get_string(p_metric->m_metric_fmt));

    if (p_in->fail_flag)
    {
        if (2 == p_alarm->m_fail || 0 == p_alarm->m_fail)
        {
            // 上一次采集正常或者从未发过采集失败的告警
            // 需要发采集失败的告警
            p_alarm->m_fail = 1;
            if (p_alarm->is_alarm_off())
            {
                // 屏蔽告警阶段，不发
                WARN_LOG("collect fail alarm off, node[%u: %s], metric[%u: %s](%s), value: %s", 
                        p_node->m_node_id, 
                        long2ip(p_node->m_node_ip),
                        p_metric->m_metric_id,
                        p_metric->m_metric_name,
                        p_alarm->m_arg,
                        p_alarm->m_value.get_string(p_metric->m_metric_fmt));

            }
            // else if (0 == strncmp(p_node->m_project_name, "Idle", PROJECT_NAME_LEN))
            // {
                // // 空闲的机器不发采集失败告警
            // }
            else if (ITL_TYPE_MYSQL == p_metric->m_metric_class)
            {
                handle_mysql_collect_fail(p_alarm);
            }
            else
            {
                send_collect_fail_alarm(p_alarm);
            }


        }
        else
        {
            // 采集失败不重复告警
        }
    }
    else
    {
        if (1 == p_alarm->m_fail)
        {
            // 上一次发了采集失败的告警
            // 需要发采集成功的告警
            p_alarm->m_fail = 2;
            if (p_alarm->is_alarm_off())
            {
                // 屏蔽告警阶段，不发
                WARN_LOG("collect recovery alarm off, node[%u: %s], metric[%u: %s](%s), value: %s", 
                        p_node->m_node_id, 
                        long2ip(p_node->m_node_ip),
                        p_metric->m_metric_id,
                        p_metric->m_metric_name,
                        p_alarm->m_arg,
                        p_alarm->m_value.get_string(p_metric->m_metric_fmt));

            }
            // else if (0 == strncmp(p_node->m_project_name, "Idle", PROJECT_NAME_LEN))
            // {
            // }
            else if (ITL_TYPE_MYSQL == p_metric->m_metric_class)
            {

            }
            else
            {
                send_collect_fail_alarm(p_alarm);
            }


        }
        else
        {
            // 0->2
            // 2->2
            p_alarm->m_fail = 2;
        }


        handle_alarm(p_alarm);
    }





    p->clear_waitcmd();
    return 0;
}


int alarm_p_node_up(DEFAULT_ARG)
{
    return db_get_node_alarm_config(p);
}

int check_itl_node_running(void * owner, void * param)
{
    c_node * p_node = reinterpret_cast< c_node * >(param);
    if (!p_node->m_node_running)
    {
        // 还是没有在运行，则判定为挂掉
        p_node->m_node_alive = false;
        send_node_alarm(p_node);
    }
    return 0;
}

int alarm_p_node_down(DEFAULT_ARG)
{
    DEBUG_LOG("node DOWN, id: %u", p->m_node_id);

    p->m_node_running = false;
    static uint32_t check_interval = config_get_intval("check_node_after_down", 15);
    p->m_node_timer = ADD_TIMER_EVENT(&p->m_event, check_itl_node_running, p, get_now_tv()->tv_sec + check_interval);

    p->clear_waitcmd();
    return 0;
}


int alarm_p_get_relative_metric(DEFAULT_ARG)
{
    p->clear_waitcmd();
    return 0;
}

int handle_head_down(int fd)
{
    iterator_t(node_mgr) it = node_mgr.begin();
    while (it != node_mgr.end())
    {

        c_node * p_node = it->second;
        if (fd == p_node->m_head_fd)
        {
            node_mgr.next(it);
            dealloc_node(p_node);
        }
        else
        {
            node_mgr.next(it);
        }
    }

    return 0;
}



int handle_mysql_collect_fail(c_metric_alarm * p_alarm)
{
    c_node * p_node = p_alarm->m_node;
    c_metric * p_metric = p_alarm->m_metric;
    ERROR_LOG("handle mysql collect fail, node[%u: %s], metric[%u: %s](%s)", 
            p_node->m_node_id, long2ip(p_node->m_node_ip),
            p_metric->m_metric_id, p_metric->m_metric_name, p_alarm->m_arg);

    if (p_alarm->m_metric->m_metric_id == INSTANCE_METRIC_ID)
    {
        ERROR_LOG("instance alive collect failed, node[%u: %s], SEND", 
                p_node->m_node_id, long2ip(p_node->m_node_ip));
        send_collect_fail_alarm(p_alarm);
        return 0;
    }

    c_metric * p_instance_metric = p_node->m_metric->get_object(INSTANCE_METRIC_ID);
    if (NULL == p_instance_metric)
    {
        ERROR_LOG("no instance alive metric found, node[%u: %s], metric[%u: %s], SKIP", 
                p_node->m_node_id, long2ip(p_node->m_node_ip),
                p_metric->m_metric_id, p_metric->m_metric_name);
        p_alarm->m_fail = 0;
        return 0;
    }

    c_metric_alarm * p_instance_alarm = find_metric_alarm(p_instance_metric->m_alarm, p_alarm->m_arg);
    if (NULL == p_instance_alarm)
    {
        ERROR_LOG("no instance alive metric alarm found, node[%u: %s], metric[%u: %s](%s), SKIP", 
                p_node->m_node_id, long2ip(p_node->m_node_ip),
                p_metric->m_metric_id, p_metric->m_metric_name, p_alarm->m_arg);
        p_alarm->m_fail = 0;
        return 0;
    }

    if (0 == p_instance_alarm->m_fail)
    {
        // instance alive的结果未知，不发数据库采集项的采集失败告警
        ERROR_LOG("instance alive collect fail = 0, node[%u: %s], SKIP", 
                p_node->m_node_id, long2ip(p_node->m_node_ip));
        p_alarm->m_fail = 0;
        return 0;
    }
    else if (1 == p_instance_alarm->m_fail)
    {
        // instance alive采集失败，也不发
        ERROR_LOG("instance alive collect fail = 1, node[%u: %s], SKIP", 
                p_node->m_node_id, long2ip(p_node->m_node_ip));
        p_alarm->m_fail = 0;
        return 0;
    }
    else if (2 == p_instance_alarm->m_fail)
    {
        // instance alive 采集成功
        double v = get_metric_value(p_instance_alarm);
        if (compare_value(v, 2.0f, OP_EQ))
        {
            // instance alive值为2，表示连接数据库失败
            // 不发告警
            ERROR_LOG("instance alive FALSE, node[%u: %s], SKIP", 
                    p_node->m_node_id, long2ip(p_node->m_node_ip));
            p_alarm->m_fail = 0;
        }
        else
        {
            ERROR_LOG("instance alive TRUE, node[%u: %s], SEND", 
                    p_node->m_node_id, long2ip(p_node->m_node_ip));
            // 连接数据库成功的情况下采集失败，要发告警
            send_collect_fail_alarm(p_alarm);
        }
        return 0;
    }

    return 0;
}



int handle_alarm(c_metric_alarm * p_alarm)
{
    if (!p_alarm->m_has_alarm_config)
    {
        return 0;
    }

    if (p_alarm->is_ban())
    {
        return 0;
    }

    if (p_alarm->is_alarm_off())
    {
        return 0;
    }


    // DEBUG_LOG("hanlde_alarm: ip[%u]",p_node->m_node_ip);


    uint32_t res = compare_metric_value(p_alarm);

    if (p_alarm->m_normal)
    {
        if (ALARM_NORMAL == res)
        {
            // 连续正常的次数累加1
            p_alarm->m_normal_count++;
            p_alarm->m_abnormal_count = 0;
            // do nothing
        }
        else
        {
            // 连续不正常的次数累加1
            p_alarm->m_abnormal_count++;
            // 连续正常的次数清零
            p_alarm->m_normal_count = 0;

            if (p_alarm->m_abnormal_count >= p_alarm->m_abnormal_threshold)
            {
                // 切换到不正常状态
                p_alarm->m_normal = false;
                p_alarm->m_alarm_level = res;

                // 要发告警
                uint32_t now = get_now_tv()->tv_sec;
                p_alarm->m_alarm_start_time = now;
                p_alarm->m_last_alarm_time = now;
                p_alarm->m_alarm_count++;
                send_metric_alarm(p_alarm);


            }
            else
            {
                // 没达到告警需要的次数
                // do nothing
            }
        }

    }
    else
    {
        if (ALARM_NORMAL == res)
        {
            // 连续不正常的次数清零
            p_alarm->m_abnormal_count = 0;
            // 连续正常的次数累加1
            p_alarm->m_normal_count++;
            if (p_alarm->m_normal_count >= p_alarm->m_normal_threshold)
            {
                // 切换到正常状态
                p_alarm->m_normal = true;
                // 发恢复
                send_metric_recovery_alarm(p_alarm);
                p_alarm->m_alarm_count = 0;
                p_alarm->m_last_alarm_time = 0;
                p_alarm->m_alarm_start_time = 0;
            }
            else
            {
                // 没达到恢复需要的次数
                // do nothing
            }
        }
        else
        {
            // 连续不正常的次数累加1
            p_alarm->m_abnormal_count++;
            p_alarm->m_normal_count = 0;

            p_alarm->m_alarm_level |= res;
            uint32_t span = p_alarm->get_span(p_alarm->m_alarm_count);
            uint32_t now = get_now_tv()->tv_sec;
            DEBUG_LOG("span: %u, last: %u, now: %u", span, p_alarm->m_last_alarm_time, now);
            if (now >= p_alarm->m_last_alarm_time + span)
            {
                p_alarm->m_last_alarm_time = now;
                p_alarm->m_alarm_count++;
                send_metric_alarm(p_alarm);
            }

        }
    }





    return 0;

}


int send_collect_fail_alarm(c_metric_alarm * p_alarm)
{
    c_node * p_node = p_alarm->m_node;
    c_metric * p_metric = p_alarm->m_metric;
    // 跳过年份
    const char * ts = timestamp2str(get_now_tv()->tv_sec) + 5;

    if (2 == p_alarm->m_fail)
    {
        // 采集恢复的
        INFO_LOG("send metric COLLECT RECOVERY alarm, node[%u: %s], metric[%u: %s](%s)", 
                p_node->m_node_id,
                long2ip(p_node->m_node_ip), 
                p_metric->m_metric_id,
                p_metric->m_metric_name,
                p_alarm->m_arg);

        snprintf(g_msg_buf, sizeof(g_msg_buf),
                "[%s(%s)|%s] %s(%s) 采集恢复 %s",
                long2ip(p_node->m_node_ip),
                p_node->m_project_name,
                get_node_url(p_node, ITL_TYPE_SERVER),
                p_metric->m_metric_name,
                p_alarm->m_arg,
                ts);

        // if (p_alarm->get_warn_way() & ALARM_WAY_MOBILE)
        // {
        // send_mobile_alarm(p_alarm->m_mobile_contact, g_msg_buf);
        // }

        // if (p_alarm->get_warn_way() & ALARM_WAY_EMAIL)
        // {
        // send_email_alarm(p_alarm->m_email_contact, g_msg_buf);
        // }

        // if (p_alarm->get_warn_way() & ALARM_WAY_RTX)
        // {
        // send_rtx_alarm(p_alarm->m_rtx_contact, g_msg_buf);
        // }

        send_rtx_alarm(p_alarm->m_rtx_contact, g_msg_buf);

    }
    else
    {
        // 采集失败的
        INFO_LOG("send metric COLLECT FAIL alarm, node[%u: %s], metric[%u: %s](%s)", 
                p_node->m_node_id,
                long2ip(p_node->m_node_ip), 
                p_metric->m_metric_id,
                p_metric->m_metric_name,
                p_alarm->m_arg);

        snprintf(g_msg_buf, sizeof(g_msg_buf),
                "[%s(%s)|%s] %s(%s) 采集失败 %s",
                long2ip(p_node->m_node_ip),
                p_node->m_project_name,
                get_node_url(p_node, ITL_TYPE_SERVER),
                p_metric->m_metric_name,
                p_alarm->m_arg,
                ts);

        // if (p_alarm->get_warn_way() & ALARM_WAY_MOBILE)
        // {
        // send_mobile_alarm(p_alarm->m_mobile_contact, g_msg_buf);
        // }

        // if (p_alarm->get_warn_way() & ALARM_WAY_EMAIL)
        // {
        // send_email_alarm(p_alarm->m_email_contact, g_msg_buf);
        // }

        // if (p_alarm->get_warn_way() & ALARM_WAY_RTX)
        // {
        // send_rtx_alarm(p_alarm->m_rtx_contact, g_msg_buf);
        // }


        send_rtx_alarm(p_alarm->m_rtx_contact, g_msg_buf);
    }

    return 0;
}


int send_metric_recovery_alarm(const c_metric_alarm * p_alarm)
{

    c_node * p_node = p_alarm->m_node;
    c_metric * p_metric = p_alarm->m_metric;
    char node_ip[IP_STRING_LEN] = {0};
    STRNCPY(node_ip, long2ip(p_node->m_node_ip), IP_STRING_LEN);

    // 跳过年份
    const char * ts = timestamp2str(get_now_tv()->tv_sec) + 5;


    INFO_LOG("send metric RECOVERY alarm, node[%u: %s], metric[%u: %s](%s)", 
            p_node->m_node_id,
            node_ip, 
            p_metric->m_metric_id, 
            p_metric->m_metric_name,
            p_alarm->m_arg);

    snprintf(g_msg_buf, sizeof(g_msg_buf),
            "%s(%s) %s(%s) recovery %s",
            node_ip, 
            p_node->m_project_name,
            p_metric->m_metric_name,
            p_alarm->m_arg,
            ts);

    // 如果既发过warn也发过critical，则先通知收到critical的联系人
    // 再通知收到warn但没有收到critical的联系人
    uint32_t way = 0;
    if (p_alarm->m_alarm_level & ALARM_CRITICAL)
    {
        way = p_alarm->get_crit_way();
        if (way & ALARM_WAY_MOBILE)
        {
            send_mobile_alarm(p_alarm->m_mobile_contact, g_msg_buf);
        }

        if (way & ALARM_WAY_EMAIL)
        {
            send_email_alarm(p_alarm->m_email_contact, g_msg_buf);
        }

        if (way & ALARM_WAY_RTX)
        {
            send_rtx_alarm(p_alarm->m_rtx_contact, g_msg_buf);
        }


        // critical中通知过的，就不用再在warn中通知了
        way = p_alarm->get_warn_way() ^ way;
    }
    else
    {
        way = p_alarm->get_warn_way();
    }


    if (p_alarm->m_alarm_level & ALARM_WARN)
    {
        if (way & ALARM_WAY_MOBILE)
        {
            send_mobile_alarm(p_alarm->m_mobile_contact, g_msg_buf);
        }

        if (way & ALARM_WAY_EMAIL)
        {
            send_email_alarm(p_alarm->m_email_contact, g_msg_buf);
        }

        if (way & ALARM_WAY_RTX)
        {
            send_rtx_alarm(p_alarm->m_rtx_contact, g_msg_buf);
        }
    }



    db_store_alarm_message_data(p_alarm, g_msg_buf);
    db_store_alarm_event_data(p_alarm, g_msg_buf);

    return 0;
}


int send_metric_alarm(const c_metric_alarm * p_alarm)
{

    c_node * p_node = p_alarm->m_node;
    c_metric * p_metric = p_alarm->m_metric;
    char node_ip[IP_STRING_LEN] = {0};
    STRNCPY(node_ip, long2ip(p_node->m_node_ip), IP_STRING_LEN);

    const char * flag = NULL;
    uint32_t way = 0;
    double threshold = 0;

    if (ALARM_CRITICAL & p_alarm->m_alarm_level)
    {
        // 严重告警
        flag = "CRIT";
        way = p_alarm->get_crit_way();
        threshold = get_threshold(p_alarm->m_crit_val_coeff, p_alarm->m_crit_alarm);
    }
    else if (ALARM_WARN & p_alarm->m_alarm_level)
    {
        // 告警
        flag = "WARN";
        way = p_alarm->get_warn_way();
        threshold = get_threshold(p_alarm->m_warn_val_coeff, p_alarm->m_warn_alarm);
    }
    else
    {
        return 0;
    }

    // 跳过年份
    const char * ts = timestamp2str(get_now_tv()->tv_sec) + 5;


    const char * op_str = op2str(p_alarm->m_op);

    double v = get_metric_value(p_alarm);
    char value_str[MAX_METRIC_UNIT_LEN + 10] = {0};
    compact_value_unit(v, p_metric->m_metric_unit, value_str, sizeof(value_str));

    char threshold_str[MAX_METRIC_UNIT_LEN + 10] = {0};
    compact_value_unit(threshold, p_metric->m_metric_unit, threshold_str, sizeof(threshold_str));


    INFO_LOG("send metric %s alarm No.%u, node[%u: %s], metric[%u: %s](%s), value: %s %s %s", 
            flag,
            p_alarm->m_alarm_count,
            p_node->m_node_id,
            node_ip, 
            p_metric->m_metric_id, 
            p_metric->m_metric_name, 
            p_alarm->m_arg,
            value_str, 
            op_str, 
            threshold_str);

    if (way & ALARM_WAY_MOBILE)
    {
        char compact_ip[IP_STRING_LEN] = {0};
        if (0 == strncmp(node_ip, "192.168.", 8))
        {
            snprintf(compact_ip, sizeof(compact_ip), "%s", node_ip + 8);
        }
        else
        {
            STRNCPY(compact_ip, node_ip, IP_STRING_LEN);
        }

        char arg_section[MAX_METRIC_ARG_LEN] = {0};
        if (p_alarm->m_arg[0])
        {
            snprintf(arg_section, sizeof(arg_section), "(%s)", p_alarm->m_arg);
        }


        if (1 == p_alarm->m_alarm_count)
        {
            // 第一次发，要加上发送人员名单
            snprintf(g_msg_buf, sizeof(g_msg_buf),
                    "%c:%s(%s) %s%s C(%s) %s T(%s), 2 %s",
                    flag[0],
                    compact_ip, 
                    p_node->m_project_name,
                    p_metric->m_metric_name,
                    arg_section,
                    value_str, 
                    op_str, 
                    threshold_str, 
                    p_alarm->m_rtx_contact);
        }
        else
        {
            snprintf(g_msg_buf, sizeof(g_msg_buf),
                    "%c:%s(%s) %s%s C(%s) %s T(%s)",
                    flag[0], 
                    compact_ip, 
                    p_node->m_project_name,
                    p_metric->m_metric_name,
                    arg_section,
                    value_str, 
                    op_str, 
                    threshold_str);
        }



        send_mobile_alarm(p_alarm->m_mobile_contact, g_msg_buf);


    }

    if (way & ALARM_WAY_EMAIL)
    {
        snprintf(g_msg_buf, sizeof(g_msg_buf),
                "%s:%s(%s) %s(%s)，当前值(%s) %s 阀值(%s) %s，发送给 %s",
                flag, 
                node_ip, 
                p_node->m_project_name,
                p_metric->m_metric_name,
                p_alarm->m_arg,
                value_str, 
                op_str, 
                threshold_str, 
                ts,
                p_alarm->m_rtx_contact);

        send_email_alarm(p_alarm->m_email_contact, g_msg_buf);

    }

    if (way & ALARM_WAY_RTX)
    {
        snprintf(g_msg_buf, sizeof(g_msg_buf),
                "%s:[%s(%s)|%s] %s(%s) 当前值(%s) %s 阀值(%s) %s，发送给 %s",
                flag, 
                node_ip, 
                p_node->m_project_name,
                get_node_url(p_node, ITL_TYPE_SERVER),
                p_metric->m_metric_name,
                p_alarm->m_arg,
                value_str, 
                op_str, 
                threshold_str, 
                ts,
                p_alarm->m_rtx_contact);

        send_rtx_alarm(p_alarm->m_rtx_contact, g_msg_buf);

    }

    db_store_alarm_message_data(p_alarm, g_msg_buf);
    if (1 == p_alarm->m_alarm_count)
    {
        db_store_alarm_event_data(p_alarm, g_msg_buf);
    }


    return 0;
}


int send_node_alarm(c_node * p_node)
{
    const char * ts = timestamp2str(get_now_tv()->tv_sec) + 5;

    if (p_node->m_node_alive)
    { 
        // 恢复
        INFO_LOG("send NODE RECOVERY alarm, node id: %u, ip: %s", 
                p_node->m_node_id,
                long2ip(p_node->m_node_ip));

        snprintf(g_msg_buf, sizeof(g_msg_buf), 
                "[%s(%s)|%s] itl node 恢复 %s",
                long2ip(p_node->m_node_ip),
                p_node->m_project_name,
                get_node_url(p_node, ITL_TYPE_SERVER),
                ts);
    }
    else
    {
        // down
        INFO_LOG("send NODE DOWN alarm, node id: %u, ip: %s", 
                p_node->m_node_id,
                long2ip(p_node->m_node_ip));

        snprintf(g_msg_buf, sizeof(g_msg_buf),
                "[%s(%s)|%s] itl node 故障 %s",
                long2ip(p_node->m_node_ip),
                p_node->m_project_name,
                get_node_url(p_node, ITL_TYPE_SERVER),
                ts);
    }

    // itl node挂掉只发rtx消息到开发者
    send_rtx_alarm(config_get_strval("itl_node_contact", "smyang"), g_msg_buf);

    return 0;

}



int send_host_alarm(c_node * p_node)
{
    const char * ts = timestamp2str(get_now_tv()->tv_sec) + 5;

    if (p_node->m_host_alive)
    {
        // 恢复
        INFO_LOG("send HOST RECOVERY alarm, node id: %u, ip: %s", 
                p_node->m_node_id,
                long2ip(p_node->m_node_ip));

        snprintf(g_msg_buf, sizeof(g_msg_buf),
                "%s(%s) host recovery %s",
                long2ip(p_node->m_node_ip),
                p_node->m_project_name,
                ts);


    }
    else
    {
        // down
        INFO_LOG("send HOST DOWN alarm, node id: %u, ip: %s", 
                p_node->m_node_id,
                long2ip(p_node->m_node_ip));

        snprintf(g_msg_buf, sizeof(g_msg_buf),
                "%s(%s) host down %s",
                long2ip(p_node->m_node_ip),
                p_node->m_project_name,
                ts);
    }


    uint32_t way = p_node->get_server_alarm_way();

    if (way & ALARM_WAY_EMAIL)
    {
        send_email_alarm(p_node->m_server_email, g_msg_buf);
    }

    if (way & ALARM_WAY_RTX)
    {
        send_rtx_alarm(p_node->m_server_rtx, g_msg_buf);
        send_rtx_alarm(g_default_rtx, g_msg_buf);
    }

    if (way & ALARM_WAY_MOBILE)
    {
        if (1 == p_node->m_alarm_count)
        {
            // 第一次发，要加上发送给哪些人
            uint32_t len = strlen(g_msg_buf);
            snprintf(g_msg_buf + len, sizeof(g_msg_buf) - len, ", 2 %s", p_node->m_server_rtx);
        }

        send_mobile_alarm(p_node->m_server_mobile, g_msg_buf);
    }


    if ((!p_node->m_host_alive && 1 == p_node->m_alarm_count) || p_node->m_host_alive)
    {
        // host down只记录头尾
        db_store_alarm_event_data(p_node, g_msg_buf);
    }

    db_store_alarm_message_data(p_node, g_msg_buf);

    return 0;
}




int send_db_alarm()
{
    INFO_LOG("send db DOWN alarm");
    return 0;
}


int send_head_alarm()
{
    INFO_LOG("send head DOWN alarm");


    return 0;
}
