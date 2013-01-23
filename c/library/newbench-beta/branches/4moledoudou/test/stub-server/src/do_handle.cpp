#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <map>

#include "newbench.h"
#include "do_handle.h"
#include "c_ip_allow.h"
#include "functions.h"

/**
* @brief 接收缓冲区
*/
static char *g_ack_buf = NULL;
/**
* @brief 接收缓冲区的最大长度
*/
static int g_max_proto_buf_size = 0;

/**
* @brief ip限制类实例
*/
i_ip_allow *g_ip_allow_mgr = NULL;

/**
* @brief 渠道映射表
*/
std::map<uint16_t, std::string> g_chnl_map;

/**
* @brief 功能函数入口表
*/
static command_entry_t *g_cmd_entry_arr = NULL;
/**
* @brief 功能函数入口表长度
*/
int g_cmd_entry_arr_size = 0;

/**
* @brief 处理MD5码的缓冲区
*/
static char *g_md5_hash_buf = NULL;

///qsort比较函数
int command_entry_cmp(const void *p, const void *q)
{
	command_entry_t *s = (command_entry_t *)p;
	command_entry_t *t = (command_entry_t *)q;
	return s->cmd_id - t->cmd_id;
}

///对入口函数按命令id排序，并检查入口函数是否存在
int check_command_entry(command_entry_t *entry_arr, int entry_arr_size)
{
	int i;
	qsort(entry_arr, entry_arr_size, sizeof(command_entry_t), command_entry_cmp);
	for (i = 0; i < entry_arr_size; i++) {
		if (entry_arr[i].do_cmd == NULL) {
			return -1;
		}
	}
	return 0;
}

/**
* @brief 初始化相关资源
*
* @param pid serverbench进程类型，work进程将初始化资源，其他进程只设置最大协议包长
* @param max_proto_buf_size 设置最大协议包长
* @param chnl_info_arr 渠道信息描述列表
* @param chnl_arr_size 渠道信息描述列表的长度
* @param entry_arr 功能函数入口表
* @param entry_arr_size 功能函数入口表大小
* @param ip_allow_conf_file ip限制配置文件路径
*
* @return 成功执行返回0，失败返回-1
*/
int do_handle_init(int pid, int max_proto_buf_size, verify_chnl_info_t *chnl_info_arr, int chnl_arr_size,
		command_entry_t *entry_arr, int entry_arr_size, const char *ip_allow_conf_file)
{
	g_max_proto_buf_size = max_proto_buf_size;
	if (pid != PROC_WORK) {
		return 0;
	}

	if (entry_arr == NULL) {
#ifdef _BENCHAPI_H
		ERROR_LOG("command entry list must not be null");
#endif
		return -1;
	}

	//初始化协议缓冲区空间
	g_ack_buf = (char *)malloc(sizeof(char) * g_max_proto_buf_size);
	if (g_ack_buf == NULL) {
#ifdef _BENCHAPI_H
		ERROR_LOG("malloc %d size memory failed", sizeof(char) * g_max_proto_buf_size);
#endif
		return -1;
	}

	//初始化MD5hash缓冲区
	g_md5_hash_buf = (char *)malloc(sizeof(char) * (g_max_proto_buf_size + 200));
	if (g_md5_hash_buf == NULL) {
#ifdef __DEBUG__
		ERROR_LOG("malloc %d size memory failed", sizeof(char) * (g_max_proto_buf_size + 200));
#endif
		return -1;
	}

	//初始化功能函数入口数组，将用户的入口数组拷贝过来
	g_cmd_entry_arr = (command_entry_t *)malloc(sizeof(command_entry_t) * entry_arr_size);
	g_cmd_entry_arr_size = entry_arr_size;
	if (g_cmd_entry_arr == NULL) {
#ifdef _BENCHAPI_H
		ERROR_LOG("malloc %d size memory failed", sizeof(command_entry_t) * entry_arr_size);
#endif
		return -1;
	}
	int i;
	for (i = 0; i < entry_arr_size; i++) {
		g_cmd_entry_arr[i] = entry_arr[i];
	}

	//检查命令入口数组是否有序，功能函数是否存在
	if (check_command_entry(g_cmd_entry_arr, g_cmd_entry_arr_size) == -1) {
		return -1;
	}

	//初始化渠道映射表
	g_chnl_map.clear();
	if (chnl_info_arr != NULL) {
		int i;
		for (i = 0; i < chnl_arr_size; i++) {
			g_chnl_map.insert(std::pair<uint16_t, std::string>(chnl_info_arr[i].chnl_id, chnl_info_arr[i].chnl_key));
		}
	} else {
		//渠道映射表为空，查看功能函数是否需要渠道验证，全部不需要验证的为正确
		int i;
		for (i = 0; i < g_cmd_entry_arr_size; i++) {
			if (g_cmd_entry_arr[i].flag & ENTRY_FLAG_NEED_VERIFY_CHNL) {
#ifdef _BENCHAPI_H
				ERROR_LOG("protocol need channel verification, but channel list is null.");
#endif
				return -1;
			}
		}
	}

	//初始化ip允许类
	if (ip_allow_conf_file != NULL) {
		if (create_ip_allow_instance(&g_ip_allow_mgr) == -1) {
#ifdef _BENCHAPI_H
			ERROR_LOG("create ip allow instance failed");
#endif
			return -1;
		}

		if (g_ip_allow_mgr->init(ip_allow_conf_file) == -1) {
#ifdef _BENCHAPI_H
			ERROR_LOG("ip allow intance init config file failed");
#endif
			return -1;
		}
	} else {
		//不设置ip限制功能，查看功能函数是否需要ip限制，全部不需要验证的为正确
		int i;
		for (i = 0; i < g_cmd_entry_arr_size; i++) {
			if (g_cmd_entry_arr[i].flag & ENTRY_FLAG_NEED_VERIFY_IP) {
#ifdef _BENCHAPI_H
				ERROR_LOG("protocol need ip verification, but ip conf file is null");
#endif
				return -1;
			}
		}
	}
	return 0;
}

