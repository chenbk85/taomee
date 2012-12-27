/**
 * =====================================================================================
 *       @file  safe_socket.cpp
 *      @brief  封装Socket的recv, send, sendfile函数
 *
 *  主要目的保证数据发送完毕, 简化接收一个文件，发送一个文件操作
 *
 *   @internal
 *     Created  2008年10月18日 10时25分03秒
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee .Inc
 *   Copyright  Copyright (c) 2008, aceway
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "safe_socket.h"
#include "../tcp_http/Log.h"

/** 
 * @brief  从fd接收数据，直到收到了指定的字符串， 如果多接收了，则多接收的数据保存在附加buffer。
 * @param  int fd, 接收数据的Socket描述符 
 * @param  char* data_buf, 保存接收数据的缓冲区，保存的是指定字符前的内容（含指定字符） 
 * @param  const int data_buf_len, 接收数据缓冲区的长度 
 * @param  const char* until_chars, 指定接收到该字符串时停止接收的字符串
 * @param  char* ext_data_buf, 当接收到指定字符串后，可能多接收了数据，这是保存这些字符的缓冲区 
 * @param  const int _ext_buf_len, 用于保存多接收内容的缓冲区长度 
 * @param  int& ext_data_len, 多接收数据的字节数 
 * @return 失败返回 －1； 成功返回接收的数据字节数 
 */
int recv_until_chars(int fd, char* data_buf, const int data_buf_len, const char* until_chars,
                     char* ext_data_buf, const int ext_buf_len, int& ext_data_len) 
{
    if(fd < 0) return -1;
    assert(data_buf != NULL);
    assert(until_chars != NULL);
    assert(ext_data_buf != NULL);
    assert(data_buf_len > 0);
    assert(ext_buf_len > 0);

    memset(data_buf, 0, data_buf_len);
    memset(ext_data_buf, 0, ext_buf_len);
    ext_data_len = 0;

    int sock_buf_len = -1;
    int int_len = sizeof(int);
    if(-1 == getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &sock_buf_len, (socklen_t*)&int_len))
    {
        LOG(ERROR, "%s, %d, getsockopt error, info:%s", __FILE__, __LINE__, strerror(errno));
        return -1;
    }

    int len_per_recv = min(sock_buf_len/2, data_buf_len/4); 
    int has_recv_len = 0;
    char* pos_until_chars = NULL; 
    int eagain_times = 0;
    //保证把包头接收完毕
    while(pos_until_chars == NULL)
    {
        int tmp_len = 0;
        tmp_len = recv(fd, data_buf + has_recv_len, len_per_recv,  0);
        //tmp_len = recv(fd, data_buf + has_recv_len, len_per_recv,  MSG_DONTWAIT);
        if(tmp_len > 0)
        {
        //接收正常
            has_recv_len += tmp_len;
            data_buf[data_buf_len-1] = '\0';
            pos_until_chars = strcasestr(data_buf, until_chars);
            eagain_times = 0;
            if(has_recv_len > (data_buf_len - len_per_recv))
            {
                LOG(ERROR, "recv_until_chars() recv len %d bigger than data buffer size %d.\n", 
                        has_recv_len, data_buf_len);
                return -1;
            }
        }
        else if( tmp_len == 0) 
        {
        //客户端关闭
            LOG(DEBUG, "recv_until_chars() client close socket\n");
            return -1;
        }else
        {
        //接收返回负数
            if(tmp_len == -1 && errno == EAGAIN) //超时
            {
                eagain_times++;
                if(eagain_times > RCV_SND_MAX_EAGAIN_TIMES)//超时次数过多
                {
                    LOG(DEBUG, "recv_until_chars() EAGAIN happen %d times, take ass error, has recv len %d.\n", 
                            eagain_times, has_recv_len);
                    return -1;
                }
                tmp_len = 0;
            }
            else //出错
            {
                LOG(DEBUG, "recv_until-chars() cleint close socket\n");
                return -1;
            }
        }
    }//while(phas_until_chars == NULL)

    //多于的数据拷贝到额外缓冲区
    int header_len = 0;
    header_len = pos_until_chars - data_buf + strlen(until_chars);
    ext_data_len = has_recv_len - header_len;
    if(ext_data_len > 0)
    {
        if(ext_data_len > ext_buf_len)
        {
            memcpy(ext_data_buf, data_buf + header_len, ext_buf_len);
        }
        else
        {
            memcpy(ext_data_buf, data_buf + header_len, ext_data_len);
        }
    }
    return has_recv_len;
}

