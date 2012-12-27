#ifndef  GF_CGF_DBPROXY_H
#define    GF_CGF_DBPROXY_H
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include "CNetComm.h"

extern CNetComm*     net;

class Cgf_dbproxy:public CtableRoute{
	public:
		Cgf_dbproxy(mysql_interface * db ); 

		//int set_role_delflg(userid_t userid,uint32_t role_regtime);
		
		int del_role(userid_t userid,uint32_t role_regtime);
	private:
		char m_recvbuf[PROTO_MAX_SIZE];
};
#endif   /* ----- #ifndef CDD_ATTIRE_INCL  ----- */

