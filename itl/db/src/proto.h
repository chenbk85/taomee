/** 
 * ========================================================================
 * @file proto.h
 * @brief 
 * @version 1.0
 * @date 2011-12-26
 * Modify $Date: 2012-07-17 10:43:44 +0800 (二, 17  7月 2012) $
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



/////////////////////////////////////////////////////////////////////////

#define DEFAULT_ARG     \
    Cmessage * c_in, Cmessage * c_out, void * param

#include "proto_bind.h"

//-------------------------------------------------------
////函数声明

#include "proto/itl_db_func_def.h"



//-------------------------------------------------------------
////对应的结构体
#include "./proto/itl.h"
#include "./proto/itl_db.h"

//-------------------------------------------------


//////////////////////////////////////////////////////////////
// 错误码，协议号
//////////////////////////////////////////////////////////////

#include "proto/itl_db_enum.h"

//////////////////////////////////////////////////////////////////////////


const cmd_proto_t * find_db_cmd(uint16_t cmd);



#endif
