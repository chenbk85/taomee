#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include <libtaomee++/utils/strings.hpp>

extern "C" {
#include <libtaomee/log.h>
#include <async_serv/net_if.h>
}

#include "CProxyClientManage.hpp"
#include "CProxyDealBase.hpp"
#include "CProxyDealGf.hpp"
#include "CProxyDealSeer.hpp"
#include "CProxyDealSeer2.hpp"
#include "CProxyDealMole.hpp"
#include "CProxyDealHero.hpp"
#include "CProxyDealXhx.hpp"
#include "CProxyDealMdd.hpp"
#include "CProxyRoute.hpp"

CProxyRoute g_proxy_route;

void
svr_data_t::init(const char* p_ip, int n_port)
{
	strncpy(ip, p_ip, sizeof(ip));
	port = n_port;
	svr_connect();
}

void
svr_data_t::svr_connect()
{
	fd = connect_to_svr(ip, port, 65535, 1);
	DEBUG_LOG("SVR CONNECT\t[ip=%s port=%d fd=%d]", ip, port, fd);
}

CProxyRoute::CProxyRoute()
{
	for (int i = 0; i < max_game_flag; i++) {
		m_deals[i] = 0;
	}
	
	m_deals[game_gf] = new CProxyDealGf();
	m_deals[game_seer] = new CProxyDealSeer();
	m_deals[game_seer2] = new CProxyDealSeer2();
	m_deals[game_mole] = new CProxyDealMole();
	m_deals[game_hero] = new CProxyDealHero();
	m_deals[game_xhx] = new CProxyDealXhx();
	m_deals[game_mdd] = new CProxyDealMdd();
	m_deals[game_mdd_ren] = new CProxyDealMdd();
	m_deals[game_mdd_sina] = new CProxyDealMdd();
}

CProxyRoute::~CProxyRoute()
{
	//delete m_gf_deal;
	//delete[] m_deals;
	for (int i = 0; i < max_game_flag; i++) {
		if (m_deals[i]) {
			delete m_deals[i];
			m_deals[i] = 0;
		}
	}
}

void
CProxyRoute::svr_fd_closed(int fd)
{
	for (int i = 0; i < max_game_flag; i++) {
		for (int j = 0; j < max_zone; j++) {
			for (int k = 0; k < max_svrs; k++) {
				if (m_svr[i][j][k].fd ==fd) {
					m_svr[i][j][k].fd = -1;
					return;
				}
			}
		}
	}
}

uint32_t
CProxyRoute::get_security_code(uint16_t channel_id)
{
	if (channel_id < max_channel_id) {
		return security_codes[channel_id];
	}

	return 0;
}

int
CProxyRoute::reload_security_config(char* filename)
{
	FILE* fp;
	if ((fp=fopen(filename, "r")) == NULL) {
		ERROR_LOG("file open error[%s]", filename);
		return -1;
	}

	
	char line[250];
	while (fgets(line, 250, fp) != NULL) {
		char channel[10];
		char security[10];
		sscanf(line, "%s%s", channel, security);
		uint16_t id = atoi(channel);
		uint32_t code = atol(security);
		if (id >= max_channel_id) {
			ERROR_LOG("conf error:channel id![%d %d]", id, max_channel_id);
			return -1;
		}
		security_codes[id] = code;
	}

	for (uint32_t i = 0; i < max_channel_id; i++) {
		DEBUG_LOG("security_code[%u]=%u", i, security_codes[i]);
	}

	fclose(fp);
	return 0;
}

int
CProxyRoute::reload_svr_config(char* filename)
{
	FILE* fp;
	if ((fp=fopen(filename, "r")) == NULL) {
		ERROR_LOG("file open error[%s]", filename);
		return -1;
	}

	char line[250];
	while (fgets(line, 250, fp) != NULL) {
		char game_name[20];
		char zone[10];
		char svr_type[20];
		char svr_ip[20];
		char svr_port[10];
		sscanf(line, "%s%s%s%s%s", game_name, zone, svr_type, svr_ip, svr_port);
		init_svr_info(game_name, zone, svr_type, svr_ip, svr_port);
	}

	fclose(fp);
	return 0;
}

