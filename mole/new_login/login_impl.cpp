extern "C" {
#include <libtaomee/tm_dirty/tm_dirty.h>
}

#include "mole_common.hpp"
#include "mulcast.hpp"
#include "time_stamp_impl.hpp"

/* @brief 构造函数
 */
MoleLogin :: MoleLogin(SwitchService *mole_switch, TimeStampService *mole_timestamp)
		  : mole_switch(mole_switch), mole_timestamp(mole_timestamp)
{
}

/* @brief 析构函数
 */
MoleLogin :: ~MoleLogin()
{
	delete mole_switch;
	delete mole_timestamp;
}

/* @brief 得到数据包的长度
 */
int MoleLogin ::  get_other_svrs_pkg_len(int fd, const void* avail_data, int avail_len)
{
	if (fd == mole_timestamp->fd) {
		return mole_timestamp->get_timestamp_pkg_len(avail_data);
	} else if (fd == mole_switch->fd) {
		return mole_switch->get_switch_pkg_len(avail_data);
	}
	return 0;
}

/* @brief 初始化变量
 */
bool MoleLogin :: init_service()
{
	mole_switch->init_switch();
	return true;
}

/* @brief 处理客户端发过来的命令号 
 */
int MoleLogin :: proc_other_cmds(usr_info_t* usr, uint8_t* body, uint32_t bodylen)
{
	switch(usr->waitcmd) {
		case PROTO_CREATE_ROLE:
			create_role_cmd(usr, body, bodylen);
			break;

		case PROTO_GET_RECOMMEND_SVR_LIST:
			get_recommeded_svr_list_cmd(usr, body, bodylen);
			break;

		case PROTO_GET_RANGED_SVR_LIST:
			get_ranged_svr_list_cmd(usr, body, bodylen);
			break;

		case PROTO_GET_VERSION:
			get_login_version(usr, body, bodylen);
			break;

		default:
			ERROR_RETURN(("WRONG CMDID %u %u", usr->uid, usr->waitcmd), -1);
	}
	return 0;
}

/* @brief 处理dbproxy的返回命令
 */
int MoleLogin :: proc_other_dbproxy_return(usr_info_t* usr, uint16_t cmdid, const uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	switch (cmdid) {
		case DB_MOLE_REGISTER:
			return mole_register_callback(usr, body, bodylen, ret);
			
		case DB_ADD_GAME_FLAG:
			return add_mole_flag_callback(usr, body, bodylen, ret);
	
		case DB_LOGIN_MOLE:
			return login_mole_callback(usr, body, bodylen, ret);
			
		case DB_CHECK_SESSION:
			return check_session_callback(usr, body, bodylen, ret);

		default:
			ERROR_RETURN(("WRONG CMDID %u, %u", usr->uid, cmdid), -1);
	}
	return 0;
}

/* @brief 处理switch和时间服务器的返回包
 */
void MoleLogin :: proc_other_svrs_return(int fd, void* dbpkg, uint32_t pkglen)
{
	if (fd == mole_switch->fd) {
		mole_switch->handle_switch_return(reinterpret_cast<switch_head_t*>(dbpkg), pkglen);
	} else if (fd == mole_timestamp->fd) {
		mole_timestamp->handle_timestamp_return(reinterpret_cast<login_timestamp_pk_header_t*>(dbpkg), pkglen);	
	} else {
		ERROR_LOG("FD NOT SUPPORT %d %d %d", mole_switch->fd, mole_timestamp->fd, fd);
	}
	return;
}

/* @brief 创建用户在游戏中的角色(包括昵称，颜色)
 */
int MoleLogin :: create_role_cmd(usr_info_t *p, const uint8_t *body, uint32_t bodylen)
{
	CHECK_BODY_LEN(bodylen, sizeof(role_info_t));
	role_info_t *role_info = (role_info_t*)body;
		
	char nick[nick_len + 1];
	memcpy(nick, role_info->nick, nick_len);
	nick[nick_len] = '\0';
	if(tm_dirty_check(7, (char*)(nick)) > 0) {
		m_clipkg->init(p->waitcmd, p->uid, INVALID_NICK_NAME);
		return send_to_user(p);
	}

	struct mole_register_in reg;
	memcpy(reg.nick, (char*)(body + sess_len), nick_len);
	reg.color = ntohl(*(uint32_t*)(body + sess_len + nick_len));
	/*save channel*/
	*(uint32_t*)p->tmpinfo = ntohl(*(uint32_t*)(body + sess_len + nick_len + 4));
	/*varify the session*/
	if (!check_session(p, (uint8_t *)role_info->sess, sess_len)) {
		return -1;
	}

	DEBUG_LOG("ROLE REGISTER\t[uid=%u, nick=%16s, color=%u]", p->uid, reg.nick, reg.color);

    return send_to_dbproxy(p, p->uid, DB_MOLE_REGISTER, &reg, sizeof(mole_register_in));
}

