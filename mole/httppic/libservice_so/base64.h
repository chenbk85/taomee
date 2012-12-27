/**
 * =====================================================================================
 *       @file  base64.h
 *      @brief  对字符串进行加码，并保证加密后的密文仍然是字符串，既不产生非打印字符；相应解密。
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  2008年10月22日 16时22分13秒 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee .Inc
 *   Copyright  Copyright (c) 2008, aceway
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef __BASE64__H_
#define __BASE64__H_

int Base64Enc(char *buf, char*text, int size);
int Base64Dec(char *buf,char*text,int size); 
char GetBase64Value(char ch);

#endif