int
CProxyRoute::init_svr_info(char* game, char* zone, char* svr_type, char* ip, char* port)
{
	DEBUG_LOG("SVR INFO[%s %s %s %s %s]", game, zone, svr_type, ip, port);
	int game_flag = get_game_flag(game);
	int game_zone = get_game_zone(zone);
	int game_svr  = get_game_svr(svr_type);
	
	if (game_flag == -1 || game_zone == -1 || game_svr == -1) {
		ERROR_LOG("conf error:no such game![%d %d %d]", game_flag, game_zone, game_svr);
		return -1;
	}

	if (game_flag >= max_game_flag || game_zone >= max_zone || game_svr >= max_svrs) {
		ERROR_LOG("conf error:more than max![%d %d %d]", game_flag, game_zone, game_svr);
		return -1;
	}

	m_svr[game_flag][game_zone][game_svr].init(ip, atoi(port));
	return 0;
}

int 
CProxyRoute::get_game_flag(char* p_game)
{
	if (!strcmp(p_game,"gf")) {
		return game_gf;
	} else if (!strcmp(p_game, "seer")) {
		return game_seer;
	} else if (!strcmp(p_game, "seer2")) {
		return game_seer2;
	} else if (!strcmp(p_game, "mole")) {
		return game_mole;
	} else if (!strcmp(p_game, "hero")) {
		return game_hero;
	} else if (!strcmp(p_game, "xhx")) {
		return game_xhx;
	} else if (!strcmp(p_game, "mdd")) {
		return game_mdd;
	} else if (!strcmp(p_game, "mdd_ren")) {
		return game_mdd_ren;
	} else if (!strcmp(p_game, "mdd_sina")) {
		return game_mdd_sina;
	}

	return -1;
}

int
CProxyRoute::get_game_zone(char* p_zone)
{
	if (!strcmp(p_zone, "tel")) {
		return zone_tel;
	} else if (!strcmp(p_zone, "cnc")) {
		return zone_cnc;
	}

	return -1;
}

int
CProxyRoute::get_game_svr(char* p_svr)
{
	if (!strcmp(p_svr, "dbproxy")) {
		return svr_dbproxy;
	} else if (!strcmp(p_svr, "switch")) {
		return svr_switch;
	}

	return -1;
}

int
CProxyRoute::send_errormsg_to_client(int fd, uint32_t err, cli_proto_t* p_header)
{
	if (p_header) {
		memcpy(&header_, p_header, sizeof(cli_proto_t));
	}
	header_.ret = err;
	return net_send(fd, reinterpret_cast<uint8_t*>(&header_), sizeof(header_));
}

const char*
CProxyRoute::req_verify_md5(const char* req_body, int req_body_len, uint16_t channel_id)
{
	char md_16[16];
	char md_32[32 + 1];
	static char md5_buf[32 + 1] = {0};
	const uint32_t verify_buf_len = 1024 * 2;
	char verify_buf[verify_buf_len] = {0};

	uint32_t s_len = snprintf(verify_buf, verify_buf_len, 
			"channelId=%d&securityCode=%u&data=", channel_id, get_security_code(channel_id));
	
	if (req_body_len + s_len >= verify_buf_len) {
		ERROR_LOG("req_body_len too long[%lu %u]", req_body_len + strlen(verify_buf), verify_buf_len);
		return 0;
	}
	
	memcpy(verify_buf + s_len, req_body, req_body_len);

	//get md5 by buf
	MD5(reinterpret_cast<unsigned char*>(verify_buf), s_len + req_body_len, reinterpret_cast<unsigned char*>(md_16));
	bin2hex_frm(md_32, md_16, 16, 0);
	sprintf(md5_buf, md_32, 32);

	return md5_buf;
}

