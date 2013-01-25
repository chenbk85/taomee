/** 
 * ========================================================================
 * @file database.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-24
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#include "database.h"



const char * g_db_priv[] = 
{
#define DB_MGR_DB_PRIV(name) #name,
#include "db_priv_define.h"
#undef DB_MGR_DB_PRIV
};



int parse_db_priv(char * str, uint32_t * p_priv)
{
    if (0 == strcmp("ALL PRIVILEGES", str))
    {
        for (uint32_t i = 0; i < array_elem_num(g_db_priv); i++)
        {
            *p_priv |= (1<<i);
        }
        return 0;
    }
    char * p = str;
    char * token = NULL;
    char * save_ptr = NULL;
    while (NULL != (token = strtok_r(p, ",", &save_ptr)))
    {
        p = NULL;
        while (' ' == token[0])
        {
            // 跳过起头的空格
            token++;
        }

        for (uint32_t i = 0; i < array_elem_num(g_db_priv); i++)
        {
            if (0 == strcasecmp(token, g_db_priv[i]))
            {
                *p_priv |= (1<<i);
            }

        }

    }

    return 0;
}


int gen_db_priv_string(uint32_t priv, char * buf, uint32_t len)
{
    buf[0] = 0;
    for (uint32_t i = 0; i < array_elem_num(g_db_priv); i++)
    {
        if (priv & (1<<i))
        {
            uint32_t buf_len = strlen(buf);
            snprintf(buf + buf_len, len - buf_len, "%s, ", g_db_priv[i]);
        }

    }
    if (buf[0])
    {
        buf[strlen(buf) - 2] = 0;
    }


    return 0;
}



