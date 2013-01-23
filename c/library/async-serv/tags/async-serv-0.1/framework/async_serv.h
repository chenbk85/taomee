/**
 * @file async_serv.h
 * @brief
 * @author jerryshao jerryshao@taomee.com
 * @version
 * @date 2011-08-31
 */

#ifndef _H_ASYNC_SERV_H_
#define _H_ASYNC_SERV_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint32_t id;
    std::string ip_addr;
    u_short port;
} fdsession_t;

typedef struct {
    std::string ip_addr;
    u_short port;
#define TYPE_TCP 1
#define TYPE_UDP 2
    int type;
} bind_conf_t;

enum {
    PROC_MAIN = 0,
    PROC_WORK
};

typedef int (*proc_pkg_from_client_t)(void* pkg, int pkglen, fdsession_t* cli_fdsess);
typedef int (*proc_pkg_from_serv_t)(void* pkg, int pkglen, fdsession_t* serv_fdsess);

typedef int (*on_serv_conn_complete_t)(fdsession_t* serv_fdsess);
typedef int (*on_serv_conn_failed_t)(void* connector_handler);

typedef int (*on_client_conn_closed_t)(fdsession_t* cli_fdsess);
typedef int (*on_serv_conn_closed_t)(fdsession_t* serv_fdsess);

typedef int (*init_service_t)(int argc, char** argv, int proc_type);
typedef int (*fini_service_t)(int proc_type);

typedef int (*get_pkg_len_t)(void* avail_data, int avail_len);

typedef int (*proc_udp_pkg_from_client_t)(void* avail_data, int avail_len, fdsession_t* fdsess);
typedef int (*proc_udp_pkg_from_serv_t)(void* avail_data, int avail_len, fdsession_t* fdsess);
typedef int (*on_udp_serv_conn_complete_t)(fdsession_t* serv_fdsess);



#endif
