/*
 * =====================================================================================
 *
 *       Filename:  Cregdeal.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/13/2008 10:13:33 AM EST
 *       Revision:  none
 *       Compiler:  gcc
 *        Company:  TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:  set tabstop=4 
 * 		
 * =====================================================================================
 */

extern "C" {
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/project/stat_agent/msglog.h>
#include <sys/types.h>
#include <arpa/inet.h>
}

#include <cstring>
#include <algorithm>
#include <errno.h>
#include "../inc/Cregdeal.h"
#include "../inc/register.h"
#include "../pkg/Cclientproto.h"
#include "../inc/CNetComm.h"
#include "../inc/service.h"
#include "../inc/global.h"
#include "../pkg/proto.h"
#include "../pkg/common.h"




extern Ccmdmap*       cp;
extern Cclientproto*  cpo;
extern CNetComm*      net;
extern uint32_t       client_ip;
extern char*          statfile;
extern struct in_addr no_count_ip;
extern uint32_t max_register_channel;
DEALFUN g_dealfun[] = {
	{CHECK_REG, &Cregdeal::check_register}, 
	{USERINFO_ACCOUNT_REG, &Cregdeal::userinfo_reg_account}
};

#define DEALFUN_COUNT (int(sizeof(g_dealfun)/sizeof(g_dealfun[0])))

Cregdeal::Cregdeal()
{
	p_cmdmap = new Ccmdmap[DEALFUN_COUNT];

	for (int i = 0; i != DEALFUN_COUNT; ++i) {
		(p_cmdmap + i)->set(g_dealfun[i].cmd_id, g_dealfun[i].p_dealfun);
	}

}

Cregdeal::~Cregdeal()
{
	delete[] p_cmdmap;
}

int Cregdeal::deal(char* recvbuf, int rcvlen, char** sendbuf, int* sndlen)
{
	uint32_t cmdid = ntohl(((PROTO_HEAD_PTR)recvbuf)->cmd_id);
	DEBUG_LOG("PKG RECEIVED\t[cmd=%u]", cmdid);

	Ccmdmap cp;
	cp.set(cmdid, NULL);	

	int i;
	for (i = 0;  i != DEALFUN_COUNT && p_cmdmap[i].get_cmd_id() != cp.get_cmd_id(); ++i) {
		;
	}
	if (i == DEALFUN_COUNT) {
		return CMDID_NODIFINE_ERR;
	}

	return (this->*(p_cmdmap[i].get_fun()))(recvbuf, rcvlen, sendbuf, sndlen);
}


int Cregdeal::check_register(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen)
{
	uint32_t pkg_len = ntohl(*(uint32_t *)recvbuf);
	if (pkg_len != PROTO_HEAD_SIZE) {
		return REG_SIZE_ERR;
	}
	PROTO_HEAD_PTR header = (PROTO_HEAD_PTR)recvbuf;
	if (header->userid == 0) {
		return SUCC;
	}
	uint32_t userid = ntohl(header->userid) - OFFSET;
	uint32_t flag;
	int ret =  cpo->userinfo_get_gameflag(userid, &flag);
	if ((ret != SUCC) || ( (flag & 0x02) == 0)) {
		if (set_sndbuf_head(sendbuf, sndlen, (PROTO_HEAD_PTR)recvbuf, 0, CHECK_ERROR) != SUCCESS) {
			return REG_SYS_ERR;
		}
	} else {
		if (set_sndbuf_head(sendbuf, sndlen, (PROTO_HEAD_PTR)recvbuf, 0, 0) != SUCCESS) {
	   		 return REG_SYS_ERR;
		}
	}
	return SUCCESS;
}


