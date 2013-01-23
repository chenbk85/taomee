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


#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid, name, c_in, c_out, md5_tag, bind_bitmap)     \
    {cmdid, name, new (c_in), new (c_out), md5_tag, bind_bitmap},



bind_proto_cmd_t g_login_cmd_list[] =
{
#include "proto/pea_login_bind_client.h"
};


bind_proto_cmd_t g_db_cmd_list[] =
{
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid, name, c_in, c_out, md5_tag, bind_bitmap)     \
        {cmdid, name ## _callback, new (c_in), new (c_out), md5_tag, bind_bitmap},
#include "proto/pea_db_bind_login.h"
};

#undef BIND_PROTO_CMD



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


c_cmd_map g_login_cmd_map(g_login_cmd_list, array_elem_num(g_login_cmd_list));


// db的协议
c_cmd_map g_db_cmd_map(g_db_cmd_list, array_elem_num(g_db_cmd_list));

//----------------------------------------------------------------------


const bind_proto_cmd_t * find_login_cmd_bind(uint16_t cmd)
{
    return g_login_cmd_map.find_cmd(cmd);
}




const bind_proto_cmd_t * find_db_cmd_bind(uint16_t cmd)
{
    return g_db_cmd_map.find_cmd(cmd);
}



// 用于读取网络序包体
bool read_from_buf_n(Cmessage * p_in, const char * buf,uint32_t buf_size)
{
	byte_array_t ba(buf,buf_size);

    // 设定为网络序
    ba.set_is_bigendian(true);

	p_in->init();

    if (!p_in->read_from_buf(ba))
    {
        return false;
    }
    if (!ba.is_end())
    {
        return false;
    }
	return true;
}
