/** 
 * ========================================================================
 * @file proto.hpp
 * @brief 
 * @version 1.0
 * @date 2011-12-26
 * Modify $Date: 2012-04-12 09:49:53 +0800 (Thu, 12 Apr 2012) $
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

class Player;

/////////////////////////////////////////////////////////////////////////
#undef DEFAULT_ARG
#define DEFAULT_ARG     \
    Player * p, Cmessage * c_in, Cmessage * c_out, void * param

//-------------------------------------------------------
////函数定义



#undef PROTO_FUNC_DEF
#define PROTO_FUNC_DEF(proto_name) \
     int proto_name ## _callback(DEFAULT_ARG = NULL);
#include "proto/pea_db_func_def.h"


#undef PROTO_FUNC_DEF
#define PROTO_FUNC_DEF(proto_name)\
    int proto_name( DEFAULT_ARG ) ;
#include "proto/pea_battle_func_def.h"


//-------------------------------------------------------------
////对应的结构体
#include "proto/pea_db.h"
#include "proto/pea_battle.h"
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

#include "proto/pea_db_enum.h"
#include "proto/pea_online_enum.h"
#include "proto/pea_battle_enum.h"
//////////////////////////////////////////////////////////////////////////

const bind_proto_cmd_t * find_db_cmd_bind(uint16_t cmd);

const bind_proto_cmd_t * find_online_cmd_bind(uint16_t cmd);

#endif
