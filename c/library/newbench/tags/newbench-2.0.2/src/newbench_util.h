/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file newbench_util.h
 * @author richard <richard@taomee.com>
 * @date 2011-07-28
 */

#ifndef NEWBENCH_UTIL_H_2011_07_28
#define NEWBENCH_UTIL_H_2011_07_28

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

#define EMERG_LOG(fmt, args...) \
		write_log(log_lvl_emerg, 0, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define ALERT_LOG(fmt, args...) \
		write_log(log_lvl_alert, 0, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define CRIT_LOG(fmt, args...) \
		write_log(log_lvl_crit, 0, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define ERROR_LOG(fmt, args...) \
		write_log(log_lvl_error, 0, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define WARN_LOG(fmt, args...) \
		write_log(log_lvl_warning, 0, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define NOTI_LOG(fmt, args...) \
		write_log(log_lvl_notice, 0, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define INFO_LOG(fmt, args...) \
		write_log(log_lvl_info, 0, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define DEBUG_LOG(fmt, args...) \
		write_log(log_lvl_debug, 0, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#define TRACE_LOG(fmt, args...) \
		write_log(log_lvl_trace, 0, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)

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

#endif /* NEWBENCH_UTIL_H_2011_07_28 */
