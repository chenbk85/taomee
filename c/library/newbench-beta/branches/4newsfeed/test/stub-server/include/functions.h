/**
* =====================================================================================
*       @file  functions.h
*      @brief  模块中使用的一些公共函数
*
*  Detailed description starts here.
*
*   @internal
*     Created  01/31/2010 12:41:06 PM
*    Revision  1.0.0.0
*    Compiler  gcc/g++
*     Company  TaoMee.Inc, ShangHai.
*   Copyright  Copyright (c) 2009, TaoMee.Inc, ShangHai.
*
*     @author  jasonwang (王国栋) jasonwang@taomee.com
* This source code was wrote for TaoMee,Inc. ShangHai CN.
* =====================================================================================
*/

#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <sys/types.h>
#include <stdint.h>

/**
* @brief 计算某个数据段的 MD5 哈希值
*
* @param src 需要计算哈希值的数据段
* @param len 数据段的长度
* @param dest 保存32字节的MD5哈希字符串
*
* @return 如果src和dest都不为空，返回转化后的结果，否则返回NULL
*
*/
const char* MD5_32(const char* src, size_t len, char *dest);

/** 
* @brief 将src中每四位转换成一个16进制数字符保存在dest中
* 
* @param src 将要转化的二进制数组
* @param len 将要转化的二进制数组大小
* @param dest 保存转化后的结果数组
* 
* @return 如果src和dest都不为空，返回转化后的结果，否则返回NULL
*/
const char* hex_dump(const uint8_t* src, size_t len, char *dest);

/**
* @brief 将二进制数据转换成 16 进制字符串的形式
*
* @param src 将要转化的二进制数组
* @param len 将要转化的二进制数组大小
* @param dest 保存结果的数组
*
* @return 如果src和dest都不为空，返回转化后的结果，否则返回NULL
*
* @note 函数不保证 dst 以 '\0' 结束，函数不检查 src 的长度是否是 len/2
* @see 
*/
const char* hex2asc_lower(const uint8_t *src, size_t len, char *dest);

/** 
* @brief 将16进制的字符串转化成二进制数
* 
* @param src 16进制字符串数组
* @param len 16进制字符串数组的长度
* @param dest 保存转化后的二进制数组
* 
* @return 如果src和dest都不为空，返回转化后的结果，否则返回NULL
*/
const uint8_t* asc2hex(const char *src, size_t len, uint8_t *dest);

/**
* @fn atoi_safe
* @brief 安全版本的 aoti 函数
*
* @param const char* nptr 数字字符串指针
*
* @return int 如果 nptr 为 NULL，则返回 0，否则返回数字字符串对应的数字
*
*/
int atoi_safe(const char *nptr);

/**
* @fn atol_safe
* @brief 安全版本的 aotl 函数
*
* @param const char* nptr 数字字符串指针
*
* @return int 如果 nptr 为 NULL，则返回 0，否则返回数字字符串对应的数字
*
*/
long atol_safe(const char *nptr);

/**
* @fn atoll_safe
* @brief 安全版本的 aotll 函数
*
* @param const char* nptr 数字字符串指针
*
* @return int 如果 nptr 为 NULL，则返回 0，否则返回数字字符串对应的数字
*
*/
long long atoll_safe(const char *nptr);

/**
* @fn millisleep
* @brief 睡眠指定毫秒数
*
* @param int milli_seconds 毫秒数
*
* @return int 参见 nanosleep 的返回值
*
* @note 
* @see 
*/
int millisleep(int milli_seconds);

/**
* @fn strip_space
* @brief 删除一个字符串中的所有空格
*
* @param char* buf 需要处理的缓冲区
*
* @return char* 处理以后的字符串
*
* @note
* @see 
*/
const char* strip_space(char* buf);

#endif // !__FUNCTIONS_H__
