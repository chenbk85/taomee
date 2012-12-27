#ifndef _UTIL_H_
#define _UTIL_H_

// C89
#include <string.h>
// C99
#include <stdint.h>
// Posix
#include <arpa/inet.h>

#include <glib.h>

#include <libxml/tree.h>

#include <libtaomee/log.h>

#include <libtaomee/tm_dirty/tm_dirty.h>

#include "benchapi.h"

#define JUMP_XML_TAG(cur, str) \
	if (!cur || xmlStrcmp(cur->name, (const xmlChar *)str))  \
ERROR_RETURN (("xmlStrcmp error: tag=%s", str), -1);

#define DECODE_XML_PROP(cur, prop, str) \
	str = xmlGetProp(cur, (xmlChar*)prop);  \
if (!str) ERROR_RETURN (("xml parse error: prop=%s", prop), -1)

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
decode_xml_prop_uint32_default(uint32_t* val, xmlNodePtr cur, void* prop, uint32_t def)
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
decode_xml_prop_food_int_default(int arr[], size_t len, xmlNodePtr cur, void* prop, uint32_t def)
{
	int k;
	char c[16];
	size_t i = 0;

	xmlChar* str;
	for (i=0; i != len; ++i) {
		arr[i] = def;
	}

	if (!cur || !(str = xmlGetProp(cur, (xmlChar*)prop))) {
		return 0;
	}

	size_t idx = 0;
	int itmid = 0;
	size_t cnt = 0;
	size_t slen = strlen((const char*)str);
	for (i=0; (idx != len) && (cnt < slen); ++i, cnt += k) {
		sscanf((const char*)str + cnt, "%d%n", &itmid, &k);
		cnt += k;
		if (cnt < slen) {
			sscanf((const char*)str + cnt, "%[^0-9]%n", c, &k);
		}
		//sscanf((const char*)str + cnt, "%d%[^0-9]%n", &itmid, c, &k);
		if(itmid == 0) break;
		if(arr[idx] == itmid){
			arr[idx+1]++;
		} else {
			if(arr[idx]) idx += 2;
			arr[idx] = itmid;
			arr[idx+1] = 1;
		}
	}
	xmlFree(str);
	return i;
}

static inline size_t
decode_xml_prop_arr_int_default(int arr[], size_t len, xmlNodePtr cur, void* prop, uint32_t def)
{
	int k;
	char c[16];
	size_t i = 0;

	xmlChar* str;
	for (i=0; i != len; ++i) {
		arr[i] = def;
	}

	if (!cur || !(str = xmlGetProp(cur, (xmlChar*)prop))) {
		return 0;
	}

	size_t cnt = 0;
	size_t slen = strlen((const char*)str);
	for (i=0; (i != len) && (cnt < slen); ++i, cnt += k) {
		//sscanf((const char*)str + cnt, "%d%[^0-9]%n", &(arr[i]), c, &k);
		sscanf((const char*)str + cnt, "%d%n", &(arr[i]), &k);
		cnt += k;
		if (cnt < slen) {
			sscanf((const char*)str + cnt, "%[^0-9]%n", c, &k);
		}
	}
	xmlFree(str);
	return i;
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
int  load_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node));

int  pipe_create(int *);

#define CHECK_INT_LE(val_, max_) \
	if ( (val_) > (max_) ) ERROR_RETURN(("val %d is greater than max %d", (val_), (uint32_t)(max_)), -1)
#define CHECK_INT_GE(val_, min_) \
	if ( (val_) < (min_) ) ERROR_RETURN(("val %d is less than min %d", (val_), (uint32_t)(min_)), -1)

#define CHECK_INT_EQ_VOID(val_, val2_) \
		if ( (val_) != (val2_) ) ERROR_RETURN_VOID("val %d is not equal to val2 %d", (val_), (uint32_t)(val2_))

#define CHECK_BODY_LEN(len_, s_) if((len_)!=(s_)) ERROR_RETURN(("error recvlen=%d expectedlen=%d", (uint32_t)(len_), (uint32_t)(s_)), -1)
#define CHECK_BODY_LEN_GE(len_, s_) if ((len_)<(s_)) ERROR_RETURN(("error recvlen=%d expectedlen>=%d", (uint32_t)(len_), (uint32_t)(s_)), -1)
#define CHECK_BODY_LEN_LE(len_, s_) if ((len_)>(s_)) ERROR_RETURN(("error recvlen=%d expectedlen<=%d", (uint32_t)(len_), (uint32_t)(s_)), -1)