/** 
 * @brief  从socket接收指定长度的数据, 不多接收一个字节，也不少接收一个字节, 否则当失败返回。
 * @param  int fd, 接收数据的socket描述符 
 * @param  char* data_buf, 用于保存接收数据的缓冲区 
 * @param  const int data_buf_len, 保存数据的缓冲区长度 
 * @param  const int wanted_data_len, 要接收多少字节的数据。(wanted_data_len < data_buf_len) 
 * @return  失败返回－1， 成功返回接收的字节数
 */
int safe_recv_len(int fd, char* data_buf, const int data_buf_len, const int wanted_data_len) 
{
    if(fd < 0) return -1;
    assert(data_buf != NULL);
    assert(data_buf_len > 0);

    memset(data_buf, 0, data_buf_len);

    int sock_buf_len = -1;
    int int_len = sizeof(int);
    if(-1 == getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &sock_buf_len, (socklen_t*)&int_len))
    {
        LOG(ERROR, "%s, %d, getsockopt error, info:%s", __FILE__, __LINE__, strerror(errno));
        return -1;
    }

    int len_per_recv = min(sock_buf_len/2, wanted_data_len); 
    int has_recv_len = 0;
    int rest_data_len = wanted_data_len;
    int eagain_times = 0;
    while(has_recv_len != wanted_data_len)
    {
        //一次接收数据
        int tmp_len = 0;
        if(rest_data_len > len_per_recv)
        {
            tmp_len = recv(fd, data_buf + has_recv_len, len_per_recv,  0);
            //tmp_len = recv(fd, data_buf + has_recv_len, len_per_recv,  MSG_DONTWAIT);
        }
        else
        {
            tmp_len = recv(fd, data_buf + has_recv_len, rest_data_len,  0);
            //tmp_len = recv(fd, data_buf + has_recv_len, rest_data_len,  MSG_DONTWAIT);
        }

        //判断接收结果
        if(tmp_len > 0) 
        {
        //正常接收到数据
            has_recv_len += tmp_len;
            rest_data_len = wanted_data_len - has_recv_len;
            eagain_times = 0;
            if(has_recv_len > data_buf_len)
            {
                LOG(ERROR, "safe_recv_len() recv len %d bigger than data buffer size %d.\n", 
                        has_recv_len, data_buf_len);
                return -1;
            }
        }
        else if( tmp_len == 0) 
        {
        //客户端关闭了连接
            LOG(DEBUG, "safe_recv_len() cleint close socket\n");
            return -1;
        }else 
        {
        //接收返回负数
            if(tmp_len == -1 && errno == EAGAIN) //超时
            {
                tmp_len = 0;
                eagain_times++;
                if(eagain_times > RCV_SND_MAX_EAGAIN_TIMES)//超时次数过多
                {
                    LOG(DEBUG, "safe_recv_len() EAGAIN happen %d times, take ass error, has recv len %d.\n", 
                            eagain_times, has_recv_len);
                    return -1;
                }
            }
            else //出错
            {
                LOG(ERROR, "recv() error, info: %s\n", strerror(errno));
                return -1;
            }
        }
    }//while(has_recv_len != wanted_data_len)
    return has_recv_len;
}

/** 
 * @brief  向文件写入指定字节数的数据，不多写一个字节，不少写一个字节，否则当失败返回。
 * @param  int file_fd, 用于写数据的文件描述符，由调用者保证其合法性。 
 * @param  char* data_buf, 要被写入的数据保存在该缓冲区。 
 * @param  const int data_buf_len, 保存数据的缓冲区的长度。 
 * @param  const int wanted_write_len, 要写入的字节数。 
 * @return 失败返回-1, 成功返回写入的字节数
 */
