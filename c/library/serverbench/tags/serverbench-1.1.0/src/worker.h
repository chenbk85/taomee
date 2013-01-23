#ifndef _WORKER_H_
#define _WORKER_H_
#include "dll.h"

int timeout;

extern int worker_procs_spawn (pid_t conn_pid, int argc, char **argv);
extern void children_monitor ();
#endif
