/**
 * =====================================================================================
 *       @file  http_processer.cpp
 *      @brief  处理http协议通讯：协议数据包，相应回应。目前只是现GET, POST方法
 *
 *  Detailed description starts here.
 *
 *   @internal
 *     Created  2008年10月22日 16时22分51秒 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee .Inc
 *   Copyright  Copyright (c) 2008, aceway
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include "./http_processer.h"
#include "./safe_socket.h"
#include "./base64.h"

/** 
 * @brief 按16进制码，或字符方式打印内存区域, 按字符打印可能遇到非打印字符, 屏幕会乱 
 * @param  const char *pbuffer, 要打印的内存区域起始地址 
 * @param  int buffer_size, 要打印的内存大小，调用者保证其合法性 
 * @param  bool bhex, 指定打印方式，true 按16进制码打印，false按字符打印 
 * @return  
 */
void print_buffer(const char *pbuffer, int buffer_size, bool bhex)
{
  printf("\r\n...START[\r\n");
  for(int i = 0; i < buffer_size; i++)
  {
    if( ((char*)pbuffer+i) == NULL )
      continue;
    if(bhex)
    {
        printf("[0x%-8x]", *((char*)pbuffer+i));
        if((1+i)%8==0)
            printf("\r\n");
    }
    else
    {
        printf("%c", *((char*)pbuffer+i));
    }
  }
  printf("\r\n]END...\r\n");
}

//打印HTTP协议头结构体

/** 
 * @brief  打印有内容的http_header结构体成员
 * @param  const http_header &header, 要打印的结构体 
 * @return void 
 */
void print_http_header(const http_header &header)
{
    printf("\r\n---------------http message info start--------------\r\n");

    printf("\tis valid: %d\r\n", header.is_valid); 
    if(strlen(header.method)>0)
    {
        printf("\tmethod: [%s] \r\n", header.method); 
    }

    printf("\theader info len=%d \r\n", header.header_info_len); 
    printf("\theader parameter len=%d \r\n", header.parameter_len); 

    if(strlen(header.absolute_uri))
    {
        printf("\tabsolute ulr: [%s] \r\n", header.absolute_uri);
    }
    if(strlen(header.host))
    {
        printf("\thost: [%s] \r\n", header.host); 
    }

    if(strlen(header.file_path_name))
    {
        printf("\tfile path name: [%s].\r\n", header.file_path_name); 
    }

    if(strlen(header.http_version))
    {
        printf("\thttp version: [%s] \r\n", header.http_version); 
    }
    if(strlen(header.date_pragma))
    {
        printf("\tdate or pragma: %s\r\n", header.date_pragma); 
    }
    if(strlen(header.authorization))
    {
        printf("\tauthorization: %s\r\n", header.authorization); 
    }
    if(strlen(header.from))
    {
        printf("\tfrom: %s\r\n", header.from); 
    }
    if(strlen(header.if_modified_since))
    {
        printf("\tif modified: %s\r\n", header.if_modified_since); 
    }
    if(strlen(header.user_agent))
    {
        printf("\tuser agent: %s\r\n", header.user_agent); 
    }
    if(strlen(header.referer))
    {
        printf("\treferer: %s\r\n", header.referer); 
    }

    if(strlen(header.status_code))
    {
        printf("\tstatus code: %s\r\n", header.status_code); 
    }
    if(strlen(header.reason_phrase))
    {
        printf("\treason phrase: %s\r\n", header.reason_phrase); 
    }

    if(strlen(header.allow))
    {
        printf("\tallow: %s\r\n", header.allow); 
    }
    if(strlen(header.content_encoding))
    {
        printf("\tcontent encoding: %s\r\n", header.content_encoding); 
    }
    if(strlen(header.content_length_char))
    {
        printf("\tcontent length:%s\r\n", header.content_length_char); 
    }
    if(strlen(header.content_type))
    {
        printf("\tcontent type: %s\r\n", header.content_type); 
    }
    if(strlen(header.expires))
    {
        printf("\texpires: %s\r\n", header.expires); 
    }
    if(strlen(header.last_modified))
    {
        printf("\tlast modified: %s\r\n", header.last_modified); 
    }
    if(strlen(header.language))
    {
        printf("\tlanguage: %s\r\n", header.language); 
    }
    if(strlen(header.accept))
    {
        printf("\taccept: %s\r\n", header.accept); 
    }
    if(strlen(header.accept_language))
    {
        printf("\taccept language:%s\r\n", header.accept_language); 
    }
    if(strlen(header.accept_encoding))
    {
        printf("\taccept encoding:%s\r\n", header.accept_encoding); 
    }
    if(strlen(header.accept_char_set))
    {
        printf("\taccept char set:%s\r\n", header.accept_char_set); 
    }
    if(strlen(header.connection))
    {
        printf("\tconnection: %s\r\n", header.connection);
    }
    if(strlen(header.parameter)>0)
    {
        printf("\tparameter: %s\r\n", header.parameter); 
    }
    if(strlen(header.query_string))
    {
        printf("\tquery string: %s\r\n", header.query_string); 
    }
    printf("\r\n---------------http message info over --------------\r\n");
}


/** 
 * @brief  从指定内存内获得http协议的field name的内容, 按字符串形式获得
 * @param  const char *http_header_buf, 保存http原始数据的内存区域
 * @param  const char *field_name, 要获得的field name 
 * @param  char *field_value_buf, 获得的内容值   
 * @param  const int value_buf_len, 保存内容值的缓存大小
 * @return bool类型，如果找到需要的field name，则返回true, 否则返回false 
 */
inline bool get_http_field_value(const char* http_header_buf, const char* field_name, 
                    char* field_value_buf, const int value_buf_len)
{
    assert(http_header_buf != NULL);
    assert(field_name != NULL);
    assert(field_value_buf != NULL);
    assert(value_buf_len > 0);
    memset(field_value_buf, 0, value_buf_len);

    if(field_name[0] == '\0') return false;
    if(field_name[0] == ':') return false;

    char *pfield_name_begin = strcasestr(http_header_buf, field_name);
    if(pfield_name_begin == NULL) return false;

    pfield_name_begin += strlen(field_name);
    while(pfield_name_begin != NULL && isspace(*pfield_name_begin))
    {
        pfield_name_begin++;
    }
    if( pfield_name_begin == NULL) return false;
    char *pfield_name_end = strchr(pfield_name_begin, '\r');
    if(pfield_name_end == NULL) pfield_name_end = strchr(pfield_name_begin, '\n');
    if(pfield_name_end == NULL) return false;

    int nfield_name_len = pfield_name_end - pfield_name_begin;
    int ncopy = nfield_name_len < value_buf_len? nfield_name_len : value_buf_len;
    
    memcpy(field_value_buf, pfield_name_begin, ncopy);
    field_value_buf[ncopy] ='\0';

    return true;
}

