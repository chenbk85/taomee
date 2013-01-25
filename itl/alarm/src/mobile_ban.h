/** 
 * ========================================================================
 * @file mobile_ban.h
 * @brief 短信屏蔽
 * @author smyang
 * @version 1.0
 * @date 2012-09-05
 * Modify $Date: 2012-09-05 17:50:04 +0800 (三, 05  9月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_MOBILE_BAN_H_2012_09_05
#define H_MOBILE_BAN_H_2012_09_05

#include "itl_common.h"

int init_mobile_ban();
int fini_mobile_ban();



bool is_mobile_ban(uint32_t time = 0);

#endif
