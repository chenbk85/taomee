#ifndef _DAEMON_H_
#define _DAEMON_H_

#define MAXFDS	100000
volatile extern int stop;
volatile extern int restart;
extern int pagesize;

extern char *prog_name;
extern char *current_dir;
extern void daemon_stop(void); 
extern int daemon_start ();
extern void daemon_set_title (const char* fmt, ...);

extern void pagesize_init ();
#endif

