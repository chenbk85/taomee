#ifndef BT_UTILS_H
#define BT_UTILS_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <glib.h>
#include <libxml/tree.h>
//#include "inet/byteswap.hpp"
#include <libtaomee/log.h>
#ifdef __cplusplus
}
#endif

#define DECODE_XML_PROP(cur, prop, str) \
	str = xmlGetProp(cur, (xmlChar*)prop);  \
if (!str) ERROR_RETURN (("xml parse error: prop=%s", prop), -1)
	
#define DECODE_XML_PROP_EX(cur, prop, str) \
		str = xmlGetProp(cur, (xmlChar*)prop);	\

#define DECODE_XML_PROP_INT_DEFAULT(v, cur, prop, def) \
		do { \
			xmlChar *str; \
			if (!cur || !(str = xmlGetProp(cur, (xmlChar*)prop))) { \
				v = def; \
			} else { \
				v = atoi ((const char*)str); \
				xmlFree (str); \
			} \
		} while (0)

#define DECODE_XML_PROP_INT(v, cur, prop) \
		do { \
			xmlChar *str; \
			DECODE_XML_PROP(cur, prop, str); \
			v = atoi ((const char*)str); \
			xmlFree (str); \
		} while (0)
			
#define DECODE_XML_PROP_INT_EX(v, cur, prop, def) \
					do { \
						xmlChar *str; \
						DECODE_XML_PROP_EX(cur, prop, str); \
						if (str){v = atoi ((const char*)str); xmlFree (str);}else{v = def;} \
					} while (0)


#define DECODE_XML_PROP_UINT32(v, cur, prop) \
		do { \
			xmlChar *str; \
			DECODE_XML_PROP(cur, prop, str); \
			char* endpt; \
			v = strtoul((const char*)str, &endpt, 10); \
			xmlFree (str); \
		} while (0)

#define DECODE_XML_PROP_ARR_INT(arr_, len_, cur_, prop_) \
		do { \
			xmlChar* str; \
			DECODE_XML_PROP((cur_), (prop_), str); \
			int i = 0, cnt = 0, k; \
			size_t slen = strlen((const char*)str); \
			for (; (i != (len_)) && (cnt != slen); ++i, cnt += k) { \
				sscanf((const char*)str + cnt, "%d%n", &((arr_)[i]), &k); \
			} \
			xmlFree (str); \
		} while (0)

#define DECODE_XML_PROP_STR(v, cur, prop) \
		do { \
			xmlChar *str; \
			DECODE_XML_PROP(cur, prop, str); \
			strncpy (v, (const char*)str, sizeof (v) - 1); \
			v[sizeof(v) - 1] = '\0'; \
			xmlFree (str); \
		} while (0)

static inline void
decode_xml_prop_uint32_default(uint32_t* val, xmlNodePtr cur, const void* prop, uint32_t def)
{
	xmlChar* str;
	if (!cur || !(str = xmlGetProp(cur, (xmlChar*)prop))) {
		*val = def;
	} else {
		char* endpt;
		*val = strtoul((const char*)str, &endpt, 10);
		xmlFree (str);
	}
}

static inline void
decode_xml_prop_uint16_default(uint16_t* val, xmlNodePtr cur, const void* prop, uint32_t def)
{
	xmlChar* str;
	if (!cur || !(str = xmlGetProp(cur, (xmlChar*)prop))) {
		*val = def;
	} else {
		char* endpt;
		*val = strtoul((const char*)str, &endpt, 10);
		xmlFree (str);
	}
}

static inline size_t
decode_xml_prop_arr_int_default(int arr[], size_t len, xmlNodePtr cur, void* prop, uint32_t def)
{
	xmlChar* str;
	if (!cur || !(str = xmlGetProp(cur, (xmlChar*)prop))) {
		size_t i = 0;
		for (; i != len; ++i) {
			arr[i] = def;
		}
		return 0;
	} else {
	
		unsigned char* tail = str + strlen((const char*)str);
		while (*(tail - 1) == ' ')  {
			*(tail - 1)= '\0';
			tail--;
		}
	
		int k;
		size_t i = 0, cnt = 0;
		size_t slen = strlen((const char*)str);
		for (; (i != len) && (cnt != slen); ++i, cnt += k) {
			sscanf((const char*)str + cnt, "%d%n", &(arr[i]), &k);
		}
		xmlFree(str);
		return i;
	}
}

