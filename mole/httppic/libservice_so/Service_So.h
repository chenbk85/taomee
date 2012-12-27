/**
 * =====================================================================================
 *       @file  Service_So.h
 *      @brief  处理socket连接，按功能分发
 *
 *   @internal
 *     Created  2008年10月22日 17时21分07秒 
 *    Revision  1.0.0.0
 *    Compiler  gcc/g++
 *     Company  TaoMee .Inc
 *   Copyright  Copyright (c) 2008, aceway
 *
 *     @author  aceway (半介书生), aceway@taomee.com
 * This source code is wrote for TaoMee,Inc. ShangHai CN.
 * =====================================================================================
 */

#ifndef __SERVER_SO_H
#define __SERVER_SO_H

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <linux/tcp.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../tcp_http/Common.h"
#include "../tcp_http/Log.h"

#define SOCKET_BUFFER_SIZE      4096

#endif
