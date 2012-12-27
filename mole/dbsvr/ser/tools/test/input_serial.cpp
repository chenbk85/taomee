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
#include "mysql_iface.h"

mysql_interface  db("localhost","root","ta0mee");	

int
main (int argc, char **argv)

{
	char sLine[250]={};
	FILE *fp=fopen( "data.txt","r" );
	int ret;
	int affect_row;
	while(!feof(fp)){
		fgets(sLine,250,fp);
		if ( (ret=db.exec_update_sql( sLine,&affect_row) )!=DB_SUCC){
			printf("DBERR [%s][%d]\n",sLine,ret)	;
		}
	}
	return SUCC;
}

