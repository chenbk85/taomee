/** 
 * ========================================================================
 * @file table.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-24
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "table.h"



const char * g_table_priv[] = 
{
#define DB_MGR_TABLE_PRIV(name) #name,
#include "table_priv_define.h"
#undef DB_MGR_TABLE_PRIV
};


int parse_table_priv(const char * str, uint32_t * p_priv)
{
    if (0 == strcmp("ALL PRIVILEGES", str))
    {
        for (uint32_t i = 0; i < array_elem_num(g_table_priv); i++)
        {
            *p_priv |= (1<<i);
        }
        return 0;
    }

    for (uint32_t i = 0; i < array_elem_num(g_table_priv); i++)
    {
        if (0 == strcasecmp(str, g_table_priv[i]))
        {
            *p_priv |= (1<<i);
        }

    }

    return 0;
}



int gen_table_priv_string(uint32_t priv, char * buf, uint32_t len)
{
    buf[0] = 0;
    for (uint32_t i = 0; i < array_elem_num(g_table_priv); i++)
    {
        if (priv & (1<<i))
        {
            uint32_t buf_len = strlen(buf);
            snprintf(buf + buf_len, len - buf_len, "%s, ", g_table_priv[i]);
        }

    }

    if (buf[0])
    {
        buf[strlen(buf) - 2] = 0;
    }


    return 0;
}


