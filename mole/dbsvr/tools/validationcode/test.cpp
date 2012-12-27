/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年02月15日 12时30分22秒 CST
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

#include "Cvalidationcode.h" 
int
main ( int argc, char *argv[] )
{
	mysql_interface *db;		
	Cvalidationcode *v;
	uint32_t id;
	int ret;
	char code [VALIDATIONCODE_LEN+1];
	memset(code,0,sizeof(code));
	db=new mysql_interface ("localhost","root","ta0mee" );
	v=new Cvalidationcode(db);
	strcpy(code,"abcde");
	if ((ret=v->add(code,&id ))==SUCC){
		printf("gen id %u\n", id);
	}else{
		printf("adderr  ret %d\n", ret );
	}
	if ((ret=v->check(id,code ))==SUCC){
		printf(" check ok %u\n", id);
	}else{
		printf(" check err %d\n", ret );
	}

	return 0 ;
}				/* ----------  end of function main  ---------- */