/** 
 * @brief  从http协议原始内容解析协议头数据，保存到http_header结构体，并返回该结构体
 * @param  const char *pHttpRequest, 保存http协议原始内容的内存   
 * @param  const int buffer_size, 保存http协议原始内容的内存大小 
 * @return  
 */
http_header parse_http_header(const char* pHttpRequest, const int buffer_size)
{
    //printf ("\n\n--------------start parse http header---------------\n");
    char *pRequest=new char[buffer_size+1];
    memset(pRequest, 0, buffer_size);
    memcpy(pRequest, pHttpRequest, buffer_size);
    pRequest[buffer_size]='\0';

    http_header header;
    memset(&header, 0, sizeof(http_header));

    char *pbegin=pRequest;
    char *pend = NULL;
    int nCount=0;
    //跳过前面有若干的空格。
    while(pbegin != NULL && isspace(*pbegin))
        pbegin++;
    assert(pbegin != NULL); //跳过空白

    //parse method name;
    pend=strchr(pbegin,' ');
    nCount=pend-pbegin;
    if(nCount>0) //解析出请求的方法
    {
        strncpy(header.method, pbegin,nCount);
        header.method[nCount]='\0';
        //printf("parse method ok: [%s]\n", header.method);
    }
    else
    {
        header.is_valid = false;
    }

    //GET, POST, PUT, HEAD, OPTIONS, DELETE, TRACE, CONNECT
    if(strcasestr(header.method, "get") != NULL || strcasestr(header.method, "post") != NULL )
    {//GET, POST, PUT, HEAD, OPTIONS, DELETE, TRACE, CONNECT
        //存在获取的方法
        header.is_valid = true;
        pbegin = pend;
        while(pbegin != NULL && isspace(*pbegin))//跳过空白
            pbegin++;
        assert(pbegin != NULL);

        pend = strchr(pbegin, ' ');
        assert(pend != NULL);
        nCount = pend - pbegin;
        
        //解析出文件路径名
        if(nCount>0)
        {
            //解密路径信息
            char file_path_name[MAX_FILE_PATH_NAME_LEN]={0};
            strncpy(file_path_name, pbegin, nCount);
            file_path_name[nCount] = '\0';
            strcpy(header.file_path_name, file_path_name);
            //printf("\r\nfile path name:[%s]\r\n", header.file_path_name);
        }

        pbegin = pend;

        //解析出版本号
        while(pbegin != NULL && isspace(*pbegin))//跳过空白
            pbegin++;
        pend = strchr(pbegin, '\n');
        assert(pend != NULL);
        nCount = pend - pbegin;
        if(nCount>0)
        {
            strncpy(header.http_version, pbegin, nCount); 
            header.http_version[nCount-1] = '\0';
            char *p = strchr(header.http_version, '\r');
            if(p != NULL)
                *p = '\0';
            p = strchr(header.http_version, '\n');
            if(p !=NULL)
                *p = '\0';
            //printf("parse http version ok: [%s] \n", header.http_version);
        }
        pbegin = pend;
        //request-line parsed over


        pend = strchr(pbegin, '\n');
        assert(pend != NULL);
        nCount = pend - pbegin;

        //如果nCount大于2， 则说明至少存在一个header

        int file_path_name_len = strlen(header.file_path_name);
        //printf("file path name len=%d.\n", file_path_name_len);
        int nTo = 0;          
        int n = 0;
        //parse query string
        pbegin = strchr(header.file_path_name, '?');
        if(pbegin != NULL) //解析出query_string
        {
            nTo = pbegin - header.file_path_name;
            nTo++;
            n = 0;
            while(nTo <  file_path_name_len )
            {
                header.query_string[n] = header.file_path_name[nTo];
                nTo++;
                n++;
                if(n>=MAX_FILE_PATH_NAME_LEN) break;
            }
        } //if(pbegin != NULL) //解析出query_string						


        n = 0; 
        //full request line

        //General-header | Request-header | Entity=header
        char *pfield_name_start = strstr(pRequest, "\n");
        if(pfield_name_start != NULL)
        {
            pfield_name_start += strlen("\n");
        }
        else
        {
            //printf("can not find the end of request line!\n");
        }
        if(pfield_name_start != NULL)  //存在Header
        {
            get_http_field_value(pfield_name_start, "date:", header.date_pragma, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "authorization:", header.authorization, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "from", header.from, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "if-modified-since:", header.if_modified_since, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "referer:", header.referer, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "user-agent:", header.user_agent, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "allow:", header.allow, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "content-encoding:", header.content_encoding, COMMEN_STR_LEN);

            if(get_http_field_value(pfield_name_start, "content-length:", header.content_length_char, 16))
            {
                header.content_length = atoi(header.content_length_char);
            }
            else
            {
                header.content_length = 0;
            }

            get_http_field_value(pfield_name_start, "content-type:", header.content_type, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "expires:", header.expires, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "last-modified:", header.last_modified, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "accept:", header.accept, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "accept-language:", header.accept_language, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "accept-encoding:", header.accept_encoding, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "accept-charset:", header.accept_char_set, COMMEN_STR_LEN);

            char user_id[32] = {'\0'};
            if(get_http_field_value(pfield_name_start, "user-id:", user_id, 32))
            {
                header.user_id = atoi(user_id);
            }
            else
            {
                header.user_id = 0;
            }

            get_http_field_value(pfield_name_start, "connection:", header.connection, COMMEN_STR_LEN);

            get_http_field_value(pfield_name_start, "host:", header.host, COMMEN_STR_LEN);

            sprintf(header.absolute_uri, "http://");
            strcat(header.absolute_uri, header.host);
            strcat(header.absolute_uri, header.file_path_name);
        }
        //以上是解析消息头部分
       
    } //if(strlen(header.method) > 0)
    else
    {
        delete [] pRequest;
        pRequest = NULL;
        header.is_valid=false;
        return header;
    }
    delete[] pRequest;
    pRequest = NULL;

    return header;
}

/** 
 * @brief  处理http请求
 *
 * @param  int fd, 发送http请求的socket描述符，将在该fd上处理，回应请求   
 * @param  http_header &header, 从fd上已经解析出来的http协议头 
 *
 * @return int类型，错误返回－1，返回相应的处理码 
 */