int Cregdeal::userinfo_reg_account(char* recvbuf, int rcvlen, char** sendbuf, int* sndlen)
{
	int ret;
	uint32_t userid, user_id_exist = 0;
	char  passwd[PASSWD_SIZE/2];
	char  safe_str[SAFE_STR_SIZE];
	PROTO_HEAD_PTR   hdp;
	USERINFO_REG_BODY_REQ_PTR byp;

	uint32_t pkg_len = ntohl(*(uint32_t *)recvbuf);
	if (pkg_len != (PROTO_HEAD_SIZE + USERINFO_REG_REQ_BODY_SIZE) 
		&& pkg_len != (PROTO_HEAD_SIZE + USERINFO_REG_EXT_BODY_SIZE)) {
		DEBUG_LOG("USERINFO PKG LEN ERROR\t[pkglen=%u expected len=%u or %u]",
					pkg_len, (PROTO_HEAD_SIZE + USERINFO_REG_REQ_BODY_SIZE), (PROTO_HEAD_SIZE 
						+ USERINFO_REG_EXT_BODY_SIZE));
		return REG_SIZE_ERR;
	}

	hdp = (PROTO_HEAD_PTR)recvbuf;
	byp = (USERINFO_REG_BODY_REQ_PTR)(recvbuf + PROTO_HEAD_SIZE);
	str2hex(byp->passwd, PASSWD_SIZE, passwd);

	safe_str_copy(byp->email, safe_str, EMAIL_SIZE); //safe copy


	//check the email here
	//not include `@` is error
	TRIM_INVAD_EMAIL(safe_str, EMAIL_SIZE);
	if (!strchr(safe_str, '@')) {
		DEBUG_LOG("INVALID EMAIL ADDRESS\t[%s]", safe_str);
		return INVALID_EMAIL;
	}

	// check if number of registered ids today have or haven't exceeded the max limit
	if (((client_ip & 0xFFFF) != 0XA8C0) 
			&& (client_ip != 0X7D635072)
			&& (client_ip != 0X76635072)
			&& (client_ip != 0X92635072)
			&& (client_ip != 0X9A635072)

			&& (client_ip != 0XCA635072)
			&& (client_ip != 0XCB635072)
			&& (client_ip != 0XCC635072)

			&& (!this->check_count_ip(client_ip))) {
		DEBUG_LOG("enter ip count");
		char rcv[4];
		struct ip_cnt {
			uint32_t commdid;
			uint32_t ip;
		} ip_check;
		ip_check.commdid = htonl(1);
		ip_check.ip = client_ip;
		if (net->net_io(reinterpret_cast<char*>(&ip_check), 8, rcv, 4) != 4) {
			DEBUG_LOG("ERROR OCCURRED WHILE CHECKING IP COUNT");
		} else {
			uint32_t cnt;
			memcpy(&cnt, rcv, 4);
			cnt = ntohl(cnt);
			if (cnt > MAX_REG_CNT) {
				DEBUG_LOG("NUM OF REG IDS EXCEEDED PER IP MAX LIMIT\t[ip=%u cnt=%u]", client_ip, cnt);
				return REG_CNT_ERR;
			}
		}	
	}
Label:
	//得到userid
	//ret = this->regact->create_user_id(userid);
	userid_get_userid_out ret_out;		
	ret=cpo->userid_get_userid(& ret_out );
	if (ret != SUCCESS) {
		DEBUG_LOG("FAIL TO GET A USER ID");
		return CUSER_ID_CREATE_ERR;
	}
	userid=ret_out.userid;

	//send information to db
	userinfo_register_ex_in user_db;
	memset(&user_db, 0x00, sizeof (userinfo_register_ex_in));
	memcpy(user_db.passwd, passwd, sizeof (passwd));
	// Make sure the value of `sex` is 0 or 1
	user_db.sex = !!(byp->sex);
	user_db.birthday = ntohl(byp->birthday);
	memcpy(user_db.email, safe_str, EMAIL_SIZE);
	user_db.reg_addr_type = ntohl(byp->location);
	user_db.ip = client_ip;


	if (pkg_len == (PROTO_HEAD_SIZE + USERINFO_REG_EXT_BODY_SIZE)) {
		//include extended information here
		char *extend_data = byp->data;
		uint16_t t_data;
		memcpy(user_db.mobile, extend_data, TEL_SIZE);
		extend_data += TEL_SIZE;
		memcpy(&t_data, extend_data, sizeof(t_data));
		user_db.addr_province = htons(t_data);
		extend_data += PROV_SIZE;
		memcpy(&t_data, extend_data, sizeof(t_data));
		user_db.addr_city = ntohs(t_data);
		extend_data += CITY_SIZE;
		memcpy(user_db.addr, extend_data, ADR_SIZE);
		extend_data += ADR_SIZE;
		memcpy(user_db.signature, extend_data, SIG_SIZE);
	}

	ret = cpo->userinfo_register(userid, &user_db, &user_id_exist);  
	if (ret != SUCCESS) {
		DEBUG_LOG("DB ERR\t[uid=%u ret=%d]", userid, ret);

		if (ret == 1104) { // User ID already existed
			goto Label;
		}
		//this->regact->cancel_user_id(userid);
		cpo->userid_set_noused(userid );

		return ret;
	}


	if (set_sndbuf_head(sendbuf, sndlen, (PROTO_HEAD_PTR)recvbuf, SESSION_LEN, SUCCESS) != SUCCESS) {
		//this->regact->cancel_user_id(userid);
		return REG_SYS_ERR;
	}

	uint32_t inbuf[4];
    inbuf[0] = client_ip;
    inbuf[1] = time(0);
    inbuf[2] = userid;
    inbuf[3] = inbuf[1];
    des_encrypt_n(LOGIN_DES_KEY, (char *)inbuf, (char *)(*sendbuf + PROTO_HEAD_SIZE), 2);
	((PROTO_HEAD_PTR)*sendbuf)->userid = htonl(userid);
	
	INFO_LOG("userid[%u]email[%s]sex[%c]birth[%u]", userid, safe_str, 
			 byp->sex, ntohl(byp->birthday));

	DEBUG_LOG("REG FROM CHANNEL\t[%u]", user_db.reg_addr_type);
    // stat client
    time_t   ts  = time(0);
    uint32_t tmp = 1;

    // 统计消息起始ID 0x2010101
    /*  0:51mole    1:92game    2:766
        3:QQ空间    4:7K7K      5:4399
        6:3839      7:2144      8:小游戏 
        9:唤醒      10:向导     11:QQ礼活动
        12:摩尔大使 13:QQmini   14:4399xyx
        15:cks      16:yx007    17:新浪
        18:宏梦     19:19       20:彩虹堂
        21:gameyes  22:游戏22   23:41717小游戏
        24:f1212    25:游戏82   26:翼动小游戏
        27:成都小游戏网 28:92小游戏
		29: 大玩国  30:赛尔, 31哈奇小镇, 32 61.com*/
	if (user_db.reg_addr_type > max_register_channel) {
		user_db.reg_addr_type = 0;
	}
	DEBUG_LOG("====MAX %u", max_register_channel);
    msglog(statfile, 0x06010101 + user_db.reg_addr_type, ts, &tmp, sizeof(uint32_t));

	msglog(statfile, 0x06010001, ts, &userid, 4);
//	this->regact->send_email(safe_str[0], pUtil->get_config_strval("send_mail_cmd"), userid, safe_str[1]);
	DEBUG_LOG("---------------REGISTER ACCOUNT END (SUCCESS)-------------");

	////////for mole emissary
	/*
	if (ntohl(byp->location) == MOLE_EMISSARY_LOC) {
		LOG(7, "email : [%64.64s]", byp->email);
		LOG(7, "cmd : [%s]", pUtil->get_config_strval("send_mole_mail_cmd"));
		this->regact->send_mole_emissary_email(byp->email, pUtil->get_config_strval("send_mole_mail_cmd"), userid);
	}
	*/

	return SUCCESS;
}







