#ifndef __PROTO_HPP__
#define __PROTO_HPP__



#define DEFAULT_ARG     \
    player_t *p, Cmessage *c_in, Cmessage *c_out, void *param


//-------------------------------------------------------------
// 处理函数定义 
#ifdef PROTO_FUNC_DEF
#undef PROTO_FUNC_DEF
#endif
#define PROTO_FUNC_DEF(proto_name)  \
    int proto_name(DEFAULT_ARG);

#include "proto/pea_btlsw_func_def.h"
#undef PROTO_FUNC_DEF


//-------------------------------------------------------------
// cmd相关包结构体定义 (in/out)
#include "proto/pea_btlsw.h"

//-------------------------------------------------
// 命令绑定
typedef int (*deal_func_t)(DEFAULT_ARG);

struct bind_proto_cmd_t {
    uint32_t cmd;
    deal_func_t func;
    Cmessage *p_in;
    Cmessage *p_out;
    uint32_t  md5_tag; //md5值
    uint32_t  bind_bitmap;

    void combine(const bind_proto_cmd_t *p_cmd)
    {
        if (!func && p_cmd->func) {
            func = p_cmd->func;
        }

        if (!p_in && p_cmd->p_in) {
            p_in = p_cmd->p_in;
        }

        if (!p_out && p_cmd->p_out) {
            p_out = p_cmd->p_out;
        }
    }
};


//////////////////////////////////////////////////////////////
// 错误码，协议号
#include "proto/pea_btlsw_enum.h"


bind_proto_cmd_t *find_btlsw_cmd_bind(uint16_t cmd);



#endif // __PROTO_HPP__
