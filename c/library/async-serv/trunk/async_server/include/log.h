/**
 *============================================================
 *  @file      log.h
 *  @brief     用于记录日志，一共分9种日志等级。日志文件可以自动轮转。注意：如果使用自动轮转，\n
 *             必须保证每天写的日志文件个数不能超过log_init时设定的最大文件个数，否则日志会写乱掉。
 *             必须先调用log_init/log_init_t来初始化日志功能。注意，每条日志不能超过8000字节。\n
 *             如果编译程序时定义宏LOG_USE_SYSLOG，则会利用syslog来记录日志，使用的facility是LOG_USER。
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef LIBTAOMEE_LOG_H_
#define LIBTAOMEE_LOG_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @typedef log_lvl_t
 * @brief   typedef of log_lvl
 */

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

/**
 * @typedef log_dest_t
 * @brief   typedef of log_dest
 */

/**
 * @enum  log_dest
 * @brief 日志输出方式
 */
typedef enum log_dest {
	/*! 仅输出到屏幕  */
	log_dest_terminal	= 1,
	/*! 仅输出到文件 */
	log_dest_file		= 2,
	/*! 既输出到屏幕，也输出到文件 */
	log_dest_both		= 3
} log_dest_t;

/**
  * @brief 初始化日志记录功能。如果使用自动轮转，必须保证每天写的日志文件个数不能超过最大文件个数（maxfiles），
  *        否则日志会写乱掉。
  *
  * @param dir 日志保存目录。如果填0，则在屏幕中输出日志。
  * @param lvl 日志输出等级。如果设置为log_lvl_notice，则log_lvl_notice以上等级的日志都不输出。
  * @param size 每个日志文件的大小限制（byte），超过这个大小则自动创建一个新的日志文件。
  * @param maxfiles 每种等级的日志的最大文件个数，用于控制日志轮转。个数越少，日志轮转时效率越高。
  *                 如果填0，则表示不使用日志轮转功能。
  * @param pre_name 日志文件名前缀。
  *
  * @see set_log_dest, log_init_t, enable_multi_thread
  *
  * @return 成功返回0，失败返回-1。
  */
int  log_init(const char* dir, log_lvl_t lvl, uint32_t size, int maxfiles, const char* pre_name);

/**
  * @brief 初始化日志记录功能。按时间周期创建新的日志文件。
  *
  * @param dir 日志保存目录。如果填0，则在屏幕中输出日志。
  * @param lvl 日志输出等级。如果设置为log_lvl_notice，则log_lvl_notice以上等级的日志都不输出。
  * @param pre_name 日志文件名前缀。
  * @param logtime 每个日志文件保存logtime分钟的日志，最大不能超过30000000分钟。假设logtime为15，则每小时产生4个日志文件，每个文件保存15分钟日志。
  *
  * @see set_log_dest, log_init, enable_multi_thread
  *
  * @return 成功返回0，失败返回-1。
  */
int log_init_t(const char* dir, log_lvl_t lvl, const char* pre_name, int logtime);

/**
  * @brief 销毁日志记录功能，所有打开的日志文件fd都会被关闭。不再需要日志功能时，可以调用这个函数。
  *        或者当你想重新设置日志目录、日志等级等参数数，可以先调用该函数，然后调用log_init或log_init_t。
  */
void log_fini();

/**
 * @brief 调用log_init初始化日志功能后，可以调用该函数动态调整日志的输出方式。如果不调用set_log_dest的话，
 *        则输出方式为log_init时确定的方式。注意：必须在log_init时指定了日志保存目录，才可以调用该函数。
 *
 * @param dest 日志输出方式
 *
 * @see log_init
 */
void set_log_dest(log_dest_t dest);
/**
 * @brief 这个写日志的库默认不支持多线程！多线程程序写日志的话，需要调用先一下这个函数，否则会有问题。
 *
 */
void enable_multi_thread();

#ifdef __GNUC__
#define LOG_CHECK_FMT(a,b) __attribute__((format(printf, a, b)))
#else
#define LOG_CHECK_FMT(a,b)
#endif

void write_log(int lvl,uint32_t key, const char* fmt, ...) LOG_CHECK_FMT(3, 4);
void write_syslog(int lvl, const char* fmt, ...) LOG_CHECK_FMT(2, 3);
void boot_log(int ok, int dummy, const char* fmt, ...) LOG_CHECK_FMT(3, 4);

