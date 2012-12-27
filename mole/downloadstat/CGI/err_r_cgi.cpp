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
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include "msglog.h"

using namespace std;

struct T_ByteRate
{
    int bytedown;
    int timediff;
};

T_ByteRate g_Stat;

uint32_t socket_addr_str2int(const char* _sAddr);

#define UNIXDG_PATH "/home/nash/CheckIP/CheckIP.sock"

int main(int argc, char * argv[])
{
    cout << "Content type: text/plain\n\n" << endl;

    int sockfd;
    struct sockaddr_un	servaddr;
    
    sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sockfd<=0)
    {
        cout << "socket error [" << strerror(errno) << ":" << errno << "]"<< endl;
        return -1;
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sun_family=AF_LOCAL;
    strcpy(servaddr.sun_path, UNIXDG_PATH);
    
    connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr));
    /*if (ret<0)
    {
        cout << "connect error [" << strerror(errno) << ":" << errno << "]"<< endl;
        return -1;
    }*/
    
    const char* ip=getenv("REMOTE_ADDR");
    if (ip==NULL)
    {
        cout << "get request ip error" << endl;
        return -1;
    }
    cout << ip << endl;
    uint32_t b=socket_addr_str2int(ip);
    send(sockfd, &b, sizeof(b), 0);
    uint32_t province=0;
    recv(sockfd, (char*)&province, sizeof(province), 0);
    
    string s=getenv("REQUEST_METHOD");
    if (s!="GET")
    {
        cout << "request method error [" << s <<"]" << endl;
        return -1;
    }

    s=getenv("QUERY_STRING");
    int pos=s.find('=', 0)+1;
    string tmp;
    for (; s[pos]!='&'; pos++)
    {
        tmp+=s[pos];
    }
    g_Stat.bytedown=atoi(tmp.c_str());
    pos=s.find('=', pos)+1;
    string tmp1;
    for (; s[pos]!='&'; pos++)
    {
        tmp1+=s[pos];
    }
    g_Stat.timediff=atoi(tmp1.c_str());
    if (g_Stat.timediff<60000&&g_Stat.timediff>500) // 屏蔽下载时间大于1分钟和小于0.5秒的用户
    {
        time_t ts=time(NULL);
        msglog("/home/nash/stat/inbox/log.bin", 0x02030001+province, ts, &g_Stat, sizeof(g_Stat));
    }
    cout << "Success [" << province << "]!!!" << endl;
    return 0;
}

uint32_t socket_addr_str2int(const char* _sAddr)
{
    struct hostent *hp;
    
    if(_sAddr==NULL)
        return 0;
    
    if(isalpha(_sAddr[0]))
    {
        hp = gethostbyname(_sAddr);
        if(NULL==hp)
            return 0;
        return ntohl(((struct in_addr*)hp->h_addr)->s_addr);
    }
    else
    {
        return ntohl(inet_addr(_sAddr));
    }
}


