/**
 * ========================================================================
 * @file c_memcached.h
 * @brief
 * @author smyang（杨思敏），smyang@taomee.com
 * @version 1.0
 * @date 2011-04-18
 * Modify $Date: 2011-06-08 14:01:09 +0800 (三, 2011-06-08) $
 * Modify $Author: henry $
 * Modify $Date: 2011-09-08 11:15:09 +0800 (四, 2011-09-08) $
 * Modify $Author: luis $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_C_MEMCACHED_H_2011_04_18
#define H_C_MEMCACHED_H_2011_04_18

#include <libmemcached/memcached.h>
#include <stdio.h>
#include "../../../common/message.h"
#include "../../../common/data_structure.h"
#include "../mem_key.h"
#include "../db_structure.h"

#define MEMCACHED_KEY_LEN       100

class c_memcached
{
public:
    c_memcached();
    ~c_memcached();

    int init(const char *server_add, int server_port);
    // 默认释放时不清理memcached缓存
    int uninit(uint8_t flush_flag = 0);

    //获得和设置用户的role信息
    int get_role(uint32_t user_id, role_t *p_role);
    int set_role(uint32_t user_id, role_t *p_role);

    //获得和设置用户的房间装饰信息
    int get_room(uint32_t user_id, int room_id, char *room, uint32_t *p_room_len);
    int set_room(uint32_t user_id, int room_id, char *room);

    //获得和设置天限制的值
    int get_day_restrict(uint32_t user_id, int type, uint32_t time, uint32_t *p_value);
    int set_day_restrict(uint32_t user_id, uint32_t value, int type, uint32_t time);

    //获得和设置小游戏总分
    int get_game_score(uint32_t user_id, int type, uint32_t *p_value);
    int set_game_score(uint32_t user_id, int type, uint32_t value);

protected:
    /**
     * @brief 将数据保存到memcached
     *
     * @param key
     * @param value 指向要保存的数据
     * @param value_len 要保存的数据的长度
     * @param expiration 过期时间
     *
     * @return 0成功，-1失败
     */
    int mem_set(const char *key, const void *value, const uint32_t value_len, time_t expiration = 0, uint32_t flags = 0);

    /**
     * @brief 读取memcached中的数据，用于取长度已知的数据
     *
     * @param key
     * @param p_data 读出的数据
     * @param buffer_len key对应的value的长度必须为buffer_len，否则失败
     *
     * @return -1失败，0成功
     */
    int mem_get(const char *key, void *p_data, const uint32_t buffer_len, uint32_t *flags = NULL);

    /**
     * @brief 用于取长度未知的数据
     *
     * @param key
     * @param p_data 存放读出的数据
     * @param buffer_len 存放读出数据的长度
     *
     * @return -1失败，0成功
     */
    int mem_get(const char *key, void *p_data, uint32_t *buffer_len, uint32_t *flags = NULL);

    int mem_del(const char *key);

private:
    // 是否初始化的标志
    uint8_t m_inited;
    memcached_st *m_p_memcached;

    memcached_return_t m_ret;

    char m_key[MEMCACHED_KEY_LEN];
};

#endif // H_C_MEMCACHED_H_2011_04_18