int process_request(int fd, http_header &header)
{
    //GET, POST, PUT, HEAD, OPTIONS, DELETE, TRACE, CONNECT
    int nret = -1;
    if(strcasecmp(header.method, "get") == 0)
    {
        nret = response_get_method(fd, header);    //3.1----处理GET方法
    }
    else if(strcasecmp(header.method, "post") == 0)
    {
        nret = response_post_method(fd, header);   //3.2---POST方法
    }
    else if(strcasecmp(header.method, "put") == 0)
    {
        nret = response_unkown_method(fd, header); //3.3---处理未实现的方法
    }
    else if(strcasecmp(header.method, "head") == 0)
    {
        nret = response_unkown_method(fd, header); //3.3---处理未实现的方法
    }
    else if(strcasecmp(header.method, "options") == 0)
    {
        nret = response_unkown_method(fd, header); //3.3---处理未实现的方法
    }
    else if(strcasecmp(header.method, "delete") == 0)
    {
        nret = response_unkown_method(fd, header); //3.3---处理未实现的方法
    }
    else if(strcasecmp(header.method, "trace") == 0)
    {
        nret = response_unkown_method(fd, header); //3.3---处理未实现的方法
    }
    else if(strcasecmp(header.method, "connect") == 0)
    {
        nret = response_unkown_method(fd, header); //3.3---处理未实现的方法
    }
    else
    {
        nret = response_unkown_method(fd, header); //3.3---处理未实现的方法
    }
    return nret;
}

/**
 * @brief  回应GET方法，包括加载GET的文件，按HTTP协议组织数据，发送文件 
 * @param  int fd, 请求的客户端Socket描述符，在该fd上回应
 * @param  http_header $header, 已经解析好的GET请求数据 
 * @return 错误码，正确返回0， 错误返回－1 
 */
int response_get_method(int fd, http_header &header)
{
    int ret_code = -1;
    if(header.parameter_len < header.content_length)
    {
        int tmp_len = -1;
        int rest_len = header.content_length - header.parameter_len;
        tmp_len = safe_recv_len(fd, header.parameter + header.parameter_len, MAX_CONTENT_LEN - header.parameter_len,  rest_len); 
        if(tmp_len == -1)
        {
            LOG(DEBUG, "recv ret %d! %s, %d\n", tmp_len, __FILE__, __LINE__);
            ret_code = -1;
        }
        else
        {
            header.parameter_len += tmp_len;
        }
    }
    char buffer[SOCKET_BUFFER_SIZE];
    memset(buffer,'0',  SOCKET_BUFFER_SIZE); 

    char *pwrite_begin = buffer;
    int write_len = 0;

    //根据请求定位,加载文件数据
    bool is_locate_load_file_ok = locate_get_load_file(header);

    char http_version[10];
    memset(http_version, '0', 10);
    strncpy(http_version, header.http_version, 9);
    http_version[9] = '\0';

    write_len = sprintf(pwrite_begin, "%s", http_version);
    pwrite_begin += write_len;
    assert(pwrite_begin!= NULL);

    //状态码和描述信息
    char status_code[255];
    sprintf(status_code, "%s\r\n", header.status_code);
    write_len = sprintf(pwrite_begin, "%s\r\n", header.status_code);
    pwrite_begin += write_len;
    assert(pwrite_begin!= NULL);
    //写实体头信息

    //General-Header
    //日期信息
    time_t current_time = time(NULL);
    char *sz_current_time = ctime(&current_time);
    char *p = strchr(sz_current_time, '\n');
    if(p!=NULL)
        *p = '\0';
    p = strchr(sz_current_time, '\r');
    if(p!= NULL)
        *p = '\0';
    write_len = sprintf(pwrite_begin, "Date: %s\r\n", sz_current_time);
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    
    //Entity-Header

    //实体数据类型
    if(header.accept[0] == '\0')//请求中没有说明客户端可接收的媒体类型，则发送默认的文本类型
    {
        write_len = sprintf(pwrite_begin, "Content-Type: %s\r\n", HTTP_DEFAULT_CONTENT_TYPE); 
    }
    else//客户端请求时，指明了它可接收的媒体类型
    {
        if(header.file_extension_name[0] != '\0') //如果请求的资源有扩展名
        {
            char *ptmp = strcasestr(header.accept, header.file_extension_name);
            if(ptmp != NULL)//通过扩展名判断，是客户单接受的媒体类型
            {
                write_len = sprintf(pwrite_begin, "Content-Type: %s\r\n", header.accept); 
            }
            else//通过扩展名判断不出， 则当应用程序类型发送
            {
                write_len = sprintf(pwrite_begin, "Content-Type: application/%s\r\n", 
                        header.file_extension_name); 
            }
        }
        else//请求资源没有扩展名
        {
            //当做默认的文本类型发送
            write_len = sprintf(pwrite_begin, "Content-Type: %s\r\n", HTTP_DEFAULT_CONTENT_TYPE); 
        }
    }
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    //实体语言信息
    if(header.accept_language != NULL && header.accept_language[0] != '\0')
    {
        write_len = sprintf(pwrite_begin, "Content-Language: %s\r\n", header.accept_language); 
        pwrite_begin += write_len;
        assert(pwrite_begin != NULL);
    }
    else
    {
        write_len = sprintf(pwrite_begin, "Content-Language: zh-cn\r\n" ); 
        pwrite_begin += write_len;
        assert(pwrite_begin != NULL);
    }

    //实体长度
    if(is_locate_load_file_ok)
    {
        write_len = sprintf(pwrite_begin, "Content-Length: %d\r\n", header.content_length); 
    }
    else
    {
        write_len = sprintf(pwrite_begin, "Content-Length: %d\r\n", strlen(TEST_RESPONSE_DATA)); 
    }
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);
//GET method
    write_len = sprintf(pwrite_begin, "Server: TaoMee-HTTP-Server.\r\n");
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    write_len = sprintf(pwrite_begin, "Cache-Control: no-store, no-cache\r\n");
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    //关闭连接，防止连接过多，
    write_len = sprintf(pwrite_begin, "Connection: close\r\n");
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);


    //结束消息头部分
    write_len = sprintf(pwrite_begin, "\r\n"); 
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    int header_len = pwrite_begin - buffer;
    int send_len = safe_send_len(fd, buffer, SOCKET_BUFFER_SIZE, header_len); 

    LOG(NORMAL, "user ip:%s, get file: %s\n", 
                 header.user_ip, header.file_path_name);  
    if(is_locate_load_file_ok)
    {//文件存在，可访问
        int file_fd = open(header.file_path_name, O_RDONLY);

        send_len = 0;
        if(file_fd >= 0)
        {//打开文件成功
            send_len = safe_sendfile(file_fd, fd);
            close(file_fd);
        }
        else //打开文件失败
        {
            LOG(ERROR, "sendfile call failed! positoin, ifno: [%s]\n", strerror(errno));
            write_len = sprintf(buffer, "%s\r\n", TEST_RESPONSE_DATA); 
            send_len = safe_send_len(fd, buffer, SOCKET_BUFFER_SIZE, write_len);
        }
    }
    else
    {//文件不存在，或者不可访问
        write_len = sprintf(buffer, "%s\r\n", TEST_RESPONSE_DATA); 
        send_len = safe_send_len(fd, buffer, SOCKET_BUFFER_SIZE, write_len);
    }

    //printf("send message's header info:\n");
    if(fd)
    {
        char *pclose = strcasestr(header.connection, "close");
        if(pclose != NULL)
        {
            ret_code = -1;
        }
    }
    return ret_code;
}

