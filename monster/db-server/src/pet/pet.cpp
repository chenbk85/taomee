/**
 * =====================================================================================
 *       @file  pet.cpp
 *      @brief  
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  08/02/2011 06:25:44 PM 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
extern "C"
{
#include <libtaomee/log.h>
}
#include <benchapi.h>
#include "../../../common/pack/c_pack.h"
#include "../../../common/message.h"
#include "../../../common/data_structure.h"
#include "../../../common/constant.h"
#include "../util.h"
#include "../db_constant.h"
#include "pet.h"

extern c_pack g_pack;
static char g_sql_str[1024] = {0};

uint32_t get_all_pet(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
    if (check_val_len(msg_type, request_len, 0) != 0)
    {
        return ERR_MSG_LEN;
    }

    sprintf(g_sql_str, "SELECT pet_id, total_num, follow_num FROM db_monster_%d.t_pet_%d WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);

    MYSQL_ROW row = NULL;
    int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
    if (result_count < 0)
    {
        KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
        return ERR_SQL_ERR;
    }

    char buffer[8192] = {0};
    pet_t *p_pet = (pet_t *)(buffer);
    p_pet->count = 0;
    while (row != NULL)
    {
        if(str2uint(&p_pet->pet[p_pet->count].pet_id, row[0]) != 0)
        {
            KCRIT_LOG(user_id, "convert pet_id:%s to uint32 failed(%s).", row[0], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_pet->pet[p_pet->count].total_num, row[1]) != 0)
        {
            KCRIT_LOG(user_id, "convert total_count:%s to uint32 failed(%s).", row[1], g_sql_str);
            return ERR_SQL_ERR;
        }

        if(str2uint(&p_pet->pet[p_pet->count].follow_num, row[2]) != 0)
        {
            KCRIT_LOG(user_id, "convert follow_count:%s to uint8 failed(%s).", row[2], g_sql_str);
            return ERR_SQL_ERR;
        }

        ++p_pet->count;

        row = p_mysql_conn->select_next_row(true);
    }  

    g_pack.pack(buffer, sizeof(pet_t) + p_pet->count * sizeof(pet_info_t));

    return 0;
}
