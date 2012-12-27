/**
 *============================================================
 *  @file      login_impl.hpp
 *  @brief     在这里编写登录服务器的具体实现。现在你看到的只是主登录的一个简单实现例子，
 *             如果你需要实现拉服务器列表、创建角色等功能，请继承Login类，然后实现具体细节。
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef LIBTAOMEEPP_LOGIN_IMPL_HPP_
#define LIBTAOMEEPP_LOGIN_IMPL_HPP_

#include <main_login/cli_proto.hpp>
#include <main_login/main_login.hpp>

/**
 * @brief 主登录类型
 */
typedef Login<CliProto1> MainLogin;

class SwitchService;

class TimeStampService;

class MoleLogin : public MainLogin {

	public:

		MoleLogin(SwitchService *mole_switch, TimeStampService *mole_timestamp);

		~MoleLogin();

	private:

		virtual bool init_service();
		virtual void fini_service()
		{
					// TODO:
					// 	
		}

		int get_other_svrs_pkg_len(int fd, const void* avail_data, int avail_len);

		int  proc_other_cmds(usr_info_t* usr, uint8_t* body, uint32_t bodylen);

		void proc_other_svrs_return(int fd, void* dbpkg, uint32_t pkglen);

		void on_other_svrs_fd_closed(int fd);

		int create_role_cmd(usr_info_t *usr, const uint8_t *body, uint32_t len);

		int mole_register_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

		int add_mole_flag_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
		
		int get_recommeded_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);

		int proc_other_dbproxy_return(usr_info_t* p, uint16_t cmdid, const uint8_t* body, uint32_t bodylen, uint32_t ret);

		int login_mole_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

		int process_login_mole_ret(usr_info_t *p, int ret);

		int get_time_stamp(usr_info_t *p, struct mole_login_out *user_login_info);

		int mulcast_friends_login(usr_info_t *p, struct mole_login_out *user_login_info);

		int get_ranged_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);

		void send_register_num_to_stat(usr_info_t *p);

		int check_session(usr_info_t *p, uint8_t *sessbody, int len);
		int check_session_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
	
		int get_login_version(usr_info_t* p, uint8_t *body, uint32_t bodylen);		

	private:

		SwitchService *mole_switch;

		TimeStampService *mole_timestamp;
};




/**
 * @brief 主登录实例
 */
extern MoleLogin login;

#endif // LIBTAOMEEPP_LOGIN_IMPL_HPP_
