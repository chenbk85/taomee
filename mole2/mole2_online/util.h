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

#include <libxml/tree.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/log.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
#include <libtaomee/bitmanip/bitmanip.h>
#include <libtaomee/random/random.h>
#include <libtaomee/crypt/qdes.h>


#ifdef __cplusplus
}
#endif


#include "benchapi.h"


#define SUCC 0

#define ROUND_100(val_)	(((val_) + 50) / 100)
#define ROUND_1000(val_)	(((val_) + 500) / 1000)
#define ROUND_10000(val_)	(((val_) + 5000) / 10000)


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
			uint32_t i = 0, cnt = 0, k; \
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
decode_xml_prop_int32_default(int32_t* val, xmlNodePtr cur, const void* prop, int32_t def)
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

static inline void
decode_xml_prop_int16_default(int16_t* val, xmlNodePtr cur, const void* prop, int16_t def)
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
decode_xml_prop_arr_int_default(int arr[], size_t len, xmlNodePtr cur, const void* prop, uint32_t def)
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

static inline size_t
decode_xml_prop_arr_time(uint32_t time[], size_t len, xmlNodePtr cur, const void* prop)
{
	memset(time, 0, len * sizeof(int));
	xmlChar* str;
	if (!cur || !(str = xmlGetProp(cur, (xmlChar *)prop))) {
		return 0;
	} else {
		uint32_t k;
		char * sstr = (char *)str;
		for (k = 0; k < strlen(sstr); k ++) {
			if (sstr[k] == '-' || sstr[k] == ':')
				sstr[k] = ' ';
		}

		unsigned char* tail = str + strlen((const char*)str);
		while (*(tail - 1) == ' ')  {
			*(tail - 1)= '\0';
			tail--;
		}
		
		size_t i = 0, cnt = 0;
		size_t slen = strlen((const char*)str);
		for (; (i != len) && (cnt != slen); ++i, cnt += k) {
			sscanf((const char*)str + cnt, "%d%n", &(time[i]), &k);
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

static inline void
decode_xml_prop_hex_default(uint32_t* val, xmlNodePtr cur, const void* prop, uint32_t def)
{
	xmlChar* str;
	if (!cur || !(str = xmlGetProp(cur, (xmlChar*)prop))) {
		*val = def;
	} else {
		sscanf((const char*)str, "%x", val);
		xmlFree(str);
	}
}

static inline uint32_t high32_val(uint64_t val)
{
	return (uint32_t)(val >> 32);
}

static inline uint32_t low32_val(uint64_t val)
{
	return (uint32_t)val;
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
	if ( (val_) > (max_) ) ERROR_RETURN(("val %d is greater than max %d", (val_), (max_)), -1)
#define CHECK_INT_GE(val_, min_) \
	if ( (val_) < (min_) ) ERROR_RETURN(("val %d is less than min %d", uint32_t(val_), (min_)), -1)

#define CHECK_INT_EQ_VOID(val_, val2_) \
		if ( (val_) != (val2_) ) ERROR_RETURN_VOID("val %d is not equal to val2 %d", (val_), (val2_))

#define CHECK_BODY_LEN(len_, s_) if((len_)!=(s_)) ERROR_RETURN(("error recvlen=%u expectedlen=%u", (len_), uint32_t(s_)), -1)
#define CHECK_BODY_LEN_GE(len_, s_) if ((len_)<(s_)) ERROR_RETURN(("error recvlen=%u expectedlen>=%u", (len_), uint32_t(s_)), -1)
#define CHECK_BODY_LEN_LE(len_, s_) if ((len_)>(s_)) ERROR_RETURN(("error recvlen=%u expectedlen<=%u", (len_), uint32_t(s_)), -1)

#define CHECK_BODY_LEN_GE_VOID(len_, s_) if ((len_)<(s_)) ERROR_RETURN_VOID("error recvlen=%u expectedlen>=%u", (len_), uint32_t(s_))
#define CHECK_BODY_LEN_VOID(len_, s_) if ((len_) != (s_)) ERROR_RETURN_VOID("error recvlen=%d expectedlen=%u", (len_), uint32_t(s_))

#define CHECK_WAITCMD(p_, cmd_) if ((p_)->waitcmd != (cmd_)) ERROR_RETURN(("waitcmd [%d %d] mismatch, id=%u", (p_)->waitcmd, (cmd_), (p_)->id), -1)

#define CHECK_DIRTYWORD(p_, msg_) do { \
			int r_ = tm_dirty_check(7, (char*)msg_); \
			if(r_ > 0) { \
				return send_to_self_error(p_, p_->waitcmd, cli_err_dirty_word, 1); \
			} \
		} while(0)

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

#define PKG_H_UINT64(b, v, j) \
		do { \
			*(uint64_t*)((b)+(j)) = (v); (j) += 8; \
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
#define PKG_BATTLE_ID(b, v, j) \
		do { \
			*(uint32_t*)((b)+(j)) = htonl((uint32_t)(((battle_id_t)(v)) >> 32)); (j) += 4; \
			*(uint32_t*)((b)+(j)) = htonl((uint32_t)(v)); (j) += 4; \
		} while (0)


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
unpkg_battle_id(const uint8_t body[], battle_id_t* dat, int *len)
{
	uint32_t timestamp, userid;
	UNPKG_UINT32(body, timestamp, *len);
	UNPKG_UINT32(body, userid, *len);
    *dat= userid + (((map_id_t)timestamp)<<32);
	return 0;
}

static inline uint32_t abs_uint(uint32_t x, uint32_t y)
{
	return (x > y) ? (x - y) : (y - x);
}

static inline void log_buf(uint32_t uid, uint8_t* buf, int len)
{
	char out[1024];
	int i, j = 0;

	for (i = 0; i < len; i ++) {
		j += sprintf (out + j, "%02x ", buf[i]);
	}

	out[j] = '\0';

	KTRACE_LOG(uid, "%s", out);
}

enum repeat_type{
	repeat_none 	= 0,
	repeat_day		= 1,
	repeat_week		= 2,
	repeat_month	= 3,
	repeat_version	= 4,
	repeat_life		= 5,
};

static inline int check_same_period(const time_t last_time,int rtype, int startday)
{
	int weekday;
	time_t start_time, now_time = get_now_tv()->tv_sec;
	struct tm tm_start, tm_now, tm_last;
	localtime_r(&now_time, &tm_now);
	localtime_r(&last_time, &tm_last);
	switch (rtype) {
		case repeat_day:
			return (tm_now.tm_yday == tm_last.tm_yday && tm_now.tm_year == tm_last.tm_year);
		case repeat_week:
			weekday = tm_now.tm_wday < startday ? tm_now.tm_wday + 7 : tm_now.tm_wday;
			start_time = now_time - (weekday - startday) * 24 * 60 * 60;
			localtime_r(&start_time, &tm_start);
			weekday = tm_last.tm_wday < startday ? tm_last.tm_wday + 7 : tm_last.tm_wday;
			start_time = last_time - (weekday - startday) * 24 * 60 * 60;
			localtime_r(&start_time, &tm_last);
			return (tm_start.tm_yday == tm_last.tm_yday && tm_start.tm_year == tm_last.tm_year);
		case repeat_month:
			return tm_now.tm_mon == tm_last.tm_mon && tm_now.tm_year == tm_last.tm_year;
		case repeat_version:
			if (last_time < 1307671200)
				return 0;
			return ((last_time - 1307671200) / (7 * 86400) == (now_time - 1307671200) / (7 * 86400));
		case repeat_life:
			KDEBUG_LOG(0,"get lv rewards");
	}
	return 1;
}

static inline uint32_t get_now_sec()
{
	return (get_now_tv()->tv_sec + 3600 * 8) % (3600 * 24);
}

static inline uint32_t  get_next_sunday()
{
 	time_t t=time(NULL);
    struct tm tm;
    localtime_r(&t, &tm) ;
    tm.tm_hour=0;
    tm.tm_min=0;
    tm.tm_sec=0;

    uint32_t opt_date=tm.tm_wday>0 ?tm.tm_wday:7;
    tm.tm_mday=tm.tm_mday-opt_date+7+7;
    t=mktime(&tm);
    
   return t ; 
}

static inline uint32_t get_now_sec(uint32_t time)
{
	return (time + 3600 * 8) % (3600 * 24);
}

static inline uint32_t get_period_midnight()
{
	time_t nowtime = get_now_tv()->tv_sec;
	struct tm tm_midnight;
	localtime_r(&nowtime, &tm_midnight);
	tm_midnight.tm_hour = 23;
	tm_midnight.tm_min = 58;
	tm_midnight.tm_sec = 59;
	return mktime(&tm_midnight) - nowtime;
}

inline char * get_datetime(time_t t  )
{
    static char  buf[100];
    struct tm tm_tmp;
    localtime_r(&t, &tm_tmp) ;
	sprintf( buf , "%d-%d-%d %d:%d:%d",
			tm_tmp.tm_year+1900,tm_tmp.tm_mon+1,tm_tmp.tm_mday, tm_tmp.tm_hour,
			tm_tmp.tm_min,tm_tmp.tm_sec);
    return buf ;
}

static inline uint32_t get_day_interval(uint32_t end, uint32_t begin)
{
	return end > begin ? (end - (end + 8 * 3600) % 86400 - (begin - (begin + 8 * 3600) % 86400)) / 86400 : 0;
}

static inline int rand_type_idx(uint32_t cnt, uint32_t* share_rate, uint32_t base)
{
	uint32_t share_rand = rand() % base, rate = 0;
	for (uint32_t loop = 0; loop < cnt; loop ++) {
		rate += share_rate[loop];
		if (share_rand < rate)
			return loop;
	}
	return 0;
}
#include <list>
static inline  void rand_type_list(uint32_t cnt, uint32_t* share_rate, uint32_t base, std::list<uint32_t > & idx_list  )
{
	for (uint32_t loop = 0; loop < cnt; loop ++) {
		uint32_t randid= rand() % base;
		if (randid <=share_rate[loop]) {
			idx_list.push_back(loop);
		}
	}
	return ;
}

#define MDKEY		"relation$$$"

#define VIP_TIME_ID_5 		1000000005 //第六次充值vip活动 累计天数 以后可被重复使用 
#define VIP_TIME_ID_6 		1000000006 //第六次充值vip活动 充值天数  以后可被重复使用 
#define VIP_TIME_ID VIP_TIME_ID_5 
#define FLAG_TYPE  			2000000000
#define VIP_NOTI_FLAGID_5 	1100000005
#define VIP_NOTI_FLAGID_6 	1100000006

#define CLIENT_OPT_FLAGID 	1200000000


#endif
