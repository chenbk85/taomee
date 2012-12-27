/*
 * =====================================================================================
 *
 *       Filename:  email_gen.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年07月14日 15时56分36秒 CST
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
#include <stdio.h>
#include"common.h"
int
main ( int argc, char *argv[] )
{
	uint32_t userid,id;	
	char email[65];
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	fp = fopen("f.in", "r");
	//fp = fopen("/etc/mtab", "r");
	if (fp == NULL)
		exit(1);

	while ((read = getline(&line, &len, fp)) != -1) {
		sscanf(line,"%u|%s",&userid,email);
		//int i=0;
		//char *p=line;
		//while ( *p !='|' && *p!='\0' ) p++; 

		id=hash_str(email);
	//	printf ("%u\n",id);
		printf ("%u%u:('%s',%u),\n",
				id%10,(id/10)%10, email, userid);
	}
	if (line)
		free(line);
	return 0;
}				/* ----------  end of function main  ---------- */
