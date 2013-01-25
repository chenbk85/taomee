/** 
 * ========================================================================
 * @file itl_util.cpp
 * @brief 
 * @author smyang
 * @version 1.0
 * @date 2012-07-19
 * Modify $Date: $
 * Modify $Author: $
 * Copyright: TaoMee, Inc. ShangHai CN. All rights reserved.
 * ========================================================================
 */

#include <glib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <string>
#include <algorithm>
#include <pwd.h>
#include <net/if.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <stdint.h>
#include <libtaomee++/utils/md5.h>
#include "itl_util.h"



uint32_t get_day_timestamp(uint32_t timestamp)
{
	struct tm tm_time;
    time_t tmp_timestamp = (time_t)timestamp;
	if (NULL == localtime_r(&tmp_timestamp, &tm_time))
	{
		return 0;
	}

    tm_time.tm_hour = 0;
    tm_time.tm_min = 0;
    tm_time.tm_sec = 0;
    time_t day_timestamp = mktime(&tm_time);
    if (-1 == day_timestamp)
    {
        return 0;
    }
    else
    {
        return ((uint32_t)day_timestamp);
    }
}


int get_file_md5(const char * file, char * file_md5)
{
    std::ifstream fin(file, std::ios::in | std::ios::binary);
    if (fin.is_open())
    {
        utils::MD5 md5_obj;
        md5_obj.update(fin);
        STRNCPY(file_md5, md5_obj.toString().c_str(), 33);
        return 0;
    }
    else
    {
        return -1;
    }
}

int str2md5(const char * src_str, char * dst_md5)
{
    if (NULL != src_str)
    {
        utils::MD5 md5_obj;
        md5_obj.update(src_str);
        STRNCPY(dst_md5, md5_obj.toString().c_str(), 33);
        return 0;
    }
    else
    {
        return -1;
    }
}




const char * timestamp2str(time_t timestamp)
{
    static char buff[32] = {0};
    
    struct tm *dt;
    dt = localtime(&timestamp);
    if (NULL != dt) 
    {
        strftime(buff, 32, "%Y-%m-%d %H:%M:%S", dt);
    }

    return buff;
}


const char * timestamp2compact_str(time_t timestamp)
{
    static char buff[32] = {0};
    
    struct tm *dt;
    dt = localtime(&timestamp);
    if (NULL != dt) 
    {
        strftime(buff, 32, "%Y%m%d%H%M%S", dt);
    }

    return buff;
}



int set_user(const char * username)
{
    struct passwd * pw = getpwnam(username);
    if(NULL == pw) 
    {
        return -1;
    }

    uid_t rval = getuid();
    if(rval != pw->pw_uid) 
    {
        if(0 != rval) 
        {
            return  -1; 
        }
        //设置effective gid如果调用者是root那么也将设置real gid
        setgid(pw->pw_gid);
        //设置effective uid如果调用者是root那么也将设置real uid
        if(0 != setuid(pw->pw_uid)) 
        {
            return -1; 
        }
    }

    return  0;
}


int lower_case(char * str)
{
    if (NULL == str)
    {
        return -1;
    }
    char * p = str;
    while (*p)
    {
        if ('A' <= *p && *p <= 'Z')
        {
            *p = *p - 'A' + 'a';
        }
        p++;
    }

    return 0;
}


const char * get_filename(const char * file)
{
    const char * p = strrchr(file, '/');
    if (NULL == p)
    {
        p = file;
    }
    else
    {
        p++;
    }
    return p;

}



int get_ip_by_name(const char * eth_name, char * ip)
{

    int ret_code = -1;
    int sockfd = -1;

    do {
        if (NULL == eth_name || NULL == ip) 
        {
            ERROR_LOG("ERROR: Parameter cannot be NULL.");
            break;
        }

        if (0 == strlen(eth_name)) 
        {
            ERROR_LOG("ERROR: eth_name length cannot be zero.");
            break;
        }

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (-1 == sockfd) 
        {
            ERROR_LOG("ERROR: socket(AF_INET, SOCK_DGRAM, 0) failed: %s", strerror(errno));
            break;
        }

        struct ifreq ifr;
        strncpy(ifr.ifr_name, eth_name, sizeof(ifr.ifr_name));
        ifr.ifr_name[sizeof(ifr.ifr_name) -1] = 0;

        if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0) 
        {
            // ERROR_LOG("ERROR: ioctl(%d, SIOCGIFADDR, %s) failed: %s", sockfd, eth_name, strerror(errno));
            break;
        }
        strcpy(ip, inet_ntoa(((struct sockaddr_in*)(&ifr.ifr_addr))->sin_addr));
        ret_code = 0;
    } while (false);

    if (sockfd >= 0) 
    {
        close(sockfd);
    }

    return ret_code;
}

int get_ip(int ip_type, char * ip)
{
    if (NULL == ip) 
    {
        ERROR_LOG("ERROR: Parameter cannot be NULL.");
        return -1;
    }

    if (0x02 == ip_type) 
    {
        if (0 == get_ip_by_name("eth0", ip)) 
        {
            ///一般情况，外网是eth0
            return 0;
        }
    }
    else
    {
        if (0 == get_ip_by_name("eth1", ip)) 
        {
            ///一般情况，内网是eth1
            return 0;
        }
    }

    ///特殊情况
    char bond_ip[16] = {0};
    char eth2_ip[16] = {0};
    char eth3_ip[16] = {0};
    get_ip_by_name("bond0", bond_ip);
    get_ip_by_name("eth2", eth2_ip);
    get_ip_by_name("eth3", eth3_ip);

    if (0x02 == ip_type) 
    {
        //外网
        if (strlen(bond_ip) >= 7 || !IS_INSIDE_IP(bond_ip)) 
        {
            strcpy(ip, bond_ip);
            return 0;
        }
        if (strlen(eth3_ip) >= 7 || !IS_INSIDE_IP(eth3_ip)) 
        {
            strcpy(ip, eth3_ip);
            return 0;
        }
        if (strlen(eth2_ip) >= 7 || !IS_INSIDE_IP(eth2_ip)) 
        {
            strcpy(ip, eth2_ip);
            return 0;
        }
    }
    else 
    {
        //内网
        if (IS_INSIDE_IP(bond_ip)) 
        {
            strcpy(ip, bond_ip);
            return 0;
        }
        if (IS_INSIDE_IP(eth3_ip)) 
        {
            strcpy(ip, eth3_ip);
            return 0;
        }
        if (IS_INSIDE_IP(eth2_ip)) 
        {
            strcpy(ip, eth2_ip);
            return 0;
        }
    }

    return -1;
}


int get_inside_ip(char * buf)
{
    char ip_str[INET_ADDRSTRLEN] = {0};
    // 先取eth1的，因为：
    // 1、一般eth1就是对应内网ip
    // 2、有的机器同时有eth0和eth1都是192.168.x.xx，但eth1才是其内网ip
    const char * device_list[] = {"eth1", "eth0", "bond0", "eth2", "eth3"};

    for (uint32_t i = 0; i < G_N_ELEMENTS(device_list); i++)
    {
        if (0 == get_ip_by_name(device_list[i], ip_str))
        {
            if (IS_INSIDE_IP(ip_str))
            {
                strncpy(buf, ip_str, sizeof(ip_str));
                return 0;
            }
        }
    }

    return -1;
}


