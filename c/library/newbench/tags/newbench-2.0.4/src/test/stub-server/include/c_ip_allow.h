/** 
* @file c_ip_allow.h
* @brief 从文件读取允许访问的ip，这些ip可以访问相关系统
* @author lemon@taomee.com
* @version 1.0.0.0
* @date 2010-07-20
*/

#ifndef __IP_ALLOW_H__
#define __IP_ALLOW_H__

#include <stdint.h>
#include <sys/types.h>
#include <vector>

#include "i_ip_allow.h"

/** 
* @brief 配置文件名称大小
*/
#define MAX_CONFIG_FILE_PATH_LEN 4096

/** 
* @brief ip范围的起始位置
*/
#define IP_RANGE_START 1

/** 
* @brief ip范围的终止位置
*/
#define IP_RANGE_END 2

/** 
* @brief ip限制类，从配置文件读取允许的ip地址，对每一个用户输入的ip进行判断，是否在配置文件允许的ip段内
*/
class c_ip_allow : public i_ip_allow
{
public:

	c_ip_allow();

	~c_ip_allow();

	/** 
	* @brief 解析配置文件
	* 
	* @param file_path 配置文件路径
	* 
	* @return 成功返回0，错误返回-1 
	*/
	int init(const char *file_path);

	/** 
	* @brief 撤销
	* 
	* @return 成功返回0，错误返回-1
	*/
	int uninit();

	/** 
	* @brief 判断ip_addr是否是包含于允许访问的ip地址段中
	* 
	* @param ip_addr 整形ip
	* 
	* @param is_net_seq 指定ip_addr是否是网络字节序
	*
	* @param check_modified 指定是否需要检查配置文件有没有被修改，如果check_modified为true且配置文件被修改，则配置文件被重新加载
	* 
	* @return 允许返回1，不允许返回0
	*/
	int is_allowed(uint32_t ip_addr, bool is_net_seq, bool check_modified);

	/** 
	* @brief 判断ip_addr是否包含于允许访问的ip地址段中
	* 
	* @param ip_addr 点分形式的ip字符串
	*
	* @param check_modified 指定是否需要检查配置文件有没有被修改，如果check_modified为true且配置文件被修改，则配置文件被重新加载
	*
	* @return 允许返回1，不允许返回0
	*
	* @note LOG(n)时间复杂度
	*/
	int is_allowed(const char *ip_addr, bool check_modified);

private:

	/** 
	* @brief ip地址范围结构体
	* @param ip_start 起始ip地址
	* @param ip_end 终止ip地址
	*/
	typedef struct ip_range_t {
		/** 
		* @brief 起始ip地址
		*/
		uint32_t ip_start;

		/** 
		* @brief 终止ip地址
		*/
		uint32_t ip_end;
	} ip_range_t;

	/** 
	* @brief ip地址描述，用于合并ip地址段
	* @param ip_addr 整形ip地址
	* @param ip_pos ip地址段的开始，还是结束
	*/
	typedef struct ip_desc_t {
		/** 
		* @brief 整形ip地址
		*/
		uint32_t ip_addr;

		/** 
		* @brief 描述这是ip段的开始，还是结束位置
		*/
		uint32_t ip_pos;

		/** 
		* @brief ip_desc_t结构体的小于号重载，用于排序
		* 
		* @param a 
		* @param b
		* 
		* @return 一个bool类型
		*/
		friend bool operator<(ip_desc_t a, ip_desc_t b) {
			if (a.ip_addr == b.ip_addr) {
				return a.ip_pos < b.ip_pos;
			} else {
				return a.ip_addr < b.ip_addr;
			}
		}
	} ip_desc_t;

	/** 
	* @brief 判断配置文件是否被修改
	* 
	* @return 返回0表示配置未被修改或操作失败，1表示配置已被修改
	*/
	int is_modified();

	/** 
	* @brief 将字符串中的空格去掉，并判断ip地址字符串是否包含非法字符
	* 
	* @param str_buf ip地址字符串，并保存处理后结果
	* 
	* @return -1表示ip地址字符串包含非法字符(非数字、'.'、'/'的字符)，0表示操作成功
	*/
	int trim_space(char *str_buf);

	/** 
	* @brief 解析字符串的ip地址范围
	* 
	* @param str_buf ip地址字符串，常规的ip地址字符串或包含主机号的ip地址段字符串
	* @param ip_range 保存解析后的ip地址段
	* 
	* @return 0表示解析成功，-1表示失败
	*/
	int parse_ip_range(const char *str_buf, ip_range_t *ip_range);

	/** 
	* @brief 保存配置文件路径
	*/
	char m_config_file_path[MAX_CONFIG_FILE_PATH_LEN];

	/** 
	* @brief 保存配置最后一次修改的时间，用于判断当前配置文件有没有被修改
	*/
	time_t m_last_modify_time;

	/** 
	* @brief 保存允许ip地址范围的数组
	*/
	std::vector<ip_range_t> m_ip_range_arr;

};

#endif