/** 
 * @brief 处理POST请求，包括接收数据，保存数据，按HTTP协议回应客户端 
 * @param  int fd, 请求的客户端Socket描述符，在该fd上回应
 * @param  http_header $header, 已经解析好的POST请求数据 
 * @return 错误码，正确返回0， 错误返回－1 
 */
int response_post_method(int fd, http_header &header)
{
    int ret_code = -1;
    //定位保存文件的位置
    int file_fd = -1;
    int file_write_len = 0;
    bool is_locate_save_file_ok = false;
    bool bsession = false; 
    int session_len = sizeof(shttp_server_session);

    if(header.content_length > 0) //有数据内容
    {
        if(header.parameter_len < session_len)
        {
            if(header.parameter_len < 0) header.parameter_len = 0;
            int tmp_rest_len =  session_len - header.parameter_len;
            int tmp_recv_len = safe_recv_len(fd, header.parameter + header.parameter_len, session_len, tmp_rest_len);
            if(tmp_recv_len > 0) header.parameter_len += tmp_recv_len;
        }
        //上面保证session信息收取完毕，header.parameter中只有上传的文件数据, header.parameter_len相应
        get_session_info(header.parameter, session_len, header.session);

        //检验session
        bsession = check_session(header);
        header.bcheck_session = bsession;

        is_locate_save_file_ok = locate_post_save_file(header);

        //打开文件准备写

        if(is_locate_save_file_ok)
        {
            file_fd = open(header.file_path_name, O_CREAT|O_RDWR, 0755);
            if(file_fd >= 0 )
            {
                int tmp_write_len = 0;
                if(header.parameter_len > session_len)
                {
                    tmp_write_len = safe_write_len(file_fd, header.parameter + session_len, 
                            MAX_CONTENT_LEN - session_len, header.parameter_len - session_len);

                    if(tmp_write_len > 0) file_write_len += tmp_write_len;
                }
                int rest_write_len = header.content_length - header.parameter_len;
                tmp_write_len = save_data_from_socket(fd, file_fd, rest_write_len);
                if(tmp_write_len > 0) file_write_len += tmp_write_len;

                LOG(NORMAL, "user id:%d, ip:%s, post open file OK!: %s\n", 
                        header.user_id, header.user_ip, header.file_path_name);  
                close(file_fd);
            }
            else
            {
                LOG(ERROR, "user id:%d, ip:%s, post open file failed!: %s\n", 
                        header.user_id, header.user_ip, header.file_path_name);  
            }
        }
        else
        {
            LOG(ERROR, "user id:%d, ip:%s, post file failed for session wrong!\n", header.user_id, header.user_ip);  
        }
    }//if(header.content_length > 0);
    else
    {
    }

    //写返回信息头
    //printf("start to response %s method\n", header.method);
    char buffer[SOCKET_BUFFER_SIZE];
    memset(buffer,'0',  SOCKET_BUFFER_SIZE); 

    char *pwrite_begin = buffer;
    int write_len = 0;

    //version
    char http_version[10];
    strncpy(http_version, header.http_version, 9);
    http_version[9] = '\0';

    //printf("start to write http version: %s\n", http_version);
    write_len = sprintf(pwrite_begin, "%s", http_version);
    pwrite_begin += write_len ;
    assert(pwrite_begin!= NULL);

    //status-code, reason-phrase
    char status_code[255];
    if(is_locate_save_file_ok && file_fd >= 0)
    {
        sprintf(status_code, "%s\r\n", HTTP_STATUS_OK);
        write_len = sprintf(pwrite_begin, "%s\r\n", HTTP_STATUS_OK);
    }
    else
    {
        sprintf(status_code, "%s\r\n", HTTP_STATUS_NOT_FOUND);
        write_len = sprintf(pwrite_begin, "%s\r\n", HTTP_STATUS_NOT_FOUND);
    }   
    pwrite_begin += write_len;
    assert(pwrite_begin!= NULL);

    //general-header
    //date
    time_t current_time = time(NULL);
    char *sz_current_time = ctime(&current_time);
    char *p = strchr(sz_current_time, '\n');
    if(p!=NULL)
        *p = '\0';
    p = strchr(sz_current_time, '\r');
    if(p!= NULL)
        *p = '\0';
    write_len = sprintf(pwrite_begin, "Date: %s\r\n", sz_current_time);
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    //POST method
    write_len = sprintf(pwrite_begin, "Server: TaoMee-HTTP-Server.\r\n");
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    //entity-header
    //content-language
    if(header.accept_language != NULL && header.accept_language[0] != '\0')
    {
        write_len = sprintf(pwrite_begin, "Content-Language: %s\r\n", header.accept_language); 
        pwrite_begin += write_len;
        assert(pwrite_begin != NULL);
    }
    else
    {
        write_len = sprintf(pwrite_begin, "Content-Language: zh-cn\r\n" ); 
        pwrite_begin += write_len;
        assert(pwrite_begin != NULL);
    }

    //content-length
    if(header.post_parameter_len > 0)
    {
        write_len = sprintf(pwrite_begin, "Content-Length: %d\r\n", header.post_parameter_len); 
        pwrite_begin += write_len;
        assert(pwrite_begin != NULL);
    }
    else
    {
        write_len = sprintf(pwrite_begin, "Content-Length: 0\r\n" ); 
        pwrite_begin += write_len;
        assert(pwrite_begin != NULL);
    }

    //content-type
    if(header.content_type[0] == '\0')
    {
        char *pfile_exten = strcasestr(header.file_name, ".htm");
        if(pfile_exten == NULL)
        {
            write_len = sprintf(pwrite_begin, "Content-Type: %s\r\n", HTTP_DEFAULT_CONTENT_TYPE); 
        }
        else
        {
            write_len = sprintf(pwrite_begin, "Content-Type: text/html\r\n"); 
        }
    }
    else
    {
        write_len = sprintf(pwrite_begin, "Content-Type: text/html\r\n"); 
    }
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    //response-header
    if(strlen(header.file_path_name) > 1)
    {
        write_len = sprintf(pwrite_begin, "Location: %s\r\n", header.host);
    }
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    //告诉客户端，这次post服务端成功接收了多少内容数据
    write_len = sprintf(pwrite_begin, "Save-DataLen: %d\r\n", file_write_len);
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    write_len = sprintf(pwrite_begin, "Cache-Control: no-store, no-cache\r\n");
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    //关闭连接，防止连接过多，
    write_len = sprintf(pwrite_begin, "Connection: close\r\n");
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    //结束消息头部分
    write_len = sprintf(pwrite_begin, "\r\n"); 
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    int header_len = pwrite_begin - buffer;
    int send_len = 0;

    send_len = safe_send_len(fd, buffer, SOCKET_BUFFER_SIZE, header_len);
    if(header.post_parameter_len > 0)
    {
        send_len += safe_send_len(fd, header.post_parameter, MAX_CONTENT_LEN,  header.post_parameter_len);
    }

    if(fd)
    {
        char *pclose = strcasestr(header.connection, "close");
        if(pclose != NULL)
        {
            ret_code = -1;
        }
    }
    return ret_code;
}

