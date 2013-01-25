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
#include <errno.h>

extern "C" {
	#include <libtaomee/tm_dirty/tm_dirty.h>
}

#include "libtype.h"


/**
 * @brief 主登录类型
 */
typedef Login<CliProto2> MainLogin;

class mole2_login : public MainLogin {

public:
	mole2_login():m_switch_fd(-1)
	{

	};	

	~mole2_login()
	{
		fini_thttpd(); 
	};

private:
	/**
	 * @brief 副登录如果需要初始化额外的内容，比如说读取配置文件等等，可以通过实现这个接口来完成
	 * @return 正常的话，返回true，如果出现任何初始化失败的状况，请返回false。
	 */
	bool init_service();

	/**
	 * @brief 副登录如果需要清理额外的内容，比如说释放内存等等，可以通过实现这个接口来完成
	 */
	void fini_service();

	/**
	 * @brief 副登录如果需要解析其它服务器来包（比如说Switch）的长度，可以通过实现这个接口来完成。
	 *        一般来说，因为服务器来包包头都一样，可以直接在这里调用get_dbproxy_pkg_len。
	 * @return 解析出来的数据包长度，或者如果失败的话，返回-1 
	 */
	int get_other_svrs_pkg_len(int fd, const void* avail_data, int avail_len);


	/**
	 * @brief 副登录如果需要处理其它的客户端协议命令，比如说拉去服务器列表/创建角色，可以通过实现这个接口来完成
	 * @return 一切正常的话，返回0，如果客户端发包非法，或者处理过程中出现系统级的错误，请返回-1
	 */
	int  proc_other_cmds(usr_info_t* usr, uint8_t* body, uint32_t bodylen);

	/**
	 * @brief 副登录如果需要处理其它的dbproxy回报，比如说创建角色的返回包，可以通过实现这个接口来完成
	 * @return 一切正常的话，返回0，如果DBproxy返回包非法，或者处理过程中出现系统级的错误，请返回-1
	 */
	int  proc_other_dbproxy_return(usr_info_t* p, uint16_t cmd, const uint8_t* body, uint32_t bodylen, uint32_t ret);

	/**
	 * @brief 副登录如果需要处理其它服务器的返回，比如说Switch的返回，可以通过实现这个接口来完成
	 */
	void proc_other_svrs_return(int fd, void* dbpkg, uint32_t pkglen);

	/**
	 * @brief 副登录如果需要处理其它服务器断开连接的情况，比如说Switch断开连接，可以通过实现这个接口来完成
	 */
	void on_other_svrs_fd_closed(int fd);

private:
	//client
	int get_recommeded_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
	int get_ranged_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
	int create_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
#ifdef mole2_key
	int check_mfan(usr_info_t* p, const uint8_t* body, int len);
	int activation_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
#endif
public:
	int send_to_switch(usr_info_t* p, const uint8_t *buf, uint32_t length);

	void init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd);

	int get_backup_svr_list(usr_info_t* p);
	
	int send_recommeded_svr_list(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt, int max_online_id);

	int get_recommended_svr_list(usr_info_t* p);
	
	int send_ranged_svrlist(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt);
	/**
	  * @brierf Send a request package to switch server, then get the respond, but do not parse it.
	  * @param  uid. user id.
	  * @param    friendcount. haw many friends.
	  * @param    allfriendid. store the ids in an array
	  * @return     int. 0 on success. less than 0 on error.
	  */
	int get_ranged_svr_list(usr_info_t* p, int start_id, int end_id);

	int my_read_conf();

public:
	int send_error_to_user(usr_info_t* p, uint32_t err_no);
	int send_to_dbproxy_check_session(usr_info_t* p, uint32_t gameid, uint8_t* session, uint32_t session_len, uint32_t del_flag);

private:
	//call back or return
	int update_backup_svrlist_callback(uint8_t* body, uint32_t bodylen, int ret);

	int login_mole2_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int get_recommeded_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret);

	int get_ranged_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret);

	int mole2_register_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int check_session_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	uint32_t get_online_usercnt_level(uint32_t user_num)
	{
		if (user_num >= 500) {
			return 6;
		} else if (user_num >= 300) {
			return 5;
		} else if (user_num >= 180) {
			return 4;
		} else {
			return (user_num + 60) / 60;
		}
	}

#ifdef mole2_key
	int check_mfan_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int activation_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
#endif

	void handle_switch_return(svr_proto_t* dbpkg, uint32_t pkglen);

	int do_create_role(usr_info_t* p);

private:
	//utility
	int init_timer();
		
	int send_to_timestamp(usr_info_t* p, const uint8_t *buf, uint32_t length);

	void init_timestamp_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd);

	/**
	  * @brierf handle the multicast message to multicast group members.
	  * @param  body_buf. Package body buffer.
	  * @param    body_len. Body buffer's len.
	  * @return     int. 0 on success. -1 on database error.
	  */
	int  multicast_player_login(uint8_t* body_buf, int body_len);

public:
	my_option_t m_my_opt;
	int		m_switch_fd;
	timer_head_t m_get_backup_tmr;
	
	uint8_t m_login_switch_sendbuf[mole2_buffer_size];
	
	svr_info_t m_backup_svrlist[mole2_max_backup_svr];

	int m_backup_svrlist_cnt;

	uint8_t m_dbproxy_buf[mole2_buffer_size];
	
};

/**
 * @brief mole2登录
 */
extern mole2_login login;

#endif // LIBTAOMEEPP_LOGIN_IMPL_HPP_
