/**
 * @file daemon.h
 * @brief
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-09-01
 */
#ifndef _H_DAEMON_H_
#define _H_DAEMON_H_

#include "i_ini_file.h"

int daemon_start(int argc, char** argv, i_ini_file* p_ini_file);
void daemon_stop();
void daemon_set_title(const char* fmt, ...);
int set_cpu_affinity(int proc_num, pid_t pid, u_int id);
extern void restart_worker_proc(int pid);

#endif

