/** 
 * ========================================================================
 * @file proto.cpp
 * @brief 
 * @version 1.0
 * @date 2012-01-04
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

extern "C"
{
#include <libtaomee/utils.h>
}
#include "proto.hpp"



bind_proto_cmd_t g_db_cmd_list[] =
{
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid, name, c_in, c_out, md5_tag, bind_bitmap)     \
	    {cmdid, name ## _callback, new (c_in), new (c_out), md5_tag, bind_bitmap},
#include "proto/pea_db_bind_battle.h"
};

bind_proto_cmd_t g_online_cmd_list[] = 
{
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid, name, c_in, c_out, md5_tag, bind_bitmap)     \
	{cmdid, name, new (c_in), new (c_out), md5_tag, bind_bitmap},
#include "proto/pea_battle_bind_online_req.h"
};



class c_cmd_map
{
    public:
        c_cmd_map(bind_proto_cmd_t * p_list, uint32_t count)
            : m_list(p_list)
        {
            list_to_map(m_list, count);
        }
        ~c_cmd_map()
        {
            m_map.clear();
        }

        // 用于根据命令号查找bind_proto_cmd_t
        inline const bind_proto_cmd_t * find_cmd(uint16_t cmd)
        {
            cmd_map_t::const_iterator it = m_map.find(cmd);
            if (it == m_map.end())
            {
                return NULL;

            }

            return &(it->second);
        }
    protected:
        void list_to_map(const bind_proto_cmd_t * p_list, uint32_t count)
        {
            if (NULL == p_list)
            {
                return;
            }

            for (uint32_t i = 0; i < count; i++)
            {
                uint32_t cmd = p_list[i].cmd;
                assert(cmd != 0);
                cmd_map_t::iterator it = m_map.find(cmd);
                if (it == m_map.end())
                {
                    m_map[cmd] = p_list[i];
                }
				else
                {
                    (it->second).combine(p_list + i);
                }

            }

        }

    private:

        //命令map
        typedef std::map<uint32_t, bind_proto_cmd_t> cmd_map_t;

        cmd_map_t m_map;


        const bind_proto_cmd_t * m_list;
        
};





// db的协议
c_cmd_map g_db_cmd_map(g_db_cmd_list, array_elem_num(g_db_cmd_list));

c_cmd_map g_online_cmd_map(g_online_cmd_list, array_elem_num(g_online_cmd_list));
//----------------------------------------------------------------------
const bind_proto_cmd_t * find_db_cmd_bind(uint16_t cmd)
{
    return g_db_cmd_map.find_cmd(cmd);
}

const bind_proto_cmd_t * find_online_cmd_bind(uint16_t cmd)
{
	return g_online_cmd_map.find_cmd(cmd);
}


