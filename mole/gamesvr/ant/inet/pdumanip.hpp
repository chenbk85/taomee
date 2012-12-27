#ifndef ANT_PDUMAINP_HPP_
#define ANT_PDUMAINP_HPP_

#include <cstring>

#include "byteswap.hpp"

namespace ant {

template <typename T>
inline void pack(void* pkg, T val, int& idx)
{
	*(reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(pkg) + idx)) = bswap(val);
	idx += sizeof val;
}

inline void pack(void* pkg, const void* val, std::size_t len, int& idx)
{
	memcpy(reinterpret_cast<uint8_t*>(pkg) + idx, val, len);
	idx += len;
}

template <typename T>
inline void pack_h(void* pkg, T val, int& idx)
{
	*(reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(pkg) + idx)) = val;
	idx += sizeof val;
}

template <typename T>
inline void unpack(const void* pkg, T& val, int& idx)
{
	val = bswap(*(reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(pkg) + idx)));
	idx += sizeof val;
}

inline void unpack(const void* pkg, void* val, std::size_t len, int& idx)
{
	memcpy(val, reinterpret_cast<const uint8_t*>(pkg) + idx, len);
	idx += len;
}

} // namespace ant

#endif // ANT_PDUMAINP_HPP_
