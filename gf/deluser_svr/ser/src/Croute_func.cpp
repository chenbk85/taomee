/*
 * =====================================================================================
 *
 *
 *	   Filename:  route_func.cpp
 *
 *	Description:
 *
 *		Version:  1.0
 *		Created:  2009 
 *	   Revision:  none
 *	   Compiler:  gcc
 *
 *		 Author:  jim(jim), jim@taomee.com
 *		Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <string.h>
#include "Croute_func.h"
#include "common.h"
#include "benchapi.h"
#include "proto.h"
#include "logproto.h"
#include "tcpip.h"
#include <limits.h>
#include "Cgf_dbproxy.h"

extern  char * version_str;
extern uint32_t gf_max_role_deleted;
CMD_MAP g_cmdmap[]={
	//--------------------------
	//FILL_CMD_WITHOUT_PRI_IN	(gf_set_role_delflg),
	
	FILL_CMD_WITHOUT_PRI_IN	(gf_add_del_role),

	FILL_CMD_WITH_PRI_IN (gf_del_allrole_overload)
};

#define DEALFUN_COUNT  (int(sizeof(g_cmdmap )/sizeof(g_cmdmap[0])))


/*
 *--------------------------------------------------------------------------------------
 *	   Class:  Croute_func
 *	  Method:  Croute_func
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */


Croute_func::Croute_func (mysql_interface * db):gf_role_deleted(db),gf_dbproxy(db)
{
	this->db=db;
	this->initlist(g_cmdmap,DEALFUN_COUNT );	
	//this->cmdmaplist.show_item();

}  /* -----  end of method Croute_func::Croute_func  (constructor)  ----- */


int Croute_func::deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
{
	PRI_STRU * p_pri_stru;
	uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
	if((p_pri_stru =this->cmdmaplist.getitem(cmdid))!=NULL)
	{
		DEBUG_LOG("I: CMD:%04X  RECV_USER_ID:%d", cmdid, ((PROTO_HEADER*)recvbuf)->id);

		//检查报文长度
		if (! p_pri_stru->check_proto_size(rcvlen - PROTO_HEADER_SIZE) ){
			DEBUG_LOG("len err pre [%d] send [%d]",
			p_pri_stru->predefine_len+PROTO_HEADER_SIZE ,rcvlen );
			return PROTO_LEN_ERR;
		}

		//调用相关DB处理函数
		this->ret=9999;
		this->ret=(this->*(p_pri_stru->p_deal_fun))(recvbuf, sendbuf, sndlen );	

		//提交数据
		if (mysql_commit(&(this->db->handle))!=DB_SUCC){
			this->db->show_error_log("COMMIT:");
		}
		
		return this->ret;
	}
	else
	{
		DEBUG_LOG("cmd no define  comid[%04X]",cmdid );
		return  CMDID_NODEFINE_ERR;
	}
}



int Croute_func::gf_add_del_role(DEAL_FUN_ARG)
{
	DEBUG_LOG("gf_add_del role");
	ret = this->gf_role_deleted.add_role_deleted(RECVBUF_USERID, RECVBUF_ROLETM);
	if (ret!=SUCC)
	{
		DEBUG_LOG("ret1!=SUCC [%d]",ret);
		return ret;
	}	
	STD_RETURN(ret);
}


int Croute_func::gf_del_allrole_overload(DEAL_FUN_ARG)
{
	gf_del_allrole_overload_in* p_in = PRI_IN_POS;
	gf_role_deleted_stru* p_list = NULL;
	uint32_t del_num = 0;
	char dbname[128] = "";
	uint32_t dbid = 0;
	uint32_t tableid = 0;
	uint32_t all_num = 0;
	uint32_t time_line = p_in->time>gf_max_role_deleted?p_in->time-gf_max_role_deleted:0;
	DEBUG_LOG("gf_del_allrole_overload");

	//for each db
	for (dbid=0;dbid<1;dbid++)
	{
		//for each table
		for (tableid=0;tableid<10;tableid++)
		{
			sprintf(dbname,"DELUSER.t_gf_role_deleted_%01d",tableid);
			ret = this->gf_role_deleted.get_list_all_role_deleted(dbname,time_line,&del_num,&p_list);
			if (ret!=SUCC)
			{
				continue;
			}
			for (uint32_t iter=0;iter<del_num;iter++)
			{
				ret = this->gf_role_deleted.del_role_deleted(p_list[iter].userid, p_list[iter].role_regtime);
				DEBUG_LOG("delete  local role:[%u]	[%u]",p_list[iter].userid,p_list[iter].role_regtime);
				if (ret==SUCC|| ret==ROLE_ID_NOFIND_ERR )
				{
					ret = this->gf_dbproxy.del_role(p_list[iter].userid, p_list[iter].role_regtime);
					DEBUG_LOG("delete remote role:[%u]	[%u]",p_list[iter].userid,p_list[iter].role_regtime);

					if (ret==SUCC || ret==ROLE_ID_NOFIND_ERR )
					{
						STD_COMMIT(ret);
						continue;	
					}
					else
					{
						STD_ROLLBACK();
					}
				}
			}
			if (p_list)
			{
				free(p_list);
				p_list=NULL;
			}		
			all_num += del_num;
		}
	}

	DEBUG_LOG("gf_del_allrole_overload:del_num[%u]",all_num);

	STD_RETURN(SUCC);

}	

