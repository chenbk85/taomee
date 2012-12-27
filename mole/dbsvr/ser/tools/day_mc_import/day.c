/*
 * =====================================================================================
 *
 *       Filename:  a.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年10月20日 18时27分19秒 CST
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
#include <inttypes.h>
#include <stdio.h>
#include <memcache.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int
main ( int argc, char *argv[] )
{
	struct memcache *mc= mc_new();
	uint32_t value;
	char key[100];
	FILE *fp=fopen("a.txt","r");
	mc_server_add4(mc, "localhost:11211");
	while(feof(fp )){
		fscanf(fp, "%s %d", key, &value );
		DEBUG_LOG("%s",key);
	    mc_set(mc,key, strlen(key), &value , sizeof(value) ,0,0 );
	}
	return 0;
}				/* ----------  end of function main  ---------- */
