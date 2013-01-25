#ifndef IP_DICT_HPP
#define IP_DICT_HPP

#include <stdint.h>
#include <map>

struct code_t
{
    uint32_t province_code;
    uint32_t city_code;
};

class CIpDict
{
public:
    bool init();
    bool find(uint32_t ip, code_t &code);

private:
    std::map<uint32_t, code_t> m_ipmap;
};

extern CIpDict g_ipDict;

#endif

