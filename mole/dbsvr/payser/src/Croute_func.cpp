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
#include "Croute_func.h"



CMD_MAP g_cmdmap[]={
	FILL_CMD_WITH_PRI_IN	(PAY_BUY_ATTIRE_BY_DAMEE ),
	FILL_CMD_WITH_PRI_IN	(PAY_MONTH_BY_DAMEE),
	FILL_CMD_WITH_PRI_IN	(PAY_ADD_DAMEE_BY_SERIAL ),
	FILL_CMD_WITHOUT_PRI_IN	(PAY_INIT_USER),
	FILL_CMD_WITH_PRI_IN	(PAY_CHANGE_DAMEE),
	FILL_CMD_WITH_PRI_IN	(PAY_SET_AUTO_MONTH_DEALMSG),
	FILL_CMD_WITHOUT_PRI_IN	(PAY_DEL_MONTH),
	FILL_CMD_WITHOUT_PRI_IN	(PAY_GET_PAY_INFO),
	FILL_CMD_WITHOUT_PRI_IN	(PAY_SET_NO_AUTO_MONTH),
	FILL_CMD_WITHOUT_PRI_IN	(PAY_AUTO_MONTH_RECORD_GEN),
	FILL_CMD_WITH_PRI_IN	(PAY_PAY_PRE),
	FILL_CMD_WITH_PRI_IN	(PAY_PAY_TRUE),
	FILL_CMD_WITH_PRI_IN	(PAY_PAY_ONCE),
	FILL_CMD_WITH_PRI_IN	(PAY_GET_DAMEE_LIST)
};
#define DEALFUN_COUNT  (int(sizeof(g_cmdmap )/sizeof(g_cmdmap[0])))

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Croute_func
 *      Method:  Croute_func
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
Croute_func::Croute_func (int attire_shm_key,int sysc_user_msg_key,
		mysql_interface * db,char * serial_ip, ushort serial_port  )
	:db(db),attire_conf(attire_shm_key),increment (db),  sync_user(sysc_user_msg_key)
	,serial_cp( serial_ip, serial_port),
	pay_history(db),damee_history(db)
{
	this->cfg.damee=config_get_intval("INIT_DAMEE",0);	
	this->cfg.month_damee=config_get_intval("INIT_MONTH_DAMEE",9900);	
	this->cfg.lastmonth_date=0;
	this->initlist(g_cmdmap,DEALFUN_COUNT );	
}  /* -----  end of method Croute_func::Croute_func  (constructor)  ----- */

int Croute_func::deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
{
	PRI_STRU * p_pri_stru;
	short cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;

	if((p_pri_stru =this->cmdmaplist.getitem(cmdid))!=NULL){
 	DEBUG_LOG("XXXXXX--BEGIN--trans come comid[%X][%s][%d]--BEGIN------------",
		             cmdid,p_pri_stru->desc_msg,((PROTO_HEADER*)recvbuf)->id);
		//检查报文长度
		if (! p_pri_stru->check_proto_size(rcvlen-PROTO_HEADER_SIZE ) ){
			DEBUG_LOG("PROTO_LEN_ERR need[%d] get[%d]",p_pri_stru->predefine_len ,
					rcvlen-PROTO_HEADER_SIZE  );
			return PROTO_LEN_ERR;
		}
		
		//调用相关
		return (this->*(p_pri_stru->p_deal_fun))(recvbuf, sendbuf, sndlen );	
		
	}else{
		DEBUG_LOG("cmd no define  comid[%X]",cmdid );
		return  CMDID_NODEFINE_ERR;
	}
}

