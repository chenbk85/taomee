/**
 *============================================================
 *  @file      main_login.hpp
 *  @brief     主登录框架，所有登录服务器都应该继承于它，并且通过实现相关接口来实现副登录功能
 *             libgd2-xpm-dev needed: -lgd
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef LIBTAOMEEPP_MAIN_LOGIN_HPP_
#define LIBTAOMEEPP_MAIN_LOGIN_HPP_

extern "C" {
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/inet/ipaddr_cat.h>

#include <libtaomee/project/utilities.h>

#include <async_serv/net_if.h>
}

#include <libtaomee++/inet/byteswap.hpp>

//#include <libtaomee++/project/main_login/common.hpp>
//#include <libtaomee++/project/main_login/id_counter.hpp>
//#include <libtaomee++/project/main_login/user_manager.hpp>
#include <main_login/common.hpp>
//#include <main_login/id_counter.hpp>
#include <main_login/user_manager.hpp>
#include <libtaomee++/utils/strings.hpp>
//#include <libtaomee++/utils/md5.h>
#include <openssl/md5.h>

/**
 * @brief db相关常量
 */
enum db_constant_t {
	/*! dbproxy 来包大小限制 */
	dbpkg_max_size		= 8192
};

enum verify_type {
	verify_ip,
	verify_id,
};

#pragma pack(1)

/**
 * @brief dbproxy包头定义，大多数服务器端的包头定义与此相同
 */
struct dbproto_t {
    /*! package length */
    uint32_t    len;
    /*! sequence number ((p->fd << 16) | p->counter) */
    uint32_t    seq;
    /*! command id */
    uint16_t    cmd;
    /*! errno */
    uint32_t    ret;
    /*! user id */
    userid_t    id;
    /*! package body */
    uint8_t     body[];
};

#pragma pack()

/**
 * @brief 主登录框架
 */
template <typename CliProtoType>
class Login {
public:
	/**
	  * @brief Base class for implementing LoginServer
	  */
	Login();

	virtual ~Login()
		{ }

	bool init();
	void fini();

	int  get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent);

	int  dispatch(void* data, fdsession_t* fdsess);

	void proc_pkg_from_serv(int fd, void* data, int len);

	void on_fd_closed(int fd);

protected:
	/**
	  * @brief 解析出dbproxy来包的长度，如果其它服务器的来包包头和dbproxy一致，也可以调用这个函数来获取其来包长度
	  * @return 解析出来的数据包长度，或者如果失败的话，返回-1 
	  */
	int  get_dbproxy_pkg_len(const void* avail_data);

	/**
	  * @brief 发包给客户端
	  * @param p 客户端用户
	  * @return 0成功，-1失败
	  */
	int  send_to_user(usr_info_t* p);

	/**
	  * @brief 发包给dbproxy
	  * @param p 客户端用户
	  * @param uid user id
	  * @param cmd 和dbproxy的协议命令号
	  * @param dbpkgbuf 数据包内容
	  * @param body_len 数据包长度
	  * @return 0成功，-1失败
	  */
	int  send_to_dbproxy(usr_info_t* p, userid_t uid, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len);

public:
	/**
	  * @brief 生成MD5码验证
	  * @param req_buf 生成MD5码的包体
	  * @param req_buf_len
	  * @return 返回MD5码，32位字符串
	  */
	const char* req_verify_md5(const char* req_buf, int req_buf_len);

	/**
	  * @brief 新创建角色添加游戏标志位
	  * @param p 客户端用户
	  * @param game_flag 游戏标志位
	  * @return 0成功，-1失败
	  */
	int db_add_game_flag(usr_info_t* p, uint32_t game_flag, uint16_t channel_id);

private:
	//----------------------------------------------------------------
	// command functions
	//----------------------------------------------------------------
	int  login_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen);
	int  renew_verifimg_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen);
	int  check_active_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen);
	int  set_active_code_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen);
	//void send_verif_img(usr_info_t* p, uint32_t flag, uint32_t cnt);
	//int  check_malicious_user(usr_info_t* p, uint8_t type);

	//----------------------------------------------------------------
	// db send functions
	//----------------------------------------------------------------
	int  db_login(usr_info_t* p);
	int  db_login_with_verif_img(usr_info_t* p);
	int  db_get_verif_img(usr_info_t* p);
	int  db_get_verif_img_anytime(usr_info_t* p);
	int  db_set_active_code(usr_info_t* p, uint8_t game_id, char* active_code, int active_code_len);
	int  db_post_mail(usr_info_t* p);
	int  db_check_meefan(usr_info_t* p);

	//----------------------------------------------------------------
	// db callback functions
	//----------------------------------------------------------------
	void handle_dbproxy_return(dbproto_t* dbpkg, uint32_t pkglen);
	int  db_login_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret);
	int  db_login_with_verif_img_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret);
	int  db_get_verif_img_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret);
	int  db_check_active_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret);
	int  db_set_active_code_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret);
	int  db_add_game_flag_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret);
	int  db_check_meefan_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret);

	/**
	 * @brief 处理一些db返回来的错误码
	 */
	int  check_db_err(usr_info_t* p, uint32_t ret);
	
	/**
	 * @brief 将数据库返回的flag转成客户端设定的对应值
	 * @return 正常的话，返回flag客户端对应值
	 */
	int  flag_convert(uint32_t flag, uint32_t db_cmd);

	static int dbproxy_timeout(void* owner, void* data);

