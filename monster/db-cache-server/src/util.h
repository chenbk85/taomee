/*
 * =====================================================================================
 *
 *       Filename:  util.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年07月20日 15时44分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_UTIL_H_20110720
#define H_UTIL_H_20110720
#include <stdint.h>
extern "C"
{
#include <libtaomee/log.h>
}

#define DB_ID(user_id) (user_id) % 10
#define TABLE_ID(user_id) ((user_id) / 100 ) % 100


#define INITIAL_EXPERIENCE 0
#define INITIAL_LEVEL   1
#define INITIAL_COINS   10

int str2uint(uint32_t *p_uint, const char *p_str);
int str2uint(uint16_t *p_uint, const char *p_str);
int str2uint(uint8_t *p_uint, const char *p_str);

int intip2str(uint32_t ip, char *ipstr, int ipstr_len);

inline int check_val_len(uint16_t msg_type, uint32_t recv_len, uint32_t need_len)
{
    if(recv_len != need_len)
    {
        ERROR_LOG("msg[%u]: length not consistant(recved_len:%u need_len:%u).", msg_type, recv_len, need_len);
        return -1;
    }
    return 0;
}

/**
 * @brief 生成min和max之间的一个随机值
 *
 * @param min
 * @param max
 *
 * @return -1 failed, 1 同一天 0 不是同一天
 */
int uniform_rand(int min, int max);

inline uint32_t timestamp_begin_day(uint32_t time_now)
{
    struct tm ts;
	time_t time_tmp = (uint32_t)time_now;
    if (NULL == localtime_r(&time_tmp, &ts))
    {
        return 0;
    }

    return time_now - ts.tm_hour * 60 * 60 - ts.tm_min * 60 - ts.tm_sec;

}

#endif //H_UTIL_H_20110720
