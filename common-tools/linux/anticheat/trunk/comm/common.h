/* 全系统公约 */


#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <confuse.h>

#define	STX	(0x2)
#define	ETX	(0x3)

#define REQUEST_CHANNEL		(0x1)
#define RESPONSE_CHANNEL	(0x2)

#define SET_SOCK_RWMEM_SIZE	(8*1024*1024)
#define DEFAULT_MAX_OPEN_FILES	(150000)

#define MAX_SVC_NAME_LEN	(32)
#define MAX_SVC_NUM			(200)
#define NA_SVC_NAME			"NA_SVC"

#define MAX_DB_USER_LEN			(256)
#define MAX_DB_PASSWD_LEN		(256)
#define MAX_DB_NAME_LEN			(1024)
#define MAX_TAB_NAME_LEN		(1024)

#define AC_CFG_STATUS_DISABLE			(0)
#define AC_CFG_STATUS_ENABLE			(1)

#define MAX_LOG_SIZE		(1<<31)

/*
 * 最多可配置监控的命令数, 注意:
 * 1. 修改这个值会改变 config-shm 的大小, 即: 需要切换 shm;
 * 2. 该值必须保证: SW_LVL_MAX_SPEC >= MAX_CONFIG_SW_CMD;
 * 	  即: 分级存储的最大 SW_LVLn_SPEC 要保证大于可配置的最多的sw数量,
 * 	  这样才能保证分级存储内存能存下业务配置的所有sw的监控;
 */
#define MAX_CONFIG_SW_CMD	(512)


/* 最大业务id编号 (配置中不能超过这个值) */
#define MAX_SVC_GAMEID		(64)


/* 从 libevent 里 copy, 为避免冲突, 把名字稍微改了一点 */
/* True iff e is an error that means a read/write operation can be retried. */
#define SOCK_ERR_RW_RETRIABLE(e)              \
	    ((e) == EINTR || (e) == EAGAIN)
/* True iff e is an error that means an connect can be retried. */
#define SOCK_ERR_CONNECT_RETRIABLE(e)         \
	    ((e) == EINTR || (e) == EINPROGRESS)
/* True iff e is an error that means a accept can be retried. */
#define SOCK_ERR_ACCEPT_RETRIABLE(e)          \
	    ((e) == EINTR || (e) == EAGAIN || (e) == ECONNABORTED)


/* likely and unlikely */
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)


/* offsetof */
#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/* COMPILE ASSERT */
#define __CAT_TOKEN(t1,t2) t1##t2
#define CAT_TOKEN(t1,t2) __CAT_TOKEN(t1,t2)
#define COMPILE_ASSERT(x)  \
	enum { CAT_TOKEN (comp_assert_at_line_, __LINE__) = 1 / !!(x) };

/* DIM */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif /* __COMMON_H__ */
