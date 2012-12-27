/*
 * =====================================================================================
 *
 *       Filename:  Csend_log.cpp
 *
 *    Description:  通过UDP发送米米修改记录和密码修改记录
 *
 *        Version:  1.0
 *        Created:  04/08/2009 10:02:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#include "Csend_log.h"

/*
 * @brief 类的构造函数，创建保存小米修改记录的FD（UDP）和创建保存密码修改记录的FD（UDP）
 */
Csend_log::Csend_log()
{
	//小米修改记录
	const char * udp_ip = NULL;
    short  udp_port;
    udp_ip = config_get_strval("CHANGE_LOG_IP");
    if(!udp_ip) {
        udp_ip = "10.1.1.89";
	}
	udp_port = config_get_intval("CHANGE_LOG_PORT", 11001);
	this->log_fd = open_socket_udp(udp_ip,udp_port,&(this->log_addr));

	//密码修改记录
	udp_ip = config_get_strval("CHANGE_PASSWD_IP");
	if (!udp_ip) {
		this->passwd_fd = this->log_fd;
		memcpy(&passwd_addr, &log_addr, sizeof(struct sockaddr_in));
	} else {
		udp_port = config_get_intval("CHANGE_LOG_PORT", 11001);
		this->passwd_fd = open_socket_udp(udp_ip, udp_port, &(this->passwd_addr));
	}

	//IP地址历史修改记录
	udp_ip = config_get_strval("CHANGE_IP_HISTORY");
	if (!udp_ip) {
		this->ip_fd = this->log_fd;
		memcpy(&ip_addr, &log_addr, sizeof(struct sockaddr_in));
	} else {
		udp_port = config_get_intval("IP_HISTORY_PORT", 11001);
		this->ip_fd = open_socket_udp(udp_ip, udp_port, &(this->ip_addr));
	}

	//论坛同步
	udp_ip = config_get_strval("DV_IP");
	if (!udp_ip) {
		this->dv_udp.fd= this->log_fd;
		memcpy(&this->dv_udp.addr , &log_addr, sizeof(struct sockaddr_in));
	} else {
		udp_port = config_get_intval("DV_PORT", 11001);
		this->dv_udp.fd= open_socket_udp(udp_ip, udp_port, &(this->dv_udp.addr));
	}

}


/* 
 * @brief 发送米米修改记录
 */
int Csend_log::send(change_log_item *p_logmesg)
{
	char buf[2000];
    PROTO_HEADER* ph = (PROTO_HEADER*)buf;
    ph->cmd_id = 0xF103;
    ph->id = p_logmesg->userid;
    ph->proto_length = sizeof (PROTO_HEADER) + sizeof(*p_logmesg) ;
    memcpy(buf + sizeof(PROTO_HEADER), p_logmesg, sizeof(*p_logmesg));
    sendto(this->log_fd, buf, ph->proto_length, 0,
			(const struct sockaddr *)&(this->log_addr), sizeof(this->log_addr));
    return SUCC;
}

/*
 * @brief 发送密码修改记录
 */
int Csend_log::send_passwd(passwd_change_item *p_in)
{
	char buf[2000];
    PROTO_HEADER* ph = (PROTO_HEADER*)buf;
    ph->cmd_id = 0xF104;
    ph->id = p_in->userid;
    ph->proto_length = sizeof (PROTO_HEADER) + sizeof(*p_in) ;
    memcpy(buf + sizeof(PROTO_HEADER), p_in, sizeof(*p_in));
    sendto(this->passwd_fd, buf, ph->proto_length, 0,
			(const struct sockaddr *)&(this->passwd_addr), sizeof(this->passwd_addr));
    return SUCC;
}
//历史修改记录
 int Csend_log::send_ip(ip_history_item *p_in)
{
	char buf[2000];
    PROTO_HEADER* ph = (PROTO_HEADER*)buf;
    ph->cmd_id = 0xF106;
    ph->id = p_in->userid;
    ph->proto_length = sizeof (PROTO_HEADER) + sizeof(*p_in) ;
    memcpy(buf + sizeof(PROTO_HEADER), p_in, sizeof(*p_in));
    sendto(this->ip_fd, buf, ph->proto_length, 0,
			(const struct sockaddr *)&(this->ip_addr), sizeof(this->ip_addr));
    return SUCC;
}

int Csend_log::dv_register(userid_t userid, dv_register_in *p_in)
{
	char buf[2000];
    PROTO_HEADER* ph = (PROTO_HEADER*)buf;
    ph->cmd_id = DV_REGISTER_CMD;
    ph->id = userid;
    ph->proto_length = PROTO_HEADER_SIZE + sizeof(*p_in) ;
    memcpy(buf + PROTO_HEADER_SIZE, p_in, sizeof(*p_in));
    sendto(this->dv_udp.fd, buf, ph->proto_length, 0,
			(const struct sockaddr *)&(this->dv_udp.addr), sizeof(this->dv_udp.addr));
    return SUCC;
}

int Csend_log::dv_change_passwd(userid_t userid, dv_change_passwd_in *p_in )
{
	char buf[2000];
    PROTO_HEADER* ph = (PROTO_HEADER*)buf;
    ph->cmd_id = DV_CHANGE_PASSWD_CMD;
    ph->id = userid;
    ph->proto_length = PROTO_HEADER_SIZE + sizeof(*p_in) ;
    memcpy(buf + PROTO_HEADER_SIZE, p_in, sizeof(*p_in));
    sendto(this->dv_udp.fd, buf, ph->proto_length, 0,
			(const struct sockaddr *)&(this->dv_udp.addr), sizeof(this->dv_udp.addr));
    return SUCC;
}


int Csend_log::dv_change_nick(userid_t userid, dv_change_nick_in *p_in )
{
	char buf[2000];
    PROTO_HEADER* ph = (PROTO_HEADER*)buf;
    ph->cmd_id = DV_CHANGE_NICK_CMD;
    ph->id = userid;
    ph->proto_length = PROTO_HEADER_SIZE + sizeof(*p_in) ;
    memcpy(buf + PROTO_HEADER_SIZE, p_in, sizeof(*p_in));
    sendto(this->dv_udp.fd, buf, ph->proto_length, 0,
			(const struct sockaddr *)&(this->dv_udp.addr), sizeof(this->dv_udp.addr));
    return SUCC;
}

