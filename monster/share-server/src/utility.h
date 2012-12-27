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
