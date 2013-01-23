/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file newbench_util.h
 * @author richard <richard@taomee.com>
 * @date 2011-07-28
 */

#ifndef NEWBENCH_UTIL_H_2011_07_28
#define NEWBENCH_UTIL_H_2011_07_28

#include <stdint.h>

/* 日志相关部分 */
typedef enum log_lvl {
	log_lvl_emerg,
	log_lvl_alert,
	log_lvl_crit,
	log_lvl_error,
	log_lvl_warning,
	log_lvl_notice,
	log_lvl_info,
	log_lvl_debug,
	log_lvl_trace,
	log_lvl_max
} log_lvl_t;

extern "C" void write_log(int lvl, uint32_t key, const char *fmt, ...) __attribute__((format(printf, 3, 4)));

#define KEMERG_LOG(key, fmt, args...) \
		write_log(log_lvl_emerg, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define KALERT_LOG(key, fmt, args...) \
		write_log(log_lvl_alert, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define KCRIT_LOG(key, fmt, args...) \
		write_log(log_lvl_crit, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define KERROR_LOG(key, fmt, args...) \
		write_log(log_lvl_error, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define KWARN_LOG(key, fmt, args...) \
		write_log(log_lvl_warning, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define KNOTI_LOG(key, fmt, args...) \
		write_log(log_lvl_notice, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define KINFO_LOG(key, fmt, args...) \
		write_log(log_lvl_info, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define KDEBUG_LOG(key, fmt, args...) \
		write_log(log_lvl_debug, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define KTRACE_LOG(key, fmt, args...) \
		write_log(log_lvl_trace, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)

#define EMERG_LOG(fmt, args...) KERMGE_LOG(0, fmt, ##args);
#define ALERT_LOG(fmt, args...) KALERT_LOG(0, fmt, ##args);
#define CRIT_LOG(fmt, args...)  KCRIT_LOG(0, fmt, ##args);
#define ERROR_LOG(fmt, args...) KERROR_LOG(0, fmt, ##args);
#define WARN_LOG(fmt, args...)  KWARN_LOG(0, fmt, ##args);
#define NOTI_LOG(fmt, args...)  KNOTI_LOG(0, fmt, ##args);
#define INFO_LOG(fmt, args...)  KINFO_LOG(0, fmt, ##args);
#define DEBUG_LOG(fmt, args...) KDEBUG_LOG(0, fmt, ##args);
#define TRACE_LOG(fmt, args...) KTRACE_LOG(0, fmt, ##args);

/**
 * @brief 打印提示信息到屏幕
 */
extern void print_prompt(bool success, const char *p_fmt, ...) __attribute__((format(printf, 2, 3)));

/**
 * @brief 设置ps查看时程序的标题
 */
extern void setproctitle(const char *fmt, ...) __attribute__((format(printf, 1, 2)));


/**
 * @brief 发送数据 - 仅用在work进程
 * @param connection_id 连接id
 * @param p_data        指向数据缓冲区
 * @param data_len      数据缓冲区长度
 * @param is_atomic     是否保持发送的原子性(暂无效,都为原子性发送)
 * @param is_broadcast  是否位广播发送
 * @return 0-成功 -1-失败
 */
extern int send_data(int connection_id, const char *p_data, int data_len, int is_atomic, int is_broadcast);

/**
 * @brief 关闭连接
 * @param connection_id 连接id,负数表示所有连接
 * @return 0-成功 -1-失败
 */
extern int close_connection(int connection_id);


/* 配置文件相关 */
/** 
 * @brief 读取字符串
 * @param section 段名字符串
 * @param key 键名字符串
 * @param value 值字符串
 * @param size 值字符串的长度
 * @param default_value 值字符串的默认值
 * @return 0 成功; <0 失败
 */
extern int ini_read_string(const char *section, const char *key,char *value, int size,const char *default_value);

/** 
 * @brief 读取整形
 * @param section 段名字符串
 * @param key 键名字符串
 * @param default_value 默认值
 * @return 读取成功时返回读取的整型值，读取失败时返回默认值
 */
extern int ini_read_int(const char *section, const char *key,int default_value);

#endif /* NEWBENCH_UTIL_H_2011_07_28 */
