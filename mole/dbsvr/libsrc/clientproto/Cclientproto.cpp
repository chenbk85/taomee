/*
 * =====================================================================================
 *
 *       Filename:  Cclientproto.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年01月10日 18时22分43秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:  set tabstop=4 
 * 		
 * =====================================================================================
 */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Cclientproto
 *      Method:  Cclientproto
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
#include "Cclientproto.h"
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include "benchapi.h"
#include  "tcpip.h"
#include  "common.h"
#include "logproto.h"
#include <netinet/in.h>
#define  SEND_DATA_AND_CHECK_SUCC()\
	if((ret=this->net_send())!=SUCC){ return ret; }\
	result=((PROTO_HEADER * )recvbuf)->result;\
	if (result==SUCC )	{

#define  SEND_DATA_AND_CHECK_SUCC_EX(resultvalue )\
	if((ret=this->net_send())!=SUCC){ return ret; }\
	result=((PROTO_HEADER * )recvbuf)->result;\
	if (result==SUCC || result==resultvalue){



#define  RETURN()   \
	}\
	free ( recvbuf);\
	return result;


Cclientproto::Cclientproto (const char * aipaddr, u_short aport)  /* constructor */
{
	this->set_ip_port(aipaddr,aport );
}  /* -----  end of method Cclientproto::Cclientproto  (constructor)  ----- */
void Cclientproto:: set_ip_port (const char * aipaddr, u_short aport)
{
	safe_copy_string(this->ipaddr,aipaddr );
	this->port=aport;
	this->sockfd=-1;
	//this->sockfd=open_socket (ipaddr, port);
}

void Cclientproto::set_pub_buf(char *buf,short cmd_id,int id,short private_size)
{
	PROTO_HEADER *ph=(PROTO_HEADER *) buf;
	ph->proto_length=PROTO_HEADER_SIZE+private_size;
//	DEBUG_LOG ("client send[%X][%u]",cmd_id,id);	
	ph->cmd_id= cmd_id;
	ph->proto_id=0x12345678;
	ph->id=id;
	ph->result=0;
}
/* 
int Cclientproto:: userinfo_login( uint32_t userid,char* passwd,uint32_t addr_type, uint32_t ip , 
	   	userinfo_login_out * p_out )
{
	userinfo_login_in  *in=PRI_SEND_IN_POS;
	int private_size=sizeof(userinfo_login_in);
	set_pub_buf(sendbuf,userinfo_login_cmd,userid, private_size);
	memcpy_with_dstsize(in->passwd,passwd);
	in->addr_type=addr_type;
	in->ip=ip;
		
	SEND_DATA_AND_CHECK_SUCC();
		uint32_t pri_msglen= ((PROTO_HEADER * )recvbuf)->proto_length-PROTO_HEADER_SIZE;

		if (pri_msglen<8 || pri_msglen  >sizeof(*p_out)) {
			return  PROTO_RETURN_LEN_ERR;   
		}
		memcpy(p_out,recvbuf+PROTO_HEADER_SIZE, pri_msglen);
		if (p_out->friendcount*sizeof(userid_t)!=pri_msglen-8 ) {
			return  PROTO_RETURN_LEN_ERR;   
		}
	RETURN();
} 
*/


int Cclientproto:: send_login_cmd( uint32_t userid,char* passwd,uint32_t addr_type, uint32_t ip , 
	   	mole_login_out * p_out )
{
	int ret;
	uint32_t gameflag;
	//检查密码
	ret=this->userinfo_login_ex(userid, passwd, ip, addr_type, &gameflag);	
	if (ret!=SUCC) return ret; 
	//得到VIP，好友列表
	ret=this->mole_login( userid, p_out );
	return ret;
	/*
	ret=this->mole_login(userid);
	if (ret!=SUCC) return ret;

	USERINFO_LOGIN_IN  *in=PRI_SEND_IN_POS;
	int private_size=sizeof(USERINFO_LOGIN_IN);
	set_pub_buf(sendbuf,USERINFO_LOGIN_CMD,userid, private_size);
	memcpy_with_dstsize(in->passwd,passwd);
	in->addr_type=addr_type;
	in->ip=ip;
		
	SEND_DATA_AND_CHECK_SUCC();
		uint32_t pri_msglen= ((PROTO_HEADER * )recvbuf)->proto_length-PROTO_HEADER_SIZE;


		if (pri_msglen<8 || pri_msglen  >sizeof(*p_out)) {
			return  PROTO_RETURN_LEN_ERR;   
		}
		memcpy(p_out,recvbuf+PROTO_HEADER_SIZE, pri_msglen);
		if (p_out->friendcount*sizeof(userid_t)!=pri_msglen-8 ) {
			return  PROTO_RETURN_LEN_ERR;   
		}
	RETURN();
	*/

}

int Cclientproto:: send_register_user_cmd( userid_t userid, user_register_in *pri )
{
	
	user_register_in * in=PRI_SEND_IN_POS;
	int private_size=sizeof(user_register_in);
	set_pub_buf(sendbuf,USER_REGISTER_CMD,userid, private_size);

	memcpy (in, pri, sizeof(user_register_in));

	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int Cclientproto:: mole_register( userid_t userid, uint32_t petcolor )
{
	user_register_ex_in * in=PRI_SEND_IN_POS;
	int private_size=sizeof(user_register_ex_in);
	set_pub_buf(sendbuf,USER_REGISTER_EX_CMD,userid, private_size);
	in->petcolor=petcolor;
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}



int Cclientproto:: send_map_email_userid_cmd(userid_t userid,char * email,
		userid_t *p_map_userid )
{
	map_email_userid_in *in=PRI_SEND_IN_POS;
	map_email_userid_out *p_out;
	int private_size=sizeof(map_email_userid_in);
	set_pub_buf(sendbuf,MAP_EMAIL_USERID_CMD, userid, private_size);
	memcpy_with_dstsize(in->email,email);
	
	SEND_DATA_AND_CHECK_SUCC();
		p_out=(map_email_userid_out *)(recvbuf+PROTO_HEADER_SIZE);
		*p_map_userid =p_out->old_map_userid;
		if (p_out->is_succ==0){//没有设置成功
			result=EMAIL_EXISTED_ERR;
		}
	RETURN();
}


int Cclientproto:: send_get_userid_by_email_cmd(char*email, uint32_t *userid)
{
	get_userid_by_email_in* in=PRI_SEND_IN_POS;
	get_userid_by_email_out *p_out;
	int private_size=sizeof(get_userid_by_email_in);
	set_pub_buf(sendbuf,GET_USERID_BY_EMAIL_CMD, 0, private_size);
	memcpy_with_dstsize(in->email,email);

	SEND_DATA_AND_CHECK_SUCC();
		p_out=(get_userid_by_email_out *)(recvbuf+PROTO_HEADER_SIZE);
		*userid=p_out->map_userid;
	RETURN();
}


int Cclientproto:: send_del_map_email_userid_cmd(uint32_t userid,char * email)
{
	del_map_email_userid_in *in=PRI_SEND_IN_POS;
	int private_size=sizeof(del_map_email_userid_in);
	set_pub_buf(sendbuf,DEL_MAP_EMAIL_USERID_CMD, userid, private_size);
	memcpy_with_dstsize(in->email,email);
	
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}
int Cclientproto:: send_login_by_email_cmd(char * email,char * passwd,
		uint32_t addr_type, uint32_t ip , 
	   	userid_t * userid, mole_login_out * p_out )

{
	int result;
	if ((result=send_get_userid_by_email_cmd(email, userid))!=SUCC){
		return result;
	}
	result=send_login_cmd(*userid, passwd, addr_type, ip,  p_out );
	return result;
}	

int Cclientproto::userinfo_register( userid_t userid, userinfo_register_ex_in *pri, 
	   	userid_t * maped_userid )
{
	int result;
	if ((result= send_map_email_userid_cmd(userid,pri-> email,maped_userid ))!=SUCC){
		* maped_userid=userid; 		
		return result;
	}
	result=this->f_userinfo_register_ex(userid,pri );
	if (result!=SUCC){
		this->send_del_map_email_userid_cmd( userid,pri->email); 
		return result ;
	}
	return result;
}

int Cclientproto:: send_register_cmd(userid_t userid, user_register_in *pri, 
	   	userid_t * maped_userid  )
{
	int result;

	if ((result= send_map_email_userid_cmd(userid,pri-> email,maped_userid ))!=SUCC){
		* maped_userid=userid; 		
		return result;
	}

	//注册基础库
	userinfo_register_ex_in	user_info; 
	memset(&user_info,0,sizeof(user_info) );
	memcpy (user_info.addr,pri->addr,sizeof(user_info.addr));
	memcpy (user_info.email,pri->email,sizeof(user_info.email));
	memcpy (user_info.mobile,pri->mobile,sizeof(user_info.mobile));
	memcpy (user_info.passwd,pri->passwd,sizeof(user_info.passwd));
	memcpy (user_info.signature,pri->signature,sizeof(user_info.signature));

	user_info.addr_city=pri->addr_city;
	user_info.addr_province=pri->addr_province;
	user_info.birthday=pri->birthday;
	user_info.ip=pri->ip;
	user_info.reg_addr_type=pri->reg_addr_type;
	user_info.sex=pri->sex;

	if ((result=this->f_userinfo_register_ex(userid,&user_info ))!=SUCC){
		this->send_del_map_email_userid_cmd( userid,pri->email); 
		return result ;
	}

	//注册摩尔库
	mole_register_in mole_in={ };
	mole_in.color=pri->petcolor;
	memcpy(mole_in.nick,pri->nick,NICK_LEN );
	if ((result=this->mole_register(userid,&mole_in ))!=SUCC){
		this->f_userinfo_del_user(userid);
		this->send_del_map_email_userid_cmd( userid,pri->email); 
	}

	//设置已注册了摩尔庄园
	if (result==SUCC ){
		result=this->userinfo_add_game(userid,1);
	}

	return result ;
}

int Cclientproto::mole_login( uint32_t userid)
{
	set_pub_buf(sendbuf,USER_LOGIN_CMD, userid , 0);
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}



int Cclientproto::user_del_user( uint32_t userid)
{
	set_pub_buf(sendbuf,USER_DEL_USER_CMD, userid , 0);
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int Cclientproto::f_userinfo_del_user( uint32_t userid)
{
	set_pub_buf(sendbuf,USERINFO_DEL_USER_CMD, userid , 0);
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int Cclientproto:: send_get_pet_cmd(int pet_id)
{
	int private_size=0;
	set_pub_buf(sendbuf,USER_GET_USER_CMD, pet_id,private_size);

	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}


int Cclientproto:: f_user_set_user_birthday_sex(userid_t userid,uint32_t birthday, uint32_t sex )
{
	user_set_user_birthday_sex_in *in=PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,USER_SET_USER_BIRTHDAY_SEX_CMD , userid, 
			sizeof( user_set_user_birthday_sex_in	));
	in->birthday=birthday;
	in->sex=sex;
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}


int Cclientproto:: send_get_user_cmd()
{
	memset(sendbuf,0,sizeof (sendbuf));
	set_pub_buf(sendbuf,USER_GET_USER_CMD, 390000, 0);

	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int  Cclientproto::user_add_attire(userid_t userid, uint32_t attiretype ,	
		uint32_t attireid, uint32_t count, uint32_t def_maxcount )
{
	user_add_attire_in *in=PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,USER_ADD_ATTIRE_CMD , userid, 
			sizeof(user_add_attire_in) );
	in->attiretype=attiretype;
	in->attireid =attireid;
	in->count=count;
	in->maxcount=def_maxcount;
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}
int  Cclientproto::user_del_attire(userid_t userid, uint32_t attiretype ,	
		uint32_t attireid, uint32_t count, uint32_t def_maxcount )
{
	user_del_attire_in *in=PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,USER_DEL_ATTIRE_CMD , userid, 
			sizeof(user_del_attire_in) );
	in->attiretype=attiretype;
	in->attireid =attireid;
	in->count=count;
	in->maxcount=def_maxcount;
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}




int Cclientproto:: f_user_change_user_value (userid_t userid, user_change_user_value_in *p_in)
{
	user_change_user_value_in *in=PRI_SEND_IN_POS;

	set_pub_buf(sendbuf,USER_CHANGE_USER_VALUE_CMD , userid, sizeof(*in ));
	memcpy (in,p_in,sizeof(*in) );
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int Cclientproto::user_get_home(uint32_t userid)
{
	set_pub_buf(sendbuf,USER_GET_HOME_CMD, userid, 0);
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int	Cclientproto::net_send_no_return()
{
	ret= net_io_no_return(sockfd, sendbuf, *((uint32_t*)sendbuf) );	
	//int net_errno=errno;
	if ( ret==NET_ERR )
	{ 	//reset socket and reconnect
		//always close it
		//DEBUG_LOG ("jim : close socket fd[%d] net_errno:[%d][%s]",net_errno,strerror(net_errno));	
		close(sockfd);
		sockfd=-1;
		//是连接中断，不是超时则重新发送
        sockfd = open_socket (ipaddr, port);
		//DEBUG_LOG ("jim :re connect [%s][%u]",ipaddr,port );	
    	if (sockfd == -1){
			//DEBUG_LOG ("jim :re connect FAIL");	
           return NET_ERR;		
		}
		//DEBUG_LOG ("jim :re connect SUCC");	
		ret=net_io_no_return(sockfd, sendbuf,*((uint32_t*)sendbuf));	
    	if(ret==NET_ERR){
			//DEBUG_LOG ("jim :re send data err [%d]",errno );	
			close(sockfd);
			sockfd=-1;

           return NET_ERR;		
		}
		return ret;
	}else{
		return ret;
	}
}




int	Cclientproto::net_send()
{
	ret= net_io(sockfd, sendbuf, *((uint32_t*)sendbuf),  &recvbuf, &rcvlen );	
	//int net_errno=errno;
	if ( ret==NET_ERR )
	{ 	//reset socket and reconnect
		//always close it
	//	DEBUG_LOG ("jim : close socket fd[%d] net_errno:[%d][%s]",net_errno,strerror(net_errno));	
		close(sockfd);
		//是连接中断，不是超时则重新发送
        sockfd = open_socket (ipaddr, port);
	//	DEBUG_LOG ("jim :re connect [%s][%u]",ipaddr,port );	
    	if (sockfd == -1){
	//		DEBUG_LOG ("jim :re connect FAIL");	
           return NET_ERR;		
		}
	//	DEBUG_LOG ("jim :re connect SUCC");	
		ret= net_io(sockfd, sendbuf,*((uint32_t*)sendbuf), &recvbuf, &rcvlen );	
    	if(ret==NET_ERR){
	//		DEBUG_LOG ("jim :re send data err [%d]",errno );	
		   close(sockfd);
		   sockfd=-1;
           return NET_ERR;		
		}
		return ret;
	}else{
		return ret;
	}
}


int Cclientproto::send_user_set_vip_flag_cmd(uint32_t userid,uint32_t flag)
{
	userinfo_set_vip_flag_in * in=PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,USER_SET_VIP_FLAG_CMD , userid,
			sizeof(*in ) );
	in->flag=flag;
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int Cclientproto::user_add_offline_msg(uint32_t userid,const char * msg )
{
	int msg_len=strlen(msg);
	set_pub_buf(sendbuf,USER_ADD_OFFLINE_MSG_CMD , userid,
			 65+4+msg_len );
	user_add_offline_msg_in_header * in=PRI_SEND_IN_POS;
	in->msglen=65+4+msg_len;
	AS_MSG *p_as_msg=(AS_MSG* )(sendbuf+PROTO_HEADER_SIZE+sizeof(user_add_offline_msg_in_header));		
/*typedef struct  as_msg{
		uint8_t version;
		uint32_t v1;
		uint32_t v2;
		uint32_t v3;
		uint32_t v4;
		uint32_t v5;
		uint32_t v6;
		char     nick[16];
		uint32_t v7;
		uint32_t logtime;
		uint32_t v8;
		uint32_t as_msglen;
		char msg[2000];
} __attribute__((packed)) AS_MSG;
*/


	p_as_msg->msglen=htonl(61+4+msg_len);
	p_as_msg->version=1;
	p_as_msg->v1=htonl(10003);
	p_as_msg->v2=0;
	p_as_msg->v3=0;
	p_as_msg->v4=0;
	p_as_msg->v5=0;
	p_as_msg->v6=0;

	p_as_msg->v7=0;
	p_as_msg->logtime= htonl(time(NULL));
	p_as_msg->v8=0;
	p_as_msg->as_msglen=htonl(msg_len);
	strcpy( p_as_msg->msg,msg );
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();

}






int Cclientproto::user_get_offline_msg_list(uint32_t userid)
{
	set_pub_buf(sendbuf,USER_GET_OFFLINE_MSG_LIST_CMD , userid, 0 );
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int Cclientproto:: f_user_get_flag ( userid_t userid, user_get_flag_out  *p_out )
{
	set_pub_buf(sendbuf,USER_GET_FLAG_CMD , userid,0);
	SEND_DATA_AND_CHECK_SUCC();
		memcpy(p_out,
			  (user_get_flag_out *)(recvbuf+PROTO_HEADER_SIZE),
			  sizeof(user_get_flag_out ) );
	RETURN();
}

int Cclientproto:: f_userinfo_get_user_email( userid_t userid,
		userinfo_get_user_email_out  *p_out )
{
	set_pub_buf(sendbuf,USERINFO_GET_USER_EMAIL_CMD , userid,0);
	SEND_DATA_AND_CHECK_SUCC();
		memcpy(p_out,
			  (userinfo_get_user_email_out*)(recvbuf+PROTO_HEADER_SIZE),
			  sizeof(*p_out ) );
	RETURN();
}

int Cclientproto:: f_USERINFO_GET_NICK( userid_t userid,
		userinfo_get_nick_out  *p_out )
{
	set_pub_buf(sendbuf,USER_GET_NICK_CMD , userid,0);
	SEND_DATA_AND_CHECK_SUCC();
		memcpy(p_out,
			  (userinfo_get_nick_out*)(recvbuf+PROTO_HEADER_SIZE),
			  sizeof(*p_out ) );
	RETURN();
}


int Cclientproto:: send_data(char* filename)
{
	int sendlen=0;
	FILE *fp=fopen(filename,"r");	
	if (fp==NULL){
		DEBUG_LOG("fp err");
		return FAIL;
	}
	sendlen= fread(sendbuf,1,sizeof (sendbuf) ,fp);
	DEBUG_LOG("sendlen %d", sendlen);
		
	fclose(fp);

	if ( sendlen<(int) PROTO_HEADER_SIZE ) {
		DEBUG_LOG("< proto len");
		return FAIL;
	}

	if ((*(int*)sendbuf)!=sendlen) {
		//发送长度不等于实际长度
		DEBUG_LOG("no eq");
		return FAIL;
	}

	SEND_DATA_AND_CHECK_SUCC();
	RETURN();

}
int  Cclientproto::game_get_score_list (uint32_t gameid , stru_count * out, score_list_item  ** pp_list )
{

	get_game_score_list_in *in=PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,GET_GAME_SCORE_LIST_CMD,0,sizeof(*in) );
	in->gameid=gameid;
		
	SEND_DATA_AND_CHECK_SUCC();
		uint32_t msglen= ((PROTO_HEADER * )recvbuf)->proto_length;
		uint32_t list_len;
		if (msglen<22) 
			return  PROTO_RETURN_LEN_ERR;   

		memcpy(out,recvbuf+PROTO_HEADER_SIZE, LIST_COUNT_SIZE  );

		list_len=sizeof (score_list_item)*out->count;
		if (msglen!=22+list_len) 
			return PROTO_RETURN_LEN_ERR;

		if ((*pp_list= (score_list_item *)malloc(msglen))==NULL){
			return SYS_ERR;
		}

		memcpy(*pp_list,recvbuf+PROTO_HEADER_SIZE +LIST_COUNT_SIZE,
		sizeof (score_list_item) * out->count);
	RETURN();
}
int Cclientproto::game_update_game_score(uint32_t gameid , userid_t userid,uint32_t score,char*nick  )
{
	set_game_score_in * in= PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,SET_GAME_SCORE_CMD , 0 ,
			sizeof( *in) );
	in->gameid=gameid;
	in->userid=userid;
	in->score=score;
	strncpy(in->nick,nick,NICK_LEN ) ;
	SEND_DATA_AND_CHECK_SUCC();
	//
	RETURN();
}
int Cclientproto::game_init_game_list(uint32_t gameid , uint32_t count  )
{
	init_game_score_list_in * in= PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,INIT_GAME_SCORE_LIST_CMD , 0 ,
			sizeof( *in) );
	in->gameid=gameid;
	in->count=count;
	SEND_DATA_AND_CHECK_SUCC();
	//
	RETURN();
}

int Cclientproto::serial_set_serial_unused_cmd(
		uint64_t serialid)
{

	set_serial_unused_in * in= PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,SET_SERIAL_UNUSED_CMD , 0 , sizeof( *in) );
	in->serialid=serialid ;
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int Cclientproto::serial_set_serial_used_cmd(
		uint64_t serialid, uint32_t useflag ,uint32_t *p_price )
{

	set_serial_used_out *out;
	set_serial_used_in * in= PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,SET_SERIAL_USED_CMD , 0 , sizeof( *in) );
	in->serialid=serialid ;
	in->useflag=useflag;
	SEND_DATA_AND_CHECK_SUCC();
	out=(set_serial_used_out *)(recvbuf+PROTO_HEADER_SIZE);
	*p_price=out->price;
	RETURN();

}



int Cclientproto::f_userinfo_register_ex(userid_t userid,userinfo_register_ex_in *p_in)
{
	userinfo_register_ex_in *in=PRI_SEND_IN_POS;
	int private_size=sizeof(*in);
	set_pub_buf(sendbuf,USERINFO_REGISTER_EX_CMD ,userid, private_size);
	memcpy(in, p_in,sizeof(*p_in ) );
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int  Cclientproto::f_emailsys_add_email( userid_t userid, emailsys_add_email_in_header *p_in )
{
	emailsys_add_email_in_header *in=PRI_SEND_IN_POS;
	int private_size=sizeof(*in)+p_in->msglen;
	set_pub_buf(sendbuf, EMAILSYS_ADD_EMAIL_CMD ,userid, private_size);
	memcpy(in, p_in,sizeof(*p_in )+p_in->msglen );
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int  Cclientproto::f_userinfo_get_nick_flist( userid_t userid, userinfo_get_nick_flist_out *p_out )
{
	set_pub_buf(sendbuf,USER_GET_NICK_FLIST_CMD , userid ,  0);
	SEND_DATA_AND_CHECK_SUCC();
		uint32_t msglen= ((PROTO_HEADER * )recvbuf)->proto_length;
		uint32_t pri_len= msglen-PROTO_HEADER_SIZE;
		if (pri_len> sizeof (userinfo_get_nick_flist_out)){
			return PROTO_RETURN_LEN_ERR;
		}
		memcpy(p_out,recvbuf+PROTO_HEADER_SIZE, pri_len );
	RETURN();
}

int  Cclientproto::f_switch_add_user_msg( userid_t userid, const char * msg)
{

	int msg_len=strlen(msg);
	set_pub_buf(sendbuf,0xF619 , 0,
			 14+ (61+4 + msg_len)+(65+4+msg_len) );

	SWITCH_MSG  *p_switch_msg =(SWITCH_MSG * )(sendbuf+PROTO_HEADER_SIZE);		
	p_switch_msg->v=0;
	p_switch_msg->serverid=0;
	p_switch_msg->userid=userid;
	p_switch_msg->msglen=61+4+msg_len;
	//strcpy (p_switch_msg->msg,msg);


	user_add_offline_msg_in_header * in= (user_add_offline_msg_in_header* )
		(sendbuf+PROTO_HEADER_SIZE+14+61+4+msg_len);		
	in->msglen=65+4+msg_len;


	AS_MSG *p_as_msg=(AS_MSG* )(sendbuf+PROTO_HEADER_SIZE+14+61+4 +msg_len 
			+sizeof(user_add_offline_msg_in_header)  );		
/*typedef struct  as_msg{
		uint8_t version;
		uint32_t v1;
		uint32_t v2;
		uint32_t v3;
		uint32_t v4;
		uint32_t v5;
		uint32_t v6;
		char     nick[16];
		uint32_t v7;
		uint32_t logtime;
		uint32_t v8;
		uint32_t as_msglen;
		char msg[2000];
} __attribute__((packed)) AS_MSG;
*/


	p_as_msg->msglen=htonl(61+4+msg_len);
	p_as_msg->version=1;
	p_as_msg->v1=htonl(10003);
	p_as_msg->v2=0;
	p_as_msg->v3=0;
	p_as_msg->v4=0;
	p_as_msg->v5=0;
	p_as_msg->v6=0;

	p_as_msg->v7=0;
	p_as_msg->logtime= htonl(time(NULL));
	p_as_msg->v8=0;
	p_as_msg->as_msglen=htonl(msg_len);
	strcpy( p_as_msg->msg,msg );
	memcpy(p_switch_msg->msg,p_as_msg,61+4+msg_len );
	return this->net_send_no_return();

}

int Cclientproto::f_other_sync_vip(userid_t userid,uint32_t vipflag )
{
	other_sync_vip_in *in=PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,OTHER_SYNC_VIP_CMD , userid ,  sizeof(*in) );
	in->vipflag=vipflag;	
	return this->net_send_no_return();
}


int Cclientproto::switch_pp_sync_vip(userid_t userid,uint32_t vipflag )
{
	other_sync_vip_in *in=PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,0xEA63, userid ,  sizeof(*in) );
	in->vipflag=vipflag;	
	return this->net_send_no_return();
}

int Cclientproto:: pp_register(userid_t userid,pp_register_in * p_in )
{
    pp_register_in * in=PRI_SEND_IN_POS;
    int private_size=sizeof(*in );
    set_pub_buf(sendbuf,PP_REGISTER_CMD,userid, private_size);
    memcpy (in, p_in, sizeof(*in ));
    SEND_DATA_AND_CHECK_SUCC();
    RETURN();
}

int Cclientproto:: mole_register(userid_t userid,mole_register_in * p_in )
{
    mole_register_in * in=PRI_SEND_IN_POS;
    int private_size=sizeof(*in );
    set_pub_buf(sendbuf,USER_REGISTER_EX2_CMD,userid, private_size);
    memcpy (in, p_in, sizeof(*in ));
    SEND_DATA_AND_CHECK_SUCC();
    RETURN();
}



//基础库登入
int Cclientproto::userinfo_login_ex( userid_t userid, char * passwd, uint32_t ip,
		uint32_t login_channel,	uint32_t *p_gameflag)
{
	userinfo_login_ex_with_stat_out *out;
	userinfo_login_ex_with_stat_in * in= PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,USERINFO_LOG_WITH_STAT_CMD, userid , sizeof( *in) );
	memcpy(in->passwd,passwd,PASSWD_LEN);
	in->ip = ip;
	in->login_channel = login_channel;
	SEND_DATA_AND_CHECK_SUCC();
		out=(userinfo_login_ex_with_stat_out *)(recvbuf+PROTO_HEADER_SIZE);
		*p_gameflag=out->gameflag;
	RETURN();
}