int safe_write_len(int file_fd, char* data_buf, const int data_buf_len, const int wanted_write_len)
{
    if(file_fd < 0) return -1;
    assert(data_buf != NULL);
    assert(data_buf_len > 0);
    if(wanted_write_len <= 0) return 0;

    int has_write_len = 0;
    int rest_write_len = wanted_write_len;
    int eagain_times = 0;
    while(has_write_len != wanted_write_len)
    {
        int tmp_len = 0;
        tmp_len = write(file_fd, data_buf + has_write_len, rest_write_len);
        if(tmp_len > 0)
        {
        //写正常
            has_write_len += tmp_len;
            rest_write_len = wanted_write_len - has_write_len;
            eagain_times = 0;
            if(has_write_len > data_buf_len)
            {
                LOG(ERROR, "safe_write_len() write len expand the data buffer len. take as error!\n");
                return -1;
            }
        }
        else if (tmp_len ==0 )
        {
        //返回0
            eagain_times++;
            if(eagain_times > RCV_SND_MAX_EAGAIN_TIMES)
            {
                LOG(ERROR, "save_write() hapend %d times write 0 byte, take as error!\n", eagain_times);
                return -1;
            }
            continue;
        }
        else
        {
        //返回负数
            if(tmp_len == -1 && errno == EAGAIN)//阻塞
            {
                tmp_len = 0;
                eagain_times++;
                if(eagain_times > 5)
                {
                    LOG(DEBUG, "safe_write_len() EAGAIN happen %d times, take ass error, has write len %d.\n", 
                            eagain_times, has_write_len);
                    return -1;
                }
                continue;
            }
            else
            {
                LOG(DEBUG, "write() return %d, error info: %s\n", tmp_len, strerror(errno));
                return -1;
            }
        }//while(has_write_len != wanted_write_len)

    }

    return has_write_len;
}

/** 
 * @brief 从Socket接收指定字节数的数据，并保存到指定文件。是safe_recv_len和safe_write_len的封装，
 *        多或少接收，多或少保存，均当失败返回。
 * @param  int socket_fd, 接收数据的socket描述符，调用者保证其合法性。 
 * @param  int file_fd, 用于保存数据的文件描述符, 调用者保证其合法性。 
 * @param  const int wanted_save_len, 要接收且保存的字节数。 
 * @return 失败返回－1，成功返回保存的字节数。 
 */
int save_data_from_socket(int socket_fd, int file_fd, const int wanted_save_len)
{
    if(socket_fd < 0 || file_fd < 0) return -1;

    const int trans_buf_len = 4096;
    char trans_buf[trans_buf_len] = {'\0'};

    int has_save_len = 0;
    int rest_save_len = wanted_save_len;

    while(has_save_len != wanted_save_len)
    {
        int recv_len = -1;
        int write_len = -1;
        if(rest_save_len > trans_buf_len)
        {
            recv_len = safe_recv_len(socket_fd, trans_buf, trans_buf_len, trans_buf_len);
            if(recv_len == -1) return -1;

            write_len = safe_write_len(file_fd, trans_buf, trans_buf_len, recv_len);
            if(write_len == -1) return -1;
        }
        else
        {
            recv_len = safe_recv_len(socket_fd, trans_buf, trans_buf_len, rest_save_len);
            if(recv_len == -1) return -1;

            write_len = safe_write_len(file_fd, trans_buf, trans_buf_len, recv_len);
            if(write_len == -1) return -1;
        }
        has_save_len += write_len;
        rest_save_len = wanted_save_len - has_save_len;
        if( has_save_len > wanted_save_len) 
        {
            return -1;
        }
    }
    return has_save_len;
}

/** 
 * @brief  向Socket发送指定字节数的数据, 不多也不少发送，否则当失败返回。
 * @param  int fd, 用于发送数据的Socket描述符。调用者保证其合法性。 
 * @param  char* data_buf, 被发送的数据缓冲区 
 * @param  const int data_buf_len，被发送的数据缓冲区的长度。 
 * @param  const int wanted_data_len, 要发送多少自己的数据。(wanted_data_len < data_buf_len) 
 * @return  
 */
