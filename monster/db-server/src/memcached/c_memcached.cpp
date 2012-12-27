/**
 * ========================================================================
 * @file c_memcached.cpp
 * @brief
 * @author smyang（杨思敏），smyang@taomee.com
 * @version 1.0
 * @date 2011-04-18
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */
extern "C"
{
#include <libtaomee/log.h>
}

#include "../../../common/bswap.h"
#include "c_memcached.h"

c_memcached::c_memcached(): m_inited(0), m_p_memcached(NULL)
{
}

c_memcached::~c_memcached()
{
    uninit();
}

int c_memcached::init(const char *server_addr, int server_port)
{
    if (NULL == server_addr)
    {
        ERROR_LOG("memcached server addr cann't be null");
        return -1;
    }

    if (m_inited)
    {
        ERROR_LOG("memcached inited failed");
        return -1;
    }

    m_p_memcached = memcached_create(NULL);
    if (NULL == m_p_memcached)
    {
        ERROR_LOG("create memcached failed");
        return -1;
    }

    m_ret = memcached_server_add(m_p_memcached, server_addr, server_port);
    if (m_ret != MEMCACHED_SUCCESS)
    {
        ERROR_LOG("add memcached server failed(%s)", memcached_strerror(m_p_memcached, m_ret));
        memcached_free(m_p_memcached);
        m_p_memcached = NULL;
        return -1;
    }
    DEBUG_LOG("memcache add:[%s:%d]", server_addr, server_port);

    //使用二进制协议传输，支持memcached_initial等协议需要,1,4以上的版本才支持这个特性
    //m_ret = memcached_behavior_set(m_p_memcached, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1);
    //if (m_ret != MEMCACHED_SUCCESS)
    //{
    //   ERROR_LOG("set memcached server failed(%s)", memcached_strerror(m_p_memcached, m_ret));
    //   memcached_free(m_p_memcached);
    //   m_p_memcached = NULL;
    //   return -1;
    //}

    DEBUG_LOG("memcache init success");
    m_inited = 1;

    return 0;
}

int c_memcached::uninit(uint8_t flush_flag)
{
    if (!m_inited)
    {
        ERROR_LOG("memcached is already uninit");
        return -1;
    }

    if (flush_flag)
    {
        memcached_flush(m_p_memcached, 0);
    }

    if (m_p_memcached != NULL)
    {
        memcached_free(m_p_memcached);
        m_p_memcached = NULL;
    }

    m_inited = 0;

    return 0;
}

int c_memcached::mem_get(const char *key, void *p_data, const uint32_t buffer_len, uint32_t *flags)
{
#ifdef ENABLE_MEMCACHE
    size_t value_len = 0;
    uint32_t flag = 0;
    char *p_buffer = memcached_get(m_p_memcached, key, strlen(key), &value_len, &flag, &m_ret);
    if (MEMCACHED_SUCCESS == m_ret && value_len > 0)
    {
        // 要求长度必须为buffer_len
        if (buffer_len == value_len)
        {
            memcpy(p_data, p_buffer, buffer_len);
            if (NULL != flags)
            {
                *flags = flag;
            }
            free(p_buffer);
            return 0;
        }
        else
        {
            free(p_buffer);
            return -1;
        }
    }
    else
    {
        return -1;
    }
#else
    return -1;
#endif
}

int c_memcached::mem_get(const char *key, void *p_data, uint32_t *buffer_len, uint32_t *flags)
{
    size_t value_len = 0;
    uint32_t flag = 0;
    char *p_buffer = memcached_get(m_p_memcached, key, strlen(key), &value_len, &flag, &m_ret);
    if (MEMCACHED_SUCCESS == m_ret && value_len > 0)
    {
        *buffer_len = value_len;
        memcpy(p_data, p_buffer, value_len);
        if (NULL != flags)
        {
            *flags = flag;
        }
        free(p_buffer);
        return 0;
    }
    else
    {
        return -1;
    }
}

