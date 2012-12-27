#include "Cgf_dbproxy.h"
#include <algorithm>

using namespace std;
//¼ÓÈë
Cgf_dbproxy::Cgf_dbproxy(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_item","userid")
{ 

}

int Cgf_dbproxy::del_role(userid_t userid,uint32_t role_regtime)
{
	PROTO_HEADER proto_header =  {0};
	PROTO_HEADER* p_proto = (PROTO_HEADER*)m_recvbuf;
	int ret;
	
	int sendsize = set_proto_buf((char*)&proto_header,gf_del_role_cmd,userid,role_regtime,NULL,0);
	
	ret = net->net_safe_io((const char*)&proto_header,sendsize,m_recvbuf);
	if (ret==-1)
	{
		return NET_ERR;
	}
	else
	{
		return (int)(p_proto->result);
	}
	return SUCC;

}

/*
int Cgf_dbproxy::set_role_delflg(userid_t userid,uint32_t role_regtime)
{
	PROTO_HEADER proto_header =  {0};
	PROTO_HEADER* p_proto = (PROTO_HEADER*)m_recvbuf;
	int ret;
	
	int sendsize = set_proto_buf((char*)&proto_header,gf_set_role_delflg_cmd,userid,role_regtime,NULL,0);
	
	ret = net->net_safe_io((const char*)&proto_header,sendsize,m_recvbuf);
	if (ret<=0)
	{
		return NET_ERR;
	}
	else
	{
		return (int)(p_proto->result);
	}
	return SUCC;
}

*/

