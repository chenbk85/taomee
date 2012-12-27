#ifndef _UTIL_H_
#define _UTIL_H_

#include <libtaomee/log.h>

#define CHECK_BODY_LEN(len, s) \
		if (len != s) \
			ERROR_RETURN(("error len=%d explen=%d", len, (uint32_t)s), -1)

#define CHECK_BODY_LEN_GE(len, s) if (len<s) ERROR_RETURN(("error len=%d", len), -1)

#define JUMP_XML_TAG(cur, str) \
	if (!cur || xmlStrcmp(cur->name, (const xmlChar *)str))  \
ERROR_RETURN (("xmlStrcmp error: tag=%s", str), -1);

#define DECODE_XML_PROP(cur, prop, str) \
	str = xmlGetProp(cur, (xmlChar*)prop);  \
if (!str) ERROR_RETURN (("xml parse error: prop=%s", prop), -1)

#define DECODE_XML_PROP_INT(v, cur, prop) do{ \
	xmlChar *str; \
	DECODE_XML_PROP(cur, prop, str); \
	v = atoi ((const char*)str); \
	xmlFree (str); \
}while(0)

#define DECODE_XML_PROP_INT_DEF(v, cur, prop, def) do{ \
	xmlChar *str; \
	str = xmlGetProp(cur, (xmlChar*)prop);  \
	if(str)\
	{\
		v = atoi ((const char*)str); \
	}\
	else\
	{\
		v = def;\
	}\
	xmlFree (str); \
}while(0)


#define DECODE_XML_PROP_STR(v, cur, prop) do{ \
	xmlChar *str; \
	DECODE_XML_PROP(cur, prop, str); \
	strncpy (v, (const char*)str, sizeof (v) - 1); \
	v[sizeof(v) - 1] = '\0'; \
	xmlFree (str); \
}while(0)

extern int pipe_handles[4];
extern int str_explode (const char *ifs, char *line, char *field[], int n);

extern int pipe_create ();
extern void str2hex (const char *what, unsigned char *dest);
extern void hex2str (char *what, int len, char *dest);
#endif
