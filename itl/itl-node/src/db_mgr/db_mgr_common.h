/** 
 * ========================================================================
 * @file db_mgr_marco.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-10-24
 * Modify $Date: 2012-10-30 17:11:46 +0800 (二, 30 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_DB_MGR_MARCO_H_2012_10_24
#define H_DB_MGR_MARCO_H_2012_10_24

#include <libtaomee/utils.h>
#include "itl_common.h"
#include "c_mysql_iface.h"


#define DB_MGR_STR_LEN  256
#define DB_MGR_NAME_LEN 64 

// sql_no从1开始
#define SQL_NO_START    1


#define DB_MGR_ERR(code, desc) code,
enum
{
    DB_MGR_ERR_OK = 0,
    DB_MGR_ERR_START = 0x1000,
#include "error_code.h"
    DB_MGR_ERR_END
};
#undef DB_MGR_ERR


#define GEN_SQLSTR(sqlstr, ...) snprintf(sqlstr, sizeof(sqlstr), __VA_ARGS__)
#define APPEND_SQLSTR(sqlstr, ...) \
    do { \
        uint32_t sql_len = strlen(sqlstr); \
        snprintf(sqlstr + sql_len, sizeof(sqlstr) - sql_len, __VA_ARGS__); \
    } while (false)

#define UNPACK_COMMON_INFO(pkg, common, idx) \
    taomee::unpack(pkg, &(common), sizeof(db_mgr_common_t), idx);

#define UNPACK_DB_INSTANCE(pkg, instance, index) \
    do { \
        taomee::unpack(pkg, instance.ip, index); \
        taomee::unpack_h(pkg, instance.port, index); \
    } while (false)

#define UNPACK_STRING(pkg, buf, index) \
    taomee::unpack(pkg, buf, sizeof(buf), index)

#define PACK_STRING(pkg, buf, index) \
    taomee::pack(pkg, buf, sizeof(buf), index)



#define PACK_DB_PRIV(pkg, data, index) \
    do { \
        PACK_STRING(pkg, (data).user, index); \
        PACK_STRING(pkg, (data).host, index); \
        PACK_STRING(pkg, (data).db, index); \
        PACK_STRING(pkg, (data).table, index); \
        PACK_STRING(pkg, (data).column, index); \
        taomee::pack_h(pkg, (data).priv, index); \
    } while (false)


#define SET_DB_MGR_PKG_HEAD(p_pkg_head, pkg_len, result) \
    do {\
        p_pkg_head->len = pkg_len;\
        p_pkg_head->ret = result;\
    } while (false)


#define DB_MGR_STRCPY(dst, src) \
    do {\
        memset(dst, 0, sizeof(dst));\
        snprintf(dst, sizeof(dst), "%s", NULL == src ? "" : src);\
    } while (false)

#define P_CONST_PROTO(in_buf) \
    (reinterpret_cast< const db_mgr_proto_t * >(in_buf))
#define P_CONST_COMMON(in_buf) \
    (reinterpret_cast< const db_mgr_common_t * >(P_CONST_PROTO(in_buf)->body))

#define STORE_CONNECT_FAIL(in_buf) \
    db_store_db_mgr_result(P_CONST_COMMON(in_buf)->serial_no1, \
            1, 1, g_mysql->get_last_errno(), NULL, g_mysql->get_last_errstr())


#define SIZEOF_DB_MGR_HEAD (sizeof(db_mgr_proto_t) + sizeof(db_mgr_common_t))


#define DB_MGR_USER     "monitor"
#define DB_MGR_PASS     "monitor@tmpwd"
#define DB_MGR_NAME     "information_schema"
#define DB_MGR_HOST     "localhost"

#define DB_MGR_PRIVATE_KEY  "422cfaac1b203578b63460406a70e5ab"

extern c_mysql_iface * g_mysql;
extern char g_sql[SQL_BUFF_LEN];



typedef itl_proto_t db_mgr_proto_t;


struct db_mgr_common_t
{
    char veri_code[32];     /**<命令验证码*/
    uint32_t timestamp;     /**<命令发送时的时间戳*/
    uint32_t serial_no1;    /**<序列号1：与序列号2联合唯一区分DB命令*/
    uint32_t serial_no2;    /**<*/
} __attribute__((packed));


struct db_mgr_instance_t
{
    db_mgr_instance_t()
    {
        ip = 0;
        port = 0;
    }
    uint32_t ip;
    uint16_t port;
};


struct db_priv_t
{
    db_priv_t()
    {
        memset(this, 0, sizeof(db_priv_t));
    }
    char user[DB_MGR_NAME_LEN];
    char host[INET_ADDRSTRLEN];
    char db[DB_MGR_NAME_LEN];
    char table[DB_MGR_NAME_LEN];
    char column[DB_MGR_NAME_LEN];
    uint32_t priv;
};


enum
{
    LEVEL_GLOBAL = 0,
    LEVEL_TOTAL = 1,
    LEVEL_EXACT = 2,
    LEVEL_TABLE = 3,
    LEVEL_COLUMN = 4
};


#endif
