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
#ifndef H_UTILITY_H_2010_08_24
#define H_UTILITY_H_2010_08_24

#include <stdint.h>
#include <ctype.h>
#include <string.h>

/**
 * @brief 格式化时间戳, 得到的格式为110803
 *
 * @param timestamp 时间戳
 *
 * @return -1 failed, >0格式化的时间
 */
int timestamp2day(uint32_t timestamp);

/**
 * @brief 比较2个时间戳的时间是否在同一天
 *
 * @param timestamp1 时间戳
 * @param timestamp2 时间戳
 *
 * @return -1 failed, 1 同一天 0 不是同一天
 */
int timestamp_equal_day(uint32_t timestamp1, uint32_t timestamp2);

/**
 * @brief 获得一天开始的时间戳
 *
 * @return 0 failed, > 0 时间戳
 */
uint32_t timestamp_begin_day();

/**
 * @brief 生成min和max之间的一个随机值
 *
 * @param min
 * @param max
 *
 * @return -1 failed, 1 同一天 0 不是同一天
 */
int uniform_rand(int min, int max);

/**
 * @brief 判断一个字符串是否是数字
 *
 * @param p_str:要检查的字符串 
 *
 * @return  true:是数字，false:不是数字
 */
bool string_is_digit(char *p_str);


inline uint32_t get_utc_second_ex(const char *input_date)
{
    if(strlen(input_date) != 19)
    {
        return 0;
    }

    struct tm _tm = {0};
    sscanf(input_date, "%d/%d/%d %d:%d:%d", &(_tm.tm_year), &(_tm.tm_mon), &(_tm.tm_mday), &(_tm.tm_hour), &(_tm.tm_min), &(_tm.tm_sec) );
    _tm.tm_year -= 1900;
    _tm.tm_mon -= 1;

    return (uint32_t)mktime(&(_tm));
}


/** @brief 获得服务端序列号 */
#define GET_SVR_SN(p_user) (uint32_t)((p_user->session->fd << 16) | p_user->counter)

/** @brief 检验包长 */
#define CHECK_VAL_LE(uid_, val_, max_, ret_) \
    if ((val_) > (max_)) { KERROR_LOG((uid_), "val %d is greater than max %d", (val_), (max_)); g_errno = ERR_MSG_LEN; return (ret_); } 
/** @brief 检验包长 */
#define CHECK_VAL_GE(uid_, val_, min_, ret_) \
    if ((val_) < (min_)) { KERROR_LOG((uid_), "val %d is less than min %d", (val_), (min_)); g_errno = ERR_MSG_LEN; return (ret_); } 
/** @brief 检验包长 */
#define CHECK_VAL(uid_, val_, expect_, ret_) \
    if ((val_) != (expect_)) { KERROR_LOG((uid_), "val %d is not equal to expect %d", (val_), (expect_)); g_errno = ERR_MSG_LEN; return (ret_); } 

#endif //H_UTILITY_H_2010_08_24
