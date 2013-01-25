/** 
 * ========================================================================
 * @file node.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-12
 * Modify $Date: 2012-11-28 11:33:47 +0800 (三, 28 11月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_NODE_H_2012_07_12
#define H_NODE_H_2012_07_12

extern "C"
{

#include <libtaomee/list.h>
#include <glib.h>
}
#include "itl_common.h"
#include "itl_timer.h"
#include "metric.h"


extern const char * g_default_rtx;
extern const char * g_default_email;
extern const char * g_default_mobile;

class c_node
{
    public:


        // 根据已发告警次数，获取发送间隔
        uint32_t get_span(uint32_t alarmed_count);
        bool is_ban(uint32_t time = 0);
        uint32_t get_server_alarm_way() const
        {
            // if (m_server_alarm_way & ALARM_WAY_MOBILE)
            // {
                // if (is_mobile_ban())
                // {
                    // // 需要把短信告警屏蔽
                    // return (m_server_alarm_way ^ ALARM_WAY_MOBILE);
                // }
            // }

            return m_server_alarm_way;
        }

    public:

        uint32_t m_node_id;
        int m_head_fd;
        uint32_t m_node_ip;
        // node所属的项目
        uint32_t m_project_id;
        char m_project_name[PROJECT_NAME_LEN];


    public:

        // node的告警配置

        uint32_t m_ban_start_time;
        uint32_t m_ban_end_time;

        // 已发告警的次数
        uint32_t m_alarm_count;
        uint32_t m_last_alarm_time;
        uint32_t m_alarm_start_time;
        uint32_t m_down_count;
        uint32_t m_up_count;

        uint32_t m_down_threshold;
        uint32_t m_up_threshold;

        // 告警发送节奏
        std::map< uint32_t, uint32_t > * m_span_map;


        // 告警联系方式和联系人
        // 服务器的
        uint32_t m_server_alarm_way;

        char m_server_mobile[CONTACT_LIST_LEN];
        char m_server_email[CONTACT_LIST_LEN];
        char m_server_rtx[CONTACT_LIST_LEN];

        // 数据库的
        uint32_t m_mysql_alarm_way;

        char m_mysql_mobile[CONTACT_LIST_LEN];
        char m_mysql_email[CONTACT_LIST_LEN];
        char m_mysql_rtx[CONTACT_LIST_LEN];

    public:

        uint16_t m_waitcmd;
        list_head_t m_waitcmd_hook;
        GQueue * m_pkg_queue;

        inline void clear_waitcmd()
        {
            m_waitcmd = 0;
        }

    public:

        timer_head_t m_event;

    public:

        // 检查host down的定时器
        timer_struct_t * m_host_timer;
        // 检查node down的定时器
        timer_struct_t * m_node_timer;

        // 主机
        bool m_host_alive;
        bool m_node_alive;
        // itl_node 是否在运行
        // 一旦收到metric数据，则表示node正在运行
        // 每次检查host时，如果node不在运行则ping
        bool m_node_running;

    public:

        bool m_inited;
        c_object_container< uint32_t, c_metric > * m_metric;
};


#define node_mgr c_single_container(uint32_t, c_node)
// typedef singleton_default< c_object_container< uint32_t, c_node > > node_mgr;


int init_nodes();


int fini_nodes();


int init_check_host_timer(c_node * p_node);

c_node * alloc_node(uint32_t node_id, int fd);


void dealloc_node(c_node * p_node);


c_node * find_node(uint32_t node_id);


const char * get_node_url(c_node * p_node, uint32_t type);

int init_server_node(c_node * p_node, Cmessage * c_out);

int init_mysql_node(c_node * p_node, Cmessage * c_out);



#endif
