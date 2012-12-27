/*******************************************************************************
 * 版权所有: 
 * 文件名称: _ERR_REPORT_CGI_
 * 内容说明: 接收客户端的错误报告以固定格式存入文件
 * 当前版本: 
 * 创 建 人: nash.shi
 * ----------------------------------------------------------------------------
 * 修改记录:
 * 日  期    修改内容
 * 00/00/00     
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include "msglog.h"

#define ERRFILE_MAX_SIZE 2*1024*1024
#define ERRFILE_HOME_DIR "/var/www/errData"
// #define ERRFILE_HOME_DIR "/home/nash/err"

char g_Home[64];
char gs_FileName[256];
int gi_ErrType;


int get_data(int * _p_ErrType, char * _ErrBuf);
int post_data(int * _p_ErrType, char * _ErrBuf, int * _ErrLen);
int do_stat_proc(int _index);
int write_err(int _ErrType, char * _ErrBuf, int _ErrLen);
// int write_err(int _ErrType, char * _ErrBuf, int _ErrLen, char * _Encode, int _EnLen);
const char* get_file_name(const char * _Home, int _ErrType);
char char_to_int(char ch);
char str_to_bin(char * str);
int url_decode(char * _Dst, int _iLen, const char * _Src, int _SrcLen);
struct tm local_tm(struct tm *p);

int main(int argc, char * argv[])
{
    int r=0;

    char g_Buf[4096];
    char g_Decode[4096];

    memset(g_Buf, 0, 4096);
    memset(g_Decode, 0, 4096);

    printf ("Content type: text/plain\n\n");

    char s[8];
    strncpy(s, getenv("REQUEST_METHOD"), sizeof(s));
    if(strlen(s)!=3&&strlen(s)!=4)
    {
        printf("[%s:%d]Unknown error!!! \n", __FILE__, __LINE__);
        return -1;
    }
    
    r=get_data(&gi_ErrType, g_Buf);
    if (r<0)
    {
        printf("[%s:%d]Get data error found!!!\n", __FILE__, __LINE__);
        return -1;
    }

    int len=post_data(&gi_ErrType, g_Buf, 0);
    if (len<=0)
    {
        printf("[%s:%d]Post data error found!!!\n", __FILE__, __LINE__);
        return -1;
    }

    printf("<<<%d>>>\n", len);

    // int ret=url_decode(g_Decode, 1024, g_Buf, len);

    printf("en_code [%s]\n", g_Buf);
    printf("de_code [%s]\n", g_Decode);
    // write_err(gi_ErrType, g_Decode, ret, g_Buf, strlen(g_Buf));
    write_err(gi_ErrType, g_Buf, strlen(g_Buf));

    int i=1;
    msglog("/home/nash/stat/inbox/log.bin", 0x02060001+gi_ErrType, time(NULL), &i, sizeof(int));
    return 0;
}

int get_data(int * _p_ErrType, char * _ErrBuf)
{
    char * data=getenv("QUERY_STRING");
    if (strlen(data)<=0||data==NULL)
    {
        printf("[%s:%d]Unknown post error found!!! \n", __FILE__, __LINE__);
        return 0;
    }

    int pos=0;
    char * ppos=data;
    char sTmp[8];
    memset(sTmp, 0, sizeof(sTmp));

    for (; pos<1024; pos++)
    {
        if (*(ppos+pos)=='=') // 找第一个=
        {
            for (int i=0; i<8; i++)
            {
                pos++;
                if (*(ppos+pos)!='&')
                    sTmp[i]=*(ppos+pos);
                else
                    break;
            }
            break;
        }
    }
    *_p_ErrType=atoi(sTmp);
    return pos+1;
}

int post_data(int * _p_ErrType, char * _ErrBuf, int * _ErrLen)
{
    int len;
    printf("<<<%s>>>\n", getenv("CONTENT_LENGTH"));
    const char *sLen=getenv("CONTENT_LENGTH");
    if (sLen==NULL)
        return 0;
    
    len=atoi(sLen);
    if (len<=0)
    {
        printf("[%s:%d]Unknown get error found!!! \n", __FILE__, __LINE__);
        return 0;
    }

    for (int pos=0; pos<len; pos++)
    {
        *(_ErrBuf+pos)=getchar();
    }
    return len;
}

// int write_err(int _ErrType, char * _ErrBuf, int _ErrLen, char * _Encode, int _EnLen)
int write_err(int _ErrType, char * _ErrBuf, int _ErrLen)
{
    int fd;
    fd = open(get_file_name(ERRFILE_HOME_DIR, _ErrType), O_RDWR);
    if (fd < 0)	
    {
        printf("[%s:%d]Fd error found[%d]!!!\n", __FILE__, __LINE__, fd);
        return -1;
    }

    if (lockf(fd, F_LOCK, 0) == -1)
        return -1;

    char sTs[64];
    uint32_t now=time(NULL);
    sprintf(sTs, "&&%d&%s&", now, getenv("REMOTE_ADDR"));
    lseek(fd, 0, SEEK_END);
    write(fd, sTs, strlen(sTs));
    lseek(fd, 0, SEEK_END);
    write(fd, _ErrBuf, _ErrLen);
    lseek(fd, 0, SEEK_END);
    write(fd, "\n", 1);

    /*lseek(fd, 0, SEEK_END);
    write(fd, sTs, strlen(sTs));
    lseek(fd, 0, SEEK_END);
    write(fd, _Encode, _EnLen);
    lseek(fd, 0, SEEK_END);
    write(fd, "\n", 1);*/

    if (lockf(fd, F_ULOCK, 0) < 0)
    {
        printf("[%s:%d]Unlock file error found!!!\n", __FILE__, __LINE__);
        return -1;
    }

    close(fd);

    printf("[%s:%d]File write success!!!\n", __FILE__, __LINE__);

    return 0;
}