/** 
 * @brief 处理未实现的请求方法
 * @param  int fd, 请求的客户端Socket描述符，在该fd上回应
 * @param  http_header $header, 已经解析好的GET请求数据 
 * @return 错误码，正确返回0， 错误返回－1 
 */
int response_unkown_method(int fd, http_header &header)
{

    int ret_code = -1;
    char buffer[SOCKET_BUFFER_SIZE];
    memset(buffer,'0',  SOCKET_BUFFER_SIZE); 

    LOG(ERROR, "\n\nstart to response unkown method, or not implement method [%s]\n", header.method);
    char *pwrite_begin = buffer;
    int write_len = 0;

    //发送的消息体部分
    //版本
    if(header.http_version[0] == '\0' || strlen(header.http_version) <= 4)
    {
        sprintf(header.http_version, "HTTP/1.1  "); 
    }
    char http_version[10];
    memset(http_version, '0', 10);
    strncpy(http_version, header.http_version, 9);
    char *p = strchr(http_version, '\n');
    if(p != NULL)
        *p = '\0';
    p = strchr(http_version, '\r');
    if(p != NULL)
        *p = '\0';
    write_len = sprintf(pwrite_begin, "%s ", http_version);
    pwrite_begin += write_len;
    assert(pwrite_begin!= NULL);

    //状态码和描述信息
    write_len = sprintf(pwrite_begin, "%s \r\n", HTTP_STATUS_NOT_IMP);
    pwrite_begin += write_len;
    assert(pwrite_begin!= NULL);

    //写实体头信息

    //General-Header
    //日期信息
    time_t current_time = time(NULL);
    char *sz_current_time = ctime(&current_time);
    p = strchr(sz_current_time, '\n');
    if(p!=NULL)
        *p = '\0';
    p = strchr(sz_current_time, '\r');
    if(p!= NULL)
        *p = '\0';
    write_len = sprintf(pwrite_begin, "Date: %s \r\n", sz_current_time);
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    write_len = sprintf(pwrite_begin, "Content-Length: 0\r\n"); 
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    //实体语言信息
    //printf("start to write conten language: %s\n", header.accept_language); 
    if(header.accept_language != NULL && header.accept_language[0] != '\0')
    {
        write_len = sprintf(pwrite_begin, "Content-Language: %s\r\n", header.accept_language); 
        pwrite_begin += write_len;
        assert(pwrite_begin != NULL);
    }
    else
    {
        write_len = sprintf(pwrite_begin, "Content-Language: zh-cn\r\n" ); 
        pwrite_begin += write_len;
        assert(pwrite_begin != NULL);
    }
    //unkown method
    write_len = sprintf(pwrite_begin, "Server: TaoMee-HTTP-Server.\r\n");
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    //结束消息头部分
    //printf("start write to end header symbol!\n"); 
    write_len = sprintf(pwrite_begin, "\r\n"); 
    pwrite_begin += write_len;
    assert(pwrite_begin != NULL);

    write_len = pwrite_begin - buffer;
    send(fd, buffer, write_len, 0);

    if(fd)
    {
        ret_code = -1;
        //printf("\n\n--------------------------------------------------------\n");
        //printf(   "*****response not implement message over! close the socket!*******\n");
        //printf(    "--------------------------------------------------------\n");
    }
    return ret_code;
}

//根据文件路径名定位到文件, 并判断有没读权限

/** 
 * @brief  更具http_header数据的file_path_name定位文件，并准备回应GET方法的失败解释
 * @param  int fd, 请求的客户端Socket描述符，在该fd上回应
 * @param  http_header $header, 已经解析好的GET请求数据 
 * @return 文件定位成功返回true, 失败返回false 
 */
