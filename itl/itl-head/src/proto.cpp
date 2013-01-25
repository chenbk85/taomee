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

char g_pkg_buf[PKG_BUF_SIZE];

cmd_proto_t g_head_cmd_list[] =
{
#include "proto/itl_head_bind_node.h"
};


#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid, name, c_in, c_out, md5_tag, bitmap)   \
    {cmdid, name ## _cb, new (c_in), new (c_out), md5_tag, bitmap},
cmd_proto_t g_db_cmd_list[] =
{
#include "proto/itl_db_bind_head.h"
};


cmd_proto_t g_alarm_cmd_list[] =
{
#include "proto/itl_alarm_bind_head.h"
};



#undef BIND_PROTO_CMD





// 协议
c_cmd_map g_head_cmd_map(g_head_cmd_list, array_elem_num(g_head_cmd_list));


// db的协议
c_cmd_map g_db_cmd_map(g_db_cmd_list, array_elem_num(g_db_cmd_list));


// alarm的协议
c_cmd_map g_alarm_cmd_map(g_alarm_cmd_list, array_elem_num(g_alarm_cmd_list));


//----------------------------------------------------------------------


const cmd_proto_t * find_head_cmd(uint16_t cmd)
{
    return g_head_cmd_map.find_cmd(cmd);
}




const cmd_proto_t * find_db_cmd(uint16_t cmd)
{
    return g_db_cmd_map.find_cmd(cmd);
}



const cmd_proto_t * find_alarm_cmd(uint16_t cmd)
{
    return g_alarm_cmd_map.find_cmd(cmd);
}

