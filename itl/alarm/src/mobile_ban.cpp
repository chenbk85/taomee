/** 
 * ========================================================================
 * @file mobile_ban.cpp
 * @brief 短信屏蔽
 * @author smyang
 * @version 1.0
 * @date 2012-09-05
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "mobile_ban.h"
#include "itl_common.h"
#include "xml.h"
#include <libtaomee/timer.h>



struct time_range_t
{
    time_range_t()
    {
        id = 0;
        start = 0;
        end = 0;
    }
    uint32_t id;
    uint32_t start;
    uint32_t end;
};

#define g_time_range_mgr c_single_container(uint32_t, time_range_t)

struct date_range_t
{
    date_range_t()
    {
        start = 0;
        end = 0;

        time_range_id = 0;
    }

    // 起始天0点的时间戳
    uint32_t start;
    // 结束点23:59:59的时间戳
    uint32_t end;

    // 屏蔽时间段的id
    uint32_t time_range_id;
};


c_object_container< uint32_t, date_range_t > g_deny_range_mgr;
c_object_container< uint32_t, date_range_t > g_allow_range_mgr;

#define reset_tm(tm)    memset(&(tm), 0, sizeof(tm))

time_range_t * g_default_time_range = NULL;


int parse_mobile_ban(xmlNodePtr root);


int init_mobile_ban()
{
    taomee::load_xmlconf(config_get_strval("mobile_ban", "./conf/mobile_ban.xml"), parse_mobile_ban);

    container_for_each(g_deny_range_mgr, it)
    {
        date_range_t * p_date_range = it->second;
        char buf[64] = {0};
        snprintf(buf, sizeof(buf), "%s", timestamp2str(p_date_range->start));
        DEBUG_LOG("deny date from %s to %s",
                buf, timestamp2str(p_date_range->end));

        uint32_t time_range_id = p_date_range->time_range_id;
        time_range_t * p_time_range = g_time_range_mgr.get_object(time_range_id);
        if (NULL == p_time_range)
        {
            continue;
        }
        uint32_t start = p_time_range->start;
        uint32_t end = p_time_range->end;
        DEBUG_LOG("\tfrom %02u:%02u:%02u to %02u:%02u:%02u", 
                start / 3600, (start % 3600) / 60, start % 60,
                end / 3600, (end % 3600) / 60, end % 60);
    }

    container_for_each(g_allow_range_mgr, it)
    {
        date_range_t * p_date_range = it->second;
        char buf[64] = {0};
        snprintf(buf, sizeof(buf), "%s", timestamp2str(p_date_range->start));
        DEBUG_LOG("allow date from %s to %s",
                buf, timestamp2str(p_date_range->end));

        uint32_t time_range_id = p_date_range->time_range_id;
        time_range_t * p_time_range = g_time_range_mgr.get_object(time_range_id);
        if (NULL == p_time_range)
        {
            continue;
        }
        uint32_t start = p_time_range->start;
        uint32_t end = p_time_range->end;
        DEBUG_LOG("\tfrom %02u:%02u:%02u to %02u:%02u:%02u", 
                start / 3600, (start % 3600) / 60, start % 60,
                end / 3600, (end % 3600) / 60, end % 60);
    }
    return 0;
}

int fini_mobile_ban()
{
    g_allow_range_mgr.uninit();
    g_deny_range_mgr.uninit();

    g_time_range_mgr.uninit();

    return 0;
}

int get_time_range(xmlNodePtr node, time_range_t * p_time_range)
{
    static char buf[16] = {0};
    struct tm tm;


    if (0 != get_xml_prop_string(buf, node, "start_time"))
    {
        return -1;
    }

    reset_tm(tm);
    strptime(buf, "%H:%M:%S", &tm);
    p_time_range->start = tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec;


    if (0 != get_xml_prop_string(buf, node, "end_time"))
    {
        return -1;
    }

    reset_tm(tm);
    strptime(buf, "%H:%M:%S", &tm);
    p_time_range->end = tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec;


    return 0;
}


int get_date_range(xmlNodePtr node, date_range_t * p_date_range)
{

    static char buf[32] = {0};
    struct tm tm;

    if (0 != get_xml_prop_string(buf, node, "start_date"))
    {
        return -1;
    }

    reset_tm(tm);
    strptime(buf, "%Y-%m-%d", &tm);
    p_date_range->start = mktime(&tm);



    if (0 != get_xml_prop_string(buf, node, "end_date"))
    {
        // 没有配置end_date
        // 就以start_date为end_date
    }
    else
    {
        reset_tm(tm);
        strptime(buf, "%Y-%m-%d", &tm);
    }

    tm.tm_hour = 23;
    tm.tm_min = 59;
    tm.tm_sec = 59;
    p_date_range->end = mktime(&tm);


    return 0;

}


int parse_mobile_ban(xmlNodePtr root)
{
    char buf[64] = {0};
    struct tm tm;




    xmlnode_for_each(root, node)
    {
        reset_tm(tm);

        if (is_xmlnode_name(node, "time_range"))
        {
            uint32_t time_range_id = 0;
            if (0 != get_xml_prop(time_range_id, node, "id"))
            {
                return -1;
            }

            time_range_t * p_time_range = new time_range_t;
            p_time_range->id = time_range_id;

            if (0 != get_time_range(node, p_time_range))
            {
                ERROR_LOG("mobile ban, parse time range failed");
                delete p_time_range;
                return -1;
            }

            g_time_range_mgr.insert_object(p_time_range->id, p_time_range);

            if (NULL == g_default_time_range)
            {
                uint8_t is_default = 0;
                taomee::get_xml_prop_def(is_default, node, "default", 0);
                if (is_default)
                {
                    g_default_time_range = p_time_range;
                }
            }

        }
        else if (is_xmlnode_name(node, "deny"))
        {

            date_range_t * p_date_range = new date_range_t;
            if (0 != get_date_range(node, p_date_range))
            {
                ERROR_LOG("mobile ban, parse date range failed");
                delete p_date_range;
                return -1;
            }

            if (p_date_range->start >= p_date_range->end)
            {
                ERROR_LOG("mobile ban, date range, start[%u] >= end[%u]",
                        p_date_range->start, 
                        p_date_range->end);
                delete p_date_range;
                return -1;
            }

            // 获取时间段
            if (0 == get_xml_prop(p_date_range->time_range_id, node, "time_range_id"))
            {
                // do nothing
            }
            else if (0 == get_xml_prop_string(buf, node, "start_time"))
            {
                time_range_t * p_time_range = new time_range_t;
                p_time_range->id = g_time_range_mgr.get_max_key() + 1;

                if (0 != get_time_range(node, p_time_range))
                {
                    ERROR_LOG("mobile ban, parse time range failed");
                    delete p_date_range;
                    delete p_time_range;
                    return -1;
                }

                g_time_range_mgr.insert_object(p_time_range->id, p_time_range);

                p_date_range->time_range_id = p_time_range->id;
            }
            else
            {
                if (NULL == g_default_time_range)
                {
                    ERROR_LOG("mobile ban, no default time range");
                    delete p_date_range;
                    return -1;
                }
                p_date_range->time_range_id = g_default_time_range->id;
            }

            g_deny_range_mgr.insert_object(g_deny_range_mgr.size(), p_date_range);

        }
        else if (is_xmlnode_name(node, "allow"))
        {

            date_range_t * p_date_range = new date_range_t;
            if (0 != get_date_range(node, p_date_range))
            {
                ERROR_LOG("mobile ban, parse date range failed");
                delete p_date_range;
                return -1;
            }

            if (p_date_range->start >= p_date_range->end)
            {
                ERROR_LOG("mobile ban, date range, start[%u] >= end[%u]",
                        p_date_range->start, 
                        p_date_range->end);
                delete p_date_range;
                return -1;
            }

            // 获取时间段
            if (0 == get_xml_prop(p_date_range->time_range_id, node, "time_range_id"))
            {
                // do nothing
            }
            else if (0 == get_xml_prop_string(buf, node, "start_time"))
            {
                time_range_t * p_time_range = new time_range_t;
                p_time_range->id = g_time_range_mgr.get_max_key() + 1;

                if (0 != get_time_range(node, p_time_range))
                {
                    ERROR_LOG("mobile ban, parse time range failed");
                    delete p_date_range;
                    delete p_time_range;
                    return -1;
                }

                g_time_range_mgr.insert_object(p_time_range->id, p_time_range);

                p_date_range->time_range_id = p_time_range->id;
            }
            else
            {
                p_date_range->time_range_id = g_default_time_range->id;
            }

            g_allow_range_mgr.insert_object(g_allow_range_mgr.size(), p_date_range);



        }


    }

    return 0;
}


bool is_mobile_ban(uint32_t time)
{
    if (0 == time)
    {
        time = get_now_tv()->tv_sec;
    }


    container_for_each(g_deny_range_mgr, it)
    {
        date_range_t * p_date_range = it->second;
        if (time < p_date_range->start || p_date_range->end < time)
        {
            continue;
        }

        time_range_t * p_time_range = g_time_range_mgr.get_object(p_date_range->time_range_id);
        if (NULL == p_time_range)
        {
            continue;
        }

        uint32_t day_sec = time - get_day_timestamp(time);
        if (p_time_range->start <= day_sec && day_sec <= p_time_range->end)
        {
            return true;
        }

    }


    container_for_each(g_allow_range_mgr, it)
    {
        date_range_t * p_date_range = it->second;
        if (time < p_date_range->start || p_date_range->end < time)
        {
            continue;
        }

        time_range_t * p_time_range = g_time_range_mgr.get_object(p_date_range->time_range_id);
        if (NULL == p_time_range)
        {
            continue;
        }

        uint32_t day_sec = time - get_day_timestamp(time);
        if (p_time_range->start <= day_sec && day_sec <= p_time_range->end)
        {
            return false;
        }

    }

    // 默认周一至周五的默认时间段要屏蔽
    time_t tmp_timestamp(time);
    struct tm tm;
    if (NULL == localtime_r(&tmp_timestamp, &tm))
    {
        return false;
    }


    switch (tm.tm_wday)
    {
        // 周六
        case 6:
        // 周日
        case 0:
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            {
                uint32_t day_sec = time - get_day_timestamp(time);
                if (g_default_time_range->start <= day_sec && day_sec <= g_default_time_range->end)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        default:
            break;

    }

    return false;
}