const char * get_file_name(const char * _Home, int _ErrType)
{
    struct tm date;
    local_tm(&date);
    for (int i=0; i<100; i++)
    {
        snprintf(gs_FileName, sizeof(gs_FileName), "%s/%4d%02d%02d/%d/%d"
            , _Home, date.tm_year+1900, date.tm_mon+1, date.tm_mday, _ErrType, i);
        int fd;
        fd=open(gs_FileName, O_RDONLY);
        if (fd<0)
        {
            fd=open(gs_FileName, O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
            if (fd<0)
            {
                printf("[%s:%d]Error file create failed[%s]!!!\n", __FILE__, __LINE__, gs_FileName);
                return NULL;
            }
            close(fd);
            return NULL;
        }
        else
        {
            struct stat statbuf;
            int ret=fstat( fd, &statbuf);
            if(ret<0)
            {
                printf("[%s:%d]Fstat error [ret=%d]\n", __FILE__, __LINE__, ret);
                return NULL;
            }
            if (statbuf.st_size>=ERRFILE_MAX_SIZE) // 文件太大了
            {
                close(fd);
                continue;
            }
            else
            {
                close(fd);
                return gs_FileName;
            }
        }
    }
    return NULL; // 错误文件太多了需要清理一下
}

char char_to_int(char ch)
{
    if(ch>='0' && ch<='9') return (char)(ch-'0');
    if(ch>='a' && ch<='f') return (char)(ch-'a'+10);
    if(ch>='A' && ch<='F') return (char)(ch-'A'+10);
    return -1;
}

char str_to_bin(char * str)
{
    char tempWord[2];
    tempWord[0] = char_to_int(str[0]); //make the B to 11 -- 00001011
    tempWord[1] = char_to_int(str[1]); //make the 0 to 0  -- 00000000
    return (tempWord[0] << 4) | tempWord[1];
}

int url_decode(char * _Dst, int _iLen, const char * _Src, int _SrcLen)
{
    char tmp[2];
    int len=_SrcLen;
    int i=0, p=0;
    for (; p<_iLen&&i<len; )
    {
        if(_Src[i]=='%')
        {
            tmp[0]=_Src[i+1];
            tmp[1]=_Src[i+2];
            _Dst[p]=str_to_bin(tmp);
            p++; i=i+3;
        }
        else if(_Src[i]=='+')
        {
            _Dst[p]=' ';
            i++;
        }
        else
        {
            _Dst[p]=_Src[i];
            p++; i++;
        }
    }
    
    return p;
}

struct tm local_tm(struct tm *p)
{
    struct tm _stm;
    struct tm *newtime;
    
    time_t long_time;
    
    time( &long_time );                /* Get time as long integer. */
    newtime = localtime( &long_time ); /* Convert to local time. */
    
    memcpy(&_stm, newtime, sizeof(struct tm));
    if(p)
        memcpy(p, newtime, sizeof(struct tm));
    
    return _stm;
}


