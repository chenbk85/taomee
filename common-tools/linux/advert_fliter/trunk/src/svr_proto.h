
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>
#include <async_serv/async_serv.h>
#ifdef __cplusplus
}
#endif


#include <libtaomee++/utils/md5.h> 
#include <libtaomee++/inet/pdumanip.hpp> 


int send_msg_to_db(userid_t id, uint16_t cmd,  Cmessage * c_in )
{
	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("service_dbproxy"), 0, 65535, 1);
	}

	if ((proxysvr_fd == -1)	) {
		KDEBUG_LOG(id, "send to dbproxy failed: fd=%d ", proxysvr_fd );
		return 0;
	}
	static char dbbuf[ sizeof(db_proto_t) ];

	db_proto_t* pkg = (db_proto_t*)dbbuf;
	pkg->len = sizeof(db_proto_t);
	pkg->seq = (p ? ((fdsess->fd << 16) | p->get_waitcmd()) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	KDEBUG_LOG(pkg->id,"SO[0x%04X]",pkg->cmd );

	return net_send_msg(proxysvr_fd, (char*)dbbuf, c_in   );
}

int send_request_to_db(userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
	char dbbuf[8192];
    if (proxysvr_fd == -1) {
        proxysvr_fd = connect_to_service(config_get_strval("service_dbproxy"), 0, 65535, 1);
    }

    if ((proxysvr_fd == -1) ||  (body_len > (sizeof(dbbuf) - sizeof(db_proto_t)))) {
        KERROR_LOG(id, "send to dbproxy failed: fd=%d len=%d", proxysvr_fd, body_len);
        return 0;
    }

    db_proto_t* pkg = (db_proto_t*)dbbuf;
    pkg->len = sizeof(db_proto_t) + body_len;
    pkg->seq = 0;
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = id;
    memcpy(pkg->body, dbpkgbuf, body_len);

    return net_send(proxysvr_fd, dbbuf, pkg->len);
}
