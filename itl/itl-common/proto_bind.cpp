/** 
 * ========================================================================
 * @file proto_bind.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include "proto_bind.h"



c_cmd_map::c_cmd_map(cmd_proto_t * p_list, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t cmd = p_list[i].cmd;
        cmd_map_t::iterator it = m_data.find(cmd);
        if (it == m_data.end())
        {
            m_data[cmd] = p_list + i;
        }
        else
        {
            ERROR_LOG("cmd: %u exists", cmd);
        }

    }

}


c_cmd_map::~c_cmd_map()
{
    cmd_map_t::iterator it = m_data.begin();
    while (it != m_data.end())
    {
        cmd_proto_t * p_cmd = it->second;
        if (NULL != p_cmd->p_in)
        {
            delete (p_cmd->p_in);
        }

        if (NULL != p_cmd->p_out)
        {
            delete (p_cmd->p_out);
        }

        it++;
    }

    m_data.clear();

}


const cmd_proto_t * c_cmd_map::find_cmd(uint16_t cmd)
{
    cmd_map_t::const_iterator it = m_data.find(cmd);
    if (it == m_data.end())
    {
        return NULL;

    }

    return (it->second);
}

