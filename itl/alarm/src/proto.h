/** 
 * ========================================================================
 * @file proto.h
 * @brief 
 * @version 1.0
 * @date 2011-12-26
 * Modify $Date: 2012-07-18 14:20:24 +0800 (三, 18  7月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_PROTO_H_2011_12_26
#define H_PROTO_H_2011_12_26


extern "C" {
#include <assert.h>

#include <libtaomee/utils.h>
}

class c_node;

/////////////////////////////////////////////////////////////////////////

#define DEFAULT_ARG     \
    c_node * p, Cmessage * c_in, Cmessage * c_out

#include "proto_bind.h"


//-------------------------------------------------------
////函数声明

#include "proto/itl_alarm_func_def.h"

#undef PROTO_FUNC_DEF
#define PROTO_FUNC_DEF(proto_name) \
     int proto_name ## _cb(DEFAULT_ARG);
#include "proto/itl_db_func_def.h"

#undef PROTO_FUNC_DEF

//-------------------------------------------------------------
////对应的结构体
#include "./proto/itl.h"
#include "./proto/itl_alarm.h"
#include "./proto/itl_db.h"

//-------------------------------------------------


//////////////////////////////////////////////////////////////
// 错误码，协议号
//////////////////////////////////////////////////////////////

#include "proto/itl_alarm_enum.h"
#include "proto/itl_db_enum.h"

//////////////////////////////////////////////////////////////////////////


const cmd_proto_t * find_alarm_cmd(uint16_t cmd);

const cmd_proto_t * find_db_cmd(uint16_t cmd);


#endif