/* @brief 处理数据库在USER库中增加用户角色信息的返回包
 */
int MoleLogin :: mole_register_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	struct in_addr cliaddr;
    memcpy(&cliaddr, &(p->session->remote_ip), 4);
	NOTI_LOG("mole register callback userid:%u,ip:%s", p->uid, inet_ntoa(cliaddr));

	switch (ret) {
	case SUCCESS:
	#ifndef TW_VER	
		send_register_num_to_stat(p);
	#endif
    	// update the gameflag
    	DEBUG_LOG("ADD GAME FLAG\t[uid=%u]", p->uid);
		break;

      	case USER_ID_EXISTED_ERR:
	  		DEBUG_LOG("ROLE CREATED\t[uid=%u]", p->uid);
	  		break;

	 	default:
			DEBUG_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
			m_clipkg->init(p->waitcmd, p->uid, SYSTEM_ERR);
			return send_to_user(p);
	}

  	//return this->add_game_flag(p );
  	uint32_t channel = *(uint32_t*)p->tmpinfo;
  	return this->db_add_game_flag(p, 1, channel);
}

/* @brief 发送注册人数到统计平台，统计总共和各个渠道的注册人数
 */
void MoleLogin :: send_register_num_to_stat(usr_info_t *p)
{
	int tmp = 1;
	uint32_t msglog_type;
	uint32_t channel = *(uint32_t*)p->tmpinfo;
	msglog_type = mole_register_channel_offset + channel;
	statistic_msglog(msglog_type, (void *)&tmp, sizeof tmp);
	msglog_type = mole_register_mimi_offset;
	uint32_t msg_info[] = {p->uid, p->uid, time(NULL)};
	statistic_msglog(msglog_type, msg_info, sizeof(msg_info));
}

/* @brief 处理数据库在USER_INFO库中设置用户已注册哪个项目的返回包
 */
int MoleLogin :: add_mole_flag_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{

	switch (ret) {
    case SUCCESS:
		m_clipkg->init(p->waitcmd, p->uid, SUCCESS);
		return send_to_user(p);
       	break;

    case USER_ID_NOFIND_ERR:
       	DEBUG_LOG("USER NOT REGISTED\t[uid=%u]", p->uid);
		m_clipkg->init(p->waitcmd, p->uid, USER_NOT_FIND);
		send_to_user(p);
		return -1;
       	break;

     default:
      	ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
	}

	uint32_t mole_flag = 1;
   	send_to_dbproxy(NULL, p->uid, DB_ADD_GAME_FLAG, &mole_flag, sizeof(mole_flag));
	m_clipkg->init(p->waitcmd, p->uid, SUCCESS);
	return send_to_user(p);
}

/* @brief 得到推荐服务器列表
 */
int MoleLogin :: get_recommeded_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	 CHECK_BODY_LEN(bodylen, 20);

	char *sessbody = (char*)body;
	p->channel = ntohl(*(uint32_t*)(body + sess_len));
	mole_login_out *user_login_info = reinterpret_cast<mole_login_out*>(p->tmpinfo);
	user_login_info->vip_flag    = 0;
	user_login_info->friendcount = 0;
	DEBUG_LOG("RECOMMD SVR\t[uid=%d]", p->uid);

	if (p->uid != 0) {
		struct session_server_data {
			uint32_t game_id;
			uint8_t	session[sess_len];
			uint32_t flag;
		}__attribute__((packed)) data;

		data.game_id = 1;
		memcpy(data.session, sessbody, sess_len);
		data.flag = 0;
    	return send_to_dbproxy(p, p->uid, DB_CHECK_SESSION,  &data, sizeof(struct session_server_data));
	} else {
	  return mole_switch->get_recommended_svr_list(p);
	}
	return 0;
}

