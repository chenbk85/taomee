/**
* @file do_handle.h
* @brief serverbench接口处理相关
* @author lemon@taomee.com
* @version 1.0.0.0
* @date 2010-08-04
*/
#ifndef __DO_HANDLE_H__
#define __DO_HANDLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <vector>
#include <map>
#include <string>

#include "newbench.h"

#include "functions.h"

///定义公共包头结构
typedef struct {
	uint32_t pkg_len;
	uint32_t seq_num;
	uint16_t cmd_id;
	uint32_t status_code;
	uint32_t user_id;
}__attribute__((packed)) common_protocol_header_t;

///渠道号和渠道码结构
typedef struct {
	uint16_t chnl_id;
	std::string chnl_key;
}verify_chnl_info_t;

typedef struct {
	/**
	* @brief 协议功能命令号
	*/
	uint16_t cmd_id;
	/**
	* @brief 协议功能函数入口
	*
	* @param user_id 把包头的user_id拷贝过来
	* @param req_body_buf 请求包体，不包含验证包头
	* @param req_body_buf_len 请求包体长度，不包含验证包头长度
	* @param ack_body_buf 应答缓冲区，保存应答协议内容
	* @param ack_body_buf_len 传入应答缓冲的长度
	* @param ack_body_len 应答协议内容长度
	* @param sk 网络相关参数
	*/
	uint32_t (* do_cmd)(uint32_t user_id, const char *req_body_buf, int req_body_len, char *ack_body_buf, int *ack_len, skinfo_t *sk);
	/**
	* @brief 标记是否需要验证，包体是否含有验证包头
	*/
	uint8_t flag;
}command_entry_t;

#define ENTRY_FLAG_NORMAL 0
#define ENTRY_FLAG_NEED_VERIFY_CHNL (1)
#define ENTRY_FLAG_NEED_VERIFY_IP (1 << 1)

#define DO_PROCESS_S_SUCCESS 					0
#define DO_PROCESS_E_INVALID_PACKAGE_LENGTH		1
#define DO_PROCESS_E_INVALID_COMMAND_ENTRY		2
#define DO_PROCESS_E_INVALID_COMMAND_ID			3
#define DO_PROCESS_E_CHANNEL_VERIFY_FAILED		4
#define DO_PROCESS_E_IP_DENIED 					5
#define DO_PROCESS_E_DO_COMMAND_SYSTEM_ERROR	6

/**
* @brief 初始化操作
*
* @param max_proto_buf_size 设置协议缓冲区的最大长度
* @param chnl_id 渠道id数组
* @param chnl_key 渠道id数组对应的验证码
* @param entry_arr 功能函数数组地址
* @param entry_arr_size 功能函数数组长度
* @param ip_allow_conf_file ip限制的配置文件
*
* @return 成功返回0， 失败返回-1
*/
int do_handle_init(int pid, int max_proto_buf_size, verify_chnl_info_t *chnl_list, int chnl_num,
		command_entry_t *entry_arr, int entry_arr_size, const char *ip_allow_conf_file);

/**
* @brief serverbench处理handle_process
*
* @param recv_buf 接收缓冲区
* @param recv_len 接收缓冲区接收到的长度
* @param send_buf 发送缓冲区
* @param send_len 发送缓冲区需要发送内容长度
* @param sk 网络相关参数
*
* @return 0成功处理，非0失败
*/
int do_handle_process(const char *recv_buf, int recv_len, char **send_buf, int *send_len, skinfo_t *sk);

/**
* @brief serverbench处理handle_input
*
* @param recv_buf 接收缓冲区
* @param recv_len 接收缓冲区接收到的长度
*
* @return 0需要继续接收，-1接收失败，大于0接收成功
*/
int do_handle_input(const char *recv_buf, int recv_len);

/**
* @brief serverbench处理handle_fini
*
* @return 0
*/
int do_handle_fini();

/// 校验头
typedef struct {
	uint16_t chnl_id;
	uint8_t vfy_code[32];
} __attribute__((packed)) chnlhash32_vfy_header_t;

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
int fill_chnlhash32(chnlhash32_vfy_header_t* vfh, uint16_t chnl_id, const char* key, const char* data, int data_len);

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
int verify_chnlhash32(const chnlhash32_vfy_header_t* vfh, const char* key, const char* data, int data_len);

#endif
