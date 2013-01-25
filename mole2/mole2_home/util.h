#ifndef _UTIL_H_
#define _UTIL_H_

// C89
#include <string.h>
#include <time.h>
// C99
#include <stdint.h>
// Posix
#include <arpa/inet.h>

#include <glib.h>

extern "C" {

#include <libxml/tree.h>

#include <libtaomee/log.h>

#include <libtaomee/tm_dirty/tm_dirty.h>

}

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

static inline void
decode_xml_prop_uint16_default(uint16_t* val, xmlNodePtr cur, void* prop, uint32_t def)
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
int  load_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node));

int  pipe_create(int *);

#define CHECK_BODY_LEN(len_, s_) if((len_)!=(s_)) ERROR_RETURN(("error recvlen=%d expectedlen=%d", (int)(len_), (int)(s_)), 0)
#define CHECK_BODY_LEN_GE(len_, s_) if ((len_)<(s_)) ERROR_RETURN(("error recvlen=%d expectedlen>=%d", (int)(len_), (int)(s_)), 0)
#define CHECK_BODY_LEN_LE(len_, s_) if ((len_)>(s_)) ERROR_RETURN(("error recvlen=%d expectedlen<=%d", (int)(len_), (int)(s_)), 0)

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
			memcpy((v), ((char*)b)+(j), (l)); (j) += (l); \
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
chk_bit_1_num(int x)
{
	int ret = 0;
	while (x) {
		ret++;
		x &= x-1;
	}
	return ret;
}

static inline uint32_t abs_uint(uint32_t x, uint32_t y)
{
	return (x > y) ? (x - y) : (y - x);
}

static inline uint32_t sqrt_uint(uint32_t a)
{
	uint32_t x = a;
	while (x * x > a) {
		x = (x + a / x) / 2;
	}
	return x;
}

static inline uint32_t round_uint(uint32_t x, uint32_t base){
	uint32_t divisor = base;
	while (x / divisor > base) {
		divisor *= base;
	}
	return (x /divisor + (x % divisor + divisor / 2) / divisor) * divisor;
}

static inline uint32_t bt_round(uint32_t x){
	if (x < 10) return x;
	uint32_t divisor = 10;
	int n = 1;
	while (x / divisor > 10) {
		if (n ++ == 4) continue;
		divisor *= 10;
	}
	return (x /divisor + (x % divisor + divisor / 2) / divisor) * divisor;
}


static inline void log_buf(uint8_t* buf, int len)
{
	char out[1024];
	int i, j = 0;

	for (i = 0; i < len; i ++) {
		j += sprintf (out + j, "%02x ", buf[i]);
	}

	out[j] = '\0';

	DEBUG_LOG("%s", out);
}

enum repeat_type{
	repeat_none 	= 0,
	repeat_day		= 1,
	repeat_week		= 2,
	repeat_month	= 3,
};

static inline int check_same_time(const time_t tx, const time_t ty, int rt)
{
	struct tm tmx, tmy;
	localtime_r(&tx, &tmx);
	localtime_r(&ty, &tmy);
	switch (rt) {
		case repeat_day:
			DEBUG_LOG("CHECK SAM DAY\t[%u %u %u %u]", tmx.tm_yday, tmy.tm_yday, tmx.tm_year, tmy.tm_year);
			return (tmx.tm_yday == tmy.tm_yday && tmx.tm_year == tmy.tm_year);
		case repeat_week:
			return 0;
		case repeat_month:
			return (tmx.tm_mon == tmy.tm_mon && tmx.tm_year == tmy.tm_year);
	}
	return 0;
}

static inline int check_same_period(const time_t last_time, int rtype, int startday)
{
	int weekday;
	time_t start_time, now_time = get_now_tv()->tv_sec;
	struct tm tm_start, tm_now, tm_last;
	localtime_r(&now_time, &tm_now);
	localtime_r(&last_time, &tm_last);
	switch (rtype) {
		case repeat_day:
			DEBUG_LOG("CHECK SAM DAY\t[%u %u %u %u]", tm_now.tm_yday, tm_last.tm_yday, tm_now.tm_year, tm_last.tm_year);
			return (tm_now.tm_yday == tm_last.tm_yday && tm_now.tm_year == tm_last.tm_year);
		case repeat_week:
			weekday = tm_now.tm_wday;
			weekday = weekday < startday ? weekday + 7 : weekday;
			start_time = now_time - (weekday - startday) * 24 * 60 * 60;
			localtime_r(&start_time, &tm_start);
			return ((tm_start.tm_year == tm_last.tm_year && tm_start.tm_yday <= tm_last.tm_yday) \
				|| (tm_start.tm_year < tm_last.tm_year));
		case repeat_month:
			return tm_now.tm_mon == tm_last.tm_mon && tm_now.tm_year == tm_last.tm_year;
	}
	return 0;
}

#endif