//基础库登入byemail
int Cclientproto::userinfo_login_by_email(  char * email,char * passwd,
	    userid_t *p_userid, uint32_t ip, uint32_t addr_type, uint32_t *p_gameflag)
{
	int result;
	if ((result=this->send_get_userid_by_email_cmd(email, p_userid))!=SUCC){
		return result;
	}
	result=this->userinfo_login_ex(*p_userid, passwd, ip, addr_type, p_gameflag );
	return result;
}


int Cclientproto::userinfo_get_gameflag(  userid_t userid , 	uint32_t *p_gameflag  )
{
	userinfo_get_gameflag_out *out;
	set_pub_buf(sendbuf,USERINFO_GET_GAMEFLAG_CMD, userid , 0 );
	SEND_DATA_AND_CHECK_SUCC();
		out=(userinfo_get_gameflag_out *)(recvbuf+PROTO_HEADER_SIZE);
		*p_gameflag=out->gameflag;
	RETURN();
}

int Cclientproto::userinfo_add_game(  userid_t userid , uint32_t	gameidflag  )
{
	userinfo_add_game_in * in= PRI_SEND_IN_POS;
	set_pub_buf(sendbuf,USERINFO_ADD_GAME_CMD, userid , sizeof( *in) );
	in->gameid_flag=gameidflag;
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}
int Cclientproto::mole_login ( userid_t userid,mole_login_out  * p_out )
{
	set_pub_buf(sendbuf,USER_LOGIN_EX_CMD ,userid, 0);
	SEND_DATA_AND_CHECK_SUCC();
		uint32_t pri_msglen= ((PROTO_HEADER * )recvbuf)->proto_length-PROTO_HEADER_SIZE;
		if (pri_msglen<8 || pri_msglen  >sizeof(*p_out)) {
			return  PROTO_RETURN_LEN_ERR;   
		}
		memcpy(p_out,recvbuf+PROTO_HEADER_SIZE, pri_msglen);
		if (p_out->friendcount*sizeof(userid_t)!=pri_msglen-8 ) {
			return  PROTO_RETURN_LEN_ERR;   
		}
	RETURN();
}

