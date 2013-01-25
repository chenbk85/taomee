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



cmd_proto_t g_db_cmd_list[] =
{
#include "proto/itl_db_bind.h"
};




// 协议
c_cmd_map g_db_cmd_map(g_db_cmd_list, array_elem_num(g_db_cmd_list));



//----------------------------------------------------------------------


const cmd_proto_t * find_db_cmd(uint16_t cmd)
{
    return g_db_cmd_map.find_cmd(cmd);
}
