/*
 * =====================================================================================
 *
 *       Filename:  bswap.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2010年08月24日 13时17分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#include <byteswap.h>
#include "bswap.h"

uint8_t bswap(uint8_t x)
{
	return x;
}

uint16_t bswap(uint16_t x)
{
	return bswap_16(x);
}

uint32_t bswap(uint32_t x)
{
	return bswap_32(x);
}