int Cclientproto::pp_login ( uint32_t userid,pp_login_out  * p_out )
{
	set_pub_buf(sendbuf,PP_LOGIN_CMD ,userid, 0);
	SEND_DATA_AND_CHECK_SUCC();
		memcpy (p_out, (pp_login_out*)(recvbuf+PROTO_HEADER_SIZE),sizeof(*p_out ) );
	RETURN();
}
int Cclientproto::f_dv_register( userid_t userid, uint32_t sex,char* passwd,char* nick,char*email)
{
    dv_register_in* in=PRI_SEND_IN_POS;
    set_pub_buf(sendbuf,DV_REGISTER_CMD ,userid , sizeof(*in)  );
    memcpy (in-> passwd,passwd,PASSWD_LEN  );
    in->sex=sex;
    memcpy (in-> nick ,nick,NICK_LEN  );
    memcpy (in-> email,email ,EMAIL_LEN  );
    SEND_DATA_AND_CHECK_SUCC();
    RETURN();
}
int Cclientproto::f_dv_change_passwd( userid_t userid,
     char* newpasswd)
{
    dv_change_passwd_in* in=PRI_SEND_IN_POS;
    set_pub_buf(sendbuf,DV_CHANGE_PASSWD_CMD ,userid , sizeof(*in)  );
    memcpy (in-> newpasswd,newpasswd,PASSWD_LEN  );
    SEND_DATA_AND_CHECK_SUCC();
    RETURN();
}

