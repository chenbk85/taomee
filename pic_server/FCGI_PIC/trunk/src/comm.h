/*
 * =====================================================================================
 *
 *       Filename:  comm.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/29/2010 08:53:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  kathy (zx), kathy@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef COMM_H
#define COMM_H

inline char * bin2hex(char * dst, char * src,int len, int max_len)
{
    static char buf[500*3+1];
    if (dst==NULL){
        max_len=500;
        dst=buf;
    }
    int hex;
    int i;
    int di;
    if (len>max_len) len=max_len;
    for(i=0;i<len;i++){
        hex=((unsigned char)src[i])>>4;
        di=i*3;
        dst[di]=hex<10?'0'+hex:'A'-10 +hex ;
        hex=((unsigned char)src[i])&0x0F;
        dst[di+1]=hex<10?'0'+hex:'A'-10 +hex ;
        dst[di+2]=' ';
    }
    dst[len*3]=0;
    return dst;
}


#endif

