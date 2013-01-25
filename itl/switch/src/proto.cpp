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

#include "proto.h"



cmd_proto_t g_switch_cmd_list[] =
{
#include "proto/itl_switch_bind.h"
};


#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid, name, c_in, c_out, md5_tag, bitmap)   \
    {cmdid, name ## _cb, new (c_in), new (c_out), md5_tag, bitmap},

//elva
cmd_proto_t g_head_cmd_list[] =
{
#include "proto/itl_head_bind_switch.h"
};


cmd_proto_t g_db_cmd_list[] =
{
#include "proto/itl_db_bind_switch.h"
};

#undef BIND_PROTO_CMD


// 协议
c_cmd_map g_switch_cmd_map(g_switch_cmd_list, array_elem_num(g_switch_cmd_list));


c_cmd_map g_head_cmd_map(g_head_cmd_list, array_elem_num(g_head_cmd_list));

c_cmd_map g_db_cmd_map(g_db_cmd_list, array_elem_num(g_db_cmd_list));

//----------------------------------------------------------------------


const cmd_proto_t * find_switch_cmd(uint16_t cmd)
{
    return g_switch_cmd_map.find_cmd(cmd);
}

const cmd_proto_t * find_head_cmd(uint16_t cmd)
{
    return g_head_cmd_map.find_cmd(cmd);
}

const cmd_proto_t * find_db_cmd(uint16_t cmd)
{
    return g_db_cmd_map.find_cmd(cmd);
}