int Croute_func::f_PAY_DEL_MONTH(DEAL_FUN_ARG)
{
	TUSER_FLAG uf;
		
	int ret;
	if ((ret= this->increment.set_del_month(USERID_FROM_RECVBUF) )!=SUCC){
		return ret;
	}	
	//同步身份位	
	uf.msg_flag=1;
	uf.sendcount=0;
	uf.userid=USERID_FROM_RECVBUF; 
	uf.flag=0;
	if (this->sync_user.send((char*) &uf, sizeof(uf) )!=SUCC){
		DEBUG_LOG("__ERROR__ SYNC USER FLAG [%u][%u]",uf.userid,uf.flag );
	}
	STD_RETURN( ret);
}
int Croute_func::f_PAY_MONTH_BY_DAMEE(DEAL_FUN_ARG)
{
	TUSER_FLAG uf;
		
	int ret;
	int del_damee; //扣款
	uint32_t leave_damee;
	int month_count;
	char pri_msg[20];
	PAY_MONTH_BY_DAMEE_IN *p_in=PRI_IN_POS;
	month_count=p_in->monthcount;
	del_damee=p_in->usedamee;


	//check userid existed
	bool user_isexisted=false;
	ret=this->increment.id_is_existed(USERID_FROM_RECVBUF,&user_isexisted);
	if (ret==SUCC &&  user_isexisted!=true ) return USER_ID_NOFIND_ERR ;


	strcpy(pri_msg,"包月" );
	//扣除damee
	if((ret=this->increment.set_damee(
				USERID_FROM_RECVBUF,-del_damee, 
			&leave_damee,PAY_TYPE_MONTH_DAMEE,pri_msg ))!=SUCC){
		return ret; 
	}

	ret=this->add_monthcount(USERID_FROM_RECVBUF,PAY_TYPE_MONTH_DAMEE,
		 del_damee,	month_count,p_in->automonthed,true  );
	if (ret==SUCC){//成功
		//处理:插入扣款记录
			//同步身份位	
		uf.msg_flag=1;
		uf.sendcount=0;
		uf.userid=USERID_FROM_RECVBUF; 
		uf.flag=1;
		if (this->sync_user.send((char*) &uf, sizeof(uf) )!=SUCC){
			DEBUG_LOG("__ERROR__ SYNC USER FLAG [%u][%u]",uf.userid,uf.flag );
		}
	}else{//出错，撤消扣除 damee 
		int tmpret;
		strcpy(pri_msg,"包月出错,返还" );
		if((tmpret=this->increment.set_damee(
				USERID_FROM_RECVBUF,del_damee,&leave_damee,
			   PAY_TYPE_MONTH_DAMEE	,pri_msg ))!=SUCC){
			DEBUG_LOG("__ERROR__ ADD DAMEE [%u][%u]error[%d]",
					USERID_FROM_RECVBUF,del_damee,tmpret  );
		}
	}
	STD_RETURN( ret);
}

int  Croute_func::f_PAY_BUY_ATTIRE_BY_DAMEE (DEAL_FUN_ARG)
{
	PAY_BUY_ATTIRE_BY_DAMEE_IN *p_in= PRI_IN_POS;
	PAY_BUY_ATTIRE_BY_DAMEE_OUT out;
	int ret;
	uint32_t price, price_total;
	if ((ret =this->attire_conf.get_price(p_in->attireid,&price))!=SUCC){
		return ret;
	}
	char pri_msg[30];
	sprintf(pri_msg,"购买:%d|%d",p_in->attiretype,p_in->count);
	price_total=price*p_in->count;
	ret=this->increment.set_damee( USERID_FROM_RECVBUF,
		-price_total,&(out.leave_damee),PAY_TYPE_BUY_ATTIRE,pri_msg );
	if (ret==SUCC){
		TATTIRE_ADD	t;
		t.msg_flag=2;
		t.sendcount=0;
		t.userid=USERID_FROM_RECVBUF;
		t.attiretype=p_in->attiretype;
		t.attireid=p_in->attireid;
		t.addcount=p_in->count;
		if (this->sync_user.send((char*) &t, sizeof(t) )!=SUCC){
			DEBUG_LOG("__ERROR__ SYNC USER ATTIRE [%u][%u][%u]",t.userid,t.attireid,t.addcount);
		}
	}
	STD_RETURN_WITH_STRUCT( ret,out );
}

int Croute_func::f_PAY_INIT_USER(DEAL_FUN_ARG)
{
	int ret;
	ret=this->increment.init_user(USERID_FROM_RECVBUF, this->cfg.damee);
	STD_RETURN(ret);
}

int Croute_func::f_PAY_ADD_DAMEE_BY_SERIAL (DEAL_FUN_ARG)
{
	PAY_ADD_DAMEE_BY_SERIAL_IN *p_in=PRI_IN_POS;
	PAY_ADD_DAMEE_BY_SERIAL_OUT out;
	uint64_t serialid;
	int ret;
	p_in->serialid[sizeof(p_in->serialid)-1]=0;
	serialid=atoll(p_in->serialid );

	if ((ret =this->serial_cp.serial_set_serial_used_cmd(serialid,
					time(NULL),&(out.price)))!=SUCC){
		return ret;
	}

	INFO_LOG("SERIAL:DEAL:userid[%u]serialid[%llu]price[%u]", 
			USERID_FROM_RECVBUF,serialid,out.price);		

	ret=this->increment.set_damee( USERID_FROM_RECVBUF,
			out.price ,&(out.leave_damee),PAY_TYPE_DAMEE_SERIAL,p_in->serialid);

	if (ret==DB_SUCC){
		INFO_LOG("SERIAL:SUCC:[%u][%llu]",
				USERID_FROM_RECVBUF,serialid );		
	}else{ 
		//将serial设置成未使用
		int noused_ret;
		INFO_LOG("SERIAL:FAIL:[%u][%llu]", USERID_FROM_RECVBUF,serialid );		
		if (( noused_ret=this->serial_cp.serial_set_serial_unused_cmd(
						serialid))!=SUCC){
			INFO_LOG("__ERROR__ SERIAL:UNUSED SERIAL:[%d][%u][%llu]",
					noused_ret,USERID_FROM_RECVBUF,serialid);		
		}
	}
	STD_RETURN_WITH_STRUCT(ret,out );
}


