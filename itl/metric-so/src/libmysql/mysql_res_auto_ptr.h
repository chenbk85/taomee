/**
 * =====================================================================================
 *       @file  mysql_res_auto_ptr.h
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
#include <mysql/mysql.h>

#ifndef _MYSQL_RES_AUTO_PTR_H_MINGLIN_XUU_080722_
#define _MYSQL_RES_AUTO_PTR_H_MINGLIN_XUU_080722_


/** 
 * @class c_mysql_res_auto_ptr_t 
 * @brief 将mysql查询结果存储记录封装 
 */
class c_mysql_res_auto_ptr_t
{
public:
    c_mysql_res_auto_ptr_t(MYSQL_RES* p_record_set);
    ~c_mysql_res_auto_ptr_t();
    
    int detach();
    int free();
    
private:
    MYSQL_RES* m_p_record_set;
};


#endif//_MYSQL_RES_AUTO_PTR_H_MINGLIN_XUU_080722_

