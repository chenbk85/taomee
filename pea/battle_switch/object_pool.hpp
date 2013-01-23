/** 
 * ========================================================================
 * @file object_pool.hpp
 * @brief 
 * @version 1.0
 * @date 2012-02-08
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_OBJECT_POOL_H_2012_02_08
#define H_OBJECT_POOL_H_2012_02_08

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include <list>

#include "common.hpp"



template <typename obj_t>
class c_object_pool
{
    public:

        typedef std::list<obj_t *> obj_list_t;

    public:

        c_object_pool()
        {
            m_index = 0;

            m_free_list.clear();
            m_used_list.clear();
        }
        virtual ~c_object_pool()
        {
            typename obj_list_t::iterator it = m_free_list.begin();
            while (it != m_free_list.end())
            {
                obj_t * obj = *it;
                delete obj;
                it++;
            }
            m_free_list.clear();

            it = m_used_list.begin();
            while (it != m_used_list.end())
            {
                obj_t * obj = *it;
                delete obj;
                it++;
            }
            m_used_list.clear();

        }

        int get_last_errno() const
        {
            return m_errno;
        }

        const char * get_last_errstr() const
        {
            return m_errstr;
        }


        int create_obj(obj_t ** pp_obj)
        {
            if (m_free_list.empty())
            {
                if (0 != malloc_object(m_index * 2))
                {
                    return -1;
                }
            }

            obj_t * p_obj = m_free_list.front();
            m_free_list.pop_front();
            m_used_list.push_back(p_obj);

            *pp_obj = p_obj;
            return 0;
        }


        int destroy_obj(obj_t * p_obj)
        {
            m_free_list.push_back(p_obj);
            m_used_list.remove(p_obj);

            return 0;
        }

    protected:


        void set_error(int err_no, const char * msg, ...)
        {
            m_errno = err_no;

            va_list argptr;
            va_start(argptr, msg);
            vsnprintf(m_errstr, MAX_ERROR_BUFFER_LEN - 1, msg, argptr);
            va_end(argptr);
        }


        virtual int malloc_object(uint32_t count)
        {
            return 0;
        }

        // 已分配对象的下标
        uint32_t m_index;

        // 可用的对象列表
        obj_list_t m_free_list;

        // 已用的对象列表
        obj_list_t m_used_list;

    private:

        uint32_t m_errno;
        char m_errstr[MAX_ERROR_BUFFER_LEN];


};

#endif