private:
	/**
	 * @brief 副登录如果需要初始化额外的内容，比如说读取配置文件等等，可以通过实现这个接口来完成
	 * @return 正常的话，返回true，如果出现任何初始化失败的状况，请返回false。
	 */
	virtual bool init_service()
		{ return true; }
	/**
	 * @brief 副登录如果需要清理额外的内容，比如说释放内存等等，可以通过实现这个接口来完成
	 */
	virtual void fini_service()
		{ }

	/**
	 * @brief 副登录如果需要解析其它服务器来包（比如说Switch）的长度，可以通过实现这个接口来完成。
	 *        一般来说，因为服务器来包包头都一样，可以直接在这里调用get_dbproxy_pkg_len。
	 * @return 解析出来的数据包长度，或者如果失败的话，返回-1 
	 */
	virtual int  get_other_svrs_pkg_len(int fd, const void* avail_data, int avail_len)
		{  return -1; }

	/**
	 * @brief 副登录如果需要处理其它的客户端协议命令，比如说拉去服务器列表/创建角色，可以通过实现这个接口来完成
	 * @return 一切正常的话，返回0，如果客户端发包非法，或者处理过程中出现系统级的错误，请返回-1
	 */
	virtual int  proc_other_cmds(usr_info_t* usr, uint8_t* body, uint32_t bodylen)
		{ return -1; }

	/**
	 * @brief 副登录如果需要处理其它的dbproxy回报，比如说创建角色的返回包，可以通过实现这个接口来完成
	 * @return 一切正常的话，返回0，如果DBproxy返回包非法，或者处理过程中出现系统级的错误，请返回-1
	 */
	virtual int  proc_other_dbproxy_return(usr_info_t* p, uint16_t cmdid, const uint8_t* body, uint32_t bodylen, uint32_t ret)
		{ return -1; }

	/**
	 * @brief 副登录如果需要处理其它服务器的返回，比如说Switch的返回，可以通过实现这个接口来完成
	 */
	virtual void proc_other_svrs_return(int fd, void* dbpkg, uint32_t pkglen)
		{ }

	/**
	 * @brief 副登录如果需要处理其它服务器断开连接的情况，比如说Switch断开连接，可以通过实现这个接口来完成
	 */
	virtual void on_other_svrs_fd_closed(int fd)
		{ }

protected:
	/*!
	 * 通过这个成员来打包发给客户端，打包前调用m_clipkg->init(cmdid, uid, errnum)，打包完成后调用send_to_user完成发包
	 */
	CliProtoType*	m_clipkg;
	/*!
	 * 公共的服务器超时，一般定义为10秒，如果你需要特殊的超时秒数，请另外定义成员变量，并从配置文件中读取
	 */
	int				m_svr_timeout;
	/*! max channel number */
	uint32_t		m_total_channels;
	/*! idc zone: 0 for telecom, 1 for cnc */
	uint32_t		m_idc_zone;
	/*! channel id for verify*/
	uint16_t		m_channel_code;

private:
	uint32_t	m_wrong_limit;
	int			m_wrong_time_range;
	int			m_ban_time;
	uint32_t	m_security_code;

	int			m_dbproxyfd;

	char		m_msg[1024 * 1024 * 2];
};

template <typename CliProtoType>
inline
Login<CliProtoType>::Login()
{
	m_dbproxyfd = -1;
	m_clipkg    = reinterpret_cast<CliProtoType*>(m_msg);
}

template <typename CliProtoType>
inline bool
Login<CliProtoType>::init()
{
    m_wrong_limit      = config_get_intval("count_limited", 0);
    m_wrong_time_range = config_get_intval("passwd_fail_time_limited", 0);
    m_ban_time         = config_get_intval("ban_time", 0);
    m_security_code    = config_get_intval("security_code", 0);
    m_channel_code       = config_get_intval("channel_id", 0);

	m_idc_zone         = config_get_intval("idc_zone", 0);

	m_total_channels   = config_get_intval("channel_total", 0);
	m_svr_timeout      = config_get_intval("svr_timeout", 12);

	stat_file          = config_get_strval("statistic_file");

	init_warning_system();
	//id_counter_init();

	return init_service();
}

template <typename CliProtoType>
inline void
Login<CliProtoType>::fini()
{
	fini_service();
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
	static char request[]  = "<policy-file-request/>";
	static char response[] = "<?xml version=\"1.0\"?>"
								"<!DOCTYPE cross-domain-policy>"
								"<cross-domain-policy>"
								"<allow-access-from domain=\"*\" to-ports=\"*\" />"
								"</cross-domain-policy>";

	/* must have 4 bytes*/
	if (avail_len < 4) {
		return 0;
	}

	/*if is policy request, send response to flash client*/
	if(isparent) {
		if ((avail_len == sizeof(request)) && !memcmp(avail_data, request, sizeof(request))) {
			net_send(fd, response, sizeof(response));
			return 0;
		}

		/*get real package length*/
		const CliProtoType* clipkg = reinterpret_cast<const CliProtoType*>(avail_data);
		#if 0
		uint32_t reallen = taomee::bswap(clipkg->m_len);
		#endif
		uint32_t reallen = m_clipkg->unpack(clipkg->m_len);
		if (reallen < sizeof(CliProtoType) || reallen > clipkg_max_size) {
			KERROR_LOG(0, "invalid length %u from fd %d", reallen, fd);
			return -1;
		}

		return reallen;
	} else {
		if (fd == m_dbproxyfd) {
			return get_dbproxy_pkg_len(avail_data);
		} else {
			return get_other_svrs_pkg_len(fd, avail_data, avail_len);
		}
	}

	return 0;
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::dispatch(void* data, fdsession_t* fdsess)
{
	CliProtoType* hdr = reinterpret_cast<CliProtoType*>(data);
#if 0
	uint32_t cmdid  = taomee::bswap(hdr->m_cmd);
	userid_t uid    = taomee::bswap(hdr->m_id);
	uint32_t pkglen = taomee::bswap(hdr->m_len);
#endif

	uint32_t cmdid = hdr->unpack(hdr->m_cmd);
	uint32_t uid = hdr->unpack(hdr->m_id);
	uint32_t pkglen = hdr->unpack(hdr->m_len);
	

	usr_info_t* p = user_mng.get_user(fdsess->fd);
	if (p == 0) {
		if (uid == 0) { // in case of login using email address
			uid = login_by_email;
		}

		p = user_mng.alloc_user(fdsess->fd, uid, fdsess);
		if (p == 0) {
			KERROR_LOG(uid, "failed to alloc user: fd=%d uid=%u", fdsess->fd, uid);
			return -1;
        }
	}

	if (p->uid != uid) {
		WARN_LOG("should be a bug of the client, user id mismatch: fd=%d uid=%u %u cmd=%u",
					fdsess->fd, uid, p->uid, cmdid);

		if (uid == 0) {
			p->uid = login_by_email;
		} else {
			p->uid = uid;
		}
	}

	if (p->waitcmd) {
		WARN_LOG("system busy: uid=%u fd=%d cmd=%u %u", p->uid, fdsess->fd, cmdid, p->waitcmd);
		return 0;
	}

	p->waitcmd = cmdid;

    uint32_t bodylen = pkglen - sizeof(CliProtoType);
    
	switch (cmdid) {
	case cliproto_renew_verifimg:
		return renew_verifimg_cmd(p, hdr->m_body, bodylen);
	case cliproto_user_login:
		return login_cmd(p, hdr->m_body, bodylen);
	case cliproto_check_active:
		return check_active_cmd(p, hdr->m_body, bodylen);
	case cliproto_set_active_code:
		return set_active_code_cmd(p, hdr->m_body, bodylen);
	case cliproto_probe:
		m_clipkg->init(cmdid, p->uid, 0);
		return send_to_user(p);
	default:
		return proc_other_cmds(p, hdr->m_body, bodylen);
	}

	return 0;
}

template <typename CliProtoType>
inline void
Login<CliProtoType>::proc_pkg_from_serv(int fd, void* data, int len)
{
	if (fd == m_dbproxyfd) {
		handle_dbproxy_return(reinterpret_cast<dbproto_t*>(data), len);
	} else {
		proc_other_svrs_return(fd, data, len);
	}
}

template <typename CliProtoType>
inline void
Login<CliProtoType>::on_fd_closed(int fd)
{
	if (fd == m_dbproxyfd) {
		KDEBUG_LOG(0, "DB PROXY CONNECTION CLOSED\t[fd=%d]", m_dbproxyfd);
		m_dbproxyfd = -1;

		//send_warning(0, warning_dbproxy, get_remote_ip(fd));
		uint32_t nip = get_remote_ip(fd);
		char     ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &nip, ip, INET_ADDRSTRLEN);
		send_warning("dbproxy", 0, 0, 0, ip);
	} else {
		on_other_svrs_fd_closed(fd);
	}
}

