#ifndef COMMON_H
#define COMMON_H

#define ACCEPT_BACKLOG		10

extern int load_dll(const char* dll_name);
extern void init_fdlimit ();
extern void daemon_start ();

typedef int (*init_factory)(int,char**);
typedef void (*fini_factory)();
typedef int (*handle_socket_factory)(int);
typedef int (*handle_accept_factory)(int);
typedef int (*handle_stat_factory)(int,int,int);

extern init_factory app_init;
extern fini_factory app_fini;
extern handle_stat_factory app_stat;
extern handle_socket_factory app_socket;
extern handle_accept_factory app_accept;
extern bool stopped;
#endif

