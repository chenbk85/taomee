#ifndef _NB_DAEMON_H_
#define _NB_DAEMON_H_


int daemon_start (int argc, char** argv);
void daemon_stop(void);
void dup_argv(int argc, char **argv, char **saved_argv);
void free_argv(char **saved_argv);

#endif