int Cregdeal::set_sndbuf_head(char** sendbuf, int* sndlen, PROTO_HEAD* rcvph, int private_size, int ret)
{
	PROTO_HEAD_PTR hdp;
	*sndlen = PROTO_HEAD_SIZE + private_size;
	if (!(*sendbuf = (char*)malloc(*sndlen))) {
		return REG_SYS_ERR;
	}

	hdp = (PROTO_HEAD_PTR )*sendbuf;
	memcpy(hdp, rcvph, PROTO_HEAD_SIZE);
	hdp->len    = htonl(*sndlen);
	hdp->result = htonl(ret);

	return SUCCESS;
}

/* @brief 得到不需要记录到IP_COUNT的IP地址 
 */
int Cregdeal::get_no_count_ip()
{
	uint32_t i = 0;	
	char *p_ip_str = strtok(config_get_strval ("NOT_COUNT_IP"), ",");
	if (p_ip_str == NULL) {
		this->not_count_ip.cnt = 0;
		return SUCCESS;
	}
    int ret = inet_pton(AF_INET, p_ip_str, &this->not_count_ip.ip[i]);
	if (ret <= 0) {
		DEBUG_LOG("no count ip is wrong!");
		return -1;
	}
    i++;
	this->not_count_ip.cnt = i;
    while ((p_ip_str = strtok(NULL, ","))) {
		if (i >= 10) {
			return -1;
		}
		if(inet_pton(AF_INET, p_ip_str, &this->not_count_ip.ip[i]) <= 0) {
			return  -1;
		} else {
			i++;
		}
		this->not_count_ip.cnt = i;
	}
	return SUCCESS;
}
/* @brief 校验client_ip是否需要记录到IP_COUNT中
 * @param client_ip 需要校验的IP地址 
 */
int Cregdeal::check_count_ip(uint32_t client_ip)
{
    for (uint32_t i = 0; i < this->not_count_ip.cnt; i++) {
        if (client_ip == not_count_ip.ip[i].s_addr) {
            return 1;
        }
    }
    return 0;
}

