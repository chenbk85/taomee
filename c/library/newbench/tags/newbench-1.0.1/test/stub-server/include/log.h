#ifndef _LOG_H
#define _LOG_H
#include <sys/cdefs.h>
#include <sys/types.h>
#include <arpa/inet.h>

#undef __attr_cdecl__
#define __attr_cdecl__ __attribute__((__cdecl__))

#ifndef likely
#define likely(x)  __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x)  __builtin_expect(!!(x), 0)
#endif

/**
 * @enum  log_lvl
 * @brief 日志等级
 */
typedef enum log_lvl {
    /*! system is unusable -- 0 */
    log_lvl_emerg,
    /*! action must be taken immediately -- 1 */
    log_lvl_alert,
    /*! critical conditions -- 2 */
    log_lvl_crit,
    /*! error conditions -- 3 */
    log_lvl_error,
    /*! warning conditions  -- 4 */
    log_lvl_warning,
    /*! normal but significant condition -- 5 */
    log_lvl_notice,
    /*! informational -- 6 */
    log_lvl_info,
    /*! debug-level messages -- 7 */
    log_lvl_debug,
    /*! trace-level messages -- 8。如果定义了宏LOG_USE_SYSLOG，则log_lvl_trace==log_lvl_debug */
#ifndef LOG_USE_SYSLOG
    log_lvl_trace,
#else
    log_lvl_trace = log_lvl_debug,
#endif

    log_lvl_max
} log_lvl_t;

/*
 * log api
 */
int log_init (const char* dir, int level, u_int size, const char* fix_name);

void write_log (int lvl, char const* fmt, ...);

void boot_log (int OK, int dummy, const char* fmt, ...);


#define APP_EMERG 		0  /* system is unusable               */
#define APP_ALERT		1  /* action must be taken immediately */
#define APP_CRIT		2  /* critical conditions              */
#define APP_ERROR		3  /* error conditions                 */
#define APP_WARNING		4  /* warning conditions               */
#define APP_NOTICE		5  /* normal but significant condition */
#define APP_INFO		6  /* informational                    */
#define APP_DEBUG		7  /* debug-level messages             */
#define APP_TRACE		8  /* trace-level messages             */

#define DETAIL(level, fmt, args...) \
	write_log (level, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)

#define SIMPLY(level, fmt, args...) write_log(level, fmt "\n", ##args)

#define ERROR_LOG(fmt, args...)	DETAIL(APP_ERROR, fmt, ##args)
#define CRIT_LOG(fmt, args...)	DETAIL(APP_CRIT, fmt, ##args)
#define ALERT_LOG(fmt, args...)	DETAIL(APP_ALERT, fmt, ##args)
#define EMERG_LOG(fmt, args...)	DETAIL(APP_EMERG, fmt, ##args)

#define WARN_LOG(fmt, args...)	SIMPLY(APP_WARNING, fmt, ##args)
#define NOTI_LOG(fmt, args...)	SIMPLY(APP_NOTICE, fmt, ##args)
#define INFO_LOG(fmt, args...)	SIMPLY(APP_INFO, fmt, ##args)
#define DEBUG_LOG(fmt, args...)	SIMPLY(APP_DEBUG, fmt, ##args)
#define BOOT_LOG(OK, fmt, args...) do{ \
	boot_log(OK, 0, fmt, ##args); \
	return OK; \
}while (0)

#define BOOT_LOG2(OK, n, fmt, args...) do{ \
	boot_log(OK, n, fmt , ##args); \
	return OK; \
}while (0)

#define ERROR_RETURN(X, Y) do{ \
	ERROR_LOG X; \
	return Y; \
}while (0)

#ifdef DEBUG
#define TRACE_LOG(fmt,args...)	SIMPLY(APP_TRACE, fmt, ##args)
#else
#define TRACE_LOG(fmt,args...)
#endif


#endif
