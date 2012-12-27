#ifndef ANDY_UTILS_H_
#define ANDY_UTILS_H_

// Linux
#include <byteswap.h>
// POSIX
#include <netinet/in.h>
// ISO
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*----------------------------------------------------------
     utilities to unpkg/pkg an integer or a string from/to a given buffer
   ----------------------------------------------------------*/
#define UNPKG_UINT(pkg, val, idx) \
	do { \
		switch ( sizeof(val) ) { \
		case 1: (val) = *(uint8_t*)((pkg) + (idx)); (idx) += 1; break; \
		case 2: (val) = ntohs( *(uint16_t*)((pkg) + (idx)) ); (idx) += 2; break; \
		case 4: (val) = ntohl( *(uint32_t*)((pkg) + (idx)) ); (idx) += 4; break; \
		} \
	} while (0)

#define UNPKG_UINT8(b, v, j) \
	do { \
		(v) = *(uint8_t*)((b)+(j)); (j) += 1; \
	} while (0)

#define UNPKG_UINT32(b, v, j) \
	do { \
		(v) = ntohl( *(uint32_t*)((b)+(j)) ); (j) += 4; \
	} while (0)

#define UNPKG_UINT64(b, v, j) \
	do { \
		(v) = bswap_64( *(uint64_t*)((b)+(j)) ); (j) += 8; \
	} while (0)

#define UNPKG_STR(b, v, j, l) \
	do { \
		memcpy((v), (b)+(j), (l)); (j) += (l); \
	} while (0)

#define UNPKG_H_UINT32(b, v, j) \
	do { \
		(v) = *(uint32_t*)((b)+(j)); (j) += 4; \
	} while (0)

#define UNPKG_H_UINT16(b, v, j) \
	do { \
		(v) = *(uint16_t*)((b)+(j)); (j) += 2; \
	} while (0)

#define PKG_UINT(pkg, val, idx) \
	do { \
		switch ( sizeof(val) ) { \
		case 1: *(uint8_t*)((pkg) + (idx)) = (val); (idx) += 1; break; \
		case 2: *(uint16_t*)((pkg) + (idx)) = htons(val); (idx) += 2; break; \
		case 4: *(uint32_t*)((pkg) + (idx)) = htonl(val); (idx) += 4; break; \
		} \
	} while (0)

#define PKG_UINT8(b, v, j) \
	do { \
		*(uint8_t*)((b)+(j)) = (v); (j) += 1; \
	} while (0)

#define PKG_UINT16(b, v, j) \
	do { \
		*(uint16_t*)((b)+(j)) = htons(v); (j) += 2; \
	} while (0)

#define PKG_UINT32(b, v, j) \
	do { \
		*(uint32_t*)((b)+(j)) = htonl(v); (j) += 4; \
	} while (0)

#define PKG_UINT64(b, v, j) \
	do { \
		*(uint64_t*)( (b)+(j) ) = bswap_64( (v) ); (j) += 8; \
	} while (0)

#define PKG_H_UINT16(b, v, j) \
	do { \
		*(uint16_t*)((b)+(j)) = (v); (j) += 2; \
	} while (0)

#define PKG_H_UINT32(b, v, j) \
	do { \
		*(uint32_t*)((b)+(j)) = (v); (j) += 4; \
	} while (0)

#define PKG_STR(b, v, j, l) \
	do { \
		memcpy((b)+(j), (v), (l)); (j) += (l); \
	} while (0)

#define HTON(to, from) \
	do { \
		switch ( sizeof(to) ) { \
		case 1: (to) = (from); break; \
		case 2: (to) = htons(from); break; \
		case 4: (to) = htonl(from); break; \
		} \
	} while (0)

/*----------------------------------------------------------
     utilities to provide the compiler with branch prediction information
   ----------------------------------------------------------*/
#ifdef  likely
#undef  likely
#endif
// branch most likely to run
#define likely(x)  __builtin_expect(!!(x), 1)

#ifdef  unlikely
#undef  unlikely
#endif
// branch unlikely to run
#define unlikely(x)  __builtin_expect(!!(x), 0)

/*----------------------------------------------------------
     utilities to generate random number
   ----------------------------------------------------------*/
/**
 * ranged_random - generates a random number with the given range
 *  @min: minimum random number
 *  @max: maximum random number
 *
 * return the randomly generated number with the given range
 */
static inline int
ranged_random(int min, int max)
{
    // generates ranged random number
    return (rand() % (max - min + 1)) + min;
}

#endif // ANDY_UTILS_H_
