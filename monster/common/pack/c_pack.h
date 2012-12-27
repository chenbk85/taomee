/**
 * =====================================================================================
 *       @file  c_pack.h
 *      @brief
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  07/19/2011 05:00:37 PM
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2011, TaoMee.Inc, ShangHai.
 *
 *     @author  luis (程龙), luis@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef H_C_PACK_H_2011_07_19
#define H_C_PACK_H_2011_07_19

#include <stdint.h>

extern "C"
{
#include <libtaomee/log.h>
}

#include "../constant.h"

//传递给pack.begin的参数
//给服务端发送的数据，不需要转字节序
#define SEND_TO_SVR false
//给as发送的数据，需要转字节序
#define SEND_TO_AS true

class c_pack
{
public:
    c_pack();
    ~c_pack();

    //change_byteorder设置begin和end之间的pack要不要转字节序，默认是不转字节序的
    //如果p_buffer不是null，则数据打包到用户提供的缓存里，否则用c_pack自己的缓存
    void begin(bool change_byteorder, char *p_buffer = NULL);

    //len设置打包消息的长度，默认返回已打包数据的长度
    void end(int len = 0);

    int get_msg_len();

    const char *get_msg();

    /**
     * @brief pack
     *
     * @param val
     * @param pos pos = 0直接在当前m_cur_pos后面pack  否则更改pos位置的值
     *
     * @return
     */
    int pack(uint8_t val, int pos = 0);

    int pack(uint16_t val, int pos = 0);

    int pack(uint32_t val, int pos = 0);

    int pack(const char *p_data, int len);

private:
    char m_msg_buffer[MAX_SEND_PKG_SIZE];
    char *m_p_msg_buffer;
    int m_cur_pos;
    int m_set_pos;
    bool m_change_byteorder;
};

inline int c_pack::get_msg_len()
{ 
    return m_cur_pos;
}

inline const char *c_pack::get_msg()
{ 
    return m_p_msg_buffer;
}

#endif //H_C_PACK_H_2011_07_19

