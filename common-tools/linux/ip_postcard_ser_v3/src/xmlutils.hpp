/**
 *============================================================
 *  @file      xmlutils.hpp
 *  @brief    utilities for manipulating xml files
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef PAIPAI_XMLUTILS_HPP_
#define PAIPAI_XMLUTILS_HPP_

#include <cstring>
#include <sstream>
#include <typeinfo>

extern "C" {
#include <libxml/tree.h>

#include <libtaomee/log.h>
}

#define DECODE_XML_PROP(cur, prop, str) \
		do { \
			str = xmlGetProp(cur, (xmlChar*)prop);  \
			if (!str) { \
				ERROR_LOG("xml parse error: prop=%s", prop); \
				return -1; \
			} \
		} while (0)

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
			xmlFree(str); \
		} while (0)

#define DECODE_XML_PROP_STR(v, cur, prop) \
		do { \
			xmlChar* str; \
			DECODE_XML_PROP(cur, prop, str); \
			strncpy(v, (const char*)str, sizeof(v) - 1); \
			v[sizeof(v) - 1] = '\0'; \
			xmlFree(str); \
		} while (0)

//---------------------------------------------

template <typename T1, typename T2, size_t len>
size_t decode_xml_prop_arr_default(T1 (&arr)[len], xmlNodePtr cur, const void* prop, const T2& def)
{
	xmlChar* str;
	if (!cur || !(str = xmlGetProp(cur, (xmlChar*)prop))) {
		for (size_t i = 0; i != len; ++i) {
			arr[i] = def;
		}
		return 0;
	} else {
		size_t i = 0;
		std::istringstream iss(reinterpret_cast<const char*>(str));

		while ((i != len) && (iss >> arr[i])) {
			if ((typeid(T1) == typeid(uint8_t)) || (typeid(T1) == typeid(int8_t))) {
				arr[i] -= '0';
			}
			++i;
		}

		xmlFree(str);
		return i;
	}
}

template <typename T1, typename T2>
void decode_xml_prop_default(T1& val, xmlNodePtr cur, const void* prop, const T2& def)
{
	xmlChar* str;
	if (!cur || !(str = xmlGetProp(cur, reinterpret_cast<const xmlChar*>(prop)))) {
		val = def;
	} else {
		std::istringstream iss(reinterpret_cast<const char*>(str));
		iss >> val;
		xmlFree(str);
	}
}

//---------------------------------------------

void decode_xml_prop_float_default(float* val, xmlNodePtr cur, const void* prop, float def);
void decode_xml_prop_uint32_default(uint32_t* val, xmlNodePtr cur, const void* prop, uint32_t def);

int  load_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node));
void reload_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node));

#endif // PAIPAI_XMLUTILS_HPP_

