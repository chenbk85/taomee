#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/utils/strings.hpp>

extern "C" {
#include <stdint.h>
#include <stdio.h>
#include <openssl/md5.h>
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/timer.h>
#include <libtaomee/log.h>
#include <libtaomee/project/stat_agent/msglog.h>
#include <async_serv/net_if.h>
}

#include "CProxyDealBase.hpp"
#include "CProxyRoute.hpp"

int
CProxyDealBase::handle_db_return(void* data, int len)
{
	return 0;
}

int
CProxyDealBase::handle_switch_return(void* data, int len)
{
	return 0;
}

/*int
CProxyDealBase::send_request_to_svr(int svr_fd, uint8_t* pkg, int pkglen)
{
	if (svr_fd == -1) {
		ERROR_LOG("svr fd error!");
		return -1;
	}
	
	DEBUG_LOG("SEND PACK TO SVR\t[fd=%d]", svr_fd);
	return net_send(svr_fd, pkg, pkglen);
}*/

int
CProxyDealBase::send_request_to_svr(request_info_t* info, uint8_t* pkg, int pkglen, uint16_t svr_type)
{
	//conect
	uint16_t svrtype = svr_type == 0 ? info->svr_type : svr_type;
	svr_data_t* p_svr = g_proxy_route.get_svr(info->game_flag, info->game_zone, svrtype);
	if (p_svr->fd == -1) {
		p_svr->svr_connect();
	}

	if (p_svr->fd == -1) {
		ERROR_LOG("fd cannot connect [%d %d %d]", info->game_flag, info->game_zone, info->svr_type);
		return -1;
	}

	DEBUG_LOG("SEND PACK TO SVR\t[svr:[%u %u %u] fd=%d uid=%u cmd=%u]", info->game_flag, info->game_zone, svrtype, 
			p_svr->fd, info->userid, info->waitcmd);
	return net_send(p_svr->fd, pkg, pkglen);
}

int
CProxyDealBase::send_header_to_client(request_info_t* info, int cmd, uint32_t err)
{
	int idx = sizeof(cli_proto_t);
	init_cli_header(cli_pkgbuf, idx, info, cmd, err);
	info->waitcmd = 0;
	//return g_clients.send_to_client(info->fd, cli_pkgbuf, idx);
	return g_clients.send_to_client(info->id, cli_pkgbuf, idx);
}

int
CProxyDealBase::init_cli_header(void *pkg, uint32_t len, request_info_t* info, uint16_t cmd, uint32_t ret)
{
	struct cli_proto_t* proto = reinterpret_cast<struct cli_proto_t*>(pkg);

	proto->len = len;
	proto->uid = info->userid;
	proto->cmd = cmd;
	proto->seq = 0;
	proto->ret = ret;
	proto->role_tm = info->roletm;
	proto->game_flag = info->game_flag;
	proto->game_zone = info->game_zone;
	proto->svr_type = info->svr_type;

	return 0;
}

/*const char*
CProxyDealBase::req_verify_md5(const char* req_body, int req_body_len, uint16_t channel_id)
{
	char md_16[16];
	char md_32[32 + 1];
	static char md5_buf[32 + 1] = {0};
	const uint32_t verify_buf_len = 1024 * 2;
	char verify_buf[verify_buf_len] = {0};

	uint32_t s_len = snprintf(verify_buf, verify_buf_len, 
			"channelId=%d&securityCode=%u&data=", channel_id, g_proxy_route.get_security_code(channel_id));
	
	if (req_body_len + s_len >= verify_buf_len) {
		ERROR_LOG("req_body_len too long[%u %u]", req_body_len + strlen(verify_buf), verify_buf_len);
		return 0;
	}
	
	memcpy(verify_buf + s_len, req_body, req_body_len);

	//get md5 by buf
	MD5(reinterpret_cast<unsigned char*>(verify_buf), s_len + req_body_len, reinterpret_cast<unsigned char*>(md_16));
	bin2hex_frm(md_32, md_16, 16, 0);
	sprintf(md5_buf, md_32, 32);

	return md5_buf;

}
*/

