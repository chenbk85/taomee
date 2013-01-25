/** 
 * ========================================================================
 * @file allow_ip.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-08-23
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <string.h>
#include "itl_common.h"


bool g_enabled = false;
std::set< uint32_t > g_allow_ip_set;

int init_allow_ip(const char * allow_client_ip)
{
    if (strncmp(allow_client_ip, "*", 1) == 0)
    {
        return 0;
    }
    uint32_t str_len = strlen(allow_client_ip);

    char * buf = (char *)malloc(str_len + 1);
    if (NULL == buf)
    {
        return -1;
    }

    memcpy(buf, allow_client_ip, str_len);
    buf[str_len] = 0;


    char * p = buf;
    char * save_ptr = NULL;
    char * token = NULL;
    while (NULL != (token = strtok_r(p, ",", &save_ptr)))
    {
        p = NULL;

        uint32_t ip = ip2long(token);

        g_allow_ip_set.insert(ip);
        DEBUG_LOG("allow ip: %s", token);
    }

    free(buf);

    // bind_ip默认是允许的
    g_allow_ip_set.insert(ip2long(config_get_strval("bind_ip", NULL)));
    g_enabled = true;

    return 0;
}


int fini_allow_ip()
{
    g_allow_ip_set.clear();
    return 0;
}



bool is_allowed_ip(const char * p_ip)
{
    uint32_t ip = ip2long(p_ip);
    return is_allowed_ip(ip);
}

bool is_allowed_ip(uint32_t ip)
{
    if (!g_enabled)
    {
        return true;

    }

    iterator_t(g_allow_ip_set) it = g_allow_ip_set.find(ip);
    if (it == g_allow_ip_set.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}
