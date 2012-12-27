/*
 * =====================================================================================
 *
 *       Filename:  utility.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2010年08月24日 13时12分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_BSWAP_H_2010_08_24
#define H_BSWAP_H_2010_08_24

#include <stdint.h>

/**
 * @brief 字节转换函数
 *
 * @param x
 *
 * @return
 */
uint8_t bswap(uint8_t x);
uint16_t bswap(uint16_t x);
uint32_t bswap(uint32_t x);

#endif //H_BSWAP_H_2010_08_24
