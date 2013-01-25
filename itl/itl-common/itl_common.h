/** 
 * ========================================================================
 * @file itl_common.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-04
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_ITL_COMMON_H_2012_07_04
#define H_ITL_COMMON_H_2012_07_04


#include <stdint.h>



#include <libtaomee/log.h>



#include "async_server.h"

// 不要包含proto_bind.h，会引起一些不必要的麻烦

// 单体模式
#include "singleton.hpp"

// 容器
#include "container.h"

// 常数定义
#include "define.h"

// so协议
#include "so_proto.h"

// 网络协议
#include "itl_proto.h"

// 工具
#include "itl_util.h"

// http transfer
#include "http_transfer.h"

// metric value
#include "metric_value.h"

// file
#include "file.h"

// pack
#include "pack.h"

// ip过滤
#include "allow_ip.h"

// 日志打包
// 因为有定时器，定时器包含嵌入式list，跟mysql库冲突，所以不要include
// #include "log_archive.h"




#endif
