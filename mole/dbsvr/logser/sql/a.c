/*
 * =====================================================================================
 *
 *       Filename:  a.c
 *
 *    Description:  i
 *
 *        Version:  1.0
 *        Created:  2007年11月15日 18时06分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
/* testsql.c
  ** An example to use MYSQL C API
  ** Copyright 2004 Coon Xu.
  ** Author: Coon Xu
  ** Date: 05 Nov 2004
  */
   
  #include <mysql.h>
  #include <stdio.h>
  #include <string.h>
int main(){
   MYSQL mysql;     // need a instance to init
   MYSQL_RES *res;
   MYSQL_ROW row;
   char query[3000];
   int t,r;
   int i;
	unsigned char k[12];
	char k_mysql[30];


   FILE *fp=fopen("/dev/urandom","r");
   mysql_init(&mysql);
   if (!mysql_real_connect(&mysql,"localhost", "root", "xcwen", "OTHER",0,NULL,0))
   {
       printf( "Error connecting to database: %s\n",mysql_error(&mysql));
   }
   else printf("Connected...\n");

 for (i=0;i<100000;i++) {
	fread(k,12,1,fp);
	 mysql_real_escape_string( &mysql, k_mysql, (char*)k,12);	 
	sprintf(query,"insert into t_serial_%02d values('%s', 0)", i/10000,k_mysql);
	//printf ("sql %s\n",query);
	 t = mysql_real_query(&mysql,query,(unsigned int) strlen(query));
	 if(i %10000==0 )
		 printf ("now %d\n",i);
 } 
  mysql_close(&mysql);

	 fclose(fp);
   return 0;
}
