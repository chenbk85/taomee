/** 
 * ========================================================================
 * @file switch_macro.h
 * @brief 
 * @author tonyliu
 * @version 1.0.0
 * @date 2012-08-16
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_SWITCH_MACRO_H_20120816
#define H_SWITCH_MACRO_H_20120816


#define SWITCH_MIN_COLLECT_INTERVAL 5
#define SWITCH_COLLECT_MAX_FAIL_COUNT 10
#define SWITCH_PROC_NAME_MAX_LEN 32

#define ITL_MEMZERO(buff) memset(buff, 0, sizeof(buff))
#define SWITCH_IS_BETWEEN(a, x, y) ((a) >= (x) && (a) <= (y))
#define SWITCH_MAX(a, b) ((a) > (b) ? (a) : (b))

#define NEED_INIT(init_flag, class_name) \
    do {\
        if (!init_flag) {\
            ERROR_LOG("%s has not been inited.", class_name);\
            return -1;\
        }\
    } while (false)

#define NEED_NOT_INIT(init_flag, class_name) \
    do {\
        if (init_flag) {\
            ERROR_LOG("%s has been inited.", class_name);\
            return -1;\
        }\
    } while (false)

#endif
