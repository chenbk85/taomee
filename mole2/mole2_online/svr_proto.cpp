
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

#include "sprite.h"
#include "util.h"
#include "cli_proto.h"
#include "svr_proto.h"
#include "center.h"
#include "global.h"
#include    "homemap.h"
/**
  * @brief send a request to db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_request_to_db(sprite_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("service_dbproxy"), 0, 65535, 1);
		//proxysvr_fd = connect_to_svr(config_get_strval("dbproxy_ip"), config_get_intval("dbproxy_port", 0), 65535, 1);
	}

	if ((proxysvr_fd == -1) || 	(body_len > (sizeof(dbbuf) - sizeof(db_proto_t)))) {
		KERROR_LOG(id, "send to dbproxy failed: fd=%d len=%d", proxysvr_fd, body_len);
		if (p) {
			if (p->waitcmd == proto_cli_login) {
				return -1;
			}
			return send_to_self_error(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	db_proto_t* pkg = (db_proto_t*)dbbuf;
	pkg->len = sizeof(db_proto_t) + body_len;
	pkg->seq = (p ? ((p->fd << 16) | p->waitcmd) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	memcpy(pkg->body, dbpkgbuf, body_len);

#if 0
	if (p) {
		KDEBUG_LOG(p->id, "SEND REQ TO DB\t[%u cmd=%u 0x%X]", id, p->waitcmd, cmd);
	}
#endif
	return net_send(proxysvr_fd, dbbuf, pkg->len);
}

int send_request_to_vipserv(sprite_t* p, uint32_t cmd, const void* codepkgbuf, uint32_t body_len)
{
	static uint8_t codebuf[code_prot_max_len];
	if (vipserv_fd == -1) {
		switch (idc_type) {
			case idc_type_dx:
			case idc_type_wt:
				vipserv_fd = connect_to_svr(config_get_strval("vipserv_ip"), config_get_intval("vipserv_port", 0), 65535, 1);
				break;
			case idc_type_internal:
			case idc_type_internal + 1:
				vipserv_fd = connect_to_svr(config_get_strval("in_vipserv_ip"), config_get_intval("vipserv_port", 0), 65535, 1);
				break;
		}
	}

	if ((vipserv_fd == -1) || 	(body_len > (sizeof(codebuf) - sizeof(db_proto_t)))) {
		KERROR_LOG(p->id, "send to vipsvr failed: fd=%d len=%d", vipserv_fd, body_len);
		return send_to_self_error(p, p->waitcmd, cli_err_system_error, 1);
	}

	db_proto_t* pkg = (db_proto_t*)codebuf;
	pkg->len = sizeof(db_proto_t) + body_len;
	pkg->seq = (p ? ((p->fd << 16) | p->waitcmd) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = p->id;
	memcpy(pkg->body, codepkgbuf, body_len);
	return net_send(vipserv_fd, codebuf, pkg->len);
}



int send_msg_to_db(sprite_t* p, userid_t id, uint16_t cmd,  Cmessage * c_in )
{
	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("service_dbproxy"), 0, 65535, 1);
		//proxysvr_fd = connect_to_svr(config_get_strval("dbproxy_ip"), config_get_intval("dbproxy_port", 0), 65535, 1);
	}

	if ((proxysvr_fd == -1)	) {
		KERROR_LOG(id, "send to dbproxy failed: fd=%d ", proxysvr_fd );
		if (p) {
			if (p->waitcmd == proto_cli_login) {
				return -1;
			}
			return send_to_self_error(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	db_proto_t* pkg = (db_proto_t*)dbbuf;
	pkg->len = sizeof(db_proto_t);
	pkg->seq = (p ? ((p->fd << 16) | p->waitcmd) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;

	return net_send_msg(proxysvr_fd, (char*)dbbuf, c_in   );
}


/**
  * @brief send a request to db proxy
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_to_db_no_sprite(userid_t id, uint16_t cmd, uint32_t seq, const void* dbpkgbuf, uint32_t body_len)
{
	if (proxysvr_fd == -1) {
		proxysvr_fd = connect_to_service(config_get_strval("service_dbproxy"), 0, 65535, 1);
		//proxysvr_fd = connect_to_svr(config_get_strval("dbproxy_ip"), config_get_intval("dbproxy_port", 0), 65535, 1);
	}

	if ((proxysvr_fd == -1) || 	(body_len > (sizeof(dbbuf) - sizeof(db_proto_t)))) {
		KERROR_LOG(id, "send to dbproxy failed: fd=%d len=%d", proxysvr_fd, body_len);
		return 0;
	}

	db_proto_t* pkg = (db_proto_t*)dbbuf;
	pkg->len = sizeof(db_proto_t) + body_len;
	pkg->seq = seq;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	memcpy(pkg->body, dbpkgbuf, body_len);

#if 0
	if (p) {
		KDEBUG_LOG(p->id, "SEND REQ TO DB\t[%u cmd=%u 0x%X]", id, p->waitcmd, cmd);
	}
#endif
	return net_send(proxysvr_fd, dbbuf, pkg->len);
}


int send_udp_login_report_to_db(const sprite_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
	db_proto_t *pkg;
	int len;

	if ( body_len > sizeof(dbbuf) - sizeof(*pkg) )
		ERROR_RETURN (("package too large, uid=%u", p ? p->id : 0), -1); 

	len = sizeof (db_proto_t) + body_len;
	pkg = (db_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	pkg->seq = 0;
	pkg->ret = 0;
	memcpy (pkg->body, dbpkgbuf, body_len);

	sendto(udp_report_fd, dbbuf, len, 0, (const sockaddr *)&udp_report_addr, sizeof(struct sockaddr_in));
	return 0;
}

int send_udp_post_msg_to_db(const sprite_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
	db_proto_t *pkg;
	int len;

	if ( body_len > sizeof(dbbuf) - sizeof(*pkg) )
		ERROR_RETURN (("package too large, uid=%u", p ? p->id : 0), -1); 

	len = sizeof (db_proto_t) + body_len;
	pkg = (db_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	pkg->seq = 0;
	pkg->ret = 0;
	memcpy (pkg->body, dbpkgbuf, body_len);

	sendto(udp_post_fd, dbbuf, len, 0, (const sockaddr *)&udp_post_addr, sizeof(struct sockaddr_in));
	return 0;
}

int send_udp_chat_msg_to_db(const sprite_t* p, userid_t id, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
	db_proto_t *pkg;
	int len;

	if ( body_len > sizeof(dbbuf) - sizeof(*pkg) )
		ERROR_RETURN (("package too large, uid=%u", p ? p->id : 0), -1); 

	len = sizeof (db_proto_t) + body_len;
	pkg = (db_proto_t *)dbbuf;
	pkg->len = len;
	pkg->cmd = cmd;
	pkg->id = id;
	pkg->seq = 0;
	pkg->ret = 0;
	memcpy (pkg->body, dbpkgbuf, body_len);

	sendto(chat_svr_fd, dbbuf, len, 0, (const sockaddr *)&udp_chat_svr_addr, sizeof(struct sockaddr_in));
	return 0;
}



/**
  * @brief send a request to code
  * @param p the player who launches the request to db proxy
  * @param id id of the requested player
  * @param cmd command id
  * @param body_buf body of the request
  * @param body_len length of the body_buf
  * @return 0 on success, -1 on error
  */
