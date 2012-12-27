#ifndef HS_UTILS_H
#define HS_UTILS_H

#include <libtaomee/log.h>

#define CHECK_VAL_LE(val_, max_) \
		if ( (val_) > (max_) ) ERROR_RETURN(("val %d is greater than max %d", (val_), (uint32_t)(max_)), -1)
#define CHECK_VAL_GE(val_, min_) \
		if ( (val_) < (min_) ) ERROR_RETURN(("val %d is less than min %d", (val_), (uint32_t)(min_)), -1)
#define CHECK_VAL(val_, expect_) if ( (val_) != (expect_) ) ERROR_RETURN(("val %d is no equal to the expected %d", (val_), (uint32_t)(expect_)), -1)

#define CHECK_VAL_GE_VOID(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %d is less than min %d"); \
				return; \
			} \
		} while (0)
#define CHECK_INT_EQ_VOID(val_, val2_) \
					if ( (val_) != (val2_) ) ERROR_RETURN_VOID("val %d is not equal to val2 %d", (val_), (uint32_t)(val2_))

#define UNPKG_UINT8(b, v, j) \
					do { \
						(v) = *(uint8_t*)((b)+(j)); (j) += 1; \
					} while (0)
			
#define UNPKG_UINT16(b_, v_, j_) \
					do { \
						(v_) = ntohs(*(uint16_t*)((b_)+(j_))); (j_) += 2; \
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
			
#define UNPKG_H_UINT8(buf_, val_, idx_) UNPKG_UINT8((buf_), (val_), (idx_))

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

#define PKG_H_UINT8(b, v, j) \
		do { \
			*(uint8_t*)((b)+(j)) = (v); (j) += 1; \
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
#define PKG_MAP_ID(b, v, j) \
		do { \
			*(uint32_t*)((b)+(j)) = htonl((uint32_t)(v)); (j) += 4; \
			*(uint32_t*)((b)+(j)) = htonl((uint32_t)(((map_id_t)(v)) >> 32)); (j) += 4; \
		} while (0)

#endif // HS_UTILS_H_

