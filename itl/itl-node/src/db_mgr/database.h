/** 
 * ========================================================================
 * @file database.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-24
 * Modify $Date: 2012-11-05 14:42:46 +0800 (一, 05 11月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DB_MGR_DATABASE_H_2012_10_24
#define H_DB_MGR_DATABASE_H_2012_10_24

#include "db_mgr_common.h"


int parse_db_priv(char * str, uint32_t * p_priv);

int gen_db_priv_string(uint32_t priv, char * buf, uint32_t len);


#endif