int send_request_to_code(sprite_t* p, uint32_t cmd, const void* codepkgbuf, uint32_t body_len)
{
	static uint8_t codebuf[code_prot_max_len];

	if (magic_fd == -1) {
		//magic_fd = connect_to_service(config_get_strval("service_code"), 0, 65535, 1);
		magic_fd = connect_to_svr(config_get_strval("codesvr_ip"), config_get_intval("codesvr_port", 0), 65535, 1);
		//magic_fd = connect_to_svr("192.168.0.158", 58111, 65535, 1);
	}

	if ((magic_fd == -1) || 	(body_len > (sizeof(codebuf) - sizeof(db_proto_t)))) {
		KERROR_LOG(p->id, "send to codesvr failed: fd=%d len=%d", magic_fd, body_len);
		return send_to_self_error(p, p->waitcmd, cli_err_system_error, 1);
	}

	code_proto_t* pkg = (code_proto_t*)codebuf;
	pkg->len = sizeof(code_proto_t) + body_len;
	pkg->ver = 0;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = p->id;
	memcpy(pkg->body, codepkgbuf, body_len);
	
	return net_send(magic_fd, codebuf, pkg->len);
}


int send_request_to_spacetime(sprite_t* p, uint32_t cmd, const void* codepkgbuf, uint32_t body_len)
{
	static uint8_t codebuf[code_prot_max_len];

	if (spacetime_fd == -1) {
		switch (idc_type) {
		case idc_type_dx:
		case idc_type_wt:
			spacetime_fd = connect_to_svr(config_get_strval("spacetime_svr_ip"), config_get_intval("spacetime_svr_port", 0), 65535, 1);
			break;
		case idc_type_internal:
		case idc_type_internal + 1:
			spacetime_fd = connect_to_svr(config_get_strval("in_spacetime_svr_ip"), config_get_intval("spacetime_svr_port", 0), 65535, 1);
			break;
		}
	}

	if ((spacetime_fd == -1) || 	(body_len > (sizeof(codebuf) - sizeof(db_proto_t)))) {
		KERROR_LOG(p->id, "send to codesvr failed: fd=%d len=%d", spacetime_fd, body_len);
		return send_to_self_error(p, p->waitcmd, cli_err_system_error, 1);
	}

	code_proto_t* pkg = (code_proto_t*)codebuf;
	pkg->len = sizeof(code_proto_t) + body_len;
	pkg->ver = 0;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = p->id;
	memcpy(pkg->body, codepkgbuf, body_len);
	
	return net_send(spacetime_fd, codebuf, pkg->len);
}

