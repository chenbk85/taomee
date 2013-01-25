/**
 * =====================================================================================
 *       @file  db_operator.cpp
 *      @brief  
 *   @internal
 *     Created  2012-01-13 10:16:50
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2012, TaoMee.Inc, ShangHai.
 *
 *     @author  tonyliu(LCT), tonyliu@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdlib.h>

#include "lib/log.h"
#include "db_operator.h"

c_head_db_operator::c_head_db_operator(): m_inited(false), m_p_db_conn(NULL)
{
}

c_head_db_operator::~c_head_db_operator()
{
    uninit();
}

int c_head_db_operator::init(const db_conf_t *p_db_conf)
{
    if (m_inited) {
        ERROR_LOG("ERROR: c_head_db_operator has been inited.");
        return -1;
    }

    if (NULL == p_db_conf) {
        ERROR_LOG("ERROR: parameter cannot be NULL.");
        return -1;
    }

    if (0 != create_mysql_iface_instance(&m_p_db_conn)) {
        ERROR_LOG("ERROR: create_mysql_iface_instance() failed.");
        return -1;
    }

    if (0 != m_p_db_conn->init(p_db_conf->ip, (unsigned short)p_db_conf->port, p_db_conf->db_name,
                p_db_conf->user, p_db_conf->passwd, p_db_conf->charset)) {
        ERROR_LOG("ERROR: m_p_db_conn->init() failed[%s].", m_p_db_conn->get_last_errstr());
        return -1;
    }
    m_inited = true;

    return 0;
}

int c_head_db_operator::uninit()
{
    if(NULL != m_p_db_conn) {
        m_p_db_conn->uninit();
        m_p_db_conn->release();
        m_p_db_conn = NULL;
    }
    m_inited = false;

    return 0;
}

/** 
 * @brief   获取第一个网段ID
 * @param   
 * @return  网段ID: succ， 0:没有网段ID，-1: failed 
 */
int c_head_db_operator::get_first_segment_id()
{
    const char sql[MAX_STR_LEN] = "SELECT segment_id FROM t_network_segment_info";
    MYSQL_ROW row;
    int row_cnt = m_p_db_conn->select_first_row(&row, sql);
    if (row_cnt < 0) {
        ERROR_LOG("ERROR: select_first_row(%s) failed[%s].", sql, m_p_db_conn->get_last_errstr()); 
        return -1;
    } else if (0 == row_cnt){
        return 0;
    }

    return atoi(row[0]);
}

/** 
 * @brief   获取第一个网段ID
 * @param   
 * @return  网段ID: succ， 0:没有网段ID，-1: failed 
 */
int c_head_db_operator::get_next_segment_id()
{
    MYSQL_ROW row = m_p_db_conn->select_next_row(true);
    if (NULL == row) {
        ERROR_LOG("ERROR: select_next_row() failed[%s].", m_p_db_conn->get_last_errstr()); 
        return -1;
    }

    return atoi(row[0]);
}

