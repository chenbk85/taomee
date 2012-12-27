//#include "myprocess.h"
#include "mynet.h"
#include "mysvrlist.h"
#include "myinit.h"
#include "mydbproxy.h"
#include "myproto.h"
extern "C" {
#include <dlfcn.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
}
#include <main_login/asyc_main_login_interface.h>

void * m_lib_handle;
int m_process_service_init()
{
    init_timer();
    my_read_conf();
    multicast_init();
	if (tm_load_dirty("./data/tm_dirty.dat") < 0) {
		BOOT_LOG(-1, " fail to laod dirty data");
	}
    return 0;
}

int m_process_client_cmd(usr_info_t* p, uint8_t* body, int bodylen)
{
    switch (p->waitcmd) {
    case PROTO_GET_RECOMMEND_SVR_LIST:
        return get_recommeded_svr_list_cmd(p, body, bodylen);
    case PROTO_GET_RANGED_SVR_LIST:
        return get_ranged_svr_list_cmd(p, body, bodylen);
    case PROTO_CREATE_ROLE:
        return create_role_cmd(p, body, bodylen);
    default:
        ERROR_LOG("invalid command=%u", p->waitcmd);
    }
    return 0;
}

int m_process_pkg_len(int fd, const void* avail_data, int avail_len)
{
    if(fd == timestamp_fd) {
        const login_timestamp_pk_header_t* pkg = reinterpret_cast<const login_timestamp_pk_header_t*>(avail_data);    
        uint32_t len = ntohl(pkg->length);
        //DEBUG_LOG("RECV TM [%d %d %d]",fd, len, avail_len);
        if (len < sizeof(login_timestamp_pk_header_t)|| len > MAX_LENGTH) {
            ERROR_LOG("invalid length %u from fd %d", len, fd);
            return CLOSE_CONN;
        }
        return len;
    }else {
        const svr_proto_t* pkg = reinterpret_cast<const svr_proto_t*>(avail_data);    
        uint32_t len = pkg->len;
        //DEBUG_LOG("RECV SVR [%d %d %d]",fd, len, avail_len);
        if (len < sizeof(svr_proto_t)|| len > MAX_LENGTH) {
            ERROR_LOG("invalid length %u from fd %d", len, fd);
            return CLOSE_CONN;
        }
        return len;
    }
}

void m_process_serv_return(int fd, void* data, int len)
{
    if (fd == timestamp_fd) {
        handle_timestamp_return(reinterpret_cast<login_timestamp_pk_header_t*>(data), len);
    } else if (fd == switch_fd) {
        handle_switch_return(reinterpret_cast<svr_proto_t*>(data), len);
    } else if (fd == mole_proxysvr_fd) {
        m_process_db_return(reinterpret_cast<svr_proto_t*>(data), len);
    }
}

void m_process_serv_fd_closed(int fd)
{
    if (fd == switch_fd) {
        DEBUG_LOG("SWITCH CONNECTION CLOSED\t[fd=%d]", switch_fd);
        switch_fd = -1;
    } else if (fd == timestamp_fd) { 
        DEBUG_LOG("CACHE SERVER CONNECTION CLOSED\t[fd=%d]", timestamp_fd);
        timestamp_fd = -1;
    } else if (fd == mole_proxysvr_fd) {
        DEBUG_LOG("MOLE DBPROXY SERVER CONNECTION CLOSED\t[fd=%d]", mole_proxysvr_fd);
        mole_proxysvr_fd = -1;
    }
}
