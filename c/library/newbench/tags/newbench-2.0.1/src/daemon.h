/* vim: set expandtab tabstop=4 softtabstop=4 shiftwidth=4: */
#ifndef _NB_DAEMON_H_
#define _NB_DAEMON_H_

int daemon_start (int argc, char** argv);
void daemon_stop(void);
void dup_argv(int argc, const char * const argv[], char **saved_argv);
void free_argv(char **saved_argv);

/**
 * @brief 判断程序是否已经有实例正在运行
 * @return 如果已经有实例正在运行返回1，否则返回0，出错时返回-1
 */
int already_running(); 

#endif
