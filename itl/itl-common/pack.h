/** 
 * ========================================================================
 * @file pack.h
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-08-16
 * Modify $Date: 2012-08-16 11:23:21 +0800 (四, 16  8月 2012) $
 * Modify $Author: smyang $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#ifndef H_PACK_H_2012_08_16
#define H_PACK_H_2012_08_16

#include <libtaomee++/inet/pdumanip.hpp>


inline void pack32(void * pkg, uint32_t v, int & idx)
{
    taomee::pack_h(pkg, v, idx);
}


inline void pack16(void * pkg, uint16_t v, int & idx)
{
    taomee::pack_h(pkg, v, idx);
}


inline void pack8(void * pkg, uint8_t v, int & idx)
{
    taomee::pack_h(pkg, v, idx);
}


inline void pack_string(void * pkg, const char * str, uint32_t max_len, int & idx)
{
    if (0 == max_len)
    {
        return;
    }

    uint32_t len = strlen(str);
    if (len >= max_len)
    {
        len = max_len - 1;
    }

    taomee::pack_h(pkg, len + 1, idx);
    taomee::pack(pkg, str, len, idx);
    pack8(pkg, 0, idx);

}

inline void unpack_string(const void * pkg, char * str, uint32_t max_len, int & idx)
{
    uint32_t len = 0;
    taomee::unpack_h(pkg, len, idx);
    if (len > max_len)
    {
        len = max_len;
    }

    taomee::unpack(pkg, str, len, idx);
    str[len - 1] = 0;
}





#endif
