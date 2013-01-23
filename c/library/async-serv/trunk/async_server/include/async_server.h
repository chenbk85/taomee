#ifndef ASYNC_SERVER_H
#define ASYNC_SERVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    PROC_MAIN = 1,
    PROC_CONN,
    PROC_WORK
};

int config_get_intval(const char *key, int defult);
const char *config_get_strval(const char *key, const char *defult);

uint32_t get_work_id();
const char *get_work_name();
uint32_t get_work_num();
uint32_t get_work_idx();
uint32_t get_idle_work_idx_conn();
// 设置进程名称
void set_prog_name(const char *prog_name);
// 设置进程名称
void set_title(const char* fmt, ...);

int net_connect_ser(const char *ip, unsigned short port, int ms_timeout);
int net_send_ser(int fd, const void *buf, int len);
void net_close_ser(int fd);

int net_send_cli(int fd, const void *buf, int len);
void net_close_cli(int fd);

// 等价于net_send_ser
int net_send_cli_conn(int fd, const void *buf, int len);

/**
 * @brief send_warning_msg 发送告警短信
 *
 * @param warning_content :短信内容
 * @param uid: 米米号
 * @param cmdid 命令号（统计里面可以是msgid）
 * @param ishex 是否十六进制显示(0:十进制)
 * @param ip 发生问题的机器ip(统计这边可以是客户端的ip)
 *
 * @return
 */
void send_warning_msg(const char *warning_content, uint32_t uid, uint32_t cmdid, uint32_t ishex, const char *ip);


#ifdef __cplusplus
} // end of extern "C"
#endif

#endif