/* @breif 校验用户传过来的session
 */
int MoleLogin :: check_session(usr_info_t *p, uint8_t *sessbody, int len)
{
	login_session_t sess;
	unpkg_auth((uint8_t*)sessbody, len, &sess);
   	if ( verify_session(p->uid, &sess) != SUCCESS ) {
		return -1;
   	}
	return 1;
}

/* @brief 处理数据库返回vip_flag和frieds的返回包
 */
int MoleLogin :: login_mole_callback(usr_info_t *p, const uint8_t* body, uint32_t bodylen, int ret)
{
    if (ret) {
        DEBUG_LOG("LOGIN FAILED\t[err=%d uid=%u]", ret, p->uid);
		process_login_mole_ret(p, ret);
		return SUCCESS;
    }
    CHECK_VAL_GE(bodylen, 8);
    mole_login_out* user_login_info = (mole_login_out*)(body);
    CHECK_BODY_LEN(bodylen, 8 + 4 * user_login_info->friendcount);
    if (user_login_info->friendcount > max_friend_num) {
        ERROR_LOG("severe db error: uid=%u frdcnt=%u", p->uid, user_login_info->friendcount);
        user_login_info->friendcount = max_friend_num;
    }
    *(uint32_t*)p->tmpinfo = user_login_info->vip_flag;
    *(uint32_t*)(p->tmpinfo + 4) = user_login_info->friendcount;
    memcpy(p->tmpinfo + 8, user_login_info->frienditem, 4*(user_login_info->friendcount));
    if (user_login_info->friendcount != 0) {
		get_time_stamp(p, user_login_info);
		mulcast_friends_login(p, user_login_info);
        return 0;
    }
    return mole_switch->get_recommended_svr_list(p);
}

/* @brief 处理得到vip_flag, friends DB返回的错误码
 */
int MoleLogin :: process_login_mole_ret(usr_info_t *p, int ret) 
{
    switch (ret) {
            case USER_ID_NOFIND_ERR:
				m_clipkg->init(p->waitcmd, p->uid, USER_NOT_FIND);
				return send_to_user(p);
                break;

            case USER_NOUSED_ERR:
				m_clipkg->init(p->waitcmd, p->uid, USER_BE_BAN);
				return send_to_user(p);
                break;

            case USER_OFFLIN24_ERR:
				m_clipkg->init(p->waitcmd, p->uid, USER_BE_24_BAN);
				return send_to_user(p);
                break;

            case USER_NOT_ENABLED_ERR:
				m_clipkg->init(p->waitcmd, p->uid, NO_ACTIVED);
				return send_to_user(p);
                break;

            case USER_OFFLINE_SEVEN_DAY_ERR:
				m_clipkg->init(p->waitcmd, p->uid, USER_BE_7_DAY_BAN);
				return send_to_user(p);
                break;

            case USER_OFFLINE_FOURTEEN_DAY_ERR:
				m_clipkg->init(p->waitcmd, p->uid, USER_BE_14_DAY_BAN);
				return send_to_user(p);
				break;

			case USER_ANXI_SERVICE_OUT_OF:
				m_clipkg->init(p->waitcmd, p->uid, USER_OUT_OF_ANXIN);
				return send_to_user(p);
                break;

            default:
                ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
				m_clipkg->init(p->waitcmd, p->uid, SYSTEM_ERR);
				return send_to_user(p);
                break;
	}
}

/* @brierf 从时间服务器得到用户更新的时间
 */
int MoleLogin :: get_time_stamp(usr_info_t *p, mole_login_out *user_login_info)
{
    static uint8_t login_timestamp_sendbuf[1024 * 1024];
	int idx = sizeof(login_timestamp_pk_header_t);
	*(uint32_t*)login_timestamp_sendbuf = p->uid;
	taomee::pack(login_timestamp_sendbuf, user_login_info->friendcount, idx);
	for (uint32_t i = 0; i != user_login_info->friendcount; ++i) {
		taomee::pack(login_timestamp_sendbuf, user_login_info->frienditem[i], idx);
	}
	mole_timestamp->init_timestamp_head(p, login_timestamp_sendbuf, idx, TIMESTAMP_GET_FRIENDTIME);
	mole_timestamp->send_to_timestamp(p, login_timestamp_sendbuf, idx);
	return 0;
}

