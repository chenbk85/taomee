/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
#ifndef _NB_UTIL_H_
#define _NB_UTIL_H_

#include "afutex.h"
#include "newbench_so.h"

#define MAX_CONF_STR_LEN 512                               //配置文件中字符串最大值
#define MAX_PACKAGE_LEN (1024 * 1024)

#define NB_COMMON_FREE(var) do{                            \
    if(var != NULL)                                        \
    {                                                      \
        free(var);                                         \
        var = NULL;                                        \
    }                                                      \
} while (0)

#define NB_CLASS_FREE(var)                                 \
    do {                                                   \
        if (var != NULL) {                                 \
            var->release();                                \
            var = NULL;                                    \
        }                                                  \
    } while (0)

#define NB_CHECK_INT(name, value) do{                      \
    if(value <= 0)                                         \
    {                                                      \
        print_prompt(false,                                \
                    "Config error: %s[%d]!", name, value); \
        return -1;                                         \
    }                                                      \
} while (0)

#define AFUTEX_LOCK_ERR_CHK(lock) do {                     \
    int ret = afutex_lock(lock);                           \
    if (ret != 0) {                                        \
        ERROR_LOG("futex lock fail: %d", ret);             \
    }                                                      \
} while (0)

#define AFUTEX_UNLOCK_ERR_CHK(lock) do {                   \
    int ret = afutex_unlock(lock);                         \
    if (ret != 0) {                                        \
        ERROR_LOG("futex unlock fail: %d", ret);           \
    }                                                      \
} while (0)

typedef struct bind_config {
    char    *bind_ip;
    u_short bind_port;
    char    type;
    int timeout;
    struct bind_config *next;
} bind_config_t;

typedef struct bench_config {
///基本信息
    char *prog_name;
    char *current_dir;
    char **saved_argv;
///日志信息
    char log_dir[MAX_CONF_STR_LEN];
    int log_level;
    u_int log_size;
    u_int log_maxfiles;
    char log_prefix[MAX_CONF_STR_LEN];
///绑定信息
    char bind_file[MAX_CONF_STR_LEN];
    struct bind_config *bind_list;
    bool use_barrier;
///工作进程信息
    char so_file[MAX_CONF_STR_LEN];
    u_int shmq_length;
    u_int pkg_timeout;
    u_int worker_num;
    u_int max_pkg_len;
    u_int avg_pkg_len;
} bench_config_t;

int load_bind_file(const char *file_name, struct bind_config **pbind_list);
int mmap_bind_file (const char *file_name, char **buf);
void free_bind_file (struct bind_config *bind_list);
void bind_dump (struct bind_config *bind_list);
int check_bind_config (struct bind_config *bind_list);

void print_prompt(bool success, const char *p_fmt, ...);

#endif
