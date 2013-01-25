/** 
 * ========================================================================
 * @file metric_value.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-11
 * Modify $Date: 2012-10-17 15:28:38 +0800 (三, 17 10月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_METRIC_VALUE_H_2012_07_11
#define H_METRIC_VALUE_H_2012_07_11



#include <stdint.h>
#include <stdlib.h>
#include <libtaomee++/inet/pdumanip.hpp>
#include "define.h"


// metric返回值的类型
enum itl_value_type_t
{
    ITL_VALUE_BEGIN = 0,

    ITL_VALUE_STRING,

    ITL_VALUE_UINT32,
    ITL_VALUE_INT32,

    ITL_VALUE_UINT64,
    ITL_VALUE_INT64,

    ITL_VALUE_FLOAT,
    ITL_VALUE_DOUBLE,

    ITL_VALUE_END
};




// 用func做遍历
#define TYPE_FUNC(func) func(uint32_t) func(int32_t) func(uint64_t) func(int64_t) func(float) func(double)



class c_value
{
    private:

        // 类型
        uint32_t m_type;
        // 值指针
        char * m_value;
        // m_value的size
        uint32_t m_size;


    public:

        // 构造函数
        c_value();
        c_value(const uint32_t type);
        // 析构函数
        ~c_value();
        // 拷贝构造
        c_value(const c_value & v);

        // 重载=
        c_value & operator =(const c_value & v);


        inline const uint32_t get_type() const
        {
            return m_type;
        }

        inline int set_type(const uint32_t type)
        {
            if (check_type(type))
            {
                return 0;
            }

            free_value();
            m_type = type;
            return alloc_value(m_type);
        }


        bool check_type(const uint32_t type) const
        {
            return (m_type == type);
        }

        inline char * get_buf()
        {
            return m_value;
        }

        inline void reset()
        {
            if (NULL == m_value)
            {
                alloc_value(m_type);
            }

            if (NULL != m_value)
            {
                memset(m_value, 0, m_size);
            }
        }

        /** 
         * @brief 赋值函数集
         * 根据m_type来决定类型
         * 
         * @param 
         * 
         * @return 0成功，-1失败
         */
#define SET_FUNC(type) c_value & operator =(const type & value);
        TYPE_FUNC(SET_FUNC);
#undef SET_FUNC
        c_value & operator =(const char * buf)
        {
            set_by_string(buf);
            return *this;
        }
        // 将字符串按照m_type转换成c_value
        int set_by_string(const char * buf);
        int sprintf(const char * fmt, ...);

        // 获取m_value的数字形式
#define GET_FUNC(type) type get_ ## type() const;
        TYPE_FUNC(GET_FUNC);
#undef GET_FUNC
        // 获得字符串形式
        const char * get_string(const char * fmt = NULL) const;

        // pack & unpack
        int pack(void * buf, int & index);
        int unpack(const void * buf, int & index);


    private:

        int alloc_value(const uint32_t type);

        inline void free_value()
        {
            if (NULL != m_value)
            {
                free(m_value);
                m_value = NULL;
            }

            m_size = 0;
        }


#define ALLOC_VALUE_FUNC(type) inline int alloc_ ## type();
        TYPE_FUNC(ALLOC_VALUE_FUNC);
        inline int alloc_string();
#undef ALLOC_VALUE_FUNC


#define GET_VALUE_FUNC(type) inline int read_ ## type(type & value);
        TYPE_FUNC(GET_VALUE_FUNC);
#undef GET_VALUE_FUNC


#define SET_VALUE_FUNC(type) inline int write_ ## type(const type value);
        TYPE_FUNC(SET_VALUE_FUNC);
        int write_string(const char * buf);
#undef SET_VALUE_FUNC

        int set(const void * value);

        int resize(uint32_t new_size);
};



const char * op2str(uint32_t op);



/** 
 * @brief 将较大的值转换成较小的，单位附加K/M/G
 * eg. 10240B -> 10KB
 * @param value 
 * @param orig_unit
 * @param buf 存放转换后的字符串
 * @param buf_len
 * 
 * @return 
 */
int compact_value_unit(double value, const char * orig_unit, char * buf, const uint32_t buf_len);



#endif