int send_request_to_mall(sprite_t* p, uint32_t cmd, const void* pkgbuf, uint32_t body_len)
{
	static uint8_t mallbuf[code_prot_max_len];

	if (mall_fd == -1) {
		if (idc_type >= idc_type_internal) {
			mall_fd = connect_to_svr(config_get_strval("in_mall_svr_ip"), config_get_intval("mall_svr_port", 0), 65535, 1);
		} else {
			mall_fd = connect_to_svr(config_get_strval("mall_svr_ip"), config_get_intval("mall_svr_port", 0), 65535, 1);
		}
	}

	if ((mall_fd == -1) || 	(body_len > (sizeof(mallbuf) - sizeof(db_proto_t)))) {
		KERROR_LOG(p->id, "send to mall svr failed: fd=%d len=%d", mall_fd, body_len);
		return send_to_self_error(p, p->waitcmd, cli_err_system_error, 1);
	}

	db_proto_t* pkg = (db_proto_t*)mallbuf;
	pkg->len = sizeof(db_proto_t) + body_len;
	pkg->seq = 0;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = p->id;

	if(body_len) memcpy(pkg->body, pkgbuf, body_len);

	return net_send(mall_fd, mallbuf, pkg->len);
}

int send_to_batrserv(sprite_t* p, int cmd, uint32_t body_len, const void* body_buf, userid_t id, battle_id_t btid, uint32_t opid)
{	
	static uint8_t bcpkg[hm_proto_max_len];
	int fd_idx = BATTLE_USER_ID(btid) % MAX_BATRSERV_NUM;
	/*
	if (BATTLE_USER_ID(btid) == 5005468 || BATTLE_USER_ID(btid) == 5007321)
		fd_idx = 2;
	*/
	if (batrserv_fd[fd_idx] == -1) {
		batrserv_fd[fd_idx] = connect_to_service(config_get_strval("service_batrserv"), fd_idx + 1, 65535, 1);
	}
	
	int batr_fd = batrserv_fd[fd_idx];

	int len = sizeof(batrserv_proto_t) + body_len;
	if ((batr_fd == -1) || (body_len > (sizeof(bcpkg) - sizeof(batrserv_proto_t)))) {	
		KERROR_LOG(id, "fail to send to batrserv\t[batr_fd=%d]", batr_fd);
		if (p && IS_NORMAL_ID(p->id)) {
			response_proto_head(p, proto_cli_btrsvr_conn_closed, 0, 0);
			p->waitcmd = 0;
			//return send_to_self_error(p, p->waitcmd, cli_err_batsvr_error, 1);
		}
		return 0;
	}

	batrserv_proto_t* pkg = (batrserv_proto_t *)bcpkg;
	pkg->len        = len;

	pkg->online_id  = get_server_id();
	pkg->cmd        = cmd;
	pkg->ret        = 0;
	pkg->id         = id;
	pkg->btid	 	= btid;
	pkg->opid       = opid;
	memcpy(pkg->body, body_buf, body_len);

	//KDEBUG_LOG(0, "SEND HOMESERV\t[%u %u %llx %u %u]", cmd, opid, mapid, id, len);
	return net_send(batr_fd, bcpkg, len);
}


/**
 * send_init_pkg - send an init pkg to central online
 * 
 */
