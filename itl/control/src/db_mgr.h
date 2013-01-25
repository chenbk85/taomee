/** 
 * ========================================================================
 * @file db_mgr.h
 * @brief 
 * @author TAOMEE
 * @version 1.0
 * @date 2012-11-02
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DB_MGR_H_2012_11_02
#define H_DB_MGR_H_2012_11_02

#include <stdint.h>

int dispatch_db_mgr_to_node(int fd, const char * buf, uint32_t len);


#endif