bool locate_get_load_file(http_header &header)
{
    //请求策略文件时，特殊处理
    char *pCrossDomain = NULL;
    pCrossDomain = strcasestr(header.file_path_name, "crossdomain.xml");

    char tmp_file_path_name[MAX_FILE_PATH_NAME_LEN]={0};
    //如果Get请求中有content-text那么，它是路径信息，解压后覆盖由 方法名后解析出来的数据
    if(header.parameter_len >0 && pCrossDomain == NULL)
    {
#ifdef HTTP_ENCRYPT
        my_decrypt(DEFAULT_URL_KEY, strlen(DEFAULT_URL_KEY),
                   header.parameter + 24, header.parameter_len - 24, 
                   tmp_file_path_name, MAX_FILE_PATH_NAME_LEN);

#else
        memcpy(tmp_file_path_name, header.parameter + 24, header.parameter_len -24);
#endif
        tmp_file_path_name[MAX_FILE_PATH_NAME_LEN] = '\0';
        parse_file_path_name(tmp_file_path_name, header.file_path, 
                header.file_name, header.file_extension_name, MAX_FILE_PATH_NAME_LEN);
    }
    if(strlen(header.file_name)==0 && strlen(header.query_string) > 0 && pCrossDomain == NULL)
    {
#ifdef HTTP_ENCRYPT
        char tmp_query_string[MAX_FILE_PATH_NAME_LEN];
        my_decrypt(DEFAULT_URL_KEY, strlen(DEFAULT_URL_KEY),
                   header.query_string, strlen(header.query_string),
                   tmp_query_string, MAX_FILE_PATH_NAME_LEN);
        strcpy(header.query_string, tmp_query_string);
#endif
        header.query_string[MAX_FILE_PATH_NAME_LEN] = '\0';
        parse_file_path_name(header.query_string, header.file_path, 
                header.file_name, header.file_extension_name, MAX_FILE_PATH_NAME_LEN);
        if(strlen(header.file_name) > 0)
        {
            strcpy(header.file_path_name, header.query_string);
        }
    }
    if(strlen(header.file_name)==0 && strlen(header.file_path_name) > 0 && pCrossDomain == NULL)
    {
#ifdef HTTP_ENCRYPT
        char tmp_file_path_name[MAX_FILE_PATH_NAME_LEN];
        my_decrypt(DEFAULT_URL_KEY, strlen(DEFAULT_URL_KEY),
                   header.file_path_name, strlen(header.file_path_name), 
                   tmp_file_path_name, MAX_FILE_PATH_NAME_LEN);
        strcpy(header.file_path_name, tmp_file_path_name);
#endif
        header.file_path_name[MAX_FILE_PATH_NAME_LEN] = '\0';
        parse_file_path_name(header.file_path_name, header.file_path, 
                header.file_name, header.file_extension_name, MAX_FILE_PATH_NAME_LEN);
    }
    if(pCrossDomain != NULL)
    {
        sprintf(header.file_path_name, "%scrossdomain.xml", RELOCATE_DEFAULT_GET_FILE_PATH );
        header.file_path_name[MAX_FILE_PATH_NAME_LEN] = '\0';
        parse_file_path_name(header.file_path_name, header.file_path, 
                header.file_name, header.file_extension_name, MAX_FILE_PATH_NAME_LEN);
    }
    else
    {
    }

    header.content_length = 0;
    sprintf(header.content_length_char, "%d", header.content_length);
    if(header.file_name[0] == '\0')//没有文件名是，去访问默认的文件夹下默认的文件名
    { 
        sprintf(header.absolute_uri, "http://");

        char host_no_port[COMMEN_STR_LEN];
        char *pend = strchr(header.host, ':');
        int nlen = 0;
        if(pend != NULL)
        {
            nlen = pend - header.host;
        }
        if(nlen > 0)//有端口时， 过滤掉端口
        {
            strncpy(host_no_port, header.host, nlen);
        }
        else//没端口, 即默认为80端口时
        {
            strcat(host_no_port, header.host);
        }
        //上面去掉端口号，下面补上本地的端口号

        sprintf(header.file_name, "%s", RELOCATE_DEFAULT_GET_FILE_NAME); 
        sprintf(header.file_path_name, "%s%s", RELOCATE_DEFAULT_GET_FILE_PATH, header.file_name);
        sprintf(header.absolute_uri, "http://%s:%d%s", 
                host_no_port, RELOCATE_PORT_INT, header.file_path_name);
    }
    else//访问指定的文件
    {
    }

    int read_ok = access(header.file_path_name, R_OK);
    if(read_ok == 0)
    {
        struct stat file_state;
        int stat_ok = stat(header.file_path_name, &file_state);
        if(stat_ok == 0)
        {
            sprintf(header.status_code, "%s", HTTP_STATUS_OK); 
            header.content_length = file_state.st_size;
            sprintf(header.content_length_char, "%s", header.content_length_char);
            return true;
        }
        else
        {
            sprintf(header.parameter, "\n\ncheck file: %s state failed! \n", header.file_path_name);
            sprintf(header.status_code, "%s", HTTP_STATUS_NOT_FOUND); 
            header.content_length = 0;
            sprintf(header.content_length_char, "%s", header.content_length_char);
            return false;
        }
    }
    else
    {
        sprintf(header.parameter, 
                "\n\nopen file: %s failed! maybe it does not exist, or you have no permission to access it!\n\n", 
                header.file_path_name);
        sprintf(header.status_code, "%s", HTTP_STATUS_NOT_FOUND); 
        header.content_length = 0;
        sprintf(header.content_length_char, "%s", header.content_length_char);
        return false;
    }
    return true;
}

/** 
 * @brief  为POST方法上传定位存储路径(含生成文件明),然后将加密的路径名存储到header.post_parameter 
 * @param  http_header $header, 已经解析好的POST请求数据 
 * @return 文件定位成功返回true, 失败返回false 
 */
bool locate_post_save_file( http_header &header)
{
    if(header.bcheck_session == false)
    {
        int len = sprintf(header.post_parameter, "msg=Session ERROR!");
        header.post_parameter_len = len;
        return false;
    }
    struct timeval tm;
    int subfolder_number= 0;
    gettimeofday(&tm, NULL);
    srand(tm.tv_usec);
    subfolder_number = rand()%MAX_SAVING_SUBFOLDER_COUNT;
    if(header.session.operate_time == 0)
    {
        sprintf(header.file_name, "%u_%u_%u.jpg", header.session.user_id, header.user_port, header.session.operate_time); 
    }
    else
    {
        sprintf(header.file_name, "%u_%u.jpg", header.session.user_id, header.session.operate_time); 
    }

    sprintf(header.file_path, "%s%d/", RELOCATE_DEFAULT_SAVE_FILE_PATH, subfolder_number);
    sprintf(header.file_path_name, "%s%s", header.file_path, header.file_name);

    char host_no_port[COMMEN_STR_LEN];
    char *pend = strchr(header.host, ':');
    int nlen = 0;
    if(pend != NULL)
    {
        nlen = pend - header.host;
    }
    if(nlen > 0)//有端口时， 过滤掉端口
    {
        strncpy(host_no_port, header.host, nlen);
    }
    else//没端口, 即默认为80端口时
    {
        strcat(host_no_port, header.host);
    }

    memset(header.post_parameter, ' ', MAX_CONTENT_LEN);
#ifdef HTTP_ENCRYPT
    //保存后，路径需要加密
    sprintf(header.post_parameter, "%s", header.host);
    int len_raw = strlen(header.file_path_name);
    int len_ret = MAX_FILE_PATH_NAME_LEN;
    my_encrypt(DEFAULT_URL_KEY, strlen(DEFAULT_URL_KEY),
               header.file_path_name, len_raw, 
               header.post_parameter+24, len_ret);
    header.post_parameter_len = 24 + len_ret;
    header.post_parameter[header.post_parameter_len] = '\0';
#else
    sprintf(header.post_parameter, "%s", header.host);
    int len = sprintf(header.post_parameter+24, "%s", header.file_path_name);
    header.post_parameter_len = 24 + len;
#endif
    return true;
}

