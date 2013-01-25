/** 
 * ========================================================================
 * @file priv.h
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-10-29
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_PRIV_H_2012_10_30
#define H_PRIV_H_2012_10_30


#include <vector>
#include "db_mgr_common.h"

int parse_show_grant(const char * str, std::vector< db_priv_t > & ret_vec);




int get_priv(uint32_t port, const char * user, const char * host, std::vector< db_priv_t > & ret_vec);




#endif