int Croute_func::f_PAY_CHANGE_DAMEE(DEAL_FUN_ARG)
{
	PAY_CHANGE_DAMEE_IN *p_in=PRI_IN_POS;
	PAY_CHANGE_DAMEE_OUT out;
	char pri_msg[20]="管理员,修正";
	int ret=this->increment.set_damee( USERID_FROM_RECVBUF,
		p_in->adddamee,&(out.leavedamee),PAY_TYPE_SYS_CHANGE,pri_msg  );
	STD_RETURN_WITH_STRUCT(ret,out );
}

int Croute_func::f_PAY_AUTO_MONTH_RECORD_GEN(DEAL_FUN_ARG)
{
	char  sqlstr[2048];
	int32_t gendate;
	gendate=get_year_month(time(NULL));
	if( this->cfg.lastmonth_date < gendate ){
		sprintf (sqlstr,"insert into  INCREMENT_DB.t_pay_history  \
		select  0,0,%u , userid, month_paytype,%u,1,'auto next',%u,0 from  INCREMENT_DB.t_increment \
		where  month_used=1 and  month_duetime<=%d and month_nexterrcount<%u ",
		get_date(time(NULL)) ,this->cfg.month_damee, PAY_ERR_NO_DEAL,gendate,NOVIP_NEXTERRCOUNT
		);
		int acount; 
		if (this->db->exec_update_sql(sqlstr,&acount )==DB_SUCC){ 
			this->cfg.lastmonth_date= gendate;	
			STD_RETURN(SUCC);
		}else {
			STD_RETURN( DB_ERR);
		}
	}else{
		STD_RETURN( CMDID_NODEFINE_ERR );
	}
}

int Croute_func::f_PAY_SET_AUTO_MONTH_DEALMSG (DEAL_FUN_ARG)
{
	PAY_SET_AUTO_MONTH_DEALMSG_IN *p_in= PRI_IN_POS;
	int ret=this->pay_history.set_dealcode( p_in->transid,
		p_in->dealflag);
	STD_RETURN(ret);
}

int Croute_func::f_PAY_SET_NO_AUTO_MONTH(DEAL_FUN_ARG)
{
	int ret=this->increment.set_no_auto_month(USERID_FROM_RECVBUF);
	STD_RETURN(ret);
}

int Croute_func::f_PAY_GET_PAY_INFO(DEAL_FUN_ARG)
{
	PAY_GET_PAY_INFO_OUT out; 	
	int ret=this->increment.get_record(USERID_FROM_RECVBUF,&out);
	STD_RETURN_WITH_STRUCT(ret,out );
}

