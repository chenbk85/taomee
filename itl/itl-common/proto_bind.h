/** 
 * ========================================================================
 * @file proto_bind.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-10
 * Modify $Date: 2012-08-16 11:13:33 +0800 (四, 16  8月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_PROTO_BIND_H_2012_07_10
#define H_PROTO_BIND_H_2012_07_10

#include <stdint.h>
#include <map>
#include <libtaomee++/proto/proto_base.h>

#include "pack.h"

#ifndef DEFAULT_ARG
#define DEFAULT_ARG \
    Cmessage * c_in, Cmessage * c_out
#endif


#ifndef PROTO_FUNC_DEF
#define PROTO_FUNC_DEF(proto_name)  \
    int proto_name(DEFAULT_ARG);
#endif


#ifndef BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid, name, c_in, c_out, md5_tag, bitmap)   \
{cmdid, name, new (c_in), new (c_out), md5_tag, bitmap},
#endif


//-------------------------------------------------
//命令绑定
typedef int (* deal_func_t)(DEFAULT_ARG);

struct cmd_proto_t
{
    uint32_t cmd;
    deal_func_t func;
    Cmessage * p_in;
    Cmessage * p_out;
    uint32_t  md5_tag; //md5值
    uint32_t  bind_bitmap;
};


class c_cmd_map
{
    public:

        c_cmd_map(cmd_proto_t * p_list, uint32_t count);

        ~c_cmd_map();

        // 用于根据命令号查找cmd_proto_t
        const cmd_proto_t * find_cmd(uint16_t cmd);


    private:

        //命令map
        typedef std::map<uint32_t, cmd_proto_t *> cmd_map_t;

        cmd_map_t m_data;

};

#endif
