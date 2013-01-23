#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "c_ip_allow.h"
using namespace std;

c_ip_allow::c_ip_allow()
{
	//初始化默认值
	strcpy(m_config_file_path, "");
	m_last_modify_time = time(NULL);
	m_ip_range_arr.clear();
}

c_ip_allow::~c_ip_allow()
{
	this->uninit();
}

int c_ip_allow::init(const char *file_path) 
{
	//检查参数
	if (file_path == NULL || strlen(file_path) > sizeof(m_config_file_path)) {
		return IP_ALLOW_E_INVALID_PARAM;
	}
	strcpy(m_config_file_path, file_path);

	//获取配置文件最后修改时间
	struct stat stat_buf;
	int ret_code = -1;
	ret_code = stat(m_config_file_path, &stat_buf);
	if (ret_code == -1) {
		return IP_ALLOW_E_OPERATION_ERROR;
	}
	m_last_modify_time = stat_buf.st_mtime;

	//解析配置文件，获取ip地址段数据
	FILE *fp = fopen(m_config_file_path, "r");
	if (fp == NULL) {
		return IP_ALLOW_E_OPERATION_ERROR;
	}
	char str_buf[1024];
	vector<ip_desc_t> ip_desc_arr;
	ip_desc_t desc;
	ip_range_t ip_range;
	while (fgets(str_buf, sizeof(str_buf), fp) != NULL) {
		this->trim_space(str_buf);
		if (this->parse_ip_range(str_buf, &ip_range) == 0) {
			desc.ip_addr = ip_range.ip_start;
			desc.ip_pos = IP_RANGE_START;
			ip_desc_arr.push_back(desc);

			desc.ip_addr = ip_range.ip_end;
			desc.ip_pos = IP_RANGE_END;
			ip_desc_arr.push_back(desc);
		}
	}
	fclose(fp);

	//将ip区间重叠的进行合并，保证m_ip_range_arr中的区间不重合，
	//类似括号匹配的操作
	sort(ip_desc_arr.begin(), ip_desc_arr.end());
	size_t i;
	size_t ip_desc_arr_size = ip_desc_arr.size();
	int top = 0;
	ip_range_t ip_merge;
	m_ip_range_arr.clear();
	for (i = 0; i < ip_desc_arr_size; i++) {
		if (ip_desc_arr[i].ip_pos == IP_RANGE_START) {
			top++;
			if (top == 1) {
				ip_merge.ip_start = ip_desc_arr[i].ip_addr;
			}
		} else if (ip_desc_arr[i].ip_pos == IP_RANGE_END) {
			top--;
			if (top == 0) {
				ip_merge.ip_end = ip_desc_arr[i].ip_addr;
				m_ip_range_arr.push_back(ip_merge);
			}
		} else {
			//只能是IP_REANGE_START和IP_RANGE_END
		}
	}

	return IP_ALLOW_S_SUCCESS;
}

/** 
* @brief 去掉字符串空格
* 
* @param str_buf 字符串地址
* 
* @return 
*/
int c_ip_allow::trim_space(char *str_buf) 
{
	size_t buf_len = strlen(str_buf);
	size_t i, j;
	for (i = 0, j = 0; j < buf_len; j++) {
		//发现注释符号
		if (str_buf[j] == '#') {
			break;
		}
		if (isspace(str_buf[j])) {
			continue;
		}
		str_buf[i++] = str_buf[j];
	}
	str_buf[i] = '\0';
	return IP_ALLOW_S_SUCCESS;
}

/** 
* @brief 解析ip字符串表示的ip地址范围
* 
* @param str_buf 点分形式的ip地址字符串，0.0.0.0表示一个ip地址，0.0.0.0/24表示一段ip地址
* @param ip_range 保存ip地址的范围数据结构
* 
* @return 
*/
int c_ip_allow::parse_ip_range(const char *str_buf, ip_range_t *ip_range)
{
	if (str_buf == NULL || ip_range == NULL) {
		return -1;
	}

	memset(ip_range, 0, sizeof(ip_range));
	size_t buf_len = strlen(str_buf);
	size_t i;
	for (i = 0; i < buf_len; i++) {
		if (str_buf[i] == '/') {
			break;
		}
	}

	//ip地址过长
	if (i > 16) {
		return -1;
	}

	struct in_addr in;
	char ip_str[20];
	strncpy(ip_str, str_buf, i);
	ip_str[i] = '\0';
	int ret = inet_aton(ip_str, &in);
	if (ret == 0) {
		return -1;
	}

	ip_range->ip_start = ntohl(in.s_addr);
	ip_range->ip_end = ntohl(in.s_addr);

	//没有主机网段
	if (i == buf_len) {
		return 0;
	}

	//获取主机字段
	size_t j;
	uint32_t host_len = 0;
	for (j = i + 1; j < buf_len; j++) {
		if (host_len > 32) {
			return -1;
		}
		if (!isdigit(str_buf[j])) {
			return -1;
		}
		host_len = host_len * 10 + str_buf[j] - '0';
	}

	uint32_t mash = (uint32_t)-1;//二进制32个1
	uint32_t ip_start_mash = (mash >> (32 - host_len)) << (32 - host_len);
	uint32_t ip_end_mash = ~ip_start_mash;
	ip_range->ip_start &= ip_start_mash;
	ip_range->ip_end |= ip_end_mash;

	return 0;
}

int c_ip_allow::uninit() 
{
	return 0;
}

int c_ip_allow::is_modified() 
{
	struct stat stat_buf;
	if (stat(m_config_file_path, &stat_buf) == -1) {
		return 0;
	}

	if (stat_buf.st_mtime != m_last_modify_time) {
		return 1;
	} else {
		return 0;
	}
}

int c_ip_allow::is_allowed(uint32_t ip_addr, bool is_net_seq, bool check_modified)
{
	if (check_modified && this->is_modified()) {
		if (this->init(m_config_file_path) == -1) {
			return IP_ALLOW_E_OPERATION_ERROR;
		}
	}

	if (is_net_seq) {
		ip_addr = ntohl(ip_addr);
	}

	int low = 0;
	int high = m_ip_range_arr.size() - 1;
	while (low <= high) {
		int mid = (low + high) / 2;
		if (ip_addr < m_ip_range_arr[mid].ip_start) {
			high = mid - 1;
		} else {
			low = mid + 1;
		}
	}

	if (high < 0) {
		return IP_ALLOW_S_IP_DENIED;
	}
	if (ip_addr >= m_ip_range_arr[high].ip_start 
			&& ip_addr <= m_ip_range_arr[high].ip_end) {
		return IP_ALLOW_S_IP_ACCEPTTED;
	} else {
		return IP_ALLOW_S_IP_DENIED;
	}
}

int c_ip_allow::is_allowed(const char *ip_addr, bool check_modified)
{
	struct in_addr in;
	if (inet_aton(ip_addr, &in) == 0) {
		return IP_ALLOW_E_OPERATION_ERROR;
	}

	return this->is_allowed(in.s_addr, true, check_modified);

}

int create_ip_allow_instance(i_ip_allow** pp_instance)
{
	if (pp_instance == NULL) {
		return -1;
	}

	c_ip_allow *p_instance = new(nothrow) c_ip_allow;
	if (p_instance == NULL) {
		return -1;
	}

	*pp_instance = dynamic_cast<i_ip_allow*> (p_instance);
	return 0;
}