int c_memcached::mem_set(const char *key, const void *value, const uint32_t value_len, time_t expiration, uint32_t flags)
{
    m_ret = memcached_set(m_p_memcached, key, strlen(key), (const char *)value, (size_t)value_len, expiration, flags);
    if (m_ret != MEMCACHED_SUCCESS)
    {
	    //CRIT_LOG("memcached set failed, reason:%s", memcached_strerror(m_p_memcached, m_ret));
        return -1;
    }

    return 0;
}

int c_memcached::mem_del(const char *key)
{
    m_ret = memcached_delete(m_p_memcached, key, strlen(key), 0);
    if (MEMCACHED_SUCCESS != m_ret)
    {
	    //CRIT_LOG("mem del key:%s failed.", key);
        return -1;
    }
    return 0;
}

int c_memcached::set_role(uint32_t user_id, role_t *p_role)
{
    sprintf(m_key, "%u/%u", user_id, MEMKEY_ROLE);

    if (0 != mem_set(m_key, p_role, sizeof(role_t)))
    {
        mem_del(m_key);
        //KCRIT_LOG(user_id, "set user role in memcached failed.");
        return -1;
    }

    return 0;
}

int c_memcached::get_role(uint32_t user_id, role_t *p_role)
{
    sprintf(m_key, "%u/%u", user_id, MEMKEY_ROLE);

    if (0 != mem_get(m_key, p_role, sizeof(role_t)))
    {
        return -1;
    }

    return 0;
}

int c_memcached::get_room(uint32_t user_id, int room_id, char *room, uint32_t *p_room_len)
{
    sprintf(m_key, "%u/%d/%u", user_id, room_id, MEMKEY_ROOM);

    if (0 != mem_get(m_key, room, p_room_len))
    {
        return -1;
    }

    if (*p_room_len != bswap(*(uint16_t *)room))
    {
        mem_del(m_key);
        //KCRIT_LOG(user_id, "room len is invalid.");
        return -1;
    }

    return 0;
}

int c_memcached::set_room(uint32_t user_id, int room_id, char *room)
{
    sprintf(m_key, "%u/%d/%u", user_id, room_id, MEMKEY_ROOM);
    int room_len = bswap(*(uint16_t *)room);
    if (0 != mem_set(m_key, room, room_len))
    {
        mem_del(m_key);
        //KCRIT_LOG(user_id, "set user room:%u in memcached failed.", room_id);
        return -1;
    }

    return 0;
}

int c_memcached::set_day_restrict(uint32_t user_id, uint32_t value, int type, uint32_t time)
{
    sprintf(m_key, "%u/%u/%u/%u", user_id, type, time, MEMKEY_DAY_RESTRICT);
    if (0 != mem_set(m_key, &value, sizeof(value), 24 * 60 * 60))
    {
        mem_del(m_key);
        //KCRIT_LOG(user_id, "set day restrict in memcached failed.");
        return -1;
    }

    return 0;
}

int c_memcached::get_day_restrict(uint32_t user_id, int type, uint32_t time, uint32_t *p_value)
{
    sprintf(m_key, "%u/%u/%u/%u", user_id, type, time, MEMKEY_DAY_RESTRICT);
    if (0 != mem_get(m_key, p_value, sizeof(uint32_t)))
    {
        return -1;
    }

    return 0;
}

int c_memcached::get_game_score(uint32_t user_id, int type, uint32_t *p_value)
{
    sprintf(m_key, "%u/%u/%u", user_id, type, MEMKEY_GAME_SCORE);
    if (0 != mem_get(m_key, p_value, sizeof(uint32_t)))
    {
        return -1;
    }

    return 0;
}

int c_memcached::set_game_score(uint32_t user_id, int type, uint32_t value)
{
    sprintf(m_key, "%u/%u/%u", user_id, type, MEMKEY_GAME_SCORE);
    if (0 != mem_set(m_key, &value, sizeof(value)))
    {
        mem_del(m_key);
        //KCRIT_LOG(user_id, "set game total score in memcached failed.");
        return -1;
    }

    return 0;
}
