/**
 * =====================================================================================
 *       @file  safe_socket.h
 *      @brief  封装Socket的recv, send, sendfile函数
 *
 *  主要目的保证数据发送完毕, 简化接收一个文件，发送一个文件操作
 *
 *   @internal
 *     Created  2008年10月18日 10时23分38秒
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee .Inc
 *   Copyright  Copyright (c) 2008, aceway
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#ifndef  SAFE_SOCKET_INC
#define  SAFE_SOCKET_INC

#define RCV_SND_MAX_EAGAIN_TIMES        2
#define SENDFILE_MAX_EAGAIN_TIMES       65535

int recv_until_chars(int fd, char* data_buf, const int data_buf_len, const char* until_chars,
                     char *ext_data_buf, const int ext_buf_len, int& ext_data_len); 

int safe_recv_len(int fd, char* data_buf, const int data_buf_len, const int wanted_data_len); 

int safe_write_len(int file_fd, char* data_buf, const int data_buf_len, const int wanted_write_len);

int save_data_from_socket(int socket_fd, int file_fd, const int save_len);

int safe_send_len(int fd, char* data_buf, const int data_buf_len, const int wanted_data_len);

int safe_sendfile(int file_fd, int socket_fd);

#endif   /* ----- #ifndef SAFE_SOCKET_INC  ----- */