int Croute_func::f_PAY_PAY_ONCE(DEAL_FUN_ARG)
{
	PAY_PAY_ONCE_IN*p_in=PRI_IN_POS; 
	PAY_PAY_ONCE_OUT out;
	int ret;
	//check userid existed
	if( p_in->out_transid!=0){ 
		//检查是否重复交易 
		if (this-> pay_history.isrecorded( p_in->out_transid,p_in->paytype)==SUCC){
			DEBUG_LOG("trans is dealed: out_transid %u ", p_in->out_transid );
			return TRANS_ID_EXISTED_ERR;			
		}
	}
	ret=this->deal_pay(p_in);
	if (ret==SUCC) 
		ret=this-> pay_history.gen_trans(p_in,PAY_SUCC, &(out.transid));		

	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::deal_pay( PAY_STRU *p_pay)
{
	int ret;
	uint32_t leave_damee;
	switch(p_pay->paytype/1000){
		case 1 :{// 充值
			ret=this->increment.set_damee(p_pay-> userid,p_pay->damee,
					&leave_damee,p_pay->paytype, p_pay->private_msg);
			if (ret!=SUCC){
					DEBUG_LOG("__ERROR__ ADDDAMEE userid nofind paytype[%d] userid[%u] damee[%u],ret[%d]", 
							p_pay->paytype,p_pay->userid,p_pay->damee,ret);
			}
			break;
		}
		case 2 :{ //包月
			 ret=this->add_monthcount(p_pay->userid, p_pay-> paytype,
				p_pay->damee ,p_pay-> months, 0, false);
			if (ret!=SUCC){
					DEBUG_LOG("__ERROR__ MONTH userid nofind paytype[%d] userid[%u] damee[%u],ret[%d]", 
						p_pay->paytype,p_pay->userid,p_pay->damee,ret);
			}
			break;
		}
		default :
			ret=ENUM_OUT_OF_RANGE_ERR;
			break;
	}
	return ret;
}

int Croute_func::f_PAY_PAY_PRE(DEAL_FUN_ARG)
{
	PAY_PAY_PRE_IN*p_in= PRI_IN_POS;
	PAY_PAY_PRE_OUT out;
	int ret;
	//check userid existed
	bool user_isexisted=false;
	ret=this->increment.id_is_existed(p_in->userid,&user_isexisted);
	if (  ret!=SUCC ) return ret;
	else if ( user_isexisted!=true ) return USER_ID_NOFIND_ERR ;

	ret=this-> pay_history .gen_trans(p_in,PAY_ERR_NO_DEAL, &(out.transid));		
	STD_RETURN_WITH_STRUCT(ret,out);
}

int Croute_func::f_PAY_PAY_TRUE(DEAL_FUN_ARG)
{
	PAY_STRU pay;
	PAY_PAY_TRUE_IN *p_in= PRI_IN_POS ;
	int ret=this-> pay_history.set_trans_dealcode(p_in->transid, 
		p_in->damee, PAY_SUCC ,&pay  );		
	if (ret==SUCC){
		pay.damee= p_in->damee;
		pay.date= get_date (time(NULL));
		ret=this-> deal_pay( &pay);
	}else{
		DEBUG_LOG("__ERROR__ month pay check err transid[%u] , damee [%u], ret[%d] ",
					 p_in->transid , p_in->damee ,ret  );
	}
	STD_RETURN(ret);
}



int Croute_func::add_monthcount(userid_t userid,
	 uint32_t paytype,uint32_t damee ,uint32_t month_count,uint32_t automonthed, bool isRecard )
{

	uint32_t transid;
	uint32_t  nexterrcount;
	int ret;
	uint32_t old_duetime,duetime, enabletime;

	//如果自动续期为0, 则设置不再续期：即：把续期错误次数设置为最大值。
	if (automonthed==0 ) nexterrcount=NOVIP_NEXTERRCOUNT;
	else nexterrcount=0; 
	
	//检查是否已经处于包月状态
	if (this->increment.isMonthed(userid, &old_duetime)==SUCC){
		//已经包月
		duetime=change_date (old_duetime, month_count) ;//包月份数
		//更新包月到期日期
		ret=this->increment.update_duetime(userid , duetime,nexterrcount);	
	}else{
		//未包月
		enabletime=get_year_month(time(NULL)) ;
		duetime=change_date (enabletime ,  month_count+1) ;//包月份数
		//插入包月 increment 
		ret=this->increment.set_month(userid,paytype,
          1,enabletime,duetime, nexterrcount);	
	}

	if (isRecard){
		int tmpret;
		PAY_STRU month;
		month.date= get_date (time(NULL));
		month.damee=damee;
		month.userid=userid;
		month.private_msg[0]='\0';
		month.out_transid=0;
		month.paytype=paytype;
		month.months=month_count;
		if((tmpret=this-> pay_history. gen_true_trans ( 
						&month, &transid))!=SUCC){
				DEBUG_LOG("__ERROR__ INSERT t_pay_history: userid[%u] damee[%u]error[%d]",
						 userid, damee ,tmpret  );
		}
	}
	return ret;
}
int Croute_func::f_PAY_GET_DAMEE_LIST(DEAL_FUN_ARG)
{
	PAY_GET_DAMEE_LIST_IN*p_in= PRI_IN_POS;
	PAY_GET_DAMEE_LIST_OUT out;
	int ret;
	if ( p_in->count>100 ) return  VALUE_OUT_OF_RANGE_ERR;
	ret=this-> damee_history.get_record(USERID_FROM_RECVBUF,
		 p_in->startdate, p_in->enddate, p_in->startindex, p_in->count,&out );		
	STD_RETURN_WITH_BUF(ret , &out,
			sizeof(out)-sizeof(out.damee_item)   //header len
			+sizeof(out.damee_item[0])*out.count);//item len
}
