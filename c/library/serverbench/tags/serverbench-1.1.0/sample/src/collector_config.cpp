/**
 * =====================================================================================
 *       @file  mc_config.cpp
 *      @brief  配置数据检验
 *
 *  读取、解析配置文件，并判断配置数据的合法性
 *
 *   @internal
 *     Created  2008年12月19日 12时00分29秒 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2008, TaoMee.Inc, ShangHai.
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#include <stdlib.h>
#include <ctype.h>
#include "collector_config.h"
#include "collector_server.h"

/** @brief 保存配置文件的全局变量结构体 */
s_config_variable        g_config_variable;

/** 
 * @brief  初始化配置文件中的数据为变量。
 * @param   无。
 * @return 初始化结果码，0 正常， －1 配置有错误。 
 */
int32_t config_variable_init()
{
    memset(&g_config_variable, 0, sizeof(s_config_variable));
    char *ptmp = NULL;
    int32_t  tmp = 0;
    int32_t ret_code = -1;
    do
    {
        //log file config
        ptmp = config_get_strval("log_dir");
        if(ptmp == NULL)
        {
            printf("%s\nconfig_variable_init() failed! no log_dir config.\n", RED_TIP);
            ret_code = -1;
            break;
        }
        if(strlen(ptmp) >= MAX_CONFIG_STR_LEN)
        {
            printf("%s\nconfig_variable_init() failed! log_dir config string is to long, max len=%d.\n", 
                    RED_TIP, MAX_CONFIG_STR_LEN);
            ret_code = -1;
            break;
        }
        if(strlen(ptmp) < 1)
        {
            printf("%s\nconfig_variable_init() failed! log_dir config string is to short, min len=1.\n", 
                    RED_TIP);
            ret_code = -1;
            break;
        }
        sprintf(g_config_variable.log_dir, "%s", ptmp);

        ptmp = config_get_strval("log_prefix");
        if(ptmp == NULL)
        {
            printf("%s\nconfig_variable_init() failed! no log_prefix config.\n", RED_TIP);
            ret_code = -1;
            break;
        }
        if(strlen(ptmp) >= MAX_CONFIG_STR_LEN)
        {
            printf("%s\nconfig_variable_init() failed! log_prefix config string is to long, max len=%d.\n",
                    RED_TIP, MAX_CONFIG_STR_LEN);
            ret_code = -1;
            break;
        }
        if(strlen(ptmp) < 2)
        {
            printf("%s\nconfig_variable_init() failed! log_prefix config string is to short, min len=2.\n", 
                    RED_TIP);
            ret_code = -1;
            break;
        }
        sprintf(g_config_variable.log_prefix, "%s", ptmp);

        tmp = config_get_intval("log_level", 8);
        if(tmp < 0 || tmp > 8)
        {
            printf("%s\nconfig_variable_init() warning! no log_level config, or its value is illegal:%d.\n", 
                    RED_TIP, tmp);
            printf("%sAccepted value:[0, 8].\n", RED_TIP);
            ret_code = -1;
            break;
        }
        g_config_variable.log_level = tmp;

        tmp = config_get_intval("log_count", 10);
        if(tmp < 1 || tmp > 999)
        {
            printf("%s\nconfig_variable_init() warning! no log_count config, or its value is illegal:%d.\n", 
                    RED_TIP, tmp);
            printf("%sAccepted value:[1, 999].\n", RED_TIP);
            ret_code = -1;
            break;
        }
        g_config_variable.log_count = tmp;

        tmp = config_get_intval("log_size", 5242880);
        if(tmp <  5242880 || tmp > 1073741824)
        {
            printf("%s\nconfig_variable_init() warning! no log_size config, or its value is illegal:%d.\n",
                    RED_TIP, tmp);
            printf("%sAccepted value:[5242880, 1073741824].\n", RED_TIP);
            ret_code = -1;
            break;
        }
        g_config_variable.log_size = tmp;

        //database config 
        ptmp = config_get_strval("db_name");
        if(ptmp == NULL)
        {
            printf("%s\nconfig_variable_init() failed! no db_name config.\n", RED_TIP);
            ret_code = -1;
            break;
        }
        if(strlen(ptmp) >= MAX_CONFIG_STR_LEN)
        {
            printf("%s\nconfig_variable_init() failed! db_name config string is to long, max len=%d.\n", 
                    RED_TIP, MAX_CONFIG_STR_LEN);
            ret_code = -1;
            break;
        }
        sprintf(g_config_variable.db_name, "%s", ptmp);

        //db_host
        ptmp = config_get_strval("db_host");
        if(ptmp == NULL)
        {
            printf("%s\nconfig_variable_init() failed! no db_host config.\n", RED_TIP);
            ret_code = -1;
            break;
        }
        if(strlen(ptmp) >= MAX_CONFIG_STR_LEN)
        {
            printf("%s\nconfig_variable_init() failed! db_host config string is to long, max len=%d.\n", 
                    RED_TIP, MAX_CONFIG_STR_LEN);
            ret_code = -1;
            break;
        }
        sprintf(g_config_variable.db_host, "%s", ptmp);

        //db_user
        ptmp = config_get_strval("db_user");
        if(ptmp == NULL)
        {
            printf("%s\nconfig_variable_init() failed! no db_user config.\n", RED_TIP);
            ret_code = -1;
            break;
        }
        if(strlen(ptmp) >= MAX_CONFIG_STR_LEN)
        {
            printf("%s\nconfig_variable_init() failed! db_user config string is to long, max len=%d.\n", 
                    RED_TIP, MAX_CONFIG_STR_LEN);
            ret_code = -1;
            break;
        }
        sprintf(g_config_variable.db_user, "%s", ptmp);

        //db_pass
        ptmp = config_get_strval("db_pass");
        if(ptmp == NULL)
        {
            printf("%s\nconfig_variable_init() failed! no db_pass config.\n", RED_TIP);
            ret_code = -1;
            break;
        }
        if(strlen(ptmp) >= MAX_CONFIG_STR_LEN)
        {
            printf("%s\nconfig_variable_init() failed! db_pass config string is to long, max len=%d.\n", 
                    RED_TIP, MAX_CONFIG_STR_LEN);
            ret_code = -1;
            break;
        }
        sprintf(g_config_variable.db_pass, "%s", ptmp);

        //db_port
        tmp = config_get_intval("db_port", 3306);
        if(tmp == 0 || tmp == 1)
        {
            printf("%s\nconfig_variable_init() warning! no db_port config, or it is not numerical value, using 3306.\n", RED_TIP);
        }
        g_config_variable.db_port = tmp;
        //以上是通用的

        //table_prefix
        ptmp = config_get_strval("table_prefix");
        if(ptmp == NULL)
        {
            printf("%s\nconfig_variable_init() failed! no table_prefix config.\n", RED_TIP);
            ret_code = -1;
            break;
        }
        if(strlen(ptmp) >= MAX_CONFIG_STR_LEN)
        {
            printf("%s\nconfig_variable_init() failed! table_prefix config string is to long, max len=%d.\n", 
                    RED_TIP, MAX_CONFIG_STR_LEN);
            ret_code = -1;
            break;
        }
        sprintf(g_config_variable.table_prefix, "%s", ptmp);

        //table_count
        tmp = config_get_intval("table_count", 3306);
        if(tmp == 0 || tmp == 1)
        {
            printf("%s\nconfig_variable_init() warning! no table_count config, or it is not numerical value.\n", RED_TIP);
        }
        g_config_variable.table_count = tmp;

        ret_code = 0;
    }while(0);
    config_exit();
    return ret_code;
}
