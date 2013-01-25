/**
 * =====================================================================================
 *       @file  main.cpp
 *      @brief  
 *
 *   @internal
 *     Created  2010-10-18 11:13:42
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee.Inc, ShangHai.
 *   Copyright  Copyright (c) 2010, TaoMee.Inc, ShangHai.
 *
 *     @author  mason, mason@taomee.com
 * This source code was wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */
#include "./main.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        printf("usage: xml_peeper  <ip|hostname> <port> [<cmd> <xml_path>]\n");
        return -1;
    }
    
    struct hostent * hptr;
    int server_ip, server_port;
    hptr = gethostbyname((const char*)argv[1]);
    if(hptr == NULL)
    {
        printf("wrong server ip or hostname\n");
        return -1;
    }
    server_ip = ((struct in_addr*)( hptr->h_addr_list[0]))->s_addr;

    server_port = atoi(argv[2]);
    if(server_port <= 0 || server_port > 65535)
    {
        printf("wrong server port\n");
        return -1;
    }

    c_data_processer processer;
    if(processer.init() != 0)
    {
        printf("create net client instance error\n");
        return -1;
    }

    char *xml_path = NULL;
    unsigned int cmd_id = 1000;
    if(argc == 5)
    {
        cmd_id = atoi(argv[3]);
        xml_path = argv[4];
    }

    processer.data_processer_main(server_ip, server_port, xml_path, cmd_id);

    return 0;
}