int 
CProxyRoute::check_security_code(void* data)
{
	cli_proto_t* pkg = reinterpret_cast<cli_proto_t*>(data);
	int bodylen = pkg->len - sizeof(cli_proto_t);

	const char* verify_md5 = req_verify_md5(reinterpret_cast<char*>(pkg->body), bodylen, pkg->channel_id);
	return strncmp(pkg->verify_code, verify_md5, 32);
}

int
CProxyRoute::dispatch(void* data, fdsession_t* fdsess, int len)
{
	cli_proto_t* pkg = reinterpret_cast<cli_proto_t*>(data);

	//check len
	if (pkg->len < sizeof(cli_proto_t)) {
		ERROR_LOG("invalid pkg len=%u from fd=%u", pkg->len, fdsess->fd);
		return -1;
	}

	//check cmd
	if (pkg->channel_id != customer_service_fixed && (pkg->cmd < cli_cmd_start || pkg->cmd > cli_cmd_end)) {
		ERROR_LOG("invalid cmd=%u from fd=%u uid=%u", pkg->cmd, fdsess->fd, pkg->uid);
		return -1;
	}

	//check game flag
	if (pkg->game_flag >= max_game_flag || pkg->game_zone >= max_zone || pkg->svr_type >= max_svrs) {
		ERROR_LOG("invalid game flag=%u zone=%u svrs=%u from fd=%u uid=%u cmd=%u", 
				pkg->game_flag, pkg->game_zone, pkg->svr_type, fdsess->fd, pkg->uid, pkg->cmd);
		return -1;
	}

	if (m_deals[pkg->game_flag] == 0) {	
		ERROR_LOG("invalid game deal=%u from fd=%u uid=%u cmd=%u flag=%u", 
				pkg->game_flag, fdsess->fd, pkg->uid, pkg->cmd, pkg->game_flag);
		return -1;
	}

	DEBUG_LOG("DISPATCH\t[len=%u fd=%d svr=%d %d %d channel=%d]", len, fdsess->fd, 
			pkg->game_flag, pkg->game_zone, pkg->svr_type, pkg->channel_id);
	//add client
	uint32_t cli_idx = g_clients.add_client(fdsess, pkg->uid, pkg->role_tm, 
			pkg->game_flag, pkg->game_zone, pkg->svr_type, pkg->channel_id);

	if (pkg->channel_id == customer_service_fixed) {
		return customer_service_fixed_dispatch(data, cli_idx);
	}

	return cmd_dispatch(data, cli_idx);
}

svr_data_t*
CProxyRoute::get_svr(int flag, int zone, int type)
{
	if ( (flag < 0 || flag >= max_game_flag) ||
		 (zone < 0 || zone >= max_zone) ||
		 (type < 0 || type >= max_svrs)) {
		ERROR_LOG("[%d %d %d]", flag, zone, type);
		return 0;
	}

	return &(m_svr[flag][zone][type]);
}

void
CProxyRoute::add_to_svrmap(int fd, uint32_t flag, uint32_t zone, uint32_t type)
{
	svr_info_t svr(flag, zone, type);
	svr_map_.insert(SvrMap::value_type(fd, svr));

	DEBUG_LOG("ADD SVR TO MAP\t[fd=%d flag=%u zone=%u type=%u]", fd, flag, zone, type);
	return;
}

void
CProxyRoute::del_to_svrmap(int fd)
{
	svr_map_.erase(fd);

	DEBUG_LOG("DEL SVR TO MAP\t[fd=%d]", fd);
	return;
}

int
CProxyRoute::get_game_flag_by_fd(int fd)
{
	for (int i = 0; i < max_game_flag; i++) {
		for (int j = 0; j < max_zone; j++) {
			for (int k = 0; k < max_svrs; k++) {
				if (m_svr[i][j][k].fd == fd) {
					return i * 10000 + j * 100 + k;
				}
			}
		}
	}

	/*SvrMap::iterator it = svr_map_.find(fd);
	if (it != svr_map_.end()) {
		svr_info_t* info = &(it->second);
		return game_flag * 10000 + game_zone * 100 + svr_type;
	}*/

	return -1;
}