int Cclientproto::f_dv_change_nick( userid_t userid,
     char* newnick)
{
    dv_change_nick_in* in=PRI_SEND_IN_POS;
    set_pub_buf(sendbuf,DV_CHANGE_NICK_CMD ,userid , sizeof(*in)  );
    memcpy (in-> newnick,newnick,NICK_LEN  );
    SEND_DATA_AND_CHECK_SUCC();
    RETURN();
}


int Cclientproto::pp_check_existed (userid_t userid,uint32_t * p_is_existed  )
{
 	set_pub_buf(sendbuf,PP_CHECK_EXISTED_CMD ,userid , 0  );
	is_existed_stru *out;
    SEND_DATA_AND_CHECK_SUCC();
		out=(is_existed_stru *)(recvbuf+PROTO_HEADER_SIZE);
		*p_is_existed=out->is_existed;
    RETURN();
}

int Cclientproto::pp_add_friend (userid_t userid, userid_t friendid)
{
	pp_add_friend_in * in=PRI_SEND_IN_POS;
    set_pub_buf(sendbuf,PP_ADD_FRIEND_CMD ,userid , sizeof(*in)  );
	in->id=friendid;
    SEND_DATA_AND_CHECK_SUCC();
    RETURN();
}

int Cclientproto::pp_add_child_count (  userid_t userid )
{
	set_pub_buf(sendbuf,PP_ADD_CHILD_COUNT_CMD , userid , 0 );
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}

int Cclientproto:: userid_get_userid( userid_get_userid_out *p_out )
{
 	set_pub_buf(sendbuf,USERID_GET_USERID_CMD ,0, 0  );
    SEND_DATA_AND_CHECK_SUCC();
		*p_out=*((userid_get_userid_out *)(recvbuf+PROTO_HEADER_SIZE));
    RETURN();
}

int Cclientproto::userid_set_noused(userid_t userid )
{
	set_pub_buf(sendbuf,USERID_SET_NOUSED_CMD , userid , 0 );
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}


