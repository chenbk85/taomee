#include <libtaomee++/inet/byteswap.hpp>
#include "CProxyClientManage.hpp"

using namespace taomee;

ClientManage g_clients;

ClientManage::ClientManage()
{
	cli_idx = 0;
}

ClientManage::~ClientManage()
{

}

uint32_t 
ClientManage::add_client(fdsession_t* fdsess, uint32_t uid, uint32_t roletm, uint32_t flag, 
		uint32_t zone, uint32_t type, uint32_t channel)
{
	struct client_info_t client;
	client.fdsess = fdsess;
	client.info.fd = fdsess->fd;
	client.info.userid = uid;
	client.info.roletm = roletm;
	client.info.game_flag = flag;
	client.info.game_zone = zone;
	client.info.svr_type = type;
	client.info.channel_id = channel;
	client.info.waitcmd = 0;

	//if (cli_idx == 65535) {
	if (cli_idx == 0xFFFFFFFF) {
		cli_idx = 0;
	} else {
		cli_idx++;
	}
	client.info.id = cli_idx;

	m_client_map.insert(ClientMap::value_type(cli_idx, client));
	m_client_fd_set[fdsess->fd].insert(cli_idx);

	DEBUG_LOG("ADD CLIENT\t[fd=%d fdsize=%lu idx=%u uid=%u svr=[%u %u %u] ch=%u]", fdsess->fd, m_client_fd_set[fdsess->fd].size(), 
			cli_idx, uid, flag, zone, type, channel);
	return cli_idx;
}

int
//ClientManage::del_client(int fd)
ClientManage::del_client(uint32_t idx, bool complete)
{
	ClientMap::iterator it = m_client_map.find(idx);
	if (it != m_client_map.end()) {
		int fd = it->second.fdsess->fd;
		if (complete) {
			m_client_fd_set[fd].erase(idx);
		}

		m_client_map.erase(it);
		DEBUG_LOG("DEL CLIENT[idx=%u fd=%d fdsize=%lu]", idx, fd, m_client_fd_set[fd].size());
	}

	return 0;
}

int
ClientManage::del_clients(int fd)
{
	ClientfdSet::iterator it = m_client_fd_set[fd].begin();
	for(;it != m_client_fd_set[fd].end(); it++) {
		uint32_t idx = (*it);
		//m_client_map.erase(idx);
		del_client(idx, false);
	}

	m_client_fd_set[fd].clear();
	return 0;
}

struct client_info_t*
//ClientManage::get_client_info(int fd)
ClientManage::get_client_info(uint32_t idx)
{
	ClientMap::iterator it = m_client_map.find(idx);
	if (it != m_client_map.end()) {
		return &(it->second);
	}

	return 0;
}

struct request_info_t*
//ClientManage::get_request_info(int fd)
ClientManage::get_request_info(uint32_t idx)
{
	struct client_info_t* p_client = get_client_info(idx);
	if (!p_client) {
		return 0;
	}

	return &(p_client->info);
}

int 
//ClientManage::send_to_client(int fd, uint8_t* pkg, uint32_t pkglen)
ClientManage::send_to_client(uint32_t idx, uint8_t* pkg, uint32_t pkglen, bool flag)
{
	struct client_info_t* p_client = get_client_info(idx);
	if (!p_client) {
		ERROR_LOG("failed idx=%u", idx);
		return -1;
	}

	if (send_pkg_to_client(p_client->fdsess, pkg, pkglen) == -1) {
		ERROR_LOG("failed to send pkg to client: fd=%u", p_client->fdsess->fd);
		return -1;				    
	}

	DEBUG_LOG("SEND PACK TO CLIENT\t[fd=%d idx=%u]", p_client->fdsess->fd, idx);
	//p_client->info.waitcmd = 0;
	if (flag) {
		del_client(idx);
	}


	return 0;
}


