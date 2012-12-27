/*
 * =====================================================================================
 * 
 *       Filename:  Csync_user.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年01月14日 16时54分40秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#ifndef  CSYNC_USER_INC
#define  CSYNC_USER_INC
#include "proto.h"
typedef struct  tuser_flag {
	long msg_flag;   
	short sendcount;
	userid_t userid;
	uint32_t flag;
} __attribute__((packed)) TUSER_FLAG;

typedef struct  tattire_add{
	long msg_flag;   
	short sendcount;
	userid_t userid;
	uint32_t attiretype;
	uint32_t attireid;
	uint32_t addcount;
} __attribute__((packed))   TATTIRE_ADD;

typedef struct  tdv_change_passwd{
	long  msg_flag;   
	short sendcount;
	userid_t userid;
	char   newpasswd[16];
} __attribute__((packed))    TDV_CHANGE_PASSWD;

typedef struct  tdv_change_nick{
	long  msg_flag;   
	short sendcount;
	userid_t userid;
	char   newnick[16];
} __attribute__((packed))    TDV_CHANGE_NICK;



typedef struct  tdv_register{
	long  msg_flag;   
	short sendcount;
	userid_t userid;
	char   passwd[16];
	uint32_t  sex;   
	char   nick[16];
	char   email[64];
} __attribute__((packed))    TDV_REGISTER;



class Csync_user
{

  public:

    /* ====================  LIFECYCLE   ========================================= */

    Csync_user (int msg_list_key );  /* constructor */
	int send(char  * buf ,int len  );

  protected:
	int qid;

  private:

}; /* -----  end of class  Csync_user  ----- */

#endif   /* ----- #ifndef CSYNC_USER_INC  ----- */