//在指定路径下建立多个子文件夹（用于上传时存储）

/** 
 * @brief  在指定的路径下建立N个子目录，子目录明知是0,1,2,...,N
 * @param  const char *pdirectory, 指定的路径 
 * @param  const short folders_count, 建立子目录数目
 * @return 成功返回true, 失败返回false 
 */
bool make_folders_in_dir(const char *pdirectory, const short folders_count)
{
  int read_ok = access(pdirectory, F_OK|R_OK|W_OK);
  if(read_ok == 0)
  {
    int n = 0;
    const int MAX_PATH_LEN = 1024;
    char file_path_name[MAX_PATH_LEN]={0};
    for(n = 0; n < folders_count; ++n)
    { 
      memset(file_path_name, 0, MAX_PATH_LEN);
      sprintf(file_path_name, "%s/%d", pdirectory, n);
      int nret = mkdir(file_path_name, 0777); 
      if(nret != 0)
      { 
        if(errno != 17)
        {
          LOG(ERROR, "create folder [%s] failed! error %d, info: %s.\r\n", file_path_name, errno, strerror(errno));
        }
      }
    }
  }
  else
  {
      //printf("\n\ncheck file: %s state failed! Maybe not exist, or you have no permission!\n", pdirectory);
  }
  return true;
}

//
//

/** 
 * @brief 获取session号 
 *
 * 从网络上获取Session数据，扔进该函数，本函数负责解密，然后将数据按协议解析，
 * 然后将字节顺序从网络顺序转换成本机顺序
 *
 * @param  char *in_buffer, 包含网络上接收的原始session数据内存 
 * @param  const unsigned short  in_buffer_len, 存原始session数据的内存大小 
 * @param  shttp_server_session &out_session_info, 解析好的本机顺序的session
 * @return 解析session成功返回true, 失败返回false 
 */
bool get_session_info(char *in_buffer, const unsigned short in_buffer_len, 
                        shttp_server_session &out_session_info)
{
//    //printf("*************session before decrypt:********* \r\n");
//    print_buffer(in_buffer, in_buffer_len);

    shttp_server_session tmp_session;
    memset(&tmp_session, 0, sizeof(shttp_server_session));
#ifdef HTTP_ENCRYPT
    decrypt_data(DEFAULT_HTTP_KEY, in_buffer_len, in_buffer, &tmp_session); 
#else
    memcpy(&tmp_session, in_buffer, in_buffer_len);
#endif
//    //printf("*************session after decrypt:******** \r\n");
//    print_buffer((char*)&tmp_session, in_buffer_len);

    out_session_info.user_id = ntohl(tmp_session.user_id);
    out_session_info.operate_time = ntohl(tmp_session.operate_time);
    out_session_info.user_ip = ntohl(tmp_session.user_ip);
    out_session_info.user_port = ntohl(tmp_session.user_port);

    return true;
}

//
//

/** 
 * @brief  生成并加密session
 *
 * 输入：填好的sesshion结构体， 按本机字节顺序
 * 输出：加密了的，网络字节顺序的session结构体，放在了指定的缓冲区
 *
 * @param  const shttp_server_session in_session, 调好的sesssion结构体 
 * @param  char *out_buffer, 存有加密好的session数据的内存
 * @param  unsigned short &out_buffer_len, 加密了的数据内存大小
 * @return 成功返回true, 失败返回false. 
 */
bool make_session_info(const shttp_server_session in_session,
                       char* out_buffer, unsigned short &out_buffer_len)
{
    shttp_server_session tmp_session;
    memset(&tmp_session, 0, sizeof(shttp_server_session));

    tmp_session.user_id = htonl(in_session.user_id);
    tmp_session.operate_time = htonl(in_session.operate_time);
    tmp_session.user_ip = htonl(in_session.user_ip); 
    tmp_session.user_port= htonl(in_session.user_port); 

    unsigned short len = 0;
    len = out_buffer_len > sizeof(in_session) ? sizeof(in_session) : out_buffer_len;
    decrypt_data(DEFAULT_HTTP_KEY, len, &tmp_session, out_buffer); 
    return true;
}


/** 
 * @brief  对提供的全路径名进行解析，解析出路径，文件名，文件爱你扩展名
 * @param  const *in_file_path_name, 要解析的文件路径名 
 * @param  char *out_path, 解析出来的纯路进 
 * @param  char *out_name, 解析出来的文件名字（含扩展名，如果有） 
 * @param  char *out_ext_name, 解析出来的扩展名 
 * @param  const int len, 上面几个内存中最小的一个
 * @return void 
 */
void parse_file_path_name(const char *in_file_path_name,
                          char *out_path, char *out_name, 
                          char *out_ext_name, const int len) 
{
    assert(out_path != NULL);
    assert(out_name != NULL);
    assert(out_ext_name != NULL);
    out_path[0]='\0';
    out_name[0] = '\0';
    out_ext_name[0]='\0';
    int file_path_name_len = strlen(in_file_path_name);
    if(file_path_name_len == 0)
    {
        return;
    }
    assert(len > file_path_name_len);
    int nFrom = 0 ;
    int nTo = 0;
    int nIndex = 0;
    if(in_file_path_name[file_path_name_len-1] == '/')
    {//只有路径，没有指定文件名
        //printf("in the file path name no pointed file name, just directory.\n");
        strcpy(out_path, in_file_path_name);
        out_name[0] = '\0';
        out_ext_name[0] = '\0';
    }
    else
    {//指定了文件名
        //printf("in the file path name has pointed the wanted directory file name.\n");
        nFrom = nTo = file_path_name_len-1;
        while(in_file_path_name[nFrom] != '/')
        {
            if(nFrom == 0) break;
            nFrom--;
            if(nFrom == 0) break;
        }
        ++nFrom;
        strncpy(out_path, in_file_path_name, nFrom);
        int n=0;
        for(nIndex = nFrom; nIndex <= nTo; ++nIndex)
        {
            if(nIndex > file_path_name_len)
                break;
            out_name[n] = in_file_path_name[nIndex];
            n++;
        }
        out_name[n] = '\0';

        char *pbegin = strchr(out_name, '.');
        if(pbegin == NULL)//文件没有扩展名
        {
            out_ext_name[0]='\0';
        }
        else//文件有扩展名
        {
            pbegin++;
            memcpy(out_ext_name, pbegin, 10);
            //strcpy(out_ext_name, pbegin);
        }
    }
    //解析出路径，文件名
/* 
    if(strlen(in_file_path_name))
    {
        //printf("\tfile path name: [%s] \r\n", in_file_path_name);
    }
    if(strlen(out_path))
    {
        //printf("\tfile path: [%s] \r\n", out_path); 
    }

    if(strlen(out_name))
    {
        //printf("\tfile name: [%s].\r\n", out_name); 
    }

    if(strlen(out_ext_name))
    {
        //printf("\tfile extention name: [%s].\r\n", out_ext_name); 
    }
*/
} 

