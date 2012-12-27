/*
 * =====================================================================================
 * 
 *       Filename:  Csync_log.h
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

#ifndef  CSYNC_LOG_INC
#define  CSYNC_LOG_INC
#include "logproto.h"
#include <string.h>


#define save_protolog(t_struct) \
inline int SAVE_PROTOLOG_##t_struct (T##t_struct value ){ \
	char  buf[PROTOBUF_MAXLEN]; \
	char* _proto_buf=buf+4;\
	*((int*)buf)=1;\
	((LOG_PROTO_HEADER*)_proto_buf)->cmd_id=LOG_##t_struct##_CMD; \
	((LOG_PROTO_HEADER*)_proto_buf)->proto_length=LOG_PROTO_HEADER_SIZE + sizeof(value);\
	memcpy(_proto_buf+LOG_PROTO_HEADER_SIZE, &value,sizeof(value) ); \
 	return this->send(buf,((LOG_PROTO_HEADER*)_proto_buf)->proto_length  );\
}

class Csync_log
{
  public:
    Csync_log (int msg_list_key );  /* constructor */
	save_protolog(LOGIN ); //use it like: SAVE_PROTOLOG_LOGIN 
	save_protolog(USER_REG );
	save_protolog( ONLINE_USERCOUNT );
  protected:
	int qid;
	int send(char  * buf ,int len  );
  private:

}; /* -----  end of class  Csync_log  ----- */

#endif   /* ----- #ifndef CSYNC_LOG_INC  ----- */