#ifndef LOG_USE_SYSLOG
#define DETAIL(level, key, fmt, args...) \
		write_log(level, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#else
#define DETAIL(level, key, fmt, args...) \
		write_syslog(level, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#endif

#ifndef LOG_USE_SYSLOG
#define SIMPLY(level, key, fmt, args...) \
		write_log(level, key, fmt "\n", ##args)
#else
#define SIMPLY(level, key, fmt, args...) \
		write_syslog(level, fmt "\n", ##args)
#endif

/**
 * @def EMERG_LOG
 * @brief 输出log_lvl_emerg等级日志。如果定义宏DISABLE_EMERG_LOG，则可以在编译期把EMERG_LOG去除。\n
 *        用法示例：EMERG_LOG("dlopen error, %s", error);
 */
/**
 * @def KEMERG_LOG
 * @brief 输出log_lvl_emerg等级日志。比EMERG_LOG多一个参数 key,一般写入userid。如果定义宏DISABLE_EMERG_LOG，则可以在编译期把KEMERG_LOG去除。\n
 *        用法示例：KEMERG_LOG(userid, "dlopen error, %s", error);
 */
#ifndef DISABLE_EMERG_LOG
#define EMERG_LOG(fmt, args...) \
		DETAIL(log_lvl_emerg, 0, fmt, ##args)
#define KEMERG_LOG(key, fmt, args...) \
		DETAIL(log_lvl_emerg, key, fmt, ##args)
#else
#define EMERG_LOG(fmt, args...)
#define KEMERG_LOG(key, fmt, args...) 
#endif

/**
 * @def ALERT_LOG
 * @brief 输出log_lvl_alert等级日志。如果定义宏DISABLE_ALERT_LOG，则可以在编译期把ALERT_LOG去除。\n
 *        用法示例：ALERT_LOG("dlopen error, %s", error);
 */
/**
 * @def KALERT_LOG
 * @brief 输出log_lvl_alert等级日志。比ALERT_LOG多一个参数 key,一般写入userid。如果定义宏DISABLE_ALERT_LOG，则可以在编译期把KALERT_LOG去除。\n
 *        用法示例：KALERT_LOG(userid, "dlopen error, %s", error);
 */

#ifndef DISABLE_ALERT_LOG
#define ALERT_LOG(fmt, args...) \
		DETAIL(log_lvl_alert, 0, fmt, ##args)
#define KALERT_LOG(key, fmt, args...) \
		DETAIL(log_lvl_alert, key, fmt, ##args)
#else
#define ALERT_LOG(fmt, args...)
#define KALERT_LOG(key, fmt, args...) 
#endif

/**
 * @def CRIT_LOG
 * @brief 输出log_lvl_crit等级日志。如果定义宏DISABLE_CRIT_LOG，则可以在编译期把CRIT_LOG去除。\n
 *        用法示例：CRIT_LOG("dlopen error, %s", error);
 */

/**
 * @def KCRIT_LOG
 * @brief 输出log_lvl_crit等级日志。比CRIT_LOG多一个参数 key,一般写入userid。如果定义宏DISABLE_CRIT_LOG，则可以在编译期把KCRIT_LOG去除。\n
 *        用法示例：KCRIT_LOG(userid, "dlopen error, %s", error);
 */

#ifndef DISABLE_CRIT_LOG
#define CRIT_LOG(fmt, args...) \
		DETAIL(log_lvl_crit, 0, fmt, ##args)
#define KCRIT_LOG(key, fmt, args...) \
		DETAIL(log_lvl_crit, key, fmt, ##args)
#else
#define CRIT_LOG(fmt, args...)
#define KCRIT_LOG(key, fmt, args...) 
#endif

/**
 * @def ERROR_LOG
 * @brief 输出log_lvl_error等级日志。如果定义宏DISABLE_ERROR_LOG，则可以在编译期把ERROR_LOG去除。\n
 *        用法示例：ERROR_LOG("dlopen error, %s", error);
 */
/**
 * @def KERROR_LOG
 * @brief 输出log_lvl_error等级日志。比ERROR_LOG多一个参数 key,一般写入userid。 如果定义宏DISABLE_ERROR_LOG，则可以在编译期把ERROR_LOG去除。\n
 *        用法示例：KERROR_LOG(userid, "dlopen error, %s", error);
 */

#ifndef DISABLE_ERROR_LOG
#define ERROR_LOG(fmt, args...) \
		DETAIL(log_lvl_error, 0, fmt, ##args)
#define KERROR_LOG(key, fmt, args...) \
		DETAIL(log_lvl_error, key, fmt, ##args)
#else
#define ERROR_LOG(fmt, args...)
#define KERROR_LOG(key, fmt, args...) 
#endif

/**
 * @def WARN_LOG
 * @brief 输出log_lvl_warning等级日志。如果定义宏DISABLE_WARN_LOG，则可以在编译期把WARN_LOG去除。\n
 *        用法示例：WARN_LOG("dlopen error, %s", error);
 */

/**
 * @def KWARN_LOG
 * @brief 输出log_lvl_warning等级日志。比WARN_LOG多一个参数 key,一般写入userid。如果定义宏DISABLE_WARN_LOG，则可以在编译期把KWARN_LOG去除。\n
 *        用法示例：KWARN_LOG(userid, "dlopen error, %s", error);
 */

#ifndef DISABLE_WARN_LOG
#define WARN_LOG(fmt, args...) \
		SIMPLY(log_lvl_warning, 0, fmt, ##args)
#define KWARN_LOG(key, fmt, args...) \
		SIMPLY(log_lvl_warning, key, fmt, ##args)
#else
#define WARN_LOG(fmt, args...)
#define KWARN_LOG(key, fmt, args...) 
#endif

/**
 * @def NOTI_LOG
 * @brief 输出log_lvl_notice等级日志。如果定义宏DISABLE_NOTI_LOG，则可以在编译期把NOTI_LOG去除。\n
 *        用法示例：NOTI_LOG("dlopen error, %s", error);
 */
/**
 * @def KNOTI_LOG
 * @brief 输出log_lvl_notice等级日志。比NOTI_LOG多一个参数 key,一般写入userid。 如果定义宏DISABLE_NOTI_LOG，则可以在编译期把KNOTI_LOG去除。\n
 *        用法示例：KNOTI_LOG(userid, "dlopen error, %s", error);
 */

#ifndef DISABLE_NOTI_LOG
#define NOTI_LOG(fmt, args...) \
		SIMPLY(log_lvl_notice, 0, fmt, ##args)
#define KNOTI_LOG(key, fmt, args...) \
		SIMPLY(log_lvl_notice, key, fmt, ##args)
#else
#define NOTI_LOG(fmt, args...)
#define KNOTI_LOG(key, fmt, args...) 
#endif

/**
 * @def INFO_LOG
 * @brief 输出log_lvl_info等级日志。如果定义宏DISABLE_INFO_LOG，则可以在编译期把INFO_LOG去除。\n
 *        用法示例：INFO_LOG("dlopen error, %s", error);
 */
/**
 * @def KINFO_LOG
 * @brief 输出log_lvl_info等级日志。比ERROR_LOG多一个参数 key,一般写入userid。如果定义宏DISABLE_INFO_LOG，则可以在编译期把KINFO_LOG去除。\n
 *        用法示例：KINFO_LOG(userid, "dlopen error, %s", error);
 */

#ifndef DISABLE_INFO_LOG
#define INFO_LOG(fmt, args...) \
		SIMPLY(log_lvl_info, 0, fmt, ##args)
#define KINFO_LOG(key, fmt, args...) \
		SIMPLY(log_lvl_info, key, fmt, ##args)
#else
#define INFO_LOG(fmt, args...)
#define KINFO_LOG(key, fmt, args...) 
#endif

/**
 * @def DEBUG_LOG
 * @brief 输出log_lvl_debug等级日志。如果定义宏DISABLE_DEBUG_LOG，则可以在编译期把DEBUG_LOG去除。\n
 *        用法示例：DEBUG_LOG("dlopen error, %s", error);
 */
/**
 * @def KDEBUG_LOG
 * @brief 输出log_lvl_debug等级日志。比DEBUG_LOG 多一个参数 key,一般写入userid。 如果定义宏DISABLE_DEBUG_LOG，则可以在编译期把KDEBUG_LOG去除。\n
 *        用法示例：KDEBUG_LOG(userid, "dlopen error, %s", error);
 */

#ifndef DISABLE_DEBUG_LOG
#define DEBUG_LOG(fmt, args...) \
		SIMPLY(log_lvl_debug, 0, fmt, ##args)
#define KDEBUG_LOG(key, fmt, args...) \
		SIMPLY(log_lvl_debug, key, fmt, ##args)
#else
#define DEBUG_LOG(fmt, args...)
#define KDEBUG_LOG(key, fmt, args...) 
#endif

/**
 * @def TRACE_LOG
 * @brief 输出log_lvl_trace等级日志。如果不定义宏ENABLE_TRACE_LOG，则可以在编译期把TRACE_LOG去除。\n
 *        如果编译时定义了宏LOG_USE_SYSLOG，则TRACE_LOG日志会写到DEBUG_LOG日志文件里。\n
 *        用法示例：TRACE_LOG("dlopen error, %s", error);
 */
/**
 * @def KTRACE_LOG
 * @brief 输出log_lvl_trace等级日志。比KTRACE_LOG多一个参数 key,一般写入userid。如果不定义宏ENABLE_TRACE_LOG，则可以在编译期把KTRACE_LOG去除。\n
 *        如果编译时定义了宏LOG_USE_SYSLOG，则KTRACE_LOG日志会写到KDEBUG_LOG日志文件里。\n
 *        用法示例：KTRACE_LOG(userid, "dlopen error, %s", error);
 */
#ifdef ENABLE_TRACE_LOG
#define TRACE_LOG(fmt, args...) \
		DETAIL(log_lvl_trace, 0, fmt, ##args)
#define KTRACE_LOG(key, fmt, args...) \
		DETAIL(log_lvl_trace, key, fmt, ##args)
#else
#define TRACE_LOG(fmt, args...)
#define KTRACE_LOG(key, fmt, args...) 
#endif

/**
 * @def BOOT_LOG
 * @brief 输出程序启动日志到屏幕。如果OK非0，则退出程序；如果OK为0，则返回上一级函数。\n
 *        用法示例：BOOT_LOG(-1, "dlopen error, %s", error);
 */
#define BOOT_LOG(OK, fmt, args...) \
		do { \
			boot_log(OK, 0, fmt, ##args); \
			return OK; \
		} while (0)

/**
 * @def BOOT_LOG2
 * @brief 输出程序启动日志到屏幕。如果OK非0，则退出程序；如果OK为0，则返回上一级函数。n是空格填充个数。\n
 *        用法示例：BOOT_LOG2(0, 8, "dlopen ok");
 */
#define BOOT_LOG2(OK, n, fmt, args...) \
		do { \
			boot_log(OK, n, fmt, ##args); \
			return OK; \
		} while (0)

/**
 * @def ERROR_RETURN
 * @brief 输出log_lvl_error等级的日志，并且返回Y到上一级函数。\n
 *        用法示例：ERROR_RETURN(("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno)), -1);
 */
#define ERROR_RETURN(X, Y) \
		do { \
			ERROR_LOG X; \
			return Y; \
		} while (0)

/**
 * @def ERROR_RETURN_VOID
 * @brief 输出log_lvl_error等级的日志，并且返回上一级函数。\n
 *        用法示例：ERROR_RETURN("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno));
 */
#define ERROR_RETURN_VOID(fmt, args...) \
		do { \
			ERROR_LOG(fmt, ##args); \
			return; \
		} while (0)

/**
 * @def WARN_RETURN
 * @brief 输出log_lvl_warning等级的日志，并且返回ret_到上一级函数。\n
 *        用法示例：WARN_RETURN(("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno)), -1);
 */
#define WARN_RETURN(msg_, ret_) \
		do { \
			WARN_LOG msg_; \
			return (ret_); \
		} while (0)

/**
 * @def WARN_RETURN_VOID
 * @brief 输出log_lvl_warning等级的日志，并且返回上一级函数。\n
 *        用法示例：WARN_RETURN_VOID("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno));
 */
#define WARN_RETURN_VOID(fmt, args...) \
		do { \
			WARN_LOG(fmt, ##args); \
			return; \
		} while (0)

/**
 * @def DEBUG_RETURN
 * @brief 输出log_lvl_debug等级的日志，并且返回ret_到上一级函数。\n
 *        用法示例：DEBUG_RETURN(("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno)), -1);
 */
#define DEBUG_RETURN(msg_, ret_) \
		do { \
			DEBUG_LOG msg_; \
			return (ret_); \
		} while (0)

/**
 * @def DEBUG_RETURN_VOID
 * @brief 输出log_lvl_debug等级的日志，并且返回上一级函数。\n
 *        用法示例：DEBUG_RETURN_VOID("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno));
 */
#define DEBUG_RETURN_VOID(fmt, args...) \
		do { \
			DEBUG_LOG(fmt, ##args); \
			return; \
		} while (0)

#ifdef __cplusplus
}
#endif

#endif // LIBTAOMEE_LOG_H_
