/**
 * =====================================================================================
 *       @file  db_operator.h
 *      @brief  
 *   @internal
 *     Created  2012-01-13 10:13:42
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2012, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu(LCT), tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef H_DB_OPERATOR_20120113_H
#define H_DB_OPERATOR_20120113_H

#include "lib/c_mysql_iface.h"

#define MAX_STR_LEN 256
#define COM_STR_LEN 64

typedef struct {
    char ip[16];
    unsigned short port;
    char db_name[COM_STR_LEN];
    char user[COM_STR_LEN];
    char passwd[MAX_STR_LEN];
    char charset[COM_STR_LEN];
} db_conf_t;

class c_head_db_operator {
public:
    c_head_db_operator();
    ~c_head_db_operator();
    int init(const db_conf_t *p_db_conf);
    int uninit();
    int get_first_segment_id();
    int get_next_segment_id();

private:
    bool m_inited;
    c_mysql_iface *m_p_db_conn;
};

#endif


