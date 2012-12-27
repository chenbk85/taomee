/*
 * =====================================================================================
 *
 *			 Filename:	input_shm.cpp
 *
 *		Description:	
 *
 *				Version:	1.0
 *				Created:	2008年01月14日 11时08分58秒 CST
 *			 Revision:	none
 *			 Compiler:	gcc
 *				 Author:	xcwen (xcwen), xcwenn@gmail.com
 *				Company:	TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:	set tabstop=4 
 * 		
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql_iface.h>


int main (int argc, char **argv)
{
	char sqlstr[1000];
	FILE *fp=fopen("/dev/urandom","r");
	uint8_t rand_tmp[9];
	int affect_row, ret; 
	int i;
	memset(rand_tmp, 0,sizeof(rand_tmp) );
	mysql_interface db("localhost","root","ta0mee");	
	
	for (i=20000;i<20500;i++){
		fread(&rand_tmp,8,1,fp);	
		for (int j=0;j<8;j++){
			rand_tmp[j]=rand_tmp[j]%36;
			if (rand_tmp[j]<10) rand_tmp[j]+='0';
			else   rand_tmp[j]=rand_tmp[j]-10+'A' ;
		}
	//	printf("%s\n",rand_tmp);
	//	continue;
		sprintf(sqlstr,"insert into REGISTER_SERIAL_DB.t_register_serial  values( '%s',%u,0)",
			 rand_tmp,i );
		if ( (ret=db.exec_update_sql( sqlstr,&affect_row) )!=DB_SUCC){
			i--;
		}
	}	

	for (i=30000;i<30500;i++){
		fread(&rand_tmp,8,1,fp);	
		for (int j=0;j<8;j++){
			rand_tmp[j]=rand_tmp[j]%36;
			if (rand_tmp[j]<10) rand_tmp[j]+='0';
			else   rand_tmp[j]=rand_tmp[j]-10+'A' ;
		}
	//	printf("%s\n",rand_tmp);
	//	continue;
		sprintf(sqlstr,"insert into REGISTER_SERIAL_DB.t_register_serial  values( '%s',%u,0)",
			 rand_tmp,i );
		if ( (ret=db.exec_update_sql( sqlstr,&affect_row) )!=DB_SUCC){
			i--;
		}
	}	
	return SUCC;
}

