/*
 * =====================================================================================
 * 
 *       Filename:  Cregdeal.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  01/13/2008 09:42:28 AM EST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#ifndef REG_REGDEAL_H_
#define REG_REGDEAL_H_

#include <cstdlib>

#include <string>

extern "C" {
#include <libtaomee/dirtyword/dirtyword.h>
}

#include "common.h"
#include "Ccmdmap.h"
#include "register.h"

class Cregdeal;

typedef int (Cregdeal::*P_DEALFUN)(char *recvbuf, int rcvlen,char **sendbuf, int *sndlen);

class Cregdeal {
public:
	int check_register(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen);
	int userinfo_reg_account(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen);

	int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen); 
	int get_no_count_ip();
	int check_count_ip(uint32_t client_ip);
	Cregdeal();
	~Cregdeal();
protected:
	int set_sndbuf_head(char **sendbuf, int *sndlen, PROTO_HEAD *rcvph, int private_size, int ret);
private:
	Ccmdmap*		 p_cmdmap;
	//Cregact*		 regact;   
	DirtyWordHandle* m_pDWH; //for check the nick
	no_count_ip not_count_ip;
};

typedef uint32_t CMD_ID;

typedef struct {
	   CMD_ID      cmd_id;
	   P_DEALFUN   p_dealfun;
} DEALFUN;

#endif   /* ----- #ifndef CREGDEAL_INC  ----- */ 

