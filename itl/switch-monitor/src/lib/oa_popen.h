#ifndef  OA_POPEN_h
#define  OA_POPEN_H

#include <stdio.h>
#include <unistd.h>

extern int *child_stderr_array;//保存子进程错误终端fd的数组

#ifndef WEXITSTATUS
#define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif

#ifndef WIFEXITED
#define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

#if defined(SIG_IGN) && !defined(SIG_ERR)
#define	SIG_ERR	((Sigfunc *)-1)
#endif

int   oa_popen_init();
void  oa_popen_uninit();
FILE *oa_popen(const char *);
int   oa_pclose(FILE *);
int   open_max(void);
void  popen_sigchld_handler(int);

#endif
