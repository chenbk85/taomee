#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<time.h>
#include "mysql_iface.h"
#include "Cdbdeal.h"
#include "benchapi.h"
#define EXIT_SYS_ERR 1
const size_t  maxbuflength=500;
int get_proto_record(FILE *fpload,char *buf,bool * isEnd );
int
main ( int argc, char *argv[] )
{
	mysql_interface *db;
	Cdbdeal *dbdeal;
	int ret;
	int exit_value=-1; 
	char proto_data_file[500];
	char buf[maxbuflength];
	FILE *fp;
	bool isEnd, isloged;
	uint32_t logcount =0 ;
	char ip[16],logtime[20];
//	db=new mysql_interface("192.168.0.7","jim","jim123");
	db=new mysql_interface(argv[1],argv[2],argv[3]);
//	db=new mysql_interface("localhost","root","ta0mee");
	dbdeal=new Cdbdeal(db);
	// argc count 
	if (argc!=7 ){
		return EXIT_SYS_ERR;
	}	
	strncpy(proto_data_file,argv[4],sizeof(proto_data_file));
	strncpy(logtime,argv[5],sizeof(logtime));
	strncpy(ip,argv[6],sizeof(ip));
	
	//set logtime
	dbdeal->setlogtime(logtime);	
	
	if (dbdeal->checkserverloged(ip,&isloged )==SUCC){
		if (isloged==true) {
	//		return EXIT_SYS_ERR;
		}
	}else{
	//	return EXIT_SYS_ERR;
	}

	fp=fopen(proto_data_file,"r"); if (fp==NULL) {
		return FAIL;
	}
		
	while(true){
		//get recode 
		ret=get_proto_record(fp,buf,&isEnd );
		if (ret==SUCC){
			if (! isEnd ){
				//deal recode
				dbdeal->deal(buf);
				logcount++;
			}else{
				exit_value=0;
				break;
			}
		}else{ //FALSE;
				exit_value=EXIT_SYS_ERR;
				break;
		}
	}
	//insert server..
	if (exit_value==0){
		if (dbdeal->insertserver(ip,logcount)!=SUCC){
			return EXIT_SYS_ERR;
		}
	}

	fclose(fp);
	delete db; 	
	delete dbdeal; 	
	return exit_value;
}				/* ----------  end of function main  ---------- */
/*
 * SUCC 成功读取  
 * FAIL 出错   
 *
 * isend: 是否读完了
 */ int 
get_proto_record(FILE *fpload,char *buf,bool * isEnd )
{
	size_t len,tmplen;
	*isEnd=false;		

	len=4;
	if ((tmplen=fread(buf,1,len,fpload))!=len){
		if ( tmplen==0 && feof(fpload)){	
			*isEnd=true;		
			return SUCC;
		}else
			return FAIL;
	}

	len=*((int*)buf)-4;
	//DEBUG_LOG("get111 recode [%d]",len);
	if (len>=maxbuflength)
		return FAIL;
	//DEBUG_LOG("get222 recode [%d]",len);
	if (fread(buf+sizeof(int),1,len,fpload)!=len)
		return FAIL;
	//DEBUG_LOG("get333 recode [%d]",len);
	return SUCC;				
}
