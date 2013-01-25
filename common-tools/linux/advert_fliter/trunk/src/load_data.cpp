/*
 * =========================================================================
 *
 *        Filename: load_data.cpp
 *
 *        Version:  1.0
 *        Created:  2011-05-18 13:33:54
 *        Description:  
 *
 *        Author:  ian (ian@taomee.com)
 *        Company:  TAOMEE
 *
 * =========================================================================
 */

#include    "load_data.h"
#include    "global.h"

int load_xmlconf(const char* file, int (*parser)(xmlNodePtr cur_node))
{
	int err = -1;

	xmlDocPtr doc = xmlReadFile(file, NULL, XML_PARSE_NOBLANKS);
	if (!doc) {
		ERROR_LOG("Failed to Load %s", file);
		return -1;
	}

	xmlNodePtr cur = xmlDocGetRootElement(doc); 
	if (!cur) {
		ERROR_LOG("xmlDocGetRootElement error");
		goto fail;
	}

	err = parser(cur);
fail:
	xmlFreeDoc(doc);
	BOOT_LOG(err, "==Load File %s", file);
}

int load_game_svr_config(xmlNodePtr cur)
{
	DEBUG_LOG("xxxxxx load_game_svr_config ");
  	cur = cur->xmlChildrenNode;
	while (cur) {
		if (!xmlStrcmp(cur->name, (const xmlChar*)"Game")) {
            uint32_t gameid;
			char word_file[64] = {};
			char stat_file[64] = {};
			DECODE_XML_PROP_INT_DEFAULT(gameid, cur, "ID", 0);
			DECODE_XML_PROP_STR(word_file, cur, "WordFile");
			DECODE_XML_PROP_STR(stat_file, cur, "StatFile");

			std::map<uint32_t, CChatCheck*>::iterator it = game_chat_check.find(gameid);
			if (it != game_chat_check.end()) {
				cur = cur->next;
				continue;
			}

			CChatCheck* pcheck = new CChatCheck(word_file, stat_file);

            xmlChar *str = xmlGetProp(cur, (xmlChar*)"IP"); 
			if (str == NULL) return -1;
			int port, port2;

            xmlChar *str2 = xmlGetProp(cur, (xmlChar*)"IP2"); 
			if (str2) {
				DECODE_XML_PROP_INT_DEFAULT(port2, cur, "Port2", 4433);
			}

			DECODE_XML_PROP_INT_DEFAULT(port, cur, "Port", 4433);
			DECODE_XML_PROP_INT_DEFAULT(pcheck->chat_protocol, cur, "ChatProtocol", 1);
			DECODE_XML_PROP_INT_DEFAULT(pcheck->send_val, cur, "SendVal", 1);
			pcheck->chat_forbid.init((const char *)str, port, (const char *)str2, port2);
			game_chat_check.insert(std::pair<uint32_t, CChatCheck*>(gameid, pcheck));
		}
		cur = cur->next;	

	}

	return 0;
}

