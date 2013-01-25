#include <string.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <fstream>
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
extern "C" {
#include <libtaomee/log.h>
}
#include "ip_dict.hpp"

CIpDict g_ipDict;

bool CIpDict::init()
{
	uint32_t size = 0;
	FILE *fp = fopen("ipdict.dat","rb");
	if (fp == NULL) {
		return false;
	}

	fread(&size, sizeof(size), 1, fp);

	for (uint32_t i = 0; i < size; ++i) {
		uint32_t key;
    	code_t code;

    	fread(&key, sizeof(key), 1, fp);
    	fread(&code, sizeof(code), 1, fp);
    	m_ipmap[key] = code;
	}

	fclose(fp);
	return true;
}

bool CIpDict::find(uint32_t ip, code_t &code, uint32_t *key)
{
    // 本地字节序.
	uint32_t hip = ip;
	std::map<uint32_t, code_t>::iterator it = m_ipmap.lower_bound(hip);
	std::map<uint32_t, code_t>::iterator itend = m_ipmap.end();
	itend --;

	if (it == itend)
		goto retf;

	if (hip == it->first) {
		if (it->second.province_code == 0)
			goto retf;
		code = it->second;
		*key = it->first;
		return true;
	}

	--it;

	if (it->second.type == 1)
		++it;

	if (it->second.province_code == 0)
		goto retf;

	code = it->second;
	*key = it->first;
	return true;

retf:
	ERROR_LOG("IP NOT FOUND:%u",hip);
	return false;
}


