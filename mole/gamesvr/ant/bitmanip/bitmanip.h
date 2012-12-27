#ifndef ANT_BITMANIP_H_
#define ANT_BITMANIP_H_

// C89
#include <assert.h>
#include <stddef.h>
// C99
#include <stdint.h>

/**
 * @brief 计算一个值有多少位是1, 即打开的位
 * @param val 需要测试的值
 * @return 打开的位的个数
 */
static inline int
nbits_on8(uint8_t val)
{
	val = ((val & 0xAA) >> 1) + (val & 0x55);
	val = ((val & 0xCC) >> 2) + (val & 0x33);
	val = ((val & 0xF0) >> 4) + (val & 0x0F);

	return val;
}

/**
 * @brief generic algorithm to evaluate number of bits on in val_(内部使用)
 */
#define NBITS_ON(val_, nloops_, cnt_) \
	do { \
		int i = 0; \
		for (; i != (nloops_); ++i) { \
			(cnt_) += nbits_on8( *(((uint8_t*)&(val_)) + i) ); \
		} \
	} while (0)

/**
 * @brief 计算一个值有多少位是1, 即打开的位
 * @param val 需要测试的值
 * @return 打开的位的个数
 */
static inline int
nbits_on16(uint16_t val)
{
	int cnt = 0;
	NBITS_ON(val, 2, cnt);

	return cnt;
}

/**
 * @brief 计算一个值有多少位是1, 即打开的位
 * @param val 需要测试的值
 * @return 打开的位的个数
 */
static inline int
nbits_on32(uint32_t val)
{
	int cnt = 0;
	NBITS_ON(val, 4, cnt);

	return cnt;
}

/**
 * @brief 计算一个值有多少位是1, 即打开的位
 * @param val 需要测试的值
 * @return 打开的位的个数
 */
static inline int
nbits_on64(uint64_t val)
{
	int cnt = 0;
	NBITS_ON(val, 8, cnt);

	return cnt;
}

/**
 * @brief 把数组的一个bit位置1
 * @param arr 指定的数组
 * @param len 数组的总长度
 * @param pos bit位的位置
 * @return 无
 */
static inline void
set_bit_on(uint8_t* arr, size_t len, int pos)
{
	pos -= 1;

	int i = pos / 8;
	assert(i < len);

	arr[i] |= (1u << (pos % 8));
}

/**
 * @brief 把整数的一个bit位置1
 * @param val 指定的整数
 * @param pos bit位的位置
 * @return 设置之后的整数值
 */
static inline uint32_t
set_bit_on32(uint32_t val, int pos)
{
	return (val | (1u << (pos - 1)));
}

/**
 * @brief 把整数的一个bit位置1
 * @param val 指定的整数
 * @param pos bit位的位置
 * @return 设置之后的整数值
 */
static inline uint64_t
set_bit_on64(uint64_t val, int pos)
{
	return (val | (1LLu << (pos - 1)));
}

/**
 * @brief 把数组的一个bit位置0
 * @param arr 指定的数组
 * @param len 数组的总长度
 * @param pos bit位的位置
 * @return 无
 */
static inline void
set_bit_off(uint8_t* arr, size_t len, int pos)
{
	pos -= 1;

	int i = pos / 8;
	assert(i < len);

	arr[i] &= ~(1u << (pos % 8));
}

/**
 * @brief 把整数的一个bit位置1
 * @param val 指定的整数
 * @param pos bit位的位置
 * @return 设置之后的整数值
 */
static inline uint32_t
set_bit_off32(uint32_t val, int pos)
{
	return (val & ~(1u << (pos - 1)));
}

/**
 * @brief 把整数的一个bit位置1
 * @param val 指定的整数
 * @param pos bit位的位置
 * @return 设置之后的整数值
 */
static inline uint64_t
set_bit_off64(uint64_t val, int pos)
{
	return (val & ~(1LLu << (pos - 1)));
}

/**
 * @brief 测试数组指定位是否为1
 * @param arr 指定的数组
 * @param len 数组的总长度
 * @param pos bit位的位置
 * @return 如果为1, 则返回1；否则返回0
 */
static inline int
test_bit_on(uint8_t* arr, size_t len, int pos)
{
	pos -= 1;

	int i = pos / 8;
	assert(i < len);

	return !!(arr[i] & (1u << (pos % 8)));
}

/**
 * @brief 测试数值指定位是否为1
 * @param val 指定的数组
 * @param pos bit位的位置
 * @return 如果为1, 则返回1；否则返回0
 */
static inline int
test_bit_on32(uint32_t val, int pos)
{
	return !!(val & (1u << (pos - 1)));
}

/**
 * @brief 测试数值指定位是否为1
 * @param val 指定的数组
 * @param pos bit位的位置
 * @return 如果为1, 则返回1；否则返回0
 */
static inline int
test_bit_on64(uint64_t val, int pos)
{
	return !!(val & (1LLu << (pos - 1)));
}

#endif // ANT_BITMANIP_H_
