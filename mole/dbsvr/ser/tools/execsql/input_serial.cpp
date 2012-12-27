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
//#include "benchapi.h" 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql_iface.h>
int insert_data()
{
	char sqlstr[8000];
	int affect_row, ret; 
	FILE *fp=fopen("/data.sql","r");
	while( fgets(sqlstr,8000, fp ) ){
		mysql_interface  db("localhost","root","ta0mee");	
		if ( (ret=db.exec_update_sql( sqlstr,&affect_row) )!=DB_SUCC){
			printf("DBERR [%d]\n",ret)	;
			//return 0;
		}
	}
	return 0;
}
int
main (int argc, char **argv)
{
	insert_data();
	return 0;
}

