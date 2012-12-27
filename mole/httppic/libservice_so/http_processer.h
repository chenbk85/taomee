/**
 * =====================================================================================
 *       @file  http_processer.h
 *      @brief  处理HTTP请求的头文件
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  2008年10月22日 17时03分01秒 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee .Inc
 *   Copyright  Copyright (c) 2008, aceway
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef __HTTP_PROCESSER__
#define __HTTP_PROCESSER__

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <linux/tcp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include "../tcp_http/Common.h"
#include "../tcp_http/Log.h"
#include "./qdes.h"

#define HTTP_ENCRYPT            1

//允许一次接收的实体长度4k
#define HTTP_STATUS_OK                  " 200 OK"
#define HTTP_STATUS_CREATEED            " 201 Created"
#define HTTP_STATUS_ACCEPTED            " 202 Accepted"
#define HTTP_STATUS_NO_CONTENT          " 204 No Content"

#define HTTP_STATUS_MOVED_PER           " 301 Moved Permanently"
#define HTTP_STATUS_MOMED_TMP           " 302 Momed Temperarirly"
#define HTTP_STATUS_NOT_MODIFIED        " 304 Not Modified"

#define HTTP_STATUS_BAD_REQ             " 400 Bad Request"
#define HTTP_STATUS_UNAUTHORIZED        " 401 Unauthorized"
#define HTTP_STATUS_FORBIDDEN           " 403 Forbidden"
#define HTTP_STATUS_NOT_FOUND           " 404 Not Found" 
#define HTTP_STATUS_SERVER_ERROR        " 500 Internal Server Error"
#define HTTP_STATUS_NOT_IMP             " 501 Not Implemented"
#define HTTP_STATUS_BAD_GATEWAY         " 502 Bad Gateway"
#define HTTP_STATUS_SERVICE_UNAV        " 503 Service Unavailable"
#define RELOCATE_PORT_INT              4267 
#define RELOCATE_DEFAULT_GET_FILE_PATH "/opt/taomee/http_pic_server/images/"
#define RELOCATE_DEFAULT_GET_FILE_NAME "index.html"

#define RELOCATE_DEFAULT_SAVE_FILE_PATH  "/opt/taomee/http_pic_server/images/"
#define RELOCATE_DEFAULT_SAVE_FILE_NAME  "test.jpg"

#define TEST_RESPONSE_DATA "<html><head><title>TaoMeeHTTPServer</title></head><body>Can not find the data for you, check you URL please!</body></html> "

#define FLASH_CROSS_DOMAIN_XML  "<?xml version=\"1.0\"?><cross-domain-policy><allow-access-from domain=\"*\" /><allow-http-request-headers-from domain=\"*\" headers=\"*\"/></cross-domain-policy>"

#define HTTP_DEFAULT_CONTENT_TYPE               "text/html"

#define MAX_SAVING_SUBFOLDER_COUNT           10



#define FILE_BUFFER_SIZE 4096
#define SOCKET_BUFFER_SIZE      4096
#define MAX_CONTENT_LEN         (4*1024)
#define MAX_FILE_PATH_NAME_LEN  1024
#define MAX_FILE_NAME_LEN       255 
#define MAX_FILE_EXTENSION_NAME_LEN    16 
#define COMMEN_STR_LEN         255

#define DEFAULT_HTTP_KEY        "pIc:Svr@"
#define DEFAULT_URL_KEY         "*UrlKey$" 


/**
 * @brief 和协议对应的session结构体
 * 
 */
typedef struct __shttp_server_session{
    unsigned int user_id; /**< @brief 该session所属用户的ID */
    unsigned int operate_time; /**< @brief 用户获取session */
    unsigned int user_ip; /**< @brief 该Session所属用户的ip */
    unsigned int user_port; /**< @brief ？？？本服务器的端口号 */
}shttp_server_session;

/** 
 * @brief  HTTP协议结构体
 */
struct http_header
{
  bool is_valid;

  char method[20];
  char host[COMMEN_STR_LEN+1];
  char file_path_name[MAX_FILE_PATH_NAME_LEN+1];
  char file_path[MAX_FILE_PATH_NAME_LEN+1];
  char file_name[MAX_FILE_NAME_LEN+1];
  char file_extension_name[MAX_FILE_EXTENSION_NAME_LEN+1];

  char http_version[10];
  char date_pragma[COMMEN_STR_LEN+1];
  char authorization[COMMEN_STR_LEN+1];
  char from[COMMEN_STR_LEN+1];
  char if_modified_since[COMMEN_STR_LEN+1];
  char referer[COMMEN_STR_LEN+1];
  char user_agent[COMMEN_STR_LEN+1];

  char status_code[COMMEN_STR_LEN+1];
  char reason_phrase[MAX_FILE_PATH_NAME_LEN+1];

  char allow[COMMEN_STR_LEN+1];
  char content_encoding[COMMEN_STR_LEN+1];
  char content_length_char[16];
  int  content_length;
  char content_type[COMMEN_STR_LEN+1];
  char expires[COMMEN_STR_LEN+1];
  char last_modified[COMMEN_STR_LEN+1];

  char accept[COMMEN_STR_LEN+1];
  char accept_language[COMMEN_STR_LEN+1];
  char accept_encoding[COMMEN_STR_LEN+1];
  char accept_char_set[COMMEN_STR_LEN+1];
  char connection[COMMEN_STR_LEN];
          
  char language[10];
  char query_string[COMMEN_STR_LEN+1];
  char absolute_uri[MAX_FILE_PATH_NAME_LEN+1];
  char parameter[MAX_CONTENT_LEN+1];
  bool has_parameter;
  int  header_info_len;//HTTP头长度
  int  parameter_len;//第一次Receive可能没收全content-text

  char post_parameter[MAX_CONTENT_LEN+1];
  int  post_parameter_len;
  shttp_server_session session;
  unsigned int   user_id;
  char  user_ip[24];
  unsigned int user_port;
  bool bcheck_session;
};

void print_buffer(const char *pbuffer, int buffer_size, bool bhex=false);
void print_http_header(const http_header &header);

bool get_session_info(char* in_buffer, const unsigned short in_buffer_len, 
                      shttp_server_session &out_session_info);

bool make_session_info(const shttp_server_session in_session,
                       char* out_buffer, unsigned short &out_buffer_len);
                        

http_header parse_http_header(const char* phttp_message_body, const int buffer_size);

int process_request(int fd, http_header &header);
int response_get_method(int fd, http_header &header);
int response_post_method(int fd, http_header &header);
int response_unkown_method(int fd, http_header &header);

bool locate_get_load_file(http_header &header);
bool locate_post_save_file(http_header &header);
bool make_folders_in_dir(const char *pdirectory, const short folders_count);
void parse_file_path_name(const char *in_file_path_name,
                          char *out_path, char *out_name, 
                          char *out_ext_name, const int len); 

inline bool get_http_field_value(const char* http_header_buf, const char* field_name, 
                          char* field_value_buf, const int value_buf_len);

bool my_encrypt(char* key,  int key_len, 
                char *src,  int src_len, 
                char *dest, int &dest_len);


bool my_decrypt(char* key,  int key_len, 
                char *src,  int src_len, 
                char *dest, int dest_len);

bool check_session(const http_header &header);
#endif
