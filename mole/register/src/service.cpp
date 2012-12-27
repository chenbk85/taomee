#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <iostream>

extern "C" {
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
}

#include "Cregdeal.h"
#include "service.h"
#include "Cclientproto.h"
#include "CNetComm.h"
#include "common.h"
#include "proto.h"

Cregdeal*     pReg;
Cclientproto* cpo;
CNetComm*     net;
uint32_t      client_ip; //used for decrypt
char*         statfile;
//uint32_t 	uid_id_file_size;
uint32_t max_register_channel;

int handle_init(int argc, char** argv, int pid_type)
{
	switch (pid_type) {
	case PROC_MAIN:
		break;
	case PROC_WORK:
		pReg = new Cregdeal();
		cpo  = new Cclientproto(config_get_strval("dbproxy_ip"),config_get_intval ("dbproxy_port",0));
		net  = new CNetComm(config_get_strval("verify_ip"), config_get_intval("verify_port",0));
		statfile = config_get_strval("statistic_logfile");
		max_register_channel = config_get_intval("max_register_channel",0);
		if (pReg->get_no_count_ip() == -1) {
			BOOT_LOG(-1, "get no count ip wrong");
		}
	
		break;
	case PROC_CONN:
		break;
	default:
		BOOT_LOG(-1, "Invalid pid_type=%d", pid_type);
	}

	return 0;
}



int handle_input(const char* buffer, int length, const skinfo_t *sk)
{
	const char* policy   = "<policy-file-request/>";
	const char* response = "<?xml version=\"1.0\"?>"
							"<!DOCTYPE cross-domain-policy SYSTEM \"/xml/dtds/cross-domain-policy.dtd\">"
							"<cross-domain-policy>"
							"<site-control permitted-cross-domain-policies=\"all\"/>"
							"<allow-access-from domain=\"*\" to-ports=\"*\" />"
							"</cross-domain-policy>";

	if (length < 4) {
		ERROR_LOG("invalid pkg len\t[len=%u] [ip=%x]", length, sk->remote_ip);
		return 0;
	}

	if (length == 23 && !memcmp(buffer, policy, 23)) {
		send(sk->sockfd, response, strlen(response) + 1, 0);

		TRACE_LOG("Policy Req [%s] Received, Rsp [%s] Sent", policy, response);
		return 0;
	}

	int reallen = ntohl(*((uint32_t* )buffer));

	if (reallen < static_cast<int>(PROTO_HEAD_SIZE) || reallen > 4096) {
		ERROR_LOG("invalid pkg len\t[len=%u] [ip=%x]", reallen, sk->remote_ip);
		return -1; //close this connection
	}

	return reallen;
}

int handle_process(char* recvbuf, int rcvlen, char** sendbuf, int* sndlen, const skinfo_t* sk)
{
	client_ip = sk->remote_ip;
	
	uint32_t cmdid = ntohl(((PROTO_HEAD_PTR)recvbuf)->cmd_id);
	if (cmdid == 30000) {
		memcpy(*sendbuf, recvbuf, *sndlen);
		PROTO_HEAD* head = reinterpret_cast<PROTO_HEAD*>(*sendbuf);
		head->len    = htonl(*sndlen);
		head->result = 0;
		return 0;
	}
	int ret = pReg->deal(recvbuf, rcvlen, sendbuf, sndlen);
	if ( ret != SUCCESS ) {
		DEBUG_LOG( "============REQUEST PROCESS END(ERROR)[%d]===========", ret);
		 //当处理出错时,标准返回  没有私有域
		*sndlen  = PROTO_HEAD_SIZE;
		*sendbuf = reinterpret_cast<char*>(malloc(*sndlen));
		if (*sendbuf == 0) {
			return SYS_ERR;
		}

		memcpy(*sendbuf, recvbuf, *sndlen);
		PROTO_HEAD* head = reinterpret_cast<PROTO_HEAD*>(*sendbuf);
		head->len    = htonl(*sndlen);
		head->result = htonl(ret);
	}

	return SUCCESS;
}

void handle_fini(int pid_type)
{
	switch (pid_type) {
	case PROC_MAIN:
		break;
	case PROC_WORK:
		delete pReg;
		delete cpo;
		delete net;
		break;
	default:
		break;
	}
}
