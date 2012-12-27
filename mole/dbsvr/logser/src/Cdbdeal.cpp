/*
 * =====================================================================================
 *
 *       Filename:  dbdeal.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月02日 16时39分06秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include "Cdbdeal.h"
#include "common.h"
#include "benchapi.h"
#include "logproto.h"
//static char logtime[20];

DEALFUN g_dealfun[]={
		{LOG_LOGIN_CMD, &Cdbdeal::login_cmd},
		{LOG_USER_REG_CMD, &Cdbdeal::user_reg_cmd},
		{LOG_ONLINE_USERCOUNT_CMD, &Cdbdeal::online_usercount_cmd},
		{LOG_XIAOMEE_USE_CMD, &Cdbdeal::xiaomee_use_cmd},
		{LOG_DAY_COUNT_CMD, &Cdbdeal::day_count_cmd},
		{LOG_USER_ON_OFF_LINE_CMD, &Cdbdeal::user_on_off_line}
};

const int DEALFUN_COUNT = (int(sizeof(g_dealfun )/sizeof(g_dealfun[0])));
/*
 *-------------------------------------------------------------------------------------- *       Class:  Cdbdeal
 *      Method:  Cdbdeal
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Cdbdeal::Cdbdeal (mysql_interface * db) 
	:login(db),petgrade (db),server(db),userinfo(db) ,petinfo(db),
	online_usercount(db), xiaomee_use(db),day_count(db),day_count_ex(db),user_opt(db)
{
	this->initlist( g_dealfun ,DEALFUN_COUNT );
}  /* -----  end of method Cdbdeal::Cdbdeal  (constructor)  ----- */

int Cdbdeal::deal(char *recvbuf)
{
	P_DEALFUN_T * p_pri_stru;
	short cmdid=((LOG_PROTO_HEADER *)recvbuf)->cmd_id;
			
	if((p_pri_stru =this->cmdmaplist.getitem(cmdid))!=NULL){
		DEBUG_LOG(" --------BEGIN--trans come comid[%X][%d]--BEGIN---------------\n",
			cmdid,cmdid);
		return (this->*(*p_pri_stru))(recvbuf );	
	}else{
		DEBUG_LOG("cmd no define  comid[%X]\n",cmdid );
		return  CMDID_NODEFINE_ERR;
	}
}

int Cdbdeal::login_cmd(char *recvbuf )
{

	TLOGIN* p_in= (TLOGIN*)(recvbuf+LOG_PROTO_HEADER_SIZE);
	this->login.insert(this->logtime,p_in );
	this->user_opt.insert(p_in->logtime,p_in->userid,1,p_in->addr_type,p_in->ip);
	return SUCC;
}

int Cdbdeal::user_reg_cmd(char *recvbuf )
{
	TUSER_REG* p_in= (TUSER_REG*)(recvbuf+LOG_PROTO_HEADER_SIZE);
	uint32_t date=get_date(p_in->logtime);
	this->petgrade.insert(this->logtime);
	this->day_count_ex.add_or_insert(0,date,
			get_minutes_count(p_in->logtime),0,1);
	this->day_count_ex.add_or_insert(3,date, 
			p_in->reg_addr_type, 0,1);
	this->user_opt.insert(p_in->logtime,p_in->userid,0,p_in->reg_addr_type,p_in->ip);
	return SUCC;
}

int Cdbdeal::user_on_off_line(char *recvbuf )
{
	TUSER_ON_OFF_LINE* p_in= (TUSER_ON_OFF_LINE*)(recvbuf+LOG_PROTO_HEADER_SIZE);
	this->user_opt.insert(p_in->logtime,p_in->userid,p_in->flag ,0,0);
	return SUCC;
}

int Cdbdeal::checkserverloged(char *ip,bool * isloged  )
{
	return this->server.check(this->logtime, ip,isloged);
}

int Cdbdeal::insertserver(char *ip, uint32_t logcount)
{
	return this->server.insert(this->logtime, ip,logcount);
}

int Cdbdeal::daydeal_userinfo( char * startdate , char *enddate ,int8_t usertype  )
{
	TUSERINFO t;
	memset(&t,0,sizeof(TUSERINFO));
	strncpy(t.logtime,startdate,sizeof(t.logtime));
	t.usertype=usertype ;

	/*
	if (this->userinfo.get(t.logtime,t.usertype,&t)==SUCC ){
				
	}else{

	}
	*/

	if (this->login.getstatisticsdata(startdate,enddate, t.usertype, &t)!=SUCC){
		return FAIL;
	}
	if (this->petgrade.get_register_count_with_enddate(enddate,t.usertype,&(t.usercount))!=SUCC){
		return FAIL;
	}
	if (this->petgrade.get_register_addcount(startdate,enddate,t.usertype,&(t.addusercount))!=SUCC){
		return FAIL;
	}

	this->userinfo.remove(t.logtime,t.usertype);				
	this->userinfo.insert(&t);				
	return SUCC;
}

int Cdbdeal::daydeal_petinfo( char * startdate , char *enddate )
{
	TPETINFO t;
	if (this->petgrade.getpetinfor(startdate,enddate,&t)==SUCC)	{	
		this->petinfo.remove(t.logtime);				
		this->petinfo.insert(&t);				
	}else return FAIL;
	return SUCC;
}

int Cdbdeal::online_usercount_cmd(char *recvbuf )
{
	this->online_usercount.insert((TONLINE_USERCOUNT*)(recvbuf+LOG_PROTO_HEADER_SIZE) );
	return SUCC;
}
int Cdbdeal::online_usercount_all(char * date )
{
	this->online_usercount.ms_all(date);
	return SUCC;
}


int Cdbdeal::day_count_cmd(char *recvbuf )
{
	TDAY_COUNT * p_in= (TDAY_COUNT*) (recvbuf+LOG_PROTO_HEADER_SIZE) ;
	int ret=this-> day_count.insert(p_in->type, p_in->date,p_in->count);
	if (ret !=SUCC){
		this-> day_count.update(p_in->type, p_in->date,p_in->count);
	}
	return SUCC;
}


int Cdbdeal::xiaomee_use_cmd(char *recvbuf )
{
	TXIAOMEE_USE *t=(TXIAOMEE_USE*)(recvbuf+LOG_PROTO_HEADER_SIZE);
	struct tm tm_tmp; 
	uint32_t logdate,hour ;
	logdate=get_date(t->logtime);
	if (t->reason==XIAOMEE_USE_DEL_BUY_ATTIRE || t->reason==XIAOMEE_USE_DEL_BY_PET )	{
		localtime_r((time_t*)&(t->logtime), &tm_tmp);
		hour=tm_tmp.tm_hour;
		this->xiaomee_use.add_xiaomee(logdate, hour, t->usecount );
	}else{

	}

	this->day_count_ex.add_or_insert(2,logdate,t->reason,t->reason_ex, t->usecount);
	return SUCC;
}

