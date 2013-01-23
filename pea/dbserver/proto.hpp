/** 
 * ========================================================================
 * @file proto.hpp
 * @brief 
 * @version 1.0
 * @date 2011-12-26
 * Modify $Date: 2012-05-24 14:31:10 +0800 (四, 24  5月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_PROTO_H_2011_12_26
#define H_PROTO_H_2011_12_26


extern "C" {
#include <assert.h>
#include <glib.h>

#include <libtaomee/utils.h>
#include <libtaomee/log.h>
}

#include <map>
#include <libtaomee++/proto/proto_base.h>

#ifndef GF_PROTO_H
#define GF_PROTO_H

#ifndef MK_PROTO_DOC
#include "proto_header.h"
#endif


#define RECVBUF_ROLETM      (RECVBUF_HEADER->role_tm)
#define RECVBUF_RESULT      (RECVBUF_HEADER->result)
#define USERID_ROLETM       RECVBUF_USERID, RECVBUF_ROLETM

#endif





/////////////////////////////////////////////////////////////////////////

#define DEAL_FUNC_ARG     \
    userid_t user_id, Cmessage * c_in, Cmessage * c_out, void * param

#define PROTO_IN(c_in)  (dynamic_cast<typeof p_out>(c_in))
#define PROTO_OUT(c_out)  (dynamic_cast<typeof p_out>(c_out))
//-------------------------------------------------------
//-------------------------------------------------------------
////对应的结构体
#include "proto/pea_db.h"
//-------------------------------------------------
//命令绑定

class Croute_func;

typedef int (Croute_func::* deal_func_t)(DEAL_FUNC_ARG);

struct bind_proto_cmd_t
{
    uint32_t cmd;
    deal_func_t func;
    Cmessage * p_in;
    Cmessage * p_out;
    uint32_t  md5_tag; //md5值
    uint32_t  bind_bitmap;

    void combine(const bind_proto_cmd_t * p_cmd)
    {
        if (NULL == func && NULL != p_cmd->func)
        {
            func = p_cmd->func;
        }

        if (NULL == p_in && NULL != p_cmd->p_in)
        {
            p_in = p_cmd->p_in;
        }

        if (NULL == p_out && NULL != p_cmd->p_out)
        {
            p_out = p_cmd->p_out;
        }
    }

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

//////////////////////////////////////////////////////////////
// 错误码，协议号
//////////////////////////////////////////////////////////////

#include "proto/pea_db_enum.h"

//////////////////////////////////////////////////////////////////////////



#endif