/** 
 * @brief  按base64方式加密字符串
 * @param  char *key, 加密用的密钥字符串（目前没用） 
 * @param  int key_len, 密钥字符串的长度 
 * @param  char *src, 源明文字符串 
 * @param  int src_len, 明文字符串长度 
 * @param  char *dest, 加密后的密文字符串 (调用者保证内存大小足够）
 * @param  int &dest_len, 加密后的密文字符串长度 
 * @return 成功返回true, 失败返回false. 
 */
bool my_encrypt(char* key,  int key_len, 
                char *src,  int src_len, 
                char *dest, int &dest_len)
{
    //printf("---------------start encrypt--------------\r\n"); 
    //printf("\rsrc len =%d, data:%s\r\n", src_len, src);
    char buffer[1024*4];
    memset(buffer, 0, 1024*4);
    int ret_len = Base64Enc(buffer, src, src_len);
    if(ret_len > dest_len)
    {
        memcpy(dest, buffer, dest_len);
        dest[dest_len] = '\0';
        //printf("\rdest len =%d, data:%s\r\n", ret_len, dest);
        return false;
    }
    dest_len = ret_len;
    memcpy(dest, buffer, ret_len);
    dest[dest_len] = '\0';
    //printf("\rdest len =%d, data:%s\r\n", ret_len, dest);
    //printf("---------------start encrypt over--------------\r\n"); 
    return true;
}

/** 
 * @brief  按base64方式解密字符串
 * @param  char *key, 解密用的密钥字符串（目前没用） 
 * @param  int key_len, 密钥字符串的长度 
 * @param  char *src, 源密文字符串 
 * @param  int src_len, 密文字符串长度 
 * @param  char *dest, 解密后的明文字符串 (调用者保证内存大小足够）
 * @param  int &dest_len, 解密后的明文字符串长度 
 * @return 成功返回true, 失败返回false. 
 */
bool my_decrypt(char* key,  int key_len, 
                char *src,  int src_len, 
                char *dest, int dest_len)
{
    //printf("---------------start decrypt--------------\r\n"); 
    //printf("\rsrc len =%d, data:%s\r\n", src_len, src);
    if(src_len%4 != 0)
    {
        return false;
    }

    char buffer[1024*4];
    memset(buffer, 0, 1024*4);
    int ret_len = Base64Dec(buffer, src, src_len);
    if(ret_len > dest_len)
    {
        memcpy(dest, buffer, dest_len);
        dest[dest_len-1] = '\0';
        //printf("\rdest len =%d, data:%s\r\n", ret_len, dest);
        return false;
    }
    memcpy(dest, buffer, ret_len);
    dest[ret_len] = '\0';
    //printf("\rdest len =%d, data:%s\r\n", ret_len, dest);
    //printf("---------------start decrypt over--------------\r\n"); 
    return true;
}


/** 
 * @brief  检验http_header结构体中的session是否合法
 * @param  const http_header &header 要检验的HTTP头数据 
 * @return 成功返回true, 失败返回false 
 */
bool check_session(const http_header &header)
{
    return true;

    char bsession = true;
    char *ptime = NULL;
    //printf("----------CHECK SESSION--------------\n");
    //printf("user info:\n");
    //printf("\tuser id:%u \n", header.user_id);
    time_t current_time = time(NULL);
    ptime = ctime(&current_time);
    //printf("\tnow time: %s", ptime);    
    //printf("\tuser ip:%s \n", header.user_ip);
    //printf("\tuser port:%u \n", header.user_port);

    ptime = NULL; 
    struct in_addr address;
    address.s_addr = htonl(header.session.user_ip);
    char *paddress = 0;
    paddress = inet_ntoa(address);
    //printf("session info:\n");
    //printf("\tsession user id: %u\n", header.session.user_id);    
    time_t operate_time = header.session.operate_time;
    ptime = ctime(&operate_time);
    //printf("\tsession operate time: %s", ptime);    
    //printf("\tsession user ip: %s\n", paddress);    
    //printf("\tsession user port: %u\n", header.session.user_port);    

    int delay_time = (int)(current_time - header.session.operate_time);
    //printf("\tdelay time: %dsecond.\r\n", delay_time);    
    if(header.user_id == header.session.user_id && delay_time <= 3600)
    {
        bsession = true;
        //printf("----------CHECK SESSION TRUE--------------\n");
    }
    else
    {
        LOG(ERROR,"----------CHECK SESSION FALSE--------------\n");
        LOG(ERROR,"user info:\n");
        LOG(ERROR,"\tuser id:%u \n", header.user_id);
        time_t current_time = time(NULL);
        ptime = ctime(&current_time);
        LOG(ERROR,"\tnow time: %s", ptime);    
        LOG(ERROR,"\tuser ip:%s \n", header.user_ip);
        LOG(ERROR,"\tuser port:%u \n", header.user_port);

        ptime = NULL; 
        struct in_addr address;
        address.s_addr = htonl(header.session.user_ip);
        char *paddress = inet_ntoa(address);
        LOG(ERROR,"session info:\n");
        LOG(ERROR, "\tsession user id: %u\n", header.session.user_id);    
        time_t operate_time = header.session.operate_time;
        ptime = ctime(&operate_time);
        LOG(ERROR,"\tsession operate time: %s", ptime);    
        LOG(ERROR,"\tsession user ip: %s\n", paddress);    
        LOG(ERROR,"\tsession user port: %u\n", header.session.user_port);    
        bsession = false;
        LOG(ERROR,"----------CHECK SESSION FALSE--------------\n");
    }
    //printf("----------CHECK SESSION OVER--------------\n");
    return bsession;
}