/* @brief 广播好友, 用户登入
 */
int MoleLogin :: mulcast_friends_login(usr_info_t *p, mole_login_out *user_login_info)
{
  	static uint32_t friends[max_friend_num + 2];
	friends[0] = p->uid;
	friends[1] = user_login_info->friendcount;
	memcpy(&(friends[2]), user_login_info->frienditem, sizeof(uint32_t) * user_login_info->friendcount);

	int pkg_len = sizeof(uint32_t) * (user_login_info->friendcount + 2); 
	multicast_player_login((uint8_t*)friends, pkg_len);
	return 0;
}

/* @breif 得到用户指定范围内的服务器列表
 */
int MoleLogin :: get_ranged_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL_GE(bodylen, 12); 
	/*get the client's request package body*/
	uint32_t friendcount;
	uint32_t startid;
	uint32_t endid;
	int idx = 0;
	
	taomee::unpack(body, startid, idx);
	taomee::unpack(body, endid, idx);
	if ((startid < min_valid_svr) || (endid > max_valid_svr)
            || (startid > endid) || ((endid - startid) > 100)) {
        INFO_LOG("INVALID SVR ID\t[startid=%u endid=%u uid=%u]", startid, endid, p->uid);
		m_clipkg->init(p->waitcmd, p->uid, SUCCESS);
		m_clipkg->pack(0);
		return send_to_user(p);
    }

	DEBUG_LOG("RANGED SVR\t[uid=%u startid=%u endid=%u]", p->uid, startid, endid);
	taomee::unpack(body, friendcount, idx);
	if (friendcount > max_friend_num) {
		ERROR_LOG("[friendcount=%u] is invalid",friendcount) ;
		return -1;
	}
	/*according to the friend count, check the size of the buffer*/
//	CHECK_BODY_LEN(static_cast<size_t>(bodylen), 12 + (friendcount * sizeof(userid_t)));
	CHECK_BODY_LEN(bodylen, 12 + (friendcount * 4));
	/* unpack friend list*/
	static uint32_t allfriendid[max_friend_num];
	for (uint32_t i = 0; i != friendcount; ++i) {
		taomee::unpack(body, allfriendid[i], idx);
	}
	return mole_switch->get_ranged_svr_list(p, startid, endid, friendcount, allfriendid);
}

/* @brief 如果switch和时间服务器与登入服务器断开，重新初始化fd
 */
void MoleLogin :: on_other_svrs_fd_closed(int fd)
{
	if (fd == mole_switch->fd) {
		mole_switch->fd = -1;
	} else if (fd == mole_timestamp->fd) {
		mole_timestamp->fd = -1;
	}
	return ;
}

int MoleLogin :: check_session_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	switch (ret) {
    	case SUCCESS:
			DEBUG_LOG("CHECK SESSION SUCC\t[uid=%u]", p->uid);
			break;
    	case USER_ID_NOFIND_ERR:
       		DEBUG_LOG("USER NOT REGISTED\t[uid=%u]", p->uid);
			m_clipkg->init(p->waitcmd, p->uid, USER_NOT_FIND);
			send_to_user(p);
			return -1;
		case USER_CHECK_SESSION_ERR:
       		ERROR_LOG("USER SESSION ERR\t[uid=%u]", p->uid);
			return -1;
     	default:
      		ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
			return -1;
	}

	/*get vip_flag, friends*/
	return send_to_dbproxy(p, p->uid, DB_LOGIN_MOLE, NULL, 0);

}

int MoleLogin::get_login_version(usr_info_t* p, uint8_t *body, uint32_t bodylen)
{
	uint8_t buf[1024] = {0};
	int idx = sizeof(cli_login_pk_header_t);
	const char* version_str = "login-2011-12-30";
	memcpy((buf+idx), version_str, strlen(version_str));
	//int len = sprintf((char*)(buf+idx), "version:%s", version_str);
	idx += strlen(version_str);
	init_proto_head(buf, idx, PROTO_GET_VERSION, p->uid, SUCCESS);
	return send_to_self(p, buf, idx);
}



