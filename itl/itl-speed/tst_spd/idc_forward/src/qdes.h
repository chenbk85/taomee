/**
 *============================================================
 *  @file      qdes.h
 *  @brief     DES ECB加密
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef QDES_H_
#define QDES_H_

/**
  * @brief DES ECB加密。只能加密大小为8字节的数据。 
  *
  * @param const void* key,  加密用的密钥。key指向的内存必须>=8个字节，并且只有前8个字节有效。
  * @param const void* s_text,  需要加密的数据。s_text指向的内存必须>=8个字节，并且只有前8个字节被加密。
  * @param void* d_text,  加密后的密文(8个字节)会存放到d_text里。
  *
  * @return void
  */
void des_encrypt(const void* key, const void* s_text, void* d_text);

/**
  * @brief DES ECB解密。只能解密大小为8字节的数据。 
  *
  * @param const void* key,  解密用的密钥，和加密用的密钥相同。key指向的内存必须>=8个字节，并且只有前8个字节有效。
  * @param const void* s_text,  需要解密的数据。s_text指向的内存必须>=8个字节，并且只有前8个字节被解密。
  * @param void* d_text,  解密后的明文(8个字节)会存放到d_text里。
  *
  * @return void
  */
void des_decrypt(const void* key, const void* s_text, void* d_text);

/**
  * @brief DES ECB加密。可以加密大小为8字节的倍数的数据。 
  *
  * @param const void* key,  加密用的密钥。key指向的内存必须>=8个字节，并且只有前8个字节有效。
  * @param const void* s_text,  需要加密的数据。
  * @param void* d_text,  加密后的密文会存放到d_text里。
  * @param int n,  表示s_text和d_text指向的内存大小为8字节的n倍。
  *
  * @return void
  */
void des_encrypt_n(const void* key, const void* s_text, void* d_text, int n);

/**
  * @brief DES ECB解密。可以解密大小为8字节的倍数的数据。 
  *
  * @param const void* key,  解密用的密钥，和加密用的密钥相同。key指向的内存必须>=8个字节，并且只有前8个字节有效。
  * @param const void* s_text,  需要解密的数据。
  * @param void* d_text,  解密后的明文会存放到d_text里。
  * @param int n,  表示s_text和d_text指向的内存大小为8字节的n倍。
  *
  * @return void
  */
void des_decrypt_n(const void* key, const void* s_text, void* d_text, int n);

#endif //QDES_H_

