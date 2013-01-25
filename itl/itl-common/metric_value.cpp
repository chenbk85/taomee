/** 
 * ========================================================================
 * @file metric_value.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-11
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <sstream>
#include "metric_value.h"
#include "pack.h"


using namespace taomee;


// c_value的空间是按需分配的，但最多不能超过这个数
#define MAX_VALUE_STRING_LEN    (1024 * 1024)
// 但pack的时候，最多pack这个数
#define MAX_PACK_SIZE           (2 * 1024)


#define TYPE_string     ITL_VALUE_STRING
#define TYPE_uint32_t   ITL_VALUE_UINT32
#define TYPE_int32_t    ITL_VALUE_INT32
#define TYPE_float      ITL_VALUE_FLOAT
#define TYPE_double     ITL_VALUE_DOUBLE
#define TYPE_uint64_t   ITL_VALUE_UINT64
#define TYPE_int64_t    ITL_VALUE_INT64

// 获取类型对应的itl_value_type_t
#define TYPENUM(type)  TYPE_ ## type


#define VP(type, p)     (reinterpret_cast< type * >(p))
#define VALUE(type, p)  *(VP(type, p))
#define M_VP(type)      VP(type, m_value)
#define M_VALUE(type)   VALUE(type, m_value)
#define MALLOC_VALUE(type)  m_value = (char *)malloc(sizeof(type))


c_value::c_value()
{
    m_type = ITL_VALUE_BEGIN;
    m_value = NULL;
    m_size = 0;
}

c_value::c_value(const uint32_t type)
{
    m_type = type;
    m_value = NULL;
    m_size = 0;
    alloc_value(m_type);
}




c_value::~c_value()
{
    free_value();
}


c_value::c_value(const c_value & v)
{
    m_type = v.get_type();

    alloc_value(m_type);
    set(v.m_value);
}



c_value & c_value::operator =(const c_value & v)
{

    if (!check_type(v.get_type()))
    {
        free_value();
        m_type = v.get_type();
        alloc_value(m_type);
    }


    set(v.m_value);

    return *this;
}


int c_value::alloc_value(const uint32_t type)
{

#define CASE(type) case TYPENUM(type): return alloc_ ## type();

    switch (type)
    {
        CASE(string);
        TYPE_FUNC(CASE);
        default:
            return -1;
    }

#undef CASE

    return 0;
}



#define ALLOC_VALUE_FUNC(type) \
    inline int c_value::alloc_ ## type() { \
        MALLOC_VALUE(type); \
        if (NULL == m_value) return -1; \
        m_size = sizeof(type); \
        memset(m_value, 0, m_size); \
        return 0; \
    }

TYPE_FUNC(ALLOC_VALUE_FUNC);

#undef ALLOC_VALUE_FUNC

inline int c_value::alloc_string()
{
    m_size = 32;
    m_value = (char *)malloc(m_size);
    if (NULL == m_value)
    {
        return -1;
    }

    memset(m_value, 0, m_size);
    return 0;
}



#define GET_VALUE_FUNC(type) \
    inline int c_value::read_ ## type(type & value) { \
        if (NULL == m_value) return -1; \
        value = M_VALUE(type); \
        return 0; \
    }

TYPE_FUNC(GET_VALUE_FUNC);

#undef GET_VALUE_FUNC


#define SET_VALUE_FUNC(type) \
    inline int c_value::write_ ## type(const type value) { \
        if (!check_type(TYPENUM(type))) return -1; \
        if (NULL == m_value) { \
            if (0 != alloc_value(m_type)) return -1; \
        } \
        M_VALUE(type) = value; \
        return 0; \
    }

TYPE_FUNC(SET_VALUE_FUNC);

int c_value::write_string(const char * buf)
{
    if (!check_type(TYPENUM(string)))
    {
        return -1;
    }

    uint32_t size = strlen(buf);
    if (size >= m_size)
    {
        if (0 != resize(size + 1))
        {
            return -1;
        }
    }

    snprintf(m_value, m_size, "%s", buf);
    return 0;
}

#undef SET_VALUE_FUNC




int c_value::set(const void * value)
{
    if (NULL == value)
    {
        return -1;
    }

    if (NULL == m_value)
    {
        if (0 != alloc_value(m_type))
        {
            return -1;
        }
    }

#define CASE(type) case TYPENUM(type): return write_ ## type(VALUE(const type, value));

    switch (m_type)
    {
        case TYPENUM(string):
            return write_string(reinterpret_cast< const char * >(value));

        TYPE_FUNC(CASE);
        default:
        return -1;

    }

#undef CASE

    return 0;
}


#define SET_FUNC(type) \
    c_value & c_value::operator =(const type & value) { \
        if (check_type(TYPENUM(type))) { \
            set(&value); \
            return *this; \
        } \
        switch (m_type) { \
            case TYPENUM(uint32_t): \
                { \
                    uint32_t tmp = (uint32_t)value; \
                    set(&tmp); \
                    return *this; \
                } \
            case TYPENUM(int32_t): \
                { \
                    int32_t tmp = (int32_t)value; \
                    set(&tmp); \
                    return *this; \
                } \
            case TYPENUM(uint64_t): \
                { \
                    uint64_t tmp = (uint64_t)value; \
                    set(&tmp); \
                    return *this; \
                } \
            case TYPENUM(int64_t): \
                { \
                    int64_t tmp = (int64_t)value; \
                    set(&tmp); \
                    return *this; \
                } \
            case TYPENUM(float): \
                { \
                    float tmp = (float)value; \
                    set(&tmp); \
                    return *this; \
                } \
            case TYPENUM(double): \
                { \
                    double tmp = (double)value; \
                    set(&tmp); \
                    return *this; \
                } \
            default: return *this; \
        } \
    }

TYPE_FUNC(SET_FUNC)

#undef SET_FUNC





int c_value::set_by_string(const char * buf)
{
    if (NULL == buf)
    {
        return -1;
    }

    switch (m_type)
    {
        case TYPENUM(string):
            return set(buf);
        case TYPENUM(uint32_t):
            {
                uint32_t x = strtoul(buf, NULL, 10);
                return set(&x);
            }
        case TYPENUM(int32_t):
            {
                int32_t x = atoi(buf);
                return set(&x);
            }
        case TYPENUM(uint64_t):
            {
                uint64_t x = strtoull(buf, NULL, 10);
                return set(&x);
            }
        case TYPENUM(int64_t):
            {
                int64_t x = atoll(buf);
                return set(&x);
            }
        case TYPENUM(float):
            {
                float x = atof(buf);
                return set(&x);
            }
        case TYPENUM(double):
            {
                double x = atof(buf);
                return set(&x);
            }
        default:
            return -1;

    }
}



int c_value::sprintf(const char * fmt, ...)
{
    if (NULL == fmt)
    {
        return -1;
    }

    static char buf[MAX_VALUE_STRING_LEN];
    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(buf, sizeof(buf), fmt, argptr);
    va_end(argptr);

    return set_by_string(buf);
}


#define GET_FUNC(type) \
    type c_value::get_ ## type() const { \
        if (NULL == m_value) return 0; \
        switch (m_type) { \
            case TYPENUM(uint32_t): return (type)(M_VALUE(uint32_t)); \
            case TYPENUM(int32_t): return (type)(M_VALUE(int32_t)); \
            case TYPENUM(uint64_t): return (type)(M_VALUE(uint64_t)); \
            case TYPENUM(int64_t): return (type)(M_VALUE(int64_t)); \
            case TYPENUM(float): return (type)(M_VALUE(float)); \
            case TYPENUM(double): return (type)(M_VALUE(double)); \
            default: return 0; \
        } \
    }

TYPE_FUNC(GET_FUNC);
#undef GET_FUNC



const char * c_value::get_string(const char * fmt) const
{

    if (NULL == m_value)
    {
        return "";
    }

    if (NULL == fmt)
    {
        static std::string str;
        std::stringstream str_stream;


        switch (m_type)
        {
#define CASE(type) case TYPENUM(type): str_stream<<M_VALUE(type); break;
            TYPE_FUNC(CASE);
#undef CASE
            case TYPENUM(string):
            str_stream<<m_value;
            break;
            default:
            return "";
        }

        str = str_stream.str();

        return str.c_str();

    }
    else
    {
        static char buf[MAX_VALUE_STRING_LEN];
        switch (m_type)
        {
            case TYPENUM(string):
                return M_VP(const char);
#define CASE(type) \
            case TYPENUM(type): \
                                snprintf(buf, sizeof(buf), fmt, M_VALUE(type)); return buf;
                TYPE_FUNC(CASE);
#undef CASE
            default:
                return "";



        }
    }
}


int c_value::pack(void * buf, int & index)
{
    if (NULL == buf || NULL == m_value)
    {
        return -1;
    }

    int old = index;
    uint32_t value_len = 0;
    pack_h(buf, value_len, index);

#define CASE(type) case TYPENUM(type): pack_h(buf, M_VALUE(type), index); break;

    switch (m_type)
    {
        TYPE_FUNC(CASE);
        case TYPENUM(string):
        {
            uint32_t size = m_size >= MAX_PACK_SIZE ? MAX_PACK_SIZE - 1 : m_size;
            taomee::pack(buf, m_value, size, index);
            pack8(buf, 0, index);
        }
        break;
        default:
        return -1;
    }
#undef CASE

    value_len = index - old - sizeof(value_len);
    pack_h(buf, value_len, old);
    return 0;

}


int c_value::unpack(const void * buf, int & index)
{
    if (NULL == buf)
    {
        return -1;
    }

    if (NULL == m_value)
    {
        if (0 != alloc_value(m_type))
        {
            return -1;
        }
    }

    uint32_t len = 0;
    unpack_h(buf, len, index);

#define CASE(type) case TYPENUM(type): unpack_h(buf, M_VALUE(type), index);break;

    switch (m_type)
    {
        case TYPENUM(string):
            {
                if (len >= m_size)
                {
                    if (0 != resize(len + 1))
                    {
                        return -1;
                    }
                }

                taomee::unpack(buf, m_value, len, index);
                m_value[m_size - 1] = 0;
            }
            break;

            TYPE_FUNC(CASE);
        default:
            return -1;
    }

    return 0;
}


int c_value::resize(uint32_t new_size)
{
    if (new_size > MAX_VALUE_STRING_LEN)
    {
        new_size = MAX_VALUE_STRING_LEN;
    }

    if (new_size == m_size)
    {
        return 0;
    }

    char * new_value = (char *)realloc(m_value, new_size);
    if (NULL == new_value)
    {
        return -1;
    }
    m_value = new_value;
    m_size = new_size;

    return 0;
}

// int pack_metric_value_with_len(void * buf, const value_t v, uint32_t value_type, int & index)
// {
// int old_index = index;
// uint32_t value_len = 0;
// pack_h(buf, value_len, index);
// pack_metric_value(buf, v, value_type, index);
// value_len = index - old_index - sizeof(uint32_t);
// pack_h(buf, value_len, old_index);
// return 0;

// }

// int pack_metric_value(void * buf, const value_t v, uint32_t value_type, int & index)
// {

// switch (value_type)
// {
// case OA_VALUE_STRING:
// pack_string(buf, v.value_str, sizeof(v.value_str), index);
// break;
// case OA_VALUE_UNSIGNED_SHORT:
// pack_h(buf, v.value_ushort, index);
// break;
// case OA_VALUE_SHORT:
// pack_h(buf, v.value_short, index);
// break;
// case OA_VALUE_UNSIGNED_INT:
// pack_h(buf, v.value_uint, index);
// break;
// case OA_VALUE_INT:
// pack_h(buf, v.value_int, index);
// break;
// case OA_VALUE_FLOAT:
// pack_h(buf, v.value_f, index);
// break;
// case OA_VALUE_DOUBLE:
// pack_h(buf, v.value_d, index);
// break;
// case OA_VALUE_UNSIGNED_LONG_LONG:
// pack_h(buf, v.value_ull, index);
// break;
// case OA_VALUE_LONG_LONG:
// pack_h(buf, v.value_ll, index);
// break;
// default:
// return -1;
// }
// return 0;
// }




// int unpack_metric_value(const void * buf, value_t & v, uint32_t value_type, int & index)
// {
// switch (value_type)
// {
// case OA_VALUE_STRING:
// unpack_string(buf, v.value_str, sizeof(v.value_str), index);
// break;
// case OA_VALUE_UNSIGNED_SHORT:
// unpack_h(buf, v.value_ushort, index);
// break;
// case OA_VALUE_SHORT:
// unpack_h(buf, v.value_short, index);
// break;
// case OA_VALUE_UNSIGNED_INT:
// unpack_h(buf, v.value_uint, index);
// break;
// case OA_VALUE_INT:
// unpack_h(buf, v.value_int, index);
// break;
// case OA_VALUE_FLOAT:
// unpack_h(buf, v.value_f, index);
// break;
// case OA_VALUE_DOUBLE:
// unpack_h(buf, v.value_d, index);
// break;
// case OA_VALUE_UNSIGNED_LONG_LONG:
// unpack_h(buf, v.value_ull, index);
// break;
// case OA_VALUE_LONG_LONG:
// unpack_h(buf, v.value_ll, index);
// break;
// default:
// return -1;
// }
// return 0;

// }


// double get_metric_value(const value_t & v, uint32_t value_type)
// {
// switch (value_type)
// {
// case OA_VALUE_UNSIGNED_SHORT:
// return v.value_ushort;
// case OA_VALUE_SHORT:
// return v.value_short;
// case OA_VALUE_UNSIGNED_INT:
// return v.value_uint;
// case OA_VALUE_INT:
// return v.value_int;
// case OA_VALUE_FLOAT:
// return v.value_f;
// case OA_VALUE_DOUBLE:
// return v.value_d;
// case OA_VALUE_UNSIGNED_LONG_LONG:
// return v.value_ull;
// case OA_VALUE_LONG_LONG:
// return v.value_ll;
// default:
// return 0;
// }

// }

// const char * metric_value2str(value_t & v, uint32_t value_type, const char * fmt)
// {
// switch (value_type)
// {
// case OA_VALUE_STRING:
// return v.value_str;
// case OA_VALUE_UNSIGNED_SHORT:
// {
// char * str = v.value_str + sizeof(v.value_ushort) + 1;
// uint32_t len = sizeof(v) - (str - v.value_str);
// if (NULL == fmt)
// {
// snprintf(str, len, "%u", v.value_ushort);
// }
// else
// {
// snprintf(str, len, fmt, v.value_ushort);
// }
// return str;
// }
// case OA_VALUE_SHORT:
// {
// char * str = v.value_str + sizeof(v.value_ushort) + 1;
// uint32_t len = sizeof(v) - (str - v.value_str);
// if (NULL == fmt)
// {
// snprintf(str, len, "%u", v.value_short);
// }
// else
// {
// snprintf(str, len, fmt, v.value_short);
// }
// return str;
// }
// case OA_VALUE_UNSIGNED_INT:
// {
// char * str = v.value_str + sizeof(v.value_uint) + 1;
// uint32_t len = sizeof(v) - (str - v.value_str);
// if (NULL == fmt)
// {
// snprintf(str, len, "%u", v.value_uint);
// }
// else
// {
// snprintf(str, len, fmt, v.value_uint);
// }
// return str;
// }
// case OA_VALUE_INT:
// {
// char * str = v.value_str + sizeof(v.value_int) + 1;
// uint32_t len = sizeof(v) - (str - v.value_str);
// if (NULL == fmt)
// {
// snprintf(str, len, "%d", v.value_int);
// }
// else
// {
// snprintf(str, len, fmt, v.value_int);
// }
// return str;
// }
// case OA_VALUE_FLOAT:
// {
// char * str = v.value_str + sizeof(v.value_f) + 1;
// uint32_t len = sizeof(v) - (str - v.value_str);
// if (NULL == fmt)
// {
// snprintf(str, len, "%f", v.value_f);
// }
// else
// {
// snprintf(str, len, fmt, v.value_f);
// }
// return str;
// }
// case OA_VALUE_DOUBLE:
// {
// char * str = v.value_str + sizeof(v.value_d) + 1;
// uint32_t len = sizeof(v) - (str - v.value_str);
// if (NULL == fmt)
// {
// snprintf(str, len, "%lf", v.value_d);
// }
// else
// {
// snprintf(str, len, fmt, v.value_d);
// }
// return str;
// }
// case OA_VALUE_UNSIGNED_LONG_LONG:
// {
// char * str = v.value_str + sizeof(v.value_ull) + 1;
// uint32_t len = sizeof(v) - (str - v.value_str);
// if (NULL == fmt)
// {
// snprintf(str, len, "%llu", v.value_ull);
// }
// else
// {
// snprintf(str, len, fmt, v.value_ull);
// }
// return str;
// }
// case OA_VALUE_LONG_LONG:
// {
// char * str = v.value_str + sizeof(v.value_ll) + 1;
// uint32_t len = sizeof(v) - (str - v.value_str);
// if (NULL == fmt)
// {
// snprintf(str, len, "%lld", v.value_ll);
// }
// else
// {
// snprintf(str, len, fmt, v.value_ll);
// }
// return str;
// }
// default:
// return "";
// }
// }



const char * op2str(uint32_t op)
{

    switch (op)
    {
        case OP_GE:
            return ">=";
        case OP_LE:
            return "<=";
        case OP_GT:
            return ">";
        case OP_LT:
            return "<";
        case OP_EQ:
            return "=";
        default:
            return "";
    }

    return "";
}

// int str2metric_value(const char * str, value_t & v, uint32_t value_type)
// {
// switch (value_type)
// {
// case OA_VALUE_STRING:
// snprintf(v.value_str, sizeof(v.value_str), "%s", str);
// return 0;
// case OA_VALUE_UNSIGNED_SHORT:
// v.value_ushort = strtoul(str, NULL, 10);
// return 0;
// case OA_VALUE_SHORT:
// v.value_short = atoi(str);
// return 0;
// case OA_VALUE_UNSIGNED_INT:
// v.value_uint = strtoul(str, NULL, 10);
// return 0;
// case OA_VALUE_INT:
// v.value_int = atoi(str);
// return 0;
// case OA_VALUE_FLOAT:
// v.value_f = atof(str);
// return 0;
// case OA_VALUE_DOUBLE:
// v.value_d = atof(str);
// return 0;
// case OA_VALUE_UNSIGNED_LONG_LONG:
// v.value_ull = strtoull(str, NULL, 10);
// return 0;
// case OA_VALUE_LONG_LONG:
// v.value_ll = atoll(str);
// return 0;
// default:
// return -1;
// }

// }

void upgrade_unit(char * unit)
{
    if (NULL == unit)
    {
        return;
    }
    switch (unit[0])
    {
        case 'K':
        case 'k':
            unit[0] = 'M';
            break;
        case 'M':
        case 'm':
            unit[0] = 'G';
            break;
        case 'G':
        case 'g':
            unit[0] = 'T';
            break;
            // case 'T':
            // case 't':
        default:
            memmove(unit + 1, unit, strlen(unit) + 1);
            unit[0] = 'K';

    }
}


int compact_value_unit(double value, const char * orig_unit, char * buf, const uint32_t buf_len)
{
    char unit[MAX_METRIC_UNIT_LEN] = {0};
    memcpy(unit, orig_unit, MAX_METRIC_UNIT_LEN);
    while (value > 1024)
    {
        value /= 1024.0;
        upgrade_unit(unit);
    }


    snprintf(buf, buf_len, "%.2lf %s", value, unit);
    return 0;
}