int
CProxyRoute::cmd_dispatch(void* data, uint32_t idx)
{
	request_info_t* info = g_clients.get_request_info(idx);

	cli_proto_t* pkg = reinterpret_cast<cli_proto_t*>(data);
	int body_len = pkg->len - sizeof(cli_proto_t);
	int ret = 0;

	//check verify code
	int update_flag = pkg->cmd % 1000;
	if (pkg->channel_id != email_send && update_flag >= 100 && check_security_code(data) != 0) {
		ERROR_LOG("security code error![cmd=%u id=%u channel=%u code=%u]", pkg->cmd, pkg->uid, 
				pkg->channel_id, get_security_code(pkg->channel_id));
		return -1;
	}

	info->waitcmd = pkg->cmd;
	switch (pkg->cmd) {
		case cli_get_friends_list:
			ret = m_deals[pkg->game_flag]->get_friends_list_cmd(info, pkg->body, body_len);
			break;
		case cli_get_user_nick:
			ret = m_deals[pkg->game_flag]->get_user_nick_cmd(info, pkg->body, body_len);
			break;
		case cli_get_user_info:
			ret = m_deals[pkg->game_flag]->get_user_info_cmd(info, pkg->body, body_len);
			break;
		case cli_send_mail:
			ret = m_deals[pkg->game_flag]->send_mail_cmd(info, pkg->body, body_len);
			break;
		case cli_add_items:
			ret = m_deals[pkg->game_flag]->add_items_cmd(info, pkg->body, body_len);
			break;
		case cli_goods_add_items:
			ret = m_deals[pkg->game_flag]->goods_add_items_cmd(info, pkg->body, body_len);
			break;
		case cli_goods_check_items:
			ret = m_deals[pkg->game_flag]->goods_check_items_cmd(info, pkg->body, body_len);
			break;
		case cli_add_notify:
			ret = m_deals[pkg->game_flag]->add_notify_cmd(info, pkg->body, body_len);
			break;
		default:
			ERROR_LOG("invalid cmd=%d uid=%u", pkg->cmd, pkg->uid);
			return -1;
	}
	
	return ret;
	//return m_deals[pkg->game_flag]->send_request_to_svr(p_svr->fd, 0, 0);
}

int
CProxyRoute::customer_service_fixed_dispatch(void* data, uint32_t idx)
{
	request_info_t* info = g_clients.get_request_info(idx);

	cli_proto_t* pkg = reinterpret_cast<cli_proto_t*>(data);
	int body_len = pkg->len - sizeof(cli_proto_t);

	info->waitcmd = pkg->cmd;
	int ret = m_deals[pkg->game_flag]->customer_service_fixed_cmd(info, pkg->body, body_len);

	return ret;
}

int 
CProxyRoute::handle_svr_return(int svr_fd, void* data, int len)
{
	int ret = get_game_flag_by_fd(svr_fd);
	if (ret == -1) {
		ERROR_LOG("no fd=%d", svr_fd);
		return -1;
	}

	int game_flag = ret / 10000;
	int game_zone = ret % 10000 / 100;
	int game_svr  = ret % 100;
	if (game_flag >= max_game_flag || game_zone >= max_zone || game_svr >= max_svrs) {
		ERROR_LOG("game info error [%d %d %d %d]", svr_fd, game_flag, game_zone, game_svr);
		return -1;
	}

	if (!m_deals[game_flag]) {
		ERROR_LOG("no svr deal:fd=%d gameflag=%d", svr_fd, game_flag);
		return -1;
	}

	DEBUG_LOG("SVR RETURN INFO\t[len=%u fd=%d svr=%d %d %d]", len, svr_fd, game_flag, game_zone, game_svr);
	if (game_svr == svr_dbproxy) {
		return m_deals[game_flag]->handle_db_return(data, len);
	} else if (game_svr == svr_switch) {
		return m_deals[game_flag]->handle_switch_return(data, len);
	} else {
		ERROR_LOG("svr type error[svr type=%d]", game_svr);
		return -1;
	}

	return 0;
}