#define CHECK_BODY_LEN_GE_VOID(len_, s_) if ((len_)<(s_)) ERROR_RETURN_VOID("error recvlen=%d expectedlen>=%d", (uint32_t)(len_), (uint32_t)(s_))
#define CHECK_BODY_LEN_VOID(len_, s_) if ((len_) != (s_)) ERROR_RETURN_VOID("error recvlen=%d expectedlen=%d", (uint32_t)(len_), (uint32_t)(s_))

#define CHECK_WAITCMD(p_, cmd_) if ((p_)->waitcmd != (cmd_)) ERROR_RETURN(("waitcmd [%d %d] mismatch, id=%u", (p_)->waitcmd, (cmd_), (p_)->id), -1)

#if 0
extern char* dirtyword[];
#define CHECK_DIRTYWORD(p_, msg_) do { \
			int loop = 0; \
			while (dirtyword[loop]) { \
				if( strstr((const char*)msg_, dirtyword[loop]) ) { \
					return send_to_self_error(p_, p_->waitcmd, -ERR_dirty_word, 1); \
				} \
				loop++; \
			}  \
		} while (0)

#endif




#define CHECK_DIRTYWORD(p_, msg_) \
	do { \
			int r_ = tm_dirty_check(7, (char*)(msg_)); \
			if(r_ > 0) { \
				uint32_t buf1[3];\
				buf1[0] = 0; buf1[1] = 1; buf1[2] = p_->id; \
				msglog(statistic_logfile, 0x0207EEED, get_now_tv()->tv_sec, buf1, sizeof(buf1)); \
				return send_to_self_error(p_, (p_)->waitcmd, -ERR_dirty_word, 1); \
			} \
	} while (0)





#if 0
#define CHECK_DIRTYWORD(p_, msg_) do { \
			int r = cdw((char*)(msg_), dirtyword_hdl); \
			uint32_t buf1[3]; \
			if (r == 8) { \
				buf1[0] = 0; buf1[1] = 1; buf1[2] = p_->id; \
				msglog(statistic_logfile, 0x0207EEED, get_now_tv()->tv_sec, buf1, sizeof(buf1)); \
				return send_to_self_error(p_, p_->waitcmd, -ERR_dirty_word, 1); \
			} else if (r == 4) { \
				uint32_t forbid = 1;  \
				db_forbid_user_24(p_, &forbid); \
				buf1[0] = 1; buf1[1] = 1; buf1[2] = p_->id; \
				msglog(statistic_logfile, 0x0207EEED, get_now_tv()->tv_sec, buf1, sizeof(buf1)); \
				send_to_self_error(p_, p_->waitcmd, -ERR_forbid_user_24_hours, 1); \
				return -1; \
			} \
		} while (0)
//#endif
#endif


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

static inline int
unpkg_uint32_uint32 (const uint8_t body[], int bodylen, uint32_t* arg1, uint32_t *arg2)
{
	int i = 0;

	CHECK_BODY_LEN(bodylen, 8);
	UNPKG_UINT32(body, *arg1, i);
	UNPKG_UINT32(body, *arg2, i);

	return 0;
}

static inline int
unpkg_uint32 (const uint8_t body[], int bodylen, uint32_t* dat)
{
	int i = 0;
	CHECK_BODY_LEN(bodylen, 4);
	UNPKG_UINT32(body, *dat, i);

	return 0;
}

static inline int
unpkg_mapid(const uint8_t body[], map_id_t* dat, int *len)
{
    uint32_t mapid, map_type;
	UNPKG_UINT32(body, mapid, *len);
	UNPKG_UINT32(body, map_type, *len);
    *dat= mapid + (((map_id_t)map_type)<<32);
	return 0;
}

static inline int
unpkg_uint32_uint8 (const uint8_t body[], int bodylen, uint32_t* arg1, uint8_t* arg2)
{
	int i = 0;
	CHECK_BODY_LEN(bodylen, 5);

	UNPKG_UINT32(body, *arg1, i);
	UNPKG_UINT8(body, *arg2, i);

	return 0;
}

static inline int
unpkg_uint8_uint32(const uint8_t body[], int bodylen, uint8_t* arg1, uint32_t* arg2)
{
	CHECK_BODY_LEN(bodylen, 5);

	int i = 0;
	UNPKG_UINT8(body, *arg1, i);
	UNPKG_UINT32(body, *arg2, i);

	return 0;
}

static inline int
unpkg_uint32_uint16(const uint8_t body[], int bodylen, uint32_t* arg1, uint16_t* arg2)
{
	int i = 0;
	CHECK_BODY_LEN(bodylen, 6);

	UNPKG_UINT32(body, *arg1, i);
	UNPKG_UINT16(body, *arg2, i);

	return 0;
}

