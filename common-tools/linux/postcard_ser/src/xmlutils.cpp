#include <cstddef>
#include <cstring>

using namespace std;

#include "xmlutils.hpp"

void decode_xml_prop_uint32_default(uint32_t* val, xmlNodePtr cur, const void* prop, uint32_t def)
{
	xmlChar* str;
	if (!cur || !(str = xmlGetProp(cur, reinterpret_cast<const xmlChar*>(prop)))) {
		*val = def;
	} else {
		char* endpt;
		*val = strtoul(reinterpret_cast<char*>(str), &endpt, 10);
		xmlFree(str);
	}
}

void decode_xml_prop_float_default(float* val, xmlNodePtr cur, const void* prop, float def)
{
	xmlChar* str;
	if (!cur || !(str = xmlGetProp(cur, reinterpret_cast<const xmlChar*>(prop)))) {
		*val = def;
	} else {
		sscanf(reinterpret_cast<char*>(str), "%f", val);
		xmlFree(str);
	}
}

int load_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node))
{
	int err = -1;

	xmlDocPtr doc = xmlParseFile(file);
	if (!doc) {
		ERROR_RETURN(("Failed to Load %s", file), -1);
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc); 
	if (!cur) {
		ERROR_LOG("xmlDocGetRootElement error when loading file '%s'", file);
		goto fail;
	}

	err = parser(cur);
fail:
	xmlFreeDoc(doc);
	BOOT_LOG(err, "Load File %s", file);
}

void reload_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node))
{
	xmlDocPtr doc = xmlParseFile(file);
	if (!doc) {
		INFO_LOG("Failed to Reload '%s'", file);
		return;
	}

	int err = -1;
	xmlNodePtr cur = xmlDocGetRootElement(doc); 
	if (!cur) {
		INFO_LOG("xmlDocGetRootElement error when reloading file '%s'", file);
		goto fail;
	}

	err = parser(cur);
fail:
	xmlFreeDoc(doc);
	INFO_LOG("Reload File '%s' %s", file, (err ? "Failed" : "Succeeded"));
}

