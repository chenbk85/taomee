/** 
 * ========================================================================
 * @file proto.hpp
 * @brief 
 * @version 1.0
 * @date 2011-12-26
 * Modify $Date: 2012-04-12 09:49:53 +0800 (四, 2012-04-12) $
 * Modify $Author: ericma $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_PROTO_H_2011_12_26
#define H_PROTO_H_2011_12_26


extern "C" {
#include <assert.h>
#include <glib.h>

#include <async_serv/net_if.h>
}

#include <map>
#include <libtaomee++/proto/proto_util.h>

typedef class usr_info_t player_t;

/////////////////////////////////////////////////////////////////////////

#define DEFAULT_ARG     \
    player_t * p, Cmessage * c_in, Cmessage * c_out, void * param

//-------------------------------------------------------
////函数定义
#ifdef PROTO_FUNC_DEF
#undef PROTO_FUNC_DEF
#endif

#define PROTO_FUNC_DEF(proto_name)  \
    int proto_name(DEFAULT_ARG = NULL);


#include "proto/pea_login_func_def.h"

#undef PROTO_FUNC_DEF

#define PROTO_FUNC_DEF(proto_name)  \
    int proto_name ## _callback(DEFAULT_ARG = NULL);

#include "proto/pea_db_func_def.h"


#undef PROTO_FUNC_DEF


//-------------------------------------------------------------
////对应的结构体
#include "proto/pea_login.h"
#include "proto/pea_db.h"
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

#include "proto/pea_login_enum.h"
#include "proto/pea_db_enum.h"

//////////////////////////////////////////////////////////////////////////


const bind_proto_cmd_t * find_login_cmd_bind(uint16_t cmd);


const bind_proto_cmd_t * find_db_cmd_bind(uint16_t cmd);

// 用于读取网络序包体
bool read_from_buf_n(Cmessage * p_in, const char * buf, uint32_t buf_size);

#endif
