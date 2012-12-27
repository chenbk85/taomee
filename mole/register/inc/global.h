/*
 * =====================================================================================
 * 
 *       Filename:  global.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  02/21/2008 09:40:23 AM EST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  spark (spark), zhezhide@163.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

/*
 * This head file will define some common id and error code
 */

#ifndef  GLOBAL_INC
#define  GLOBAL_INC

//register CMD
#define CHECK_REG		1
#define USERINFO_ACCOUNT_REG	2
#define ACT_TEP_REG		9  //for temporary action
#define ACCOUNT_TEP_REG	10  //for temporary action
//for reserve register
#define ACCOUNT_RES_REG	11

#define OFFSET 1321047
//error code
#define SUCCESS 0
#define FAIL -1

#define TRUE 1
#define FALSE 0

//register protocol length error
#define REG_SIZE_ERR 20001 

//user id verify error , for the game score verify
#define CHECK_ERROR 20002

//game id verify errir, for the game score verify
//#define DB_ERR 20003

//game id not exist
#define GAME_NOT_EXIST 20004

//get the the user nick error, for the game score
#define GET_USER_NICK_ERR 20005

//submit the game score error
#define SUBMIT_SCORE_ERR 20006

//game id verify error, for the game score verify
#define GAME_SCORE_ERR 20007

#define TIME_OVER_ERR 20012

#define MORE_USER_ERR 20013

#define CMDID_NODIFINE_ERR  20015

#define  GS_CLINET_IP_ERR  20016

#define REG_SYS_ERR 2003 //system error

//FIXME
#define CSYS_ERR -10000
#define CCHECK_PASSWD_ERR -10001
#define CUSER_ID_NOACTIVE_ERR -10002
#define CDB_ERR -10003
#define CUSER_ID_NOFIND_ERR -10004
#define CPROTO_LEN_ERR  -10005
#define CUSER_ID_CREATE_ERR -10006

//激活码过期或还没有激活
#define CACT_CODE_DUE	-20007
//号码已被注册
#define CMM_USED		-20008

//The nick include dirty word
#define	 DIRTY_NICK_ERR		-20009
//The email is invalid here
#define INVALID_EMAIL		-20010
//The sex is invalid here
#define INVALID_SEX			-20011

//The user active code is error
#define ACT_CODE_ERR		 -20010
#define ACT_CODE_REP_ERR	 -20011

#define REG_CNT_ERR			-20012


#define CHECK_PROTO_HEAD(size) \
		if (ntohl(*(uint32_t *)recvbuf)!=(PROTO_HEAD_SIZE+size)) { \
			return (REG_SIZE_ERR);\
		}

//copy size bytes of src to dst. Note: size <= sizeof(src) and size<sizeof(dst)
#define SAFE_STR_SIZE	128
#define safe_str_copy(src, dst, size) \
	do{\
		memcpy (dst, src, size); \
		dst[size] = '\0'; \
	}while(0)

#define TRIM_INVAD_EMAIL(email, size) \
	do { \
		char *p, *q; \
		int i; \
		p = q = email; \
		for (i=0; i<size;++i) { \
			if (*p == ' ' || *p == '>' || *p == '<') { \
				*p++=0x00; continue; \
			} \
			else { \
				*q = *p; \
				if (p>q) *p=0x00; \
				p++; ++q;\
			} \
		} \
	}while(0)

#define SET_BLANK_TO_NULL(ptr, size) \
	{\
		int loNg_aNd_cOMPlicate_vARiAble_NaME; \
		for(loNg_aNd_cOMPlicate_vARiAble_NaME=size-1;\
				loNg_aNd_cOMPlicate_vARiAble_NaME>=0&&*(ptr+loNg_aNd_cOMPlicate_vARiAble_NaME)==0x20;\
				--loNg_aNd_cOMPlicate_vARiAble_NaME); \
		if(loNg_aNd_cOMPlicate_vARiAble_NaME<size-1){ \
			*(ptr+loNg_aNd_cOMPlicate_vARiAble_NaME+1)=0x00; \
		} \
	}


#define TEP_ARRAY_SIZE 1024
#define TEP_ARRAY_NUM  4
#define LOGINFORSTART \
	{ \
		char **ppArray=(char **)malloc(TEP_ARRAY_NUM*sizeof(char *)); \
		int iiii; \
		for (iiii=0; iiii<TEP_ARRAY_NUM; ++iiii) { \
			*(ppArray+iiii)=(char *)malloc(TEP_ARRAY_SIZE*sizeof(char)); \
			memset(*(ppArray+iiii), 0x00, TEP_ARRAY_SIZE); \
		} \

#define LOGINFOREND \
		for (iiii=0; iiii<TEP_ARRAY_NUM; ++iiii) { \
			free(*(ppArray+iiii)); \
		} \
		free(ppArray); \
	} 
#endif   /* ----- #ifndef GLOBAL_INC  ----- */

