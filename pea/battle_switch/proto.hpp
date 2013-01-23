/** 
 * ========================================================================
 * @file proto.hpp
 * @brief 
 * @version 1.0
 * @date 2012-02-08
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_PROTO_H_2012_02_08
#define H_PROTO_H_2012_02_08


extern "C" {
#include <assert.h>
#include <glib.h>

#include <async_serv/net_if.h>
}

#include <map>
#include <libtaomee++/proto/proto_util.h>

#include "common.hpp"

/////////////////////////////////////////////////////////////////////////

#define DEFAULT_ARG     \
    c_player * p, Cmessage * c_in, Cmessage * c_out, void * param

//-------------------------------------------------------
////函数定义
#ifdef PROTO_FUNC_DEF
#undef PROTO_FUNC_DEF
#endif

#define PROTO_FUNC_DEF(proto_name)  \
    int proto_name(DEFAULT_ARG);

#include "proto/pea_btlsw_func_def.h"
#undef PROTO_FUNC_DEF


//-------------------------------------------------------------
////对应的结构体
#include "proto/pea_btlsw.h"

//-------------------------------------------------
//命令绑定
typedef int (* deal_func_t)(DEFAULT_ARG);

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




//////////////////////////////////////////////////////////////
// 错误码，协议号
//////////////////////////////////////////////////////////////


#include "proto/pea_btlsw_enum.h"



//////////////////////////////////////////////////////////////////////////



int find_btlsw_cmd_bind(uint16_t cmd, bind_proto_cmd_t ** pp_cmd);



void init_proto_head(void * buf, uint32_t uid, uint32_t len, uint16_t cmd, uint32_t ret, uint32_t seq);


typedef struct
{
    // 包头
    btlsw_proto_t * p_header;
    // 要发送的包体
    Cmessage * p_out;
} btlsw_proto_out_t;

#endif
