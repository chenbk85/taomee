/** 
* @file c_ip_allow.h
* @brief 从文件读取允许访问的ip，这些ip可以访问相关系统
* @author lemon@taomee.com
* @version 1.0.0.0
* @date 2010-07-20
*/

#ifndef __I_IP_ALLOW_H__
#define __I_IP_ALLOW_H__

#include <stdint.h>
#include <sys/types.h>
#include <vector>

/**
* @brief 操作成功
* */
#define IP_ALLOW_S_SUCCESS 0

/** 
* @brief 操作成功，但是ip不在允许的段内
*/
#define IP_ALLOW_S_IP_DENIED 0

/** 
* @brief 操作成功，ip在允许的段内
*/
#define IP_ALLOW_S_IP_ACCEPTTED 1

/** 
* @brief 操作失败，调用其他函数出错，或ip地址不合法，或读取配置文件失败等
*/
#define IP_ALLOW_E_OPERATION_ERROR -1

/** 
* @brief 操作失败，传入的参数有误
*/
#define IP_ALLOW_E_INVALID_PARAM -2

/** 
* @brief ip限制类的接口描述
*/
class i_ip_allow
{
public:

	virtual ~i_ip_allow() {};

	/** 
	* @brief 解析配置文件
	* 
	* @param file_path 配置文件路径
	* 
	* @return 成功返回0，错误返回-1 
	*/
	virtual int init(const char *file_path) = 0;

	/** 
	* @brief 撤销
	* 
	* @return 成功返回0，错误返回-1
	*/
	virtual int uninit() = 0;

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
	virtual int is_allowed(uint32_t ip_addr, bool is_net_seq = false, bool check_modified = false) = 0;

	/** 
	* @brief 判断ip_addr是否包含于允许访问的ip地址段中
	* 
	* @param ip_addr 点分形式的ip字符串
	*
	* @param check_modified 指定是否需要检查配置文件有没有被修改，如果check_modified为true且配置文件被修改，则配置文件被重新加载
	*
	* @return 允许返回1，不允许返回0
	*/
	virtual int is_allowed(const char *ip_addr, bool check_modified = false) = 0;


};

/** 
* @brief 创建i_ip_allow实例
* 
* @param pp_instance i_ip_allow实例指针的引用
* 
* @return 成功返回0，失败返回-1
*/
int create_ip_allow_instance(i_ip_allow** pp_instance);

#endif