/**
* @brief 设置公共包头信息
*
* @param h 包头结构体
* @param pkg_len 协议包长
* @param seq_num 序列号
* @param cmd_id 功能命令号
* @param status_code 状态码
* @param user_id 用户米米号
*
* @return h不为NULL返回0，否则返回-1
*/
int set_common_protocol_header(common_protocol_header_t *h, uint32_t pkg_len, uint32_t seq_num,
				uint16_t cmd_id, uint32_t status_code, uint32_t user_id)
{
	if (h == NULL) {
		return -1;
	}
	h->pkg_len = pkg_len;
	h->seq_num = seq_num;
	h->cmd_id = cmd_id;
	h->status_code = status_code;
	h->user_id = user_id;
	return 0;
}

/**
* @brief handle_process处理函数
*
* @param recv_buf 接收缓冲区
* @param recv_len 接收缓冲区数据长度
* @param send_buf 发送缓冲区
* @param send_len 发送缓冲区数据长度
* @param sk 网络相关参数
*
* @return 成功执行返回0，失败返回-1
*/
int do_handle_process(const char *recv_buf, int recv_len, char **send_buf, int *send_len, skinfo_t *sk)
{
    send_data(sk->connection_id, "welcome", strlen("welcome"), 1, 0);

	if ((u_int)recv_len < sizeof(common_protocol_header_t)) {
		return -1;
	}

	//二分查找命令号对应的命令函数入口结构体
	common_protocol_header_t *req_h = (common_protocol_header_t *)recv_buf;
	int low = 0, high = g_cmd_entry_arr_size - 1;
	while (low <= high) {
		int mid = (low + high) / 2;
		if (req_h->cmd_id < g_cmd_entry_arr[mid].cmd_id) {
			high = mid - 1;
		} else {
			low = mid + 1;
		}
	}
	low--;
	if (low < 0 || low >= (int)g_cmd_entry_arr_size) {
		return -1;
	}

	//设置应答包头
	*send_buf = g_ack_buf;
	*send_len = sizeof(common_protocol_header_t);
	common_protocol_header_t *ack_h = (common_protocol_header_t *)g_ack_buf;

	//验证渠道
	if (g_cmd_entry_arr[low].flag & ENTRY_FLAG_NEED_VERIFY_CHNL) {
		chnlhash32_vfy_header_t vfy_h;
		memcpy(&vfy_h, recv_buf + sizeof(common_protocol_header_t), sizeof(vfy_h));
		if (g_chnl_map.find(vfy_h.chnl_id) == g_chnl_map.end()) {
			set_common_protocol_header(ack_h, sizeof(common_protocol_header_t), req_h->seq_num,
					req_h->cmd_id, DO_PROCESS_E_CHANNEL_VERIFY_FAILED, req_h->user_id);
			return DO_PROCESS_E_CHANNEL_VERIFY_FAILED;
		}
		const char *chnl_key = g_chnl_map[vfy_h.chnl_id].c_str();
		if (verify_chnlhash32(&vfy_h,
				chnl_key,
				recv_buf + sizeof(common_protocol_header_t) + sizeof(chnlhash32_vfy_header_t),
				req_h->pkg_len - sizeof(common_protocol_header_t) - sizeof(chnlhash32_vfy_header_t)) == -1) {
			set_common_protocol_header(ack_h, sizeof(common_protocol_header_t), req_h->seq_num,
					req_h->cmd_id, DO_PROCESS_E_CHANNEL_VERIFY_FAILED, req_h->user_id);
			return DO_PROCESS_E_CHANNEL_VERIFY_FAILED;
		}
	}

	//验证ip是否允许
	if (g_cmd_entry_arr[low].flag & ENTRY_FLAG_NEED_VERIFY_IP) {
#ifdef __DEBUG__
		struct in_addr in;
		in.s_addr = sk->remote_ip;
		printf("need verify ip, remote ip: %u[%s]\n", sk->remote_ip, inet_ntoa(in));
#endif
		if (g_ip_allow_mgr->is_allowed(sk->remote_ip, true, true) == 0) {
			set_common_protocol_header(ack_h, sizeof(common_protocol_header_t), req_h->seq_num,
			req_h->cmd_id, DO_PROCESS_E_IP_DENIED, req_h->user_id);
			return DO_PROCESS_E_IP_DENIED;
		}
	}

	//设置请求包体的地址和包体的长度，这里的包体要去掉验证包头
	uint32_t user_id = req_h->user_id;
	char *req_b = (char *)(recv_buf + sizeof(common_protocol_header_t));
	int req_b_len = req_h->pkg_len - sizeof(common_protocol_header_t);
	if (g_cmd_entry_arr[low].flag & ENTRY_FLAG_NEED_VERIFY_CHNL) {
		req_b = req_b + sizeof(chnlhash32_vfy_header_t);
		req_b_len = req_b_len - sizeof(chnlhash32_vfy_header_t);
	}

	//设置应答包体的地址
	char *ack_b = (char *)(g_ack_buf + sizeof(common_protocol_header_t));
	int ack_len = sizeof(ack_b);

	//执行协议服务并设置好应答协议
	ack_h->status_code = g_cmd_entry_arr[low].do_cmd(user_id, req_b, req_b_len, ack_b, &ack_len, sk);
	ack_h->pkg_len = sizeof(common_protocol_header_t) + ack_len;
	ack_h->seq_num = req_h->seq_num;
	ack_h->cmd_id = req_h->cmd_id;
	ack_h->user_id = req_h->user_id;

	*send_buf = g_ack_buf;
	*send_len = ack_h->pkg_len;

	return 0;
}

