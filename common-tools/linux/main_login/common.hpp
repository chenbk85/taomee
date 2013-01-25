/**
 *============================================================
 *  @file      common.hpp
 *  @brief     定义了公共的变量和函数
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef LIBTAOMEEPP_MAIN_LOGIN_COMMON_HPP_
#define LIBTAOMEEPP_MAIN_LOGIN_COMMON_HPP_

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/project/types.h>
#include <libtaomee/project/stat_agent/msglog.h>
}

/**
  * @brief 公共常量
  */
enum common_constants_t {
	/*! login using email */
	login_by_email			= 8888,
	/*! 邮件最大长度 */
	email_max_len			= 64,
	/*! 二进制密码最大长度 */
	password_bin_max_len	= 16,
	/*! 字符形式密码最大长度 */
	password_str_max_len	= 32,

	/*! 登录session长度 */
	login_session_len		= 16,

	/*! Length of a Verification Image ID */
	verif_img_id_len		= 16,
	/*! Length of a Verification Code */
	verif_code_len			= 6,

	/*! 游戏项目最大数目 */
	gameprj_max_num			= 32,
};

/**
  * @brief 统计信息ID号
  */
enum stat_id_t {
	/*! 主登录统计主ID */
	stat_base_id					= 0x06050000,

	/*! 登录渠道 */
	stat_login_channel_offset		= 0,
	/*! 非法的登录渠道 */
	stat_undefined_channel_offset	= 0x0000FFFF,

	/*! 用户密码错误过多被屏蔽统计项 */
	stat_banned_user_offset			= 0x00001000,
	/*! 用户输错密码统计项 */
	stat_wrong_password_offset		= 0x00001001,
};

/**
  * @brief 统计信息ID号
  */
enum warning_service_t {
	/*! db出错 */
    warning_db      =   1,
	/*! switch出错 */
    warning_switch  =   2,
	/*! 登录服务器出错 */
    warning_login   =   3,
	/*! Online出错 */
    warning_online  =   4,
	/*! dbproxy出错 */
    warning_dbproxy =   5,
};

/**
  * @brief 客户端协议命令号
  */
enum cli_cmd_id_t {
	/*! renew verification image */
	cliproto_renew_verifimg	= 101,
	/*! check if player actived */
	cliproto_check_active	= 102,
	/*! user login */
	cliproto_user_login		= 103,
	/*! user put active code */
	cliproto_set_active_code	= 104,

	/*! for testing if this server is OK */
	cliproto_probe			= 30000
};

/**
  * @brief 主登录正在使用的错误码。注意：副登录使用错误码的话，请从6001开始。
  */
enum cli_errno_t {
	/*! 系统出错 */
	clierr_system_error			= 5001,
	/*! user is banned */
	clierr_user_is_banned			= 5002,
	/*! 密码错误 */
	clierr_wrong_password			= 5003,
	/*! 用户不存在 */
	clierr_userid_not_found			= 5005,
	/*! 用户被暂时禁止登录 */
	clierr_wrong_password_too_much		= 5009,
	/*! 激活码不正确 */
	clierr_wrong_active_code		= 5010,
};

/**
  * @brief 和数据库的协议命令号
  */
enum dbproxy_cmd_t {
	/*! 登录验证 */
	dbproto_login					= 0xA021,
	/*! 登录验证 */
	//dbproto_login_with_verif_img	= 0xA026,
	dbproto_login_with_verif_img	= 0xA032,
	/*! 获取验证码 */
	dbproto_get_verif_img		= 0xA027,
	dbproto_get_verif_img_anytime	= 0xA031,
	/*! 设置创建角色标志位 */
	//dbproto_add_game_flag		= 0xA123,
	dbproto_add_game_flag		= 0xA134,
	/*! 验证是否激活过 */
	dbproto_check_active		= 0xA029,
	/*! 验证激活码 */
	dbproto_set_active_code		= 0xA128,
	/*! 邮件推送 */
	dbproto_post_mail			= 0xC080,
	/*! 查看是否是米饭 */
	dbproto_check_meefan			= 0x0060,
};

/**
  * @brief 数据库返回的错误码
  */
enum db_err_t {
	/*! system error */
	dberr_sys_error		= 1001,
	/*! db error */
	dberr_db_error		= 1002,
	/*! network error */
	dberr_net_error		= 1003,
	/*! time out */
	dberr_timeout		= 1017,
	/*! wrong password */
	dberr_wrong_passwd	= 1103,
	/*! no such user id */
	dberr_no_uid		= 1105,
	/*! user is banned */
	dberr_user_banned	= 1107,
	/*! wrong active code */
	dberr_active_code	= 3001,
};

#pragma pack(1)

struct cli_login_req_t {
	uint8_t		passwd[password_str_max_len];
	uint32_t  	channel;
	uint32_t    which_game;
	uint32_t    user_ip;
	char		img_id[verif_img_id_len];
	char		verif_code[verif_code_len];
};

struct login_tmpinfo_t {
	uint8_t		passwd[password_str_max_len];
	uint32_t  	channel;
	uint32_t  	cli_channel_id;
	uint32_t    which_game;
	uint32_t    user_ip;
	uint8_t		email[email_max_len];
	char		img_id[verif_img_id_len];
	char		verif_code[verif_code_len];
	uint8_t		verif_pass;
};

#pragma pack()

// forward declaration
struct usr_info_t;

extern const char* stat_file;

/**
  * @brief 记录统计信息
  * @param stat_id 统计id号
  * @param data 统计数据
  * @param len 数据长度
  */
inline void statistic_msglog(uint32_t stat_id, const void* data, size_t len)
{
	int msgret = msglog(stat_file, stat_id, get_now_tv()->tv_sec, data, len);
	if (msgret != 0) {
		WARN_LOG("statistic log error: stat_id=%X ret=%d", stat_id, msgret);
	}
}

/**
  * @brief 发送报警信息
  * @param p 用户
  * @param server_id 服务Id
  * @param cmdid 命令号
  */
void send_warning(const usr_info_t* p, int server_id, int cmdid);

/**
  * @brief 发送报警信息。一分钟内，同一个svr累计send_warning超过10次，则配置文件phone_numbers配置项的手机号码都会收到报警短信。
  * @param svr 出错的服务器名称。db, dbproxy, switch, cachesvr等。
  * @param uid 出问题的米米号
  * @param cmd 出问题的命令号，可以填db的命令号，也可以填和客户端的命令号
  * @param hex 0则命令号按十进制打印，1则命令号按十六进制打印。一般db的命令号需要按十六进制打印。
  * @param ip 出问题的机器的ip地址。如果没有，可以不填，或者填0。
  */
void send_warning(const char* svr, userid_t uid, uint32_t cmd, int hex, const char* ip = "");

void init_warning_system();

#endif // LIBTAOMEEPP_MAIN_LOGIN_COMMON_HPP_