void send_init_pkg()
{
	static uint32_t seqno = 0;

	int i = 0;
	uint8_t pkg[48];
	PKG_H_UINT16(pkg, config_get_intval("domain", 0), i);
	PKG_H_UINT32(pkg, get_server_id(), i);
	PKG_STR(pkg, get_server_name(), i, 16);
	PKG_STR(pkg, get_server_ip(), i, 16);
	PKG_H_UINT16(pkg, get_server_port(), i);
	PKG_H_UINT32(pkg, sprites_count, i);
	//PKG_H_UINT32(pkg, 500, i);
	PKG_H_UINT32(pkg, seqno, i);
	++seqno;

	send_to_switch(NULL, COCMD_init_pkg, sizeof(pkg), (void*)pkg, 0);
}

int send_msg_to_switch(sprite_t* p, userid_t id, uint16_t cmd,  Cmessage * c_in )
{
	static uint8_t sw_pkg[switch_proto_max_len];
	if (switch_fd == -1) {
		connect_to_switch();
	}
	int len = sizeof(switch_proto_t) ;
	if ((switch_fd == -1)) {		
		KERROR_LOG(id, "FAILED SENDING PACKAGE TO SWITCH\t[switch_fd=%d]", switch_fd);
		if (p) {
			return send_to_self_error(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	//Build the package
	switch_proto_t* pkg = (switch_proto_t*)sw_pkg;
	pkg->len = len;
	pkg->seq = (p ? ((p->fd) << 16) | (p->waitcmd) : 0); // set higher 16 bytes fd and lower 16 bytes waitcmd
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->sender_id  = id;

	if (p) {
		KDEBUG_LOG(p->id, "TO SW\t[cmd=%u waitcmd=%u]", cmd, p->waitcmd);
	}

	return net_send_msg(switch_fd, (char*)sw_pkg, c_in);
}

int send_to_switch(sprite_t* p, uint16_t cmd, uint32_t body_len, void* body_buf, userid_t sender_id)
{
	//Common buffer for sending package to switch
	static uint8_t sw_pkg[switch_proto_max_len];

	//Connect to switch and send init package in case of disconnection
	if (switch_fd == -1) {
		connect_to_switch();
	}
	
	int len = sizeof(switch_proto_t) + body_len;
	if ((switch_fd == -1) || (body_len > (sizeof(sw_pkg) - sizeof(switch_proto_t)))) {		
		KERROR_LOG(sender_id, "FAILED SENDING PACKAGE TO SWITCH\t[switch_fd=%d]", switch_fd);
		if (p) {
			return send_to_self_error(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	//Build the package
	switch_proto_t* pkg = (switch_proto_t*)sw_pkg;
	pkg->len = len;
	pkg->seq = (p ? ((p->fd) << 16) | (p->waitcmd) : 0); // set higher 16 bytes fd and lower 16 bytes waitcmd
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->sender_id  = sender_id;
	memcpy(pkg->body, body_buf, body_len);

	if (p) {
		KDEBUG_LOG(p->id, "TO SW\t[cmd=%u waitcmd=%u]", cmd, p->waitcmd);
	}
	return net_send(switch_fd, sw_pkg, len);
}

/**
  * @brief connect to switch and send an init package of online information when online is launched
  */
void connect_to_switch()
{
	switch_fd = connect_to_service(config_get_strval("service_switch"), 0, 65535, 1);
	if (switch_fd != -1) {
		//KDEBUG_LOG(0, "BEGIN CONNECTING TO CENTRAL ONLINE AND SEND INIT PKG");
		send_init_pkg();
	}
}

int send_sw_keepalive_pkg(void* owner, void* data)
{
	send_to_switch(0, COCMD_keep_alive, 0, 0, 0);   // send keepalive header package to switch
	ADD_ONLINE_TIMER(&g_events, n_send_sw_keepalive_pkg, 0, 30);
	if (beast_cnt == -1) {
		//report_global_beast_kill(0);
	}
	return 0;
}

int batrserv_fd_idx(int fd)
{	
	for (int loop = 0; loop < MAX_BATRSERV_NUM; loop++) {
		if (batrserv_fd[loop] == fd)
			return loop;
	}
	return -1;
}

int homeserv_fd_idx(int fd)
{	
	for (int loop = 0; loop < MAX_HOMESERV_NUM; loop++) {
		if (home_serv_fd[loop] == fd)
			return loop;
	}
	return -1;
}
int send_msg_to_homeserv_ex(sprite_t* p, int cmd, char * head_buf, Cmessage* c_in, userid_t id, map_id_t mapid, uint32_t opid)
{
	static uint8_t bcpkg[hm_proto_max_len];
	
	int fd_idx = mapid % MAX_HOMESERV_NUM;
	if (home_serv_fd[fd_idx] == -1) {
		home_serv_fd[fd_idx] = connect_to_service(config_get_strval("service_homeserv"), fd_idx + 1, 65535, 1);
	}
	
	int home_fd = home_serv_fd[fd_idx];

	int len = sizeof(homeserv_proto_t) + ntohl(*(uint32_t *)head_buf) ;

	if (home_fd == -1) {
		KERROR_LOG(id, "failed sending package to home [home_fd = %d]", home_fd);
		return 0;
	}

	homeserv_proto_t* pkg = (homeserv_proto_t *)bcpkg;
	pkg->len        = len;
	pkg->online_id  = get_server_id();
	pkg->cmd        = cmd;
	pkg->ret        = 0;
	pkg->id         = id;
	pkg->mapid      = mapid;
	pkg->opid       = opid;
	memcpy(pkg->body, head_buf, ntohl(*(uint32_t *)head_buf));
	//TODO  先假定全是小端
	KDEBUG_LOG(id, "SEND MSG HOME\t[%u %u %u %u %u]", cmd, opid, high32_val(mapid), low32_val(mapid), len);
	
	return net_send_msg(home_fd,(char*)pkg,c_in, sizeof (homeserv_proto_t ), true);	
}

int send_msg_to_homeserv( userid_t id, map_id_t mapid, uint32_t opid, Cmessage * c_in )
{
	static uint8_t bcpkg[hm_proto_max_len];
	
	int fd_idx = mapid % MAX_HOMESERV_NUM;
	if (home_serv_fd[fd_idx] == -1) {
		home_serv_fd[fd_idx] = connect_to_service(config_get_strval("service_homeserv"), fd_idx + 1, 65535, 1);
	}
	
	int home_fd = home_serv_fd[fd_idx];

	int len = sizeof(homeserv_proto_t);

	if (home_fd == -1) {
		KERROR_LOG(id, "failed sending package to home [home_fd = %d]", home_fd);
		return 0;
	}

	homeserv_proto_t* pkg = (homeserv_proto_t *)bcpkg;
	pkg->len        = len;
	pkg->online_id  = get_server_id();
	pkg->cmd        = MCAST_home_msg;
	pkg->ret        = 0;
	pkg->id         = id;
	pkg->mapid      = mapid;
	pkg->opid       = opid;

	KDEBUG_LOG(id, "MSG SEND HOMESERV\t[ %u %u %u %u]",  opid, high32_val(mapid), low32_val(mapid), len);
	return net_send_msg(home_fd, (char*)bcpkg, c_in );	
}

int send_to_homeserv(sprite_t* p, int cmd, uint32_t body_len, const void* body_buf, userid_t id, map_id_t mapid, uint32_t opid)
{
	static uint8_t bcpkg[hm_proto_max_len];
	
	int fd_idx = mapid % MAX_HOMESERV_NUM;
	if (home_serv_fd[fd_idx] == -1) {
		home_serv_fd[fd_idx] = connect_to_service(config_get_strval("service_homeserv"), fd_idx + 1, 65535, 1);
	}
	
	int home_fd = home_serv_fd[fd_idx];

	int len = sizeof(homeserv_proto_t) + body_len;
	if ( body_len > (sizeof(bcpkg) - sizeof(batrserv_proto_t))) {	
		KERROR_LOG(id, "failed sending package to homeserv [home_fd=%d]", home_fd);
		if (p) {
			return send_to_self_error(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	} 
	if (home_fd == -1) {
		KERROR_LOG(id, "failed sending package to home [home_fd = %d]", home_fd);
		return 0;
	}

	homeserv_proto_t* pkg = (homeserv_proto_t *)bcpkg;
	pkg->len        = len;
	pkg->online_id  = get_server_id();
	pkg->cmd        = cmd;
	pkg->ret        = 0;
	pkg->id         = id;
	pkg->mapid      = mapid;
	pkg->opid       = opid;
	memcpy(pkg->body, body_buf, body_len);

	KDEBUG_LOG(id, "SEND HOMESERV\t[%u %u %u %u %u]", cmd, opid, high32_val(mapid), low32_val(mapid), len);
	return net_send(home_fd, bcpkg, len);	
}
