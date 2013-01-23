/** 
 * ========================================================================
 * @file session.hpp
 * @brief 
 * @version 1.0
 * @date 2012-04-25
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_SESSION_H_2012_04_25
#define H_SESSION_H_2012_04_25


#define db_check_session_cmd    0xA024


#include "login_impl.hpp"

typedef usr_info_t player_t;

int check_session(player_t * p, uint32_t del_flag = 0);


int check_session_callback(player_t * p, const uint8_t * body, uint32_t body_len, int ret);

#endif
