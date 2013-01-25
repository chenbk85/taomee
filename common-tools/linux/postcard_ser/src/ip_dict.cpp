#include <string.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <fstream>
#include <arpa/inet.h>
#include <iostream>
extern "C"
{
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/log.h>
}
#include "ip_dict.hpp"

CIpDict g_ipDict;

bool CIpDict::init()
{
    std::ifstream ifs;
    ifs.open("./ipdict.dat", std::ios_base::binary);
    if (!ifs.is_open())
    {
        ERROR_LOG("can't find ipdict.dat");
        return false;
    }
    uint32_t size = 0;
    ifs >> size;

    DEBUG_LOG("ip map num=%u", size);

    for (uint32_t i = 0; i < size; ++i)
    {
        uint32_t key;
        code_t code;
        ifs >> key;
        ifs >> code.province_code;
        ifs >> code.city_code;
        m_ipmap[key] = code;
    }

    return true;
}

bool CIpDict::find(uint32_t ip, code_t &code)
{
    // 网络序到本地字节序转换.
    uint32_t hip = ntohl(ip);

    std::map<uint32_t, code_t>::iterator it = m_ipmap.upper_bound(hip);

    if (it == m_ipmap.begin())
    {
        ERROR_LOG("m_ipmap.upper_bound = begin");
        return false;
    }

    --it;

    if ((*it).second.province_code == 0)
    {
        DEBUG_LOG("can't find ip %u", hip);
        return false;
    }
    code = (*it).second;
    return true;
}


