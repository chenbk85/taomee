#ifndef _PACK_EX_H_
#define _PACK_EX_H_

#include <stdint.h>
#include <math.h>
extern "C" {
#include <libtaomee/log.h>
}
#include <libtaomee++/pdumanip.hpp>

using namespace taomee;

template <typename T>
inline void pack_ex(void* pkg, T val, int& idx,  int max_pkg_len)
{
	if( sizeof(val) + idx > max_pkg_len){
		ERROR_LOG("pack_ex T error cur_len = %u, max_len = %u, copy_len = %u", idx, max_pkg_len, sizeof(val));
		return;
	}

	*(reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(pkg) + idx)) = bswap(val);
	idx += sizeof val;
}

inline void pack_ex(void* pkg, const void* val, std::size_t len, int& idx, int max_pkg_len)
{
	if( len + idx > max_pkg_len){
		ERROR_LOG("pack_ex M error cur_len = %u, max_len = %u, copy_len = %u", idx, max_pkg_len, len);
		return;
	}

	memcpy(reinterpret_cast<uint8_t*>(pkg) + idx, val, len);
	idx += len;
}

template <typename T>
inline void pack_ex_h(void* pkg, T val, int& idx, int max_pkg_len)
{
	if( sizeof(val) + idx > max_pkg_len){
		ERROR_LOG("pack_ex_h M error cur_len = %u, max_len = %u, copy_len = %u", idx, max_pkg_len, sizeof(val));
		return;
	}

	*(reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(pkg) + idx)) = val;
	idx += sizeof val;
}














#endif
