/** 
 * ========================================================================
 * @file proto.h
 * @brief 
 * @version 1.0
 * @date 2011-12-26
 * Modify $Date: 2012-10-11 16:51:28 +0800 (四, 11 10月 2012) $
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
}


/////////////////////////////////////////////////////////////////////////

#define DEFAULT_ARG     \
    Cmessage * c_in, Cmessage * c_out

#include "proto_bind.h"

#include "metric_value.h"
// #include "itl_rrd.h"




//-------------------------------------------------------
////函数声明

#include "proto/itl_rrd_func_def.h"


//-------------------------------------------------------------
////对应的结构体
#include "./proto/itl.h"
#include "./proto/itl_rrd.h"

//-------------------------------------------------


//////////////////////////////////////////////////////////////
// 错误码，协议号
//////////////////////////////////////////////////////////////

#include "proto/itl_rrd_enum.h"

//////////////////////////////////////////////////////////////////////////


const cmd_proto_t * find_rrd_cmd(uint16_t cmd);



#endif
