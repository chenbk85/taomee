#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

char char_to_int(char ch);
char str_to_bin(char * str);
int url_decode(char * _Dst, int _iLen, const char * _Src, int _SrcLen);
char * get_line(int _fd, char * _Buf);

int main(int argc, char * argv[])
{
    char * data;
    char file_name[256];
    char tmp_name[256];
    char cmd[256];
    int fd1, fd2;
    char line[4096];
    char de_code[4096];

    printf ("Content type: text/plain\n\n");

    data=getenv("QUERY_STRING");
    if (strlen(data)<=0||data==NULL)
    {
        printf("[%s:%d]unknown get error found!!! \n", __FILE__, __LINE__);
        return 0;
    }

    memset(line, 0, 4096);
    memset(de_code, 0, 4096);

    sprintf(file_name, "/var/www/errData/%s", data+5);
    sprintf(tmp_name, "/var/www/errData/tmp.dat");
    sprintf(cmd, "rm -rf %s", tmp_name);
    
    printf("file_name:%s\n", file_name);
    printf("tmp_name:%s\n", tmp_name);

    system(cmd);

    fd1=open(file_name, O_RDONLY);
    fd2=open(tmp_name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if (!fd1||!fd2)
    {
        printf("[%s:%d]open file error \n", __FILE__, __LINE__);
        return 0;
    }

    while (get_line(fd1, line)!=NULL)
    {
        url_decode(de_code, 4096, line, 4096);
        write(fd2, de_code, strlen(de_code));
    }

    printf("success!!!\n");
    return 0;
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

char * get_line(int _fd, char * _Buf)
{
    char ch;
    int i;
    int ret;

    _Buf[0]='&';
    _Buf[1]='&';
    i=2;
    lseek(_fd, 2, SEEK_CUR);

    while (1)
    {
        ret=read(_fd, &ch, 1);
        if (ret==0) return NULL;
        _Buf[i]=ch;
        i++;
        
        if (ch=='&')
        {
            read(_fd, &ch, 1);
            _Buf[i]=ch;
            i++;
            if (ch=='&')
            {
                _Buf[i-2]=0;
                lseek(_fd, -2, SEEK_CUR);
                break;
            }
            else
                continue;
        }
    }
    return _Buf;
}

