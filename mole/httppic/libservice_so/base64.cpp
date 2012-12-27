/**
 * =====================================================================================
 *       @file  base64.cpp
 *      @brief  对字符串进行加码，并保证加密后的密文仍然是字符串，既不产生非打印字符；相映解密。
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  2008年10月22日 16时21分18秒 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee .Inc
 *   Copyright  Copyright (c) 2008, aceway
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <stdio.h>
#include "base64.h"

/** 
 * @brief  加密字符串, 提供对外调用
 * @param  char *buf, 加密后的字符串, 调用者保证内存大小足够 
 * @param  char *text, 将被加密的字符串 
 * @param  int size, 要加密的字符串长度 
 * @return int 类型， 加密后字符串的长度
 */
int Base64Enc(char *buf, char*text,int size) 
{ 
    static char *base64_encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int buflen = 0; 

    while(size>0)
    {
        *buf++ = base64_encoding[ (text[0] >> 2 ) & 0x3f];
        if(size>2)
        {
            *buf++ = base64_encoding[((text[0] & 3) << 4) | (text[1] >> 4)];
            *buf++ = base64_encoding[((text[1] & 0xF) << 2) | (text[2] >> 6)];
            *buf++ = base64_encoding[text[2] & 0x3F];
        }
        else
        {
            switch(size)
            {
                case 1:
                    *buf++ = base64_encoding[(text[0] & 3) << 4 ];
                    *buf++ = '=';
                    *buf++ = '=';
                    break;
                case 2: 
                    *buf++ = base64_encoding[((text[0] & 3) << 4) | (text[1] >> 4)]; 
                    *buf++ = base64_encoding[((text[1] & 0x0F) << 2) | (text[2] >> 6)]; 
                    *buf++ = '='; 
                    break; 
            } 
        } 

        text +=3; 
        size -=3; 
        buflen +=4; 
    } 
    *buf = 0; 
    return buflen; 
} 

/** 
 * @brief  解密字符串, 提供对外调用 
 * @param  char *buf, 解密后的字符串, 调用者提供，并保证长度足够
 * @param  char *text, 将解密的字符串 
 * @param  int size, 解密字符串的长度, 长度必须是4的倍数，否则返回错误(MIME标准)
 * @return int 类型，解密后字符串的长度, 错误返回-1 
 */
int Base64Dec(char *buf,char*text,int size) 
{
    if(size%4)
        return -1;
    unsigned char chunk[4];
    int parsenum=0;

    int i = 0;
    while(size>0)
    {
        chunk[0] = GetBase64Value(text[0]); 
        chunk[1] = GetBase64Value(text[1]); 
        chunk[2] = GetBase64Value(text[2]); 
        chunk[3] = GetBase64Value(text[3]); 

        *buf++ = (chunk[0] << 2) | (chunk[1] >> 4); 
        *buf++ = (chunk[1] << 4) | (chunk[2] >> 2); 
        *buf++ = (chunk[2] << 6) | (chunk[3]);

        text+=4;
        size-=4;
        parsenum+=3;
        i++;
    }
    return parsenum;
} 

//base64解码的实现

/** 
 * @brief  获取一个字符的对应base64码, 供本模块内部调用
 * @param  char ch, 源字符 
 * @return char 类型，相应的base64码 
 */
char GetBase64Value(char ch)
{
    if ((ch >= 'A') && (ch <= 'Z')) 
        return ch - 'A'; 
    if ((ch >= 'a') && (ch <= 'z')) 
        return ch - 'a' + 26; 
    if ((ch >= '0') && (ch <= '9')) 
        return ch - '0' + 52; 
    switch (ch) 
    { 
        case '+': 
            return 62; 
        case '/': 
            return 63; 
        case '=': /* base64 padding */ 
            return 0; 
        default: 
            return 0; 
    } 
}

