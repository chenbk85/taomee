/**
 * =====================================================================================
 *       @file  c_pack.cpp
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/20/2011 03:29:41 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <string.h>
#include <libtaomee++/inet/pdumanip.hpp>
#include "c_pack.h"

c_pack::c_pack() : m_p_msg_buffer(NULL), m_cur_pos(0), m_set_pos(0), m_change_byteorder(false) 
{

}

c_pack::~c_pack()
{
    m_p_msg_buffer = NULL;
    m_cur_pos = 0; 
    m_set_pos = 0; 
    m_change_byteorder = false;
}

void c_pack::begin(bool change_byteorder, char *p_buffer)
{ 
    if (p_buffer != NULL)
    {
        m_p_msg_buffer = p_buffer;
    }
    else
    {
        m_p_msg_buffer = m_msg_buffer;
    }

    m_cur_pos = sizeof(uint32_t); 
    m_change_byteorder = change_byteorder;    
}

void c_pack::end(int len)
{
    uint32_t msg_len = 0;
    if (0 == len)
    {
        msg_len = m_cur_pos;
    }
    else
    {
        msg_len = len; 
    }

    if (m_change_byteorder)
    {
        *(uint32_t *)m_p_msg_buffer = taomee::bswap(msg_len);
    }
    else
    {
        *(uint32_t *)m_p_msg_buffer = msg_len;
    }

    m_change_byteorder = false;    
}

int c_pack::pack(uint8_t val, int pos)
{
    if (0 == pos)
    {
        m_set_pos = m_cur_pos; 
    }
    else if (m_set_pos + (int)sizeof(val) > m_cur_pos)
    {
        ERROR_LOG("pack failed, send buffer len:%u", m_set_pos);
        return -1;
    }
    else
    {
        m_set_pos = pos;
    }

    if (m_change_byteorder)
    {
        *((uint8_t *)(m_p_msg_buffer + m_set_pos)) = taomee::bswap(val);
    }
    else
    {
        *((uint8_t *)(m_p_msg_buffer + m_set_pos)) = val;
    }

    if(0 == pos)
    {
        m_cur_pos += sizeof(val);
    }

    return 0;
}

int c_pack::pack(uint16_t val, int pos)
{
    if (0 == pos)
    {
        m_set_pos = m_cur_pos; 
    }
    else if (m_set_pos + (int)sizeof(val) > m_cur_pos)
    {
        ERROR_LOG("pack failed, send buffer len:%u", m_set_pos);
        return -1;
    }
    else
    {
        m_set_pos = pos;
    }
    
    if (m_change_byteorder)
    {
        *((uint16_t *)(m_p_msg_buffer + m_set_pos)) = taomee::bswap(val);
    }
    else
    {
        *((uint16_t *)(m_p_msg_buffer + m_set_pos)) = val;
    }

    if(0 == pos)
    {
        m_cur_pos += sizeof(val);
    }

    return 0;
}

int c_pack::pack(uint32_t val, int pos)
{
    if (0 == pos)
    {
        m_set_pos = m_cur_pos; 
    }
    else if (m_set_pos + (int)sizeof(val) > m_cur_pos)
    {
        ERROR_LOG("pack failed, send buffer len:%u", m_set_pos);
        return -1;
    }
    else
    {
        m_set_pos = pos;
    }

    if (m_change_byteorder)
    {
        *((uint32_t *)(m_p_msg_buffer + m_set_pos)) = taomee::bswap(val);
    }
    else
    {
        *((uint32_t *)(m_p_msg_buffer + m_set_pos)) = val;
    }

    if(0 == pos)
    {
        m_cur_pos += sizeof(val);
    }

    return 0;
}

int c_pack::pack(const char *p_data, int len)
{
    memcpy(m_p_msg_buffer + m_cur_pos, p_data, len);
    m_cur_pos += len;

    return 0;
}