//----------------------------------------------------------------
// Protected Methods
//
template <typename CliProtoType>
inline int
Login<CliProtoType>::get_dbproxy_pkg_len(const void* avail_data)
{
	uint32_t reallen = *reinterpret_cast<const uint32_t*>(avail_data);
	if ((reallen <= dbpkg_max_size) && (reallen >= sizeof(dbproto_t))) {
		return reallen;
	}

	KERROR_LOG(0, "invalid db pkg len: %u", reallen);
	return -1;
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::send_to_user(usr_info_t* p)
{
	p->waitcmd = 0;
	p->inc_counter();
	return m_clipkg->send_to(p->session);
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::send_to_dbproxy(usr_info_t* p, userid_t uid, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
	static uint8_t dbbuf[dbpkg_max_size];

	if (m_dbproxyfd == -1) {
		m_dbproxyfd = connect_to_service(config_get_strval("dbproxy_name"), config_get_intval("dbproxy_id", 0), 65535, 1);
	}

    if ((m_dbproxyfd == -1) || (body_len > (sizeof(dbbuf) - sizeof(dbproto_t)))) {
		if (p) {
			m_clipkg->init(p->waitcmd, p->uid, clierr_system_error);
			send_to_user(p);

			const char* ip = resolve_service_name(config_get_strval("dbproxy_name"), config_get_intval("dbproxy_id", 0));
			//send_warning(p, warning_dbproxy, ip ? inet_addr(ip) : 0);
			send_warning("dbproxy", p->uid, p->waitcmd, 0, ip);
		}

		KERROR_LOG(0, "send to dbproxy failed: fd=%d len=%d", m_dbproxyfd, body_len);
		return -1;
    }

	dbproto_t* pkg = reinterpret_cast<dbproto_t*>(dbbuf);
	pkg->len = sizeof(dbproto_t) + body_len;
	pkg->seq = p ? ((p->session->fd << 16) | p->counter) : 0;
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = uid;
	memcpy(pkg->body, dbpkgbuf, body_len);

	if (p) {
		ADD_TIMER_EVENT(p, dbproxy_timeout, this, get_now_tv()->tv_sec + m_svr_timeout);
	}

	return net_send(m_dbproxyfd, dbbuf, pkg->len);
}

//----------------------------------------------------------------
// Public Methods
//
template <typename CliProtoType>
inline const char*
Login<CliProtoType>::req_verify_md5(const char* req_body, int req_body_len)
{
	char md_16[16];
	char md_32[32 + 1];
	static char md5_buf[32 + 1] = {0};
	const uint32_t verify_buf_len = 1024;
	char verify_buf[verify_buf_len] = {0};

	uint32_t s_len = snprintf(verify_buf, verify_buf_len, 
			"channelId=%d&securityCode=%u&data=", m_channel_code, m_security_code);
	
	if (req_body_len + s_len >= verify_buf_len) {
		KERROR_LOG(0, "req_body_len too long[%u %u]", req_body_len + strlen(verify_buf), verify_buf_len);
		return 0;
	}
	
	memcpy(verify_buf + s_len, req_body, req_body_len);

	//get md5 by buf
	MD5(reinterpret_cast<unsigned char*>(verify_buf), s_len + req_body_len, reinterpret_cast<unsigned char*>(md_16));
	bin2hex_frm(md_32, md_16, 16, 0);
	sprintf(md5_buf, md_32, 32);

	return md5_buf;
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::flag_convert(uint32_t flag, uint32_t db_cmd)
{
	switch (db_cmd) {
		case dbproto_login_with_verif_img:
			if (flag == 1) {
				return 1;
			} else if (flag == 4 || flag == 5) {
				return 2;
			} else if (flag == 2) {
				return 6;
			} else if (flag == 3) {
				return 7;
			} else {
				return 0;
			}
			break;
		case dbproto_set_active_code:
			if (flag == 1) {
				return 1;
			} else if (flag == 4 || flag == 5) {
				return 2;
			} else {
				return 0;
			}
			break;
		case dbproto_get_verif_img:
			return (flag == 5 ? 1 : 0);
			break;
	}
	return 0;
}

//----------------------------------------------------------------
// Private Methods
//
template <typename CliProtoType>
inline int
Login<CliProtoType>::renew_verifimg_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen)
{
	/*const id_info_t* ip = ip_counter_info(p->session->remote_ip);
	const id_info_t* id = id_counter_info(p->uid);
	uint32_t cnt = 0;
	if (ip) {
		cnt = ip->id_cnt;
	}
	if (id && id->id_cnt > cnt) {
		cnt = id->id_cnt;
	}
	if (cnt <= m_wrong_limit) {
		m_clipkg->init(p->waitcmd, p->uid, 0);
		m_clipkg->pack(0);
		send_to_user(p);
		return 0;
	}

	const ImagePackage* img;
	// check malicious user
	if (cnt < 35) {
		img = g_vig_worker.get_image();
	} else {
		img = g_vig_hard_worker.get_image();
	}

	if (img) {
		m_clipkg->init(p->waitcmd, p->uid, 0);
		m_clipkg->pack(1);
		m_clipkg->pack(img->verif_id, sizeof(img->verif_id));
		m_clipkg->pack(img->img_size);
		m_clipkg->pack(img->img, img->img_size);
	} else {
		m_clipkg->init(p->waitcmd, p->uid, clierr_system_error);
		WARN_LOG("failed to get verification image: uid=%u ip=%X", p->uid, p->session->remote_ip);
	}
	send_to_user(p);

	ip_counter_add(p->session->remote_ip);
	if (is_valid_uid(p->uid)) {
		id_counter_add(p->uid);
	}

	return 0;*/

	if (bodylen == 4) {
		uint32_t flag = *reinterpret_cast<uint32_t*>(body);
		if (flag == 1) {
			return db_get_verif_img_anytime(p);
		}
	}

	return db_get_verif_img(p);
}

/*template <typename CliProtoType>
inline int
Login<CliProtoType>::check_malicious_user(usr_info_t* p, uint8_t type)
{
	login_tmpinfo_t* login_tmp = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);

	const id_info_t* p_count = (type == verify_ip) ? ip_counter_info(p->session->remote_ip) : id_counter_info(p->uid);
	if (!login_tmp->verif_pass && p_count) {
		int gap = static_cast<int>(difftime(get_now_tv()->tv_sec, p_count->first_time));

		if ((gap >= m_ban_time) || (gap >= m_wrong_time_range && p_count->id_cnt <= m_wrong_limit)) {
			//clear the count info.
			if (type == verify_ip) {
				ip_counter_erase(p->session->remote_ip);
			} else {
				id_counter_erase(p->uid);
			}
		} else if (p_count->id_cnt <= m_wrong_limit) {
			// do nothing
		} else {
			// log some statistic info
			if (type == verify_id) {
				uint32_t buf[2] = { 1, p->uid };
				statistic_msglog(stat_base_id + stat_banned_user_offset, buf, sizeof(buf));
			}

			// ask user to input verification code
			if (login_tmp->verif_code[0] == '\0') {
				DEBUG_LOG("(IP) PASSWORD ERROR TOO MUCH, VERIFICATION NEEDED\t[ip=%X uid=%u]",
							p->session->remote_ip, p->uid);
				send_verif_img(p, 1, p_count->id_cnt);
				return -1;
			} else if (VigWorker::verify_code(login_tmp->img_id, login_tmp->verif_code) == false) {
				DEBUG_LOG("(IP) WRONG VERIFICATION CODE\t[ip=%X uid=%u code=%s]",
							p->session->remote_ip, p->uid, login_tmp->verif_code);
				send_verif_img(p, 2, p_count->id_cnt);
				return -1;
			}
			
			login_tmp->verif_pass = 1;
		}
	}

	return 0;
}*/

template <typename CliProtoType>
inline int
Login<CliProtoType>::login_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	login_tmpinfo_t* login_tmp = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);

	//check bodylen
	if (p->uid != login_by_email) { // login using user id
		if (bodylen != sizeof(cli_login_req_t)) {
			KERROR_LOG(p->uid, "userid login, invalid len: uid=%u fd=%d cmd=%u len=%u explen=%lu",
						p->uid, p->session->fd, p->waitcmd, bodylen, sizeof(cli_login_req_t));
			return -1;
		}
		memset(login_tmp->email, 0x00, email_max_len);
		KDEBUG_LOG(p->uid, "UID LOGIN\t[uid=%u fd=%d cmd=%u]", p->uid, p->session->fd, p->waitcmd);
	} else { // uid == 0, login using email address
		if (bodylen != (sizeof(cli_login_req_t) + email_max_len)) {
			KERROR_LOG(p->uid, "email login, invalid len: uid=%u fd=%d cmd=%u len=%u explen=%lu",
						p->uid, p->session->fd, p->waitcmd, bodylen, sizeof(cli_login_req_t) + email_max_len);
			return -1;
		}

		taomee::unpack(body, login_tmp->email, email_max_len, idx);
		KDEBUG_LOG(0, "EMAIL LOGIN\t[email=%.64s fd=%d cmd=%u]", login_tmp->email, p->session->fd, p->waitcmd);
	}

	cli_login_req_t* tmp = reinterpret_cast<cli_login_req_t*>(body + idx);
	login_tmp->cli_channel_id = m_clipkg->unpack(tmp->channel);
	login_tmp->channel = m_channel_code;
	login_tmp->which_game = m_clipkg->unpack(tmp->which_game);
	login_tmp->user_ip    = tmp->user_ip;
	str2hex(tmp->passwd, sizeof(login_tmp->passwd), login_tmp->passwd);
	memcpy(login_tmp->img_id, tmp->img_id, verif_img_id_len);
	memcpy(login_tmp->verif_code, tmp->verif_code, verif_code_len);
	login_tmp->verif_code[verif_code_len - 1] = '\0';
	login_tmp->verif_pass = 0;

	//check game flag
	if ((login_tmp->which_game < 1) || (login_tmp->which_game > gameprj_max_num)) {
		KERROR_LOG(p->uid, "invalid game project id: uid=%u gid=%u cmd=%u fd=%d",
					p->uid, login_tmp->which_game, p->waitcmd, p->session->fd);
		return -1;
	}

	// check malicious user
	/*if (check_malicious_user(p, verify_ip) == -1) {
		return 0;
	}
	if (p->uid != login_by_email && check_malicious_user(p, verify_id) == -1) {
		return 0;
	}*/

	if (!is_private_ip2(p->session->remote_ip)) { // public ip address
		if (login_tmp->user_ip) {
			WARN_LOG("user ip should be zero: ip=%X %X", login_tmp->user_ip, p->session->remote_ip);
		}
		login_tmp->user_ip = p->session->remote_ip;
	}
	login_tmp->user_ip = p->session->remote_ip;

	//return db_login(p);
	return db_login_with_verif_img(p);
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::check_active_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen)
{
	//uint32_t which_game = taomee::bswap(*static_cast<uint32_t*>(body));
	uint32_t which_game = m_clipkg->unpack(*reinterpret_cast<uint32_t*>(body));
	int idx = 0;
	char dbbuf[1];
	taomee::pack_h(dbbuf, static_cast<uint8_t>(which_game), idx);
	return send_to_dbproxy(p, p->uid, dbproto_check_active, dbbuf, idx);
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::set_active_code_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen)
{
	login_tmpinfo_t* login_tmp = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);
	memset(login_tmp->img_id, 0x00, sizeof(login_tmp->img_id));
	memset(login_tmp->verif_code, 0x00, sizeof(login_tmp->verif_code));

	int idx = 4;
	uint32_t gameid = 0;
	const int active_code_len = 10;
	char active_code[active_code_len];
	gameid = m_clipkg->unpack(*reinterpret_cast<uint32_t *>(body));
	taomee::unpack(body, active_code, active_code_len, idx);
	taomee::unpack(body, login_tmp->img_id, verif_img_id_len, idx);
	taomee::unpack(body, login_tmp->verif_code, verif_code_len, idx);

	return db_set_active_code(p, gameid, active_code, active_code_len);
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_set_active_code(usr_info_t* p, uint8_t game_id, char* active_code, int active_code_len)
{
	login_tmpinfo_t* login_tmp = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);
	
	const int verify_code_len = 32;
	int idx = 0;
	char tmp_buf[sizeof(game_id) + active_code_len + 4 + verif_img_id_len + verif_code_len];
	//char buf[sizeof(m_channel_code) + verify_code_len + sizeof(tmp_buf)];
	char buf[256];

	//get verify md5
	idx = 0;
	taomee::pack_h(tmp_buf, game_id, idx);
	taomee::pack(tmp_buf, active_code, active_code_len, idx);
	taomee::pack_h(tmp_buf, p->session->remote_ip, idx);
	taomee::pack(tmp_buf, login_tmp->img_id, verif_img_id_len, idx);
	taomee::pack(tmp_buf, login_tmp->verif_code, verif_code_len, idx);
	const char* verify_code = req_verify_md5(tmp_buf, idx);
	
	//send buf
	idx = 0;
	taomee::pack_h(buf, m_channel_code, idx);
	taomee::pack(buf, verify_code, verify_code_len, idx);
	taomee::pack(buf, tmp_buf, sizeof(tmp_buf), idx);
	TRACE_LOG("ll %u %u %10s", p->uid, game_id, active_code);
	return send_to_dbproxy(p, p->uid, dbproto_set_active_code, buf, idx);
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_login(usr_info_t* p)
{
	login_tmpinfo_t* loginreq = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);

	static char dbbuf[email_max_len + password_bin_max_len + sizeof(loginreq->channel) + 
		sizeof(m_idc_zone) + sizeof(loginreq->which_game) + sizeof(loginreq->user_ip)];

	int idx = 0;
	taomee::pack(dbbuf, loginreq->email, email_max_len, idx);
	taomee::pack(dbbuf, loginreq->passwd, password_bin_max_len, idx);
	taomee::pack_h(dbbuf, static_cast<uint16_t>(loginreq->channel), idx);
	taomee::pack_h(dbbuf, static_cast<uint16_t>(m_idc_zone), idx);
	taomee::pack_h(dbbuf, static_cast<uint16_t>(loginreq->which_game), idx);
	taomee::pack_h(dbbuf, loginreq->user_ip, idx);

	uint32_t uid = (p->uid == login_by_email) ? 0 : p->uid;
	return send_to_dbproxy(p, uid, dbproto_login, dbbuf, idx);

}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_login_with_verif_img(usr_info_t* p)
{
	login_tmpinfo_t* loginreq = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);

	//take care of the size of 'dbbuf', or will cause memory overrun
	static char dbbuf[email_max_len + password_bin_max_len + sizeof(loginreq->channel) + 
		sizeof(m_idc_zone) + sizeof(loginreq->which_game) + sizeof(loginreq->user_ip) + 
		sizeof(loginreq->img_id) + sizeof(loginreq->verif_code)];

	int idx = 0;
	taomee::pack(dbbuf, loginreq->email, email_max_len, idx);
	taomee::pack(dbbuf, loginreq->passwd, password_bin_max_len, idx);
	taomee::pack_h(dbbuf, static_cast<uint16_t>(loginreq->channel), idx);
	taomee::pack_h(dbbuf, static_cast<uint16_t>(m_idc_zone), idx);
	taomee::pack_h(dbbuf, static_cast<uint16_t>(loginreq->which_game), idx);
	taomee::pack_h(dbbuf, loginreq->user_ip, idx);
	taomee::pack(dbbuf, loginreq->img_id, verif_img_id_len, idx);
	taomee::pack(dbbuf, loginreq->verif_code, verif_code_len, idx);
	taomee::pack_h(dbbuf, static_cast<uint16_t>(loginreq->cli_channel_id), idx);

	uint32_t uid = (p->uid == login_by_email) ? 0 : p->uid;
	return send_to_dbproxy(p, uid, dbproto_login_with_verif_img, dbbuf, idx);

}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_get_verif_img(usr_info_t* p)
{
	static char verifbuf[22];
	int idx = 0;
	taomee::pack_h(verifbuf, static_cast<uint16_t>(m_channel_code), idx);
	taomee::pack_h(verifbuf, static_cast<uint32_t>(p->session->remote_ip), idx);
	//taomee::pack(verifbuf, imgid, verif_img_id_len, idx);
	return send_to_dbproxy(p, p->uid, dbproto_get_verif_img, verifbuf, idx);
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_get_verif_img_anytime(usr_info_t* p)
{
	static char verifbuf[22];
	char imgid[16] = {0};
	int idx = 0;
	taomee::pack_h(verifbuf, static_cast<uint16_t>(m_channel_code), idx);
	taomee::pack_h(verifbuf, static_cast<uint32_t>(p->session->remote_ip), idx);
	taomee::pack(verifbuf, imgid, verif_img_id_len, idx);
	return send_to_dbproxy(p, p->uid, dbproto_get_verif_img_anytime, verifbuf, idx);
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_add_game_flag(usr_info_t* p, uint32_t game_flag, uint16_t channel_id)
{
	int idx = 0;
	char tmp_buf[sizeof(m_idc_zone) + sizeof(game_flag)];
	static char buf[sizeof(m_channel_code) + 32 + sizeof(game_flag) + sizeof(m_idc_zone )];
	login_tmpinfo_t* login_tmp = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);
	login_tmp->which_game = game_flag;
	login_tmp->cli_channel_id = channel_id;

	//get verify md5
	idx = 0;
	taomee::pack_h(tmp_buf, m_idc_zone, idx);
	taomee::pack_h(tmp_buf, game_flag, idx);
	taomee::pack_h(tmp_buf, channel_id, idx);
	const char* verify_code = req_verify_md5(tmp_buf, idx);
	
	//send buf
	idx = 0;
	taomee::pack_h(buf, m_channel_code, idx);
	taomee::pack(buf, verify_code, 32, idx);
	taomee::pack_h(buf, m_idc_zone, idx);
	taomee::pack_h(buf, game_flag, idx);
	taomee::pack_h(buf, channel_id, idx);
	TRACE_LOG("ll %u %u %u", p->uid, m_idc_zone, game_flag);
	return send_to_dbproxy(p, p->uid, dbproto_add_game_flag, buf, idx);
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_post_mail(usr_info_t* p)
{
	struct db_post_mail_t {
		uint32_t game_flag;
		uint32_t game_ip;
		uint16_t game_zone;
	}__attribute__((packed));

	int idx = 0;
	db_post_mail_t mail_info;
	char buf[sizeof(db_post_mail_t)];
	login_tmpinfo_t* login_tmp = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);

	mail_info.game_flag = login_tmp->which_game;
	mail_info.game_ip   = login_tmp->user_ip;
	mail_info.game_zone = m_idc_zone;

	taomee::pack(buf, &mail_info, sizeof(mail_info), idx);
	return send_to_dbproxy(0, p->uid, dbproto_post_mail, buf, idx);
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_check_meefan(usr_info_t* p)
{
	int idx = 0;
	char buf[1];
	return send_to_dbproxy(p, p->uid, dbproto_check_meefan, buf, idx);
}
/*template <typename CliProtoType>
inline void
Login<CliProtoType>::send_verif_img(usr_info_t* p, uint32_t flag, uint32_t cnt)
{
	const ImagePackage* img;
	if (cnt < 35) {
		img = g_vig_worker.get_image();
	} else {
		img = g_vig_hard_worker.get_image();
	}

	if (img) {
		m_clipkg->init(p->waitcmd, p->uid, 0);
		m_clipkg->pack(flag);			
		m_clipkg->pack(img->verif_id, sizeof(img->verif_id));
		m_clipkg->pack(img->img_size);
		m_clipkg->pack(img->img, img->img_size);
	} else {
		m_clipkg->init(p->waitcmd, p->uid, clierr_system_error);
		WARN_LOG("failed to get verification image: uid=%u ip=%X", p->uid, p->session->remote_ip);
	}
	send_to_user(p);
}*/

template <typename CliProtoType>
inline void
Login<CliProtoType>::handle_dbproxy_return(dbproto_t* dbpkg, uint32_t pkglen)
{
	if (!dbpkg->seq) {
		return;
	}

	uint32_t counter = dbpkg->seq & 0xFFFF;
	uint32_t connfd  = dbpkg->seq >> 16;

	usr_info_t* p = user_mng.get_user(connfd);
	if(!p || (counter != p->counter)) {
		KDEBUG_LOG(0, "CONNECTION HAD BEEN CLOSED OR DB TIMEOUT\t[uid=%u cmd=%X %u cnt=%d %d]",
					p ? p->uid : 0, dbpkg->cmd, p ? p->waitcmd : 0, counter, p ? p->counter : 0);
		return;
	}

	switch (dbpkg->ret) {
	case dberr_sys_error:
	case dberr_db_error:
	case dberr_net_error:
	case dberr_timeout:
		send_warning("db", p->uid, dbpkg->cmd, 1);
		break;
	default:
		break;
	}
	
	KDEBUG_LOG(p->uid, "DB R\t[uid=%u %u fd=%d cmd=0x%X ret=%u]",
				p->uid, dbpkg->id, connfd, dbpkg->cmd, dbpkg->ret);

	REMOVE_TIMERS(p);

	int err = -1;
	uint32_t bodylen = dbpkg->len - sizeof(dbproto_t); 

	switch(dbpkg->cmd) {
	case dbproto_login:
		p->uid = dbpkg->id;
		err = db_login_callback(p, dbpkg->body, bodylen, dbpkg->ret);
		break;
	case dbproto_add_game_flag:
		err = db_add_game_flag_callback(p, dbpkg->body, bodylen, dbpkg->ret);
		break;
	case dbproto_login_with_verif_img:
		p->uid = dbpkg->id;
		err = db_login_with_verif_img_callback(p, dbpkg->body, bodylen, dbpkg->ret);
		break;
	case dbproto_get_verif_img:
	case dbproto_get_verif_img_anytime:
		err = db_get_verif_img_callback(p, dbpkg->body, bodylen, dbpkg->ret);
		break;
	case dbproto_check_active:
		//err = 0;
		err = db_check_active_callback(p, dbpkg->body, bodylen, dbpkg->ret);
		break;
	case dbproto_set_active_code:
		//err = 0;
		err = db_set_active_code_callback(p, dbpkg->body, bodylen, dbpkg->ret);
		break;
	case dbproto_check_meefan:
		err = db_check_meefan_callback(p, dbpkg->body, bodylen, dbpkg->ret);
		break;
	default:
		err = proc_other_dbproxy_return(p, dbpkg->cmd, dbpkg->body, bodylen, dbpkg->ret);
		break;
	}

	if(err) {
		close_client_conn(connfd);
	}
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_login_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	//check ret
	/*if (ret != 0) {
		if (ret == dberr_wrong_passwd) {
			id_counter_add(p->uid);
			ip_counter_add(p->session->remote_ip);
	
			// log some statistic info
			uint32_t buf[2] = { 1, p->uid };
			statistic_msglog(stat_base_id + stat_wrong_password_offset, buf, sizeof(buf));
		}

		return check_db_err(p, ret);
	}

	if (bodylen != login_session_len + 4) {
		ERROR_LOG("invalid dbpkg len: uid=%u len=%u explen=%u", p->uid, bodylen, 20);
		return -1;
	}
	
	login_tmpinfo_t* loginreq = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);
	// check malicious user
	if (strlen(reinterpret_cast<char*>(loginreq->email)) != 0 
			&& check_malicious_user(p, verify_id) == -1) {
		return 0;
	}
	id_counter_erase(p->uid);

	uint32_t gameflag = *reinterpret_cast<const uint32_t*>(body + login_session_len);
	p->create_role_flag = (gameflag & (0x01 << (loginreq->which_game - 1))) ? 1 : 0; 

	DEBUG_LOG("LOGIN SUCC\t[uid=%u ip=%X gameflag=%X gameid=%u created=%u]", 
				p->uid, loginreq->user_ip, gameflag, loginreq->which_game, p->create_role_flag);

	//send to player
	m_clipkg->init(p->waitcmd, p->uid, 0);
	m_clipkg->pack(0);
	m_clipkg->pack(body, login_session_len);
	m_clipkg->pack(p->create_role_flag);
	send_to_user(p);*/

	return 0;
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_login_with_verif_img_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	if (ret != 0) {
		return check_db_err(p, ret);
	}

	uint32_t flag = *reinterpret_cast<const uint32_t*>(body);

	login_tmpinfo_t* loginreq = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);
	
	m_clipkg->init(p->waitcmd, p->uid, 0);
	uint32_t cli_flag = flag_convert(flag, dbproto_login_with_verif_img);
	m_clipkg->pack(cli_flag);
	if (flag == 0) {//login successfully
		if (bodylen != 24) {
			KERROR_LOG(p->uid, "invalid dbpkg len: uid=%u len=%u explen=%u", p->uid, bodylen, 24);
			return -1;
		}	
		uint32_t gameflag = *reinterpret_cast<const uint32_t*>(body + 4 + login_session_len);
		p->create_role_flag = (gameflag & (0x01 << (loginreq->which_game - 1))) ? 1 : 0; 
	
		KDEBUG_LOG(p->uid, "LOGIN SUCCESS\t[uid=%u ip=%X gameflag=%X gameid=%u created=%u]", 
				p->uid, loginreq->user_ip, gameflag, loginreq->which_game, p->create_role_flag);

		m_clipkg->pack(body + 4, login_session_len);
		m_clipkg->pack(p->create_role_flag);
	} else if (flag == 1 || flag == 4 || flag == 5) {//password error or verify code error
		uint32_t img_size = *reinterpret_cast<const uint32_t*>(body + 20);
		if (bodylen != 24 + img_size) {
			KERROR_LOG(p->uid, "invalid dbpkg len: uid=%u len=%u explen=%u", p->uid, bodylen, 24 + img_size);
			return -1;
		}
		m_clipkg->pack(body + 4, 16);
		m_clipkg->pack(img_size);
		m_clipkg->pack(body + 24, img_size);
	} else if (flag == 2 || flag == 3) {//login successful, but ip has be changed
		if (bodylen < 24) {
			KERROR_LOG(p->uid, "invalid dbpkg len: uid=%u len=%u explen=%u", p->uid, bodylen, 160);
			return -1;
		}	
		uint32_t gameflag = *reinterpret_cast<const uint32_t*>(body + 4 + login_session_len);
		p->create_role_flag = (gameflag & (0x01 << (loginreq->which_game - 1))) ? 1 : 0; 
	
		KDEBUG_LOG(p->uid, "LOGIN SUCCESS!\t[uid=%u flag=%d ip=%X gameflag=%X gameid=%u created=%u]", 
				p->uid, flag, loginreq->user_ip, gameflag, loginreq->which_game, p->create_role_flag);

		m_clipkg->pack(body + 4, login_session_len);
		m_clipkg->pack(p->create_role_flag);
		//ip and city information
		uint32_t last_login_ip = *reinterpret_cast<const uint32_t*>(body + 8 + login_session_len);
		uint32_t last_login_time = *reinterpret_cast<const uint32_t*>(body + 12 + login_session_len);
		m_clipkg->pack(last_login_ip);
		m_clipkg->pack(last_login_time);
		m_clipkg->pack(body + 32, 64);
		m_clipkg->pack(body + 96, 64);
	}
	send_to_user(p);

	//post mail
	if (flag == 0 || flag == 2 || flag == 3) {
		db_post_mail(p);
	}

	return 0;
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_check_active_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	if (ret != 0) {
		return check_db_err(p, ret);
	}

	uint32_t is_actived = *static_cast<const uint8_t*>(body);

	//if not actived, check if meefan
	if (!is_actived) {
		return db_check_meefan(p);
	}

	//send to player
	m_clipkg->init(p->waitcmd, p->uid, 0);
	m_clipkg->pack(is_actived);
	send_to_user(p);

	return 0;
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_set_active_code_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	if (ret != 0) {
		return check_db_err(p, ret);
	}

	uint32_t flag = *reinterpret_cast<const uint32_t*>(body);
	
	m_clipkg->init(p->waitcmd, p->uid, 0);
	uint32_t cli_flag = flag_convert(flag, dbproto_set_active_code);
	m_clipkg->pack(cli_flag);
	if (flag != 0) {
		uint32_t img_size = *reinterpret_cast<const uint32_t*>(body + 20);
		if (bodylen != 24 + img_size) {
			KERROR_LOG(p->uid, "invalid dbpkg len: uid=%u len=%u explen=%u", p->uid, bodylen, 24 + img_size);
			return -1;
		}
		m_clipkg->pack(body + 4, 16);
		m_clipkg->pack(img_size);
		m_clipkg->pack(body + 24, img_size);
	}

	send_to_user(p);
	return 0;
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_get_verif_img_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	struct db_verif_img_t {
		uint32_t flag;
		char	 imgid[16];
		uint32_t img_size;
	}__attribute__((packed));

	if (ret != 0) {
		KERROR_LOG(p->uid, "GET VERIF IMG DB ERR\t[err=%d uid=%u]", ret, p->uid);
		m_clipkg->init(p->waitcmd, p->uid, ret);
	} else {
		const db_verif_img_t* p_img = reinterpret_cast<const db_verif_img_t*>(body);
		m_clipkg->init(p->waitcmd, p->uid, 0);
		uint32_t cli_flag = flag_convert(p_img->flag, dbproto_get_verif_img);
		m_clipkg->pack(cli_flag); 
		if (p_img->flag == 5) {
			if (bodylen != (sizeof(db_verif_img_t) + p_img->img_size)) {
				KERROR_LOG(p->uid, "invalid img callback len: uid=%u len=%u explen=%u", p->uid, bodylen, 
						sizeof(db_verif_img_t) + p_img->img_size);
				return -1;				    
			}
			m_clipkg->pack(p_img->imgid, sizeof(p_img->imgid));
			m_clipkg->pack(p_img->img_size);
			m_clipkg->pack(body + sizeof(db_verif_img_t), p_img->img_size);
		}
	}
	send_to_user(p);

	return 0;
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_check_meefan_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	if (ret != 0) {
		return check_db_err(p, ret);
	}

	uint32_t is_meefan = *reinterpret_cast<const uint32_t*>(body);

	//send to player
	uint32_t is_actived = is_meefan ? 1 : 0;
	m_clipkg->init(p->waitcmd, p->uid, 0);
	m_clipkg->pack(is_actived);
	send_to_user(p);

	return 0;

}

template <typename CliProtoType>
inline int
Login<CliProtoType>::db_add_game_flag_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	login_tmpinfo_t* login_tmp = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);
	const int success = 0;
	switch (ret) {
		case success:
			m_clipkg->init(p->waitcmd, p->uid, success);
			break;
		case dberr_no_uid:
			KERROR_LOG(p->uid, "USER NOT REGISTED\t[uid=%u]", p->uid);
			m_clipkg->init(p->waitcmd, p->uid, dberr_no_uid);
			break;
		default:
			KERROR_LOG(p->uid, "ADD GAME FLAG UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
			db_add_game_flag(p, login_tmp->which_game, login_tmp->cli_channel_id);
			m_clipkg->init(p->waitcmd, p->uid, success);
			break;
	}
	send_to_user(p);

	return success;
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::check_db_err(usr_info_t* p, uint32_t ret)
{
	int err;

	switch (ret) {
	case dberr_wrong_passwd:
		err = 1;
		m_clipkg->init(p->waitcmd, p->uid, clierr_wrong_password);
		break;
	case dberr_no_uid:
		err = 3;
		m_clipkg->init(p->waitcmd, p->uid, clierr_userid_not_found);
		break;
	case dberr_user_banned:
		err = 4;
		m_clipkg->init(p->waitcmd, p->uid, clierr_user_is_banned);
		break;
	case dberr_active_code:
		m_clipkg->init(p->waitcmd, p->uid, clierr_wrong_active_code);
		break;
	default:
		err = 5;
		m_clipkg->init(p->waitcmd, p->uid, clierr_system_error);
		WARN_LOG("severe db err: uid=%u fd=%d err=%u", p->uid, p->session->fd, ret);
		break;
	}

	/*const id_info_t* ip = ip_counter_info(p->session->remote_ip);
	const id_info_t* id = id_counter_info(p->uid);
	uint32_t cnt = 0;
	if (ip) {
		cnt = ip->id_cnt;
	}
	if (id && id->id_cnt > cnt) {
		cnt = id->id_cnt;
	}
	if (cnt > m_wrong_limit) {
		send_verif_img(p, err, cnt);
		return 0;
	}*/

	return send_to_user(p);
}

template <typename CliProtoType>
inline int
Login<CliProtoType>::dbproxy_timeout(void* owner, void* data)
{
	usr_info_t*          p  = reinterpret_cast<usr_info_t*>(owner);
	Login<CliProtoType>* lg = reinterpret_cast<Login<CliProtoType>*>(data);

	KDEBUG_LOG(p->uid, "DB TIMEOUT\t[uid=%u fd=%d cmd=%u]", p->uid, p->session->fd, p->waitcmd);

	lg->m_clipkg->init(p->waitcmd, p->uid, clierr_system_error);
	lg->send_to_user(p);

	//send_warning(p, warning_db, p->waitcmd);
	send_warning("db", p->uid, p->waitcmd, 0);
	return 0;
}

#endif // LIBTAOMEEPP_MAIN_LOGIN_HPP_

