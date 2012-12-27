/**
 * =====================================================================================
 *       @file  i_msg.h
 *      @brief  解析接受的数据包的类
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/19/2011 05:00:37 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef H_C_MSG_H_2011_07_19
#define H_C_MSG_H_2011_07_19

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

extern "C"
{
#include <libtaomee/log.h>
}

#include "bswap.h"
#include "constant.h"
#include "data_structure.h"

template <class MSG_HEADER_TYPE, bool change_byteorder> 
class c_msg
{
public:
    c_msg() : m_inited(0), m_p_msg_header(NULL), m_p_msg_body(NULL), m_msg_body_pos(0)  { }
    ~c_msg() { uninit(); }

    // 用收到的数据来初始化消息
    int init(char *p_data);

    int uninit();

    // 获得消息头的指针
    const MSG_HEADER_TYPE *get_msg_header() { return m_p_msg_header; }

    const char *get_msg_body() { return m_p_msg_body; }

    uint32_t get_msg_len() { return m_p_msg_header->len; }

    void set_msg_body(int len) { m_msg_body_pos = len; }

    // 获得消息体
    uint8_t get_uint8();

    // 获得消息体
    uint16_t get_uint16();

    // 获得消息体
    uint32_t get_uint32();

    // 获得消息体
    void get_str(char *p_data, int len);

private:
    int m_inited;

    MSG_HEADER_TYPE *m_p_msg_header;    
    char *m_p_msg_body;
    int m_msg_body_pos;
};

template <class MSG_HEADER_TYPE, bool change_byteorder> 
int c_msg<MSG_HEADER_TYPE, change_byteorder>::init(char *p_data)
{
    if (m_inited)
    {
        ERROR_LOG("c_msg has been inited.");
        return -1; 
    }

    if (NULL == p_data)
    {
        ERROR_LOG("parameter canot be NULL.");
        return -1; 
    }

    m_p_msg_header = (MSG_HEADER_TYPE *)p_data; 
    m_p_msg_body = p_data + sizeof(MSG_HEADER_TYPE);
    m_msg_body_pos = 0;

    m_inited = 1;

    return 0;
}

template <class MSG_HEADER_TYPE, bool change_byteorder> 
int c_msg<MSG_HEADER_TYPE, change_byteorder>::uninit()
{
    if (!m_inited)
    {
        ERROR_LOG("c_msg has been uninited.");
        return -1;
    }

    m_p_msg_header = NULL;
    m_p_msg_body = NULL;
    m_msg_body_pos = 0;

    m_inited = 0;
    return 0;
}

template <class MSG_HEADER_TYPE, bool change_byteorder> 
uint8_t c_msg<MSG_HEADER_TYPE, change_byteorder>::get_uint8()
{
    uint8_t value = 0;
    if (change_byteorder)
    {
        value = bswap(*(uint8_t *)(m_p_msg_body + m_msg_body_pos));
    }
    else
    {
        value = *(uint8_t *)(m_p_msg_body + m_msg_body_pos);
    }
    m_msg_body_pos += sizeof(uint8_t);

    return value;
}

template <class MSG_HEADER_TYPE, bool change_byteorder> 
uint16_t c_msg<MSG_HEADER_TYPE, change_byteorder>::get_uint16()
{
    uint16_t value = 0;
    if (change_byteorder)
    {
        value = bswap(*(uint16_t *)(m_p_msg_body + m_msg_body_pos));
    }
    else
    {
        value = *(uint16_t *)(m_p_msg_body + m_msg_body_pos);
    }
    m_msg_body_pos += sizeof(uint16_t);

    return value;
}

template <class MSG_HEADER_TYPE, bool change_byteorder> 
uint32_t c_msg<MSG_HEADER_TYPE, change_byteorder>::get_uint32()
{
    uint32_t value = 0;
    if (change_byteorder)
    {
        value = bswap(*(uint32_t *)(m_p_msg_body + m_msg_body_pos));
    }
    else
    {
        value = *(uint32_t *)(m_p_msg_body + m_msg_body_pos);
    }
    m_msg_body_pos += sizeof(uint32_t);

    return value;
}

template <class MSG_HEADER_TYPE, bool change_byteorder> 
inline void c_msg<MSG_HEADER_TYPE, change_byteorder>::get_str(char *p_data, int len)
{
    memcpy(p_data, m_p_msg_body + m_msg_body_pos, len);
    m_msg_body_pos += len;
}

#endif //H_C_MSG_H_2011_07_19 

