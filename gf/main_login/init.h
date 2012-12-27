#ifndef INIT_H
#define INIT_H
//#include "asyc_main_login_interface.h"
#include "asyc_main_login_type.h"
extern dirser_ini_option_t g_ds_ini;
int read_conf();
int load_lib();

dirser_ini_option_t* get_global_opt();
extern void (*process_serv_return)(int, void*, int);
extern void (*process_serv_fd_closed)(int);
extern int (*process_client_cmd)(usr_info_t*, uint8_t*, int);
extern int (*process_pkg_len)(int, const void*, int);
extern int (*process_service_init)();
#endif
