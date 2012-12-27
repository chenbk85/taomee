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

//#include <libtaomee++/project/main_login/cli_proto.hpp>
//#include <libtaomee++/project/main_login/main_login.hpp>
#include <main_login/cli_proto.hpp>
#include <main_login/main_login.hpp>
#include "libtype.h"
#include "kf/vip_config_data_mgr.hpp"

class KfLogin : public Login<CliProto2> {

public:
	KfLogin(): m_deluser_fd(-1), m_timestamp_fd(-1), 
		m_switch_fd(-1), m_multicast_fd(-1) {};
	
	~KfLogin() {};

private:
	vip_config_data_mgr* mgr;
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
	int  get_other_svrs_pkg_len(int fd, const void* avail_data, int);

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
	
	int check_invite_code_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen);
	
	int get_recommeded_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
	
	int get_ranged_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);

	int get_role_list_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen);

	int create_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);

	int create_role_with_invite_code_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);

	int logic_delete_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
	
	int resume_gray_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
	
	int gray_delete_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);

	void translate_users_num_to_type(uint32_t online_cnt, svr_info_t* p_svrs);
	//int update_backup_svrlist(void* owner, void* data);
public:
	int send_request_to_deluser(usr_info_t* p, userid_t uid, uint16_t cmd, const void* pkgbuf, uint32_t body_len);

	int send_to_switch(usr_info_t* p, const uint8_t *buf, uint32_t length);

	void init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd);

	int get_backup_svr_list(usr_info_t* p);

	//int add_game_flag(usr_info_t* p);
	
	//int switch_timeout(void* owner, void* data);

	int send_recommeded_svr_list(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt, int max_online_id);

	int get_recommended_svr_list(usr_info_t* p, uint32_t lv);
	
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

	int multicast_init();

	int get_version_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen);
private:
	//call back or return
	int get_mee_fans_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
	int get_amb_info_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int check_invite_code_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
	int check_user_invited_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
	
	int update_backup_svrlist_callback(uint8_t* body, uint32_t bodylen, int ret);

	int login_gf_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int get_recommeded_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret);

	int get_ranged_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret);

	int get_friend_time_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret);

	int gf_create_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	//int add_game_flag_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int add_childid_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);
	
	int get_role_list_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int gray_delete_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int logic_delete_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int resume_gray_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int verify_pay_passwd_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int verify_pay_passwd(usr_info_t *p);
	
	int delete_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	void handle_deluser_return(svr_proto_t* pkg, uint32_t pkglen);
	
	void handle_switch_return(svr_proto_t* dbpkg, uint32_t pkglen);

private:
	//utility
	int init_timer();

	int check_dirty_word(usr_info_t* p, char* msg);
	
	int unpkg_auth(const uint8_t body[], int len, login_session_t* sess);

	int verify_session(const userid_t uid, const login_session_t* sess);

	int check_role_type(usr_info_t *p, uint32_t role_type);

	int check_session_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret);

	int do_create_role(usr_info_t *p, uint32_t use_amb);
	int do_add_childid(usr_info_t *p, uint32_t child_role_tm, userid_t parentid, uint32_t parent_role_tm);
	int do_get_amb_info(usr_info_t *p);
	int do_get_mee_fans(usr_info_t *p);
	int do_create_role_with_inv_code(usr_info_t *p);
	
	int do_gray_del_role(usr_info_t *p);
	int do_logic_del_role(usr_info_t *p);
	int do_resume_gray_role(usr_info_t *p);
	
	int do_get_role_list(usr_info_t *p);
	int do_check_user_invited(usr_info_t *p);
	int db_check_session(usr_info_t* p, uint32_t gameid, uint8_t* session, 
		uint32_t session_len, uint32_t del_flag);
	int do_get_recommend_svr_list(usr_info_t* p);

	/**
	  * @brierf handle the multicast message to multicast group members.
	  * @param  body_buf. Package body buffer.
	  * @param    body_len. Body buffer's len.
	  * @return     int. 0 on success. -1 on database error.
	  */
	int  multicast_player_login(uint8_t* body_buf, int body_len);

	int init_dbsvr_onoff_socket();

	int db_add_record(uint32_t userid, uint32_t value);

	int send_to_gf_db(usr_info_t* p, uint32_t userid, uint32_t role_tm, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);
public:
	int send_error_to_user(usr_info_t* p, uint32_t err_no);
	int send_head_to_user(usr_info_t* p, uint32_t cmd, uint32_t err_no);
	int send_to_gf_db(usr_info_t* p, uint32_t role_tm, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

public:

	my_option_t m_my_opt;

	int m_deluser_fd;
	int m_timestamp_fd;
	int m_switch_fd;
	int m_multicast_fd;

	struct sockaddr_in m_multicast_addr;

	timer_head_t m_get_backup_tmr;
	uint8_t m_login_switch_sendbuf[gf_buffer_size];
	uint8_t m_gf_db_sendbuf[gf_buffer_size];

	int m_backup_svrlist_cnt;
	svr_info_t m_backup_svrlist[gf_max_backup_svr];
};

/**
 * @brief 主登录实例
 */
extern KfLogin login;

#endif // LIBTAOMEEPP_LOGIN_IMPL_HPP_
