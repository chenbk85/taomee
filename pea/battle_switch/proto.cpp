/** 
 * ========================================================================
 * @file proto.cpp
 * @brief 
 * @version 1.0
 * @date 2012-02-08
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


#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid, name, c_in, c_out, md5_tag, bind_bitmap)     \
    {cmdid, name, new (c_in), new (c_out), md5_tag, bind_bitmap},


bind_proto_cmd_t g_btlsw_cmd_list[] = 
{
#include "proto/pea_btlsw_bind_online_req.h"
};

#undef  BIND_PROTO_CMD

class c_cmd_map
{
    public:

        c_cmd_map(bind_proto_cmd_t * p_list, uint32_t count)
            : m_list(p_list)
        {
            list_to_map(m_list, count);
        }




        // 用于根据命令号查找bind_proto_cmd_t
        inline int find_cmd(uint16_t cmd, bind_proto_cmd_t ** pp_cmd)
        {
            cmd_map_t::iterator it = m_map.find(cmd);
            if (it == m_map.end())
            {
                return -1;

            }

            if (NULL != pp_cmd)
            {
                *pp_cmd = &(it->second);
            }

            return 0;
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
                bind_proto_cmd_t * p_cmd = NULL;
                if (0 != find_cmd(cmd, &p_cmd))
                {
                    m_map[cmd] = p_list[i];
                }
                else
                {
                    p_cmd->combine(p_list + i);
                }

            }

        }

    private:


        //命令map
        typedef std::map<uint32_t, bind_proto_cmd_t> cmd_map_t;

        cmd_map_t m_map;


        const bind_proto_cmd_t * m_list;
        
};

c_cmd_map g_btlsw_cmd_map(g_btlsw_cmd_list, array_elem_num(g_btlsw_cmd_list));




//----------------------------------------------------------------------


int find_btlsw_cmd_bind(uint16_t cmd, bind_proto_cmd_t ** pp_cmd)
{
    if (0 != g_btlsw_cmd_map.find_cmd(cmd, pp_cmd))
    {

        ERROR_TLOG("btl sw cmd = %u id not find", cmd);
        return -1;
    }

    return 0;
}

void init_proto_head(void * buf, uint32_t uid, uint32_t len, uint16_t cmd, uint32_t ret, uint32_t seq)
{
    btlsw_proto_t * p_header = reinterpret_cast<btlsw_proto_t *>(buf);
    p_header->len = len;
    p_header->seq = seq;
    p_header->cmd = cmd;
    p_header->ret = ret;
    p_header->id = uid;
}

