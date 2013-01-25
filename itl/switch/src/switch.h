/** 
 * ========================================================================
 * @file switch.h
 * @brief 
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-08-14
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_SWITCH_H_20120814
#define H_SWITCH_H_20120814

#include <stdint.h>
#include "async_server.h"
#include "define.h"
#include "itl_common.h"

#define SWITCH_TAG_MAX_LEN 32
#define SWITCH_COMMUNITY_MAX_LEN 64

extern const char *g_itl_head_ip;
extern uint16_t g_itl_head_port;

typedef struct {
    uint32_t fail_count;//采集失败次数
    time_t last_collect_time;//最后一次采集时间
} switch_cansel_collect_t;

class c_switch
{
public:
    c_switch();
    ~c_switch();
    int init(
            uint32_t switch_id,
            uint32_t switch_type,
            const char *p_switch_tag,
            const char *p_switch_ip,
            const char *p_switch_community);
    int uninit();
    int send_register_message();
    int send_to_head(const void * buf, uint32_t len);
    int send_to_head(uint16_t cmd, Cmessage * p_out);

public:
    inline bool is_connected_to_head()
    {
        return (-1 != m_switch_fd);
    }

    inline int connect_to_head()
    {
        m_switch_fd = net_connect_ser(g_itl_head_ip, g_itl_head_port, 0);
        if (-1 == m_switch_fd)
        {
            return -1;
        }
        send_register_message();

        return 0;
    }


public:
    inline uint32_t get_switch_id()
    {
        return m_switch_id;
    }

    inline uint32_t get_switch_type()
    {
        return m_switch_type;
    }

    inline const char * get_switch_tag()
    {
        return m_switch_tag;
    }

    inline uint32_t get_switch_ip_num()
    {
        return m_switch_ip_num;
    }
    inline const char * get_switch_ip_str()
    {
        return m_switch_ip_str;
    }

    inline const char * get_switch_community()
    {
        return m_switch_community;
    }

    inline int get_switch_fd()
    {
        return m_switch_fd;
    }
    inline void set_switch_fd(int fd)
    {
        m_switch_fd = fd;
    }

    inline int get_reference_count()
    {
        return m_reference_count;
    }
    inline void increase_reference_count()
    {
        ++m_reference_count;
    }

    inline int get_fail_cout()
    {
        return m_collect_fail_count;
    }
    inline void increase_fail_cout()
    {
        ++m_collect_fail_count;
    }
    inline void reset_fail_cout()
    {
        m_collect_fail_count = 0;
    }

    inline time_t get_last_collect_time()
    {
        return m_last_collect_time;
    }
    inline void set_last_collect_time(time_t collect_time)
    {
        m_last_collect_time = collect_time;
    }

private:
    bool m_inited;
    int m_reference_count;
    int m_switch_fd; /**<与head连接的fd*/

    //switch cansel collect
    int m_collect_fail_count; /**<metric采集失败次数*/
    time_t m_last_collect_time; /**<最后一次采集时间*/

    uint32_t m_switch_id;
    uint32_t m_switch_type;
    char m_switch_tag[SWITCH_TAG_MAX_LEN];
    uint32_t m_switch_ip_num;
    char m_switch_ip_str[16];
    char m_switch_community[SWITCH_COMMUNITY_MAX_LEN];
};

#define switch_mgr c_single_container(uint32_t, c_switch)

bool is_switch_head_fd(int fd);
void close_switch_head_fd(int fd);
int dispatch_switch_head(int fd, const char * buf, uint32_t len);

#endif
