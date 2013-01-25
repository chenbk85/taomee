
#ifndef _DLL_INTERFACE_H_
#define _DLL_INTERFACE_H_

#include <async_serv/dll.h>
extern "C" {


int  init_service(int isparent);
int  fini_service(int isparent);
void proc_events();

int	 get_pkg_len(int fd, const void* pkg, int pkglen, int isparent);

int  proc_pkg_from_client(void* data, int len, fdsession_t* fdsess);
void proc_pkg_from_serv(int fd, void* data, int len);

void on_client_conn_closed(int fd);
void on_fd_closed(int fd);

void proc_mcast_pkg(const void* data, int len);

int	reload_global_data();



}
#endif