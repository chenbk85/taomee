/** 
 * ========================================================================
 * @file arg.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-08-06
 * Modify $Date: 2012-08-09 16:45:54 +0800 (Thu, 09 Aug 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_ARG_H_2012_08_06
#define H_ARG_H_2012_08_06


extern "C"
{
#include <glib.h>
}
#include "itl_common.h"

class c_metric;

class c_metric_arg
{
    public:

        c_metric_arg()
        {
            m_metric = NULL;
            memset(m_arg, 0, sizeof(m_arg));
            memset(&m_value, 0, sizeof(value_t));
            m_fail = 0;
        }

    public:

        char m_arg[MAX_METRIC_ARG_LEN];
        value_t m_value;

        uint8_t m_fail;

    public:
        c_metric * m_metric;
};



c_metric_arg * alloc_metric_arg(const char * p_arg, c_metric * p_metric);


void free_metric_arg(void * p);


c_metric_arg * find_metric_arg(GHashTable * p_table, const char * p_arg);

#endif
