/** 
 * ========================================================================
 * @file log_archive.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-08-24
 * Modify $Date: 2012-10-08 15:16:08 +0800 (一, 08 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_LOG_ARCHIVE_H_2012_08_24
#define H_LOG_ARCHIVE_H_2012_08_24


#include "itl_timer.h"

extern timer_head_t g_log_archive_timer;

int init_log_archive(const char * log_dir, uint32_t begin_offset, bool remove_file = true, uint32_t keep_tar_num = 7);

int fini_log_archive();



#endif