/*
int Croute_func::gf_del_allrole_overload(DEAL_FUN_ARG)
{
	gf_del_allrole_overload_in* p_in = PRI_IN_POS;
	gf_role_deleted_stru* p_list = NULL;
	uint32_t del_num = 0;
	char dbname[128] = "";
	uint32_t dbid = 0;
	uint32_t tableid = 0;
	uint32_t all_num = 0;
	uint32_t time_line = p_in->time>gf_max_role_deleted?p_in->time-gf_max_role_deleted:0;
	DEBUG_LOG("gf_del_allrole_overload");

	//for each db
	for (dbid=0;dbid<1;dbid++)
	{
		//for each table
		for (tableid=0;tableid<10;tableid++)
		{
			sprintf(dbname,"DELUSER.t_gf_role_deleted_%01d",tableid);
			ret = this->gf_role_deleted.get_list_all_role_deleted(dbname,time_line,&del_num,&p_list);
			if (ret!=SUCC)
			{
				continue;
			}
			for (uint32_t iter=0;iter<del_num;iter++)
			{
				ret = this->gf_role_deleted.del_role_deleted(p_list[iter].userid, p_list[iter].role_regtime);
				DEBUG_LOG("delete  local role:[%u]	[%u]",p_list[iter].userid,p_list[iter].role_regtime);
				if (ret==SUCC|| ret==ROLE_ID_NOFIND_ERR )
				{
					ret = this->gf_dbproxy.del_role(p_list[iter].userid, p_list[iter].role_regtime);
					DEBUG_LOG("delete remote role:[%u]	[%u]",p_list[iter].userid,p_list[iter].role_regtime);

					if (ret==SUCC || ret==ROLE_ID_NOFIND_ERR )
					{
						STD_COMMIT(ret);
						continue;	
					}
					else
					{
						STD_ROLLBACK();
					}
				}
			}
			if (p_list)
			{
				free(p_list);
				p_list=NULL;
			}		
			all_num += del_num;
		}
	}

	DEBUG_LOG("gf_del_allrole_overload:del_num[%u]",all_num);

	STD_RETURN(SUCC);
}	

int Croute_func::gf_del_allrole_overload(DEAL_FUN_ARG)
{
	gf_role_deleted_stru* p_list = NULL;
	uint32_t get_num = 0;
	uint32_t del_num = 0;
	DEBUG_LOG("gf_del_role");
	ret = this->gf_role_deleted.get_list_role_deleted(RECVBUF_USERID,&get_num,&p_list);
	if (ret!=SUCC)
	{
		return ret;
	}

	del_num = get_num>GF_ROLE_DEL_NUM_MAX?GF_ROLE_DEL_NUM_MAX:get_num;
	for (uint32_t iter=0;iter<del_num;iter++)
	{
		
		ret = this->gf_role_deleted.del_role_deleted(RECVBUF_USERID, p_list[iter].role_regtime);
		DEBUG_LOG("delete  local role:[%u]	[%u]",RECVBUF_USERID,p_list[iter].role_regtime);
		if (ret==SUCC || ret==ROLE_ID_NOFIND_ERR)
		{
			ret = this->gf_dbproxy.del_role(RECVBUF_USERID, p_list[iter].role_regtime);
			DEBUG_LOG("delete remote role:[%u]  [%u]",RECVBUF_USERID,p_list[iter].role_regtime);
			if (ret==SUCC || ret==ROLE_ID_NOFIND_ERR)
			{
				STD_COMMIT(ret);
				continue;
			}
			else
			{
				STD_ROLLBACK();
			}
		}
	}
	if (p_list)
	{
		free(p_list);
	}
	DEBUG_LOG("gf_del_role:del_num[%u]",del_num);
	STD_RETURN(SUCC);
}	


int Croute_func::gf_set_role_delflg(DEAL_FUN_ARG)
{
	DEBUG_LOG("gf_set_role_delflg");
	ret = this->gf_role_deleted.add_role_deleted(RECVBUF_USERID, RECVBUF_ROLETM);
	if (ret!=SUCC)
	{
		DEBUG_LOG("ret1!=SUCC [%d]",ret);
		return ret;
	}	
	ret = this->gf_dbproxy.set_role_delflg(RECVBUF_USERID, RECVBUF_ROLETM);
	if (ret!=SUCC)
	{
		DEBUG_LOG("ret2!=SUCC [%d]",ret);
		STD_ROLLBACK();
		return ret;
	}
	
	STD_RETURN(ret);
}


*/


