/** 
 * ========================================================================
 * @file db_macro.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-17
 * Modify $Date: 2012-10-23 15:01:25 +0800 (二, 23 10月 2012) $
 * Modify $Author: tonyliu $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */


#ifndef H_DB_MARCO_H_2012_07_17
#define H_DB_MARCO_H_2012_07_17

#define DB_ERR  (-1)
#define ITL_MAX_BUF_LEN  (4096000)
#define ITL_MAX_STR_LEN  (512)
#define ITL_MAX_TAG_LEN  (64)
#define ITL_ALARM_TYPE_STR_LEN  (127)
#define ITL_ALARM_INTERVAL  (60)
#define ITL_MIN_PORT  (1025)
#define ITL_MAX_PORT  (65535)
#define IF_SPEED_BASE  (1000000)

#define ITL_BETWEEN(a, x, y) ((a) >= (x) && (a) <= (y))
#define ITL_MIN(a, b) ((a) < (b) ? (a) : (b))

#define GEN_SQLSTR(sqlstr, ...) snprintf(sqlstr, sizeof(sqlstr), __VA_ARGS__)
#define ITL_STRCAT(dst, src) strncat(dst, NULL == src ? "" : src, sizeof(dst) - strlen(dst))

#define ITL_STRCPY(dst, src) \
    do {\
        memset(dst, 0, sizeof(dst));\
        snprintf(dst, sizeof(dst), "%s", NULL == src ? "" : src);\
    } while (false)


typedef struct 
{
    int node_id;
    const char *node_ip;
    char node_tag[ITL_MAX_TAG_LEN];
} log_node_t;

#define ITL_STRCPY_LEN(owner, dst, src) \
    do{\
        ITL_STRCPY((owner)->dst, src);\
        (owner)->_##dst##_len = strlen((owner)->dst) + 1;\
    } while (false)



#define STD_EXECSQL(db, sql, ret) \
    do \
    { \
        if (g_is_log_sql) \
        { \
            DEBUG_LOG("SQL: %s", sql); \
        } \
        if (db->execsql(sql) < 0) \
        { \
            ERROR_LOG("SQL exec failed: %s", db->get_last_errstr()); \
            ret = DB_ERR; \
        } \
        else \
        { \
            ret = DB_SUCC; \
        } \
    } while (0)


#define GET_IF_COMMA_INFO(read_str, p_str) \
    do {\
        p_str = NULL;\
        if (NULL == read_str) {\
            break;\
        }\
        char *comma_pos = index(read_str, ',');\
        if (NULL != comma_pos) {\
            *comma_pos = '\0';\
        }\
        p_str = read_str;\
        read_str = (NULL == comma_pos) ? NULL : comma_pos + 1;\
    } while (false)

#define DB_CHECK_ROW(row_num, row) \
    do {\
        if (row_num <= 0 || NULL == row) {\
            ERROR_LOG("Database ERROR: DB_NO_RECORD, SQL: %s", g_sql_buff);\
            return DB_NO_RECORD;\
        }\
    } while (false)

//1-RTX 2-Mobile 3-Email
#define GET_ALAMR_WAY(alarm_way_int, alarm_way_str) \
    do {\
        if (NULL == alarm_way_str) {\
            alarm_way_int = 7;\
            break;\
        }\
        alarm_way_int = 0;\
        if (index(alarm_way_str, '1')) {\
            alarm_way_int += ALARM_WAY_RTX;\
        }\
        if (index(alarm_way_str, '2')) {\
            alarm_way_int += ALARM_WAY_MOBILE;\
        }\
        if (index(alarm_way_str, '3')) {\
            alarm_way_int += ALARM_WAY_EMAIL;\
        }\
        alarm_way_int = (alarm_way_int == 0) ? 7 : alarm_way_int;\
    } while (false)


#define STR_MERGE(dst, src) \
    do {\
        if (NULL != (src) && NULL == strstr(dst, src)) {\
            snprintf(dst + strlen(dst), sizeof(dst) - strlen(dst), "%s,", src);\
        }\
    } while(false)

#define STR_MERGE_LEN(owner, dst, src) \
    do {\
        STR_MERGE((owner)->dst, src);\
        (owner)->_##dst##_len = strlen((owner)->dst) + 1;\
    } while(false)

#define IS_LOG_NODE_IP(ip) (NULL != g_log_node.node_ip && 0 == strcmp(g_log_node.node_ip, ip))

#endif