int safe_send_len(int fd, char* data_buf, const int data_buf_len, const int wanted_data_len)
{
    if(fd < 0) return -1;
    assert(data_buf != NULL);
    assert(data_buf_len > 0);

    int sock_buf_len = -1;
    int int_len = sizeof(int);
    if(-1 == getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &sock_buf_len, (socklen_t*)&int_len))
    {
        LOG(ERROR, "%s, %d, getsockopt error, info:%s", __FILE__, __LINE__, strerror(errno));
        return -1;
    }
    int len_per_send = min(sock_buf_len/2, wanted_data_len); 
    int has_send_len = 0;
    int rest_send_len = wanted_data_len;
    int eagain_times = 0;
    while(has_send_len != wanted_data_len)
    {
        int tmp_len = 0;
        if(rest_send_len > len_per_send)
        {
            tmp_len = send(fd, data_buf + has_send_len, len_per_send, 0);
            //tmp_len = send(fd, data_buf + has_send_len, len_per_send, MSG_DONTWAIT);
        }
        else
        {
            tmp_len = send(fd, data_buf + has_send_len, rest_send_len, 0);
            //tmp_len = send(fd, data_buf + has_send_len, rest_send_len, MSG_DONTWAIT);
        }

        if(tmp_len > 0)
        {
            has_send_len += tmp_len;
            rest_send_len = wanted_data_len - has_send_len;
            eagain_times = 0;
            if(has_send_len > data_buf_len)
            {
                LOG(ERROR, "safe_send_len() send len %d bigger thand data buff size %s\n", 
                        has_send_len, data_buf_len);
                return -1;
            }
        }
        else if(tmp_len == 0)
        {
            eagain_times++;
            if(eagain_times > RCV_SND_MAX_EAGAIN_TIMES)
            {
                LOG(ERROR, "safe_send_len() hapend %d times send 0 byte, take as error! has send len %d.\n", 
                        eagain_times, has_send_len);
                return -1;
            }
            continue;
        }
        else
        {
            //if(tmp_len == -1 && (errno == EAGAIN || errno ==EWOULDBLOCK))
            if(tmp_len == -1 && errno == EAGAIN )
            {
                eagain_times++;
                if(eagain_times > RCV_SND_MAX_EAGAIN_TIMES)
                {
                    LOG(DEBUG, "safe_send_len() EAGAIN happen %d times, take ass error, has send len %d.\n", 
                            eagain_times, has_send_len);
                    return -1;
                }
                continue;
            }
            else
            {
                LOG(ERROR, "send() error, info:%s!\n", strerror(errno));
                return -1;
            }
        }
    }//while(has_send_len != wanted_data_len)
    return has_send_len;
}

/** 
 * @brief  将指定文件完整的发送到指定的socket, 是系统函数sendfile()的封装
 * @param  int file_fd, 指定的被发送的文件的描述符，调用者保证其合法性。 
 * @param  int socket_fd, 指定的发送数据的socket描述符，调用者保证其合法性。 
 * @return 成功发送完毕文件返回发送字节数，否则返回－1。 
 */
int safe_sendfile(int file_fd, int socket_fd)
{
    if(file_fd < 0 || socket_fd < 0) return -1;

    off_t read_start = 0;
    off_t has_send_len = 0;
    size_t rest_data_len = 0;

    struct stat file_state;
    memset(&file_state, 0, sizeof(file_state));
    int stat_ok = fstat(file_fd, &file_state);
    if(stat_ok == 0)
    {
        rest_data_len = file_state.st_size;
    }
    else
    {
        LOG(ERROR, "fstat() error, info:%s, %s, %d\n", strerror(errno), __FILE__, __LINE__);
        return -1;
    }
    int eagain_times = 0;
    while(has_send_len != file_state.st_size)
    {
        ssize_t tmp_len = 0;
        tmp_len = sendfile(socket_fd, file_fd, &read_start, rest_data_len);
        if(tmp_len >0)
        {
            has_send_len += tmp_len;
            rest_data_len = file_state.st_size - has_send_len;
            eagain_times = 0;
            if(has_send_len > file_state.st_size)
            {
                return has_send_len;
            }
        }else if(tmp_len == 0)
        {
            eagain_times++;
            if(eagain_times > SENDFILE_MAX_EAGAIN_TIMES)
            {
                LOG(ERROR, "sendfile() return 0 hapend %d times. take as error.\n", eagain_times);
                return -1;
            }
            tmp_len = 0;
            continue;
        }else
        {
            if(tmp_len == -1 && errno == EAGAIN)
            {
                eagain_times++;
                if(eagain_times > SENDFILE_MAX_EAGAIN_TIMES)
                {
                    LOG(DEBUG, "safe_send_len() EAGAIN happen %d times, take ass error, has send len %d.\n", 
                            eagain_times, has_send_len);
                    return -1;
                }
                tmp_len = 0;
                continue;
            }
            else
            {
                LOG(ERROR, "sendfile() error, info %s .\n", strerror(errno));
                return -1;
            }
        }
    }
    return has_send_len;
}

