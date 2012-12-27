#include "version.h"
#include "libtaomee++/utils/strings.hpp"
char g_version_str [255]="ONLINE-0805-";

const char *  get_version_str() {
	std::vector<std::string> file_name_list;
	file_name_list.push_back("../conf/TaskData.xml");
	file_name_list.push_back("../conf/ItemXmlData.xml");
	return get_version_send_buf(g_version_str,file_name_list );
}

     
