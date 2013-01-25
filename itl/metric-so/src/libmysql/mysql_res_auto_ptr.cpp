/**
 * =====================================================================================
 *       @file  mysql_res_auto_ptr.cpp
 *      @brief  将mysql查询结果存储记录封装,便于内存的自动释放
 *
 *
 *   @internal
 *     Created  2008年12月10日 09时27分29秒 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2008, TaoMee.Inc, ShangHai.
 *
 *     @author  xml, xml@taomee.com
 * =====================================================================================
 */
#include <stdlib.h>
#include "mysql_res_auto_ptr.h"
#include "my_errno.h"
#include <libtaomee/log.h>



/** 
 * @brief  构造函数
 * @param  MYSQL_RES* p_record_set, mysql API返回的指针 
 * @return none 
 */
c_mysql_res_auto_ptr_t::c_mysql_res_auto_ptr_t(MYSQL_RES* p_record_set)
{
    m_p_record_set = p_record_set;
}


/** 
 * @brief  析构函数
 * @param  none 
 * @return none  
 */
c_mysql_res_auto_ptr_t::~c_mysql_res_auto_ptr_t()
{
    free();
}


int c_mysql_res_auto_ptr_t::detach()
{
    if(m_p_record_set != NULL)
    {
        m_p_record_set = NULL;

        return S_OK;
    }
    else
    {
        return E_LOGIC;
    }
}


/** 
 * @brief  清理内存资源
 * @param  none 
 * @return  
 */
int c_mysql_res_auto_ptr_t::free()
{
    if(m_p_record_set != NULL)
    {
        mysql_free_result(m_p_record_set);
        m_p_record_set = NULL;
        
        return S_OK;
    }
    else
    {
        return E_LOGIC;
    }
}