/**
* @brief handle_input 处理函数
*
* @param recv_buf 接收缓冲区
* @param recv_len 接收缓冲区数据长度
*
* @return 0继续接收，-1发生错误，大于0接收成功
*/
int do_handle_input(const char *recv_buf, int recv_len)
{
	uint32_t pkg_len;
	if ((u_int)recv_len < sizeof(pkg_len)) {
		return 0;
	}

	pkg_len = *(uint32_t *)recv_buf;
	if ((u_int)recv_len < pkg_len) {
		return 0;
	}

	if (pkg_len < sizeof(common_protocol_header_t)) {
#ifdef _BENCHAPI_H
		ERROR_LOG("pkg_len %d is smaller than protocol header size", pkg_len);
#endif
		return -1;
	}
	if (pkg_len > (u_int)g_max_proto_buf_size) {
#ifdef _BENCHAPI_H
		ERROR_LOG("pkg_len is larger than max protocol buf size, pkg_len = %u, max_proto_buf_size = %u\n", pkg_len, g_max_proto_buf_size);
#endif
		return -1;
	}
	return pkg_len;
}

/**
* @brief handle_fini处理函数
*
* @return 返回0
*/
int do_handle_fini()
{
	if (g_ack_buf != NULL) {
		free(g_ack_buf);
		g_ack_buf = NULL;
	}
	if (g_cmd_entry_arr != NULL) {
		free(g_cmd_entry_arr);
		g_cmd_entry_arr = NULL;
	}
	if (g_ip_allow_mgr != NULL) {
		delete g_ip_allow_mgr;
		g_ip_allow_mgr = NULL;
	}
	if (g_md5_hash_buf != NULL) {
		free(g_md5_hash_buf);
		g_md5_hash_buf = NULL;
	}
	return 0;
}

/**
* @brief 获取渠道的校验码
*
* @param vfh 渠道校验协议包头
* @param chnl_id 渠道号
* @param key 渠道码
* @param data 包体内容
* @param data_len 包体内容长度
*
* @return 成功返回0，失败返回-1
*/
int fill_chnlhash32(chnlhash32_vfy_header_t* vfh, uint16_t chnl_id, const char* key, const char* data, int data_len)
{
	vfh->chnl_id = chnl_id;

	int hash_body_len = sprintf(g_md5_hash_buf, "channelId=%d&securityCode=%s&data=", vfh->chnl_id, key);
	if (hash_body_len <= 0) {
		ERROR_LOG("Verify hash: sprintf error.");
		return -1;
	}

	memcpy(g_md5_hash_buf + hash_body_len, data, data_len);
	hash_body_len += data_len;
	char md_buf[33];
	memcpy(vfh->vfy_code, MD5_32(g_md5_hash_buf, hash_body_len, md_buf), 32);

	return 0;
}

/**
* @brief 校验协议包头
*
* @param vfh 协议包头
* @param key 渠道码
* @param data 包体内容
* @param data_len 包体内容长度
*
* @return 校验正确返回0，出错返回-1
*/
int verify_chnlhash32(const chnlhash32_vfy_header_t* vfh, const char* key, const char* data, int data_len)
{
	int hash_body_len = sprintf(g_md5_hash_buf, "channelId=%d&securityCode=%s&data=", vfh->chnl_id, key);
	if (hash_body_len <= 0) {
		ERROR_LOG("Verify hash: sprintf error.");
		return -1;
	}

	memcpy(g_md5_hash_buf + hash_body_len, data, data_len);
	hash_body_len += data_len;

	char md_buf[33];
	if (memcmp(vfh->vfy_code, MD5_32(g_md5_hash_buf, hash_body_len, md_buf), 32) == 0)
		return 0;

	return -1;
}
