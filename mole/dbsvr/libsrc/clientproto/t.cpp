#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "Cclientproto.h"
#include "logproto.h"
#include "Csem.h"
int main (int argc, char* argv[])
{
	//if (argc!=4){
		//printf ("need 3 arg:ip:port:userid");
		//return 1;
	//}
	//char  * comip= argv[1];
	//short comport=atoi(argv[2]);
	//userid_t userid=atoi(argv[3]);
	//USERINFO_GET_NICK_OUT out={ };
//	char  comip[20]= "10.1.1.5" ;

	int ret;
	uint32_t map_uid,gameflag;
    Cclientproto * cp=new  Cclientproto( "10.1.1.5", 21001);
	//ret=cp->mole_register(88666,0);	
	userinfo_register_ex_in  in;
	strcpy(in.email,"sdlfa@cccom");
//	ret=cp->userinfo_register(88167,&in,&map_uid );
	uint32_t ip = 10;
	uint32_t login_channel = 10;
	ret=cp->userinfo_login_by_email(in.email, in.passwd, &map_uid, ip, login_channel, &gameflag );
	printf("%d,%u,%u\n",ret,map_uid,gameflag );
	
	delete cp;
	return 0;
}
/*	
	char buf[4096];
	char msg[4096];
	//HOPE_GET_HOPE_BY_DATE_ITEM * p_list;
	LIST_COUNT out_header;
	USER_REGISTER_IN in; 
	userid_t *p_list;
	userid_t uid;
	memset(&in, 0,sizeof (in));
	
	
	strcpy( in.nick,"show");
	strcpy( in.passwd,"1111111111111111");
	//strcpy( in.email,"bbc@ddd.com");
	strcpy( in.email,"blclrlg5@ggg.com");
//	USER_GET_FLAG_OUT out;
//	USERINFO_GET_USER_EMAIL_OUT out;
//	USERINFO_GET_NICK_OUT out;
	USERINFO_LOGIN_OUT 	out;
		
//	ret=cp->send_register_cmd( 850010,&in ,&uid );
//	ret=cp->send_login_cmd( 850010,in.passwd ,0,0,&out_header,&p_list  );
//	ret=cp->f_USERINFO_GET_NICK_FLIST( 20066 , &out  );
	//EMAILSYS_ADD_EMAIL_IN_HEADER * e_in;
	//e_in = (EMAILSYS_ADD_EMAIL_IN_HEADER *) buf ;	
	//e_in->sendtime= time(NULL);
	//e_in->senderid=0;
	//e_in->type=1000018;
	//memcpy( e_in->sendernick, "ssssssssssssssssssssssssss", 16 );
	//sprintf(msg,"%s(%u)",
	 //"show" ,20088);
	ret=cp->send_login_cmd(50548,in.passwd, 1,1,&out );
	
	printf("%d\n",ret);
	*/

