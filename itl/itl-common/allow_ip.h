/** 
 * ========================================================================
 * @file allow_ip.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-08-23
 * Modify $Date: 2012-08-27 18:21:53 +0800 (一, 27  8月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_ALLOW_IP_H_2012_08_23
#define H_ALLOW_IP_H_2012_08_23


int init_allow_ip(const char * allow_client_ip);

int fini_allow_ip();


bool is_allowed_ip(const char * ip);

bool is_allowed_ip(uint32_t ip);

#endif