static inline void
decode_xml_prop_float_default(float* val, xmlNodePtr cur, void* prop, float def)
{
	xmlChar* str;
	if (!cur || !(str = xmlGetProp(cur, (xmlChar*)prop))) {
		*val = def;
	} else {
		sscanf((const char*)str, "%f", val);
		xmlFree(str);
	}
}

/**
 * load_xmlconf - load a given config file in xml format
 * @file: file to load
 * @parser: function to parse @file
 * 
 * return: -1 if failed
 */
static inline int load_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node))
{
	int err = -1;

	xmlDocPtr doc = xmlParseFile(file);
	if (!doc) {
		ERROR_RETURN(("Failed to Load %s", file), -1);
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc); 
	if (!cur) {
		ERROR_LOG("xmlDocGetRootElement error");
		goto fail;
	}

	err = parser(cur);
fail:
	xmlFreeDoc(doc);
	BOOT_LOG(err, "Load File %s", file);
}


#define CHECK_USER_ID(uid_) \
		do { \
			if ((uid_) < 10000 || (uid_) > 2000000000) { \
				ERROR_LOG("invalid uid=%u", (uid_)); \
				return -1; \
			} \
		} while (0)

#define CHECK_VAL_LE(val_, max_) \
		if ( (val_) > (max_) ) ERROR_RETURN(("val %d is greater than max %d", (val_), (max_)), -1)
#define CHECK_VAL_GE(val_, min_) \
		if ( (val_) < (min_) ) ERROR_RETURN(("val %d is less than min %d", (val_), (min_)), -1)
#define CHECK_VAL(val_, expect_) if ( (val_) != (expect_) ) ERROR_RETURN(("val %d is no equal to the expected %d", (val_), (expect_)), -1)

#define CHECK_VAL_GE_VOID(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %d is less than min %d",val_,min_); \
				return; \
			} \
		} while (0)
#define CHECK_INT_EQ_VOID(val_, val2_) \
					if ( (val_) != (val2_) ) ERROR_RETURN_VOID("val %d is not equal to val2 %d", (val_), (val2_))

#define CHECK_INT_EQ_RET(val_, val2_, ret__) \
										if ( (val_) != (val2_) ) ERROR_RETURN(("val %d is not equal to val2 %d", (val_), (val2_)), ret__)


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
					
#define UNPKG_H_UINT64(b, v, j) \
					do { \
						(v) = *(uint64_t*)((b)+(j)); (j) += 8; \
					} while (0)
					
#define UNPKG_H_UINT32(b, v, j) \
					do { \
						(v) = *(uint32_t*)((b)+(j)); (j) += 4; \
					} while (0)

#define UNPKG_H_INT32(b, v, j) \
	                do { \
						(v) = *(int32_t*)((b)+(j)); (j) += 4; \
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
		*(uint32_t*)((b)+(j)) = htonl((uint32_t)(v)); (j) += 4; \
		*(uint32_t*)((b)+(j)) = htonl((uint32_t)(((battle_id_t)(v)) >> 32)); (j) += 4; \
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

#define PKG_H_INT32(b, v, j) \
		do { \
			*(int32_t*)((b)+(j)) = (v); (j) += 4; \
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

inline int bt_get_valid_value(int value,int min_value,int max_value ){
		if (value<min_value ) {
		   	return min_value;
		} else if (value>max_value ) {
		   	return max_value;
		}else{
		   	return value;
		}
}
inline bool is_at_rand(uint32_t rand_value , uint32_t range=100 ){
	 	return (random()%range)<rand_value;
}

#endif // HS_UTILS_H_

