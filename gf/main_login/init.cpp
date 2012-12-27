#include "init.h"
extern "C" {
#include <libtaomee/conf_parser/config.h>
#include <arpa/inet.h>
#include <dlfcn.h>
}
#include "asyc_main_login_interface.h"

dirser_ini_option_t g_ds_ini;

dirser_ini_option_t* get_global_opt()
{
    return &g_ds_ini;
}
void* lib_handle;
void (*process_serv_return)(int, void*, int);
void (*process_serv_fd_closed)(int);
int (*process_client_cmd)(usr_info_t*, uint8_t*, int);
int (*process_pkg_len)(int, const void*, int);
int (*process_service_init)();

int read_conf()
{
    /*read all configuration param into global variable*/
    strcpy(g_ds_ini.log_dir, config_get_strval("log_dir"));
    g_ds_ini.log_level = config_get_intval("log_level", 0);
    g_ds_ini.log_size = config_get_intval("log_size", 0);
    g_ds_ini.log_size = static_cast<uint32_t>(g_ds_ini.log_size);

    strcpy(g_ds_ini.mainlogin_dbproxy_ip, config_get_strval("mainlogin_dbproxy_ip"));
    g_ds_ini.mainlogin_dbproxy_port = config_get_intval("mainlogin_dbproxy_port", 0);

    g_ds_ini.business_type = config_get_intval("business_type", 0);

    strcpy(g_ds_ini.warning_ip, config_get_strval("warning_ip"));
    g_ds_ini.warning_port= config_get_intval("warning_port", 0);

    g_ds_ini.svr_timeout = config_get_intval("svr_timeout", 3);
#ifndef TW_VER
    g_ds_ini.count_limited = config_get_intval("count_limited", 0);
    g_ds_ini.passwd_fail_time_limited = config_get_intval("passwd_fail_time_limited", 0);
    g_ds_ini.ban_time = config_get_intval("ban_time", 0);
    g_ds_ini.channel_total = config_get_intval("channel_total", 0);

    strcpy(g_ds_ini.statistic_file, config_get_strval("statistic_file"));

    //g_ds_ini.msglog_type_start = config_get_intval("msglog_type_start", 0);
    //g_ds_ini.weekday_maintance = config_get_intval("weekday_maintance", -1);
    //g_ds_ini.maintain_ip = inet_addr(config_get_strval("maintain_ip"));
    //g_ds_ini.inner_ip_start = inet_addr(config_get_strval("inner_ip_start"));
    //g_ds_ini.inner_ip_end = inet_addr(config_get_strval("inner_ip_end"));

#endif
    return 0;
}

#define DLFUNC(h, v, name, type) do { \
	v = (type)(dlsym (h, name)); \
	if ((error = dlerror ()) != NULL) { \
		ERROR_LOG ("dlsym error, %s", error); \
		dlclose (h); \
		h = NULL; \
		return -1; \
	} \
}while (0)

int load_lib()
{
    lib_handle = dlopen (config_get_strval("lib_file"), RTLD_NOW);

	char *error; 
    if ((error = dlerror()) != NULL) {
        ERROR_LOG("dlopen error, %s", error);
        return -1;
    }
    DLFUNC(lib_handle, process_serv_return, "m_process_serv_return", void (*)(int, void*, int));
    DLFUNC(lib_handle, process_serv_fd_closed, "m_process_serv_fd_closed", void (*)(int));
    DLFUNC(lib_handle, process_client_cmd, "m_process_client_cmd", int (*)(usr_info_t*, uint8_t*, int));
    DLFUNC(lib_handle, process_pkg_len, "m_process_pkg_len", int (*)(int, const void*, int));
    DLFUNC(lib_handle, process_service_init, "m_process_service_init", int (*)());
    return 0;
}
