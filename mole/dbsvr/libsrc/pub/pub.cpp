/*
 * =====================================================================================
 *
 *       Filename:  pub.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年12月27日 15时53分49秒 CST
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
#include <stdlib.h>
#include <string.h>
#include <iconv.h>

#ifdef __cplusplus
extern "C" {
#endif
	void DES(char *,char *,char *);
	void _DES(char *,char *,char *);

#ifdef __cplusplus
}
#endif



bool _iconv( char *in ,char *out, size_t inlen , 
		size_t  outlen, char *  inencode="UTF-8",char * outencode="GBK" )
{ 	
	iconv_t cd;
    int n ;
    cd = iconv_open(outencode ,inencode  );
    if (cd == (iconv_t)-1) { return false; }
    n = iconv(cd, &in, &inlen, &out, &outlen);
    iconv_close(cd);
    if (n == -1) { return false;}
    return true;
}



