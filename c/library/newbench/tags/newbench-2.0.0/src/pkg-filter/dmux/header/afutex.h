/**
 * @file afutex.h
 * @brief use atomic in user space and syscall futex in syscall
 * @author baron baron@taomee.com
 * @version 1.0
 * @date 2010-07-06
 */

#ifndef __AFUTEX_H__
#define __AFUTEX_H__

#include <unistd.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <stdio.h>
#include <errno.h>

#define AFUTEX_UNLOCKED     0
#define AFUTEX_NO_WAITERS   1
#define AFUTEX_HAS_WAITERS  2

#define AFUTEX_OP_WAIT      0
#define AFUTEX_OP_WAKE      1
#define AFUTEX_OP_WAKE_ALL  2

/**
 * @brief 初始化整形互斥量
 *
 * @param ptr   指向整形互斥量
 */
#define afutex_mutex_init(ptr) do { \
    *(ptr) = 0; \
} while(0)

/**
 * @brief 尝试加锁
 *
 * @param p_mutex 指向整形互斥量的指针
 *
 * @return 0-成功 -1-失败
 */
static inline int afutex_trylock(int *p_mutex)
{
    if (__sync_val_compare_and_swap(p_mutex, 0, 1))
    {
        // 未成功获取锁
        return -1;
    }

    // 成功获取锁
    return 0;
}

/**
 * @brief 加锁
 *
 * @param p_mutex 指向整形互斥量的指针
 *
 * @return 0-成功
 */
static inline int afutex_lock(int *p_mutex)
{
    int val = 0;

    if ((val = __sync_val_compare_and_swap(p_mutex, 0, 1)))
    {
        // 尚未获得锁
        if (val != 2)
        {
            val = __sync_lock_test_and_set(p_mutex, 2);
        }

        while (val)
        {
            // 尚未获得锁,进入等待
            syscall(SYS_futex, p_mutex, FUTEX_WAIT, 2, NULL, NULL, 0);
            // 尝试加锁
            val = __sync_lock_test_and_set(p_mutex, 2);
        }
    }

    return 0;
}

/**
 * @brief 解锁操作
 *
 * @param p_mutex   指向整形互斥量的指针
 * @param n2wake    欲唤醒的对象个数
 *
 * @return 0-成功
 */
static inline int afutex_unlock(int *p_mutex, int n2wake = 1)
{
    if (__sync_fetch_and_sub(p_mutex, 1) != 1)
    {
        __sync_lock_release(p_mutex);
        syscall(SYS_futex, p_mutex, FUTEX_WAKE, n2wake);
    }

    return 0;
}

/**
 * @brief afutex操作结构体变量类型
 */
typedef struct afutex_opbuf {
    int mutex;
    int cnter;
} afutex_opbuf_t;

/**
 * @brief 初始化afutex操作结构体变量
 *
 * @param p_afutex_opbuf afutex操作结构体变量
 */
static inline void afutex_opbuf_init(afutex_opbuf_t *p_afutex_opbuf)
{
    p_afutex_opbuf->mutex = 1;
    p_afutex_opbuf->cnter = 0;
    return;
}

/**
 * @brief futex系列操作封装
 *
 * @param p_afutex_opbuf    指向操作变量结构体
 * @param op_flag           标志位决定操作类型
 * @param p_tsp             等待时间结构体
 *
 * @return 0-成功 -1-失败
 */
static inline int afutex_op(struct afutex_opbuf *p_afutex_opbuf,
                            int op_flag,
                            const struct timespec *p_tsp = NULL)
{
    switch (op_flag)
    {
        case AFUTEX_OP_WAIT:
            __sync_fetch_and_add(&p_afutex_opbuf->cnter, 1);

            if (-1 == syscall(SYS_futex,
                                    &p_afutex_opbuf->mutex,
                                    FUTEX_WAIT,
                                    1,
                                    p_tsp,
                                    NULL,
                                    0)
                && ETIMEDOUT == errno)
            {
                __sync_fetch_and_sub(&p_afutex_opbuf->cnter, 1);
            }

            break;
        case AFUTEX_OP_WAKE:
            if (p_afutex_opbuf->cnter > 0)
            {
                int ret = syscall(SYS_futex,
                                  &p_afutex_opbuf->mutex,
                                  FUTEX_WAKE,
                                  1);

                if (ret > 0)
                {
                    __sync_fetch_and_sub(&p_afutex_opbuf->cnter, ret);
                }
            }

            break;
        case AFUTEX_OP_WAKE_ALL:
            if (p_afutex_opbuf->cnter > 0)
            {
                int ret = syscall(SYS_futex,
                                  &p_afutex_opbuf->mutex,
                                  FUTEX_WAKE,
                                  p_afutex_opbuf->cnter);

                if (ret > 0)
                {
                    __sync_fetch_and_sub(&p_afutex_opbuf->cnter, ret);
                }
            }

            break;
        default:
            return -1;
    }

    return 0;
}

#endif //!__AFUTEX_H__
