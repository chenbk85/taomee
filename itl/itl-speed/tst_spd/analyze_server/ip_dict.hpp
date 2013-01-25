#ifndef IP_DICT_HPP
#define IP_DICT_HPP

#include <stdint.h>
#include <map>

#ifndef ISP_LEN
#define	ISP_LEN	64
#endif

struct code_t
{
	uint32_t	ipspan;
	uint32_t	province_code;
	uint32_t	city_code;
	uint32_t	isp_id;
	uint8_t	isp[ISP_LEN];
	uint8_t	type;
	uint8_t	key[80];
	uint32_t	offset;
}__attribute__((packed)) ;

class CIpDict
{
public:
    bool init(const char *file_path);
    bool find(uint32_t ip, code_t &code, uint32_t *key);

//private:
    std::map<uint32_t, code_t> m_ipmap;
};

extern CIpDict g_ipDict;

#endif

