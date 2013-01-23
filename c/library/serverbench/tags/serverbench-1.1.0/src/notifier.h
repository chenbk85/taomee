#ifndef _NOTIFIER_H_
#define _NOTIFIER_H_
#include "benchapi.h"

extern int __init pipe_create ();
extern inline void wr_pipe_close ();
extern inline void rd_pipe_close ();
extern inline void read_pipe ();
extern inline void write_pipe ();
extern inline int pipe_rd_fd ();
#endif