static inline int
unpkg_uint16_uint16(const uint8_t body[], int bodylen, uint16_t* arg1, uint16_t* arg2)
{
	int i = 0;
	CHECK_BODY_LEN(bodylen, 4);

	UNPKG_UINT16(body, *arg1, i);
	UNPKG_UINT16(body, *arg2, i);

	return 0;
}

//------------------------------------------------
static inline int
u32cmp(const void* arg1, const void* arg2)
{
	return (*((uint32_t*)arg1) > *((uint32_t*)arg2));
}


static inline int
unpack(const uint8_t *body, int bodylen, char *format, ...)
{
#define MACRO_UNPACK(type)\
{\
	type *p = NULL;\
	p = (type *)va_arg(parameters, type *);\
	type tmp = *(type *)(body + index);\
	index += sizeof(type);\
	*p = ntohl(tmp);\
	format++;\
}
	int index = 0;
	va_list parameters;
	va_start(parameters, format);
	while (*format != '\0') {
		switch(*format) {
		case 'L':/*unsigned log(always 32 bit)*/
		{
			MACRO_UNPACK(uint32_t);
		}
		break;

		case 'l':/*signed log(always 32 bit)*/
		{
			MACRO_UNPACK(int32_t);
		}
		break;

		case 'C': /*unsigned char*/
		{
			MACRO_UNPACK(uint8_t);
		}
		break;

		case 'c': /*signed char*/
		{
			MACRO_UNPACK(int8_t);
		}
		break;

		case 'S': /*unsigned short(always 16 bit)*/
		{
			MACRO_UNPACK(uint16_t);
		}
		break;

		case 's' : /*signed short(always 16 bit)*/
		{
			MACRO_UNPACK(int16_t);
		}

		default:
			printf("error format\n");
		}
	}
	va_end(parameters);

	if (index > bodylen) {
		ERROR_RETURN(("THE LEN IS WRONG %u %u", index, bodylen), -1);
	}
	return index;
}

static inline int
pkg_host_uint32(const uint8_t *buf, uint32_t value)
{
	*(uint32_t *)buf = value;
	return 0;
}

static inline int
pkg_host_uint32_uint32(const uint8_t *buf, uint32_t value_first, uint32_t value_second)
{
	*(uint32_t *)buf = value_first;
	*(uint32_t *)(buf + 4) = value_second;
	return 0;
}

static inline int
pkg_host_uint32_uint32_uint32(const uint8_t *buf, uint32_t value_first,
		uint32_t value_second, uint32_t value_third)
{
	*(uint32_t *)buf = value_first;
	*(uint32_t *)(buf + 4) = value_second;
	*(uint32_t *)(buf + 8) = value_third;
	return 0;
}

static inline int
unpkg_host_uint32(const uint8_t *buf, uint32_t *ret)
{
	*ret = *(uint32_t *)buf;
	return 0;
}

static inline int
unpkg_host_uint32_uint32(const uint8_t *buf, uint32_t *value_first, uint32_t *value_second)
{
	*value_first = *(uint32_t *)buf;
	*value_second = *(uint32_t *)(buf + 4);
	return 0;
}

static inline int
unpkg_host_uint32_uint32_uint32(const uint8_t *buf, uint32_t *value_first,
		uint32_t *value_second, uint32_t *value_third)
{
	*value_first = *(uint32_t *)buf;
	*value_second = *(uint32_t *)(buf + 4);
	*value_third = *(uint32_t *)(buf + 8);
	return 0;
}

static inline int
unpkg_host_uint32_uint32_uint32_uint32(const uint8_t *buf, uint32_t *value_first,
		uint32_t *value_second, uint32_t *value_third, uint32_t *value_fourth)
{
	*value_first = *(uint32_t *)buf;
	*value_second = *(uint32_t *)(buf + 4);
	*value_third = *(uint32_t *)(buf + 8);
	*value_fourth = *(uint32_t *)(buf + 12);
	return 0;
}

static inline int get_today()
{
	int year = get_now_tm()->tm_year + 1900;
	int month= get_now_tm()->tm_mon + 1;
	int day = get_now_tm()->tm_mday;
	return year*10000+month*100+day;
}

static inline int get_week_id(time_t now)
{
	now += 86400 * 3;
	char buffer[10];
	strftime(buffer, 10, "%W", localtime(&now));
	return  atoll(buffer);
}

int log_init_ex(const char* dir, log_lvl_t lvl, uint32_t size, int maxfiles, const char* pre_name, uint32_t logtime);

#endif
