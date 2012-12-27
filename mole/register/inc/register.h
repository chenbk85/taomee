#ifndef REGSERV_REGISTER_H_
#define REGSERV_REGISTER_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "packet.h"

//used for active code
#define ACT_KEY "12345678"
#define FILLKEY1 4444
#define FILLKEY2 4444
#define FILLKEY3 4444
#define TIME_NEXT_SEP 44
#define TIME_SEP_WEEK 604800  //used for active


//////////////////for mole emissary 
#define MOLE_EMISSARY_KEY	"mole_emissary"
#define MOLE_EMISSARY_RAND	 444999
#define MOLE_EMISSARY_LOC	 12
////////////////////////////////

//used for the genarate the active code for another user to register
#define ACT_CNT_KEY		"onlyajoy"
#define ACT_MAX_CNT		50
#define RAND_NUM		9413

//////////////////

// 最大注册个数
#define MAX_REG_CNT     500

/*************************end*********************************/

typedef struct reg_body_req {
	char		nick[NICK_SIZE];
	char		passwd[PASSWD_SIZE];
	uint32_t   	family;
	char		email[EMAIL_SIZE];
	uint32_t	location;
	char		sex;
	uint32_t	birthday;
	/*
	char		tel[TEL_SIZE];
	uint16_t	prov;
	uint16_t	city;
	char		adr[ADR_SIZE];
	char		sig[SIG_SIZE];
	*/
	char		data[0];
} __attribute__((packed)) REG_BODY_REQ, *REG_BODY_REQ_PTR;


typedef struct userinfo_reg_body_req {
	char passwd[PASSWD_SIZE];
	char email[EMAIL_SIZE];
	char sex;
	uint32_t location;
	uint32_t birthday;
	char 	data[0];
}__attribute__((packed)) USERINFO_REG_BODY_REQ, *USERINFO_REG_BODY_REQ_PTR;

//local 
typedef struct reg_res_body_req {
	uint32_t	user_id;
	char		nick[16];
	char		passwd[32];
	char		email[64];
	char		sex;
	uint32_t	family;
} __attribute__((packed)) REQ_RES_BODY_REQ, *REQ_RES_BODY_REQ_PTR;

#define REQ_RES_BODY_REQ_SIZE	sizeof(REQ_RES_BODY_REQ)


typedef struct  stru_active_key {
	uint32_t	user_id;
	uint32_t	pri_key1;
	uint32_t	now;
	uint32_t	pri_key2;
	uint32_t	now_next;
	uint32_t	pri_key3;
} __attribute__((packed))   STRU_ACTIVE_KEY, *STRU_ACTIVE_KEY_PTR;

typedef struct act_body_req {
	char		active_code[48];
}__attribute__((packed)) ACT_BODY_REQ, *ACT_BODY_REQ_PTR;

struct no_count_ip {
	uint32_t cnt;
	struct in_addr ip[10];
};

#define REG_REQ_BODY_SIZE  	(sizeof(REG_BODY_REQ))
#define USERINFO_REG_REQ_BODY_SIZE (sizeof(USERINFO_REG_BODY_REQ))
#define REG_EXT_BODY_SIZE	(REG_REQ_BODY_SIZE+TEL_SIZE+PROV_SIZE+CITY_SIZE+ADR_SIZE+SIG_SIZE)
#define USERINFO_REG_EXT_BODY_SIZE	(USERINFO_REG_REQ_BODY_SIZE+TEL_SIZE+PROV_SIZE+CITY_SIZE+ADR_SIZE+SIG_SIZE)

#define ACT_BODY_REQ_SIZE	(sizeof(ACT_BODY_REQ))
#define SESSION_LEN			16


#endif // REGSERV_REGISTER_H_
